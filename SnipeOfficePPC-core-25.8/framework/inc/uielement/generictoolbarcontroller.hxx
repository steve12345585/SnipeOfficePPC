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

#ifndef __FRAMEWORK_UIELEMENT_GENERICTOOLBARCONTROLLER_HXX_
#define __FRAMEWORK_UIELEMENT_GENERICTOOLBARCONTROLLER_HXX_

//_________________________________________________________________________________________________________________
//  includes of other projects
//_________________________________________________________________________________________________________________

#include <svtools/toolboxcontroller.hxx>
#include <vcl/toolbox.hxx>
#include <memory>

class PopupMenu;

namespace framework
{

struct ExecuteInfo;
class ToolBar;
class GenericToolbarController : public svt::ToolboxController
{
    public:
        GenericToolbarController( const com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >& rServiceManager,
                                  const com::sun::star::uno::Reference< com::sun::star::frame::XFrame >& rFrame,
                                  ToolBox* pToolBar,
                                  sal_uInt16   nID,
                                  const rtl::OUString& aCommand );
        virtual ~GenericToolbarController();

        // XComponent
        virtual void SAL_CALL dispose() throw ( ::com::sun::star::uno::RuntimeException );

        // XToolbarController
        virtual void SAL_CALL execute( sal_Int16 KeyModifier ) throw (::com::sun::star::uno::RuntimeException);

        // XStatusListener
        virtual void SAL_CALL statusChanged( const ::com::sun::star::frame::FeatureStateEvent& Event ) throw ( ::com::sun::star::uno::RuntimeException );

         DECL_STATIC_LINK( GenericToolbarController, ExecuteHdl_Impl, ExecuteInfo* );

    protected:
        ToolBox*        m_pToolbar;
        sal_uInt16      m_nID;
        sal_Bool        m_bEnumCommand : 1,
                        m_bMadeInvisible : 1;
        rtl::OUString   m_aEnumCommand;
};

class MenuToolbarController : public GenericToolbarController
{
    com::sun::star::uno::Reference< com::sun::star::container::XIndexAccess > m_xMenuDesc;
    PopupMenu* pMenu;
    com::sun::star::uno::Reference< com::sun::star::lang::XComponent > m_xMenuManager;
    rtl::OUString m_aModuleIdentifier;
    public:
        MenuToolbarController( const com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >& rServiceManager,
                                  const com::sun::star::uno::Reference< com::sun::star::frame::XFrame >& rFrame,
                                  ToolBox* pToolBar,
                                  sal_uInt16   nID,
                                  const rtl::OUString& aCommand,
                                  const rtl::OUString& aModuleIdentifier,
                                  const com::sun::star::uno::Reference< com::sun::star::container::XIndexAccess >& xMenuDesc );

    ~MenuToolbarController();
    // XToolbarController
    virtual void SAL_CALL click() throw ( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow > SAL_CALL createPopupWindow() throw (::com::sun::star::uno::RuntimeException);

};

}

#endif // __FRAMEWORK_UIELEMENT_GENERICTOOLBARCONTROLLER_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
