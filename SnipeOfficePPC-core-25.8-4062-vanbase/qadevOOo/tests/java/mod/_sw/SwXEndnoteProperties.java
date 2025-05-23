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
import util.dbg;

import com.sun.star.beans.XPropertySet;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.text.XEndnotesSupplier;
import com.sun.star.text.XText;
import com.sun.star.text.XTextContent;
import com.sun.star.text.XTextCursor;
import com.sun.star.text.XTextDocument;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;

/**
 * Test for object which is represented by service
 * <code>com.sun.star.text.FootnoteSettings</code>. <p>
 * Object implements the following interfaces :
 * <ul>
 *  <li> <code>com::sun::star::text::FootnoteSettings</code></li>
 * </ul> <p>
 * This object test <b> is NOT </b> designed to be run in several
 * threads concurently.
 * @see com.sun.star.text.FootnoteSettings
 * @see ifc.text._FootnoteSettings
 */
public class SwXEndnoteProperties extends TestCase {
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
    * <code>com.sun.star.text.Endnote</code>. Then created endnote is inserted
    * to the text document, and finally endnote settings are gotten from text
    * document using <code>XEndnotesSupplier</code> interface.
    */
    public synchronized TestEnvironment createTestEnvironment(
            TestParameters Param, PrintWriter log ) throws StatusException {
        XEndnotesSupplier oInterface = null;
        XInterface oObj = null;
        XInterface oEndnote;

        log.println( "Creating a test environment" );
        XMultiServiceFactory msf = UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDoc);
        log.println("creating a endnote");
        try {
            oEndnote = UnoRuntime.queryInterface(XInterface.class,
                    msf.createInstance("com.sun.star.text.Endnote"));
        } catch (com.sun.star.uno.Exception e) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't create endnote", e);
        }

        XText oText = xTextDoc.getText();
        XTextCursor oCursor = oText.createTextCursor();

        log.println("inserting the footnote into text document");
        XTextContent xTC = UnoRuntime.queryInterface(XTextContent.class, oEndnote);
        try {
            oText.insertTextContent(oCursor, xTC, false);
        } catch (com.sun.star.lang.IllegalArgumentException e) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't insert the endnote", e);
        }

        oInterface = UnoRuntime.queryInterface(XEndnotesSupplier.class, xTextDoc);
        oObj = oInterface.getEndnoteSettings();
        dbg.printPropertiesNames((XPropertySet) oObj);

        TestEnvironment tEnv = new TestEnvironment(oObj);
        return tEnv;
    }

}

