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

#ifndef _XMLOFF_XMLREPLACEMENTIMAGECONTEXT_HXX
#define _XMLOFF_XMLREPLACEMENTIMAGECONTEXT_HXX

#include <com/sun/star/xml/sax/XDocumentHandler.hpp>
#include <xmloff/xmlictxt.hxx>


namespace com { namespace sun { namespace star {
    namespace beans { class XPropertySet; }
    namespace io { class XOutputStream; } } } }

class XMLReplacementImageContext : public SvXMLImportContext
{
    ::com::sun::star::uno::Reference <
        ::com::sun::star::io::XOutputStream > m_xBase64Stream;
::com::sun::star::uno::Reference <
        ::com::sun::star::beans::XPropertySet > m_xPropSet;

    ::rtl::OUString m_sHRef;
    const ::rtl::OUString m_sGraphicURL;

public:

    TYPEINFO();

    XMLReplacementImageContext( SvXMLImport& rImport,
            sal_uInt16 nPrfx,
            const ::rtl::OUString& rLName,
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::xml::sax::XAttributeList > & rAttrList,
            const ::com::sun::star::uno::Reference <
                ::com::sun::star::beans::XPropertySet >& rPropSet );
    virtual ~XMLReplacementImageContext();

    virtual void EndElement();

    SvXMLImportContext *CreateChildContext( sal_uInt16 nPrefix,
                const ::rtl::OUString& rLocalName,
                 const ::com::sun::star::uno::Reference<
                    ::com::sun::star::xml::sax::XAttributeList > & xAttrList );

};

#endif  //  _XMLOFF_XMLREPLACEMENTIMAGECONTEXT_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
