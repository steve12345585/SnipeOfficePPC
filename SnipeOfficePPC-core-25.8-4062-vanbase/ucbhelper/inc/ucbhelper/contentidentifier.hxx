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

#ifndef _UCBHELPER_CONTENTIDENTIFIER_HXX
#define _UCBHELPER_CONTENTIDENTIFIER_HXX

#include <cppuhelper/weak.hxx>
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/ucb/XContentIdentifier.hpp>
#include "ucbhelper/ucbhelperdllapi.h"

namespace rtl {
    class OUString;
}

namespace ucbhelper
{

struct ContentIdentifier_Impl;

//=========================================================================

/**
  * This class implements a simple identifier object for UCB contents.
  * It mainly stores and returns the URL as it was passed to the constructor -
  * The only difference is that the URL scheme will be lower cased. This can
  * be done, because URL schemes are never case sensitive.
  */
class UCBHELPER_DLLPUBLIC ContentIdentifier :
                public cppu::OWeakObject,
                public com::sun::star::lang::XTypeProvider,
                  public com::sun::star::ucb::XContentIdentifier
{
public:
    ContentIdentifier( const rtl::OUString& rURL );
    virtual ~ContentIdentifier();

    // XInterface
    virtual com::sun::star::uno::Any SAL_CALL
    queryInterface( const com::sun::star::uno::Type & rType )
        throw( com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL
    acquire() throw();
    virtual void SAL_CALL
    release() throw();

    // XTypeProvider
    virtual com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL
    getImplementationId()
        throw( com::sun::star::uno::RuntimeException );
    virtual com::sun::star::uno::Sequence< com::sun::star::uno::Type > SAL_CALL
    getTypes()
        throw( com::sun::star::uno::RuntimeException );

    // XContentIdentifier
    virtual rtl::OUString SAL_CALL
    getContentIdentifier()
        throw( com::sun::star::uno::RuntimeException );
    virtual rtl::OUString SAL_CALL
    getContentProviderScheme()
        throw( com::sun::star::uno::RuntimeException );

private:
    ContentIdentifier_Impl* m_pImpl;
};

} /* namespace ucbhelper */

#endif /* !_UCBHELPER_CONTENTIDENTIFIER_HXX */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
