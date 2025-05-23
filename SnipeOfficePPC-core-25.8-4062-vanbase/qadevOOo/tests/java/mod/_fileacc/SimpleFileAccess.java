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

package mod._fileacc;

import java.io.PrintWriter;

import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;

import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.XInterface;

/**
* Test for object which is represented by service
* <code>com.sun.star.ucb.SimpleFileAccess</code>. <p>
* Object implements the following interfaces :
* <ul>
*  <li><code>com::sun::star::ucb.XSimpleFileAccess</code></li>
* </ul>
* This object test <b> can </b> be run in several
* threads concurently.
* @see com.sun.star.ucb.XSimpleFileAccess
* @see com.sun.star.ucb.SimpleFileAccess
* @see ifc.ucb._XSimpleFileAccess
*/
public class SimpleFileAccess extends TestCase {

    /**
    * Creating a Testenvironment for the interfaces to be tested.
    * Creates service <code>com.sun.star.ucb.SimpleFileAccess</code>.
    */
    public TestEnvironment createTestEnvironment( TestParameters Param,
                                                  PrintWriter log )
                                                    throws StatusException {
        XInterface oObj = null;
        Object oInterface = null;
        Object oIH = null;

        try {
            XMultiServiceFactory xMSF = (XMultiServiceFactory)Param.getMSF();
            oInterface = xMSF.createInstance
                ("com.sun.star.comp.ucb.SimpleFileAccess");
            oIH = xMSF.createInstance("com.sun.star.sdb.InteractionHandler");
        } catch( com.sun.star.uno.Exception e ) {
            log.println("SimpleFileAccess Service not available" );
        }

        oObj = (XInterface) oInterface;

        log.println( "    creating a new environment for CoreReflection object" );
        TestEnvironment tEnv = new TestEnvironment( oObj );

        tEnv.addObjRelation("InteractionHandler", oIH);

        return tEnv;
    } // finish method getTestEnvironment

}    // finish class SimpleFileAccess

