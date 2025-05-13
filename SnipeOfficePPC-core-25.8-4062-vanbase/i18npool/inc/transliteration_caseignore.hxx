/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
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
#ifndef TRANSLITERATION_CASE_IGNORE_H
#define TRANSLITERATION_CASE_IGNORE_H

#define TRANSLITERATION_UPPER_LOWER
#include "transliteration_body.hxx"

namespace com { namespace sun { namespace star { namespace i18n {

class Transliteration_caseignore: public Transliteration_body
{
public:
    Transliteration_caseignore();

    // Methods which are shared.
    sal_Int16 SAL_CALL getType() throw(com::sun::star::uno::RuntimeException);

    void SAL_CALL loadModule( TransliterationModules modName, const com::sun::star::lang::Locale& rLocale )
    throw(com::sun::star::uno::RuntimeException);

    com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL transliterateRange(
        const rtl::OUString& str1, const rtl::OUString& str2 )
    throw(com::sun::star::uno::RuntimeException);

    sal_Bool SAL_CALL equals(
        const rtl::OUString& str1, sal_Int32 pos1, sal_Int32 nCount1, sal_Int32& nMatch1,
        const rtl::OUString& src2, sal_Int32 pos2, sal_Int32 nCount2, sal_Int32& nMatch2)
    throw(com::sun::star::uno::RuntimeException);

    sal_Int32 SAL_CALL compareSubstring(
        const rtl::OUString& s1, sal_Int32 off1, sal_Int32 len1,
        const rtl::OUString& s2, sal_Int32 off2, sal_Int32 len2)
    throw(com::sun::star::uno::RuntimeException);

    sal_Int32 SAL_CALL compareString(
        const rtl::OUString& s1,
        const rtl::OUString& s2)
    throw(com::sun::star::uno::RuntimeException);

protected:
    TransliterationModules moduleLoaded;
private:
    sal_Int32 SAL_CALL compare(
        const rtl::OUString& str1, sal_Int32 pos1, sal_Int32 nCount1, sal_Int32& nMatch1,
        const rtl::OUString& str2, sal_Int32 pos2, sal_Int32 nCount2, sal_Int32& nMatch2)
    throw(com::sun::star::uno::RuntimeException);
};

} } } }

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
