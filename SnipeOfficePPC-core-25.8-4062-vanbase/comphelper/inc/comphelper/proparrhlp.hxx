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

#ifndef _COMPHELPER_PROPERTY_ARRAY_HELPER_HXX_
#define _COMPHELPER_PROPERTY_ARRAY_HELPER_HXX_

#include <comphelper/stl_types.hxx>
#include <comphelper/propagg.hxx>
#include <cppuhelper/propshlp.hxx>
#include <osl/mutex.hxx>
#include <osl/diagnose.h>
#include <rtl/instance.hxx>

namespace cppu {
    class IPropertyArrayHelper;
}

//... namespace comphelper ................................................
namespace comphelper
{
//.........................................................................

    namespace staruno   = ::com::sun::star::uno;
    namespace starbeans = ::com::sun::star::beans;


//==================================================================

template <typename TYPE> struct OPropertyArrayUsageHelperMutex
    : public rtl::Static< ::osl::Mutex, OPropertyArrayUsageHelperMutex<TYPE> > {};


template <class TYPE>
class OPropertyArrayUsageHelper
{
protected:
    static sal_Int32                        s_nRefCount;
    static ::cppu::IPropertyArrayHelper*    s_pProps;

public:
    OPropertyArrayUsageHelper();
    virtual ~OPropertyArrayUsageHelper()
    {   // ARGHHHHHHH ..... would like to implement this after the class
        // definition (as we do with all other methods) but SUNPRO 5 compiler
        // (linker) doesn't like this
        ::osl::MutexGuard aGuard(OPropertyArrayUsageHelperMutex<TYPE>::get());
        OSL_ENSURE(s_nRefCount > 0, "OPropertyArrayUsageHelper::~OPropertyArrayUsageHelper : suspicious call : have a refcount of 0 !");
        if (!--s_nRefCount)
        {
            delete s_pProps;
            s_pProps = NULL;
        }
    }

    /** call this in the getInfoHelper method of your derived class. The method returns the array helper of the
        class, which is created if neccessary.
    */
    ::cppu::IPropertyArrayHelper*   getArrayHelper();

protected:
    /** used to implement the creation of the array helper which is shared amongst all instances of the class.
        This method needs to be implemented in derived classes.
        <BR>
        The method gets called with Mutex acquired.
        <BR>
        as long as IPropertyArrayHelper has no virtual destructor, the implementation of ~OPropertyArrayUsageHelper
        assumes that you created an ::cppu::OPropertyArrayHelper when deleting s_pProps.
        @return                         an pointer to the newly created array helper. Must not be NULL.
    */
    virtual ::cppu::IPropertyArrayHelper* createArrayHelper( ) const = 0;
};

//==================================================================
/** a OPropertyArrayUsageHelper which will create an OPropertyArrayAggregationHelper
*/
template <class TYPE>
class OAggregationArrayUsageHelper: public OPropertyArrayUsageHelper<TYPE>
{
protected:
    /** overwrite this in your derived class. initialize the two sequences with your and your aggregate's
        properties.
        <BR>
        The method gets called with Mutex acquired.
        @param      _rProps             out parameter to be filled with the property descriptions of your own class
        @param      _rAggregateProps    out parameter to be filled with the properties of your aggregate.
    */
    virtual void fillProperties(
        ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property >& /* [out] */ _rProps,
        ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property >& /* [out] */ _rAggregateProps
        ) const = 0;

    /** creates an OPropertyArrayAggregationHelper filled with properties for which's initialization
        fillProperties is called. getInfoService and getFirstAggregateId may be overwritten to determine
        the additional parameters of the OPropertyArrayAggregationHelper.
    */
    virtual ::cppu::IPropertyArrayHelper* createArrayHelper( ) const;

    /** the return value is used for the construction of the OPropertyArrayAggregationHelper.
        Beware of the lifetime of the returned object, as it has to exist 'til the last instance
        of this class dies.
    */
    virtual IPropertyInfoService* getInfoService() const { return NULL; }

    /** the return value is used for the construction of the OPropertyArrayAggregationHelper.
    */
    virtual sal_Int32 getFirstAggregateId() const { return DEFAULT_AGGREGATE_PROPERTY_ID; }
};

//------------------------------------------------------------------
template<class TYPE>
sal_Int32                       OPropertyArrayUsageHelper< TYPE >::s_nRefCount  = 0;

template<class TYPE>
::cppu::IPropertyArrayHelper*   OPropertyArrayUsageHelper< TYPE >::s_pProps = NULL;

//------------------------------------------------------------------
template <class TYPE>
OPropertyArrayUsageHelper<TYPE>::OPropertyArrayUsageHelper()
{
    ::osl::MutexGuard aGuard(OPropertyArrayUsageHelperMutex<TYPE>::get());
    ++s_nRefCount;
}

//------------------------------------------------------------------
template <class TYPE>
::cppu::IPropertyArrayHelper* OPropertyArrayUsageHelper<TYPE>::getArrayHelper()
{
    OSL_ENSURE(s_nRefCount, "OPropertyArrayUsageHelper::getArrayHelper : suspicious call : have a refcount of 0 !");
    if (!s_pProps)
    {
        ::osl::MutexGuard aGuard(OPropertyArrayUsageHelperMutex<TYPE>::get());
        if (!s_pProps)
        {
            s_pProps = createArrayHelper();
            OSL_ENSURE(s_pProps, "OPropertyArrayUsageHelper::getArrayHelper : createArrayHelper returned nonsense !");
        }
    }
    return s_pProps;
}

//------------------------------------------------------------------
template <class TYPE> inline
::cppu::IPropertyArrayHelper* OAggregationArrayUsageHelper<TYPE>::createArrayHelper() const
{
    ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property > aProps;
    ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property > aAggregateProps;
    fillProperties(aProps, aAggregateProps);
    OSL_ENSURE(aProps.getLength(), "OAggregationArrayUsageHelper::createArrayHelper : fillProperties returned nonsense !");
    return new OPropertyArrayAggregationHelper(aProps, aAggregateProps, getInfoService(), getFirstAggregateId());
}

//.........................................................................
}
//... namespace comphelper ................................................

#endif // _COMPHELPER_PROPERTY_ARRAY_HELPER_HXX_


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
