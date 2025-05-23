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

#include "XMLTextMarkImportContext.hxx"


#include <rtl/ustring.hxx>
#include <tools/debug.hxx>
#include <xmloff/xmluconv.hxx>
#include <xmloff/xmltoken.hxx>
#include <xmloff/xmlimp.hxx>
#include <xmloff/nmspmap.hxx>
#include "xmloff/xmlnmspe.hxx"
#include <xmloff/odffields.hxx>
#include <com/sun/star/xml/sax/XAttributeList.hpp>
#include <com/sun/star/text/XTextContent.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/container/XNamed.hpp>
#include <com/sun/star/rdf/XMetadatable.hpp>

#include <com/sun/star/text/XFormField.hpp>

#include "RDFaImportHelper.hxx"


using ::rtl::OUString;
using ::rtl::OUStringBuffer;

using namespace ::com::sun::star;
using namespace ::com::sun::star::text;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::xml::sax;
using namespace ::xmloff::token;

using rtl::OUString;


XMLFieldParamImportContext::XMLFieldParamImportContext(
    SvXMLImport& rImport,
    XMLTextImportHelper& rHlp,
    sal_uInt16 nPrefix,
    const OUString& rLocalName ) :
        SvXMLImportContext(rImport, nPrefix, rLocalName),
        rHelper(rHlp)
{
}


void XMLFieldParamImportContext::StartElement(const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList> & xAttrList)
{
    SvXMLImport& rImport = GetImport();
    ::rtl::OUString sName;
    ::rtl::OUString sValue;

    sal_Int16 nLength = xAttrList->getLength();
    for(sal_Int16 nAttr = 0; nAttr < nLength; nAttr++)
    {
        OUString sLocalName;
        sal_uInt16 nPrefix = rImport.GetNamespaceMap().
            GetKeyByAttrName( xAttrList->getNameByIndex(nAttr),
                              &sLocalName );

        if ( (XML_NAMESPACE_FIELD == nPrefix) &&
             IsXMLToken(sLocalName, XML_NAME)   )
        {
            sName = xAttrList->getValueByIndex(nAttr);
        }
        if ( (XML_NAMESPACE_FIELD == nPrefix) &&
             IsXMLToken(sLocalName, XML_VALUE)   )
        {
            sValue = xAttrList->getValueByIndex(nAttr);
        }
    }
    if (rHelper.hasCurrentFieldCtx() && !sName.isEmpty()) {
        rHelper.addFieldParam(sName, sValue);
    }
}


TYPEINIT1( XMLTextMarkImportContext, SvXMLImportContext);

XMLTextMarkImportContext::XMLTextMarkImportContext(
    SvXMLImport& rImport,
    XMLTextImportHelper& rHlp,
    sal_uInt16 nPrefix,
    const OUString& rLocalName )
    : SvXMLImportContext(rImport, nPrefix, rLocalName)
    , m_rHelper(rHlp)
    , m_bHaveAbout(false)
{
}

enum lcl_MarkType { TypeReference, TypeReferenceStart, TypeReferenceEnd,
                    TypeBookmark, TypeBookmarkStart, TypeBookmarkEnd,
                    TypeFieldmark, TypeFieldmarkStart, TypeFieldmarkEnd
                  };

static SvXMLEnumMapEntry const lcl_aMarkTypeMap[] =
{
    { XML_REFERENCE_MARK,           TypeReference },
    { XML_REFERENCE_MARK_START,     TypeReferenceStart },
    { XML_REFERENCE_MARK_END,       TypeReferenceEnd },
    { XML_BOOKMARK,                 TypeBookmark },
    { XML_BOOKMARK_START,           TypeBookmarkStart },
    { XML_BOOKMARK_END,             TypeBookmarkEnd },
    { XML_FIELDMARK,                TypeFieldmark },
    { XML_FIELDMARK_START,          TypeFieldmarkStart },
    { XML_FIELDMARK_END,            TypeFieldmarkEnd },
    { XML_TOKEN_INVALID,            0 },
};


