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

package mod._odbc;

import java.io.PrintWriter;

import lib.Status;
import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;

import com.sun.star.beans.PropertyValue;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.XInterface;

/**
 * Here <code>com.sun.star.sdbc.Driver</code> service is tested.<p>
 * Test allows to run object tests in several threads concurently.
 * @see com.sun.star.sdbc.Driver
 * @see com.sun.star.sdbc.XDriver
 * @see ifc.sdbc._XDriver
 */
public class ODBCDriver extends TestCase {
    /**
     * Creates an instance of the service
     * <code>com.sun.star.sdbc.Driver</code>. <p>
     * Object relations created :
     * <ul>
     *  <li> <code>'XDriver.URL'</code> for {@link ifc.sdbc._XDriver}:
     *      is the URL of the database to which to connect.
     *      The URL is obtained from the parameter <code>odbc.url</code></li>
     *  <li> <code>'XDriver.UNSUITABLE_URL'</code> for {@link ifc.sdbc._XDriver}:
     *      the wrong kind of URL to connect using given driver.
     *      The URL is obtained from the parameter <code>jdbc.url</code></li>
     *  <li> <code>'XDriver.INFO'</code> for {@link ifc.sdbc._XDriver}:
     *      a list of arbitrary string tag/value pairs as connection arguments.
     *      </li>
     * </ul>
     */
    protected synchronized TestEnvironment createTestEnvironment(TestParameters Param, PrintWriter log) {

        XInterface oObj = null;

        try {
            oObj = (XInterface)((XMultiServiceFactory)Param.getMSF()).createInstance(
                "com.sun.star.comp.sdbc.ODBCDriver");
        } catch (com.sun.star.uno.Exception e) {
            e.printStackTrace(log);
            throw new StatusException(Status.failed("Couldn't create object"));
        }

        log.println("creating a new environment for ODBCDriver object");
        TestEnvironment tEnv = new TestEnvironment(oObj);

        //adding relation for sdbc.XDriver
        String odbcURL = (String) Param.get("odbc.url");
        if (odbcURL == null) {
            throw new StatusException(Status.failed(
                "Couldn't get 'odbc.url' from ini-file"));
        }
        tEnv.addObjRelation("XDriver.URL", "sdbc:odbc:" + odbcURL);

        PropertyValue[] info = new PropertyValue[0];
        tEnv.addObjRelation("XDriver.INFO", info);

        String jdbcUrl = (String) Param.get("jdbc.url");
        if (jdbcUrl == null) {
            throw new StatusException(Status.failed(
                "Couldn't get 'jdbc.url' from ini-file"));
        }
        tEnv.addObjRelation("XDriver.UNSUITABLE_URL", "jdbc:" + jdbcUrl);

        return tEnv;
    }
}
