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


#include "signaturecreatorimpl.hxx"
#include <com/sun/star/xml/crypto/XXMLSignatureTemplate.hpp>
#include <com/sun/star/xml/wrapper/XXMLElementWrapper.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>

namespace cssu = com::sun::star::uno;
namespace cssl = com::sun::star::lang;
namespace cssxc = com::sun::star::xml::crypto;
namespace cssxw = com::sun::star::xml::wrapper;

#define SERVICE_NAME "com.sun.star.xml.crypto.sax.SignatureCreator"
#define IMPLEMENTATION_NAME "com.sun.star.xml.security.framework.SignatureCreatorImpl"

SignatureCreatorImpl::SignatureCreatorImpl( const cssu::Reference< cssl::XMultiServiceFactory >& rxMSF )
    :m_nIdOfBlocker(-1)
{
    mxMSF = rxMSF;
}

SignatureCreatorImpl::~SignatureCreatorImpl( )
{
}

bool SignatureCreatorImpl::checkReady() const
/****** SignatureCreatorImpl/checkReady **************************************
 *
 *   NAME
 *  checkReady -- checks the conditions for the signature generation.
 *
 *   SYNOPSIS
 *  bReady = checkReady( );
 *
 *   FUNCTION
 *  checks whether all following conditions are satisfied:
 *  1. the result listener is ready;
 *  2. the id of the template blocker is known;
 *  3. the SignatureEngine is ready.
 *
 *   INPUTS
 *  empty
 *
 *   RESULT
 *  bReady - true if all conditions are satisfied, false otherwise
 *
 *   AUTHOR
 *  Michael Mi
 *  Email: michael.mi@sun.com
 ******************************************************************************/
{
    return (m_xResultListener.is() &&
            (m_nIdOfBlocker != -1) &&
            SignatureEngine::checkReady());
}

void SignatureCreatorImpl::notifyResultListener() const
    throw (cssu::Exception, cssu::RuntimeException)
/****** SignatureCreatorImpl/notifyResultListener *****************************
 *
 *   NAME
 *  notifyResultListener -- notifies the listener about the signature
 *  creation result.
 *
 *   SYNOPSIS
 *  notifyResultListener( );
 *
 *   FUNCTION
 *  see NAME.
 *
 *   INPUTS
 *  empty
 *
 *   RESULT
 *  empty
 *
 *   AUTHOR
 *  Michael Mi
 *  Email: michael.mi@sun.com
 ******************************************************************************/
{
    cssu::Reference< cssxc::sax::XSignatureCreationResultListener >
        xSignatureCreationResultListener ( m_xResultListener , cssu::UNO_QUERY ) ;

    xSignatureCreationResultListener->signatureCreated( m_nSecurityId, m_nStatus );
}

void SignatureCreatorImpl::startEngine( const cssu::Reference<
    cssxc::XXMLSignatureTemplate >&
    xSignatureTemplate)
        throw (cssu::Exception, cssu::RuntimeException)
/****** SignatureCreatorImpl/startEngine *************************************
 *
 *   NAME
 *  startEngine -- generates the signature.
 *
 *   SYNOPSIS
 *  startEngine( xSignatureTemplate );
 *
 *   FUNCTION
 *  generates the signature element, then if succeeds, updates the link
 *  of old template element to the new signature element in
 *  SAXEventKeeper.
 *
 *   INPUTS
 *  xSignatureTemplate - the signature template (along with all referenced
 *  elements) to be signed.
 *
 *   RESULT
 *  empty
 *
 *   AUTHOR
 *  Michael Mi
 *  Email: michael.mi@sun.com
 ******************************************************************************/
{
    cssu::Reference< cssxc::XXMLSignatureTemplate > xResultTemplate;
    try
    {
        xResultTemplate = m_xXMLSignature->generate(xSignatureTemplate, m_xSecurityEnvironment);
        m_nStatus = xResultTemplate->getStatus();
    }
    catch( cssu::Exception& )
    {
        m_nStatus = cssxc::SecurityOperationStatus_RUNTIMEERROR_FAILED;
    }

    if (m_nStatus == cssxc::SecurityOperationStatus_OPERATION_SUCCEEDED)
    {
        cssu::Reference < cssxw::XXMLElementWrapper > xResultSignature = xResultTemplate->getTemplate();
        m_xSAXEventKeeper->setElement(m_nIdOfTemplateEC, xResultSignature);
    }
}

