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

#ifndef SD_WORKBENCH_CTP_PANEL_HXX
#define SD_WORKBENCH_CTP_PANEL_HXX

#include <com/sun/star/ui/XToolPanel.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/awt/XPaintListener.hpp>
#include <com/sun/star/awt/XWindow.hpp>
#include <com/sun/star/ui/XUIElement.hpp>

#include <cppuhelper/compbase2.hxx>
#include <cppuhelper/compbase1.hxx>
#include <cppuhelper/basemutex.hxx>

#include <boost/scoped_ptr.hpp>

//......................................................................................................................
namespace sd { namespace colortoolpanel
{
//......................................................................................................................

    //==================================================================================================================
    //= SingleColorPanel
    //==================================================================================================================
    typedef ::cppu::WeakComponentImplHelper2    <   ::com::sun::star::ui::XToolPanel
                                                ,   ::com::sun::star::awt::XPaintListener
                                                >   SingleColorPanel_Base;
    class SingleColorPanel  :public ::cppu::BaseMutex
                            ,public SingleColorPanel_Base
    {
    public:
        SingleColorPanel(
            const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& i_rContext,
            const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow >& i_rParentWindow,
            const ::sal_Int32 i_nPanelColor
        );

        // XToolPanel
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow > SAL_CALL getWindow() throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL createAccessible( const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible >& i_rParentAccessible ) throw (::com::sun::star::uno::RuntimeException);

        // XPaintListener
        virtual void SAL_CALL windowPaint( const ::com::sun::star::awt::PaintEvent& e ) throw (::com::sun::star::uno::RuntimeException);

        // XEventListener
        virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& Source ) throw (::com::sun::star::uno::RuntimeException);

        // XComponent equivalents
        virtual void SAL_CALL disposing();

    protected:
        ~SingleColorPanel();

    private:
        ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow >  m_xWindow;
        const sal_Int32                                                     m_nPanelColor;
    };

    //==================================================================================================================
    //= PanelUIElement
    //==================================================================================================================
    typedef ::cppu::WeakComponentImplHelper1    <   ::com::sun::star::ui::XUIElement
                                                >   PanelUIElement_Base;
    class PanelUIElement    :public ::cppu::BaseMutex
                            ,public PanelUIElement_Base
    {
    public:
        PanelUIElement(
            const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& i_rContext,
            const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow >& i_rParentWindow,
            const ::rtl::OUString& i_rResourceURL,
            const ::sal_Int32 i_nPanelColor
        );

        // XUIElement
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame > SAL_CALL getFrame() throw (::com::sun::star::uno::RuntimeException);
        virtual ::rtl::OUString SAL_CALL getResourceURL() throw (::com::sun::star::uno::RuntimeException);
        virtual ::sal_Int16 SAL_CALL getType() throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL getRealInterface(  ) throw (::com::sun::star::uno::RuntimeException);

        // OComponentHelper
        virtual void SAL_CALL disposing();

    protected:
        ~PanelUIElement();

    private:
        const ::rtl::OUString   m_sResourceURL;
        ::com::sun::star::uno::Reference< ::com::sun::star::ui::XToolPanel >
                                m_xToolPanel;
    };

//......................................................................................................................
} } // namespace sd::colortoolpanel
//......................................................................................................................

#endif // SD_WORKBENCH_CTP_PANEL_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
