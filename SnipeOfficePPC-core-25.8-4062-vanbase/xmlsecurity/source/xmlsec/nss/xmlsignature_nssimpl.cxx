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

#include <sal/config.h>
#include <rtl/uuid.h>
#include "xmlsignature_nssimpl.hxx"

#include "xmldocumentwrapper_xmlsecimpl.hxx"

#include "xmlelementwrapper_xmlsecimpl.hxx"

#include "securityenvironment_nssimpl.hxx"

#include "xmlsecuritycontext_nssimpl.hxx"
#include "xmlstreamio.hxx"
#include "errorcallback.hxx"

#include <sal/types.h>
//For reasons that escape me, this is what xmlsec does when size_t is not 4
#if SAL_TYPES_SIZEOFPOINTER != 4
#    define XMLSEC_NO_SIZE_T
#endif
#include "xmlsec/xmlsec.h"
#include "xmlsec/xmldsig.h"
#include "xmlsec/crypto.h"

using namespace ::com::sun::star::uno ;
using namespace ::com::sun::star::lang ;
using ::com::sun::star::lang::XMultiServiceFactory ;
using ::com::sun::star::lang::XSingleServiceFactory ;
using ::rtl::OUString ;

using ::com::sun::star::xml::wrapper::XXMLElementWrapper ;
using ::com::sun::star::xml::wrapper::XXMLDocumentWrapper ;
using ::com::sun::star::xml::crypto::XSecurityEnvironment ;
using ::com::sun::star::xml::crypto::XXMLSignature ;
using ::com::sun::star::xml::crypto::XXMLSignatureTemplate ;
using ::com::sun::star::xml::crypto::XSecurityEnvironment ;
using ::com::sun::star::xml::crypto::XXMLSecurityContext ;
using ::com::sun::star::xml::crypto::XUriBinding ;
using ::com::sun::star::xml::crypto::XMLSignatureException ;

XMLSignature_NssImpl :: XMLSignature_NssImpl( const Reference< XMultiServiceFactory >& aFactory ) : m_xServiceManager( aFactory ) {
}

XMLSignature_NssImpl :: ~XMLSignature_NssImpl() {
}

/* XXMLSignature */
Reference< XXMLSignatureTemplate >
SAL_CALL XMLSignature_NssImpl :: generate(
    const Reference< XXMLSignatureTemplate >& aTemplate ,
    const Reference< XSecurityEnvironment >& aEnvironment
) throw( com::sun::star::xml::crypto::XMLSignatureException,
         com::sun::star::uno::SecurityException )
{
    xmlSecKeysMngrPtr pMngr = NULL ;
    xmlSecDSigCtxPtr pDsigCtx = NULL ;
    xmlNodePtr pNode = NULL ;

    if( !aTemplate.is() )
        throw RuntimeException() ;

    if( !aEnvironment.is() )
        throw RuntimeException() ;

    //Get the xml node
    Reference< XXMLElementWrapper > xElement = aTemplate->getTemplate() ;
    if( !xElement.is() ) {
        throw RuntimeException() ;
    }

    Reference< XUnoTunnel > xNodTunnel( xElement , UNO_QUERY ) ;
    if( !xNodTunnel.is() ) {
        throw RuntimeException() ;
    }

    XMLElementWrapper_XmlSecImpl* pElement =
        reinterpret_cast<XMLElementWrapper_XmlSecImpl*>(
            sal::static_int_cast<sal_uIntPtr>(
                xNodTunnel->getSomething( XMLElementWrapper_XmlSecImpl::getUnoTunnelImplementationId() )));
    if( pElement == NULL ) {
        throw RuntimeException() ;
    }

    pNode = pElement->getNativeElement() ;

    //Get the stream/URI binding
    Reference< XUriBinding > xUriBinding = aTemplate->getBinding() ;
    if( xUriBinding.is() ) {
        //Register the stream input callbacks into libxml2
        if( xmlRegisterStreamInputCallbacks( xUriBinding ) < 0 )
            throw RuntimeException() ;
    }

    //Get Keys Manager
    Reference< XUnoTunnel > xSecTunnel( aEnvironment , UNO_QUERY ) ;
    if( !xSecTunnel.is() ) {
         throw RuntimeException() ;
    }

    //i39448 : the key manager should be retrieved from SecurityEnvironment, instead of SecurityContext

    SecurityEnvironment_NssImpl* pSecEnv =
        reinterpret_cast<SecurityEnvironment_NssImpl*>(
            sal::static_int_cast<sal_uIntPtr>(
                xSecTunnel->getSomething( SecurityEnvironment_NssImpl::getUnoTunnelId() )));
    if( pSecEnv == NULL )
        throw RuntimeException() ;

     setErrorRecorder();

    pMngr = pSecEnv->createKeysManager() ; //i39448
    if( !pMngr ) {
        throw RuntimeException() ;
    }

    //Create Signature context
    pDsigCtx = xmlSecDSigCtxCreate( pMngr ) ;
    if( pDsigCtx == NULL )
    {
        pSecEnv->destroyKeysManager( pMngr ) ; //i39448
        //throw XMLSignatureException() ;
        clearErrorRecorder();
        return aTemplate;
    }

    //Sign the template
    if( xmlSecDSigCtxSign( pDsigCtx , pNode ) == 0 )
    {
        if (pDsigCtx->status == xmlSecDSigStatusSucceeded)
            aTemplate->setStatus(com::sun::star::xml::crypto::SecurityOperationStatus_OPERATION_SUCCEEDED);
        else
            aTemplate->setStatus(com::sun::star::xml::crypto::SecurityOperationStatus_UNKNOWN);
    }
    else
    {
        aTemplate->setStatus(com::sun::star::xml::crypto::SecurityOperationStatus_UNKNOWN);
    }


    xmlSecDSigCtxDestroy( pDsigCtx ) ;
    pSecEnv->destroyKeysManager( pMngr ) ; //i39448

    //Unregistered the stream/URI binding
    if( xUriBinding.is() )
        xmlUnregisterStreamInputCallbacks() ;

    clearErrorRecorder();
    return aTemplate ;
}

