/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/


#include "loggerconfig.hxx"

/** === begin UNO includes === **/
#include <com/sun/star/configuration/theDefaultProvider.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#include <com/sun/star/util/XChangesBatch.hpp>
#include <com/sun/star/logging/LogLevel.hpp>
#include <com/sun/star/lang/NullPointerException.hpp>
#include <com/sun/star/lang/ServiceNotRegisteredException.hpp>
#include <com/sun/star/beans/NamedValue.hpp>
#include <com/sun/star/logging/XLogHandler.hpp>
#include <com/sun/star/logging/XLogFormatter.hpp>
/** === end UNO includes === **/

#include <tools/diagnose_ex.h>

#include <comphelper/componentcontext.hxx>

#include <cppuhelper/component_context.hxx>

#include <vector>
#include <sal/macros.h>

//........................................................................
namespace logging
{
//........................................................................

    /** === begin UNO using === **/
    using ::com::sun::star::uno::Reference;
    using ::com::sun::star::logging::XLogger;
    using ::com::sun::star::lang::XMultiServiceFactory;
    using ::com::sun::star::uno::Sequence;
    using ::com::sun::star::uno::Any;
    using ::com::sun::star::container::XNameContainer;
    using ::com::sun::star::uno::UNO_QUERY_THROW;
    using ::com::sun::star::lang::XSingleServiceFactory;
    using ::com::sun::star::uno::XInterface;
    using ::com::sun::star::util::XChangesBatch;
    using ::com::sun::star::uno::makeAny;
    using ::com::sun::star::lang::NullPointerException;
    using ::com::sun::star::uno::Exception;
    using ::com::sun::star::lang::ServiceNotRegisteredException;
    using ::com::sun::star::beans::NamedValue;
    using ::com::sun::star::logging::XLogHandler;
    using ::com::sun::star::logging::XLogFormatter;
    using ::com::sun::star::container::XNameAccess;
    using ::com::sun::star::uno::XComponentContext;
    /** === end UNO using === **/
    namespace LogLevel = ::com::sun::star::logging::LogLevel;

    namespace
    {
        //----------------------------------------------------------------
        typedef void (*SettingTranslation)( const Reference< XLogger >&, const ::rtl::OUString&, Any& );

        //----------------------------------------------------------------
        void    lcl_substituteFileHandlerURLVariables_nothrow( const Reference< XLogger >& _rxLogger, ::rtl::OUString& _inout_rFileURL )
        {
            struct Variable
            {
                const sal_Char*         pVariablePattern;
                const sal_Int32         nPatternLength;
                rtl_TextEncoding        eEncoding;
                const ::rtl::OUString   sVariableValue;

                Variable( const sal_Char* _pVariablePattern,  const sal_Int32 _nPatternLength, rtl_TextEncoding _eEncoding,
                        const ::rtl::OUString& _rVariableValue )
                    :pVariablePattern( _pVariablePattern )
                    ,nPatternLength( _nPatternLength )
                    ,eEncoding( _eEncoding )
                    ,sVariableValue( _rVariableValue )
                {
                }
            };

            ::rtl::OUString sLoggerName;
            try { sLoggerName = _rxLogger->getName(); }
            catch( const Exception& ) { DBG_UNHANDLED_EXCEPTION(); }

            Variable aVariables[] =
            {
                Variable( RTL_CONSTASCII_USTRINGPARAM( "$(loggername)" ), sLoggerName )
            };

            for ( size_t i = 0; i < SAL_N_ELEMENTS( aVariables ); ++i )
            {
                ::rtl::OUString sPattern( aVariables[i].pVariablePattern, aVariables[i].nPatternLength, aVariables[i].eEncoding );
                sal_Int32 nVariableIndex = _inout_rFileURL.indexOf( sPattern );
                if  (   ( nVariableIndex == 0 )
                    ||  (   ( nVariableIndex > 0 )
                        &&  ( sPattern[ nVariableIndex - 1 ] != '$' )
                        )
                    )
                {
                    // found an (unescaped) variable
                    _inout_rFileURL = _inout_rFileURL.replaceAt( nVariableIndex, sPattern.getLength(), aVariables[i].sVariableValue );
                }
            }
        }

        //----------------------------------------------------------------
        void    lcl_transformFileHandlerSettings_nothrow( const Reference< XLogger >& _rxLogger, const ::rtl::OUString& _rSettingName, Any& _inout_rSettingValue )
        {
            if ( _rSettingName != "FileURL" )
                // not interested in this setting
                return;

            ::rtl::OUString sURL;
            OSL_VERIFY( _inout_rSettingValue >>= sURL );
            lcl_substituteFileHandlerURLVariables_nothrow( _rxLogger, sURL );
            _inout_rSettingValue <<= sURL;
        }

