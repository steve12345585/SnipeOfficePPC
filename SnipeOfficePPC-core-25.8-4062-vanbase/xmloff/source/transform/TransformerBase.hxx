/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
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

#ifndef _XMLOFF_TRANSFORMER_BASE_HXX
#define _XMLOFF_TRANSFORMER_BASE_HXX

#include <com/sun/star/xml/sax/SAXParseException.hpp>
#include <com/sun/star/xml/sax/SAXException.hpp>
#include <com/sun/star/xml/sax/XAttributeList.hpp>
#include <com/sun/star/xml/sax/XLocator.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <xmloff/xmltoken.hxx>

#include "Transformer.hxx"

namespace rtl { class OUString; }
namespace com { namespace sun { namespace star {
    namespace i18n { class XCharacterClassification; }
}}}

class SvXMLNamespaceMap;
class XMLTransformerContext;
class XMLTransformerContextVector;
class XMLTransformerActions;
struct XMLTransformerActionInit;
struct TransformerAction_Impl;
class XMLMutableAttributeList;
class XMLTransformerTokenMap;

const sal_uInt16 INVALID_ACTIONS = 0xffff;

class XMLTransformerBase : public XMLTransformer
{
    friend class XMLTransformerContext;

    ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XLocator >
        m_xLocator;

    ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XDocumentHandler >            m_xHandler;     // the handlers
    ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XExtendedDocumentHandler >    m_xExtHandler;
    ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > m_xPropSet;
    ::com::sun::star::uno::Reference<
        ::com::sun::star::i18n::XCharacterClassification > xCharClass;

    ::rtl::OUString m_aExtPathPrefix;
    ::rtl::OUString m_aClass;

    SvXMLNamespaceMap           *m_pNamespaceMap;
    SvXMLNamespaceMap           *m_pReplaceNamespaceMap;
    XMLTransformerContextVector *m_pContexts;
    XMLTransformerActions       *m_pElemActions;
    XMLTransformerTokenMap      *m_pTokenMap;

protected:
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel >     mxModel;

    // This method is called after the namespace map has been updated, but
    // before a context for the current element has been pushed.
    XMLTransformerContext *CreateContext( sal_uInt16 nPrefix,
                                      const ::rtl::OUString& rLocalName,
                                      const ::rtl::OUString& rQName );

    // this method may return an empty reference when the transformer service
    // was created outside the xmloff environment.
    // It is strictly forbiden to use this as a write access to the model!
    const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel >& getModel() const { return mxModel; }

public:
    XMLTransformerBase( XMLTransformerActionInit *pInit=0,
                           ::xmloff::token::XMLTokenEnum *pTKMapInit=0 ) throw();
    virtual ~XMLTransformerBase() throw();

