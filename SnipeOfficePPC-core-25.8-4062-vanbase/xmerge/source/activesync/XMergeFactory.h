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

// XMergeFactory.h: interface for the CXMergeFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMERGEFACTORY_H__3150043C_57FB_4BC8_9104_379506FA6B9F__INCLUDED_)
#define AFX_XMERGEFACTORY_H__3150043C_57FB_4BC8_9104_379506FA6B9F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CXMergeFactory : public IClassFactory
{
private:
    LONG m_cRef;
    virtual ~CXMergeFactory() {};

public:
    CXMergeFactory() : m_cRef(1) {};    // Set reference count when first created


    /********** IUnknown methods **********/
    STDMETHODIMP            QueryInterface(REFIID riid, void **ppvObject);
    STDMETHODIMP_(ULONG)    AddRef(void);
    STDMETHODIMP_(ULONG)    Release();


    /********** IUnknown methods **********/
    STDMETHODIMP            CreateInstance(IUnknown* pUnkOuter, REFIID riid, void **ppvObject);
    STDMETHODIMP            LockServer(BOOL fLock);
};

#endif // !defined(AFX_XMERGEFACTORY_H__3150043C_57FB_4BC8_9104_379506FA6B9F__INCLUDED_)

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
