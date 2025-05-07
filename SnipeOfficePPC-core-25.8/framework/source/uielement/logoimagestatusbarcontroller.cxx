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

#include <uielement/logoimagestatusbarcontroller.hxx>
#include <classes/fwlresid.hxx>
#include <services.h>
#include <classes/resource.hrc>
#include <osl/mutex.hxx>
#include <vcl/svapp.hxx>
#include <vcl/window.hxx>
#include <vcl/status.hxx>
#include <toolkit/unohlp.hxx>
#include <toolkit/helper/convert.hxx>

using namespace ::rtl;
using namespace ::cppu;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::frame;

namespace framework
{

DEFINE_XSERVICEINFO_MULTISERVICE        (   LogoImageStatusbarController            ,
                                            OWeakObject                             ,
                                            SERVICENAME_STATUSBARCONTROLLER         ,
                                            IMPLEMENTATIONNAME_LOGOIMAGESTATUSBARCONTROLLER
                                        )

DEFINE_INIT_SERVICE                     (   LogoImageStatusbarController, {} )

LogoImageStatusbarController::LogoImageStatusbarController( const uno::Reference< lang::XMultiServiceFactory >& xServiceManager ) :
    svt::StatusbarController( xServiceManager, uno::Reference< frame::XFrame >(), rtl::OUString(), 0 )
{
    Image aImage( FwlResId( RID_IMAGE_STATUSBAR_LOGO ));
    m_aLogoImage = aImage;
}

LogoImageStatusbarController::~LogoImageStatusbarController()
{
}

// XInterface
Any SAL_CALL LogoImageStatusbarController::queryInterface( const Type& rType )
throw ( RuntimeException )
{
    return svt::StatusbarController::queryInterface( rType );
}

void SAL_CALL LogoImageStatusbarController::acquire() throw ()
{
    svt::StatusbarController::acquire();
}

void SAL_CALL LogoImageStatusbarController::release() throw ()
{
    svt::StatusbarController::release();
}

void SAL_CALL LogoImageStatusbarController::initialize( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments )
throw (::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException)
{
    SolarMutexGuard aSolarMutexGuard;

    svt::StatusbarController::initialize( aArguments );
}

// XComponent
void SAL_CALL LogoImageStatusbarController::dispose()
throw (::com::sun::star::uno::RuntimeException)
{
    svt::StatusbarController::dispose();
}

// XEventListener
void SAL_CALL LogoImageStatusbarController::disposing( const EventObject& Source )
throw ( RuntimeException )
{
    svt::StatusbarController::disposing( Source );
}

// XStatusListener
void SAL_CALL LogoImageStatusbarController::statusChanged( const FeatureStateEvent& )
throw ( RuntimeException )
{
}

// XStatusbarController
::sal_Bool SAL_CALL LogoImageStatusbarController::mouseButtonDown(
    const awt::MouseEvent& )
throw (::com::sun::star::uno::RuntimeException)
{
    return sal_False;
}

::sal_Bool SAL_CALL LogoImageStatusbarController::mouseMove(
    const awt::MouseEvent& )
throw (::com::sun::star::uno::RuntimeException)
{
    return sal_False;
}

::sal_Bool SAL_CALL LogoImageStatusbarController::mouseButtonUp(
    const awt::MouseEvent& )
throw (::com::sun::star::uno::RuntimeException)
{
    return sal_False;
}

void SAL_CALL LogoImageStatusbarController::command(
    const awt::Point& aPos,
    ::sal_Int32 nCommand,
    ::sal_Bool bMouseEvent,
    const ::com::sun::star::uno::Any& aData )
throw (::com::sun::star::uno::RuntimeException)
{
    svt::StatusbarController::command( aPos, nCommand, bMouseEvent, aData );
}

void SAL_CALL LogoImageStatusbarController::paint(
    const ::com::sun::star::uno::Reference< awt::XGraphics >& xGraphics,
    const awt::Rectangle& rOutputRectangle,
    ::sal_Int32 /*nItemId*/,
    ::sal_Int32 /*nStyle*/ )
throw (::com::sun::star::uno::RuntimeException)
{
    SolarMutexGuard aGuard;

    OutputDevice* pOutDev = VCLUnoHelper::GetOutputDevice( xGraphics );;
    if ( pOutDev )
    {
        ::Rectangle aRect = VCLRectangle( rOutputRectangle );
        pOutDev->DrawImage( aRect.TopLeft(), aRect.GetSize(), m_aLogoImage );
    }
}

void SAL_CALL LogoImageStatusbarController::click()
throw (::com::sun::star::uno::RuntimeException)
{
    svt::StatusbarController::click();
}

void SAL_CALL LogoImageStatusbarController::doubleClick() throw (::com::sun::star::uno::RuntimeException)
{
    svt::StatusbarController::doubleClick();
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
