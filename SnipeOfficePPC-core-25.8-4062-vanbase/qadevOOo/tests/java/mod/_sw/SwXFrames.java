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

import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.text.XText;
import com.sun.star.text.XTextCursor;
import com.sun.star.text.XTextDocument;
import com.sun.star.text.XTextFrame;
import com.sun.star.text.XTextFramesSupplier;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;

/**
 * Object implements the following interfaces :
 * <ul>
 *  <li> <code>com::sun::star::container::XContainer</code></li>
 *  <li> <code>com::sun::star::container::XNameAccess</code></li>
 *  <li> <code>com::sun::star::container::XIndexAccess</code></li>
 *  <li> <code>com::sun::star::container::XElementAccess</code></li>
 * </ul> <p>
 * This object test <b> is NOT </b> designed to be run in several
 * threads concurently.
 * @see com.sun.star.container.XContainer
 * @see com.sun.star.container.XNameAccess
 * @see com.sun.star.container.XIndexAccess
 * @see com.sun.star.container.XElementAccess
 * @see ifc.container._XContainer
 * @see ifc.container._XNameAccess
 * @see ifc.container._XIndexAccess
 * @see ifc.container._XElementAccess
 */
public class SwXFrames extends TestCase {
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
    * <code>com.sun.star.text.TextFrame</code>. Then inserts created text frame
    * to the text, and finally gets all frames of text document using
    * <code>XTextFramesSupplier</code> interface.<br>
    */
    public synchronized TestEnvironment createTestEnvironment(
            TestParameters Param, PrintWriter log ) throws StatusException {
        XInterface oObj = null;
        XTextFrame oFrame1 = null;
        XText oText = null;
        XTextCursor oCursor = null;
        XMultiServiceFactory oDocMSF = null;
        XTextFramesSupplier oInterface = null;

        log.println( "creating a test environment" );
        try {
            oDocMSF = UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDoc);
            Object oInt = oDocMSF.createInstance("com.sun.star.text.TextFrame");
            oFrame1 = UnoRuntime.queryInterface( XTextFrame.class, oInt );
        } catch ( com.sun.star.uno.Exception e ) {
            e.printStackTrace(log);
            throw new StatusException
                ("Couldn't create instance of TextFrame", e);
        }

        oText = xTextDoc.getText();
        oCursor = oText.createTextCursor();

        try {
            oText.insertTextContent(oCursor, oFrame1, false);
        } catch ( com.sun.star.lang.IllegalArgumentException e ) {
            e.printStackTrace(log);
            throw new StatusException
                ("Error: can't insert text content to text document", e);
        }

        oInterface = UnoRuntime.queryInterface( XTextFramesSupplier.class, xTextDoc );

        oObj = oInterface.getTextFrames();

        log.println( "creating a new environment for Frame object" );
        TestEnvironment tEnv = new TestEnvironment( oObj );

        return tEnv;
    } // finish method getTestEnvironment

}    // finish class SwXFrames

