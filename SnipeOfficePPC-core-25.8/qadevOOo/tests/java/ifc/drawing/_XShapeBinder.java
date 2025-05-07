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

package ifc.drawing;

import lib.MultiMethodTest;

import com.sun.star.drawing.XDrawPage;
import com.sun.star.drawing.XShape;
import com.sun.star.drawing.XShapeBinder;
import com.sun.star.drawing.XShapes;
import com.sun.star.uno.UnoRuntime;

/**
* Testing <code>com.sun.star.drawing.XShapeBinder</code>
* interface methods :
* <ul>
*  <li><code> bind()</code></li>
*  <li><code> unbind()</code></li>
* </ul> <p>
* This test needs the following object relations :
* <ul>
*  <li> <code>'DrawPage'</code> (must implement <code>XShapes</code>):
*   the collection of shapes in a document which used to create a group.</li>
* <ul> <p>
* Test is <b> NOT </b> multithread compilant. <p>
* @see com.sun.star.drawing.XShapeBinder
*/
public class _XShapeBinder extends MultiMethodTest {

    public XShapeBinder oObj = null;
    XShape group = null;
    int countBeforeBind = 0;
    XShapes oShapes = null;

    /**
    * Retrieves draw page collection from relation and binds them. <p>
    *
    * Has <b> OK </b> status if the shape group returned is not null
    * number of shapes in collection is 1 (shapes are binded into a single
    * shape). <p>
    */
    public void _bind () {
        XDrawPage dp = (XDrawPage) tEnv.getObjRelation("DrawPage");
        oShapes = (XShapes)UnoRuntime.queryInterface( XShapes.class, dp );
        boolean result = false;
        log.println("testing bind() ... ");
        countBeforeBind = oShapes.getCount();
        log.println("Count before bind:" + countBeforeBind);
        group = oObj.bind(oShapes);
        int countAfterBind = oShapes.getCount();
        log.println("Count after bind:" + countAfterBind);
        result = group != null &&  countAfterBind == 1;

        tRes.tested("bind()", result);

    }

    /**
    * Unbinds the group created before. <p>
    *
    * Has <b> OK </b> status if number of shapes in collection
    * increases after the method call. <p>
    *
    * The following method tests are to be completed successfully before :
    * <ul>
    *  <li> <code> bind() </code> : to create a shape group </li>
    * </ul>
    */
    public void _unbind () {
        requiredMethod("bind()");
        boolean result = false;

        // get the current thread's holder
        log.println("unbinding the shape...");

        oObj.unbind(group);
        int countAfterUnbind = oShapes.getCount();
        log.println("Count after unbind:" + countAfterUnbind);
        result = countAfterUnbind >= countBeforeBind;

        tRes.tested("unbind()", result);
    }
}

