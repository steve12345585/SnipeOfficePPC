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

#ifndef _CONNECTIVITY_JAVA_SQL_REF_HXX_
#define _CONNECTIVITY_JAVA_SQL_REF_HXX_

#include "java/lang/Object.hxx"
#include <com/sun/star/sdbc/XRef.hpp>
#include <cppuhelper/implbase1.hxx>

namespace connectivity
{
    //**************************************************************
    //************ Class: java.sql.Ref
    //**************************************************************
    class java_sql_Ref :    public java_lang_Object,
                            public ::cppu::WeakImplHelper1< ::com::sun::star::sdbc::XRef>
    {
    protected:
    // Static data for the class
        static jclass theClass;
        virtual ~java_sql_Ref();
    public:
        virtual jclass getMyClass() const;

        // A ctor that is needed for returning the object
        java_sql_Ref( JNIEnv * pEnv, jobject myObj );

        // XRef
        virtual ::rtl::OUString SAL_CALL getBaseTypeName(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
    };
}
#endif // _CONNECTIVITY_JAVA_SQL_REF_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
