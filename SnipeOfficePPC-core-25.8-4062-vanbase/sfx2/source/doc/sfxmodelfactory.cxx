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

#include "sfx2/sfxmodelfactory.hxx"

#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/beans/NamedValue.hpp>
#include <com/sun/star/lang/XInitialization.hpp>

#include <comphelper/namedvaluecollection.hxx>
#include <cppuhelper/implbase2.hxx>

#include <algorithm>
#include <functional>

//........................................................................
namespace sfx2
{
//........................................................................

    /** === begin UNO using === **/
    using ::com::sun::star::uno::Reference;
    using ::com::sun::star::uno::XInterface;
    using ::com::sun::star::uno::UNO_QUERY;
    using ::com::sun::star::uno::UNO_QUERY_THROW;
    using ::com::sun::star::uno::UNO_SET_THROW;
    using ::com::sun::star::uno::Exception;
    using ::com::sun::star::uno::RuntimeException;
    using ::com::sun::star::uno::Any;
    using ::com::sun::star::uno::makeAny;
    using ::com::sun::star::lang::XMultiServiceFactory;
    using ::com::sun::star::uno::Sequence;
    using ::com::sun::star::lang::XSingleServiceFactory;
    using ::com::sun::star::lang::XServiceInfo;
    using ::com::sun::star::beans::NamedValue;
    using ::com::sun::star::beans::PropertyValue;
    using ::com::sun::star::lang::XInitialization;
    /** === end UNO using === **/

    //====================================================================
    //= SfxModelFactory - declaration
    //====================================================================
    typedef ::cppu::WeakImplHelper2 <   XSingleServiceFactory
                                    ,   XServiceInfo
                                    >   SfxModelFactory_Base;
    /** implements a XSingleServiceFactory which can be used to created instances
        of classes derived from SfxBaseModel

        In opposite to the default implementations from module cppuhelper, this
        factory evaluates certain creation arguments (passed to createInstanceWithArguments)
        and passes them to the factory function of the derived class.
    */
    class SfxModelFactory : public SfxModelFactory_Base
    {
    public:
        SfxModelFactory(
            const Reference< XMultiServiceFactory >& _rxServiceFactory,
            const ::rtl::OUString& _rImplementationName,
            const SfxModelFactoryFunc _pComponentFactoryFunc,
            const Sequence< ::rtl::OUString >& _rServiceNames
        );

        // XSingleServiceFactory
        virtual Reference< XInterface > SAL_CALL createInstance(  ) throw (Exception, RuntimeException);
        virtual Reference< XInterface > SAL_CALL createInstanceWithArguments( const Sequence< Any >& aArguments ) throw (Exception, RuntimeException);

