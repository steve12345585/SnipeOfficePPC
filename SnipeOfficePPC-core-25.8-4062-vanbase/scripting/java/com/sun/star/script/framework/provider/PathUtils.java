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

package com.sun.star.script.framework.provider;

import java.util.StringTokenizer;

import java.lang.reflect.Method;

import com.sun.star.frame.XModel;
import com.sun.star.uno.IQueryInterface;

public class PathUtils {

    public static String FILE_URL_PREFIX;
    public static String BOOTSTRAP_NAME;
    private static boolean m_windows = false;

    static {
        // detect if windows platform
        if (System.getProperty("os.name").startsWith("Windows"))
            m_windows = true;

        FILE_URL_PREFIX = m_windows ? "file:///" : "file://";
        BOOTSTRAP_NAME = m_windows ? "bootstrap.ini" : "bootstraprc";
    }
    public static String getOidForModel( XModel xModel )
    {
        String oid = new String("");
        if ( xModel != null )
        {
            try
            {
                Method getOid = IQueryInterface.class.getMethod("getOid", (java.lang.Class[])null);
                if ( getOid != null )
                {
                    oid = (String)getOid.invoke( xModel, new Object[0] );
                }

            }
            catch ( Exception ignore )
            {
            }
        }
        return oid;
    }
    static  public String make_url( String baseUrl, String url )
    {
        StringBuffer buff = new StringBuffer( baseUrl.length() + url.length() );        buff.append( baseUrl );
        StringTokenizer t = new StringTokenizer( url, "/");
        while ( t.hasMoreElements() )
        {
           if ( buff.charAt( buff.length() - 1 ) != '/' )
           {
               buff.append('/');
           }
           buff.append( java.net.URLEncoder.encode( (String)t.nextElement() ) );        }
        return buff.toString();
    }

    private PathUtils() {
    }
}
