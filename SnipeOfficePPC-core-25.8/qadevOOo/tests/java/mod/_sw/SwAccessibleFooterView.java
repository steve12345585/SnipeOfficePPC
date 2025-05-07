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
import com.sun.star.container.XNameAccess;
import com.sun.star.frame.XController;
import com.sun.star.frame.XModel;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.style.XStyle;
import com.sun.star.style.XStyleFamiliesSupplier;
import com.sun.star.text.XTextDocument;
import com.sun.star.uno.AnyConverter;
import com.sun.star.uno.Type;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;
import com.sun.star.view.XViewSettingsSupplier;

/**
* Test of accessible object for a footer of a text document.<p>
* Object implements the following interfaces :
* <ul>
*  <li> <code>::com::sun::star::accessibility::XAccessible</code></li>
* </ul>
* @see com.sun.star.accessibility.XAccessible
*/
public class SwAccessibleFooterView extends TestCase {

    XTextDocument xTextDoc = null;

    /**
    * Called to create an instance of <code>TestEnvironment</code>
    * with an object to test and related objects. Obtains style
    * <code>Standard</code> from style family <code>PageStyles</code>.
    * Changes values of property <code>FooterIsOn</code> by <code>true</code>.
    * Changes zoom value to 10%(footer must be in vissible area of the document).
    * Obtains accessible component for the footer.
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
        XNameAccess PageStyles = null;
        XStyle StdStyle = null;

        XStyleFamiliesSupplier StyleFam = (XStyleFamiliesSupplier)
            UnoRuntime.queryInterface(XStyleFamiliesSupplier.class, xTextDoc);
        XNameAccess StyleFamNames = StyleFam.getStyleFamilies();

        // obtains style 'Standard' from style family 'PageStyles'
        try {
            PageStyles = (XNameAccess) AnyConverter.toObject(
                new Type(XNameAccess.class),StyleFamNames.getByName("PageStyles"));
            StdStyle = (XStyle) AnyConverter.toObject(
                    new Type(XStyle.class),PageStyles.getByName("Standard"));
        } catch ( com.sun.star.lang.WrappedTargetException e ) {
            e.printStackTrace(log);
            throw new StatusException("Error getting style by name!", e);
        } catch ( com.sun.star.container.NoSuchElementException e ) {
            e.printStackTrace(log);
            throw new StatusException("Error, no such style name! ", e);
        } catch ( com.sun.star.lang.IllegalArgumentException e ) {
            e.printStackTrace(log);
            throw new StatusException("Error getting style by name!", e);
        }

        final XPropertySet PropSet = (XPropertySet)
            UnoRuntime.queryInterface( XPropertySet.class, StdStyle);

        // changing/getting some properties
        try {
            log.println( "Switching on footer" );
            PropSet.setPropertyValue("FooterIsOn", new Boolean(true));

            //change zoom value to 10%
            //footer should be in the vissible area of the document
            XController xController = xTextDoc.getCurrentController();
            XViewSettingsSupplier xViewSetSup = (XViewSettingsSupplier)
                UnoRuntime.queryInterface(XViewSettingsSupplier.class,
                xController);
            XPropertySet xPropSet = xViewSetSup.getViewSettings();
            xPropSet.setPropertyValue("ZoomValue", new Short("20"));
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

        at.getAccessibleObjectForRole(xRoot, AccessibleRole.FOOTER);

        oObj = AccessibilityTools.SearchedContext;

        log.println("ImplementationName " + utils.getImplName(oObj));
        at.printAccessibleTree(log, xRoot, Param.getBool(util.PropertyName.DEBUG_IS_ACTIVE));

        TestEnvironment tEnv = new TestEnvironment(oObj);

        tEnv.addObjRelation("EventProducer",
            new ifc.accessibility._XAccessibleEventBroadcaster.EventProducer(){
                public void fireEvent() {
                    try {
                        PropSet.setPropertyValue("FooterLeftMargin",
                            new Integer(1000));
                    } catch (com.sun.star.uno.Exception e) {
                        e.printStackTrace();
                        throw new StatusException("Cann't change footer.", e);
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
     * Called while the <code>TestCase</code> initialization.
     * Creates a text document.
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
