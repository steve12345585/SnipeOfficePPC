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

//API Interfaces
import java.io.PrintWriter;

import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;
import util.SOfficeFactory;

import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.text.XTextDocument;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;

/**
 * Test for object which is represented by service
 * <code>com.sun.star.text.Defaults</code> created by a writer
 * document's <code>XMultiServiceFactory</code>. <p>
 *
 * Object implements the following interfaces :
 * <ul>
 *  <li> <code>com::sun::star::style::ParagraphProperties</code></li>
 *  <li> <code>com::sun::star::style::CharacterPropertiesComplex</code></li>
 *  <li> <code>com::sun::star::style::CharacterPropertiesAsian</code></li>
 *  <li> <code>com::sun::star::text::Defaults</code></li>
 *  <li> <code>com::sun::star::style::CharacterProperties</code></li>
 *  <li> <code>com::sun::star::beans::XPropertySet</code></li>
 *  <li> <code>com::sun::star::style::ParagraphPropertiesComplex</code></li>
 * </ul> <p>
 *
 * @see com.sun.star.style.ParagraphProperties
 * @see com.sun.star.style.CharacterPropertiesComplex
 * @see com.sun.star.style.CharacterPropertiesAsian
 * @see com.sun.star.text.Defaults
 * @see com.sun.star.style.CharacterProperties
 * @see com.sun.star.beans.XPropertySet
 * @see com.sun.star.style.ParagraphPropertiesComplex
 * @see ifc.style._ParagraphProperties
 * @see ifc.style._CharacterPropertiesComplex
 * @see ifc.style._CharacterPropertiesAsian
 * @see ifc.text._Defaults
 * @see ifc.style._CharacterProperties
 * @see ifc.beans._XPropertySet
 * @see ifc.style._ParagraphPropertiesComplex
 */
public class SwXTextDefaults extends TestCase {

    XTextDocument xTextDoc;

    /**
     * Creates the service <code>com.sun.star.text.Defaults</code>
     */
    protected TestEnvironment createTestEnvironment(
            TestParameters tParam, PrintWriter log) {

        XInterface oObj = null;

        XMultiServiceFactory docMSF = UnoRuntime.queryInterface(XMultiServiceFactory.class,xTextDoc);

        try {
            oObj = (XInterface)
                docMSF.createInstance("com.sun.star.text.Defaults");
        } catch (com.sun.star.uno.Exception e) {
            throw new StatusException("Couldn't create Object",e);
        }

        TestEnvironment tEnv = new TestEnvironment(oObj);

        return tEnv;
    }

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
}
