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

#ifndef _CIPHERCONTEXT_HXX
#define _CIPHERCONTEXT_HXX

#include <com/sun/star/xml/crypto/XCipherContext.hpp>

#include <cppuhelper/implbase1.hxx>
#include <osl/mutex.hxx>
#include <pk11pub.h>

class OCipherContext : public cppu::WeakImplHelper1< ::com::sun::star::xml::crypto::XCipherContext >
{
private:
    ::osl::Mutex m_aMutex;

    PK11SlotInfo* m_pSlot;
    PK11SymKey* m_pSymKey;
    SECItem* m_pSecParam;
    PK11Context* m_pContext;

    sal_Int32 m_nBlockSize;
    ::com::sun::star::uno::Sequence< sal_Int8 > m_aLastBlock;

    bool m_bEncryption;
    bool m_bPadding;
    bool m_bW3CPadding;
    sal_Int64 m_nConverted;

    bool m_bDisposed;
    bool m_bBroken;

    void Dispose();

    OCipherContext()
    : m_pSlot( NULL )
    , m_pSymKey( NULL )
    , m_pSecParam( NULL )
    , m_pContext( NULL )
    , m_nBlockSize( 0 )
    , m_bEncryption( false )
    , m_bPadding( false )
    , m_bW3CPadding( false )
    , m_nConverted( 0 )
    , m_bDisposed( false )
    , m_bBroken( false )
    {}

public:

    virtual ~OCipherContext()
    {
        Dispose();
    }

    static ::com::sun::star::uno::Reference< ::com::sun::star::xml::crypto::XCipherContext > Create( CK_MECHANISM_TYPE nNSSCipherID, const ::com::sun::star::uno::Sequence< ::sal_Int8 >& aKey, const ::com::sun::star::uno::Sequence< ::sal_Int8 >& aInitializationVector, bool bEncryption, bool bW3CPadding );

    // XCipherContext
    virtual ::com::sun::star::uno::Sequence< ::sal_Int8 > SAL_CALL convertWithCipherContext( const ::com::sun::star::uno::Sequence< ::sal_Int8 >& aData ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::DisposedException, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::sal_Int8 > SAL_CALL finalizeCipherContextAndDispose(  ) throw (::com::sun::star::lang::DisposedException, ::com::sun::star::uno::RuntimeException);
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
