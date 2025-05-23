/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the SnipeOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#include "basscript.hxx"
#include <osl/mutex.hxx>
#include <vcl/svapp.hxx>
#include <basic/sbx.hxx>
#include <basic/sbstar.hxx>
#include <basic/sbmod.hxx>
#include <basic/sbmeth.hxx>
#include <basic/basmgr.hxx>
#include <com/sun/star/script/provider/ScriptFrameworkErrorType.hpp>
#include "bcholder.hxx"
#include <comphelper/proparrhlp.hxx>
#include <comphelper/propertycontainer.hxx>
#include <com/sun/star/beans/PropertyAttribute.hpp>
#include <map>


using namespace ::com::sun::star;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::script;
using namespace ::com::sun::star::document;
using namespace ::com::sun::star::beans;

extern ::com::sun::star::uno::Any sbxToUnoValue( SbxVariable* pVar );
extern void unoToSbxValue( SbxVariable* pVar, const ::com::sun::star::uno::Any& aValue );


//.........................................................................
namespace basprov
{
//.........................................................................
#define BASSCRIPT_PROPERTY_ID_CALLER         1
#define BASSCRIPT_PROPERTY_CALLER            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Caller" ) )

#define BASSCRIPT_DEFAULT_ATTRIBS()       PropertyAttribute::BOUND | PropertyAttribute::TRANSIENT

    typedef ::std::map< sal_Int16, Any, ::std::less< sal_Int16 > > OutParamMap;

    // =============================================================================
    // BasicScriptImpl
    // =============================================================================

    // -----------------------------------------------------------------------------

    BasicScriptImpl::BasicScriptImpl( const ::rtl::OUString& funcName, SbMethodRef xMethod )
        : ::scripting_helper::OBroadcastHelperHolder( m_aMutex )
        ,OPropertyContainer( GetBroadcastHelper() )
        ,m_xMethod( xMethod )
        ,m_funcName( funcName )
        ,m_documentBasicManager( NULL )
        ,m_xDocumentScriptContext()
    {
        registerProperty( BASSCRIPT_PROPERTY_CALLER, BASSCRIPT_PROPERTY_ID_CALLER, BASSCRIPT_DEFAULT_ATTRIBS(), &m_caller, ::getCppuType( &m_caller ) );
    }

    // -----------------------------------------------------------------------------

    BasicScriptImpl::BasicScriptImpl( const ::rtl::OUString& funcName, SbMethodRef xMethod,
        BasicManager& documentBasicManager, const Reference< XScriptInvocationContext >& documentScriptContext ) : ::scripting_helper::OBroadcastHelperHolder( m_aMutex )
        ,OPropertyContainer( GetBroadcastHelper() )
        ,m_xMethod( xMethod )
        ,m_funcName( funcName )
        ,m_documentBasicManager( &documentBasicManager )
        ,m_xDocumentScriptContext( documentScriptContext )
    {
        StartListening( *m_documentBasicManager );
        registerProperty( BASSCRIPT_PROPERTY_CALLER, BASSCRIPT_PROPERTY_ID_CALLER, BASSCRIPT_DEFAULT_ATTRIBS(), &m_caller, ::getCppuType( &m_caller ) );
    }

    // -----------------------------------------------------------------------------
    BasicScriptImpl::~BasicScriptImpl()
    {
        if ( m_documentBasicManager )
            EndListening( *m_documentBasicManager );
    }

    // -----------------------------------------------------------------------------
    // SfxListener
    // -----------------------------------------------------------------------------
    void BasicScriptImpl::Notify( SfxBroadcaster& rBC, const SfxHint& rHint )
    {
        if ( &rBC != m_documentBasicManager )
        {
            OSL_ENSURE( false, "BasicScriptImpl::Notify: where does this come from?" );
            // not interested in
            return;
        }
        const SfxSimpleHint* pSimpleHint = PTR_CAST( SfxSimpleHint, &rHint );
        if ( pSimpleHint && ( pSimpleHint->GetId() == SFX_HINT_DYING ) )
        {
            m_documentBasicManager = NULL;
            EndListening( rBC );    // prevent multiple notifications
        }
    }

    // -----------------------------------------------------------------------------
    // XInterface
    // -----------------------------------------------------------------------------

    IMPLEMENT_FORWARD_XINTERFACE2( BasicScriptImpl, BasicScriptImpl_BASE, OPropertyContainer )

    // -----------------------------------------------------------------------------
    // XTypeProvider
    // -----------------------------------------------------------------------------

    IMPLEMENT_FORWARD_XTYPEPROVIDER2( BasicScriptImpl, BasicScriptImpl_BASE, OPropertyContainer )

    // -----------------------------------------------------------------------------
    // OPropertySetHelper
    // -----------------------------------------------------------------------------

    ::cppu::IPropertyArrayHelper& BasicScriptImpl::getInfoHelper(  )
    {
        return *getArrayHelper();
    }

    // -----------------------------------------------------------------------------
    // OPropertyArrayUsageHelper
    // -----------------------------------------------------------------------------

    ::cppu::IPropertyArrayHelper* BasicScriptImpl::createArrayHelper(  ) const
    {
        Sequence< Property > aProps;
        describeProperties( aProps );
        return new ::cppu::OPropertyArrayHelper( aProps );
    }

    // -----------------------------------------------------------------------------
    // XPropertySet
    // -----------------------------------------------------------------------------

    Reference< XPropertySetInfo > BasicScriptImpl::getPropertySetInfo(  ) throw (RuntimeException)
    {
        Reference< XPropertySetInfo > xInfo( createPropertySetInfo( getInfoHelper() ) );
        return xInfo;
    }

    // -----------------------------------------------------------------------------
    // XScript
    // -----------------------------------------------------------------------------

