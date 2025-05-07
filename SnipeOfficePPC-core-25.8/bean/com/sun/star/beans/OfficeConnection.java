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

package com.sun.star.beans;

import java.awt.Container;

import com.sun.star.lang.XComponent;
import com.sun.star.uno.XComponentContext;

/**
 * This abstract class reprecents a connection to the office
 * application.
 * @deprecated
 */
public interface OfficeConnection
    extends XComponent
{
    /**
     * Sets a connection URL.
     *
     * @param url This is UNO URL which describes the type of a connection.
     */
    void setUnoUrl(String url)
        throws java.net.MalformedURLException;

    /**
     * Sets an AWT container catory.
     *
     * @param containerFactory This is a application provided AWT container
     *  factory.
     */
    void setContainerFactory(ContainerFactory containerFactory);

    /**
     * Retrives the UNO component context.
     * Establishes a connection if necessary and initialises the
     * UNO service manager if it has not already been initialised.
     *
     * @return The office UNO component context.
     */
    XComponentContext getComponentContext();

    /**
     * Creates an office window.
     * The window is either a sub-class of java.awt.Canvas (local) or
     * java.awt.Container (RVP).
     *
     * This method does not add add the office window to its container.
     *
     * @param container This is an AWT container.
     * @return The office window instance.
     */
    OfficeWindow createOfficeWindow(Container container);
}
