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

#include "xmlsorti.hxx"
#include "xmlimprt.hxx"
#include "docuno.hxx"
#include "convuno.hxx"
#include "XMLConverter.hxx"
#include "unonames.hxx"
#include "rangeutl.hxx"

#include <xmloff/xmltkmap.hxx>
#include <xmloff/nmspmap.hxx>
#include <comphelper/extract.hxx>
#include <xmloff/xmltoken.hxx>

#define SC_USERLIST "UserList"

using namespace com::sun::star;
using namespace xmloff::token;

//------------------------------------------------------------------

ScXMLSortContext::ScXMLSortContext( ScXMLImport& rImport,
                                      sal_uInt16 nPrfx,
                                      const ::rtl::OUString& rLName,
                                      const ::com::sun::star::uno::Reference<
                                      ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
                                        ScXMLDatabaseRangeContext* pTempDatabaseRangeContext) :
    SvXMLImportContext( rImport, nPrfx, rLName ),
    pDatabaseRangeContext(pTempDatabaseRangeContext),
    sCountry(),
    sLanguage(),
    sAlgorithm(),
    nUserListIndex(0),
    bCopyOutputData(false),
    bBindFormatsToContent(true),
    bIsCaseSensitive(false),
    bEnabledUserList(false)
{
    sal_Int16 nAttrCount(xAttrList.is() ? xAttrList->getLength() : 0);
    const SvXMLTokenMap& rAttrTokenMap(GetScImport().GetSortAttrTokenMap());
    for( sal_Int16 i=0; i < nAttrCount; ++i )
    {
        const rtl::OUString& sAttrName(xAttrList->getNameByIndex( i ));
        rtl::OUString aLocalName;
        sal_uInt16 nPrefix(GetScImport().GetNamespaceMap().GetKeyByAttrName(
                                            sAttrName, &aLocalName ));
        const rtl::OUString& sValue(xAttrList->getValueByIndex( i ));

        switch( rAttrTokenMap.Get( nPrefix, aLocalName ) )
        {
            case XML_TOK_SORT_ATTR_BIND_STYLES_TO_CONTENT :
            {
                bBindFormatsToContent = IsXMLToken(sValue, XML_TRUE);
            }
            break;
            case XML_TOK_SORT_ATTR_TARGET_RANGE_ADDRESS :
            {
                ScRange aScRange;
                sal_Int32 nOffset(0);
                if (ScRangeStringConverter::GetRangeFromString( aScRange, sValue, GetScImport().GetDocument(), ::formula::FormulaGrammar::CONV_OOO, nOffset ))
                {
                    ScUnoConversion::FillApiAddress( aOutputPosition, aScRange.aStart );
                    bCopyOutputData = true;
                }
            }
            break;
            case XML_TOK_SORT_ATTR_CASE_SENSITIVE :
            {
                bIsCaseSensitive = IsXMLToken(sValue, XML_TRUE);
            }
            break;
            case XML_TOK_SORT_ATTR_LANGUAGE :
                sLanguage = sValue;
            break;
            case XML_TOK_SORT_ATTR_COUNTRY :
                sCountry = sValue;
            break;
            case XML_TOK_SORT_ATTR_ALGORITHM :
                sAlgorithm = sValue;
            break;
        }
    }
}

ScXMLSortContext::~ScXMLSortContext()
{
}

SvXMLImportContext *ScXMLSortContext::CreateChildContext( sal_uInt16 nPrefix,
                                            const ::rtl::OUString& rLName,
                                            const ::com::sun::star::uno::Reference<
                                          ::com::sun::star::xml::sax::XAttributeList>& xAttrList )
{
    SvXMLImportContext *pContext(0);

    const SvXMLTokenMap& rTokenMap(GetScImport().GetSortElemTokenMap());
    switch( rTokenMap.Get( nPrefix, rLName ) )
    {
        case XML_TOK_SORT_SORT_BY :
        {
            pContext = new ScXMLSortByContext( GetScImport(), nPrefix,
                                                          rLName, xAttrList, this);
        }
        break;
    }

    if( !pContext )
        pContext = new SvXMLImportContext( GetImport(), nPrefix, rLName );

    return pContext;
}