static const char *lcl_getFormFieldmarkName(rtl::OUString &name)
{
    static const char sCheckbox[]=ODF_FORMCHECKBOX;
    static const char sFormDropDown[]=ODF_FORMDROPDOWN;
    if (name.compareToAscii("msoffice.field.FORMCHECKBOX")==0 ||
            name.compareToAscii("ecma.office-open-xml.field.FORMCHECKBOX")==0)
        return sCheckbox;
    else if (name.compareToAscii(ODF_FORMCHECKBOX)==0)
        return sCheckbox;
    if (name.compareToAscii(ODF_FORMDROPDOWN)==0 ||
            name.compareToAscii("ecma.office-open-xml.field.FORMDROPDOWN")==0)
        return sFormDropDown;
    else
        return NULL;
}

static rtl::OUString lcl_getFieldmarkName(rtl::OUString &name)
{
    static const char sFormtext[]=ODF_FORMTEXT;
    if (name.compareToAscii("msoffice.field.FORMTEXT")==0 ||
            name.compareToAscii("ecma.office-open-xml.field.FORMTEXT")==0)
        return rtl::OUString::createFromAscii(sFormtext);
    else if (name.compareToAscii(ODF_FORMTEXT)==0)
        return rtl::OUString::createFromAscii(sFormtext);
    else
        return name;
}


void XMLTextMarkImportContext::StartElement(
    const Reference<XAttributeList> & xAttrList)
{
    if (!FindName(GetImport(), xAttrList))
    {
        m_sBookmarkName = OUString();
    }

    if (IsXMLToken(GetLocalName(), XML_FIELDMARK_END))
    {
        m_sBookmarkName = m_rHelper.FindActiveBookmarkName();
    }

    if (IsXMLToken(GetLocalName(), XML_FIELDMARK_START) || IsXMLToken(GetLocalName(), XML_FIELDMARK))
    {
        if (m_sBookmarkName.isEmpty())
        {
            m_sBookmarkName = ::rtl::OUString("Unknown");
        }
        m_rHelper.pushFieldCtx( m_sBookmarkName, m_sFieldName );
    }
}

