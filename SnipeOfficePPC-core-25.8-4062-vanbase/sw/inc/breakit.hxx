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

#ifndef _BREAKIT_HXX
#define _BREAKIT_HXX

#include <boost/noncopyable.hpp>
#include <com/sun/star/uno/Reference.h>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/i18n/XBreakIterator.hpp>
#include <com/sun/star/i18n/XScriptTypeDetector.hpp>
#include <com/sun/star/i18n/ForbiddenCharacters.hpp>
#include <i18npool/languagetag.hxx>
#include <swdllapi.h>

/*************************************************************************
 *                      class SwBreakIt
 *************************************************************************/


class SW_DLLPUBLIC SwBreakIt : private ::boost::noncopyable
{
    com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext > m_xContext;
    mutable com::sun::star::uno::Reference< com::sun::star::i18n::XBreakIterator > xBreak;

    LanguageTag * m_pLanguageTag;   ///< language tag of the current locale
    com::sun::star::i18n::ForbiddenCharacters * m_pForbidden;

    LanguageType aForbiddenLang; ///< language of the current forbiddenChar struct

    void _GetLocale( const LanguageType aLang );
    void _GetLocale( const LanguageTag& rLanguageTag );
    void _GetForbidden( const LanguageType  aLang );

    void createBreakIterator() const;

    // private (see @ _Create, _Delete).
    explicit SwBreakIt(
        const com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext > & rxContext);
    ~SwBreakIt();

public:
    // private (see @ source/core/bastyp/init.cxx).
    static void _Create(
        const com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext > & rxContext);
    static void _Delete();

public:
    static SwBreakIt * Get();

    com::sun::star::uno::Reference< com::sun::star::i18n::XBreakIterator > GetBreakIter()
    {
        createBreakIterator();
        return xBreak;
    }

    const com::sun::star::lang::Locale& GetLocale( const LanguageType aLang )
    {
        if( !m_pLanguageTag || m_pLanguageTag->getLanguageType() != aLang )
            _GetLocale( aLang );
        return m_pLanguageTag->getLocale();
    }

    const com::sun::star::lang::Locale& GetLocale( const LanguageTag& rLanguageTag )
    {
        // Use LanguageType comparison instead of LanguageTag::operator!=()
        // because here the LanguageTag is already a known LanguageType value
        // assigned, so LanguageTag does not need to convert to BCP47 for
        // comparison.
        if( !m_pLanguageTag || m_pLanguageTag->getLanguageType() != rLanguageTag.getLanguageType() )
            _GetLocale( rLanguageTag );
        return m_pLanguageTag->getLocale();
    }

    const LanguageTag& GetLanguageTag( const LanguageType aLang )
    {
        if( !m_pLanguageTag || m_pLanguageTag->getLanguageType() != aLang )
            _GetLocale( aLang );
        return *m_pLanguageTag;
    }

    const LanguageTag& GetLanguageTag( const LanguageTag& rLanguageTag )
    {
        // Use LanguageType comparison instead of LanguageTag::operator!=()
        // because here the LanguageTag is already a known LanguageType value
        // assigned, so LanguageTag does not need to convert to BCP47 for
        // comparison.
        if( !m_pLanguageTag || m_pLanguageTag->getLanguageType() != rLanguageTag.getLanguageType() )
            _GetLocale( rLanguageTag );
        return *m_pLanguageTag;
    }

    const com::sun::star::i18n::ForbiddenCharacters& GetForbidden( const LanguageType aLang )
    {
        if( !m_pForbidden || aForbiddenLang != aLang )
            _GetForbidden( aLang );
        return *m_pForbidden;
    }

    sal_uInt16 GetRealScriptOfText( const rtl::OUString& rTxt, sal_Int32 nPos ) const;
    sal_uInt16 GetAllScriptsOfText( const rtl::OUString& rTxt ) const;

    sal_Int32 getGraphemeCount(const rtl::OUString& rStr,
        sal_Int32 nStart, sal_Int32 nEnd) const;
    sal_Int32 getGraphemeCount(const rtl::OUString& rStr) const
    {
        return getGraphemeCount(rStr, 0, rStr.getLength());
    }
};

#define SW_BREAKITER()  SwBreakIt::Get()

// @@@ backward compatibility @@@
SW_DLLPUBLIC extern SwBreakIt* pBreakIt;

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
