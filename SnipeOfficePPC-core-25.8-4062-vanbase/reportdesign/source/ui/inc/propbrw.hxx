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

#ifndef _REPORT_PROPBRW_HXX
#define _REPORT_PROPBRW_HXX

#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/awt/XControlContainer.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/inspection/XObjectInspector.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <svl/brdcst.hxx>
#include <svl/lstner.hxx>
#include <vcl/dockwin.hxx>
#include <svx/svdmark.hxx>
#include "ModuleHelper.hxx"

namespace rptui
{

class OSectionView;
class ODesignView;
class OObjectBase;
//============================================================================
// PropBrw
//============================================================================

class PropBrw : public DockingWindow , public SfxListener, public SfxBroadcaster
{
private:
    OModuleClient       m_aModuleClient;
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >
                        m_xInspectorContext;
    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >
                        m_xORB;
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >
                        m_xMeAsFrame;
    ::com::sun::star::uno::Reference< ::com::sun::star::inspection::XObjectInspector >
                        m_xBrowserController;
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow >
                        m_xBrowserComponentWindow;
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface>
                        m_xLastSection; /// is the previously displayed section
    ::rtl::OUString     m_sLastActivePage;
    ODesignView*        m_pDesignView;
    OSectionView*       m_pView;
    sal_Bool            m_bInitialStateChange;

    PropBrw(PropBrw&);
    void operator =(PropBrw&);
protected:

    virtual void Resize();
    virtual sal_Bool Close();

    ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface> >
        CreateCompPropSet(const SdrMarkList& rMarkList);

    void implSetNewObject(
        const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface> >& _aObjects = ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface> >());

    ::rtl::OUString GetHeadlineName(
        const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface> >& _aObjects);

    void implDetachController();
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface> CreateComponentPair(OObjectBase* _pObj);
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface> CreateComponentPair(
             const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface>& _xFormComponent
            ,const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface>& _xReportComponent);
    DECL_LINK( OnAsyncGetFocus, void* );

public:
    PropBrw(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _xORB
            ,Window* pParent
            ,ODesignView*  _pDesignView);
    virtual ~PropBrw();

    virtual void LoseFocus();

    void    Update( OSectionView* m_pView );
    void    Update( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface>& _xReportComponent);
    inline OSectionView*    GetCurView() const { return m_pView; }
    ::rtl::OUString         getCurrentPage() const;
    void                    setCurrentPage(const ::rtl::OUString& _sLastActivePage);

    ::Size getMinimumSize() const;
private:
    using Window::Update;
};
//==============================================================================
} // rptui
//==============================================================================
#endif // _REPORT_PROPBRW_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
