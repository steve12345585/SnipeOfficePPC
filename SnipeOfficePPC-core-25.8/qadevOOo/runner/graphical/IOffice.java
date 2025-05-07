/*
 * ************************************************************************
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
 * ***********************************************************************
 */

package graphical;

/**
 *
 * @author ll93751
 */
public interface IOffice
{
    /**
     * start an Office, if need
     * @throws graphical.OfficeException
     */
    public void start() throws OfficeException;

    /**
     * Load a document by it's Name
     * @param Name
     * @throws graphical.OfficeException
     */
    public void load(String Name) throws OfficeException;

    /**
     * Create a postscript file in the DOC_COMPARATOR_OUTPUT_DIR directory from a loaded document
     * @throws graphical.OfficeException
     */
    public void storeAsPostscript() throws OfficeException;


    /**
     * Close the background office
     * @throws graphical.OfficeException
     */
    public void close() throws OfficeException;
}
