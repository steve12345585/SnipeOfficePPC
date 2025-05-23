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

package org.openoffice.xmerge.converter.xml.sxw;

import org.openoffice.xmerge.converter.xml.OfficeDocument;
import org.openoffice.xmerge.converter.xml.OfficeConstants;

/**
 *  This class is an implementation of <code>OfficeDocument</code> for
 *  the SXW format.
 */
public class SxwDocument extends OfficeDocument {


    /**
     *  Constructor with arguments to set <code>name</code>.
     *
     *  @param  name  The name of the <code>Document</code>
     */
    public SxwDocument(String name) {
        super(name);
    }


    /**
     *  Constructor with arguments to set <code>name</code>, the
     *  <code>namespaceAware</code> flag, and the <code>validating</code>
     *  flag.
     *
     *  @param  name            The name of the <code>Document</code>.
     *  @param  namespaceAware  The value of the namespaceAware flag.
     *  @param  validating      The value of the validating flag.
     */
    public SxwDocument(String name, boolean namespaceAware, boolean validating) {

        super(name, namespaceAware, validating);
    }


    /**
     *  Returns the Office file extension for the SXW format.
     *
     *  @return  The Office file extension for the SXW format.
     */
    protected String getFileExtension() {
        return OfficeConstants.SXW_FILE_EXTENSION;
    }


    /**
     *  Returns the Office attribute for the SXW format.
     *
     *  @return  The Office attribute for the SXW format.
     */
    protected String getOfficeClassAttribute() {
        return OfficeConstants.SXW_TYPE;
    }

    /**
     * Method to return the MIME type of the document.
     *
     * @return  String  The document's MIME type.
     */
    protected final String getDocumentMimeType() {
        return OfficeConstants.SXW_MIME_TYPE;
    }

}

