/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/
package helper;

import com.sun.star.comp.helper.Bootstrap;
import com.sun.star.lang.XMultiComponentFactory;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XComponentContext;
import java.util.Hashtable;
import lib.TestParameters;
import util.PropertyName;
import util.utils;

/**
 * Bootstrap UNO from a Java environment.
 * Needed parameters:
 * <ol>
 *  <li>
 *      <ul>
 *          <li>UNORC - complete path to the unorc file</li>
 *      </ul>
 *  </li>
 *  <li>
 *      <ul>
 *          <li>AppExecutionCommand - path to the soffice executable</li>
 *          <li>OS - the operating system in case it's Windows, because the
 *              unorc is called uno.ini</li>
 *      </ul>
 *  </li>
 * </ol>
 */
public class UnoProvider implements AppProvider {

    public UnoProvider(){

    }

    /**
     * Close existing office: calls disposeManager()
     * @param param The test parameters.
     * @param closeIfPossible Not needed, since UNO is bootstrapped by this
     * class in every case.
     * @return True, if bootstrapping worked.
     */
    public boolean closeExistingOffice(TestParameters param,
                                                    boolean closeIfPossible) {
        return disposeManager(param);
    }

    /**
     * Dispose the UNO environment: just clears the bootstrapped
     * MultiServiceFactory
     * @param param The test parameters.
     * @return True, if bootstrapping worked.
     */
    public boolean disposeManager(TestParameters param) {
        XMultiServiceFactory xMSF =
                    (XMultiServiceFactory)param.remove("ServiceManager");
        xMSF = null;
        System.gc();
        try {
            Thread.sleep(1000);
        }
        catch(java.lang.InterruptedException e) {}
        return true;
    }

    /**
     * Bootstrap UNO and return the created MultiServiceFactory.
     * @param param The test parameters.
     * @return A created MultiServiceFactory.
     */
    public Object getManager(TestParameters param) {
        XMultiServiceFactory xMSF = (XMultiServiceFactory)param.getMSF();
        if (xMSF == null) {
            // bootstrap UNO.
            String unorcName = getUnorcName(param);
            Hashtable env = new Hashtable();
            env.put("SYSBINDIR", getSysBinDir(param));

            XComponentContext xContext = null;
            try {
                xContext = Bootstrap.defaultBootstrap_InitialComponentContext(
                                                                 unorcName, env);
            }
            catch(Exception e) {
                e.printStackTrace();
                System.out.println("Could not get XComponentContext. Maybe you must add program folder to LD_LIBRARY_PATH");
                return null;
            }
            XMultiComponentFactory xMCF = xContext.getServiceManager();
            xMSF = (XMultiServiceFactory)UnoRuntime.queryInterface(
                                XMultiServiceFactory.class, xMCF);
        }
        return xMSF;
    }

    private String getUnorcName(TestParameters param) {
        String unorcName = (String)param.get("UNORC");
        if (unorcName == null) {
            String office = (String)param.get("AppExecutionCommand");
            // determine unorc name: unorc or uno.ini on windows
            String opSystem = (String)param.get(PropertyName.OPERATING_SYSTEM);
            if ( opSystem != null && opSystem.equalsIgnoreCase(PropertyName.WNTMSCI)) {
                unorcName = "uno.ini";
            }
            else {
                unorcName = "unorc";
            }
            if (office == null)
                return null;
            // use '/', because this will be a URL in any case.
            unorcName = office.substring(0, office.indexOf("program")+7) +
                        "/" + unorcName;
        }
        unorcName = utils.getFullURL(unorcName);
        if (param.DebugIsActive) {
            System.out.println("UnoUcr: " + unorcName);
        }
        return unorcName;
    }

    private String getSysBinDir(TestParameters param) {
        String base = (String)param.get("AppExecutionCommand");
        if (base == null)
            base = (String)param.get("UNORC");

        if (base == null)
            return null;

        String sysbindir = base.substring(0,
                                base.indexOf("program")+7);

        sysbindir = utils.getFullURL(sysbindir);
        if (param.DebugIsActive) {
            System.out.println("SysBinDir: " + sysbindir);
        }
        return sysbindir;
    }
}
