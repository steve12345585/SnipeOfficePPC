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
#ifndef _I18N_TRANSLITERATION_TRANSLITERATION_COMMONCLASS_H_
#define _I18N_TRANSLITERATION_TRANSLITERATION_COMMONCLASS_H_

#include <com/sun/star/i18n/XExtendedTransliteration.hpp>
#include <com/sun/star/i18n/TransliterationType.hpp>
#include <cppuhelper/implbase1.hxx>
#include <rtl/ustrbuf.h>
#include <rtl/ustring.hxx>

namespace com { namespace sun { namespace star { namespace i18n {

class transliteration_commonclass : public cppu::WeakImplHelper1< com::sun::star::i18n::XExtendedTransliteration >
{
public:
        transliteration_commonclass();

        // Methods which are shared.
        void SAL_CALL
        loadModule( TransliterationModules modName, const com::sun::star::lang::Locale& rLocale )
        throw(com::sun::star::uno::RuntimeException);

        void SAL_CALL
        loadModuleNew( const com::sun::star::uno::Sequence< TransliterationModulesNew >& modName, const com::sun::star::lang::Locale& rLocale )
        throw(com::sun::star::uno::RuntimeException);

        void SAL_CALL
        loadModuleByImplName( const rtl::OUString& implName, const com::sun::star::lang::Locale& rLocale )
        throw(com::sun::star::uno::RuntimeException);

        void SAL_CALL
        loadModulesByImplNames(const com::sun::star::uno::Sequence< rtl::OUString >& modNamelist, const com::sun::star::lang::Locale& rLocale)
        throw(com::sun::star::uno::RuntimeException);

        com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL
        getAvailableModules( const com::sun::star::lang::Locale& rLocale, sal_Int16 sType )
        throw(com::sun::star::uno::RuntimeException);

        // Methods which should be implemented in each transliteration module.
        virtual rtl::OUString SAL_CALL getName() throw(com::sun::star::uno::RuntimeException);

        virtual sal_Int16 SAL_CALL getType(  ) throw(com::sun::star::uno::RuntimeException) = 0;

        virtual rtl::OUString SAL_CALL
        transliterate( const rtl::OUString& inStr, sal_Int32 startPos, sal_Int32 nCount, com::sun::star::uno::Sequence< sal_Int32 >& offset  )
        throw(com::sun::star::uno::RuntimeException) = 0;

        virtual rtl::OUString SAL_CALL
        folding( const rtl::OUString& inStr, sal_Int32 startPos, sal_Int32 nCount, com::sun::star::uno::Sequence< sal_Int32 >& offset)
        throw(com::sun::star::uno::RuntimeException) = 0;

        // Methods in XExtendedTransliteration
        virtual rtl::OUString SAL_CALL
        transliterateString2String( const rtl::OUString& inStr, sal_Int32 startPos, sal_Int32 nCount )
        throw(com::sun::star::uno::RuntimeException);
        virtual rtl::OUString SAL_CALL
        transliterateChar2String( sal_Unicode inChar)
        throw(com::sun::star::uno::RuntimeException);
        virtual sal_Unicode SAL_CALL
        transliterateChar2Char( sal_Unicode inChar )
        throw(com::sun::star::i18n::MultipleCharsOutputException,
                com::sun::star::uno::RuntimeException) = 0;

        virtual sal_Bool SAL_CALL
        equals( const rtl::OUString& str1, sal_Int32 pos1, sal_Int32 nCount1, sal_Int32& nMatch1, const rtl::OUString& str2, sal_Int32 pos2, sal_Int32 nCount2, sal_Int32& nMatch2 )
        throw(com::sun::star::uno::RuntimeException) = 0;

        virtual com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL
        transliterateRange( const rtl::OUString& str1, const rtl::OUString& str2 )
        throw(com::sun::star::uno::RuntimeException) = 0;

        virtual sal_Int32 SAL_CALL
        compareSubstring( const rtl::OUString& s1, sal_Int32 off1, sal_Int32 len1, const rtl::OUString& s2, sal_Int32 off2, sal_Int32 len2)
        throw(com::sun::star::uno::RuntimeException);

        virtual sal_Int32 SAL_CALL
        compareString( const rtl::OUString& s1, const rtl::OUString& s2)
        throw(com::sun::star::uno::RuntimeException);

        //XServiceInfo
        virtual rtl::OUString SAL_CALL getImplementationName()
            throw( com::sun::star::uno::RuntimeException );
        virtual sal_Bool SAL_CALL supportsService(const rtl::OUString& ServiceName)
            throw( com::sun::star::uno::RuntimeException );
        virtual com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames()
            throw( com::sun::star::uno::RuntimeException );
protected:
        com::sun::star::lang::Locale   aLocale;
        const sal_Char* transliterationName;
        const sal_Char* implementationName;
        sal_Bool useOffset;
};

} } } }

#endif // _I18N_TRANSLITERATION_TRANSLITERATION_COMMONCLASS_H_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
