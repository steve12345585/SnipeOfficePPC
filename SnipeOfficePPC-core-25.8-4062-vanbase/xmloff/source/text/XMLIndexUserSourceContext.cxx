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

#include "XMLIndexUserSourceContext.hxx"
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
#include <rtl/ustring.hxx>


using ::rtl::OUString;
using ::com::sun::star::beans::XPropertySet;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Any;
using ::com::sun::star::xml::sax::XAttributeList;
using ::xmloff::token::IsXMLToken;
using ::xmloff::token::XML_USER_INDEX_ENTRY_TEMPLATE;
using ::xmloff::token::XML_OUTLINE_LEVEL;


const sal_Char sAPI_CreateFromEmbeddedObjects[] = "CreateFromEmbeddedObjects";
const sal_Char sAPI_CreateFromGraphicObjects[] = "CreateFromGraphicObjects";
const sal_Char sAPI_CreateFromMarks[] = "CreateFromMarks";
const sal_Char sAPI_CreateFromTables[] = "CreateFromTables";
const sal_Char sAPI_CreateFromTextFrames[] = "CreateFromTextFrames";
const sal_Char sAPI_UseLevelFromSource[] = "UseLevelFromSource";
const sal_Char sAPI_CreateFromLevelParagraphStyles[] = "CreateFromLevelParagraphStyles";
const sal_Char sAPI_UserIndexName[] = "UserIndexName";


TYPEINIT1(XMLIndexUserSourceContext, XMLIndexSourceBaseContext);


XMLIndexUserSourceContext::XMLIndexUserSourceContext(
    SvXMLImport& rImport,
    sal_uInt16 nPrfx,
    const OUString& rLocalName,
    Reference<XPropertySet> & rPropSet) :
        XMLIndexSourceBaseContext(rImport, nPrfx, rLocalName,
                                  rPropSet, sal_True),
        sCreateFromEmbeddedObjects(RTL_CONSTASCII_USTRINGPARAM(
            sAPI_CreateFromEmbeddedObjects)),
        sCreateFromGraphicObjects(RTL_CONSTASCII_USTRINGPARAM(
            sAPI_CreateFromGraphicObjects)),
        sCreateFromMarks(RTL_CONSTASCII_USTRINGPARAM(sAPI_CreateFromMarks)),
        sCreateFromTables(RTL_CONSTASCII_USTRINGPARAM(sAPI_CreateFromTables)),
        sCreateFromTextFrames(RTL_CONSTASCII_USTRINGPARAM(
            sAPI_CreateFromTextFrames)),
        sUseLevelFromSource(RTL_CONSTASCII_USTRINGPARAM(
            sAPI_UseLevelFromSource)),
        sCreateFromLevelParagraphStyles(RTL_CONSTASCII_USTRINGPARAM(
            sAPI_CreateFromLevelParagraphStyles)),
        sUserIndexName(RTL_CONSTASCII_USTRINGPARAM(sAPI_UserIndexName)),
        bUseObjects(sal_False),
        bUseGraphic(sal_False),
        bUseMarks(sal_False),
        bUseTables(sal_False),
        bUseFrames(sal_False),
        bUseLevelFromSource(sal_False),
        bUseLevelParagraphStyles(sal_False)
{
}

XMLIndexUserSourceContext::~XMLIndexUserSourceContext()
{
}

void XMLIndexUserSourceContext::ProcessAttribute(
    enum IndexSourceParamEnum eParam,
    const OUString& rValue)
{
    bool bTmp(false);

    switch (eParam)
    {
        case XML_TOK_INDEXSOURCE_USE_INDEX_MARKS:
            if (::sax::Converter::convertBool(bTmp, rValue))
            {
                bUseMarks = bTmp;
            }
            break;

        case XML_TOK_INDEXSOURCE_USE_OBJECTS:
            if (::sax::Converter::convertBool(bTmp, rValue))
            {
                bUseObjects = bTmp;
            }
            break;

        case XML_TOK_INDEXSOURCE_USE_GRAPHICS:
            if (::sax::Converter::convertBool(bTmp, rValue))
            {
                bUseGraphic = bTmp;
            }
            break;

        case XML_TOK_INDEXSOURCE_USE_TABLES:
            if (::sax::Converter::convertBool(bTmp, rValue))
            {
                bUseTables = bTmp;
            }
            break;

        case XML_TOK_INDEXSOURCE_USE_FRAMES:
            if (::sax::Converter::convertBool(bTmp, rValue))
            {
                bUseFrames = bTmp;
            }
            break;

        case XML_TOK_INDEXSOURCE_COPY_OUTLINE_LEVELS:
            if (::sax::Converter::convertBool(bTmp, rValue))
            {
                bUseLevelFromSource = bTmp;
            }
            break;

        case XML_TOK_INDEXSOURCE_USE_INDEX_SOURCE_STYLES:
            if (::sax::Converter::convertBool(bTmp, rValue))
            {
                bUseLevelParagraphStyles = bTmp;
            }
            break;

        case XML_TOK_INDEXSOURCE_USER_INDEX_NAME:
            sIndexName = rValue;
            break;

        default:
            XMLIndexSourceBaseContext::ProcessAttribute(eParam, rValue);
            break;
    }
}


void XMLIndexUserSourceContext::EndElement()
{
    Any aAny;

    aAny.setValue(&bUseObjects, ::getBooleanCppuType());
    rIndexPropertySet->setPropertyValue(sCreateFromEmbeddedObjects, aAny);

    aAny.setValue(&bUseGraphic, ::getBooleanCppuType());
    rIndexPropertySet->setPropertyValue(sCreateFromGraphicObjects, aAny);

    aAny.setValue(&bUseLevelFromSource, ::getBooleanCppuType());
    rIndexPropertySet->setPropertyValue(sUseLevelFromSource, aAny);

    aAny.setValue(&bUseMarks, ::getBooleanCppuType());
    rIndexPropertySet->setPropertyValue(sCreateFromMarks, aAny);

    aAny.setValue(&bUseTables, ::getBooleanCppuType());
    rIndexPropertySet->setPropertyValue(sCreateFromTables, aAny);

    aAny.setValue(&bUseFrames, ::getBooleanCppuType());
    rIndexPropertySet->setPropertyValue(sCreateFromTextFrames, aAny);

    aAny.setValue(&bUseLevelParagraphStyles, ::getBooleanCppuType());
    rIndexPropertySet->setPropertyValue(sCreateFromLevelParagraphStyles, aAny);

    if( !sIndexName.isEmpty() )
    {
        aAny <<= sIndexName;
        rIndexPropertySet->setPropertyValue(sUserIndexName, aAny);
    }

    XMLIndexSourceBaseContext::EndElement();
}


SvXMLImportContext* XMLIndexUserSourceContext::CreateChildContext(
    sal_uInt16 nPrefix,
    const OUString& rLocalName,
    const Reference<XAttributeList> & xAttrList )
{
    if ( (XML_NAMESPACE_TEXT == nPrefix) &&
         (IsXMLToken(rLocalName, XML_USER_INDEX_ENTRY_TEMPLATE)) )
    {
        return new XMLIndexTemplateContext(GetImport(), rIndexPropertySet,
                                           nPrefix, rLocalName,
                                           aLevelNameTOCMap,
                                           XML_OUTLINE_LEVEL,
                                           aLevelStylePropNameTOCMap,
                                           aAllowedTokenTypesUser);
    }
    else
    {
        return XMLIndexSourceBaseContext::CreateChildContext(nPrefix,
                                                             rLocalName,
                                                             xAttrList);
    }

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