void SignatureCreatorImpl::clearUp() const
/****** SignatureCreatorImpl/clearUp *****************************************
 *
 *   NAME
 *  clearUp -- clear up all resources used by the signature generation.
 *
 *   SYNOPSIS
 *  clearUp( );
 *
 *   FUNCTION
 *  cleaning resources up includes:
 *  1. SignatureEngine's clearing up;
 *  2. releases the Blocker for the signature template element.
 *
 *   INPUTS
 *  empty
 *
 *   RESULT
 *  empty
 *
 *   AUTHOR
 *  Michael Mi
 *  Email: michael.mi@sun.com
 ******************************************************************************/
{
    SignatureEngine::clearUp();

    if (m_nIdOfBlocker != -1)
    {
        m_xSAXEventKeeper->removeBlocker(m_nIdOfBlocker);
    }
}

/* XBlockerMonitor */
void SAL_CALL SignatureCreatorImpl::setBlockerId( sal_Int32 id )
        throw (cssu::Exception, cssu::RuntimeException)
{
    m_nIdOfBlocker = id;
    tryToPerform();
}

/* XSignatureCreationResultBroadcaster */
void SAL_CALL SignatureCreatorImpl::addSignatureCreationResultListener(
    const cssu::Reference< cssxc::sax::XSignatureCreationResultListener >& listener )
    throw (cssu::Exception, cssu::RuntimeException)
{
    m_xResultListener = listener;
    tryToPerform();
}

void SAL_CALL SignatureCreatorImpl::removeSignatureCreationResultListener(
    const cssu::Reference< cssxc::sax::XSignatureCreationResultListener >&)
    throw (cssu::RuntimeException)
{
}

/* XInitialization */
void SAL_CALL SignatureCreatorImpl::initialize( const cssu::Sequence< cssu::Any >& aArguments )
    throw (cssu::Exception, cssu::RuntimeException)
{
    OSL_ASSERT(aArguments.getLength() == 5);

    rtl::OUString ouTempString;

    aArguments[0] >>= ouTempString;
    m_nSecurityId = ouTempString.toInt32();
    aArguments[1] >>= m_xSAXEventKeeper;
    aArguments[2] >>= ouTempString;
    m_nIdOfTemplateEC = ouTempString.toInt32();
    aArguments[3] >>= m_xSecurityEnvironment;
    aArguments[4] >>= m_xXMLSignature;
}


rtl::OUString SignatureCreatorImpl_getImplementationName ()
    throw (cssu::RuntimeException)
{
    return rtl::OUString ( RTL_CONSTASCII_USTRINGPARAM ( IMPLEMENTATION_NAME ) );
}

sal_Bool SAL_CALL SignatureCreatorImpl_supportsService( const rtl::OUString& ServiceName )
    throw (cssu::RuntimeException)
{
    return ServiceName == SERVICE_NAME;
}

cssu::Sequence< rtl::OUString > SAL_CALL SignatureCreatorImpl_getSupportedServiceNames(  )
    throw (cssu::RuntimeException)
{
    cssu::Sequence < rtl::OUString > aRet(1);
    rtl::OUString* pArray = aRet.getArray();
    pArray[0] =  rtl::OUString ( RTL_CONSTASCII_USTRINGPARAM ( SERVICE_NAME ) );
    return aRet;
}
#undef SERVICE_NAME

cssu::Reference< cssu::XInterface > SAL_CALL SignatureCreatorImpl_createInstance(
    const cssu::Reference< cssl::XMultiServiceFactory >& rSMgr)
    throw( cssu::Exception )
{
    return (cppu::OWeakObject*) new SignatureCreatorImpl( rSMgr );
}

/* XServiceInfo */
rtl::OUString SAL_CALL SignatureCreatorImpl::getImplementationName(  )
    throw (cssu::RuntimeException)
{
    return SignatureCreatorImpl_getImplementationName();
}
sal_Bool SAL_CALL SignatureCreatorImpl::supportsService( const rtl::OUString& rServiceName )
    throw (cssu::RuntimeException)
{
    return SignatureCreatorImpl_supportsService( rServiceName );
}
cssu::Sequence< rtl::OUString > SAL_CALL SignatureCreatorImpl::getSupportedServiceNames(  )
    throw (cssu::RuntimeException)
{
    return SignatureCreatorImpl_getSupportedServiceNames();
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
