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

#ifndef _XSEC_CTL_PARSER_HXX
#define _XSEC_CTL_PARSER_HXX

#include <xsecctl.hxx>

#include <com/sun/star/xml/sax/XParser.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/xml/sax/XDocumentHandler.hpp>
#include <com/sun/star/xml/sax/XAttributeList.hpp>

#include <cppuhelper/implbase2.hxx>

class XSecParser: public cppu::WeakImplHelper2
<
    com::sun::star::xml::sax::XDocumentHandler,
    com::sun::star::lang::XInitialization
>
/****** XSecController.hxx/CLASS XSecParser ***********************************
 *
 *   NAME
 *  XSecParser -- a SAX parser that can detect security elements
 *
 *   FUNCTION
 *  The XSecParser object is connected on the SAX chain and detects
 *  security elements in the SAX event stream, then notifies
 *  the XSecController.
 *
 *   NOTES
 *  This class is used when importing a document.
 *
 *   AUTHOR
 *  Michael Mi
 *  Email: michael.mi@sun.com
 ******************************************************************************/
{
    friend class XSecController;
private:
    /*
     * the following members are used to reserve the signature information,
     * including X509IssuerName, X509SerialNumber, and X509Certificate,etc.
     */
    rtl::OUString m_ouX509IssuerName;
    rtl::OUString m_ouX509SerialNumber;
    rtl::OUString m_ouX509Certificate;
    rtl::OUString m_ouDigestValue;
    rtl::OUString m_ouSignatureValue;
    rtl::OUString m_ouDate;

    /*
     * whether inside a particular element
     */
    bool m_bInX509IssuerName;
    bool m_bInX509SerialNumber;
    bool m_bInX509Certificate;
    bool m_bInDigestValue;
    bool m_bInSignatureValue;
    bool m_bInDate;

    /*
     * the XSecController collaborating with XSecParser
     */
    XSecController* m_pXSecController;

    /*
     * the next XDocumentHandler on the SAX chain
     */
    com::sun::star::uno::Reference<
        com::sun::star::xml::sax::XDocumentHandler > m_xNextHandler;

    /*
     * this string is used to remember the current handled reference's URI,
     *
     * because it can be decided whether a stream reference is xml based or binary based
     * only after the Transforms element is read in, so we have to reserve the reference's
     * URI when the startElement event is met.
     */
    rtl::OUString m_currentReferenceURI;
    bool m_bReferenceUnresolved;

private:
    rtl::OUString getIdAttr(const com::sun::star::uno::Reference<
            com::sun::star::xml::sax::XAttributeList >& xAttribs );

public:
    XSecParser( XSecController* pXSecController,
        const com::sun::star::uno::Reference<
            com::sun::star::xml::sax::XDocumentHandler >& xNextHandler );
    ~XSecParser(){};

    /*
     * XDocumentHandler
     */
    virtual void SAL_CALL startDocument(  )
        throw (com::sun::star::xml::sax::SAXException, com::sun::star::uno::RuntimeException);

    virtual void SAL_CALL endDocument(  )
        throw (com::sun::star::xml::sax::SAXException, com::sun::star::uno::RuntimeException);

    virtual void SAL_CALL startElement(
        const rtl::OUString& aName,
        const com::sun::star::uno::Reference<
            com::sun::star::xml::sax::XAttributeList >& xAttribs )
        throw (com::sun::star::xml::sax::SAXException, com::sun::star::uno::RuntimeException);

    virtual void SAL_CALL endElement( const rtl::OUString& aName )
        throw (com::sun::star::xml::sax::SAXException, com::sun::star::uno::RuntimeException);

    virtual void SAL_CALL characters( const rtl::OUString& aChars )
        throw (com::sun::star::xml::sax::SAXException, com::sun::star::uno::RuntimeException);

    virtual void SAL_CALL ignorableWhitespace( const rtl::OUString& aWhitespaces )
        throw (com::sun::star::xml::sax::SAXException, com::sun::star::uno::RuntimeException);

    virtual void SAL_CALL processingInstruction(
        const rtl::OUString& aTarget,
        const rtl::OUString& aData )
        throw (com::sun::star::xml::sax::SAXException, com::sun::star::uno::RuntimeException);

    virtual void SAL_CALL setDocumentLocator(
        const com::sun::star::uno::Reference<
            com::sun::star::xml::sax::XLocator >& xLocator )
        throw (com::sun::star::xml::sax::SAXException, com::sun::star::uno::RuntimeException);

    /*
     * XInitialization
     */
    virtual void SAL_CALL initialize(
        const com::sun::star::uno::Sequence< com::sun::star::uno::Any >& aArguments )
        throw(com::sun::star::uno::Exception, com::sun::star::uno::RuntimeException);
};

#endif


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
