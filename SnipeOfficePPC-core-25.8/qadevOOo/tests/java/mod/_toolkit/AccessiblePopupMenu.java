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
package mod._toolkit;

import com.sun.star.accessibility.AccessibleRole;
import com.sun.star.accessibility.XAccessible;
import com.sun.star.accessibility.XAccessibleComponent;
import com.sun.star.accessibility.XAccessibleSelection;
import com.sun.star.awt.Point;
import com.sun.star.awt.Rectangle;
import com.sun.star.awt.XExtendedToolkit;
import com.sun.star.awt.XWindow;
import com.sun.star.frame.XDesktop;
import com.sun.star.frame.XModel;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.text.XTextDocument;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;

import java.awt.Robot;
import java.awt.event.InputEvent;

import java.io.PrintWriter;

import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;

import util.AccessibilityTools;
import util.DesktopTools;
import util.SOfficeFactory;


public class AccessiblePopupMenu extends TestCase {
    private static XDesktop the_Desk;
    private static XTextDocument xTextDoc;
    private static Point point;

    /**
     * Creates the Desktop service (<code>com.sun.star.frame.Desktop</code>).
     */
    protected void initialize(TestParameters Param, PrintWriter log) {
        the_Desk = (XDesktop) UnoRuntime.queryInterface(XDesktop.class,
                                                        DesktopTools.createDesktop(
                                                                (XMultiServiceFactory) Param.getMSF()));
    }

    /**
     * Disposes the document, if exists, created in
     * <code>createTestEnvironment</code> method.
     */
    protected void cleanup(TestParameters Param, PrintWriter log) {

        log.println("release the popup menu");
        try {
            Robot rob = new Robot();
            int x = point.X;
            int y = point.Y;
            rob.mouseMove(x, y);
            rob.mousePress(InputEvent.BUTTON1_MASK);
            rob.mouseRelease(InputEvent.BUTTON1_MASK);
        } catch (java.awt.AWTException e) {
            log.println("couldn't press mouse button");
        }

        log.println("disposing xTextDoc");

        if (xTextDoc != null) {
            closeDoc();
        }
    }

    /**
     * Creates a text document.
     * Then obtains an accessible object with
     * the role <code>AccessibleRole.PUSHBUTTON</code> and with the name
     * <code>"Bold"</code>.
     * Object relations created :
     * <ul>
     *  <li> <code>'EventProducer'</code> for
     *      {@link ifc.accessibility._XAccessibleEventBroadcaster}</li>
     *  <li> <code>'XAccessibleText.Text'</code> for
     *      {@link ifc.accessibility._XAccessibleText}: the name of button</li>
     * </ul>
     *
     * @param tParam test parameters
     * @param log writer to log information while testing
     *
     * @see com.sun.star.awt.Toolkit
     * @see com.sun.star.accessibility.AccessibleRole
     * @see ifc.accessibility._XAccessibleEventBroadcaster
     * @see ifc.accessibility._XAccessibleText
     * @see com.sun.star.accessibility.XAccessibleEventBroadcaster
     * @see com.sun.star.accessibility.XAccessibleText
     */
    protected TestEnvironment createTestEnvironment(TestParameters tParam,
                                                    PrintWriter log) {
        log.println("creating a test environment");

        if (xTextDoc != null) {
            closeDoc();
        }

        // get a soffice factory object
        SOfficeFactory SOF = SOfficeFactory.getFactory(
                                     (XMultiServiceFactory) tParam.getMSF());

        XInterface toolkit = null;

        try {
            log.println("creating a text document");
            xTextDoc = SOF.createTextDoc(null);
            toolkit = (XInterface) ((XMultiServiceFactory) tParam.getMSF()).createInstance(
                              "com.sun.star.awt.Toolkit");
        } catch (com.sun.star.uno.Exception e) {
            // Some exception occures.FAILED
            e.printStackTrace(log);
            throw new StatusException("Couldn't create document", e);
        }

        shortWait(tParam);

        XModel aModel = (XModel) UnoRuntime.queryInterface(XModel.class,
                                                           xTextDoc);

        XInterface oObj = null;

        AccessibilityTools at = new AccessibilityTools();

        XWindow xWindow = at.getCurrentWindow(
                                  (XMultiServiceFactory) tParam.getMSF(),
                                  aModel);

        XAccessible xRoot = at.getAccessibleObject(xWindow);

        oObj = at.getAccessibleObjectForRole(xRoot, AccessibleRole.PANEL);

        XAccessibleComponent window = (XAccessibleComponent) UnoRuntime.queryInterface(
                                              XAccessibleComponent.class, oObj);

        point = window.getLocationOnScreen();
        Rectangle rect = window.getBounds();

        try {
            Robot rob = new Robot();
            int x = point.X + (rect.Width / 2);
            int y = point.Y + (rect.Height / 2);
            rob.mouseMove(x, y);
            rob.mousePress(InputEvent.BUTTON3_MASK);
            rob.mouseRelease(InputEvent.BUTTON3_MASK);
        } catch (java.awt.AWTException e) {
            log.println("couldn't press mouse button");
        }

        shortWait(tParam);

        XExtendedToolkit tk = (XExtendedToolkit) UnoRuntime.queryInterface(
                                      XExtendedToolkit.class, toolkit);

        try {
            xWindow = (XWindow) UnoRuntime.queryInterface(XWindow.class,
                                                          tk.getTopWindow(0));

            xRoot = at.getAccessibleObject(xWindow);

        at.printAccessibleTree(log, xRoot, tParam.getBool(util.PropertyName.DEBUG_IS_ACTIVE));
        } catch (com.sun.star.lang.IndexOutOfBoundsException e) {
            log.println("Couldn't get Window");
        }

        oObj = at.getAccessibleObjectForRole(xRoot, AccessibleRole.POPUP_MENU);

        log.println("ImplementationName: " + util.utils.getImplName(oObj));

        TestEnvironment tEnv = new TestEnvironment(oObj);

        tEnv.addObjRelation("XAccessibleSelection.multiSelection",
                            new Boolean(false));

        final XAccessibleSelection sel = (XAccessibleSelection) UnoRuntime.queryInterface(
                                                 XAccessibleSelection.class,
                                                 oObj);

        tEnv.addObjRelation("EventProducer",
                            new ifc.accessibility._XAccessibleEventBroadcaster.EventProducer() {
            public void fireEvent() {
                try {
                    sel.selectAccessibleChild(2);
                } catch (com.sun.star.lang.IndexOutOfBoundsException e) {
                    System.out.println("Couldn't fire event");
                }
            }
        });

        return tEnv;
    }

    protected void closeDoc() {
        util.DesktopTools.closeDoc(xTextDoc);
    }

    private void shortWait(TestParameters tParam) {
        util.utils.shortWait(tParam.getInt(util.PropertyName.SHORT_WAIT));
    }
}
