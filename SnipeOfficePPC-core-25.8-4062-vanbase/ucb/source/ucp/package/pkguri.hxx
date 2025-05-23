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

#ifndef _PKGURI_HXX
#define _PKGURI_HXX

#include <rtl/ustring.hxx>

namespace package_ucp {

//=========================================================================

#define PACKAGE_URL_SCHEME          "vnd.sun.star.pkg"
#define PACKAGE_ZIP_URL_SCHEME      "vnd.sun.star.zip"
#define PACKAGE_URL_SCHEME_LENGTH   16

//=========================================================================

class PackageUri
{
    mutable ::rtl::OUString m_aUri;
    mutable ::rtl::OUString m_aParentUri;
    mutable ::rtl::OUString m_aPackage;
    mutable ::rtl::OUString m_aPath;
    mutable ::rtl::OUString m_aName;
    mutable ::rtl::OUString m_aParam;
    mutable ::rtl::OUString m_aScheme;
    mutable bool            m_bValid;

private:
    void init() const;

public:
    PackageUri() : m_bValid( false ) {}
    PackageUri( const ::rtl::OUString & rPackageUri )
    : m_aUri( rPackageUri ), m_bValid( false ) {}

    sal_Bool isValid() const
    { init(); return m_bValid; }

    const ::rtl::OUString & getUri() const
    { init(); return m_aUri; }

    void setUri( const ::rtl::OUString & rPackageUri )
    { m_aPath = ::rtl::OUString(); m_aUri = rPackageUri; m_bValid = false; }

    const ::rtl::OUString & getParentUri() const
    { init(); return m_aParentUri; }

    const ::rtl::OUString & getPackage() const
    { init(); return m_aPackage; }

    const ::rtl::OUString & getPath() const
    { init(); return m_aPath; }

    const ::rtl::OUString & getName() const
    { init(); return m_aName; }

    const ::rtl::OUString & getParam() const
    { init(); return m_aParam; }

    const ::rtl::OUString & getScheme() const
    { init(); return m_aScheme; }

    inline sal_Bool isRootFolder() const;
};

inline sal_Bool PackageUri::isRootFolder() const
{
    init();
    return ( ( m_aPath.getLength() == 1 ) &&
             ( m_aPath.getStr()[ 0 ] == sal_Unicode( '/' ) ) );
}

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