        // XServiceInfo
        virtual ::rtl::OUString SAL_CALL getImplementationName(  ) throw (RuntimeException);
        virtual ::sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName ) throw (RuntimeException);
        virtual Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  ) throw (RuntimeException);

    protected:
        virtual ~SfxModelFactory();

    private:
        Reference< XInterface > impl_createInstance( const sal_uInt64 _nCreationFlags ) const;

    private:
        const Reference< XMultiServiceFactory >     m_xServiceFactory;
        const ::rtl::OUString                       m_sImplementationName;
        const Sequence< ::rtl::OUString >           m_aServiceNames;
        const SfxModelFactoryFunc                   m_pComponentFactoryFunc;
    };

    //====================================================================
    //= SfxModelFactory - implementation
    //====================================================================
    //--------------------------------------------------------------------
    SfxModelFactory::SfxModelFactory( const Reference< XMultiServiceFactory >& _rxServiceFactory,
            const ::rtl::OUString& _rImplementationName, const SfxModelFactoryFunc _pComponentFactoryFunc,
            const Sequence< ::rtl::OUString >& _rServiceNames )
        :m_xServiceFactory( _rxServiceFactory )
        ,m_sImplementationName( _rImplementationName )
        ,m_aServiceNames( _rServiceNames )
        ,m_pComponentFactoryFunc( _pComponentFactoryFunc )
    {
    }

    //--------------------------------------------------------------------
    SfxModelFactory::~SfxModelFactory()
    {
    }

    //--------------------------------------------------------------------
    Reference< XInterface > SfxModelFactory::impl_createInstance( const sal_uInt64 _nCreationFlags ) const
    {
        return (*m_pComponentFactoryFunc)( m_xServiceFactory, _nCreationFlags );
    }

    //--------------------------------------------------------------------
    Reference< XInterface > SAL_CALL SfxModelFactory::createInstance(  ) throw (Exception, RuntimeException)
    {
        return createInstanceWithArguments( Sequence< Any >() );
    }

    //--------------------------------------------------------------------
    namespace
    {
        struct IsSpecialArgument : public ::std::unary_function< Any, bool >
        {
            static bool isSpecialArgumentName( const ::rtl::OUString& _rValueName )
            {
                return  _rValueName == "EmbeddedObject" || _rValueName == "EmbeddedScriptSupport" || _rValueName == "DocumentRecoverySupport";
            }

            bool operator()( const Any& _rArgument ) const
            {
                NamedValue aNamedValue;
                if ( ( _rArgument >>= aNamedValue ) && isSpecialArgumentName( aNamedValue.Name ) )
                    return true;
                PropertyValue aPropertyValue;
                if ( ( _rArgument >>= aPropertyValue ) && isSpecialArgumentName( aPropertyValue.Name ) )
                    return true;
                return false;
            }
        };
    }

    //--------------------------------------------------------------------
    Reference< XInterface > SAL_CALL SfxModelFactory::createInstanceWithArguments( const Sequence< Any >& _rArguments ) throw (Exception, RuntimeException)
    {
        ::comphelper::NamedValueCollection aArgs( _rArguments );
        const sal_Bool bEmbeddedObject = aArgs.getOrDefault( "EmbeddedObject", sal_False );
        const sal_Bool bScriptSupport = aArgs.getOrDefault( "EmbeddedScriptSupport", sal_True );
        const sal_Bool bDocRecoverySupport = aArgs.getOrDefault( "DocumentRecoverySupport", sal_True );

        sal_uInt64 nCreationFlags =
                ( bEmbeddedObject ? SFXMODEL_EMBEDDED_OBJECT : 0 )
            |   ( bScriptSupport ? 0 : SFXMODEL_DISABLE_EMBEDDED_SCRIPTS )
            |   ( bDocRecoverySupport ? 0 : SFXMODEL_DISABLE_DOCUMENT_RECOVERY );

        Reference< XInterface > xInstance( impl_createInstance( nCreationFlags ) );

        // to mimic the bahaviour of the default factory's createInstanceWithArguments, we initialize
        // the object with the given arguments, stripped by the three special ones
        Sequence< Any > aStrippedArguments( _rArguments.getLength() );
        Any* pStrippedArgs = aStrippedArguments.getArray();
        Any* pStrippedArgsEnd = ::std::remove_copy_if(
            _rArguments.getConstArray(),
            _rArguments.getConstArray() + _rArguments.getLength(),
            pStrippedArgs,
            IsSpecialArgument()
        );
        aStrippedArguments.realloc( pStrippedArgsEnd - pStrippedArgs );

        if ( aStrippedArguments.getLength() )
        {
            Reference< XInitialization > xModelInit( xInstance, UNO_QUERY );
            OSL_ENSURE( xModelInit.is(), "SfxModelFactory::createInstanceWithArguments: no XInitialization!" );
            if ( xModelInit.is() )
                xModelInit->initialize( aStrippedArguments );
        }

        return xInstance;
    }

    //--------------------------------------------------------------------
    ::rtl::OUString SAL_CALL SfxModelFactory::getImplementationName(  ) throw (RuntimeException)
    {
        return m_sImplementationName;
    }

    //--------------------------------------------------------------------
    ::sal_Bool SAL_CALL SfxModelFactory::supportsService( const ::rtl::OUString& _rServiceName ) throw (RuntimeException)
    {
        return ::std::find(
            m_aServiceNames.getConstArray(),
            m_aServiceNames.getConstArray() + m_aServiceNames.getLength(),
            _rServiceName
        )  != m_aServiceNames.getConstArray() + m_aServiceNames.getLength();
    }

    //--------------------------------------------------------------------
    Sequence< ::rtl::OUString > SAL_CALL SfxModelFactory::getSupportedServiceNames(  ) throw (RuntimeException)
    {
        return m_aServiceNames;
    }

    //--------------------------------------------------------------------
    Reference< XSingleServiceFactory > createSfxModelFactory( const Reference< XMultiServiceFactory >& _rxServiceFactory,
            const ::rtl::OUString& _rImplementationName, const SfxModelFactoryFunc _pComponentFactoryFunc,
            const Sequence< ::rtl::OUString >& _rServiceNames )
    {
        return new SfxModelFactory( _rxServiceFactory, _rImplementationName, _pComponentFactoryFunc, _rServiceNames );
    }

//........................................................................
} // namespace sfx2
//........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
