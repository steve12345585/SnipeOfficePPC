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

package mod._ucpftp;

import java.io.PrintWriter;

import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;

import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.ucb.XContentIdentifierFactory;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;

public class FTPContentProvider extends TestCase {

    public TestEnvironment createTestEnvironment
            ( TestParameters Param,PrintWriter log ) {

        XInterface oObj = null;
        Object oInterface = null;
        Object aUCB = null;

        try {
            XMultiServiceFactory xMSF = (XMultiServiceFactory)Param.getMSF();
            oInterface = xMSF.createInstance
                ( "com.sun.star.ucb.FTPContentProvider" );
            aUCB = xMSF.createInstance
                ( "com.sun.star.comp.ucb.UniversalContentBroker" );
        } catch( com.sun.star.uno.Exception e ) {
            log.println("Can't create an object." );
            throw new StatusException( "Can't create an object", e );
        }

        oObj = (XInterface) oInterface;

        XContentIdentifierFactory CIF = UnoRuntime.queryInterface(XContentIdentifierFactory.class,aUCB);

        log.println("ImplementationName: "+util.utils.getImplName(oObj));
        util.dbg.printInterfaces(oObj);
        TestEnvironment tEnv = new TestEnvironment( oObj );

        //Adding ObjRelation for XContentProvider
        tEnv.addObjRelation("FACTORY",CIF);
        tEnv.addObjRelation("CONTENT1","ftp:///ftp.sun.com");
        tEnv.addObjRelation("CONTENT2","ftp:///ftp.openoffice.org");

        return tEnv;

    } // finish method getTestEnvironment

}    // finish class FileProvider

