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

package ifc.style;

import ifc.style._PageProperties;

/**
 * Test page style properties.
 * These properties are a subset of the PageProperties. So just use the
 * PageProperties test
 */
public class _PageStyle extends _PageProperties {

    /**
     * This property is system dependent and printer dependent.
     * Test is used from _PageProperties
     * @see ifc.style._PageProperties
     */
    public void _PrinterPaperTray() {
        super._PrinterPaperTray();
    }

    /**
     * Create some valid user defined attributes
     * Test is used from _PageProperties
     * @see ifc.style._PageProperties
     */
    public void _UserDefinedAttributes() {
        super._UserDefinedAttributes();
    }
}
