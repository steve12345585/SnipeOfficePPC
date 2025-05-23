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

#ifndef _CERTIFICATEEXTENSION_XMLSECIMPL_HXX_
#define _CERTIFICATEEXTENSION_XMLSECIMPL_HXX_

#include <sal/config.h>
#include <rtl/ustring.hxx>
#include <cppuhelper/factory.hxx>
#include <cppuhelper/implbase1.hxx>
#include <com/sun/star/uno/Exception.hpp>
#include "com/sun/star/uno/SecurityException.hpp"
#include <com/sun/star/security/XCertificateExtension.hpp>

class CertificateExtension_XmlSecImpl : public ::cppu::WeakImplHelper1<
    ::com::sun::star::security::XCertificateExtension >
{
    private :
        sal_Bool m_critical ;
        ::com::sun::star::uno::Sequence< sal_Int8 > m_xExtnId ;
        ::com::sun::star::uno::Sequence< sal_Int8 > m_xExtnValue ;

    public :
        CertificateExtension_XmlSecImpl() ;
        virtual ~CertificateExtension_XmlSecImpl() ;

        //Methods from XCertificateExtension
        virtual sal_Bool SAL_CALL isCritical() throw( ::com::sun::star::uno::RuntimeException ) ;

        virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getExtensionId() throw( ::com::sun::star::uno::RuntimeException ) ;

        virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getExtensionValue() throw( ::com::sun::star::uno::RuntimeException ) ;

        void setCertExtn( unsigned char* value, unsigned int vlen, unsigned char* id, unsigned int idlen, sal_Bool critical ) ;
} ;

#endif  // _CERTIFICATEEXTENSION_XMLSECIMPL_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
