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


#include <svtools/svtresid.hxx>
#include <svtools/svtools.hrc>
#include <svtools/collatorres.hxx>

// -------------------------------------------------------------------------
//
//  wrapper for locale specific translations data of collator algorithm
//
// -------------------------------------------------------------------------

class CollatorRessourceData
{
    friend class CollatorRessource;
    private: /* data */
        rtl::OUString ma_Name;
        rtl::OUString ma_Translation;
    private: /* member functions */
        CollatorRessourceData () {}
    public:
        CollatorRessourceData ( const rtl::OUString &r_Algorithm, const rtl::OUString &r_Translation)
                    : ma_Name (r_Algorithm), ma_Translation (r_Translation) {}

        const rtl::OUString& GetAlgorithm () const { return ma_Name; }

        const rtl::OUString& GetTranslation () const { return ma_Translation; }

        ~CollatorRessourceData () {}

        CollatorRessourceData& operator= (const CollatorRessourceData& r_From)
        {
            ma_Name         = r_From.GetAlgorithm();
            ma_Translation  = r_From.GetTranslation();
            return *this;
        }
};

// -------------------------------------------------------------------------
//
//  implementation of the collator-algorithm-name translation
//
// -------------------------------------------------------------------------

#define COLLATOR_RESSOURCE_COUNT (STR_SVT_COLLATE_END - STR_SVT_COLLATE_START + 1)

CollatorRessource::CollatorRessource()
{
    mp_Data = new CollatorRessourceData[COLLATOR_RESSOURCE_COUNT];

    #define ASCSTR(str) rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(str))
    #define RESSTR(rid) SvtResId(rid).toString()

    mp_Data[0] = CollatorRessourceData (ASCSTR("alphanumeric"), RESSTR(STR_SVT_COLLATE_ALPHANUMERIC));
    mp_Data[1] = CollatorRessourceData (ASCSTR("charset"), RESSTR(STR_SVT_COLLATE_CHARSET));
    mp_Data[2] = CollatorRessourceData (ASCSTR("dict"), RESSTR(STR_SVT_COLLATE_DICTIONARY));
    mp_Data[3] = CollatorRessourceData (ASCSTR("normal"), RESSTR(STR_SVT_COLLATE_NORMAL));
    mp_Data[4] = CollatorRessourceData (ASCSTR("pinyin"), RESSTR(STR_SVT_COLLATE_PINYIN));
    mp_Data[5] = CollatorRessourceData (ASCSTR("radical"), RESSTR(STR_SVT_COLLATE_RADICAL));
    mp_Data[6] = CollatorRessourceData (ASCSTR("stroke"), RESSTR(STR_SVT_COLLATE_STROKE));
    mp_Data[7] = CollatorRessourceData (ASCSTR("unicode"), RESSTR(STR_SVT_COLLATE_UNICODE));
    mp_Data[8] = CollatorRessourceData (ASCSTR("zhuyin"), RESSTR(STR_SVT_COLLATE_ZHUYIN));
    mp_Data[9] = CollatorRessourceData (ASCSTR("phonebook"), RESSTR(STR_SVT_COLLATE_PHONEBOOK));
    mp_Data[10] = CollatorRessourceData (ASCSTR("phonetic (alphanumeric first)"), RESSTR(STR_SVT_COLLATE_PHONETIC_F));
    mp_Data[11] = CollatorRessourceData (ASCSTR("phonetic (alphanumeric last)"), RESSTR(STR_SVT_COLLATE_PHONETIC_L));
}

CollatorRessource::~CollatorRessource()
{
    delete[] mp_Data;
}

const rtl::OUString&
CollatorRessource::GetTranslation(const rtl::OUString &r_Algorithm)
{
    sal_Int32 nIndex = r_Algorithm.indexOf('.');
    rtl::OUString aLocaleFreeAlgorithm;

    if (nIndex == -1)
    {
        aLocaleFreeAlgorithm = r_Algorithm;
    }
    else
    {
        nIndex += 1;
        aLocaleFreeAlgorithm = r_Algorithm.copy(nIndex, r_Algorithm.getLength() - nIndex);
    }

    for (sal_uInt32 i = 0; i < COLLATOR_RESSOURCE_COUNT; i++)
    {
        if (aLocaleFreeAlgorithm == mp_Data[i].GetAlgorithm())
            return mp_Data[i].GetTranslation();
    }

    return r_Algorithm;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
