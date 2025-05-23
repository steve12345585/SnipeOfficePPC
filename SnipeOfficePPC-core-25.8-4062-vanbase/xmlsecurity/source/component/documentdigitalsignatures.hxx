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

#ifndef _XMLSECURITY_DOCUMENTDIGITALSIGNATURES_HXX
#define _XMLSECURITY_DOCUMENTDIGITALSIGNATURES_HXX

#include <cppuhelper/implbase2.hxx>

#include "com/sun/star/lang/XInitialization.hpp"
#include <com/sun/star/security/XDocumentDigitalSignatures.hpp>
#include <com/sun/star/io/XStream.hpp>
#include <com/sun/star/io/XInputStream.hpp>
#include <xmlsecurity/documentsignaturehelper.hxx>

namespace com { namespace  sun { namespace star {

    namespace uno {
        class XComponentContext;
    }
}}}

class DocumentDigitalSignatures : public cppu::WeakImplHelper2
<
    com::sun::star::security::XDocumentDigitalSignatures,
    com::sun::star::lang::XInitialization
>
{
private:
    com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext > mxCtx;
    // will be set by XInitialization. If not we assume true. false means an earlier version.
    ::rtl::OUString m_sODFVersion;
    //The number of arguments which were passed in XInitialization::initialize
    int m_nArgumentsCount;
    //Indicates if the document already contains a document signature
    bool m_bHasDocumentSignature;

    sal_Bool ImplViewSignatures( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& rxStorage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XStream >& xSignStream, DocumentSignatureMode eMode, bool bReadOnly ) throw (::com::sun::star::uno::RuntimeException);
    sal_Bool ImplViewSignatures( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& rxStorage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& xSignStream, DocumentSignatureMode eMode, bool bReadOnly ) throw (::com::sun::star::uno::RuntimeException);
    com::sun::star::uno::Sequence< ::com::sun::star::security::DocumentSignatureInformation > ImplVerifySignatures( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& rxStorage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& xSignStream, DocumentSignatureMode eMode ) throw (::com::sun::star::uno::RuntimeException);

public:
    DocumentDigitalSignatures( const com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext>& rxCtx );

    // for service registration...
    static ::rtl::OUString GetImplementationName() throw (com::sun::star::uno::RuntimeException);
    static ::com::sun::star::uno::Sequence < ::rtl::OUString > GetSupportedServiceNames() throw (com::sun::star::uno::RuntimeException);

    //XInitialization
    void SAL_CALL initialize( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments )
        throw (::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);

    // XDocumentDigitalSignatures
    ::sal_Bool SAL_CALL signDocumentContent( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& xStorage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XStream >& xSignStream ) throw (::com::sun::star::uno::RuntimeException);
    ::com::sun::star::uno::Sequence< ::com::sun::star::security::DocumentSignatureInformation > SAL_CALL verifyDocumentContentSignatures( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& xStorage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& xSignInStream ) throw (::com::sun::star::uno::RuntimeException);
    void SAL_CALL showDocumentContentSignatures( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& xStorage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& xSignInStream ) throw (::com::sun::star::uno::RuntimeException);
    ::rtl::OUString SAL_CALL getDocumentContentSignatureDefaultStreamName(  ) throw (::com::sun::star::uno::RuntimeException);
    ::sal_Bool SAL_CALL signScriptingContent( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& xStorage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XStream >& xSignStream ) throw (::com::sun::star::uno::RuntimeException);
    ::com::sun::star::uno::Sequence< ::com::sun::star::security::DocumentSignatureInformation > SAL_CALL verifyScriptingContentSignatures( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& xStorage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& xSignInStream ) throw (::com::sun::star::uno::RuntimeException);
    void SAL_CALL showScriptingContentSignatures( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& xStorage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& xSignInStream ) throw (::com::sun::star::uno::RuntimeException);
    ::rtl::OUString SAL_CALL getScriptingContentSignatureDefaultStreamName(  ) throw (::com::sun::star::uno::RuntimeException);
    ::sal_Bool SAL_CALL signPackage( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& Storage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XStream >& xSignStream ) throw (::com::sun::star::uno::RuntimeException);
    ::com::sun::star::uno::Sequence< ::com::sun::star::security::DocumentSignatureInformation > SAL_CALL verifyPackageSignatures( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& Storage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& xSignInStream ) throw (::com::sun::star::uno::RuntimeException);
    void SAL_CALL showPackageSignatures( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& xStorage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& xSignInStream ) throw (::com::sun::star::uno::RuntimeException);
    ::rtl::OUString SAL_CALL getPackageSignatureDefaultStreamName(  ) throw (::com::sun::star::uno::RuntimeException);
    void SAL_CALL showCertificate( const ::com::sun::star::uno::Reference< ::com::sun::star::security::XCertificate >& Certificate ) throw (::com::sun::star::uno::RuntimeException);
    void SAL_CALL manageTrustedSources(  ) throw (::com::sun::star::uno::RuntimeException);
    ::sal_Bool SAL_CALL isAuthorTrusted( const ::com::sun::star::uno::Reference< ::com::sun::star::security::XCertificate >& Author ) throw (::com::sun::star::uno::RuntimeException);
    ::sal_Bool SAL_CALL isLocationTrusted( const ::rtl::OUString& Location ) throw (::com::sun::star::uno::RuntimeException);
    void SAL_CALL addAuthorToTrustedSources( const ::com::sun::star::uno::Reference< ::com::sun::star::security::XCertificate >& Author ) throw (::com::sun::star::uno::RuntimeException);
    void SAL_CALL addLocationToTrustedSources( const ::rtl::OUString& Location ) throw (::com::sun::star::uno::RuntimeException);

    ::com::sun::star::uno::Reference< ::com::sun::star::security::XCertificate > SAL_CALL chooseCertificate( ) throw (::com::sun::star::uno::RuntimeException);
};

com::sun::star::uno::Reference< com::sun::star::uno::XInterface > SAL_CALL DocumentDigitalSignatures_CreateInstance(
    const com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext >& rCtx) throw ( com::sun::star::uno::Exception );

#endif // _XMLSECURITY_DOCUMENTDIGITALSIGNATURES_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
