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

package org.openoffice.idesupport.filter;

public class ExceptParcelFilter implements FileFilter {
    private static final String DESCRIPTION = "Remove specified Parcel";
    private static final ExceptParcelFilter filter = new ExceptParcelFilter();
    private static String parcelName = null;

    private ExceptParcelFilter() {
    }

    public void setParcelToRemove(String parcelName)
    {
        this.parcelName = parcelName;
    }

    public static ExceptParcelFilter getInstance() {
        return filter;
    }
    public boolean validate(String name) {
        if (name.startsWith(this.parcelName))
            return true;
        return false;
    }

    public String toString() {
        StringBuffer buf = new StringBuffer(DESCRIPTION + ": ");

        buf.append("<" + this.parcelName + ">");

        return buf.toString();
    }
}
