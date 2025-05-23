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
#ifndef _SVX_EXTRUSION_CONTROLS_HXX
#define _SVX_EXTRUSION_CONTROLS_HXX

#include "svx/svxdllapi.h"

#include <svtools/treelistbox.hxx>
#include <vcl/button.hxx>
#include <vcl/dialog.hxx>
#include <vcl/field.hxx>
#include <vcl/fixed.hxx>

#include <svtools/toolbarmenu.hxx>
#include <svtools/popupwindowcontroller.hxx>
#include <svtools/popupmenucontrollerbase.hxx>

class ValueSet;

//========================================================================

namespace svx
{
class ExtrusionDirectionWindow : public svtools::ToolbarMenu
{
public:
    ExtrusionDirectionWindow( svt::ToolboxController& rController, const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& rFrame, Window* pParentWindow );

    virtual void SAL_CALL statusChanged( const ::com::sun::star::frame::FeatureStateEvent& Event ) throw ( ::com::sun::star::uno::RuntimeException );
    virtual void DataChanged( const DataChangedEvent& rDCEvt );

private:
    svt::ToolboxController& mrController;
    ValueSet* mpDirectionSet;

    Image       maImgDirection[9];
    Image       maImgPerspective;
    Image       maImgParallel;

    const rtl::OUString msExtrusionDirection;
    const rtl::OUString msExtrusionProjection;

    DECL_LINK( SelectHdl, void * );
    void            FillValueSet();

    void implSetDirection( sal_Int32 nSkew, bool bEnabled = true );
    void implSetProjection( sal_Int32 nProjection, bool bEnabled = true );

};

//========================================================================

class ExtrusionDirectionControl : public svt::PopupWindowController
{
public:
    ExtrusionDirectionControl( const com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >& rServiceManager );

    virtual ::Window* createPopupWindow( ::Window* pParent );

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName() throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames() throw( ::com::sun::star::uno::RuntimeException );

    using svt::PopupWindowController::createPopupWindow;
};

//========================================================================

class ExtrusionDepthWindow : public svtools::ToolbarMenu
{
private:
    svt::ToolboxController& mrController;

    Image maImgDepth0;
    Image maImgDepth1;
    Image maImgDepth2;
    Image maImgDepth3;
    Image maImgDepth4;
    Image maImgDepthInfinity;

    FieldUnit   meUnit;
    double      mfDepth;

    const rtl::OUString msExtrusionDepth;
    const rtl::OUString msMetricUnit;

    DECL_LINK( SelectHdl, void * );

    void    implFillStrings( FieldUnit eUnit );
    void    implSetDepth( double fDepth );

public:
    ExtrusionDepthWindow( svt::ToolboxController& rController, const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& rFrame, Window* pParentWindow );

    virtual void SAL_CALL statusChanged( const ::com::sun::star::frame::FeatureStateEvent& Event ) throw ( ::com::sun::star::uno::RuntimeException );
    virtual void DataChanged( const DataChangedEvent& rDCEvt );
};

//========================================================================

class ExtrusionDepthController : public svt::PopupWindowController
{
public:
    ExtrusionDepthController( const com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >& rServiceManager );

    virtual ::Window* createPopupWindow( ::Window* pParent );

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName() throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames() throw( ::com::sun::star::uno::RuntimeException );

    using svt::PopupWindowController::createPopupWindow;
};

//========================================================================

class ExtrusionLightingWindow : public svtools::ToolbarMenu
{
private:
    svt::ToolboxController& mrController;
    ValueSet*    mpLightingSet;

    Image maImgLightingOff[9];
    Image maImgLightingOn[9];
    Image maImgLightingPreview[9];

    Image maImgBright;
    Image maImgNormal;
    Image maImgDim;

    int     mnLevel;
    bool    mbLevelEnabled;
    int     mnDirection;
    bool    mbDirectionEnabled;

    const rtl::OUString msExtrusionLightingDirection;
    const rtl::OUString msExtrusionLightingIntensity;

    void    implSetIntensity( int nLevel, bool bEnabled );
    void    implSetDirection( int nDirection, bool bEnabled );

    DECL_LINK( SelectHdl, void * );
public:
    ExtrusionLightingWindow( svt::ToolboxController& rController, const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& rFrame, Window* pParentWindow );

    virtual void SAL_CALL statusChanged( const ::com::sun::star::frame::FeatureStateEvent& Event ) throw ( ::com::sun::star::uno::RuntimeException );
    virtual void DataChanged( const DataChangedEvent& rDCEvt );
};

//========================================================================

class ExtrusionLightingControl : public svt::PopupWindowController
{
public:
    ExtrusionLightingControl( const com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >& rServiceManager );

    virtual ::Window* createPopupWindow( ::Window* pParent );

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName() throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames() throw( ::com::sun::star::uno::RuntimeException );

    using svt::PopupWindowController::createPopupWindow;
};

//========================================================================

class ExtrusionSurfaceWindow : public svtools::ToolbarMenu
{
private:
    svt::ToolboxController& mrController;

    Image maImgSurface1;
    Image maImgSurface2;
    Image maImgSurface3;
    Image maImgSurface4;

    const rtl::OUString msExtrusionSurface;

    DECL_LINK( SelectHdl, void * );

    void    implSetSurface( int nSurface, bool bEnabled );

public:
    ExtrusionSurfaceWindow( svt::ToolboxController& rController, const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& rFrame, Window* pParentWindow );

    virtual void SAL_CALL statusChanged( const ::com::sun::star::frame::FeatureStateEvent& Event ) throw ( ::com::sun::star::uno::RuntimeException );
};

//========================================================================

class ExtrusionSurfaceControl : public svt::PopupWindowController
{
public:
    ExtrusionSurfaceControl( const com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >& rServiceManager );

    virtual ::Window* createPopupWindow( ::Window* pParent );

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName() throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames() throw( ::com::sun::star::uno::RuntimeException );

    using svt::PopupWindowController::createPopupWindow;
};

//========================================================================

}
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
