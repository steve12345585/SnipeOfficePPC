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

package mod._ucpdav;

import java.io.PrintWriter;

import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;

import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.ucb.XContentIdentifierFactory;
import com.sun.star.uno.Exception;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;

/**
 * Creates a <code>com.sun.star.ucb.WebDAVContentProvider</code> instance.
 */
public class WebDAVContentProvider extends TestCase {
    /**
     * Creates a <code>com.sun.star.ucb.WebDAVContentProvider</code> instance
     * and adds a <code>XContentIdentifierFactory</code> instance as a "FACTORY"
     * relation for <code>XContentProviderTest</code>.
     */
    public TestEnvironment createTestEnvironment
            (TestParameters tParam, PrintWriter log) {
        XMultiServiceFactory xMSF = (XMultiServiceFactory)tParam.getMSF();
        XInterface xInt;

        log.println("creating WebDAVContentProvider instance");
        try {
            xInt = (XInterface)xMSF.createInstance(
                    "com.sun.star.ucb.WebDAVContentProvider");
        } catch (Exception e) {
            log.println("Unexpected exception " + e.getMessage());
            e.printStackTrace(log);
            throw new StatusException("Unexpected exception", e);
        }

        TestEnvironment tEnv = new TestEnvironment(xInt);

        log.println("creating XContentIdentifierFactory");
        XContentIdentifierFactory xCIF;

        try {
            xCIF = UnoRuntime.queryInterface(
                    XContentIdentifierFactory.class,
                    xMSF.createInstance(
                            "com.sun.star.comp.ucb.UniversalContentBroker"));
        } catch (Exception e) {
            log.println("Unexpected exception " + e.getMessage());
            e.printStackTrace(log);
            throw new StatusException("Unexpected exception", e);
        }

        tEnv.addObjRelation("FACTORY", xCIF);

        tEnv.addObjRelation("CONTENT1",
            "vnd.sun.star.webdav://localhost/davhome/");

        return tEnv;
    }
}
