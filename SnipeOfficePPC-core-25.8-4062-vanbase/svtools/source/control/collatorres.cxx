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


#include <svtools/svtresid.hxx>
#include <svtools/svtools.hrc>
#include <svtools/collatorres.hxx>

// -------------------------------------------------------------------------
//
//  wrapper for locale specific translations data of collator algorithm
//
// -------------------------------------------------------------------------

class CollatorResourceData
{
    friend class CollatorResource;
    private: /* data */
        rtl::OUString ma_Name;
        rtl::OUString ma_Translation;
    private: /* member functions */
        CollatorResourceData () {}
    public:
        CollatorResourceData ( const rtl::OUString &r_Algorithm, const rtl::OUString &r_Translation)
                    : ma_Name (r_Algorithm), ma_Translation (r_Translation) {}

        const rtl::OUString& GetAlgorithm () const { return ma_Name; }

        const rtl::OUString& GetTranslation () const { return ma_Translation; }

        ~CollatorResourceData () {}

        CollatorResourceData& operator= (const CollatorResourceData& r_From)
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

#define COLLATOR_RESOURCE_COUNT (STR_SVT_COLLATE_END - STR_SVT_COLLATE_START + 1)

CollatorResource::CollatorResource()
{
    mp_Data = new CollatorResourceData[COLLATOR_RESOURCE_COUNT];

    #define ASCSTR(str) rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(str))
    #define RESSTR(rid) SvtResId(rid).toString()

    mp_Data[0] = CollatorResourceData (ASCSTR("alphanumeric"), RESSTR(STR_SVT_COLLATE_ALPHANUMERIC));
    mp_Data[1] = CollatorResourceData (ASCSTR("charset"), RESSTR(STR_SVT_COLLATE_CHARSET));
    mp_Data[2] = CollatorResourceData (ASCSTR("dict"), RESSTR(STR_SVT_COLLATE_DICTIONARY));
    mp_Data[3] = CollatorResourceData (ASCSTR("normal"), RESSTR(STR_SVT_COLLATE_NORMAL));
    mp_Data[4] = CollatorResourceData (ASCSTR("pinyin"), RESSTR(STR_SVT_COLLATE_PINYIN));
    mp_Data[5] = CollatorResourceData (ASCSTR("radical"), RESSTR(STR_SVT_COLLATE_RADICAL));
    mp_Data[6] = CollatorResourceData (ASCSTR("stroke"), RESSTR(STR_SVT_COLLATE_STROKE));
    mp_Data[7] = CollatorResourceData (ASCSTR("unicode"), RESSTR(STR_SVT_COLLATE_UNICODE));
    mp_Data[8] = CollatorResourceData (ASCSTR("zhuyin"), RESSTR(STR_SVT_COLLATE_ZHUYIN));
    mp_Data[9] = CollatorResourceData (ASCSTR("phonebook"), RESSTR(STR_SVT_COLLATE_PHONEBOOK));
    mp_Data[10] = CollatorResourceData (ASCSTR("phonetic (alphanumeric first)"), RESSTR(STR_SVT_COLLATE_PHONETIC_F));
    mp_Data[11] = CollatorResourceData (ASCSTR("phonetic (alphanumeric last)"), RESSTR(STR_SVT_COLLATE_PHONETIC_L));
}

CollatorResource::~CollatorResource()
{
    delete[] mp_Data;
}

const rtl::OUString&
CollatorResource::GetTranslation(const rtl::OUString &r_Algorithm)
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

    for (sal_uInt32 i = 0; i < COLLATOR_RESOURCE_COUNT; i++)
    {
        if (aLocaleFreeAlgorithm == mp_Data[i].GetAlgorithm())
            return mp_Data[i].GetTranslation();
    }

    return r_Algorithm;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
