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

package com.sun.star.comp.loader;

import com.sun.star.lang.XSingleServiceFactory;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.lang.XServiceInfo;


public class JavaLoaderFactory implements XSingleServiceFactory, XServiceInfo {

    private static String[] supportedServices = {
        "com.sun.star.loader.Java",
        "com.sun.star.loader.Java2"
    };

    private static final boolean DEBUG = false;

    private static final void DEBUG(String dbg) {
        if (DEBUG)
            System.err.println(" >>> JavaLoaderFactory - " + dbg);
    }

    protected XMultiServiceFactory multiServiceFactory = null;

    /** default constructor
     */
//      public JavaLoaderFactory() {}

    public JavaLoaderFactory(XMultiServiceFactory factory) {
        multiServiceFactory = factory;
    }

    public java.lang.Object createInstance()
            throws com.sun.star.uno.Exception,
                   com.sun.star.uno.RuntimeException
    {
        return new JavaLoader(multiServiceFactory);
    }

    public java.lang.Object createInstanceWithArguments( java.lang.Object[] args )
            throws com.sun.star.uno.Exception,
                   com.sun.star.uno.RuntimeException
    {
        JavaLoader loader = new JavaLoader();
        loader.initialize(args);

        return loader;
    }

    /** implements the XServiceInfo interface
     */
    public String getImplementationName()
            throws com.sun.star.uno.RuntimeException
    {
        return JavaLoader.class.getName();
    }

    /** implements the XServiceInfo interface
     */
    public boolean supportsService(String serviceName)
            throws com.sun.star.uno.RuntimeException
    {
        for ( int i = 0; i < supportedServices.length; i++ ) {
            if ( supportedServices[i].equals(serviceName) )
                return true;
        }
        return false;
    }

    /** implements the XServiceInfo interface
     */
    public String[] getSupportedServiceNames()
            throws com.sun.star.uno.RuntimeException
    {
        return supportedServices;
    }
}

