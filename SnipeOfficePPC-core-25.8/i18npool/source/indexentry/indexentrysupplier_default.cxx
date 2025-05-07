/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#include <indexentrysupplier_default.hxx>
#include <localedata.hxx>
#include <i18nutil/unicode.hxx>
#include <com/sun/star/i18n/CollatorOptions.hpp>

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::rtl;

namespace com { namespace sun { namespace star { namespace i18n {

IndexEntrySupplier_Unicode::IndexEntrySupplier_Unicode(
    const com::sun::star::uno::Reference < com::sun::star::lang::XMultiServiceFactory >& rxMSF ) :
    IndexEntrySupplier_Common(rxMSF)
{
    implementationName = "com.sun.star.i18n.IndexEntrySupplier_Unicode";
    index = new Index(rxMSF);
}

IndexEntrySupplier_Unicode::~IndexEntrySupplier_Unicode()
{
    delete index;
}

sal_Bool SAL_CALL IndexEntrySupplier_Unicode::loadAlgorithm( const lang::Locale& rLocale,
    const OUString& rAlgorithm, sal_Int32 collatorOptions ) throw (RuntimeException)
{
    index->init(rLocale, rAlgorithm);
    return IndexEntrySupplier_Common::loadAlgorithm(rLocale, rAlgorithm, collatorOptions);
}

OUString SAL_CALL IndexEntrySupplier_Unicode::getIndexKey( const OUString& rIndexEntry,
    const OUString& rPhoneticEntry, const lang::Locale& rLocale ) throw (RuntimeException)
{
    return index->getIndexDescription(getEntry(rIndexEntry, rPhoneticEntry, rLocale));
}

sal_Int16 SAL_CALL IndexEntrySupplier_Unicode::compareIndexEntry(
    const OUString& rIndexEntry1, const OUString& rPhoneticEntry1, const lang::Locale& rLocale1,
    const OUString& rIndexEntry2, const OUString& rPhoneticEntry2, const lang::Locale& rLocale2 )
    throw (RuntimeException)
{
    sal_Int16 result =
            index->getIndexWeight(getEntry(rIndexEntry1, rPhoneticEntry1, rLocale1)) -
            index->getIndexWeight(getEntry(rIndexEntry2, rPhoneticEntry2, rLocale2));
    if (result == 0)
        return IndexEntrySupplier_Common::compareIndexEntry(
                    rIndexEntry1, rPhoneticEntry1, rLocale1,
                    rIndexEntry2, rPhoneticEntry2, rLocale2);
    return result > 0 ? 1 : -1;
}

OUString SAL_CALL IndexEntrySupplier_Unicode::getIndexCharacter( const OUString& rIndexEntry,
    const lang::Locale& rLocale, const OUString& rAlgorithm ) throw (RuntimeException) {

    if (loadAlgorithm( rLocale, rAlgorithm, CollatorOptions::CollatorOptions_IGNORE_CASE_ACCENT))
        return index->getIndexDescription(rIndexEntry);
    else
        return IndexEntrySupplier_Common::getIndexCharacter(rIndexEntry, rLocale, rAlgorithm);
}

IndexTable::IndexTable()
{
    table = NULL;
}

IndexTable::~IndexTable()
{
    if (table) free(table);
}

void IndexTable::init(sal_Unicode start_, sal_Unicode end_, IndexKey *keys, sal_Int16 key_count, Index *index)
{
    start=start_;
    end=end_;
    table = (sal_uInt8*) malloc((end-start+1)*sizeof(sal_uInt8));
    for (sal_Unicode i = start; i <= end; i++) {
        sal_Int16 j;
        for (j = 0; j < key_count; j++) {
            if (keys[j].key > 0 && (i == keys[j].key || index->compare(i, keys[j].key) == 0)) {
                table[i-start] = sal::static_int_cast<sal_uInt8>(j);
                break;
            }
        }
        if (j == key_count)
            table[i-start] = 0xFF;
    }
}

Index::Index(const com::sun::star::uno::Reference < com::sun::star::lang::XMultiServiceFactory >& rxMSF)
{
    collator = new CollatorImpl(rxMSF);
}

Index::~Index()
{
    delete collator;
}

sal_Int16 Index::compare(sal_Unicode c1, sal_Unicode c2)
{
    return sal::static_int_cast<sal_Int16>( collator->compareString(OUString(&c1, 1), OUString(&c2, 1)) );
}

sal_Int16 Index::getIndexWeight(const OUString& rIndexEntry)
{
    sal_Int32 startPos=0;
    if (!skipping_chars.isEmpty())
        while (skipping_chars.indexOf(rIndexEntry[startPos]) >= 0)
            startPos++;
    if (mkey_count > 0) {
        for (sal_Int16 i = 0; i < mkey_count; i++) {
            sal_Int32 len = keys[mkeys[i]].mkey.getLength();
            if (collator->compareSubstring(rIndexEntry, startPos, len,
                                    keys[mkeys[i]].mkey, 0, len) == 0)
                return mkeys[i];
        }
    }
    sal_Unicode code = rIndexEntry[startPos];
    for (sal_Int16 i = 0; i < table_count; i++) {
        if (tables[i].start <= code && code <= tables[i].end)
            return tables[i].table[code-tables[i].start];
    }
    return 0xFF;
}

OUString Index::getIndexDescription(const OUString& rIndexEntry)
{
    sal_Int16 wgt = getIndexWeight(rIndexEntry);
    if (wgt < MAX_KEYS) {
        if (!keys[wgt].desc.isEmpty())
            return keys[wgt].desc;
        else if (keys[wgt].key > 0)
            return OUString(&keys[wgt].key, 1);
        else
            return keys[wgt].mkey;
    }
    sal_Int32 nPos=0;
    sal_uInt32 indexChar=rIndexEntry.iterateCodePoints(&nPos, 0);
    return OUString(&indexChar, 1);
}

#define LOCALE_EN lang::Locale(OUString("en"), OUString(), OUString())

void Index::makeIndexKeys(const lang::Locale &rLocale, const OUString &algorithm) throw (RuntimeException)
{
    OUString keyStr = LocaleData().getIndexKeysByAlgorithm(rLocale, algorithm);

    if (keyStr.isEmpty()) {
        keyStr = LocaleData().getIndexKeysByAlgorithm(LOCALE_EN,
                    LocaleData().getDefaultIndexAlgorithm(LOCALE_EN));
        if (keyStr.isEmpty())
            throw RuntimeException();
    }

    sal_Int16 len = sal::static_int_cast<sal_Int16>( keyStr.getLength() );
    mkey_count=key_count=0;
    skipping_chars=OUString();
    sal_Int16 i, j;

    for (i = 0; i < len && key_count < MAX_KEYS; i++)
    {
        sal_Unicode curr = keyStr[i];
        sal_Unicode close = sal_Unicode(')');

        if (unicode::isWhiteSpace(curr))
            continue;

        switch(curr) {
            case sal_Unicode('-'):
                if (key_count > 0 && i + 1 < len ) {
                    for (curr = keyStr[++i]; key_count < MAX_KEYS && keys[key_count-1].key < curr; key_count++) {
                        keys[key_count].key = keys[key_count-1].key+1;
                        keys[key_count].desc = OUString();
                    }
                } else
                    throw RuntimeException();
                break;
            case sal_Unicode('['):
                for (i++; i < len && keyStr[i] != sal_Unicode(']'); i++) {
                    if (unicode::isWhiteSpace(keyStr[i])) {
                        continue;
                    } else if (keyStr[i] == sal_Unicode('_')) {
                        for (curr=keyStr[i-1]+1;  curr <= keyStr[i+1]; curr++)
                            skipping_chars+=OUString(curr);
                        i+=2;
                    } else {
                        skipping_chars+=OUString(keyStr[i]);
                    }
                }
                break;
            case sal_Unicode('{'):
                close = sal_Unicode('}');
            case sal_Unicode('('):
                if (key_count > 0) {
                    sal_Int16 end = i+1;
                    for (end=i+1; end < len && keyStr[end] != close; end++) ;

                    if (end >= len) // no found
                        throw RuntimeException();
                    if (close == sal_Unicode(')'))
                        keys[key_count-1].desc = keyStr.copy(i+1, end-i-1);
                    else {
                        mkeys[mkey_count++]=key_count;
                        keys[key_count].key = 0;
                        keys[key_count].mkey = keyStr.copy(i+1, end-i-1);
                        keys[key_count++].desc=OUString();
                    }
                    i=end+1;
                } else
                    throw RuntimeException();
                break;
            default:
                keys[key_count].key = curr;
                keys[key_count++].desc = OUString();
                break;
        }
    }
    for (i = 0; i < mkey_count; i++) {
        for (j=i+1; j < mkey_count; j++) {
            if (keys[mkeys[i]].mkey.getLength() < keys[mkeys[j]].mkey.getLength()) {
                sal_Int16 k = mkeys[i];
                mkeys[i] = mkeys[j];
                mkeys[j] = k;
            }
        }
    }
}

void Index::init(const lang::Locale &rLocale, const OUString& algorithm) throw (RuntimeException)
{
    makeIndexKeys(rLocale, algorithm);

    Sequence< UnicodeScript > scriptList = LocaleData().getUnicodeScripts( rLocale );

    if (scriptList.getLength() == 0) {
        scriptList = LocaleData().getUnicodeScripts(LOCALE_EN);
        if (scriptList.getLength() == 0)
            throw RuntimeException();
    }

    table_count = sal::static_int_cast<sal_Int16>( scriptList.getLength() );
    if (table_count > MAX_TABLES)
        throw RuntimeException();

    collator->loadCollatorAlgorithm(algorithm, rLocale, CollatorOptions::CollatorOptions_IGNORE_CASE_ACCENT);
    sal_Int16 j=0;
    sal_Unicode start = unicode::getUnicodeScriptStart((UnicodeScript)0);
    sal_Unicode end = unicode::getUnicodeScriptEnd((UnicodeScript)0);
    for (sal_Int16 i= (scriptList[0] == (UnicodeScript)0) ? 1 : 0; i< scriptList.getLength(); i++) {
        if (unicode::getUnicodeScriptStart(scriptList[i]) != end+1) {
            tables[j++].init(start, end, keys, key_count, this);
            start = unicode::getUnicodeScriptStart(scriptList[i]);
        }
        end = unicode::getUnicodeScriptEnd(scriptList[i]);
    }
    tables[j++].init(start, end, keys, key_count, this);
    table_count = j;
}

} } } }

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
