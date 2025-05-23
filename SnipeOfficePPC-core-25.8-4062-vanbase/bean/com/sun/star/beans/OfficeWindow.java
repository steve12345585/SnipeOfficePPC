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

package com.sun.star.beans;

import java.awt.Component;

import com.sun.star.awt.XWindowPeer;

/**
 * The concreate implementation of the OfficeWindow extends an
 * approperate type of visual component (java.awt.Canvas for local
 * and java.awt.Container for remote).
 * @deprecated
 */
public interface OfficeWindow
{
    /**
     * Retrives an AWT component object associated with the OfficeWindow.
     *
     * @return The AWT component object associated with the OfficeWindow.
     */
    Component getAWTComponent();

    /**
     * Retrives an UNO XWindowPeer object associated with the OfficeWindow.
     *
     * @return The UNO XWindowPeer object associated with the OfficeWindow.
     */
    XWindowPeer getUNOWindowPeer();
}
