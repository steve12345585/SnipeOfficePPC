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

package ifc.container;

import lib.MultiMethodTest;
import lib.Status;

import com.sun.star.container.XChild;
import com.sun.star.container.XNamed;
import com.sun.star.uno.UnoRuntime;

/*
* Testing <code>com.sun.star.container.XChild</code>
* interface methods :
* <ul>
*  <li><code> getParent()</code></li>
*  <li><code> setParent()</code></li>
* </ul>
* @see com.sun.star.container.XChild
*/
public class _XChild extends MultiMethodTest {

    public XChild oObj = null;
    public Object gotten = null;


    /**
    * Test calls the method and checks return value and that
    * no exceptions were thrown. Parent returned is stored.<p>
    * Has <b> OK </b> status if the method returns not null value
    * and no exceptions were thrown. <p>
    */
    public void _getParent() {
        gotten = oObj.getParent();
        XNamed the_name = (XNamed) UnoRuntime.queryInterface(XNamed.class,gotten);
        if (the_name != null)
            log.println("Parent:"+the_name.getName());
        tRes.tested("getParent()",gotten != null);
    }

    /**
    * Sets existing parent and checks that
    * no exceptions were thrown. <p>
    * Has <b> OK </b> status if no exceptions were thrown. <p>
    * The following method tests are to be completed successfully before :
    * <ul>
    *  <li> <code> getParent() </code> : to get the parent. </li>
    * </ul>
    */
    public void _setParent() {
        requiredMethod("getParent()") ;

        String parentComment = (String) tEnv.getObjRelation("cannotSwitchParent");

        if (parentComment != null) {
            log.println(parentComment);
            tRes.tested("setParent()",Status.skipped(true));
            return;
        }

        try {
            oObj.setParent(gotten);
            tRes.tested("setParent()",true);
        }
        catch (com.sun.star.lang.NoSupportException ex) {
            log.println("Exception occurred during setParent()");
            ex.printStackTrace(log);
            tRes.tested("setParent()",false);
        }

    }

}  // finish class _XChild


