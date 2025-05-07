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

#ifndef _BIBCONFIG_HXX
#define _BIBCONFIG_HXX

#include <unotools/configitem.hxx>
class MappingArray;

//-----------------------------------------------------------------------------
#define COLUMN_COUNT                31
#define IDENTIFIER_POS              0
#define AUTHORITYTYPE_POS           1
#define AUTHOR_POS                  2
#define TITLE_POS                   3
#define YEAR_POS                    4
#define ISBN_POS                    5
#define BOOKTITLE_POS               6
#define CHAPTER_POS                 7
#define EDITION_POS                 8
#define EDITOR_POS                  9
#define HOWPUBLISHED_POS            10
#define INSTITUTION_POS             11
#define JOURNAL_POS                 12
#define MONTH_POS                   13
#define NOTE_POS                    14
#define ANNOTE_POS                  15
#define NUMBER_POS                  16
#define ORGANIZATIONS_POS           17
#define PAGES_POS                   18
#define PUBLISHER_POS               19
#define ADDRESS_POS                 20
#define SCHOOL_POS                  21
#define SERIES_POS                  22
#define REPORTTYPE_POS              23
#define VOLUME_POS                  24
#define URL_POS                     25
#define CUSTOM1_POS                 26
#define CUSTOM2_POS                 27
#define CUSTOM3_POS                 28
#define CUSTOM4_POS                 29
#define CUSTOM5_POS                 30
//-----------------------------------------------------------------------------
struct StringPair
{
    rtl::OUString   sRealColumnName;
    rtl::OUString   sLogicalColumnName;
};
//-----------------------------------------------------------------------------
struct Mapping
{
    rtl::OUString       sTableName;
    rtl::OUString       sURL;
    sal_Int16           nCommandType;
    StringPair          aColumnPairs[COLUMN_COUNT];

    Mapping() :
        nCommandType(0){}
};
//-----------------------------------------------------------------------------
struct BibDBDescriptor
{
    rtl::OUString   sDataSource;
    rtl::OUString   sTableOrQuery;
    sal_Int32       nCommandType;
};
//-----------------------------------------------------------------------------

class BibConfig : public utl::ConfigItem
{
    rtl::OUString   sDataSource;
    rtl::OUString   sTableOrQuery;
    sal_Int32       nTblOrQuery;

    rtl::OUString   sQueryField;
    rtl::OUString   sQueryText;
    MappingArray*               pMappingsArr;
    long            nBeamerSize;
    long            nViewSize;
    sal_Bool        bShowColumnAssignmentWarning;

    rtl::OUString               aColumnDefaults[COLUMN_COUNT];

    com::sun::star::uno::Sequence<rtl::OUString> GetPropertyNames();
public:
    BibConfig();
    ~BibConfig();

    virtual void    Commit();
    virtual void            Notify( const com::sun::star::uno::Sequence<rtl::OUString>& aPropertyNames);

    BibDBDescriptor         GetBibliographyURL();
    void                    SetBibliographyURL(const BibDBDescriptor& rDesc);

    const Mapping*          GetMapping(const BibDBDescriptor& rDesc) const;
    void                    SetMapping(const BibDBDescriptor& rDesc, const Mapping* pMapping);

    const rtl::OUString&    GetDefColumnName(sal_uInt16 nIndex) const
                                            {return aColumnDefaults[nIndex];}


    void                    setBeamerSize(long nSize) {SetModified(); nBeamerSize = nSize;}
    long                    getBeamerSize()const {return nBeamerSize;}
    void                    setViewSize(long nSize) {SetModified(); nViewSize = nSize;}
    long                    getViewSize() {return nViewSize;}

    const rtl::OUString&    getQueryField() const {return sQueryField;}
    void                    setQueryField(const rtl::OUString& rSet) {SetModified(); sQueryField = rSet;}

    const rtl::OUString&    getQueryText() const {return sQueryText;}
    void                    setQueryText(const rtl::OUString& rSet) {SetModified(); sQueryText = rSet;}

    sal_Bool                IsShowColumnAssignmentWarning() const
                                { return bShowColumnAssignmentWarning;}
    void                    SetShowColumnAssignmentWarning(sal_Bool bSet)
                                { bShowColumnAssignmentWarning = bSet;}
};

class DBChangeDialogConfig_Impl
{
    com::sun::star::uno::Sequence<rtl::OUString> aSourceNames;
public:
    DBChangeDialogConfig_Impl();
    ~DBChangeDialogConfig_Impl();

    const com::sun::star::uno::Sequence<rtl::OUString>& GetDataSourceNames();

};
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