/* XXMLSignature */
Reference< XXMLSignatureTemplate >
SAL_CALL XMLSignature_NssImpl :: validate(
    const Reference< XXMLSignatureTemplate >& aTemplate ,
    const Reference< XXMLSecurityContext >& aSecurityCtx
) throw( com::sun::star::uno::RuntimeException,
         com::sun::star::uno::SecurityException,
         com::sun::star::xml::crypto::XMLSignatureException ) {
    xmlSecKeysMngrPtr pMngr = NULL ;
    xmlSecDSigCtxPtr pDsigCtx = NULL ;
    xmlNodePtr pNode = NULL ;
    //sal_Bool valid ;

    if( !aTemplate.is() )
        throw RuntimeException() ;

    if( !aSecurityCtx.is() )
        throw RuntimeException() ;

    //Get the xml node
    Reference< XXMLElementWrapper > xElement = aTemplate->getTemplate() ;
    if( !xElement.is() )
        throw RuntimeException() ;

    Reference< XUnoTunnel > xNodTunnel( xElement , UNO_QUERY ) ;
    if( !xNodTunnel.is() ) {
        throw RuntimeException() ;
    }

    XMLElementWrapper_XmlSecImpl* pElement =
        reinterpret_cast<XMLElementWrapper_XmlSecImpl*>(
            sal::static_int_cast<sal_uIntPtr>(
                xNodTunnel->getSomething( XMLElementWrapper_XmlSecImpl::getUnoTunnelImplementationId() )));
    if( pElement == NULL )
        throw RuntimeException() ;

    pNode = pElement->getNativeElement() ;

    //Get the stream/URI binding
    Reference< XUriBinding > xUriBinding = aTemplate->getBinding() ;
    if( xUriBinding.is() ) {
        //Register the stream input callbacks into libxml2
        if( xmlRegisterStreamInputCallbacks( xUriBinding ) < 0 )
            throw RuntimeException() ;
    }

     setErrorRecorder();

    sal_Int32 nSecurityEnvironment = aSecurityCtx->getSecurityEnvironmentNumber();
    sal_Int32 i;

    for (i=0; i<nSecurityEnvironment; ++i)
    {
        Reference< XSecurityEnvironment > aEnvironment = aSecurityCtx->getSecurityEnvironmentByIndex(i);

        //Get Keys Manager
        Reference< XUnoTunnel > xSecTunnel( aEnvironment , UNO_QUERY ) ;
        if( !xSecTunnel.is() ) {
             throw RuntimeException() ;
        }

        SecurityEnvironment_NssImpl* pSecEnv =
            reinterpret_cast<SecurityEnvironment_NssImpl*>(
                sal::static_int_cast<sal_uIntPtr>(
                    xSecTunnel->getSomething( SecurityEnvironment_NssImpl::getUnoTunnelId() )));
        if( pSecEnv == NULL )
            throw RuntimeException() ;

        pMngr = pSecEnv->createKeysManager() ; //i39448
        if( !pMngr ) {
            throw RuntimeException() ;
        }

        //Create Signature context
        pDsigCtx = xmlSecDSigCtxCreate( pMngr ) ;
        if( pDsigCtx == NULL )
        {
            pSecEnv->destroyKeysManager( pMngr ) ; //i39448
            //throw XMLSignatureException() ;
            clearErrorRecorder();
            return aTemplate;
        }

        //Verify signature
        int rs = xmlSecDSigCtxVerify( pDsigCtx , pNode );


        if (rs == 0 &&
            pDsigCtx->status == xmlSecDSigStatusSucceeded)
        {
            aTemplate->setStatus(com::sun::star::xml::crypto::SecurityOperationStatus_OPERATION_SUCCEEDED);
            xmlSecDSigCtxDestroy( pDsigCtx ) ;
            pSecEnv->destroyKeysManager( pMngr );
            break;
        }
        else
        {
            aTemplate->setStatus(com::sun::star::xml::crypto::SecurityOperationStatus_UNKNOWN);
        }
        xmlSecDSigCtxDestroy( pDsigCtx ) ;
        pSecEnv->destroyKeysManager( pMngr );
    }



    //Unregistered the stream/URI binding
    if( xUriBinding.is() )
        xmlUnregisterStreamInputCallbacks() ;

    //return valid ;
    clearErrorRecorder();
    return aTemplate;
}