void XMLTextMarkImportContext::EndElement()
{
    SvXMLImportContext::EndElement();

    static const OUString sAPI_reference_mark( "com.sun.star.text.ReferenceMark");
    static const OUString sAPI_bookmark( "com.sun.star.text.Bookmark");
    static const OUString sAPI_fieldmark( "com.sun.star.text.Fieldmark");
    static const OUString sAPI_formfieldmark( "com.sun.star.text.FormFieldmark");

    if (!m_sBookmarkName.isEmpty())
    {
        sal_uInt16 nTmp;
        if (SvXMLUnitConverter::convertEnum(nTmp, GetLocalName(),
                                            lcl_aMarkTypeMap))
        {
            switch ((lcl_MarkType)nTmp)
            {
                case TypeReference:
                    // export point reference mark
                    CreateAndInsertMark(GetImport(),
                        sAPI_reference_mark,
                        m_sBookmarkName,
                        m_rHelper.GetCursorAsRange()->getStart(),
                        ::rtl::OUString());
                    break;

                case TypeFieldmark:
                case TypeBookmark:
                    {
                        const char *formFieldmarkName=lcl_getFormFieldmarkName(m_sFieldName);
                        bool bImportAsField=((lcl_MarkType)nTmp==TypeFieldmark && formFieldmarkName!=NULL); //@TODO handle abbreviation cases..
                        // export point bookmark
                        const Reference<XInterface> xContent(
                            CreateAndInsertMark(GetImport(),
                                        (bImportAsField?sAPI_formfieldmark:sAPI_bookmark),
                                m_sBookmarkName,
                                m_rHelper.GetCursorAsRange()->getStart(),
                                m_sXmlId) );
                        if ((lcl_MarkType)nTmp==TypeFieldmark) {
                            if (xContent.is() && bImportAsField) {
                                // setup fieldmark...
                                Reference< ::com::sun::star::text::XFormField> xFormField(xContent, UNO_QUERY);
                                xFormField->setFieldType(rtl::OUString::createFromAscii(formFieldmarkName));
                                if (xFormField.is() && m_rHelper.hasCurrentFieldCtx()) {
                                    m_rHelper.setCurrentFieldParamsTo(xFormField);
                                }
                            }
                            m_rHelper.popFieldCtx();
                        }
                    }
                    break;

                case TypeFieldmarkStart:
                case TypeBookmarkStart:
                    // save XTextRange for later construction of bookmark
                    {
                        ::boost::shared_ptr< ::xmloff::ParsedRDFaAttributes >
                            pRDFaAttributes;
                        if (m_bHaveAbout && (TypeBookmarkStart
                                == static_cast<lcl_MarkType>(nTmp)))
                        {
                            pRDFaAttributes =
                                GetImport().GetRDFaImportHelper().ParseRDFa(
                                    m_sAbout, m_sProperty,
                                    m_sContent, m_sDatatype);
                        }
                        m_rHelper.InsertBookmarkStartRange(
                            m_sBookmarkName,
                            m_rHelper.GetCursorAsRange()->getStart(),
                            m_sXmlId, pRDFaAttributes);
                    }
                    break;

                case TypeFieldmarkEnd:
                case TypeBookmarkEnd:
                {
                    // get old range, and construct
                    Reference<XTextRange> xStartRange;
                    ::boost::shared_ptr< ::xmloff::ParsedRDFaAttributes >
                        pRDFaAttributes;
                    if (m_rHelper.FindAndRemoveBookmarkStartRange(
                            m_sBookmarkName, xStartRange,
                            m_sXmlId, pRDFaAttributes))
                    {
                        Reference<XTextRange> xEndRange(
                            m_rHelper.GetCursorAsRange()->getStart());

                        // check if beginning and end are in same XText
                        if (xStartRange->getText() == xEndRange->getText())
                        {
                            // create range for insertion
                            Reference<XTextCursor> xInsertionCursor =
                                m_rHelper.GetText()->createTextCursorByRange(
                                    xEndRange);
                            try {
                            xInsertionCursor->gotoRange(xStartRange, sal_True);
                            } catch (uno::Exception&) {
                                OSL_ENSURE(false,
                                    "cannot go to end position of bookmark");
                            }

                            //DBG_ASSERT(! xInsertionCursor->isCollapsed(),
                            //              "we want no point mark");
                            // can't assert, because someone could
                            // create a file with subsequence
                            // start/end elements

                            Reference<XTextRange> xInsertionRange(
                                xInsertionCursor, UNO_QUERY);

                            bool bImportAsField=((lcl_MarkType)nTmp==TypeFieldmarkEnd && m_rHelper.hasCurrentFieldCtx());

                            // insert reference
                            const Reference<XInterface> xContent(
                                CreateAndInsertMark(GetImport(),
                                                (bImportAsField?sAPI_fieldmark:sAPI_bookmark),
                                    m_sBookmarkName,
                                    xInsertionRange,
                                    m_sXmlId) );
                            if (pRDFaAttributes)
                            {
                                const Reference<rdf::XMetadatable>
                                    xMeta(xContent, UNO_QUERY);
                                GetImport().GetRDFaImportHelper().AddRDFa(
                                    xMeta, pRDFaAttributes);
                            }

                            if ((lcl_MarkType)nTmp==TypeFieldmarkEnd) {
                                if (xContent.is() && bImportAsField) {
                                    // setup fieldmark...
                                    Reference< ::com::sun::star::text::XFormField> xFormField(xContent, UNO_QUERY);
                                    if (xFormField.is() && m_rHelper.hasCurrentFieldCtx()) {
                                        rtl::OUString givenTypeName=m_rHelper.getCurrentFieldType();
                                        rtl::OUString fieldmarkTypeName=lcl_getFieldmarkName(givenTypeName);

                                        xFormField->setFieldType(fieldmarkTypeName);
                                        m_rHelper.setCurrentFieldParamsTo(xFormField);
                                    }
                                }
                                m_rHelper.popFieldCtx();
                            }
                        }
                        // else: beginning/end in different XText -> ignore!
                    }
                    // else: no start found -> ignore!
                    break;
                }

                case TypeReferenceStart:
                case TypeReferenceEnd:
                    OSL_FAIL("reference start/end are handled in txtparai !");
                    break;

                default:
                    OSL_FAIL("unknown mark type");
                    break;
            }
        }
    }
}

