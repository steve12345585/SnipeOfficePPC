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
package mod._sw;

import java.io.PrintWriter;

import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;
import util.AccessibilityTools;
import util.WriterTools;
import util.utils;

import com.sun.star.accessibility.AccessibleRole;
import com.sun.star.accessibility.XAccessible;
import com.sun.star.awt.XWindow;
import com.sun.star.beans.XPropertySet;
import com.sun.star.frame.XController;
import com.sun.star.frame.XModel;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.text.XText;
import com.sun.star.text.XTextContent;
import com.sun.star.text.XTextCursor;
import com.sun.star.text.XTextDocument;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;
import com.sun.star.view.XViewSettingsSupplier;

public class SwAccessibleEndnoteView extends TestCase {

    XTextDocument xTextDoc = null;

    /**
    * Called to create an instance of <code>TestEnvironment</code>
    * with an object to test and related objects.
    * Inserts the created endnote to the document.
    * Changes zoom value to 10%(endnote must be in vissible area of the document).
    * Obtains accessible object for the inserted endnote.
    *
    * @param tParam test parameters
    * @param log writer to log information while testing
    *
    * @see TestEnvironment
    * @see #getTestEnvironment()
    */
    protected TestEnvironment createTestEnvironment(
        TestParameters Param, PrintWriter log) {

        XInterface oObj = null;
        XInterface oEndnote = null;

        log.println( "Creating a test environment" );
        XMultiServiceFactory msf = (XMultiServiceFactory)
            UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDoc);
        log.println("creating a endnote");
        try {
            oEndnote = (XInterface) UnoRuntime.queryInterface(XInterface.class,
                    msf.createInstance("com.sun.star.text.Endnote"));
        } catch (com.sun.star.uno.Exception e) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't create endnote", e);
        }

        XText oText = xTextDoc.getText();
        XTextCursor oCursor = oText.createTextCursor();

        log.println("inserting the footnote into text document");
        XTextContent xTC = (XTextContent)
            UnoRuntime.queryInterface(XTextContent.class, oEndnote);
        try {
            oText.insertTextContent(oCursor, xTC, false);
        } catch (com.sun.star.lang.IllegalArgumentException e) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't insert the endnote", e);
        }

        XController xController = xTextDoc.getCurrentController();
        XViewSettingsSupplier xViewSetSup = (XViewSettingsSupplier)
                UnoRuntime.queryInterface(XViewSettingsSupplier.class,
                xController);
        XPropertySet xPropSet = xViewSetSup.getViewSettings();

        try {
            //change zoom value to 10%
            //footer should be in the vissible area of the document
            xPropSet.setPropertyValue("ZoomValue", new Short("10"));
        } catch ( com.sun.star.lang.WrappedTargetException e ) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't set propertyValue...", e);
        }  catch ( com.sun.star.lang.IllegalArgumentException e ) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't set propertyValue...", e);
        } catch ( com.sun.star.beans.PropertyVetoException e ) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't set propertyValue...", e);
        } catch ( com.sun.star.beans.UnknownPropertyException e ) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't set propertyValue...", e);
        }

        XModel aModel = (XModel)
            UnoRuntime.queryInterface(XModel.class, xTextDoc);

        AccessibilityTools at = new AccessibilityTools();

        XWindow xWindow = at.getCurrentWindow((XMultiServiceFactory)Param.getMSF(), aModel);
        XAccessible xRoot = at.getAccessibleObject(xWindow);

        at.getAccessibleObjectForRole(xRoot, AccessibleRole.END_NOTE);

        oObj = AccessibilityTools.SearchedContext;

        log.println("ImplementationName " + utils.getImplName(oObj));
        at.printAccessibleTree(log, xRoot, Param.getBool(util.PropertyName.DEBUG_IS_ACTIVE));

        TestEnvironment tEnv = new TestEnvironment(oObj);

        final XPropertySet PropSet = xViewSetSup.getViewSettings();

        tEnv.addObjRelation("EventProducer",
            new ifc.accessibility._XAccessibleEventBroadcaster.EventProducer() {
                public void fireEvent() {
                    try {
                        //change zoom value to 130%
                        PropSet.setPropertyValue("ZoomValue", new Short("15"));
                        //and back to 10%
                        PropSet.setPropertyValue("ZoomValue", new Short("10"));
                    } catch ( com.sun.star.lang.WrappedTargetException e ) {

                    }  catch ( com.sun.star.lang.IllegalArgumentException e ) {

                    } catch ( com.sun.star.beans.PropertyVetoException e ) {

                    } catch ( com.sun.star.beans.UnknownPropertyException e ) {

                    }
                }
            });

        return tEnv;

    }

    /**
    * Called while disposing a <code>TestEnvironment</code>.
    * Disposes text document.
    * @param tParam test parameters
    * @param tEnv the environment to cleanup
    * @param log writer to log information while testing
    */
    protected void cleanup( TestParameters Param, PrintWriter log) {
        log.println("dispose text document");
        util.DesktopTools.closeDoc(xTextDoc);
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
        log.println( "creating a text document" );
        xTextDoc = WriterTools.createTextDoc((XMultiServiceFactory)Param.getMSF());
    }
}
