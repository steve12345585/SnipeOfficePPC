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

package org.openoffice.xmerge;

/**
 *  <p>All plug-in implementations of the <code>PluginFactory</code>
 *  interface that also support merging must also implement
 *  this interface.</p>
 *
 *  <p>Merge is useful when an <code>OfficeDocument</code>
 *  is converted to a &quot;Device&quot; <code>Document</code> format,
 *  and the &quot;Device&quot; <code>Document</code> version is modified.
 *  Those changes can be merged back into the original
 *  <code>OfficeDocument</code> with the merger.  The merger is capable
 *  of doing this even if the &quot;Device&quot; format is lossy in
 *  comparison to the <code>OfficeDocument</code> format.</p>
 *
 *  @see  PluginFactory
 *  @see  DocumentMerger
 *  @see  ConverterCapabilities
 *
 */
public interface DocumentMergerFactory {

    /**
     *  <p>Create a <code>DocumentMerger</code> object given a
     *  <code>Document</code> object.</p>
     *
     *  @param  doc  <code>Document</code> object that the created
     *               <code>DocumentMerger</code> object uses as a base
     *               <code>Document</code> for merging changes into.
     *
     *  @return  A <code>DocumentMerger</code> object or null if none
     *           exists.
     */
    public DocumentMerger createDocumentMerger(Document doc);
}

