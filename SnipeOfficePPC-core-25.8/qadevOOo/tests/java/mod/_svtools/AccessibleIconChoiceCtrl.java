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
package mod._svtools;

import java.io.PrintWriter;

import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;
import util.AccessibilityTools;
import util.DesktopTools;
import util.SOfficeFactory;

import com.sun.star.accessibility.AccessibleRole;
import com.sun.star.accessibility.XAccessible;
import com.sun.star.accessibility.XAccessibleAction;
import com.sun.star.accessibility.XAccessibleContext;
import com.sun.star.accessibility.XAccessibleSelection;
import com.sun.star.awt.XExtendedToolkit;
import com.sun.star.awt.XWindow;
import com.sun.star.beans.PropertyValue;
import com.sun.star.frame.XController;
import com.sun.star.frame.XDesktop;
import com.sun.star.frame.XDispatch;
import com.sun.star.frame.XDispatchProvider;
import com.sun.star.frame.XModel;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.text.XTextDocument;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;
import com.sun.star.util.URL;
import com.sun.star.util.XURLTransformer;


/**
 * Test for object that implements the following interfaces :
 * <ul>
 *  <li><code>
 *  ::com::sun::star::accessibility::XAccessibleContext</code></li>
 *  <li><code>
 *  ::com::sun::star::accessibility::XAccessibleEventBroadcaster
 *  </code></li>
 *  <li><code>
 *  ::com::sun::star::accessibility::XAccessibleComponent</code></li>
 *  <li><code>
 *  ::com::sun::star::accessibility::XAccessibleSelection</code></li>
 * </ul> <p>
 *
 * @see com.sun.star.accessibility.XAccessibleEventBroadcaster
 * @see com.sun.star.accessibility.XAccessibleContext
 * @see com.sun.star.accessibility.XAccessibleComponent
 * @see com.sun.star.accessibility.XAccessibleSelection
 * @see ifc.accessibility._XAccessibleEventBroadcaster
 * @see ifc.accessibility._XAccessibleContext
 * @see ifc.accessibility._XAccessibleComponent
 * @see ifc.accessibility._XAccessibleSelection
 */
public class AccessibleIconChoiceCtrl extends TestCase {
    static XDesktop the_Desk;
    static XTextDocument xTextDoc;
    static XAccessibleAction accCloseButton = null;

    /**
     * Creates the Desktop service (<code>com.sun.star.frame.Desktop</code>).
     */
    protected void initialize(TestParameters Param, PrintWriter log) {
        the_Desk = (XDesktop) UnoRuntime.queryInterface(XDesktop.class,
                                                        DesktopTools.createDesktop(
        (XMultiServiceFactory) Param.getMSF()));
    }

    /**
     * Closes the Hyperlink dialog.
     * Disposes the document, if exists, created in
     * <code>createTestEnvironment</code> method.
     */
    protected void cleanup(TestParameters Param, PrintWriter log) {
        log.println("closing HyperlinkDialog");

        try {
            if (accCloseButton != null) {
                accCloseButton.doAccessibleAction(0);
            }
        } catch (com.sun.star.lang.IndexOutOfBoundsException e) {
            e.printStackTrace(log);
        }

        log.println("disposing xTextDoc");

        if (xTextDoc != null) {
            xTextDoc.dispose();
        }
    }

    /**
     * Creates a text document. Opens the Hyperlink dialog.
     * Creates an instance of the service
     * <code>com.sun.star.awt.Toolkit</code> and gets active top window.
     * Then obtains an accessible object with the role
     * <code>AccessibleRole.TREE</code>.
     * Object relations created :
     * <ul>
     *  <li> <code>'EventProducer'</code> for
     *      {@link ifc.accessibility._XAccessibleEventBroadcaster}:
     *      method <code>fireEvent()</code> selects accessible children
     *   </li>
     * </ul>
     *
     * @param tParam test parameters
     * @param log writer to log information while testing
     *
     * @see com.sun.star.awt.Toolkit
     * @see com.sun.star.accessibility.AccessibleRole
     * @see ifc.accessibility._XAccessibleEventBroadcaster
     * @see com.sun.star.accessibility.XAccessibleEventBroadcaster
     */
    protected TestEnvironment createTestEnvironment(TestParameters tParam,
                                                    PrintWriter log) {
        log.println("creating a test environment");

        if (xTextDoc != null) {
            xTextDoc.dispose();
        }

        // get a soffice factory object
        SOfficeFactory SOF = SOfficeFactory.getFactory( (XMultiServiceFactory) tParam.getMSF());

        try {
            log.println("creating a text document");
            xTextDoc = SOF.createTextDoc(null);
        } catch (com.sun.star.uno.Exception e) {
            // Some exception occures.FAILED
            e.printStackTrace(log);
            throw new StatusException("Couldn't create document", e);
        }

        shortWait();

        XModel aModel1 = (XModel) UnoRuntime.queryInterface(XModel.class,
                                                            xTextDoc);

        XController secondController = aModel1.getCurrentController();

        XDispatchProvider aProv = (XDispatchProvider) UnoRuntime.queryInterface(
                                          XDispatchProvider.class,
                                          secondController);

        XURLTransformer urlTransf = null;

        try {
            XInterface transf = (XInterface) ( (XMultiServiceFactory) tParam.getMSF())
                                                   .createInstance("com.sun.star.util.URLTransformer");
            urlTransf = (XURLTransformer) UnoRuntime.queryInterface(
                                XURLTransformer.class, transf);
        } catch (com.sun.star.uno.Exception e) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't create URLTransformer", e);
        }

