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

#ifndef __SVTOOLS_POPUPWINDOWCONTROLLER_HXX_
#define __SVTOOLS_POPUPWINDOWCONTROLLER_HXX_

#include "svtools/svtdllapi.h"

#include <com/sun/star/lang/XServiceInfo.hpp>

#include "svtools/toolboxcontroller.hxx"
#include "boost/scoped_ptr.hpp"

class Window;

namespace svt
{
class PopupWindowControllerImpl;

class SVT_DLLPUBLIC PopupWindowController : public svt::ToolboxController, public ::com::sun::star::lang::XServiceInfo
{
public:
    PopupWindowController( const com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >& rServiceManager,
                           const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& xFrame,
                           const rtl::OUString& aCommandURL );
    ~PopupWindowController();

    virtual ::Window* createPopupWindow( ::Window* pParent ) = 0;

    // XInterface
    virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& aType ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL acquire() throw ();
    virtual void SAL_CALL release() throw ();

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName() throw( ::com::sun::star::uno::RuntimeException ) = 0;
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName ) throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames() throw( ::com::sun::star::uno::RuntimeException ) = 0;

    // XInitialization
    virtual void SAL_CALL initialize( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments ) throw (::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);

    // XComponent
    virtual void SAL_CALL dispose() throw (::com::sun::star::uno::RuntimeException);

    // XStatusListener
    virtual void SAL_CALL statusChanged( const ::com::sun::star::frame::FeatureStateEvent& Event ) throw ( ::com::sun::star::uno::RuntimeException );

    // XToolbarController
    virtual void SAL_CALL execute( sal_Int16 KeyModifier ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL click() throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL doubleClick() throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow > SAL_CALL createPopupWindow() throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow > SAL_CALL createItemWindow( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow >& Parent ) throw (::com::sun::star::uno::RuntimeException);
private:
    boost::scoped_ptr< PopupWindowControllerImpl >  mpImpl;
};

} // namespace svt

#endif // __SVTOOLS_POPUPWINDOWCONTROLLER_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
