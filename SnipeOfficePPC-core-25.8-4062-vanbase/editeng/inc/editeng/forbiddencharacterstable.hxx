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

#ifndef _FORBIDDENCHARACTERSTABLE_HXX
#define _FORBIDDENCHARACTERSTABLE_HXX

#include <salhelper/simplereferenceobject.hxx>
#include <com/sun/star/uno/Reference.hxx>
#include <com/sun/star/i18n/ForbiddenCharacters.hpp>
#include "editeng/editengdllapi.h"
#include <map>

namespace com {
namespace sun {
namespace star {
namespace uno {
    class XComponentContext;
}}}}

class EDITENG_DLLPUBLIC SvxForbiddenCharactersTable : public salhelper::SimpleReferenceObject
{
public:
    typedef std::map<sal_uInt16, com::sun::star::i18n::ForbiddenCharacters> Map;
private:
    Map maMap;
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > m_xContext;

public:
    SvxForbiddenCharactersTable( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& rxContext);
    ~SvxForbiddenCharactersTable() {}

    Map& GetMap() { return maMap; }
    const com::sun::star::i18n::ForbiddenCharacters* GetForbiddenCharacters( sal_uInt16 nLanguage, sal_Bool bGetDefault );
    void    SetForbiddenCharacters(  sal_uInt16 nLanguage , const com::sun::star::i18n::ForbiddenCharacters& );
    void    ClearForbiddenCharacters( sal_uInt16 nLanguage );
};

#endif // _FORBIDDENCHARACTERSTABLE_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
