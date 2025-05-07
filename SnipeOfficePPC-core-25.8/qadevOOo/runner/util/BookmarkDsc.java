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

package util;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.XInterface;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.text.XTextContent;

/**
 * describes a Bookmark to be inserted in a container
 */
public class BookmarkDsc extends InstDescr {

    final String service = "com.sun.star.text.Bookmark";
    String ifcName = "com.sun.star.text.XTextContent";
    private String name = null;


    public BookmarkDsc() {
            initBookmark();
    }

    public BookmarkDsc( String name ) {
            this.name = name;
            initBookmark();
    }

    public String getName() {
            return name;
    }

    public String getIfcName() {
            return ifcName;
    }

    public String getService() {
            return service;
    }

    private void initBookmark() {
            try {
            ifcClass = Class.forName( ifcName );
        }
        catch( ClassNotFoundException cnfE ) {
        }
    }
    public XInterface createInstance( XMultiServiceFactory docMSF ) {
            Object ServiceObj = null;

            try {
                    ServiceObj = docMSF.createInstance( service );
            }
            catch( com.sun.star.uno.Exception cssuE ){
            }
            XTextContent BM = (XTextContent)UnoRuntime.queryInterface( ifcClass,
                                                                                                                            ServiceObj );
            return BM;
    }
}