    // ::com::sun::star::xml::sax::XDocumentHandler
    virtual void SAL_CALL startDocument(void)
        throw( ::com::sun::star::xml::sax::SAXException, ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL endDocument(void)
        throw( ::com::sun::star::xml::sax::SAXException, ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL startElement(const ::rtl::OUString& aName,
                              const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList > & xAttribs)
        throw( ::com::sun::star::xml::sax::SAXException, ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL endElement(const ::rtl::OUString& aName)
        throw( ::com::sun::star::xml::sax::SAXException, ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL characters(const ::rtl::OUString& aChars)
        throw( ::com::sun::star::xml::sax::SAXException, ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL ignorableWhitespace(const ::rtl::OUString& aWhitespaces)
        throw( ::com::sun::star::xml::sax::SAXException, ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL processingInstruction(const ::rtl::OUString& aTarget,
                                       const ::rtl::OUString& aData)
        throw( ::com::sun::star::xml::sax::SAXException, ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL setDocumentLocator(const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XLocator > & xLocator)
        throw( ::com::sun::star::xml::sax::SAXException, ::com::sun::star::uno::RuntimeException );

    // ::com::sun::star::xml::sax::XExtendedDocumentHandler
    virtual void SAL_CALL startCDATA(void) throw( ::com::sun::star::xml::sax::SAXException, ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL endCDATA(void) throw( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL comment(const ::rtl::OUString& sComment)
        throw( ::com::sun::star::xml::sax::SAXException, ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL allowLineBreak(void)
        throw( ::com::sun::star::xml::sax::SAXException, ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL unknown(const ::rtl::OUString& sString)
        throw( ::com::sun::star::xml::sax::SAXException, ::com::sun::star::uno::RuntimeException );

    // XInitialization
    virtual void SAL_CALL initialize( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments ) throw(::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);

    // C++
    const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XDocumentHandler > & GetDocHandler() { return m_xHandler; }

    const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > & GetPropertySet() { return m_xPropSet; }


    SvXMLNamespaceMap& GetNamespaceMap() { return *m_pNamespaceMap; }
    const SvXMLNamespaceMap& GetNamespaceMap() const { return *m_pNamespaceMap; }
    SvXMLNamespaceMap& GetReplaceNamespaceMap() { return *m_pReplaceNamespaceMap; }

    XMLTransformerActions& GetElemActions() { return *m_pElemActions; }
    virtual XMLTransformerActions *GetUserDefinedActions( sal_uInt16 n );
    virtual XMLTransformerContext *CreateUserDefinedContext(
                                      const TransformerAction_Impl& rAction,
                                      const ::rtl::OUString& rQName,
                                         sal_Bool bPersistent=sal_False ) = 0;
    virtual ::rtl::OUString GetEventName( const ::rtl::OUString& rName,
                                             sal_Bool bForm = sal_False ) = 0;


    XMLMutableAttributeList *ProcessAttrList( ::com::sun::star::uno::Reference<
                ::com::sun::star::xml::sax::XAttributeList >& rAttrList,
                         sal_uInt16 nActionMap, sal_Bool bClone );

    static sal_Bool ReplaceSingleInchWithIn( ::rtl::OUString& rValue );
    static sal_Bool ReplaceSingleInWithInch( ::rtl::OUString& rValue );
    static sal_Bool ReplaceInchWithIn( ::rtl::OUString& rValue );
    static sal_Bool ReplaceInWithInch( ::rtl::OUString& rValue );

    sal_Bool EncodeStyleName( ::rtl::OUString& rName ) const;
    static sal_Bool DecodeStyleName( ::rtl::OUString& rName );
    static sal_Bool NegPercent( ::rtl::OUString& rValue );

    sal_Bool AddNamespacePrefix( ::rtl::OUString& rName,
                                 sal_uInt16 nPrefix ) const;
    sal_Bool RemoveNamespacePrefix( ::rtl::OUString& rName,
                                    sal_uInt16 nPrefixOnly=0xffffU ) const;

    sal_Bool ConvertURIToOASIS( ::rtl::OUString& rURI,
                                sal_Bool bSupportPackage=sal_False ) const;
    sal_Bool ConvertURIToOOo( ::rtl::OUString& rURI,
                                sal_Bool bSupportPackage=sal_False ) const;

    /** renames the given rOutAttributeValue if one of the parameters contains a
        matching token in its lower 16 bits.  The value is converted to the
        token that is given in the upper 16 bits of the matching parameter.
     */
    sal_Bool RenameAttributeValue( ::rtl::OUString& rOutAttributeValue,
                                   sal_Int32 nParam1,
                                   sal_Int32 nParam2,
                                   sal_Int32 nParam3 );

    /** converts the '.' that separates fractions of seconds in a dateTime
        string into a ',' that was used in the OOo format

        @param rDateTime
            A dateTime string that will be parsed and changed in case a match
            was found.
        @return <TRUE/> if the given string was changed
     */
    static bool ConvertRNGDateTimeToISO( ::rtl::OUString& rDateTime );

    ::xmloff::token::XMLTokenEnum GetToken( const ::rtl::OUString& rStr ) const;

    const XMLTransformerContext *GetCurrentContext() const;
    const XMLTransformerContext *GetAncestorContext( sal_uInt32 i ) const;

    // C++
    inline void SetClass( const ::rtl::OUString& r ) { m_aClass = r; }
    inline const ::rtl::OUString& GetClass() const { return m_aClass; }

    bool isWriter() const;

};

#endif  //  _XMLOFF_TRANSFORMER_BASE_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
