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
#ifndef _XMLOFF_SYMBOLIMAGECONTEXT_HXX_
#define _XMLOFF_SYMBOLIMAGECONTEXT_HXX_

#include "XMLElementPropertyContext.hxx"

namespace com { namespace sun { namespace star {
    namespace io { class XOutputStream; }
} } }

class XMLSymbolImageContext : public XMLElementPropertyContext
{
public:
    TYPEINFO();

    XMLSymbolImageContext( SvXMLImport& rImport, sal_uInt16 nPrfx,
                           const ::rtl::OUString& rLName,
                           const XMLPropertyState& rProp,
                           ::std::vector< XMLPropertyState > &rProps );
    virtual ~XMLSymbolImageContext();

    virtual void StartElement( const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList );
    virtual SvXMLImportContext *CreateChildContext(
        sal_uInt16 nPrefix,
        const ::rtl::OUString& rLocalName,
        const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList );
    virtual void EndElement();

private:
    ::rtl::OUString msURL;
    ::com::sun::star::uno::Reference < ::com::sun::star::io::XOutputStream > mxBase64Stream;
};

#endif  // _XMLOFF_SYMBOLIMAGECONTEXT_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
