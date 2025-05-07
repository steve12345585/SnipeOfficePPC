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

#ifndef __FILTER_TEXTFILTERDETECT_FILTERDETECT_HXX__
#define __FILTER_TEXTFILTERDETECT_FILTERDETECT_HXX__

#include <com/sun/star/document/XExtendedFilterDetection.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>

#include <cppuhelper/implbase3.hxx>

class PlainTextFilterDetect : public cppu::WeakImplHelper3<
    com::sun::star::document::XExtendedFilterDetection,
    com::sun::star::lang::XInitialization,
    com::sun::star::lang::XServiceInfo>
{
    com::sun::star::uno::Reference<com::sun::star::lang::XMultiServiceFactory> mxMSF;

public:

    PlainTextFilterDetect (const com::sun::star::uno::Reference<com::sun::star::lang::XMultiServiceFactory> &xMSF);
    virtual ~PlainTextFilterDetect();

    // XExtendedFilterDetection

    virtual ::rtl::OUString SAL_CALL detect(com::sun::star::uno::Sequence<com::sun::star::beans::PropertyValue>& lDescriptor)
            throw( com::sun::star::uno::RuntimeException );

    // XInitialization

    virtual void SAL_CALL initialize( const ::com::sun::star::uno::Sequence<com::sun::star::uno::Any>& aArguments)
        throw (com::sun::star::uno::Exception, com::sun::star::uno::RuntimeException);

    // XServiceInfo

    virtual ::rtl::OUString SAL_CALL getImplementationName()
        throw (com::sun::star::uno::RuntimeException);

    virtual sal_Bool SAL_CALL supportsService(const rtl::OUString& ServiceName)
        throw (com::sun::star::uno::RuntimeException);

    virtual com::sun::star::uno::Sequence<rtl::OUString> SAL_CALL getSupportedServiceNames()
        throw (com::sun::star::uno::RuntimeException);
};

rtl::OUString PlainTextFilterDetect_getImplementationName();

sal_Bool PlainTextFilterDetect_supportsService(const rtl::OUString& ServiceName);

com::sun::star::uno::Sequence<rtl::OUString> PlainTextFilterDetect_getSupportedServiceNames();

com::sun::star::uno::Reference<com::sun::star::uno::XInterface>
PlainTextFilterDetect_createInstance(const com::sun::star::uno::Reference<com::sun::star::lang::XMultiServiceFactory>& rSMgr);

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
