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


#include <stdio.h>

#include "ScriptImpl.hxx"
#include <util/util.hxx>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::script::framework;

namespace func_provider
{

//*************************************************************************
ScriptImpl::ScriptImpl(
    const Reference< beans::XPropertySet > & scriptingContext,
    const Reference< runtime::XScriptInvocation > & runtimeMgr,
    const ::rtl::OUString& scriptURI )
throw ( RuntimeException ) :
        m_XScriptingContext( scriptingContext, UNO_SET_THROW ),
        m_RunTimeManager( runtimeMgr, UNO_SET_THROW ),
        m_ScriptURI( scriptURI )
{
    OSL_TRACE( "<!constucting a ScriptImpl>" );
}

//*************************************************************************
ScriptImpl::~ScriptImpl()
{
    OSL_TRACE( "<Destructing a ScriptImpl>" );
}

//*************************************************************************
Any SAL_CALL
ScriptImpl::invoke( const Sequence< Any >& aParams,
                      Sequence< sal_Int16 >& aOutParamIndex, Sequence< Any >& aOutParam )
throw ( lang::IllegalArgumentException, script::CannotConvertException,
        reflection::InvocationTargetException, RuntimeException )
{
    OSL_TRACE( "<ScriptImpl::invoke>" );
    Any result;
    Any anyScriptingContext;

    anyScriptingContext <<= m_XScriptingContext;
    try
    {
        result = m_RunTimeManager->invoke( m_ScriptURI, anyScriptingContext, aParams,
                                           aOutParamIndex, aOutParam );
    }
    catch ( const lang::IllegalArgumentException & iae )
    {
        ::rtl::OUString temp = OUSTR( "ScriptImpl::invoke IllegalArgumentException : " );
        throw lang::IllegalArgumentException( temp.concat( iae.Message ),
                                              Reference< XInterface > (),
                                              iae.ArgumentPosition );
    }
    catch ( const script::CannotConvertException & cce )
    {
        ::rtl::OUString temp = OUSTR( "ScriptImpl::invoke CannotConvertException : " );
        throw script::CannotConvertException( temp.concat( cce.Message ),
                                              Reference< XInterface > (),
                                              cce.DestinationTypeClass,
                                              cce.Reason,
                                              cce.ArgumentIndex );
    }
    catch ( const reflection::InvocationTargetException & ite )
    {
        ::rtl::OUString temp = OUSTR( "ScriptImpl::invoke InvocationTargetException : " );
        throw reflection::InvocationTargetException( temp.concat( ite.Message ),
                Reference< XInterface > (),
                ite.TargetException );
    }
    catch ( const RuntimeException & re )
    {
        ::rtl::OUString temp = OUSTR( "ScriptImpl::invoke RuntimeException : " );
        throw RuntimeException( temp.concat( re.Message ),
                                Reference< XInterface > () );
    }
#ifdef _DEBUG
    catch ( ... )
    {
        throw RuntimeException(
            OUSTR( "ScriptImpl::invoke Unknown Exception caught - RuntimeException rethrown" ),
            Reference< XInterface > () );
    }
#endif
    return result;
}
} // namespace func_provider

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
