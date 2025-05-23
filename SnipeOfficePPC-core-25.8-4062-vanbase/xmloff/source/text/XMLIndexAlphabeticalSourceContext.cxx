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


#include "XMLIndexAlphabeticalSourceContext.hxx"

#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/container/XIndexReplace.hpp>

#include <sax/tools/converter.hxx>

#include "XMLIndexTemplateContext.hxx"
#include "XMLIndexTitleTemplateContext.hxx"
#include "XMLIndexTOCStylesContext.hxx"
#include <xmloff/xmlictxt.hxx>
#include <xmloff/xmlimp.hxx>
#include <xmloff/txtimp.hxx>
#include "xmloff/xmlnmspe.hxx"
#include <xmloff/nmspmap.hxx>
#include <xmloff/xmltoken.hxx>
#include <xmloff/xmluconv.hxx>
#include <rtl/ustring.hxx>




using ::rtl::OUString;
using ::com::sun::star::beans::XPropertySet;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Any;
using ::com::sun::star::xml::sax::XAttributeList;
using ::xmloff::token::IsXMLToken;
using ::xmloff::token::XML_ALPHABETICAL_INDEX_ENTRY_TEMPLATE;
using ::xmloff::token::XML_OUTLINE_LEVEL;

const sal_Char sAPI_MainEntryCharacterStyleName[] = "MainEntryCharacterStyleName";
const sal_Char sAPI_UseAlphabeticalSeparators[] = "UseAlphabeticalSeparators";
const sal_Char sAPI_UseCombinedEntries[] = "UseCombinedEntries";
const sal_Char sAPI_IsCaseSensitive[] = "IsCaseSensitive";
const sal_Char sAPI_UseKeyAsEntry[] = "UseKeyAsEntry";
const sal_Char sAPI_UseUpperCase[] = "UseUpperCase";
const sal_Char sAPI_UseDash[] = "UseDash";
const sal_Char sAPI_UsePP[] = "UsePP";
const sal_Char sAPI_SortAlgorithm[] = "SortAlgorithm";
const sal_Char sAPI_Locale[] = "Locale";


TYPEINIT1( XMLIndexAlphabeticalSourceContext, XMLIndexSourceBaseContext );

XMLIndexAlphabeticalSourceContext::XMLIndexAlphabeticalSourceContext(
    SvXMLImport& rImport,
    sal_uInt16 nPrfx,
    const OUString& rLocalName,
    Reference<XPropertySet> & rPropSet)
:   XMLIndexSourceBaseContext(rImport, nPrfx, rLocalName, rPropSet, sal_False)
,   sMainEntryCharacterStyleName(RTL_CONSTASCII_USTRINGPARAM(sAPI_MainEntryCharacterStyleName))
,   sUseAlphabeticalSeparators(RTL_CONSTASCII_USTRINGPARAM(sAPI_UseAlphabeticalSeparators))
,   sUseCombinedEntries(RTL_CONSTASCII_USTRINGPARAM(sAPI_UseCombinedEntries))
,   sIsCaseSensitive(RTL_CONSTASCII_USTRINGPARAM(sAPI_IsCaseSensitive))
,   sUseKeyAsEntry(RTL_CONSTASCII_USTRINGPARAM(sAPI_UseKeyAsEntry))
,   sUseUpperCase(RTL_CONSTASCII_USTRINGPARAM(sAPI_UseUpperCase))
,   sUseDash(RTL_CONSTASCII_USTRINGPARAM(sAPI_UseDash))
,   sUsePP(RTL_CONSTASCII_USTRINGPARAM(sAPI_UsePP))
,   sIsCommaSeparated(RTL_CONSTASCII_USTRINGPARAM("IsCommaSeparated"))
,   sSortAlgorithm(RTL_CONSTASCII_USTRINGPARAM(sAPI_SortAlgorithm))
,   sLocale(RTL_CONSTASCII_USTRINGPARAM(sAPI_Locale))
,   bMainEntryStyleNameOK(sal_False)
,   bSeparators(sal_False)
,   bCombineEntries(sal_True)
,   bCaseSensitive(sal_True)
,   bEntry(sal_False)
,   bUpperCase(sal_False)
,   bCombineDash(sal_False)
,   bCombinePP(sal_True)
,   bCommaSeparated(sal_False)
{
}

XMLIndexAlphabeticalSourceContext::~XMLIndexAlphabeticalSourceContext()
{
}