    Any BasicScriptImpl::invoke( const Sequence< Any >& aParams, Sequence< sal_Int16 >& aOutParamIndex, Sequence< Any >& aOutParam )
        throw ( provider::ScriptFrameworkErrorException, reflection::InvocationTargetException, uno::RuntimeException)
    {
        // TODO: throw CannotConvertException
        // TODO: check length of aOutParamIndex, aOutParam

        SolarMutexGuard aGuard;

        Any aReturn;

        if ( m_xMethod )
        {
            // check if compiled
            SbModule* pModule = static_cast< SbModule* >( m_xMethod->GetParent() );
            if ( pModule && !pModule->IsCompiled() )
                pModule->Compile();

            // check number of parameters
            sal_Int32 nParamsCount = aParams.getLength();
            SbxInfo* pInfo = m_xMethod->GetInfo();
            if ( pInfo )
            {
                sal_Int32 nSbxOptional = 0;
                sal_uInt16 n = 1;
                for ( const SbxParamInfo* pParamInfo = pInfo->GetParam( n ); pParamInfo; pParamInfo = pInfo->GetParam( ++n ) )
                {
                    if ( ( pParamInfo->nFlags & SBX_OPTIONAL ) != 0 )
                        ++nSbxOptional;
                    else
                        nSbxOptional = 0;
                }
                sal_Int32 nSbxCount = n - 1;
                if ( nParamsCount < nSbxCount - nSbxOptional )
                {
                    throw provider::ScriptFrameworkErrorException(
                        ::rtl::OUString(
                            RTL_CONSTASCII_USTRINGPARAM(
                                "wrong number of parameters!" ) ),
                         Reference< XInterface >(),
                         m_funcName,
                         ::rtl::OUString(
                             RTL_CONSTASCII_USTRINGPARAM( "Basic" ) ),
                        provider::ScriptFrameworkErrorType::NO_SUCH_SCRIPT  );
                }
            }

            // set parameters
            SbxArrayRef xSbxParams;
            if ( nParamsCount > 0 )
            {
                xSbxParams = new SbxArray;
                const Any* pParams = aParams.getConstArray();
                for ( sal_Int32 i = 0; i < nParamsCount; ++i )
                {
                    SbxVariableRef xSbxVar = new SbxVariable( SbxVARIANT );
                    unoToSbxValue( static_cast< SbxVariable* >( xSbxVar ), pParams[i] );
                    xSbxParams->Put( xSbxVar, static_cast< sal_uInt16 >( i ) + 1 );

                    // Enable passing by ref
                    if ( xSbxVar->GetType() != SbxVARIANT )
                        xSbxVar->SetFlag( SBX_FIXED );
                 }
            }
            if ( xSbxParams.Is() )
                m_xMethod->SetParameters( xSbxParams );

            // call method
            SbxVariableRef xReturn = new SbxVariable;
            ErrCode nErr = SbxERR_OK;
            {
                // if it's a document-based script, temporarily reset ThisComponent to the script invocation context
                Any aOldThisComponent;
                if ( m_documentBasicManager && m_xDocumentScriptContext.is() )
                    aOldThisComponent = m_documentBasicManager->SetGlobalUNOConstant( "ThisComponent", makeAny( m_xDocumentScriptContext ) );

            if ( m_caller.getLength() && m_caller[ 0 ].hasValue()  )
            {
                SbxVariableRef xCallerVar = new SbxVariable( SbxVARIANT );
                unoToSbxValue( static_cast< SbxVariable* >( xCallerVar ), m_caller[ 0 ] );
                nErr = m_xMethod->Call( xReturn, xCallerVar );
            }
            else
                nErr = m_xMethod->Call( xReturn );
                if ( m_documentBasicManager && m_xDocumentScriptContext.is() )
                    m_documentBasicManager->SetGlobalUNOConstant( "ThisComponent", aOldThisComponent );
            }
            if ( nErr != SbxERR_OK )
            {
                // TODO: throw InvocationTargetException ?
            }

            // get output parameters
            if ( xSbxParams.Is() )
            {
                SbxInfo* pInfo_ = m_xMethod->GetInfo();
                if ( pInfo_ )
                {
                    OutParamMap aOutParamMap;
                    for ( sal_uInt16 n = 1, nCount = xSbxParams->Count(); n < nCount; ++n )
                    {
                        const SbxParamInfo* pParamInfo = pInfo_->GetParam( n );
                        if ( pParamInfo && ( pParamInfo->eType & SbxBYREF ) != 0 )
                        {
                            SbxVariable* pVar = xSbxParams->Get( n );
                            if ( pVar )
                            {
                                SbxVariableRef xVar = pVar;
                                aOutParamMap.insert( OutParamMap::value_type( n - 1, sbxToUnoValue( xVar ) ) );
                            }
                        }
                    }
                    sal_Int32 nOutParamCount = aOutParamMap.size();
                    aOutParamIndex.realloc( nOutParamCount );
                    aOutParam.realloc( nOutParamCount );
                    sal_Int16* pOutParamIndex = aOutParamIndex.getArray();
                    Any* pOutParam = aOutParam.getArray();
                    for ( OutParamMap::iterator aIt = aOutParamMap.begin(); aIt != aOutParamMap.end(); ++aIt, ++pOutParamIndex, ++pOutParam )
                    {
                        *pOutParamIndex = aIt->first;
                        *pOutParam = aIt->second;
                    }
                }
            }

            // get return value
            aReturn = sbxToUnoValue( xReturn );

            // reset parameters
            m_xMethod->SetParameters( NULL );
        }

        return aReturn;
    }

    // -----------------------------------------------------------------------------

//.........................................................................
}   // namespace basprov
//.........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
