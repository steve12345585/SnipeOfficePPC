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

package mod._sw;

import com.sun.star.beans.PropertyAttribute;
import com.sun.star.beans.XPropertySet;
import com.sun.star.container.XIndexAccess;
import com.sun.star.container.XNameAccess;
import com.sun.star.container.XNameContainer;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.style.XStyle;
import com.sun.star.style.XStyleFamiliesSupplier;
import com.sun.star.text.XText;
import com.sun.star.text.XTextCursor;
import com.sun.star.text.XTextDocument;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;
import java.io.PrintWriter;
import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;
import util.DesktopTools;
import util.SOfficeFactory;
import util.utils;

/**
 * Test for object which is represented by service
 * <code>com.sun.star.style.CharacterStyle</code>. <p>
 * @see com.sun.star.style.CharacterStyle
 */
public class CharacterStyle extends TestCase {
    private XTextDocument xTextDoc;

    /**
    * Creates text document.
    */
    protected void initialize( TestParameters tParam, PrintWriter log ) {
        SOfficeFactory SOF = SOfficeFactory.getFactory( (XMultiServiceFactory)tParam.getMSF() );
        try {
            log.println( "creating a textdocument" );
            xTextDoc = SOF.createTextDoc( null );
        } catch ( com.sun.star.uno.Exception e ) {
            e.printStackTrace( log );
            throw new StatusException( "Couldn't create document", e );
        }
    }

    /**
    * Disposes text document.
    */
    protected void cleanup( TestParameters tParam, PrintWriter log ) {
        log.println( "    disposing xTextDoc " );
        DesktopTools.closeDoc(xTextDoc);
    }

    /**
    * Creating a Testenvironment for the interfaces to be tested.
    * At first style families are gotten from a text document using
    * <code>XStyleFamiliesSupplier</code> interface, then family indexed '0' is
    * gotten from this style family using <code>XIndexAccess</code> interface.
    * Next, method creates an instance of the service
    * <code>com.sun.star.style.CharacterStyle</code> and inserts it to a
    * previously obtained style family using <code>XNameContainer</code>
    * interface. Finally, method creates a cursor of a major text of text
    * document and sets it's property 'CharStyleName' value to the name of
    * previously created style's name.
    *     Object relations created :
    * <ul>
    *  <li> <code>'PoolStyle'</code> for
    *      {@link ifc.style._XStyle} : style indexed '10' obtained from
    *  StyleFamily indexed '0' from text document using
    *  <code>XIndexAccess</code> interface.</li>
    * </ul>
    */
    protected TestEnvironment createTestEnvironment(TestParameters tParam, PrintWriter log) {

        TestEnvironment tEnv = null;
        XNameAccess oSFNA = null;
        XStyle oStyle = null;
        XStyle oMyStyle = null;

        log.println("creating a test environment");

        try {
            log.println("getting style");
            XStyleFamiliesSupplier oSFS = UnoRuntime.queryInterface(XStyleFamiliesSupplier.class,
            xTextDoc);
            XNameAccess oSF = oSFS.getStyleFamilies();
            XIndexAccess oSFsIA = UnoRuntime.queryInterface(XIndexAccess.class, oSF);
            oSFNA = UnoRuntime.queryInterface(
                            XNameAccess.class,oSFsIA.getByIndex(0));
            XIndexAccess oSFIA = UnoRuntime.queryInterface(XIndexAccess.class, oSFNA);
            oStyle = UnoRuntime.queryInterface(
                            XStyle.class,oSFIA.getByIndex(0));
        } catch ( com.sun.star.lang.WrappedTargetException e ) {
            log.println("Error: exception occurred.");
            e.printStackTrace(log);
            throw new StatusException( "Couldn't create environment ", e );
        } catch ( com.sun.star.lang.IndexOutOfBoundsException e ) {
            log.println("Error: exception occurred.");
            e.printStackTrace(log);
            throw new StatusException( "Couldn't create environment ", e );
        }

        try {
            log.print("Creating a user-defined style... ");
            XMultiServiceFactory oMSF = UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDoc);
            XInterface oInt = (XInterface)
                oMSF.createInstance("com.sun.star.style.CharacterStyle");
            oMyStyle = UnoRuntime.queryInterface(XStyle.class, oInt);
        } catch ( com.sun.star.uno.Exception e ) {
            log.println("Error: exception occurred.");
            e.printStackTrace(log);
            throw new StatusException( "Couldn't create environment ", e );
        }


        if (oMyStyle == null)
            log.println("FAILED");
        else
            log.println("OK");
            XNameContainer oSFNC = UnoRuntime.queryInterface(XNameContainer.class, oSFNA);

        try {
            if ( oSFNC.hasByName("My Style") )
                oSFNC.removeByName("My Style");
            oSFNC.insertByName("My Style", oMyStyle);
        } catch ( com.sun.star.lang.WrappedTargetException e ) {
            e.printStackTrace(log);
            throw new StatusException( "Couldn't create environment ", e );
        } catch     ( com.sun.star.lang.IllegalArgumentException e ) {
            e.printStackTrace(log);
            throw new StatusException( "Couldn't create environment ", e );
        } catch ( com.sun.star.container.NoSuchElementException e ) {
            e.printStackTrace(log);
            throw new StatusException( "Couldn't create environment ", e );
        } catch ( com.sun.star.container.ElementExistException e ) {
            e.printStackTrace(log);
            throw new StatusException( "Couldn't create environment ", e );
        }

        XText oText = xTextDoc.getText();
        XTextCursor oCursor = oText.createTextCursor();
        XPropertySet xProp = UnoRuntime.queryInterface(XPropertySet.class, oCursor);

        try {
            xProp.setPropertyValue("CharStyleName", oMyStyle.getName());
        } catch ( com.sun.star.lang.WrappedTargetException e ) {
            e.printStackTrace( log );
            throw new StatusException( "Couldn't create environment ", e );
        } catch ( com.sun.star.lang.IllegalArgumentException e ) {
            e.printStackTrace( log );
            throw new StatusException( "Couldn't create environment ", e );
        } catch ( com.sun.star.beans.PropertyVetoException e ) {
            e.printStackTrace( log );
            throw new StatusException( "Couldn't create environment ", e );
        } catch ( com.sun.star.beans.UnknownPropertyException e ) {
            e.printStackTrace( log );
            throw new StatusException( "Couldn't create environment ", e );
        }

        log.println("creating a new environment for object");
        tEnv = new TestEnvironment(oMyStyle);
        tEnv.addObjRelation("PoolStyle", oStyle);

        XPropertySet xStyleProp = UnoRuntime.queryInterface(XPropertySet.class, oMyStyle);

        short exclude = PropertyAttribute.READONLY;
        tEnv.addObjRelation("PropertyNames",utils.getFilteredPropertyNames(xStyleProp, (short)0, exclude));

        return tEnv;
    }

}