SvXMLImportContext *XMLTextMarkImportContext::CreateChildContext( sal_uInt16 nPrefix,
                                        const ::rtl::OUString& rLocalName,
                                        const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >&  )
{
    return new XMLFieldParamImportContext(GetImport(), m_rHelper,
                nPrefix, rLocalName);
}


Reference<XTextContent> XMLTextMarkImportContext::CreateAndInsertMark(
    SvXMLImport& rImport,
    const OUString& sServiceName,
    const OUString& sMarkName,
    const Reference<XTextRange> & rRange,
    const OUString& i_rXmlId)
{
    // create mark
    const Reference<XMultiServiceFactory> xFactory(rImport.GetModel(),
        UNO_QUERY);
    Reference<XInterface> xIfc;

    if (xFactory.is())
    {
        xIfc = xFactory->createInstance(sServiceName);

        if (!xIfc.is())
        {
            OSL_FAIL("CreateAndInsertMark: cannot create service?");
            return 0;
        }

        // set name (unless there is no name (text:meta))
        const Reference<XNamed> xNamed(xIfc, UNO_QUERY);
        if (xNamed.is())
        {
            xNamed->setName(sMarkName);
        }
        else
        {
            if (!sMarkName.isEmpty())
            {
                OSL_FAIL("name given, but XNamed not supported?");
                return 0;
            }
        }

        // cast to XTextContent and attach to document
        const Reference<XTextContent> xTextContent(xIfc, UNO_QUERY);
        if (xTextContent.is())
        {
            try
            {
                // if inserting marks, bAbsorb==sal_False will cause
                // collapsing of the given XTextRange.
                rImport.GetTextImport()->GetText()->insertTextContent(rRange,
                    xTextContent, sal_True);

                // xml:id for RDF metadata -- after insertion!
                rImport.SetXmlId(xIfc, i_rXmlId);

                return xTextContent;
            }
            catch (com::sun::star::lang::IllegalArgumentException &)
            {
                OSL_FAIL("CreateAndInsertMark: cannot insert?");
                return 0;
            }
        }
    }
    return 0;
}

sal_Bool XMLTextMarkImportContext::FindName(
    SvXMLImport& rImport,
    const Reference<XAttributeList> & xAttrList)
{
    sal_Bool bNameOK = sal_False;

    // find name attribute first
    const sal_Int16 nLength = xAttrList->getLength();
    for(sal_Int16 nAttr = 0; nAttr < nLength; nAttr++)
    {
        OUString sLocalName;
        const sal_uInt16 nPrefix = rImport.GetNamespaceMap().
            GetKeyByAttrName( xAttrList->getNameByIndex(nAttr),
                              &sLocalName );

        if ( (XML_NAMESPACE_TEXT == nPrefix) &&
             IsXMLToken(sLocalName, XML_NAME)   )
        {
            m_sBookmarkName = xAttrList->getValueByIndex(nAttr);
            bNameOK = sal_True;
        }
        else if ( (XML_NAMESPACE_XML == nPrefix) &&
             IsXMLToken(sLocalName, XML_ID)   )
        {
            m_sXmlId = xAttrList->getValueByIndex(nAttr);
        }
        else if ( XML_NAMESPACE_XHTML == nPrefix )
        {
            // RDFa
            if ( IsXMLToken( sLocalName, XML_ABOUT) )
            {
                m_sAbout = xAttrList->getValueByIndex(nAttr);
                m_bHaveAbout = true;
            }
            else if ( IsXMLToken( sLocalName, XML_PROPERTY) )
            {
                m_sProperty = xAttrList->getValueByIndex(nAttr);
            }
            else if ( IsXMLToken( sLocalName, XML_CONTENT) )
            {
                m_sContent = xAttrList->getValueByIndex(nAttr);
            }
            else if ( IsXMLToken( sLocalName, XML_DATATYPE) )
            {
                m_sDatatype = xAttrList->getValueByIndex(nAttr);
            }
        }
        else if ( (XML_NAMESPACE_FIELD == nPrefix) &&
             IsXMLToken(sLocalName, XML_TYPE)   )
        {
            m_sFieldName = xAttrList->getValueByIndex(nAttr);
        }
    }

    return bNameOK;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