void ScXMLSortContext::EndElement()
{
    sal_Int32 nLangLength(sLanguage.getLength());
    sal_Int32 nCountryLength(sCountry.getLength());
    sal_Int32 nAlgoLength(sAlgorithm.getLength());
    sal_uInt8 i (0);
    if (nLangLength || nCountryLength)
        ++i;
    if (nAlgoLength)
        ++i;
    uno::Sequence <beans::PropertyValue> aSortDescriptor(7 + i);
    aSortDescriptor[0].Name = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_BINDFMT));
    aSortDescriptor[0].Value = ::cppu::bool2any(bBindFormatsToContent);
    aSortDescriptor[1].Name = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_COPYOUT));
    aSortDescriptor[1].Value = ::cppu::bool2any(bCopyOutputData);
    aSortDescriptor[2].Name = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_ISCASE));
    aSortDescriptor[2].Value = ::cppu::bool2any(bIsCaseSensitive);
    aSortDescriptor[3].Name = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_ISULIST));
    aSortDescriptor[3].Value = ::cppu::bool2any(bEnabledUserList);
    aSortDescriptor[4].Name = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_OUTPOS));
    aSortDescriptor[4].Value <<= aOutputPosition;
    aSortDescriptor[5].Name = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_UINDEX));
    aSortDescriptor[5].Value <<= nUserListIndex;
    aSortDescriptor[6].Name = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_SORTFLD));
    aSortDescriptor[6].Value <<= aSortFields;
    if (nLangLength || nCountryLength)
    {
        lang::Locale aLocale;
        aLocale.Language = sLanguage;
        aLocale.Country = sCountry;
        aSortDescriptor[7].Name = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_COLLLOC));
        aSortDescriptor[7].Value <<= aLocale;
    }
    if (nAlgoLength)
    {
        aSortDescriptor[6 + i].Name = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_COLLALG));
        aSortDescriptor[6 + i].Value <<= sAlgorithm;
    }
    pDatabaseRangeContext->SetSortSequence(aSortDescriptor);
}

void ScXMLSortContext::AddSortField(const rtl::OUString& sFieldNumber, const rtl::OUString& sDataType, const rtl::OUString& sOrder)
{
    util::SortField aSortField;
    aSortField.Field = sFieldNumber.toInt32();
    if (IsXMLToken(sOrder, XML_ASCENDING))
        aSortField.SortAscending = true;
    else
        aSortField.SortAscending = false;
    if (sDataType.getLength() > 8)
    {
        rtl::OUString sTemp = sDataType.copy(0, 8);
        if (sTemp.compareToAscii(SC_USERLIST) == 0)
        {
            bEnabledUserList = true;
            sTemp = sDataType.copy(8);
            nUserListIndex = static_cast<sal_Int16>(sTemp.toInt32());
        }
        else
        {
            if (IsXMLToken(sDataType, XML_AUTOMATIC))
                aSortField.FieldType = util::SortFieldType_AUTOMATIC;
        }
    }
    else
    {
        if (IsXMLToken(sDataType, XML_TEXT))
            aSortField.FieldType = util::SortFieldType_ALPHANUMERIC;
        else if (IsXMLToken(sDataType, XML_NUMBER))
            aSortField.FieldType = util::SortFieldType_NUMERIC;
    }
    aSortFields.realloc(aSortFields.getLength() + 1);
    aSortFields[aSortFields.getLength() - 1] = aSortField;
}

ScXMLSortByContext::ScXMLSortByContext( ScXMLImport& rImport,
                                      sal_uInt16 nPrfx,
                                      const ::rtl::OUString& rLName,
                                      const ::com::sun::star::uno::Reference<
                                      ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
                                        ScXMLSortContext* pTempSortContext) :
    SvXMLImportContext( rImport, nPrfx, rLName ),
    pSortContext(pTempSortContext),
    sDataType(GetXMLToken(XML_AUTOMATIC)),
    sOrder(GetXMLToken(XML_ASCENDING))
{
    sal_Int16 nAttrCount(xAttrList.is() ? xAttrList->getLength() : 0);
    const SvXMLTokenMap& rAttrTokenMap(GetScImport().GetSortSortByAttrTokenMap());
    for( sal_Int16 i=0; i < nAttrCount; ++i )
    {
        const rtl::OUString& sAttrName(xAttrList->getNameByIndex( i ));
        rtl::OUString aLocalName;
        sal_uInt16 nPrefix(GetScImport().GetNamespaceMap().GetKeyByAttrName(
                                            sAttrName, &aLocalName ));
        const rtl::OUString& sValue(xAttrList->getValueByIndex( i ));

        switch( rAttrTokenMap.Get( nPrefix, aLocalName ) )
        {
            case XML_TOK_SORT_BY_ATTR_FIELD_NUMBER :
            {
                sFieldNumber = sValue;
            }
            break;
            case XML_TOK_SORT_BY_ATTR_DATA_TYPE :
            {
                sDataType = sValue;
            }
            break;
            case XML_TOK_SORT_BY_ATTR_ORDER :
            {
                sOrder = sValue;
            }
            break;
        }
    }
}

ScXMLSortByContext::~ScXMLSortByContext()
{
}

SvXMLImportContext *ScXMLSortByContext::CreateChildContext( sal_uInt16 nPrefix,
                                            const ::rtl::OUString& rLName,
                                            const ::com::sun::star::uno::Reference<
                                        ::com::sun::star::xml::sax::XAttributeList>& /* xAttrList */ )
{
    return new SvXMLImportContext( GetImport(), nPrefix, rLName );
}

void ScXMLSortByContext::EndElement()
{
    pSortContext->AddSortField(sFieldNumber, sDataType, sOrder);
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
