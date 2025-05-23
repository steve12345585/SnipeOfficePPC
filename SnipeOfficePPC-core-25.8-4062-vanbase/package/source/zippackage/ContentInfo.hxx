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
#ifndef _CONTENT_INFO_HXX_
#define _CONTENT_INFO_HXX_

#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <ZipPackageFolder.hxx>
#include <ZipPackageStream.hxx>

namespace com { namespace sun { namespace star { namespace packages {
class ContentInfo : public cppu::OWeakObject
{
public:
    com::sun::star::uno::Reference < com::sun::star::lang::XUnoTunnel > xTunnel;
    bool bFolder;
    union
    {
        ZipPackageFolder *pFolder;
        ZipPackageStream *pStream;
    };
    ContentInfo ( ZipPackageStream * pNewStream )
    : xTunnel ( pNewStream )
    , bFolder ( false )
    , pStream ( pNewStream )
    {
    }
    ContentInfo ( ZipPackageFolder * pNewFolder )
    : xTunnel ( pNewFolder )
    , bFolder ( true )
    , pFolder ( pNewFolder )
    {
    }
    virtual ~ContentInfo ()
    {
        if ( bFolder )
            pFolder->releaseUpwardRef();
        else
            pStream->clearParent();
    }
};
} } } }
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
