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

package ifc.frame;

import lib.MultiMethodTest;
import util.utils;

import com.sun.star.frame.DispatchDescriptor;
import com.sun.star.frame.FrameSearchFlag;
import com.sun.star.frame.XDispatch;
import com.sun.star.frame.XDispatchProvider;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.util.URL;
import com.sun.star.util.XURLTransformer;

/**
* Testing <code>com.sun.star.frame.XDispatchProvider</code>
* interface methods:
* <ul>
*  <li><code> queryDispatch() </code></li>
*  <li><code> queryDispatches() </code></li>
* </ul><p>
* This test needs the following object relations :
* <ul>
*  <li> <code>'XDispatchProvider.URL'</code> (of type <code>String</code>):
*  used to obtain unparsed url of DispatchProvider </li>
* </ul> <p>
* Test is <b> NOT </b> multithread compilant. <p>
* @see com.sun.star.frame.XDispatchProvider
*/
public class _XDispatchProvider extends MultiMethodTest {
    public static XDispatchProvider oObj = null;
    private String dispatchUrl = null ;

    /**
    * Retrieves object relation.
    */
    public void before() {
        dispatchUrl = (String) tEnv.getObjRelation("XDispatchProvider.URL") ;
        if (dispatchUrl == null) {
            dispatchUrl = utils.getFullTestDocName("index.html");
        }
        log.println("Using URL: '" + dispatchUrl + "'");
    }

    /**
    * Test calls the method. <p>
    * Has <b> OK </b> status if the method does not return null.
    */
    public void _queryDispatch() {
        URL url = new URL();
        String frameName = "_top";

        url.Complete = dispatchUrl;
        try {
            XURLTransformer xParser=(XURLTransformer)
                UnoRuntime.queryInterface(XURLTransformer.class,
                    ((XMultiServiceFactory)tParam.getMSF()).createInstance
                        ("com.sun.star.util.URLTransformer"));
            // Because it's an in/out parameter we must use an array of
            // URL objects.
            URL[] aParseURL = new URL[1];
            aParseURL[0] = new URL();
            aParseURL[0].Complete = dispatchUrl;
            xParser.parseStrict(aParseURL);
            url = aParseURL[0];
        } catch (com.sun.star.uno.Exception e) {
            log.println("Couldn't parse URL");
        }
        XDispatch xDispatch = oObj.queryDispatch(url,
                frameName, FrameSearchFlag.ALL);
        tRes.tested("queryDispatch()", xDispatch != null);
    }

    /**
    * Before test calls the method, DispatchDescriptor sequence is defined.<p>
    * Has <b> OK </b> status if the method does not return null, returned
    * sequence length is equal to a number of DispatchDescriptors
    * and returned sequence consists of non-null elements.
    */
    public void _queryDispatches() {
        String name1 = "_self";
        String name2 = "_top";
        URL url1 = new URL();
        URL url2 = new URL();

        url1.Complete = dispatchUrl;
        url2.Complete = dispatchUrl;
        try {
            log.println("Parsing URL");
            XURLTransformer xParser = (XURLTransformer)
                UnoRuntime.queryInterface(XURLTransformer.class,
                    ((XMultiServiceFactory)tParam.getMSF()).createInstance
                        ("com.sun.star.util.URLTransformer"));
            // Because it's an in/out parameter we must use an array of
            // URL objects.
            URL[] aParseURL = new URL[1];
            aParseURL[0] = new URL();
            aParseURL[0].Complete = dispatchUrl;
            xParser.parseStrict(aParseURL);
            url1 = aParseURL[0];
            url2 = aParseURL[0];
        } catch (com.sun.star.uno.Exception e) {
            log.println("Couldn't parse URL");
        }
        DispatchDescriptor descs[] = new DispatchDescriptor[] {
            new DispatchDescriptor(url1, name1, FrameSearchFlag.ALL),
            new DispatchDescriptor(url2, name2, FrameSearchFlag.ALL)
        };
        XDispatch[] xDispatches = oObj.queryDispatches(descs);

        if (xDispatches == null) {
            log.println("queryDispatches() returned null");
            tRes.tested("queryDispatches()", false);
            return;
        }

        if (xDispatches.length != descs.length) {
            log.println("queryDispatches() returned "
                    + xDispatches.length
                    + " amount of XDispatch instead of "
                    + descs.length);
            tRes.tested("queryDispatches()", false);
            return;
        }

        for (int i = 0; i < xDispatches.length; i++) {
            if (xDispatches[i] == null) {
                log.println("queryDispatches() result contains"
                        + " null object");
                tRes.tested("queryDispatches()", false);
                return;
            }
        }

        tRes.tested("queryDispatches()", true);
        return;
    }

}

