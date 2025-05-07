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

package ifc.sdb;

import com.sun.star.container.XIndexAccess;
import com.sun.star.sdb.XParametersSupplier;
import lib.MultiMethodTest;

/**
* Testing <code>com.sun.star.sdb.XParametersSupplier</code>
* interface methods :
* <ul>
*  <li><code> getParameters()</code></li>
* </ul> <p>
* Test is multithread compilant. <p>
* @see com.sun.star.sdb.XParametersSupplier
*/
public class _XParametersSupplier extends MultiMethodTest {

    // oObj filled by MultiMethodTest
    public XParametersSupplier oObj = null ;

    /**
    * checks of the return of <code>getParameters()</code>
    * is not null
    */
    public void _getParameters() {

        XIndexAccess the_Set = oObj.getParameters();
        if (the_Set == null) log.println("'getParameters()' returns NULL");
        tRes.tested("getParameters()",the_Set != null);

    }
}  // finish class _XParametersSupplier


