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

#ifndef _XMLTEXTLISTITEMCONTEXT_HXX
#define _XMLTEXTLISTITEMCONTEXT_HXX

#include <xmloff/xmlictxt.hxx>

class XMLTextImportHelper;

class XMLTextListItemContext : public SvXMLImportContext
{
    XMLTextImportHelper& rTxtImport;

    sal_Int16                   nStartValue;

    // quantity of <text:list> child elements
    sal_Int16 mnSubListCount;
    // list style instance for text::style-override property
    ::com::sun::star::uno::Reference<
                ::com::sun::star::container::XIndexReplace > mxNumRulesOverride;

public:

    TYPEINFO();

    XMLTextListItemContext(
            SvXMLImport& rImport,
            XMLTextImportHelper& rTxtImp,
            const sal_uInt16 nPrfx,
            const ::rtl::OUString& rLName,
            const ::com::sun::star::uno::Reference<
            ::com::sun::star::xml::sax::XAttributeList > & xAttrList,
            const sal_Bool bIsHeader = sal_False );
    virtual ~XMLTextListItemContext();

    virtual void EndElement();

    SvXMLImportContext *CreateChildContext( sal_uInt16 nPrefix,
                 const ::rtl::OUString& rLocalName,
                 const ::com::sun::star::uno::Reference<
                     ::com::sun::star::xml::sax::XAttributeList > & xAttrList );

    sal_Bool HasStartValue() const { return -1 != nStartValue; }
    sal_Int16 GetStartValue() const { return nStartValue; }

    inline sal_Bool HasNumRulesOverride() const
    {
        return mxNumRulesOverride.is();
    }
    inline const ::com::sun::star::uno::Reference <
        ::com::sun::star::container::XIndexReplace >& GetNumRulesOverride() const
    {
        return mxNumRulesOverride;
    }
};


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
