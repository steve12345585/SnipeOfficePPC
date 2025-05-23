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
#ifndef _CPPUHELPER_IMPLBASE_EX_HXX_
#define _CPPUHELPER_IMPLBASE_EX_HXX_

#include <cppuhelper/weak.hxx>
#include <cppuhelper/weakagg.hxx>
#include <com/sun/star/lang/XTypeProvider.hpp>

// Despite the fact that the following include is not used in this header, it has to remain,
// because it is expected by files including cppuhelper/implbaseN.hxx.
// So maybe we can omit it some time in the future...
#include <com/sun/star/lang/XMultiServiceFactory.hpp>

/* If you need to define implementation helper classes that deal with more than
   12 interfaces, then use macros as follows, e.g. for 3 interfaces:

#include <cppuhelper/implbase_ex_pre.hxx>
#define __IFC_EX_TYPE_INIT3( class_cast ) \
    __IFC_EX_TYPE_INIT( class_cast, 1 ), __IFC_EX_TYPE_INIT( class_cast, 2 ), \
    __IFC_EX_TYPE_INIT( class_cast, 3 )
#include <cppuhelper/implbase_ex_post.hxx>
__DEF_IMPLHELPER_EX( 3 )
*/

/// @cond INTERNAL

namespace cppu
{

/** function pointer signature for getCppuType
*/
typedef ::com::sun::star::uno::Type const & (SAL_CALL * fptr_getCppuType)( void * ) SAL_THROW(());

/** single type + object offset
*/
struct type_entry
{
    /** the type_entry is initialized with function pointer to ::getCppuType() function first,
        but holds an unacquired typelib_TypeDescriptionReference * after initialization,
        thus reusing the memory.  Flag class_data::m_storedTypeRefs
    */
    union
    {
        fptr_getCppuType getCppuType;
        typelib_TypeDescriptionReference * typeRef;
    } m_type;
    /** offset for interface pointer
    */
    sal_IntPtr m_offset;
};

/** identical dummy struct for casting class_dataN to class_data
*/
struct class_data
{
    /** number of supported types in m_typeEntries
    */
    sal_Int16 m_nTypes;

    /** determines whether m_typeEntries is initialized and carries unacquired type refs
    */
    sal_Bool m_storedTypeRefs;

    /** determines whether an implementation id was created in m_id
    */
    sal_Bool m_createdId;

    /** implementation id
     */
    sal_Int8 m_id[ 16 ];

    /** type, object offset
    */
    type_entry m_typeEntries[ 1 ];
};

/** ImplHelper
*/
CPPUHELPER_DLLPUBLIC ::com::sun::star::uno::Any SAL_CALL ImplHelper_query(
    ::com::sun::star::uno::Type const & rType,
    class_data * cd,
    void * that )
    SAL_THROW( (::com::sun::star::uno::RuntimeException) );
/** ImplHelper
*/
CPPUHELPER_DLLPUBLIC ::com::sun::star::uno::Any SAL_CALL ImplHelper_queryNoXInterface(
    ::com::sun::star::uno::Type const & rType,
    class_data * cd,
    void * that )
    SAL_THROW( (::com::sun::star::uno::RuntimeException) );
/** ImplHelper
*/
CPPUHELPER_DLLPUBLIC ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type >
SAL_CALL ImplHelper_getTypes(
    class_data * cd )
    SAL_THROW( (::com::sun::star::uno::RuntimeException) );
/** ImplHelper
*/
CPPUHELPER_DLLPUBLIC ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type >
SAL_CALL ImplInhHelper_getTypes(
    class_data * cd,
    ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > const & rAddTypes )
    SAL_THROW( (::com::sun::star::uno::RuntimeException) );
/** ImplHelper
*/
CPPUHELPER_DLLPUBLIC ::com::sun::star::uno::Sequence< sal_Int8 >
SAL_CALL ImplHelper_getImplementationId(
    class_data * cd )
    SAL_THROW( (::com::sun::star::uno::RuntimeException) );

/** WeakImplHelper
*/
CPPUHELPER_DLLPUBLIC ::com::sun::star::uno::Any SAL_CALL WeakImplHelper_query(
    ::com::sun::star::uno::Type const & rType,
    class_data * cd,
    void * that,
    ::cppu::OWeakObject * pBase )
    SAL_THROW( (::com::sun::star::uno::RuntimeException) );
/** WeakImplHelper
*/
CPPUHELPER_DLLPUBLIC ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type >
SAL_CALL WeakImplHelper_getTypes(
    class_data * cd )
    SAL_THROW( (::com::sun::star::uno::RuntimeException) );

/** WeakAggImplHelper
*/
CPPUHELPER_DLLPUBLIC ::com::sun::star::uno::Any
SAL_CALL WeakAggImplHelper_queryAgg(
    ::com::sun::star::uno::Type const & rType,
    class_data * cd,
    void * that,
    ::cppu::OWeakAggObject * pBase )
    SAL_THROW( (::com::sun::star::uno::RuntimeException) );
/** WeakAggImplHelper
*/
CPPUHELPER_DLLPUBLIC ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type >
SAL_CALL WeakAggImplHelper_getTypes(
    class_data * cd )
    SAL_THROW( (::com::sun::star::uno::RuntimeException) );

}

/// @endcond

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
