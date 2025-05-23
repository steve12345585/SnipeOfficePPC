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

#ifndef _XMLOFF_XMLSTARBASICCONTEXTFACTORY_HXX
#define _XMLOFF_XMLSTARBASICCONTEXTFACTORY_HXX

#include <com/sun/star/uno/Reference.hxx>
#include <xmloff/xmlevent.hxx>


namespace com { namespace sun { namespace star {
    namespace xml { namespace sax { class XAttributeList; } }
} } }
namespace rtl { class OUString; }
class SvXMLImport;
class XMLEventsImportContext;


class XMLStarBasicContextFactory : public XMLEventContextFactory
{
    const ::rtl::OUString sEventType;
    const ::rtl::OUString sLibrary;
    const ::rtl::OUString sMacroName;
    const ::rtl::OUString sStarBasic;

public:
    XMLStarBasicContextFactory();
    virtual ~XMLStarBasicContextFactory();

    virtual SvXMLImportContext* CreateContext(
        SvXMLImport& rImport,               /// import context
        sal_uInt16 nPrefix,                 /// element: namespace prefix
        const ::rtl::OUString& rLocalName,  /// element: local name
        const ::com::sun::star::uno::Reference<     /// attribute list
            ::com::sun::star::xml::sax::XAttributeList> & xAttrList,
        /// the context for the enclosing <script:events> element
        XMLEventsImportContext* rEvents,
        /// the event name (as understood by the API)
        const ::rtl::OUString& rApiEventName,
        /// the event type name (as registered)
        const ::rtl::OUString& rLanguage);
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
