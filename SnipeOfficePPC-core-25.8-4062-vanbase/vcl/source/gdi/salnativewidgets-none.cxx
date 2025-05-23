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


#include <salgdi.hxx>

using ::rtl::OUString;

/****************************************************************
 *  Placeholder for no native widgets
 ***************************************************************/


/*
 * IsNativeControlSupported()
 *
 *  Returns sal_True if the platform supports native
 *  drawing of the control defined by nPart
 */
sal_Bool SalGraphics::IsNativeControlSupported( ControlType, ControlPart )
{
    return( sal_False );
}


/*
 * HitTestNativeControl()
 *
 *  If the return value is sal_True, bIsInside contains information whether
 *  aPos was or was not inside the native widget specified by the
 *  nType/nPart combination.
 */
sal_Bool SalGraphics::hitTestNativeControl( ControlType,
                              ControlPart,
                              const Rectangle&,
                              const Point&,
                              sal_Bool& )
{
    return( sal_False );
}


/*
 * DrawNativeControl()
 *
 *  Draws the requested control described by nPart/nState.
 *
 *  rControlRegion: The bounding region of the complete control in VCL frame coordinates.
 *  aValue:         An optional value (tristate/numerical/string)
 *  aCaption:   A caption or title string (like button text etc)
 */
sal_Bool SalGraphics::drawNativeControl(    ControlType,
                            ControlPart,
                            const Rectangle&,
                            ControlState,
                            const ImplControlValue&,
                            const OUString& )
{
    return( sal_False );
}

/*
 * GetNativeControlRegion()
 *
 *  If the return value is sal_True, rNativeBoundingRegion
 *  contains the sal_True bounding region covered by the control
 *  including any adornment, while rNativeContentRegion contains the area
 *  within the control that can be safely drawn into without drawing over
 *  the borders of the control.
 *
 *  rControlRegion: The bounding region of the control in VCL frame coordinates.
 *  aValue:     An optional value (tristate/numerical/string)
 *  aCaption:       A caption or title string (like button text etc)
 */
sal_Bool SalGraphics::getNativeControlRegion(  ControlType,
                                ControlPart,
                                const Rectangle&,
                                ControlState,
                                const ImplControlValue&,
                                const OUString&,
                                Rectangle &,
                                Rectangle & )
{
    return( sal_False );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
