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

#ifndef __FRAMEWORK_CLASSES_ACTIONTRIGGERSEPARATORPROPERTYSET_HXX_
#define __FRAMEWORK_CLASSES_ACTIONTRIGGERSEPARATORPROPERTYSET_HXX_

#include <threadhelp/threadhelpbase.hxx>
#include <cppuhelper/propshlp.hxx>
#include <cppuhelper/weak.hxx>
#include <osl/mutex.hxx>
#include <rtl/ustring.hxx>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/lang/IllegalArgumentException.hpp>
#include <framework/fwedllapi.h>

#define SERVICENAME_ACTIONTRIGGERSEPARATOR          "com.sun.star.ui.ActionTriggerSeparator"
#define IMPLEMENTATIONNAME_ACTIONTRIGGERSEPARATOR   "com.sun.star.comp.ui.ActionTriggerSeparator"


namespace framework
{

class ActionTriggerSeparatorPropertySet :   public ThreadHelpBase                       ,   // Struct for right initalization of mutex member! Must be first of baseclasses.
                                            public ::com::sun::star::lang::XServiceInfo ,
                                            public ::com::sun::star::lang::XTypeProvider,
                                            public ::cppu::OBroadcastHelper             ,
                                            public ::cppu::OPropertySetHelper           ,   // -> XPropertySet, XFastPropertySet, XMultiPropertySet
                                            public ::cppu::OWeakObject
{
    public:
        ActionTriggerSeparatorPropertySet( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceManager );
        virtual ~ActionTriggerSeparatorPropertySet();

        // XInterface
        virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& aType )
            throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL acquire() throw ();
        virtual void SAL_CALL release() throw ();

        // XServiceInfo
        virtual ::rtl::OUString SAL_CALL getImplementationName(  ) throw (::com::sun::star::uno::RuntimeException);
        virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  ) throw (::com::sun::star::uno::RuntimeException);

        // XTypeProvider
        virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId(  ) throw (::com::sun::star::uno::RuntimeException);

    private:
        //---------------------------------------------------------------------------------------------------------
        //  OPropertySetHelper
        //---------------------------------------------------------------------------------------------------------
        virtual sal_Bool SAL_CALL convertFastPropertyValue( com::sun::star::uno::Any&       aConvertedValue,
                                                            com::sun::star::uno::Any&       aOldValue,
                                                            sal_Int32                       nHandle,
                                                            const com::sun::star::uno::Any& aValue          )
            throw( com::sun::star::lang::IllegalArgumentException );


        virtual void SAL_CALL setFastPropertyValue_NoBroadcast( sal_Int32 nHandle, const com::sun::star::uno::Any& aValue )
            throw( com::sun::star::uno::Exception );

        using cppu::OPropertySetHelper::getFastPropertyValue;
        virtual void SAL_CALL getFastPropertyValue( com::sun::star::uno::Any& aValue, sal_Int32 nHandle ) const;

        virtual ::cppu::IPropertyArrayHelper& SAL_CALL getInfoHelper();

        virtual com::sun::star::uno::Reference< com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo()
            throw (::com::sun::star::uno::RuntimeException);

        static const com::sun::star::uno::Sequence< com::sun::star::beans::Property > impl_getStaticPropertyDescriptor();

        //---------------------------------------------------------------------------------------------------------
        //  helper
        //---------------------------------------------------------------------------------------------------------

        sal_Bool impl_tryToChangeProperty(  sal_Int16                           aCurrentValue   ,
                                            const   com::sun::star::uno::Any&   aNewValue       ,
                                            com::sun::star::uno::Any&           aOldValue       ,
                                            com::sun::star::uno::Any&           aConvertedValue ) throw( com::sun::star::lang::IllegalArgumentException );

        //---------------------------------------------------------------------------------------------------------
        //  members
        //---------------------------------------------------------------------------------------------------------

        sal_Int16   m_nSeparatorType;
};

}

#endif // __FRAMEWORK_CLASSES_ACTIONTRIGGERSEPARATORPROPERTYSET_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
