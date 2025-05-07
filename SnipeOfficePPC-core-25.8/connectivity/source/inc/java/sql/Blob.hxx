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

#ifndef _CONNECTIVITY_JAVA_SQL_BLOB_HXX_
#define _CONNECTIVITY_JAVA_SQL_BLOB_HXX_

#include "java/lang/Object.hxx"
#include <com/sun/star/sdbc/XBlob.hpp>
#include <cppuhelper/implbase1.hxx>

namespace connectivity
{

    //**************************************************************
    //************ Class: java.sql.SQLWarning
    //**************************************************************
    class java_sql_Blob :   public java_lang_Object,
                            public ::cppu::WeakImplHelper1< ::com::sun::star::sdbc::XBlob>
    {
    protected:
    // Static data for the class
        static jclass theClass;
        virtual ~java_sql_Blob();
    public:
        virtual jclass getMyClass() const;

        // A ctor that is needed for returning the object
        java_sql_Blob( JNIEnv * pEnv, jobject myObj );

        // XBlob
        virtual sal_Int64 SAL_CALL length(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getBytes( sal_Int64 pos, sal_Int32 length ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream > SAL_CALL getBinaryStream(  ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
        virtual sal_Int64 SAL_CALL position( const ::com::sun::star::uno::Sequence< sal_Int8 >& pattern, sal_Int64 start ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
        virtual sal_Int64 SAL_CALL positionOfBlob( const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XBlob >& pattern, sal_Int64 start ) throw(::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
    };
}
#endif // _CONNECTIVITY_JAVA_SQL_BLOB_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