/* XInitialization */
void SAL_CALL XMLSignature_NssImpl :: initialize( const Sequence< Any >& /*aArguments*/ ) throw( Exception, RuntimeException ) {
    // TBD
} ;

/* XServiceInfo */
OUString SAL_CALL XMLSignature_NssImpl :: getImplementationName() throw( RuntimeException ) {
    return impl_getImplementationName() ;
}

/* XServiceInfo */
sal_Bool SAL_CALL XMLSignature_NssImpl :: supportsService( const OUString& serviceName) throw( RuntimeException ) {
    Sequence< OUString > seqServiceNames = getSupportedServiceNames() ;
    const OUString* pArray = seqServiceNames.getConstArray() ;
    for( sal_Int32 i = 0 ; i < seqServiceNames.getLength() ; i ++ ) {
        if( *( pArray + i ) == serviceName )
            return sal_True ;
    }
    return sal_False ;
}

/* XServiceInfo */
Sequence< OUString > SAL_CALL XMLSignature_NssImpl :: getSupportedServiceNames() throw( RuntimeException ) {
    return impl_getSupportedServiceNames() ;
}

//Helper for XServiceInfo
Sequence< OUString > XMLSignature_NssImpl :: impl_getSupportedServiceNames() {
    ::osl::Guard< ::osl::Mutex > aGuard( ::osl::Mutex::getGlobalMutex() ) ;
    Sequence< OUString > seqServiceNames( 1 ) ;
    seqServiceNames.getArray()[0] = OUString("com.sun.star.xml.crypto.XMLSignature") ;
    return seqServiceNames ;
}

OUString XMLSignature_NssImpl :: impl_getImplementationName() throw( RuntimeException ) {
    return OUString("com.sun.star.xml.security.bridge.xmlsec.XMLSignature_NssImpl") ;
}

//Helper for registry
Reference< XInterface > SAL_CALL XMLSignature_NssImpl :: impl_createInstance( const Reference< XMultiServiceFactory >& aServiceManager ) throw( RuntimeException ) {
    return Reference< XInterface >( *new XMLSignature_NssImpl( aServiceManager ) ) ;
}

Reference< XSingleServiceFactory > XMLSignature_NssImpl :: impl_createFactory( const Reference< XMultiServiceFactory >& aServiceManager ) {
    //Reference< XSingleServiceFactory > xFactory ;
    //xFactory = ::cppu::createSingleFactory( aServiceManager , impl_getImplementationName , impl_createInstance , impl_getSupportedServiceNames ) ;
    //return xFactory ;
    return ::cppu::createSingleFactory( aServiceManager , impl_getImplementationName() , impl_createInstance , impl_getSupportedServiceNames() ) ;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
