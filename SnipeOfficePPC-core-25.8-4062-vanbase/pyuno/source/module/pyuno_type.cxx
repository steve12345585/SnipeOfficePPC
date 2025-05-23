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
#include "pyuno_impl.hxx"

#include <rtl/ustrbuf.hxx>
#include <rtl/strbuf.hxx>

#include <typelib/typedescription.hxx>

using rtl::OString;
using rtl::OUString;
using rtl::OUStringBuffer;
using rtl::OUStringToOString;
using rtl::OStringBuffer;

using com::sun::star::uno::TypeClass;
using com::sun::star::uno::Type;
using com::sun::star::uno::RuntimeException;
using com::sun::star::uno::Any;
using com::sun::star::uno::XInterface;
using com::sun::star::uno::Reference;
using com::sun::star::uno::TypeDescription;

#define USTR_ASCII(x) rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( x ) )
namespace pyuno
{
const char *typeClassToString( TypeClass t )
{
    const char * ret = 0;
    switch (t)
    {
    case com::sun::star::uno::TypeClass_VOID:
        ret = "VOID"; break;
    case com::sun::star::uno::TypeClass_CHAR:
        ret = "CHAR"; break;
    case com::sun::star::uno::TypeClass_BOOLEAN:
        ret = "BOOLEAN"; break;
    case com::sun::star::uno::TypeClass_BYTE:
        ret = "BYTE"; break;
    case com::sun::star::uno::TypeClass_SHORT:
        ret = "SHORT"; break;
    case com::sun::star::uno::TypeClass_UNSIGNED_SHORT:
        ret = "UNSIGNED_SHORT"; break;
    case com::sun::star::uno::TypeClass_LONG:
        ret = "LONG"; break;
    case com::sun::star::uno::TypeClass_UNSIGNED_LONG:
        ret = "UNSIGNED_LONG"; break;
    case com::sun::star::uno::TypeClass_HYPER:
        ret = "HYPER"; break;
    case com::sun::star::uno::TypeClass_UNSIGNED_HYPER:
        ret = "UNSIGNED_HYPER"; break;
    case com::sun::star::uno::TypeClass_FLOAT:
        ret = "FLOAT"; break;
    case com::sun::star::uno::TypeClass_DOUBLE:
        ret = "DOUBLE"; break;
    case com::sun::star::uno::TypeClass_STRING:
        ret = "STRING"; break;
    case com::sun::star::uno::TypeClass_TYPE:
        ret = "TYPE"; break;
    case com::sun::star::uno::TypeClass_ANY:
        ret = "ANY";break;
    case com::sun::star::uno::TypeClass_ENUM:
        ret = "ENUM";break;
    case com::sun::star::uno::TypeClass_STRUCT:
        ret = "STRUCT"; break;
    case com::sun::star::uno::TypeClass_EXCEPTION:
        ret = "EXCEPTION"; break;
    case com::sun::star::uno::TypeClass_SEQUENCE:
        ret = "SEQUENCE"; break;
    case com::sun::star::uno::TypeClass_INTERFACE:
        ret = "INTERFACE"; break;
    case com::sun::star::uno::TypeClass_TYPEDEF:
        ret = "TYPEDEF"; break;
    case com::sun::star::uno::TypeClass_UNION:
        ret = "UNION"; break;
    case com::sun::star::uno::TypeClass_ARRAY:
        ret = "ARRAY"; break;
    case com::sun::star::uno::TypeClass_SERVICE:
        ret = "SERVICE"; break;
    case com::sun::star::uno::TypeClass_MODULE:
        ret = "MODULE"; break;
    case com::sun::star::uno::TypeClass_INTERFACE_METHOD:
        ret = "INTERFACE_METHOD"; break;
    case com::sun::star::uno::TypeClass_INTERFACE_ATTRIBUTE:
        ret = "INTERFACE_ATTRIBUTE"; break;
    default:
        ret = "UNKNOWN"; break;
    }
    return ret;
}

static PyRef getClass( const Runtime & r , const char * name)
{
    return PyRef( PyDict_GetItemString( r.getImpl()->cargo->getUnoModule().get(), (char*) name ) );
}

PyRef getTypeClass( const Runtime & r )
{
    return getClass( r , "Type" );
}

PyRef getEnumClass( const Runtime & r )
{
    return getClass( r , "Enum" );
}

PyRef getCharClass( const Runtime & r )
{
    return getClass( r , "Char" );
}

PyRef getByteSequenceClass( const Runtime & r )
{
    return getClass( r , "ByteSequence" );
}

PyRef getAnyClass( const Runtime & r )
{
    return getClass( r , "Any" );
}


sal_Unicode PyChar2Unicode( PyObject *obj ) throw ( RuntimeException )
{
    PyRef value( PyObject_GetAttrString( obj, "value" ), SAL_NO_ACQUIRE );
    if( ! PyUnicode_Check( value.get() ) )
    {
        throw RuntimeException(
            USTR_ASCII( "attribute value of uno.Char is not a unicode string" ),
            Reference< XInterface > () );
    }

    if( PyUnicode_GetSize( value.get() ) < 1 )
    {
        throw RuntimeException(
            USTR_ASCII( "uno.Char contains an empty unicode string" ),
            Reference< XInterface > () );
    }

    sal_Unicode c = (sal_Unicode)PyUnicode_AsUnicode( value.get() )[0];
    return c;
}

Any PyEnum2Enum( PyObject *obj ) throw ( RuntimeException )
{
    Any ret;
    PyRef typeName( PyObject_GetAttrString( obj,"typeName" ), SAL_NO_ACQUIRE);
    PyRef value( PyObject_GetAttrString( obj, "value" ), SAL_NO_ACQUIRE);
    if( !PyStr_Check( typeName.get() ) || ! PyStr_Check( value.get() ) )
    {
        throw RuntimeException(
            USTR_ASCII( "attributes typeName and/or value of uno.Enum are not strings" ),
            Reference< XInterface > () );
    }

    OUString strTypeName( OUString::createFromAscii( PyStr_AsString( typeName.get() ) ) );
    char *stringValue = PyStr_AsString( value.get() );

    TypeDescription desc( strTypeName );
    if( desc.is() )
    {
        if(desc.get()->eTypeClass != typelib_TypeClass_ENUM )
        {
            OUStringBuffer buf;
            buf.appendAscii( "pyuno.checkEnum: " ).append(strTypeName).appendAscii( "is a " );
            buf.appendAscii(
                typeClassToString( (com::sun::star::uno::TypeClass) desc.get()->eTypeClass));
            buf.appendAscii( ", expected ENUM" );
            throw RuntimeException( buf.makeStringAndClear(), Reference< XInterface>  () );
        }

        desc.makeComplete();

        typelib_EnumTypeDescription *pEnumDesc = (typelib_EnumTypeDescription*) desc.get();
        int i = 0;
        for( i = 0; i < pEnumDesc->nEnumValues ; i ++ )
        {
            if( (*((OUString *)&pEnumDesc->ppEnumNames[i])).compareToAscii( stringValue ) == 0 )
            {
                break;
            }
        }
        if( i == pEnumDesc->nEnumValues )
        {
            OUStringBuffer buf;
            buf.appendAscii( "value " ).appendAscii( stringValue ).appendAscii( "is unknown in enum " );
            buf.appendAscii( PyStr_AsString( typeName.get() ) );
            throw RuntimeException( buf.makeStringAndClear(), Reference<XInterface> () );
        }
        ret = Any( &pEnumDesc->pEnumValues[i], desc.get()->pWeakRef );
    }
    else
    {
        OUStringBuffer buf;
        buf.appendAscii( "enum " ).appendAscii( PyStr_AsString(typeName.get()) ).appendAscii( " is unknown" );
        throw RuntimeException( buf.makeStringAndClear(), Reference< XInterface>  () );
    }
    return ret;
}


Type PyType2Type( PyObject * o ) throw(RuntimeException )
{
    PyRef pyName( PyObject_GetAttrString( o, "typeName" ), SAL_NO_ACQUIRE);
    if( !PyStr_Check( pyName.get() ) )
    {
        throw RuntimeException(
            USTR_ASCII( "type object does not have typeName property" ),
            Reference< XInterface > () );
    }

    PyRef pyTC( PyObject_GetAttrString( o, "typeClass" ), SAL_NO_ACQUIRE );
    Any enumValue = PyEnum2Enum( pyTC.get() );

    OUString name( OUString::createFromAscii( PyStr_AsString( pyName.get() ) ) );
    TypeDescription desc( name );
    if( ! desc.is() )
    {
        OUStringBuffer buf;
        buf.appendAscii( "type " ).append(name).appendAscii( " is unknown" );
        throw RuntimeException(
            buf.makeStringAndClear(), Reference< XInterface > () );
    }
    if( desc.get()->eTypeClass != (typelib_TypeClass) *(sal_Int32*)enumValue.getValue() )
    {
        OUStringBuffer buf;
        buf.appendAscii( "pyuno.checkType: " ).append(name).appendAscii( " is a " );
        buf.appendAscii( typeClassToString( (TypeClass) desc.get()->eTypeClass) );
        buf.appendAscii( ", but type got construct with typeclass " );
        buf.appendAscii( typeClassToString( (TypeClass) *(sal_Int32*)enumValue.getValue() ) );
        throw RuntimeException(
            buf.makeStringAndClear(), Reference< XInterface > () );
    }
    return desc.get()->pWeakRef;
}

static PyObject* callCtor( const Runtime &r , const char * clazz, const PyRef & args )
{
    PyRef code( PyDict_GetItemString( r.getImpl()->cargo->getUnoModule().get(), (char*)clazz ) );
    if( ! code.is() )
    {
        OStringBuffer buf;
        buf.append( "couldn't access uno." );
        buf.append( clazz );
        PyErr_SetString( PyExc_RuntimeError, buf.getStr() );
        return NULL;
    }
    PyRef instance( PyObject_CallObject( code.get(), args.get()  ), SAL_NO_ACQUIRE);
    Py_XINCREF( instance.get() );
    return instance.get();

}


PyObject *PyUNO_Enum_new( const char *enumBase, const char *enumValue, const Runtime &r )
{
    PyRef args( PyTuple_New( 2 ), SAL_NO_ACQUIRE );
    PyTuple_SetItem( args.get() , 0 , PyStr_FromString( enumBase ) );
    PyTuple_SetItem( args.get() , 1 , PyStr_FromString( enumValue ) );

    return callCtor( r, "Enum" , args );
}


PyObject* PyUNO_Type_new (const char *typeName , TypeClass t , const Runtime &r )
{
    // retrieve type object
    PyRef args( PyTuple_New( 2 ), SAL_NO_ACQUIRE );

    PyTuple_SetItem( args.get() , 0 , PyStr_FromString( typeName ) );
    PyObject *typeClass = PyUNO_Enum_new( "com.sun.star.uno.TypeClass" , typeClassToString(t), r );
    if( ! typeClass )
        return NULL;
    PyTuple_SetItem( args.get() , 1 , typeClass);

    return callCtor( r, "Type" , args );
}

PyObject* PyUNO_char_new ( sal_Unicode val , const Runtime &r )
{
    // retrieve type object
    PyRef args( PyTuple_New( 1 ), SAL_NO_ACQUIRE );

    Py_UNICODE u[2];
    u[0] = val;
    u[1] = 0;
    PyTuple_SetItem( args.get() , 0 , PyUnicode_FromUnicode( u ,1) );

    return callCtor( r, "Char" , args );
}

PyObject *PyUNO_ByteSequence_new(
    const com::sun::star::uno::Sequence< sal_Int8 > &byteSequence, const Runtime &r )
{
    PyRef str(
        PyStrBytes_FromStringAndSize( (char*)byteSequence.getConstArray(), byteSequence.getLength()),
        SAL_NO_ACQUIRE );
    PyRef args( PyTuple_New( 1 ), SAL_NO_ACQUIRE );
    PyTuple_SetItem( args.get() , 0 , str.getAcquired() );
    return callCtor( r, "ByteSequence" , args );

}
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
