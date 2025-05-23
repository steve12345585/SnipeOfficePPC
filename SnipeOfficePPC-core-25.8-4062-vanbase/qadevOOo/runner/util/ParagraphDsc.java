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

package util;

import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.XInterface;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.text.XTextContent;

/**
 * the class ParagraphDsc
 */
public class ParagraphDsc extends InstDescr {

    final String service = "com.sun.star.text.Paragraph";
    String ifcName = "com.sun.star.text.XTextContent";
    private String name = null;


    public ParagraphDsc() {
        initParagraph();
    }

    public ParagraphDsc( String name ) {
        this.name = name;
        initParagraph();
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

    private void initParagraph() {
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
        XTextContent PG = (XTextContent)UnoRuntime.queryInterface( ifcClass,
                                                                ServiceObj );
        return PG;
    }
}