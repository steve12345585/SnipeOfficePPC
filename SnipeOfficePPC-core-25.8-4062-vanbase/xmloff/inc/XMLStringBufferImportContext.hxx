/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
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
#ifndef _XMLOFF_XMLSTRINGBUFFERIMPORTCONTEXT_HXX
#define _XMLOFF_XMLSTRINGBUFFERIMPORTCONTEXT_HXX


#include <xmloff/xmlictxt.hxx>

#include <xmloff/xmlimp.hxx>
#include <rtl/ustrbuf.hxx>


/**
 * Import all text into a string buffer.  Paragraph elements (<text:p>)
 * are recognized and cause a return character (0x0a) to be added.
 */
class XMLStringBufferImportContext : public SvXMLImportContext
{
    ::rtl::OUStringBuffer& rTextBuffer;

public:

    TYPEINFO();

    XMLStringBufferImportContext(
        SvXMLImport& rImport,
        sal_uInt16 nPrefix,
        const ::rtl::OUString& sLocalName,
        ::rtl::OUStringBuffer& rBuffer);

    virtual ~XMLStringBufferImportContext();

    virtual SvXMLImportContext *CreateChildContext(
        sal_uInt16 nPrefix,
        const ::rtl::OUString& rLocalName,
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::xml::sax::XAttributeList >& xAttrList );

    virtual void Characters(
        const ::rtl::OUString& rChars );

    virtual void EndElement();
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
