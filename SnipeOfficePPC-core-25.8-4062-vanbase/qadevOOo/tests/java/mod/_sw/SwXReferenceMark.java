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

import java.io.PrintWriter;

import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;
import util.SOfficeFactory;

import com.sun.star.container.XNamed;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.text.XText;
import com.sun.star.text.XTextContent;
import com.sun.star.text.XTextCursor;
import com.sun.star.text.XTextDocument;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;

/**
 * Test for object which is represented by service
 * <code>com.sun.star.text.ReferenceMark</code>. <p>
 * Object implements the following interfaces :
 * <ul>
 *  <li> <code>com::sun::star::container::XNamed</code></li>
 *  <li> <code>com::sun::star::lang::XComponent</code></li>
 *  <li> <code>com::sun::star::text::XTextContent</code></li>
 * </ul> <p>
 * This object test <b> is NOT </b> designed to be run in several
 * threads concurently.
 * @see com.sun.star.container.XNamed
 * @see com.sun.star.lang.XComponent
 * @see com.sun.star.text.XTextContent
 * @see com.sun.star.text.ReferenceMark
 * @see ifc.container._XNamed
 * @see ifc.lang._XComponent
 * @see ifc.text._XTextContent
 */
public class SwXReferenceMark extends TestCase {
    XTextDocument xTextDoc;

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
        util.DesktopTools.closeDoc(xTextDoc);
    }

    /**
    * Creating a Testenvironment for the interfaces to be tested.
    * Creates an instance of the service
    * <code>com.sun.star.text.ReferenceMark</code>, then sets new
    * name of created ReferenceMark using <code>XNamed</code> interface.
    * Finally, renamed ReferenceMark is inserted to a major text of text
    * document as a text content.
    */
    protected synchronized TestEnvironment createTestEnvironment(TestParameters Param, PrintWriter log) {
        XInterface oObj = null;
        XText oText = null;
        String Name = "SwXReferenceMark";

        log.println( "creating a test environment" );
        oText = xTextDoc.getText();
        XMultiServiceFactory oDocMSF = UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDoc);
        Object instance = null;
        try {
            oObj = (XInterface) oDocMSF.createInstance
                ("com.sun.star.text.ReferenceMark");
            instance = oDocMSF.createInstance
                ("com.sun.star.text.ReferenceMark");
        } catch ( com.sun.star.uno.Exception e ) {
            e.printStackTrace( log );
            throw new StatusException( "Couldn't get ReferenceMark", e);
        }

        XNamed oObjN = UnoRuntime.queryInterface(XNamed.class, oObj);
        oObjN.setName(Name);
        XTextContent oObjTC = UnoRuntime.queryInterface(XTextContent.class, oObj);

        XTextCursor oCursor = oText.createTextCursor();
        try {
            oText.insertTextContent(oCursor, oObjTC, false);
        } catch ( com.sun.star.lang.IllegalArgumentException e ){
            e.printStackTrace( log );
            throw new StatusException(" Couldn't insert ReferenceMark", e);
        }

        TestEnvironment tEnv = new TestEnvironment( oObj );

        tEnv.addObjRelation("CONTENT", UnoRuntime.queryInterface(XTextContent.class,instance));
        tEnv.addObjRelation("RANGE", xTextDoc.getText().createTextCursor());

        return tEnv;
    } // finish method getTestEnvironment

}    // finish class SwXReferenceMark

