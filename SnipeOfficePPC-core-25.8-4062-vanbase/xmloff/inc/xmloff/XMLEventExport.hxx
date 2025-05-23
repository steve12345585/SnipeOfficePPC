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

#ifndef _XMLOFF_XMLEVENTEXPORT_HXX
#define _XMLOFF_XMLEVENTEXPORT_HXX

#include "sal/config.h"
#include "xmloff/dllapi.h"
#include "sal/types.h"
#include <com/sun/star/uno/Sequence.hxx>
#include <com/sun/star/uno/Reference.hxx>
#include <rtl/ustring.hxx>
#include <xmloff/xmlevent.hxx>

#include <map>

class SvXMLExport;
namespace com { namespace sun { namespace star {
    namespace document { class XEventsSupplier; }
    namespace container { class XNameReplace; }
    namespace container { class XNameAccess; }
    namespace beans { struct PropertyValue; }
} } }

typedef ::std::map< ::rtl::OUString, XMLEventExportHandler* > HandlerMap;
typedef ::std::map< ::rtl::OUString, XMLEventName > NameMap;

/**
 * Export instances of EventsSupplier services. To use this class you
 * must fulfill two conditions:
 *
 * 1) provide a translation from the API event names to XML event
 * names
 * 2) Register XMLEventExportHandler instances for all script types
 * that you would like to export.
 *
 * The Export()-Methods all have a bUseWhitespace parameter that
 * causes the exported elements to be surrounded by whitespace, which
 * in turn causes the elements to be indented properly. By default,
 * whitespace is used, but it may not be called for in all cases (e.g
 * events attached to hyperlink within a paragraph.)
 */
class XMLOFF_DLLPUBLIC XMLEventExport
{
    const ::rtl::OUString sEventType;

    SvXMLExport& rExport;

    HandlerMap aHandlerMap;
    NameMap aNameTranslationMap;

    bool bExtNamespace;

public:
    XMLEventExport(SvXMLExport& rExport,
                   const XMLEventNameTranslation* pTranslationTable = NULL);
    ~XMLEventExport();

    /// register an EventExportHandler for a particular script type
    ///
    /// The handlers will be deleted when the object is destroyed, hence
    /// no pointers to a handler registered with AddHandler() should be
    /// held by anyone.
    void AddHandler( const ::rtl::OUString& rName,
                     XMLEventExportHandler* rHandler );

    /// register additional event names
    void AddTranslationTable( const XMLEventNameTranslation* pTransTable );

    /// export the events (calls EventExport::Export(Reference<XNameAcess>) )
    void Export( ::com::sun::star::uno::Reference<
                     ::com::sun::star::document::XEventsSupplier> & xAccess,
                sal_Bool bUseWhitespace = sal_True);

    /// export the events (calls EventExport::Export(Reference<XNameAcess>) )
    void Export( ::com::sun::star::uno::Reference<
                     ::com::sun::star::container::XNameReplace> & xAccess,
                sal_Bool bUseWhitespace = sal_True);

    /// export the events (writes <office:events> element)
    void Export( ::com::sun::star::uno::Reference<
                     ::com::sun::star::container::XNameAccess> & xAccess,
                sal_Bool bUseWhitespace = sal_True);

    /// export the events, but write <officeooo:events> element
    /// (for new file format additions)
    void ExportExt( ::com::sun::star::uno::Reference<
                     ::com::sun::star::container::XNameAccess> & xAccess,
                sal_Bool bUseWhitespace = sal_True);

    /// export a single event (writes <office:events> element)
    void ExportSingleEvent(
        ::com::sun::star::uno::Sequence<
            ::com::sun::star::beans::PropertyValue>& rEventValues,
        const ::rtl::OUString& rApiEventName,
        sal_Bool bUseWhitespace = sal_True );

private:

    /// export one event (start container-element if necessary)
    SAL_DLLPRIVATE void ExportEvent(
        ::com::sun::star::uno::Sequence<
            ::com::sun::star::beans::PropertyValue>& rEventValues,
        const XMLEventName& rXmlEventName,
        sal_Bool bUseWhitespace,
        sal_Bool& rExported);

    /// export the start element
    SAL_DLLPRIVATE void StartElement(sal_Bool bUseWhitespace);

    /// export the end element
    SAL_DLLPRIVATE void EndElement(sal_Bool bUseWhitespace);
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
