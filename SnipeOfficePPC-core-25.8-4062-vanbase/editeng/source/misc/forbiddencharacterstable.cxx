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


#include <editeng/forbiddencharacterstable.hxx>

#include <unotools/localedatawrapper.hxx>
#include <editeng/unolingu.hxx>

#include <com/sun/star/lang/XMultiServiceFactory.hpp>

SvxForbiddenCharactersTable::SvxForbiddenCharactersTable( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& rxContext)
{
    m_xContext = rxContext;
}

const com::sun::star::i18n::ForbiddenCharacters* SvxForbiddenCharactersTable::GetForbiddenCharacters( sal_uInt16 nLanguage, sal_Bool bGetDefault )
{
    com::sun::star::i18n::ForbiddenCharacters* pForbiddenCharacters = NULL;
    Map::iterator it = maMap.find( nLanguage );
    if ( it != maMap.end() )
        pForbiddenCharacters = &(it->second);
    else if ( bGetDefault && m_xContext.is() )
    {
        LocaleDataWrapper aWrapper( m_xContext, LanguageTag( nLanguage ) );
        maMap[ nLanguage ] = aWrapper.getForbiddenCharacters();
        pForbiddenCharacters = &maMap[ nLanguage ];
    }
    return pForbiddenCharacters;
}

void SvxForbiddenCharactersTable::SetForbiddenCharacters( sal_uInt16 nLanguage, const com::sun::star::i18n::ForbiddenCharacters& rForbiddenChars )
{
    maMap[ nLanguage ] = rForbiddenChars;
}

void SvxForbiddenCharactersTable::ClearForbiddenCharacters( sal_uInt16 nLanguage )
{
    maMap.erase( nLanguage );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
