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

package mod._svx;

import java.io.PrintWriter;

import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;
import util.AccessibilityTools;
import util.DrawTools;
import util.SOfficeFactory;
import util.utils;

import com.sun.star.accessibility.AccessibleRole;
import com.sun.star.accessibility.XAccessible;
import com.sun.star.awt.Size;
import com.sun.star.awt.XWindow;
import com.sun.star.beans.XPropertySet;
import com.sun.star.drawing.XShape;
import com.sun.star.frame.XModel;
import com.sun.star.lang.XComponent;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;

public class AccessiblePresentationShape extends TestCase {

    static XComponent xDrawDoc;
    static XModel aModel;

    protected void initialize( TestParameters tParam, PrintWriter log ) {

        SOfficeFactory SOF = SOfficeFactory.getFactory( (XMultiServiceFactory)tParam.getMSF() );

        try {
            log.println( "creating a drawdoc" );
            xDrawDoc = SOF.createImpressDoc(null);
            aModel = UnoRuntime.queryInterface(XModel.class, xDrawDoc);

        } catch ( com.sun.star.uno.Exception e ) {
            // Some exception occures.FAILED
            e.printStackTrace( log );
            throw new StatusException( "Couldn't create document", e );
        }
    }

    /**
     * Disposes the Draw document loaded before.
     */
    protected void cleanup( TestParameters tParam, PrintWriter log ) {
        log.println( "    disposing xDrawDoc " );
        util.DesktopTools.closeDoc(xDrawDoc);
    }

    protected TestEnvironment createTestEnvironment
            (TestParameters tParam, PrintWriter log) {

        XInterface oObj = null;
        XShape oShape = null;

        // creation of testobject here
        // first we write what we are intend to do to log file
        log.println( "creating a test environment" );

        XMultiServiceFactory docMSF = UnoRuntime.queryInterface(XMultiServiceFactory.class, xDrawDoc);
        try {
            oShape = UnoRuntime.queryInterface(XShape.class,
                docMSF.createInstance
                ("com.sun.star.presentation.HandoutShape"));
        } catch (com.sun.star.uno.Exception e) {
            throw new StatusException("couldn't create component", e);
        }


        DrawTools.getShapes(DrawTools.getDrawPage(xDrawDoc,0)).add(oShape);

        XPropertySet shapeProps = UnoRuntime.queryInterface(XPropertySet.class, oShape);

        try {
            shapeProps.setPropertyValue(
                                "IsEmptyPresentationObject", new Boolean(false));
        } catch (Exception e) {}

        AccessibilityTools at = new AccessibilityTools();

        XWindow xWindow = at.getCurrentWindow ((XMultiServiceFactory)tParam.getMSF(),aModel);
        XAccessible xRoot = at.getAccessibleObject(xWindow);

        at.printAccessibleTree(log, xRoot, tParam.getBool(util.PropertyName.DEBUG_IS_ACTIVE));
//        oObj = at.getAccessibleObjectForRole
//            (xRoot, AccessibleRole.SHAPE, "ImpressHandout");
        oObj = at.getAccessibleObjectForRole(xRoot, AccessibleRole.UNKNOWN, "ImpressHandout");

        log.println("Implementation Name: "+utils.getImplName(oObj));

        // create test environment here
        TestEnvironment tEnv = new TestEnvironment( oObj );

        final XShape fShape = oShape ;
        tEnv.addObjRelation("EventProducer",
            new ifc.accessibility._XAccessibleEventBroadcaster.EventProducer() {
                public void fireEvent() {
                    try {
                        Size size = fShape.getSize();
                        size.Width += 100;
                        fShape.setSize(size);
                    } catch(com.sun.star.beans.PropertyVetoException e) {}
                }
            });

        log.println("Implementation Name: " + utils.getImplName(oObj));

        return tEnv;
    } // finish method getTestEnvironment

}

