/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * A LibreOffice extension to send the menubar structure through DBusMenu
 *
 * Copyright 2011 Canonical, Ltd.
 * Authors:
 *     Alberto Ruiz <alberto.ruiz@codethink.co.uk>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the the GNU Lesser General Public License version 3, as published by the Free
 * Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranties of MERCHANTABILITY,
 * SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR PURPOSE.  See the applicable
 * version of the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with this program. If not, see <http://www.gnu.org/licenses/>
 *
 */

#include <stdio.h>

#include <osl/mutex.hxx>
#include <osl/thread.h>
#include <cppuhelper/factory.hxx>
#include <rtl/ustring.hxx>
#include <rtl/ustrbuf.hxx>
#include <sal/types.h>

#ifndef _COM_SUN_STAR_LANG_XSINGLESERVICEFACTORY_HPP_
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#endif

#include "DesktopJob.hxx"
#include "FrameJob.hxx"

namespace css = ::com::sun::star;

extern "C"
{
SAL_DLLPUBLIC_EXPORT void* SAL_CALL lomenubar_component_getFactory(const sal_Char* pImplName      ,
                                                                void*     pServiceManager,
                                                                void*     /*pRegistryKey*/  )
{
    if ( !pServiceManager || !pImplName )
        return 0;

    css::uno::Reference< css::lang::XSingleServiceFactory > xFactory  ;
    css::uno::Reference< css::lang::XMultiServiceFactory >  xSMGR     (reinterpret_cast< css::lang::XMultiServiceFactory* >(pServiceManager), css::uno::UNO_QUERY);
    ::rtl::OUString                                         sImplName = ::rtl::OUString::createFromAscii(pImplName);

    if (sImplName.equalsAscii(DESKTOPJOB_IMPLEMENTATION_NAME))
    {
        css::uno::Sequence< ::rtl::OUString > lNames(1);
        lNames[0] = ::rtl::OUString::createFromAscii(DESKTOPJOB_IMPLEMENTATION_NAME);
        xFactory = ::cppu::createSingleFactory(xSMGR, sImplName,  DesktopJob_createInstance, lNames);
    }
    if (sImplName.equalsAscii(FRAMEJOB_IMPLEMENTATION_NAME))
    {
        css::uno::Sequence< ::rtl::OUString > lNames(1);
        lNames[0] = ::rtl::OUString::createFromAscii(FRAMEJOB_IMPLEMENTATION_NAME);
        xFactory = ::cppu::createSingleFactory(xSMGR, sImplName,  FrameJob_createInstance, lNames);
    }


    if (!xFactory.is())
        return 0;

    xFactory->acquire();
    return xFactory.get();
}

} // extern C

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