        //----------------------------------------------------------------
        Reference< XInterface > lcl_createInstanceFromSetting_throw(
                const ::comphelper::ComponentContext& _rContext,
                const Reference< XLogger >& _rxLogger,
                const Reference< XNameAccess >& _rxLoggerSettings,
                const sal_Char* _pServiceNameAsciiNodeName,
                const sal_Char* _pServiceSettingsAsciiNodeName,
                SettingTranslation _pSettingTranslation = NULL
            )
        {
            Reference< XInterface > xInstance;

            // read the settings for the to-be-created service
            Reference< XNameAccess > xServiceSettingsNode( _rxLoggerSettings->getByName(
                ::rtl::OUString::createFromAscii( _pServiceSettingsAsciiNodeName ) ), UNO_QUERY_THROW );

            Sequence< ::rtl::OUString > aSettingNames( xServiceSettingsNode->getElementNames() );
            size_t nServiceSettingCount( aSettingNames.getLength() );
            Sequence< NamedValue > aSettings( nServiceSettingCount );
            if ( nServiceSettingCount )
            {
                const ::rtl::OUString* pSettingNames = aSettingNames.getConstArray();
                const ::rtl::OUString* pSettingNamesEnd = aSettingNames.getConstArray() + aSettingNames.getLength();
                NamedValue* pSetting = aSettings.getArray();

                for (   ;
                        pSettingNames != pSettingNamesEnd;
                        ++pSettingNames, ++pSetting
                    )
                {
                    pSetting->Name = *pSettingNames;
                    pSetting->Value = xServiceSettingsNode->getByName( *pSettingNames );

                    if ( _pSettingTranslation )
                        (_pSettingTranslation)( _rxLogger, pSetting->Name, pSetting->Value );
                }
            }

            ::rtl::OUString sServiceName;
            _rxLoggerSettings->getByName( ::rtl::OUString::createFromAscii( _pServiceNameAsciiNodeName ) ) >>= sServiceName;
            if ( !sServiceName.isEmpty() )
            {
                bool bSuccess = false;
                if ( aSettings.getLength() )
                {
                    Sequence< Any > aConstructionArgs(1);
                    aConstructionArgs[0] <<= aSettings;
                    bSuccess = _rContext.createComponentWithArguments( sServiceName, aConstructionArgs, xInstance );
                }
                else
                {
                    bSuccess = _rContext.createComponent( sServiceName, xInstance );
                }

                if ( !bSuccess )
                    throw ServiceNotRegisteredException( sServiceName, NULL );
            }

            return xInstance;
        }
    }

    //--------------------------------------------------------------------
    void initializeLoggerFromConfiguration( const ::comphelper::ComponentContext& _rContext, const Reference< XLogger >& _rxLogger )
    {
        try
        {
            if ( !_rxLogger.is() )
                throw NullPointerException();

            Reference< XMultiServiceFactory > xConfigProvider(
                com::sun::star::configuration::theDefaultProvider::get(
                    _rContext.getUNOContext()));

            // write access to the "Settings" node (which includes settings for all loggers)
            Sequence< Any > aArguments(1);
            aArguments[0] <<= NamedValue(
                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "nodepath" ) ),
                makeAny( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "/org.openoffice.Office.Logging/Settings" ) ) )
            );
            Reference< XNameContainer > xAllSettings( xConfigProvider->createInstanceWithArguments(
                ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.configuration.ConfigurationUpdateAccess" ) ),
                aArguments
            ), UNO_QUERY_THROW );

            ::rtl::OUString sLoggerName( _rxLogger->getName() );
            if ( !xAllSettings->hasByName( sLoggerName ) )
            {
                // no node yet for this logger. Create default settings.
                Reference< XSingleServiceFactory > xNodeFactory( xAllSettings, UNO_QUERY_THROW );
                Reference< XInterface > xLoggerSettings( xNodeFactory->createInstance(), UNO_QUERY_THROW );
                xAllSettings->insertByName( sLoggerName, makeAny( xLoggerSettings ) );
                Reference< XChangesBatch > xChanges( xAllSettings, UNO_QUERY_THROW );
                xChanges->commitChanges();
            }

            // actually read and forward the settings
            Reference< XNameAccess > xLoggerSettings( xAllSettings->getByName( sLoggerName ), UNO_QUERY_THROW );

            // the log level
            sal_Int32 nLogLevel( LogLevel::OFF );
            OSL_VERIFY( xLoggerSettings->getByName( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "LogLevel" ) ) ) >>= nLogLevel );
            _rxLogger->setLevel( nLogLevel );

            // the default handler, if any
            Reference< XInterface > xUntyped( lcl_createInstanceFromSetting_throw( _rContext, _rxLogger, xLoggerSettings, "DefaultHandler", "HandlerSettings", &lcl_transformFileHandlerSettings_nothrow ) );
            if ( !xUntyped.is() )
                // no handler -> we're done
                return;
            Reference< XLogHandler > xHandler( xUntyped, UNO_QUERY_THROW );
            _rxLogger->addLogHandler( xHandler );

            // The newly created handler might have an own (default) level. Ensure that it uses
            // the same level as the logger.
            xHandler->setLevel( nLogLevel );

            // the default formatter for the handler
            xUntyped = lcl_createInstanceFromSetting_throw( _rContext, _rxLogger, xLoggerSettings, "DefaultFormatter", "FormatterSettings" );
            if ( !xUntyped.is() )
                // no formatter -> we're done
                return;
            Reference< XLogFormatter > xFormatter( xUntyped, UNO_QUERY_THROW );
            xHandler->setFormatter( xFormatter );

            // TODO: we could first create the formatter, then the handler. This would allow
            // passing the formatter as value in the component context, so the handler would
            // not create an own default formatter
        }
        catch( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
    }

//........................................................................
} // namespace logging
//........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
