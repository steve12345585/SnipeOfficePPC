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
#ifndef _TYPELIB_TYPEDESCRIPTION_HXX_
#define _TYPELIB_TYPEDESCRIPTION_HXX_

#include <rtl/alloc.h>
#include <rtl/ustring.hxx>
#include <com/sun/star/uno/Type.h>
#include <typelib/typedescription.h>


namespace com
{
namespace sun
{
namespace star
{
namespace uno
{

/** C++ wrapper for typelib_TypeDescription.
    Constructors by name, type, type description reference will get the full type description.

    @see typelib_TypeDescription
*/
class TypeDescription
{
    /** C typelib type description
    */
    mutable typelib_TypeDescription * _pTypeDescr;

public:
    /// @cond INTERNAL
    // these are here to force memory de/allocation to sal lib.
    inline static void * SAL_CALL operator new ( size_t nSize ) SAL_THROW(())
        { return ::rtl_allocateMemory( nSize ); }
    inline static void SAL_CALL operator delete ( void * pMem ) SAL_THROW(())
        { ::rtl_freeMemory( pMem ); }
    inline static void * SAL_CALL operator new ( size_t, void * pMem ) SAL_THROW(())
        { return pMem; }
    inline static void SAL_CALL operator delete ( void *, void * ) SAL_THROW(())
        {}
    /// @endcond

    /** Constructor:

        @param pTypeDescr a type description
    */
    inline TypeDescription( typelib_TypeDescription * pTypeDescr = 0 ) SAL_THROW(());
    /** Constructor:

        @param pTypeDescrRef a type description reference
    */
    inline TypeDescription( typelib_TypeDescriptionReference * pTypeDescrRef ) SAL_THROW(());
    /** Constructor:

        @param rType a type
    */
    inline TypeDescription( const ::com::sun::star::uno::Type & rType ) SAL_THROW(());
    /** Copy constructor:

        @param rDescr another TypeDescription
    */
    inline TypeDescription( const TypeDescription & rDescr ) SAL_THROW(());
    /** Constructor:

        @param pTypeName a type name
    */
    inline TypeDescription( rtl_uString * pTypeName ) SAL_THROW(());
    /** Constructor:

        @param rTypeName a type name
    */
    inline TypeDescription( const ::rtl::OUString & rTypeName ) SAL_THROW(());
    /** Destructor: releases type description
    */
    inline ~TypeDescription() SAL_THROW(());

    /** Assignment operator: acquires given type description and releases a set one.

        @param pTypeDescr another type description
        @return this TypeDescription
    */
    inline TypeDescription & SAL_CALL operator = ( typelib_TypeDescription * pTypeDescr ) SAL_THROW(());
    /** Assignment operator: acquires given type description and releases a set one.

        @param rTypeDescr another type description
        @return this TypeDescription
    */
    inline TypeDescription & SAL_CALL operator =( const TypeDescription & rTypeDescr ) SAL_THROW(())
        { return this->operator =( rTypeDescr.get() ); }

    /** Tests whether two type descriptions are equal.

        @param pTypeDescr another type description
        @return true, if both type descriptions are equal, false otherwise
    */
    inline sal_Bool SAL_CALL equals( const typelib_TypeDescription * pTypeDescr ) const SAL_THROW(());
    /** Tests whether two type descriptions are equal.

        @param rTypeDescr another type description
        @return true, if both type descriptions are equal, false otherwise
    */
    inline sal_Bool SAL_CALL equals( const TypeDescription & rTypeDescr ) const SAL_THROW(())
        { return equals( rTypeDescr._pTypeDescr ); }

    /** Makes stored type description complete.
    */
    inline void SAL_CALL makeComplete() const SAL_THROW(());

    /** Gets the UNacquired type description pointer.

        @return stored pointer of type description
    */
    inline typelib_TypeDescription * SAL_CALL get() const SAL_THROW(())
        { return _pTypeDescr; }
    /** Tests if a type description is set.

        @return true, if a type description is set, false otherwise
    */
    inline sal_Bool SAL_CALL is() const SAL_THROW(())
        { return (_pTypeDescr != 0); }
};
//__________________________________________________________________________________________________
inline TypeDescription::TypeDescription( typelib_TypeDescription * pTypeDescr ) SAL_THROW(())
    : _pTypeDescr( pTypeDescr )
{
    if (_pTypeDescr)
        typelib_typedescription_acquire( _pTypeDescr );
}
//__________________________________________________________________________________________________
inline TypeDescription::TypeDescription( typelib_TypeDescriptionReference * pTypeDescrRef ) SAL_THROW(())
    : _pTypeDescr( 0 )
{
    if (pTypeDescrRef)
        typelib_typedescriptionreference_getDescription( &_pTypeDescr, pTypeDescrRef );
}
//__________________________________________________________________________________________________
inline TypeDescription::TypeDescription( const ::com::sun::star::uno::Type & rType ) SAL_THROW(())
    : _pTypeDescr( 0 )
{
    if (rType.getTypeLibType())
        typelib_typedescriptionreference_getDescription( &_pTypeDescr, rType.getTypeLibType() );
}
//__________________________________________________________________________________________________
inline TypeDescription::TypeDescription( const TypeDescription & rTypeDescr ) SAL_THROW(())
    : _pTypeDescr( rTypeDescr._pTypeDescr )
{
    if (_pTypeDescr)
        typelib_typedescription_acquire( _pTypeDescr );
}
//__________________________________________________________________________________________________
inline TypeDescription::TypeDescription( rtl_uString * pTypeName ) SAL_THROW(())
    : _pTypeDescr( 0 )
{
    typelib_typedescription_getByName( &_pTypeDescr , pTypeName );
}
//__________________________________________________________________________________________________
inline TypeDescription::TypeDescription( const ::rtl::OUString & rTypeName ) SAL_THROW(())
    : _pTypeDescr( 0 )
{
    typelib_typedescription_getByName( &_pTypeDescr , rTypeName.pData );
}
//__________________________________________________________________________________________________
inline TypeDescription::~TypeDescription() SAL_THROW(())
{
    if (_pTypeDescr)
        typelib_typedescription_release( _pTypeDescr );
}
//__________________________________________________________________________________________________
inline TypeDescription & TypeDescription::operator = ( typelib_TypeDescription * pTypeDescr ) SAL_THROW(())
{
    if (pTypeDescr)
        typelib_typedescription_acquire( pTypeDescr );
    if (_pTypeDescr)
        typelib_typedescription_release( _pTypeDescr );
    _pTypeDescr = pTypeDescr;
    return *this;
}
//__________________________________________________________________________________________________
inline sal_Bool TypeDescription::equals( const typelib_TypeDescription * pTypeDescr ) const SAL_THROW(())
{
    return (_pTypeDescr && pTypeDescr &&
            typelib_typedescription_equals( _pTypeDescr, pTypeDescr ));
}
//__________________________________________________________________________________________________
inline void TypeDescription::makeComplete() const SAL_THROW(())
{
    if (_pTypeDescr && !_pTypeDescr->bComplete)
        ::typelib_typedescription_complete( &_pTypeDescr );
}

}
}
}
}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
