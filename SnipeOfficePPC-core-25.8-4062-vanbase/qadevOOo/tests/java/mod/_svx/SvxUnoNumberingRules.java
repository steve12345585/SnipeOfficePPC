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
package mod._svx;

import com.sun.star.container.XIndexAccess;
import com.sun.star.lang.XComponent;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.Exception;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;

import java.io.PrintWriter;

import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;

import util.DrawTools;


public class SvxUnoNumberingRules extends TestCase {
    static XComponent xDrawDoc;

    protected void initialize(TestParameters tParam, PrintWriter log) {
        log.println("creating a drawdoc");
        xDrawDoc = DrawTools.createDrawDoc(
                           (XMultiServiceFactory) tParam.getMSF());
    }

    protected TestEnvironment createTestEnvironment(TestParameters tParam,
                                                    PrintWriter log) {
        XMultiServiceFactory docMSF = UnoRuntime.queryInterface(
                                              XMultiServiceFactory.class,
                                              xDrawDoc);
        XInterface oObj = null;

        try {
            oObj = (XInterface) docMSF.createInstance(
                           "com.sun.star.text.NumberingRules");
        } catch (Exception e) {
            e.printStackTrace(log);
            throw new StatusException("Unexpected exception", e);
        }

        TestEnvironment tEnv = new TestEnvironment(oObj);

        log.println("Implementationname: " + util.utils.getImplName(oObj));

        Object NewRules=null;

        try{
        XIndexAccess xIA = UnoRuntime.queryInterface(XIndexAccess.class, oObj);
        NewRules = xIA.getByIndex(2);
        } catch (com.sun.star.lang.IndexOutOfBoundsException e) {
            log.println("Couldn't get new Rules for XIndexReplace");
        } catch (com.sun.star.lang.WrappedTargetException e) {
            log.println("Couldn't get new Rules for XIndexReplace");
        }

        tEnv.addObjRelation("INSTANCE1",NewRules);

        return tEnv;
    }

    protected void cleanup(TestParameters tParam, PrintWriter log) {
        log.println("    disposing xDrawDoc ");
        util.DesktopTools.closeDoc(xDrawDoc);
    }
}
