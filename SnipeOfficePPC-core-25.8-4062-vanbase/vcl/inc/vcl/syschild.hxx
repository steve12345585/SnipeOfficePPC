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

#ifndef _SV_SYSCHILD_HXX
#define _SV_SYSCHILD_HXX

#include <tools/solar.h>
#include <vcl/dllapi.h>
#include <vcl/window.hxx>

struct SystemEnvData;
struct SystemWindowData;

// ---------------------
// - SystemChildWindow -
// ---------------------

class VCL_DLLPUBLIC SystemChildWindow : public Window
{
private:
    using Window::ImplInit;
    SAL_DLLPRIVATE void     ImplInitSysChild( Window* pParent, WinBits nStyle, SystemWindowData *pData, sal_Bool bShow = sal_False );
    SAL_DLLPRIVATE void     ImplTestJavaException( void* pEnv );

    // Copy assignment is forbidden and not implemented.
    SAL_DLLPRIVATE          SystemChildWindow (const SystemChildWindow &);
    SAL_DLLPRIVATE          SystemChildWindow & operator= (const SystemChildWindow &);

public:
                            SystemChildWindow( Window* pParent, WinBits nStyle = 0 );
                            // create a SystemChildWindow using the given SystemWindowData
                            SystemChildWindow( Window* pParent, WinBits nStyle, SystemWindowData *pData, sal_Bool bShow = sal_True );
                            ~SystemChildWindow();

    const SystemEnvData*    GetSystemData() const;

    //  per default systemchildwindows erase their background for better plugin support
    //  however, this might not always be required
    void                    EnableEraseBackground( sal_Bool bEnable = sal_True );
    void                    SetForwardKey( sal_Bool bEnable );
    // return the platform specific handle/id of this window;
    // in case the flag bUseJava is set, a java compatible overlay window
    // is created on which other java windows can be created (plugin interface)
    sal_IntPtr              GetParentWindowHandle( sal_Bool bUseJava = sal_False );
};

#endif // _SV_SYSCHILD_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
