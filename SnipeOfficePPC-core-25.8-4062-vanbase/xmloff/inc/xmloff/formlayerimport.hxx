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

#ifndef _XMLOFF_FORMLAYERIMPORT_HXX_
#define _XMLOFF_FORMLAYERIMPORT_HXX_

#include "sal/config.h"
#include "xmloff/dllapi.h"
#include <com/sun/star/drawing/XDrawPage.hpp>
#include <com/sun/star/xml/sax/XAttributeList.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <rtl/ref.hxx>
#include <salhelper/simplereferenceobject.hxx>
#include <xmloff/xmlimppr.hxx>

class SvXMLImport;
class SvXMLImportContext;
class SvXMLStylesContext;
class XMLPropStyleContext;

//.........................................................................
namespace xmloff
{
//.........................................................................

    //=====================================================================
    //= OFormLayerXMLImport
    //=====================================================================
    class OFormLayerXMLImport_Impl;
    /** allows you to import a &lt;form:form&gt; element
    */
    class XMLOFF_DLLPUBLIC OFormLayerXMLImport
                :public ::salhelper::SimpleReferenceObject
    {
        OFormLayerXMLImport_Impl*   m_pImpl;

    public:
        OFormLayerXMLImport(SvXMLImport& _rImporter);
        ~OFormLayerXMLImport();

        /** start importing the forms of the given page

            <p>starting the import of a new page (by using this method) invalidates the current page position.</p>

            @see endPage
        */
        void startPage(
            const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XDrawPage >& _rxDrawPage);

        /** creates an import context for the office:forms element
        */
        SvXMLImportContext* createOfficeFormsContext(
            SvXMLImport& _rImport,
            sal_uInt16 _nPrefix,
            const rtl::OUString& _rLocalName);

        /** create an <type>SvXMLImportContext</type> instance which is able to import the &lt;form:form&gt;
            element.

            <p>You must have called <method>startPage</method> before you can create such an import context.</p>

            @see startPage
            @see endPage
        */
        SvXMLImportContext* createContext(
            const sal_uInt16 _nPrefix,
            const rtl::OUString& _rLocalName,
            const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& _rxAttribs);

        /** lookup a control given by id.

            <p>The control must be part of the page which is currently beeing imported.</p>

            <p>(And, of course, the control must have been imported already at the moment you call this.)</p>

            @see startPage
        */
        ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >
                lookupControl(const ::rtl::OUString& _rId);

        /** end importing the forms of the current page

            <p>You must call this method if you want the import to be finished correctly. Without calling
            it, you will lose information.</p>

            @see startPage
        */
        void endPage();

        /** announces the auto-style context to the form importer
        */
        void setAutoStyleContext(SvXMLStylesContext* _pNewContext);

        /** sets the given number style on the given control
            @param _rxControlModel
                the control model which's style is to be set
            @param _rControlNumerStyleName
                the style name for the control's number style
        */
        void applyControlNumberStyle(
            const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxControlModel,
            const ::rtl::OUString& _rControlNumerStyleName
        );

        /** to be called when the document has been completely imported

            <p>For some documents (currently: only some spreadsheet documents) it's necessary
            do to a post processing, since not all information from the file can be processed
            if the document is not completed, yet.</p>
        */
        void documentDone( );
    };

//.........................................................................
}   // namespace xmloff
//.........................................................................

#endif // _XMLOFF_FORMLAYERIMPORT_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
