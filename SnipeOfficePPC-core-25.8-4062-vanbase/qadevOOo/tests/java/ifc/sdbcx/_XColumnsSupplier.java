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

package ifc.sdbcx;

import lib.MultiMethodTest;

import com.sun.star.container.XNameAccess;
import com.sun.star.sdbcx.XColumnsSupplier;

/**
* Testing <code>com.sun.star.sdbcx.XColumnsSupplier</code>
* interface methods :
* <ul>
*  <li><code> getColumns()</code></li>
* </ul> <p>
* @see com.sun.star.sdbcx.XColumnsSupplier
*/
public class _XColumnsSupplier extends MultiMethodTest {

    // oObj filled by MultiMethodTest
    public XColumnsSupplier oObj = null ;

    /**
    * Has OK status if method returns not null
    * <code>XNameAccess</code> object, FAILED otherwise.
    */
    public void _getColumns() {

        XNameAccess cols = oObj.getColumns() ;

        /*
        String[] colNames = cols.getElementNames() ;

        log.println("Column names:") ;
        for (int i =0 ; i < colNames.length; i++) {
            log.println("  " + colNames[i]) ;
        }

        XServiceInfo info = null ;
        try {
            info = (XServiceInfo) UnoRuntime.queryInterface
                (XServiceInfo.class, cols.getByName("colNames[0]")) ;
        } catch (com.sun.star.uno.Exception e) {}

        log.println("Support : " +
            info.supportsService("com.sun.star.sdbcx.Column")) ;
        */

        tRes.tested("getColumns()", cols != null) ;
    }

}  // finish class _XColumnsSupplier


