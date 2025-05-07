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

package mod._sc;

import java.io.PrintWriter;

import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;
import util.AccessibilityTools;
import util.SOfficeFactory;
import util.utils;

import com.sun.star.accessibility.AccessibleRole;
import com.sun.star.accessibility.XAccessible;
import com.sun.star.accessibility.XAccessibleAction;
import com.sun.star.accessibility.XAccessibleContext;
import com.sun.star.awt.XExtendedToolkit;
import com.sun.star.awt.XWindow;
import com.sun.star.beans.PropertyValue;
import com.sun.star.lang.XComponent;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;

public class ScAccessibleCsvCell extends TestCase {

    Thread lThread = null;
    static XAccessibleAction accAction = null;

    /**
     * Called to create an instance of <code>TestEnvironment</code> with an
     * object to test and related objects. Subclasses should implement this
     * method to provide the implementation and related objects. The method is
     * called from <code>getTestEnvironment()</code>.
     *
     * @param tParam test parameters
     * @param log writer to log information while testing
     *
     * @see TestEnvironment
     * @see #getTestEnvironment()
     */
    protected TestEnvironment createTestEnvironment(TestParameters tParam, PrintWriter log) {
        XInterface oObj = null;

        shortWait();

        try {
            oObj = (XInterface) ((XMultiServiceFactory)tParam.getMSF()).createInstance
                ("com.sun.star.awt.Toolkit") ;
        } catch (com.sun.star.uno.Exception e) {
            log.println("Couldn't get toolkit");
            e.printStackTrace(log);
            throw new StatusException("Couldn't get toolkit", e );
        }


        XExtendedToolkit tk = (XExtendedToolkit)
                        UnoRuntime.queryInterface(XExtendedToolkit.class,oObj);

        XWindow xWindow = (XWindow)
                UnoRuntime.queryInterface(XWindow.class,tk.getActiveTopWindow());

        XAccessible xRoot = AccessibilityTools.getAccessibleObject(xWindow);

        AccessibilityTools.printAccessibleTree(log, xRoot, tParam.getBool(util.PropertyName.DEBUG_IS_ACTIVE));
        oObj = AccessibilityTools.getAccessibleObjectForRole
            (xRoot, AccessibleRole.PUSH_BUTTON, "Cancel");

        accAction = (XAccessibleAction) UnoRuntime.queryInterface(XAccessibleAction.class, oObj);

        oObj = AccessibilityTools.getAccessibleObjectForRole
            (xRoot, AccessibleRole.TABLE, true);

        //util.dbg.printInterfaces(oObj);

        XAccessibleContext cont = (XAccessibleContext)
                UnoRuntime.queryInterface(XAccessibleContext.class, oObj);

        String name = "";
        try {
            XAccessible acc = cont.getAccessibleChild(3);
            name = acc.getAccessibleContext().getAccessibleName();
            log.println("Child: "+ name);
            log.println("ImplementationName " + utils.getImplName(acc));
            oObj = acc;
        }
        catch(com.sun.star.lang.IndexOutOfBoundsException e) {}

        TestEnvironment tEnv = new TestEnvironment(oObj);

        tEnv.addObjRelation("EditOnly",
                    "This method isn't supported in this dialog");

        tEnv.addObjRelation("XAccessibleText.Text", name);

        return tEnv;
    }

    /**
    * Called while disposing a <code>TestEnvironment</code>.
    * Disposes calc document.
    * @param tParam test parameters
    * @param tEnv the environment to cleanup
    * @param log writer to log information while testing
    */
    protected void cleanup( TestParameters Param, PrintWriter log) {
        log.println( "    closing Dialog " );
        try {
            accAction.doAccessibleAction(0);
        } catch (com.sun.star.lang.IndexOutOfBoundsException iae) {
            log.println("Couldn't close dialog");
        }
    }

    /**
     * Called while the <code>TestCase</code> initialization. In the
     * implementation does nothing. Subclasses can override to initialize
     * objects shared among all <code>TestEnvironment</code>s.
     *
     * @param tParam test parameters
     * @param log writer to log information while testing
     *
     * @see #initializeTestCase()
     */
    protected void initialize(TestParameters Param, PrintWriter log) {
        // get a soffice factory object
        SOfficeFactory SOF = SOfficeFactory.getFactory( (XMultiServiceFactory)Param.getMSF());

        log.println("opening dialog");

        PropertyValue[] args = new PropertyValue[1];
        try {
            args[0] = new PropertyValue();
            args[0].Name = "InteractionHandler";
            args[0].Value = ((XMultiServiceFactory)Param.getMSF()).createInstance(
                "com.sun.star.comp.uui.UUIInteractionHandler");
        } catch(com.sun.star.uno.Exception e) {
        }

        lThread = new loadThread(SOF, args);
        lThread.start();
        shortWait();
    }

    /**
    * Sleeps for 2 sec. to allow StarOffice to react on <code>
    * reset</code> call.
    */
    private void shortWait() {
        try {
            Thread.sleep(2000) ;
        } catch (InterruptedException e) {
            log.println("While waiting :" + e) ;
        }
    }

    public class loadThread extends Thread {

        private SOfficeFactory SOF = null ;
        private PropertyValue[] args = null;
        public XComponent xSpreadSheedDoc = null;

        public loadThread(SOfficeFactory SOF, PropertyValue[] Args) {
            this.SOF = SOF;
            this.args = Args;
        }

        public void run() {
            try {
                String url= utils.getFullTestURL("10test.csv");
                log.println("loading "+url);
                SOF.loadDocument(url,args);
            } catch (com.sun.star.uno.Exception e) {
                e.printStackTrace();
                throw new StatusException( "Couldn't create document ", e );
            }
        }
    }


}
