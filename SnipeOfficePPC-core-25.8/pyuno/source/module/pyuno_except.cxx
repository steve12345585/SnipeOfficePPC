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
#include "pyuno_impl.hxx"

#include <rtl/ustrbuf.hxx>

#include <typelib/typedescription.hxx>

using rtl::OUString;
using rtl::OUStringBuffer;
using rtl::OUStringToOString;

using com::sun::star::uno::RuntimeException;
using com::sun::star::uno::Sequence;
using com::sun::star::uno::Type;
using com::sun::star::uno::Reference;
using com::sun::star::uno::XInterface;
using com::sun::star::uno::TypeDescription;

namespace pyuno
{

void raisePyExceptionWithAny( const com::sun::star::uno::Any &anyExc )
{
    try
    {
        Runtime runtime;
        PyRef exc = runtime.any2PyObject( anyExc );
        if( exc.is() )
        {
            PyRef type( getClass( anyExc.getValueType().getTypeName(),runtime ) );
            PyErr_SetObject( type.get(), exc.get());
        }
        else
        {
            com::sun::star::uno::Exception e;
            anyExc >>= e;

            OUStringBuffer buf;
            buf.appendAscii( "Couldn't convert uno exception to a python exception (" );
            buf.append(anyExc.getValueType().getTypeName());
            buf.appendAscii( ": " );
            buf.append(e.Message );
            buf.appendAscii( ")" );
            PyErr_SetString(
                PyExc_SystemError,
                OUStringToOString(buf.makeStringAndClear(),RTL_TEXTENCODING_ASCII_US).getStr() );
        }
    }
    catch(const com::sun::star::lang::IllegalArgumentException & e)
    {
        PyErr_SetString( PyExc_SystemError,
                         OUStringToOString( e.Message, RTL_TEXTENCODING_ASCII_US).getStr() );
    }
    catch(const com::sun::star::script::CannotConvertException & e)
    {
        PyErr_SetString( PyExc_SystemError,
                         OUStringToOString( e.Message, RTL_TEXTENCODING_ASCII_US).getStr() );
    }
    catch(const RuntimeException & e)
    {
        PyErr_SetString( PyExc_SystemError,
                         OUStringToOString( e.Message, RTL_TEXTENCODING_ASCII_US).getStr() );
    }
}


static PyRef createClass( const OUString & name, const Runtime &runtime )
    throw ( RuntimeException )
{
    // assuming that this is never deleted !
    // note I don't have the knowledge how to initialize these type objects correctly !
    TypeDescription desc( name );
    if( ! desc.is() )
    {
        OUStringBuffer buf;
        buf.appendAscii( "pyuno.getClass: uno exception " );
        buf.append(name).appendAscii( " is unknown" );
        throw RuntimeException( buf.makeStringAndClear(), Reference< XInterface > () );
    }

    sal_Bool isStruct = desc.get()->eTypeClass == typelib_TypeClass_STRUCT;
    sal_Bool isExc = desc.get()->eTypeClass == typelib_TypeClass_EXCEPTION;
    sal_Bool isInterface = desc.get()->eTypeClass == typelib_TypeClass_INTERFACE;
    if( !isStruct  && !isExc && ! isInterface )
    {
        OUStringBuffer buf;
        buf.appendAscii( "pyuno.getClass: " ).append(name).appendAscii( "is a " );
        buf.appendAscii(
            typeClassToString( (com::sun::star::uno::TypeClass) desc.get()->eTypeClass));
        buf.appendAscii( ", expected EXCEPTION, STRUCT or INTERFACE" );
        throw RuntimeException( buf.makeStringAndClear(), Reference< XInterface>() );
    }

    // retrieve base class
    PyRef base;
    if( isInterface )
    {
        typelib_InterfaceTypeDescription *pDesc = (typelib_InterfaceTypeDescription * )desc.get();
        if( pDesc->pBaseTypeDescription )
        {
            base = getClass( pDesc->pBaseTypeDescription->aBase.pTypeName, runtime );
        }
        else
        {
            // must be XInterface !
        }
    }
    else
    {
        typelib_CompoundTypeDescription *pDesc = (typelib_CompoundTypeDescription*)desc.get();
        if( pDesc->pBaseTypeDescription )
        {
            base = getClass( pDesc->pBaseTypeDescription->aBase.pTypeName, runtime );
        }
        else
        {
            if( isExc )
                // we are currently creating the root UNO exception
                base = PyRef(PyExc_Exception);
        }
    }
    PyRef args( PyTuple_New( 3 ), SAL_NO_ACQUIRE );

    PyRef pyTypeName = ustring2PyString( name /*.replace( '.', '_' )*/ );

    PyRef bases;
    if( base.is() )
    {
        { // for CC, keeping ref-count being 1
        bases = PyRef( PyTuple_New( 1 ), SAL_NO_ACQUIRE );
        }
        PyTuple_SetItem( bases.get(), 0 , base.getAcquired() );
    }
    else
    {
        bases = PyRef( PyTuple_New( 0 ), SAL_NO_ACQUIRE );
    }

    PyTuple_SetItem( args.get(), 0, pyTypeName.getAcquired());
    PyTuple_SetItem( args.get(), 1, bases.getAcquired() );
    PyTuple_SetItem( args.get(), 2, PyDict_New() );

    PyRef ret(
        PyObject_CallObject(reinterpret_cast<PyObject *>(&PyType_Type) , args.get()),
        SAL_NO_ACQUIRE );

    // now overwrite ctor and attrib functions
    if( isInterface )
    {
        PyObject_SetAttrString(
            ret.get(), "__pyunointerface__",
            ustring2PyString(name).get() );
    }
    else
    {
        PyRef ctor = getObjectFromUnoModule( runtime,"_uno_struct__init__" );
        PyRef setter = getObjectFromUnoModule( runtime,"_uno_struct__setattr__" );
        PyRef getter = getObjectFromUnoModule( runtime,"_uno_struct__getattr__" );
        PyRef repr = getObjectFromUnoModule( runtime,"_uno_struct__repr__" );
        PyRef eq = getObjectFromUnoModule( runtime,"_uno_struct__eq__" );

        PyObject_SetAttrString(
            ret.get(), "__pyunostruct__",
            ustring2PyString(name).get() );
        PyObject_SetAttrString(
            ret.get(), "typeName",
            ustring2PyString(name).get() );
        PyObject_SetAttrString(
            ret.get(), "__init__", ctor.get() );
        PyObject_SetAttrString(
            ret.get(), "__getattr__", getter.get() );
        PyObject_SetAttrString(
            ret.get(), "__setattr__", setter.get() );
        PyObject_SetAttrString(
            ret.get(), "__repr__", repr.get() );
        PyObject_SetAttrString(
            ret.get(), "__str__", repr.get() );
        PyObject_SetAttrString(
            ret.get(), "__eq__", eq.get() );
    }
    return ret;
}

bool isInstanceOfStructOrException( PyObject *obj)
{
    PyRef attr(
        PyObject_GetAttrString(obj, "__class__"),
        SAL_NO_ACQUIRE );
    return PyObject_HasAttrString(
        attr.get(), "__pyunostruct__");
}

sal_Bool isInterfaceClass( const Runtime &runtime, PyObject * obj )
{
    const ClassSet & set = runtime.getImpl()->cargo->interfaceSet;
    return set.find( obj ) != set.end();
}

PyRef getClass( const OUString & name , const Runtime &runtime)
{
    PyRef ret;

    RuntimeCargo *cargo =runtime.getImpl()->cargo;
    ExceptionClassMap::iterator ii = cargo->exceptionMap.find( name );
    if( ii == cargo->exceptionMap.end() )
    {
        ret = createClass( name, runtime );
        cargo->exceptionMap[name] = ret;
        if( PyObject_HasAttrString(
                ret.get(), "__pyunointerface__" ) )
            cargo->interfaceSet.insert( ret );

        PyObject_SetAttrString(
            ret.get(), "__pyunointerface__",
            ustring2PyString(name).get() );
    }
    else
    {
        ret = ii->second;
    }

    return ret;
}


}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
