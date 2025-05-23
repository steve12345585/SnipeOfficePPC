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

#ifndef _XMLOFF_XMLINDEXBIBLIOGRAPHYCONFIGURATIONCONTEXT_HXX_
#define _XMLOFF_XMLINDEXBIBLIOGRAPHYCONFIGURATIONCONTEXT_HXX_

#include <xmloff/xmlstyle.hxx>
#include <com/sun/star/uno/Reference.h>
#include <com/sun/star/uno/Sequence.h>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/lang/Locale.hpp>
#include <rtl/ustring.hxx>


#include <vector>

namespace com { namespace sun { namespace star {
    namespace xml { namespace sax { class XAttributeList; } }
} } }


/**
 * Import bibliography configuration.
 *
 * Little cheat: Cover all child elements in CreateChildContext.
 */
class XMLIndexBibliographyConfigurationContext : public SvXMLStyleContext
{
    const ::rtl::OUString sFieldMaster_Bibliography;
    const ::rtl::OUString sBracketBefore;
    const ::rtl::OUString sBracketAfter;
    const ::rtl::OUString sIsNumberEntries;
    const ::rtl::OUString sIsSortByPosition;
    const ::rtl::OUString sSortKeys;
    const ::rtl::OUString sSortKey;
    const ::rtl::OUString sIsSortAscending;
    const ::rtl::OUString sSortAlgorithm;
    const ::rtl::OUString sLocale;

    ::rtl::OUString sSuffix;
    ::rtl::OUString sPrefix;
    ::rtl::OUString sAlgorithm;
    ::com::sun::star::lang::Locale aLocale;
    sal_Bool bNumberedEntries;
    sal_Bool bSortByPosition;

    ::std::vector< ::com::sun::star::uno::Sequence<
                        ::com::sun::star::beans::PropertyValue> > aSortKeys;

public:

    TYPEINFO();

    XMLIndexBibliographyConfigurationContext(
        SvXMLImport& rImport,
        sal_uInt16 nPrfx,
        const ::rtl::OUString& rLocalName,
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::xml::sax::XAttributeList> & xAttrList);

    ~XMLIndexBibliographyConfigurationContext();

protected:

    virtual void StartElement(
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::xml::sax::XAttributeList> & xAttrList);

    virtual void CreateAndInsert( sal_Bool bOverwrite );

    virtual SvXMLImportContext *CreateChildContext(
        sal_uInt16 nPrefix,
        const ::rtl::OUString& rLocalName,
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::xml::sax::XAttributeList> & xAttrList );

    void ProcessAttribute(
        sal_uInt16 nPrefix,
        const ::rtl::OUString sLocalName,
        const ::rtl::OUString sValue);
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
