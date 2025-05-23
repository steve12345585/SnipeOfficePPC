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

#include <rtl/ustrbuf.hxx>
#include <indexentrysupplier.hxx>
#include <localedata.hxx>

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::rtl;

static const sal_Unicode under = sal_Unicode('_');

namespace com { namespace sun { namespace star { namespace i18n {

IndexEntrySupplier::IndexEntrySupplier( const Reference < XMultiServiceFactory >& rxMSF ) : xMSF( rxMSF )
{
}

Sequence < Locale > SAL_CALL IndexEntrySupplier::getLocaleList() throw (RuntimeException)
{
        return LocaleData().getAllInstalledLocaleNames();
}

Sequence < OUString > SAL_CALL IndexEntrySupplier::getAlgorithmList( const Locale& rLocale ) throw (RuntimeException)
{
        return LocaleData().getIndexAlgorithm(rLocale);
}

sal_Bool SAL_CALL IndexEntrySupplier::loadAlgorithm( const Locale& rLocale, const OUString& SortAlgorithm,
        sal_Int32 collatorOptions ) throw (RuntimeException)
{
        Sequence < OUString > algorithmList = getAlgorithmList( rLocale );
        for (sal_Int32 i = 0; i < algorithmList.getLength(); i++) {
            if (algorithmList[i] == SortAlgorithm) {
                if (getLocaleSpecificIndexEntrySupplier(rLocale, SortAlgorithm).is())
                    return xIES->loadAlgorithm(rLocale, SortAlgorithm, collatorOptions);
            }
        }
        return sal_False;
}

sal_Bool SAL_CALL IndexEntrySupplier::usePhoneticEntry( const Locale& rLocale ) throw (RuntimeException)
{
        return LocaleData().hasPhonetic(rLocale);
}

OUString SAL_CALL IndexEntrySupplier::getPhoneticCandidate( const OUString& rIndexEntry,
        const Locale& rLocale ) throw (RuntimeException)
{
        if (getLocaleSpecificIndexEntrySupplier(rLocale, OUString()).is())
            return xIES->getPhoneticCandidate(rIndexEntry, rLocale);
        else
            throw RuntimeException();
}

OUString SAL_CALL IndexEntrySupplier::getIndexKey( const OUString& rIndexEntry,
        const OUString& rPhoneticEntry, const Locale& rLocale ) throw (RuntimeException)
{
        if (xIES.is())
            return xIES->getIndexKey(rIndexEntry, rPhoneticEntry, rLocale);
        else
            throw RuntimeException();
}

sal_Int16 SAL_CALL IndexEntrySupplier::compareIndexEntry(
        const OUString& rIndexEntry1, const OUString& rPhoneticEntry1, const Locale& rLocale1,
        const OUString& rIndexEntry2, const OUString& rPhoneticEntry2, const Locale& rLocale2 )
        throw (com::sun::star::uno::RuntimeException)
{
        if (xIES.is())
            return xIES->compareIndexEntry(rIndexEntry1, rPhoneticEntry1, rLocale1,
                                        rIndexEntry2, rPhoneticEntry2, rLocale2);
        else
            throw RuntimeException();
}

OUString SAL_CALL IndexEntrySupplier::getIndexCharacter( const OUString& rIndexEntry,
        const Locale& rLocale, const OUString& rSortAlgorithm )
        throw (RuntimeException)
{
        return getLocaleSpecificIndexEntrySupplier(rLocale, rSortAlgorithm)->
                        getIndexCharacter( rIndexEntry, rLocale, rSortAlgorithm );
}

sal_Bool SAL_CALL IndexEntrySupplier::createLocaleSpecificIndexEntrySupplier(const OUString& name) throw( RuntimeException )
{
        Reference < XInterface > xI = xMSF->createInstance(
            OUString("com.sun.star.i18n.IndexEntrySupplier_") + name);

        if ( xI.is() ) {
            xI->queryInterface( ::getCppuType((const Reference< com::sun::star::i18n::XExtendedIndexEntrySupplier>*)0) ) >>= xIES;
            return xIES.is();
        }
        return sal_False;
}

Reference < com::sun::star::i18n::XExtendedIndexEntrySupplier > SAL_CALL
IndexEntrySupplier::getLocaleSpecificIndexEntrySupplier(const Locale& rLocale, const OUString& rSortAlgorithm) throw (RuntimeException)
{
        if (xIES.is() && rSortAlgorithm == aSortAlgorithm && rLocale.Language == aLocale.Language &&
                rLocale.Country == aLocale.Country && rLocale.Variant == aLocale.Variant)
            return xIES;
        else if (xMSF.is()) {
            LocaleData ld;
            aLocale = rLocale;
            if (rSortAlgorithm.isEmpty())
                aSortAlgorithm = ld.getDefaultIndexAlgorithm( rLocale );
            else
                aSortAlgorithm = rSortAlgorithm;

            OUString module = ld.getIndexModuleByAlgorithm(rLocale, aSortAlgorithm);
            if (!module.isEmpty() && createLocaleSpecificIndexEntrySupplier(module))
                return xIES;

            sal_Int32 l = rLocale.Language.getLength();
            sal_Int32 c = rLocale.Country.getLength();
            sal_Int32 v = rLocale.Variant.getLength();
            sal_Int32 a = aSortAlgorithm.getLength();
            OUStringBuffer aBuf(l+c+v+a+4);

            if ((l > 0 && c > 0 && v > 0 && a > 0 &&
                        // load service with name <base>_<lang>_<country>_<varian>_<algorithm>
                        createLocaleSpecificIndexEntrySupplier(aBuf.append(rLocale.Language).append(under).append(
                                    rLocale.Country).append(under).append(rLocale.Variant).append(under).append(
                                    aSortAlgorithm).makeStringAndClear())) ||
                (l > 0 && c > 0 && a > 0 &&
                        // load service with name <base>_<lang>_<country>_<algorithm>
                        createLocaleSpecificIndexEntrySupplier(aBuf.append(rLocale.Language).append(under).append(
                                    rLocale.Country).append(under).append(aSortAlgorithm).makeStringAndClear())) ||
                (l > 0 && c > 0 && a > 0 && rLocale.Language.compareToAscii("zh") == 0 &&
                                            (rLocale.Country.compareToAscii("HK") == 0 ||
                                            rLocale.Country.compareToAscii("MO") == 0) &&
                        // if the country code is HK or MO, one more step to try TW.
                        createLocaleSpecificIndexEntrySupplier(aBuf.append(rLocale.Language).append(under).appendAscii(
                                    "TW").append(under).append(aSortAlgorithm).makeStringAndClear())) ||
                (l > 0 && a > 0 &&
                        // load service with name <base>_<lang>_<algorithm>
                        createLocaleSpecificIndexEntrySupplier(aBuf.append(rLocale.Language).append(under).append(
                                    aSortAlgorithm).makeStringAndClear())) ||
                        // load service with name <base>_<algorithm>
                (a > 0 && createLocaleSpecificIndexEntrySupplier(aSortAlgorithm)) ||
                        // load default service with name <base>_Unicode
                        createLocaleSpecificIndexEntrySupplier(OUString("Unicode"))) {
                return xIES;
            }
        }
        throw RuntimeException();
}

OUString SAL_CALL IndexEntrySupplier::getIndexFollowPageWord( sal_Bool bMorePages,
        const Locale& rLocale ) throw (RuntimeException)
{
        Sequence< OUString > aFollowPageWords = LocaleData().getFollowPageWords(rLocale);

        return (bMorePages && aFollowPageWords.getLength() > 1) ?
            aFollowPageWords[1] : (aFollowPageWords.getLength() > 0 ?
            aFollowPageWords[0] : OUString());
}

#define implementationName "com.sun.star.i18n.IndexEntrySupplier"

OUString SAL_CALL
IndexEntrySupplier::getImplementationName() throw( RuntimeException )
{
        return OUString::createFromAscii( implementationName );
}

sal_Bool SAL_CALL
IndexEntrySupplier::supportsService(const OUString& rServiceName) throw( RuntimeException )
{
        return rServiceName.compareToAscii(implementationName) == 0;
}

Sequence< OUString > SAL_CALL
IndexEntrySupplier::getSupportedServiceNames() throw( RuntimeException )
{
        Sequence< OUString > aRet(1);
        aRet[0] = OUString::createFromAscii( implementationName );
        return aRet;
}

} } } }

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