        XDispatch getting = null;
        log.println("opening HyperlinkDialog");

        URL[] url = new URL[1];
        url[0] = new URL();
        url[0].Complete = ".uno:HyperlinkDialog";
        urlTransf.parseStrict(url);
        getting = aProv.queryDispatch(url[0], "", 0);

        PropertyValue[] noArgs = new PropertyValue[0];
        getting.dispatch(url[0], noArgs);

        shortWait();

        XInterface oObj = null;

        try {
            oObj = (XInterface) ( (XMultiServiceFactory) tParam.getMSF())
                                      .createInstance("com.sun.star.awt.Toolkit");
        } catch (com.sun.star.uno.Exception e) {
            log.println("Couldn't get toolkit");
            e.printStackTrace(log);
            throw new StatusException("Couldn't get toolkit", e);
        }

        XExtendedToolkit tk = (XExtendedToolkit) UnoRuntime.queryInterface(
                                      XExtendedToolkit.class, oObj);

        AccessibilityTools at = new AccessibilityTools();

        shortWait();

        XWindow xWindow = (XWindow) UnoRuntime.queryInterface(XWindow.class,
                                                              tk.getActiveTopWindow());

        XAccessible xRoot = at.getAccessibleObject(xWindow);

        oObj = at.getAccessibleObjectForRole(xRoot, AccessibleRole.TREE);

        XAccessibleContext closeButton = at.getAccessibleObjectForRole(xRoot,
                                                                       AccessibleRole.PUSH_BUTTON,
                                                                       "Close");

        accCloseButton = (XAccessibleAction) UnoRuntime.queryInterface(
                                 XAccessibleAction.class, closeButton);

        log.println("ImplementationName: " + util.utils.getImplName(oObj));

        TestEnvironment tEnv = new TestEnvironment(oObj);
        tEnv.addObjRelation("XAccessibleSelection.multiSelection",
                            new Boolean(false));
        tEnv.addObjRelation("XAccessibleSelection.OneAlwaysSelected",
                            new Boolean(true));

        String[] expectedStateNames = new String[] {
            "com.sun.star.accessibility.AccessibleStateType.ENABLED",
            "com.sun.star.accessibility.AccessibleStateType.FOCUSABLE",
            "com.sun.star.accessibility.AccessibleStateType.SHOWING",
            "com.sun.star.accessibility.AccessibleStateType.VISIBLE",
            "com.sun.star.accessibility.AccessibleStateType.MANAGES_DESCENDANTS",
        };

        short[] expectedStates = new short[] {
            com.sun.star.accessibility.AccessibleStateType.ENABLED,
            com.sun.star.accessibility.AccessibleStateType.FOCUSABLE,
            com.sun.star.accessibility.AccessibleStateType.SHOWING,
            com.sun.star.accessibility.AccessibleStateType.VISIBLE,
            com.sun.star.accessibility.AccessibleStateType.MANAGES_DESCENDANTS,
        };

        tEnv.addObjRelation("expectedStateNames", expectedStateNames);
        tEnv.addObjRelation("expectedStates", expectedStates);

        final XAccessibleSelection selection = (XAccessibleSelection) (XAccessibleSelection) UnoRuntime.queryInterface(
                                                                              XAccessibleSelection.class,
                                                                              oObj);

        tEnv.addObjRelation("EventProducer",
                            new ifc.accessibility._XAccessibleEventBroadcaster.EventProducer() {
            public void fireEvent() {
                try {
                    selection.selectAccessibleChild(1);
                    selection.selectAccessibleChild(0);
                } catch (com.sun.star.lang.IndexOutOfBoundsException e) {
                }
            }
        });

        return tEnv;
    }

    /**
    * Sleeps for 0.5 sec. to allow StarOffice to react on <code>
    * reset</code> call.
    */
    private void shortWait() {
        try {
            Thread.sleep(5000);
        } catch (InterruptedException e) {
            System.out.println("While waiting :" + e);
        }
    }
}