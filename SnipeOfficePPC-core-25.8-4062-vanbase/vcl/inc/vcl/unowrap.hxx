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

#ifndef _VCL_UNOWRAP_HXX
#define _VCL_UNOWRAP_HXX

#include <tools/solar.h>
#include <vcl/dllapi.h>
#include <com/sun/star/uno/Reference.h>

class XWindowPeer;
class XToolkit;
class EventList;
class Window;
class OutputDevice;
class MouseEvent;
class CommandEvent;
class KeyEvent;
class Rectangle;
class Menu;

namespace com {
namespace sun {
namespace star {
namespace awt {
    class XGraphics;
    class XToolkit;
    class XWindowPeer;
}
namespace lang {
    class XMultiServiceFactory;
}
} } }

namespace com {
namespace sun {
namespace star {
namespace accessibility {
    class XAccessible;
}}}}

class VCL_DLLPUBLIC UnoWrapperBase
{
public:
    virtual void                Destroy() = 0;

    // Toolkit
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::awt::XToolkit > GetVCLToolkit() = 0;

    // Graphics
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::awt::XGraphics >    CreateGraphics( OutputDevice* pOutDev ) = 0;
    virtual void                ReleaseAllGraphics( OutputDevice* pOutDev ) = 0;

    // Window
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindowPeer> GetWindowInterface( Window* pWindow, sal_Bool bCreate ) = 0;
    virtual void                SetWindowInterface( Window* pWindow, ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindowPeer > xIFace ) = 0;

    virtual void                WindowDestroyed( Window* pWindow ) = 0;

    // Accessibility
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible >
                                CreateAccessible( Menu* pMenu, sal_Bool bIsMenuBar ) = 0;

protected:
    ~UnoWrapperBase() {}
};

#endif  // _VCL_UNOWRAP_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