void XMLIndexAlphabeticalSourceContext::ProcessAttribute(
    enum IndexSourceParamEnum eParam,
    const OUString& rValue)
{
    bool bTmp(false);

    switch (eParam)
    {
        case XML_TOK_INDEXSOURCE_MAIN_ENTRY_STYLE:
            {
                sMainEntryStyleName = rValue;
                OUString sDisplayStyleName = GetImport().GetStyleDisplayName(
                    XML_STYLE_FAMILY_TEXT_TEXT, sMainEntryStyleName );
                const Reference < ::com::sun::star::container::XNameContainer >&
                    rStyles = GetImport().GetTextImport()->GetTextStyles();
                bMainEntryStyleNameOK = rStyles.is() && rStyles->hasByName( sDisplayStyleName );
            }
            break;

        case XML_TOK_INDEXSOURCE_IGNORE_CASE:
            if (::sax::Converter::convertBool(bTmp, rValue))
            {
                bCaseSensitive = !bTmp;
            }
            break;

        case XML_TOK_INDEXSOURCE_SEPARATORS:
            if (::sax::Converter::convertBool(bTmp, rValue))
            {
                bSeparators = bTmp;
            }
            break;

        case XML_TOK_INDEXSOURCE_COMBINE_ENTRIES:
            if (::sax::Converter::convertBool(bTmp, rValue))
            {
                bCombineEntries = bTmp;
            }
            break;

        case XML_TOK_INDEXSOURCE_COMBINE_WITH_DASH:
            if (::sax::Converter::convertBool(bTmp, rValue))
            {
                bCombineDash = bTmp;
            }
            break;
        case XML_TOK_INDEXSOURCE_KEYS_AS_ENTRIES:
            if (::sax::Converter::convertBool(bTmp, rValue))
            {
                bEntry = bTmp;
            }
            break;

        case XML_TOK_INDEXSOURCE_COMBINE_WITH_PP:
            if (::sax::Converter::convertBool(bTmp, rValue))
            {
                bCombinePP = bTmp;
            }
            break;

        case XML_TOK_INDEXSOURCE_CAPITALIZE:
            if (::sax::Converter::convertBool(bTmp, rValue))
            {
                bUpperCase = bTmp;
            }
            break;

        case XML_TOK_INDEXSOURCE_COMMA_SEPARATED:
            if (::sax::Converter::convertBool(bTmp, rValue))
            {
                bCommaSeparated = bTmp;
            }
            break;

        case XML_TOK_INDEXSOURCE_SORT_ALGORITHM:
            sAlgorithm = rValue;
            break;
        case XML_TOK_INDEXSOURCE_LANGUAGE:
            aLocale.Language = rValue;
            break;
        case XML_TOK_INDEXSOURCE_COUNTRY:
            aLocale.Country = rValue;
            break;

        default:
            XMLIndexSourceBaseContext::ProcessAttribute(eParam, rValue);
            break;
    }
}

void XMLIndexAlphabeticalSourceContext::EndElement()
{

    Any aAny;

    if (bMainEntryStyleNameOK)
    {
        aAny <<= GetImport().GetStyleDisplayName(
                            XML_STYLE_FAMILY_TEXT_TEXT, sMainEntryStyleName );
        rIndexPropertySet->setPropertyValue(sMainEntryCharacterStyleName,aAny);
    }

    aAny.setValue(&bSeparators, ::getBooleanCppuType());
    rIndexPropertySet->setPropertyValue(sUseAlphabeticalSeparators, aAny);

    aAny.setValue(&bCombineEntries, ::getBooleanCppuType());
    rIndexPropertySet->setPropertyValue(sUseCombinedEntries, aAny);

    aAny.setValue(&bCaseSensitive, ::getBooleanCppuType());
    rIndexPropertySet->setPropertyValue(sIsCaseSensitive, aAny);

    aAny.setValue(&bEntry, ::getBooleanCppuType());
    rIndexPropertySet->setPropertyValue(sUseKeyAsEntry, aAny);

    aAny.setValue(&bUpperCase, ::getBooleanCppuType());
    rIndexPropertySet->setPropertyValue(sUseUpperCase, aAny);

    aAny.setValue(&bCombineDash, ::getBooleanCppuType());
    rIndexPropertySet->setPropertyValue(sUseDash, aAny);

    aAny.setValue(&bCombinePP, ::getBooleanCppuType());
    rIndexPropertySet->setPropertyValue(sUsePP, aAny);

    aAny.setValue(&bCommaSeparated, ::getBooleanCppuType());
    rIndexPropertySet->setPropertyValue(sIsCommaSeparated, aAny);


    if (!sAlgorithm.isEmpty())
    {
        aAny <<= sAlgorithm;
        rIndexPropertySet->setPropertyValue(sSortAlgorithm, aAny);
    }

    if ( !aLocale.Language.isEmpty()  && !aLocale.Country.isEmpty() )
    {
        aAny <<= aLocale;
        rIndexPropertySet->setPropertyValue(sLocale, aAny);
    }

    XMLIndexSourceBaseContext::EndElement();
}

SvXMLImportContext* XMLIndexAlphabeticalSourceContext::CreateChildContext(
    sal_uInt16 nPrefix,
    const OUString& rLocalName,
    const Reference<XAttributeList> & xAttrList )
{
    if ( (XML_NAMESPACE_TEXT == nPrefix) &&
         IsXMLToken( rLocalName, XML_ALPHABETICAL_INDEX_ENTRY_TEMPLATE ) )
    {
        return new XMLIndexTemplateContext(GetImport(), rIndexPropertySet,
                                           nPrefix, rLocalName,
                                           aLevelNameAlphaMap,
                                           XML_OUTLINE_LEVEL,
                                           aLevelStylePropNameAlphaMap,
                                           aAllowedTokenTypesAlpha);
    }
    else
    {
        return XMLIndexSourceBaseContext::CreateChildContext(nPrefix,
                                                             rLocalName,
                                                             xAttrList);
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
