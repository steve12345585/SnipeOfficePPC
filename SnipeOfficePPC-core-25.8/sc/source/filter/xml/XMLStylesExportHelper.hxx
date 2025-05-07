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

#ifndef SC_XMLSTYLESEXPORTHELPER_HXX
#define SC_XMLSTYLESEXPORTHELPER_HXX

#include <vector>
#include <list>
#include <com/sun/star/uno/Any.h>
#include <com/sun/star/table/CellRangeAddress.hpp>
#include <com/sun/star/table/CellAddress.hpp>
#include <com/sun/star/sheet/ConditionOperator.hpp>
#include <com/sun/star/sheet/ValidationAlertStyle.hpp>
#include <com/sun/star/sheet/ValidationType.hpp>

#include <boost/ptr_container/ptr_vector.hpp>
#include <mdds/flat_segment_tree.hpp>

class ScDocument;
class ScXMLExport;

struct ScMyValidation
{
    rtl::OUString               sName;
    rtl::OUString               sErrorMessage;
    rtl::OUString               sErrorTitle;
    rtl::OUString               sImputMessage;
    rtl::OUString               sImputTitle;
    rtl::OUString               sFormula1;
    rtl::OUString               sFormula2;
    com::sun::star::table::CellAddress          aBaseCell;
    com::sun::star::sheet::ValidationAlertStyle aAlertStyle;
    com::sun::star::sheet::ValidationType       aValidationType;
    com::sun::star::sheet::ConditionOperator    aOperator;
    sal_Int16                   nShowList;
    bool                        bShowErrorMessage;
    bool                        bShowImputMessage;
    bool                        bIgnoreBlanks;

                                ScMyValidation();
                                ~ScMyValidation();

    bool                        IsEqual(const ScMyValidation& aVal) const;
};

typedef std::vector<ScMyValidation>         ScMyValidationVec;

class ScMyValidationsContainer
{
private:
    ScMyValidationVec           aValidationVec;
    const rtl::OUString         sEmptyString;
    const rtl::OUString         sERRALSTY;
    const rtl::OUString         sIGNOREBL;
    const rtl::OUString         sSHOWLIST;
    const rtl::OUString         sTYPE;
    const rtl::OUString         sSHOWINP;
    const rtl::OUString         sSHOWERR;
    const rtl::OUString         sINPTITLE;
    const rtl::OUString         sINPMESS;
    const rtl::OUString         sERRTITLE;
    const rtl::OUString         sERRMESS;
    const rtl::OUString         sOnError;
    const rtl::OUString         sEventType;
    const rtl::OUString         sStarBasic;
    const rtl::OUString         sScript;
    const rtl::OUString         sLibrary;
    const rtl::OUString         sMacroName;

public:
                                ScMyValidationsContainer();
                                ~ScMyValidationsContainer();
    bool                        AddValidation(const com::sun::star::uno::Any& aAny,
                                    sal_Int32& nValidationIndex);
    rtl::OUString               GetCondition(ScXMLExport& rExport, const ScMyValidation& aValidation);
    rtl::OUString               GetBaseCellAddress(ScDocument* pDoc, const com::sun::star::table::CellAddress& aCell);
    void                        WriteMessage(ScXMLExport& rExport,
                                    const rtl::OUString& sTitle, const rtl::OUString& sMessage,
                                    const bool bShowMessage, const bool bIsHelpMessage);
    void                        WriteValidations(ScXMLExport& rExport);
    const rtl::OUString&        GetValidationName(const sal_Int32 nIndex);
};

//==============================================================================

struct ScMyDefaultStyle
{
    sal_Int32   nIndex;
    sal_Int32   nRepeat;
    bool        bIsAutoStyle;

    ScMyDefaultStyle() : nIndex(-1), nRepeat(1),
        bIsAutoStyle(true) {}
};

typedef std::vector<ScMyDefaultStyle> ScMyDefaultStyleList;

class ScFormatRangeStyles;

class ScMyDefaultStyles
{
    ScMyDefaultStyleList* pRowDefaults;
    ScMyDefaultStyleList* pColDefaults;

    sal_Int32 GetStyleNameIndex(const ScFormatRangeStyles* pCellStyles,
        const sal_Int32 nTable, const sal_Int32 nPos,
        const sal_Int32 i, const bool bRow, bool& bIsAutoStyle);
    void FillDefaultStyles(const sal_Int32 nTable,
        const sal_Int32 nLastRow, const sal_Int32 nLastCol,
        const ScFormatRangeStyles* pCellStyles, ScDocument* pDoc,
        const bool bRow);
public:
    ScMyDefaultStyles() : pRowDefaults(NULL), pColDefaults(NULL) {}
    ~ScMyDefaultStyles();

    void FillDefaultStyles(const sal_Int32 nTable,
        const sal_Int32 nLastRow, const sal_Int32 nLastCol,
        const ScFormatRangeStyles* pCellStyles, ScDocument* pDoc);

    const ScMyDefaultStyleList* GetRowDefaults() const { return pRowDefaults; }
    const ScMyDefaultStyleList* GetColDefaults() const { return pColDefaults; }
};

struct ScMyRowFormatRange
{
    sal_Int32   nStartColumn;
    sal_Int32   nRepeatColumns;
    sal_Int32   nRepeatRows;
    sal_Int32   nIndex;
    sal_Int32   nValidationIndex;
    bool        bIsAutoStyle;

    ScMyRowFormatRange();
    bool operator<(const ScMyRowFormatRange& rRange) const;
};

class ScRowFormatRanges
{
    typedef std::list<ScMyRowFormatRange> ScMyRowFormatRangesList;
    ScMyRowFormatRangesList     aRowFormatRanges;
    const ScMyDefaultStyleList* pRowDefaults;
    const ScMyDefaultStyleList* pColDefaults;
    sal_uInt32                  nSize;

    void AddRange(const sal_Int32 nPrevStartCol, const sal_Int32 nRepeat, const sal_Int32 nPrevIndex,
        const bool bPrevAutoStyle, const ScMyRowFormatRange& rFormatRange);

public:
    ScRowFormatRanges();
    ScRowFormatRanges(const ScRowFormatRanges* pRanges);
    ~ScRowFormatRanges();

    void SetRowDefaults(const ScMyDefaultStyleList* pDefaults) { pRowDefaults = pDefaults; }
    void SetColDefaults(const ScMyDefaultStyleList* pDefaults) { pColDefaults = pDefaults; }
    void Clear();
    void AddRange(ScMyRowFormatRange& rFormatRange, const sal_Int32 nStartRow);
    bool GetNext(ScMyRowFormatRange& rFormatRange);
    sal_Int32 GetMaxRows() const;
    sal_Int32 GetSize() const;
    void Sort();
};

typedef std::vector<rtl::OUString*>     ScMyOUStringVec;

struct ScMyFormatRange
{
    com::sun::star::table::CellRangeAddress aRangeAddress;
    sal_Int32                               nStyleNameIndex;
    sal_Int32                               nValidationIndex;
    sal_Int32                               nNumberFormat;
    bool                                    bIsAutoStyle;

    ScMyFormatRange();
    bool operator< (const ScMyFormatRange& rRange) const;
};

class ScFormatRangeStyles
{
    typedef std::list<ScMyFormatRange>          ScMyFormatRangeAddresses;
    typedef std::vector<ScMyFormatRangeAddresses*>  ScMyFormatRangeListVec;

    ScMyFormatRangeListVec      aTables;
    ScMyOUStringVec             aStyleNames;
    ScMyOUStringVec             aAutoStyleNames;
    const ScMyDefaultStyleList* pRowDefaults;
    const ScMyDefaultStyleList* pColDefaults;

public:
    ScFormatRangeStyles();
    ~ScFormatRangeStyles();

    void SetRowDefaults(const ScMyDefaultStyleList* pDefaults) { pRowDefaults = pDefaults; }
    void SetColDefaults(const ScMyDefaultStyleList* pDefaults) { pColDefaults = pDefaults; }
    void AddNewTable(const sal_Int32 nTable);
    bool AddStyleName(rtl::OUString* pString, sal_Int32& rIndex, const bool bIsAutoStyle = true);
    sal_Int32 GetIndexOfStyleName(const rtl::OUString& rString, const rtl::OUString& rPrefix, bool& bIsAutoStyle);
    // does not delete ranges
    sal_Int32 GetStyleNameIndex(const sal_Int32 nTable, const sal_Int32 nColumn, const sal_Int32 nRow,
        bool& bIsAutoStyle) const;
    // deletes not necessary ranges if wanted
    sal_Int32 GetStyleNameIndex(const sal_Int32 nTable, const sal_Int32 nColumn, const sal_Int32 nRow,
        bool& bIsAutoStyle, sal_Int32& nValidationIndex, sal_Int32& nNumberFormat, const sal_Int32 nRemoveBeforeRow);
    void GetFormatRanges(const sal_Int32 nStartColumn, const sal_Int32 nEndColumn, const sal_Int32 nRow,
                    const sal_Int32 nTable, ScRowFormatRanges* pFormatRanges);
    void AddRangeStyleName(const com::sun::star::table::CellRangeAddress aCellRangeAddress, const sal_Int32 nStringIndex,
                    const bool bIsAutoStyle, const sal_Int32 nValidationIndex, const sal_Int32 nNumberFormat);
    rtl::OUString* GetStyleNameByIndex(const sal_Int32 nIndex, const bool bIsAutoStyle);
    void Sort();
};

class ScColumnRowStylesBase
{
    ScMyOUStringVec             aStyleNames;

public:
    ScColumnRowStylesBase();
    virtual ~ScColumnRowStylesBase();

    virtual void AddNewTable(const sal_Int32 nTable, const sal_Int32 nFields) = 0;
    sal_Int32 AddStyleName(rtl::OUString* pString);
    sal_Int32 GetIndexOfStyleName(const rtl::OUString& rString, const rtl::OUString& rPrefix);
    virtual rtl::OUString* GetStyleName(const sal_Int32 nTable, const sal_Int32 nField) = 0;
    rtl::OUString* GetStyleNameByIndex(const sal_Int32 nIndex);
};

struct ScColumnStyle
{
    sal_Int32   nIndex;
    bool        bIsVisible;

    ScColumnStyle() : nIndex(-1), bIsVisible(true) {}
};

class ScColumnStyles : public ScColumnRowStylesBase
{
    typedef std::vector<ScColumnStyle>  ScMyColumnStyleVec;
    typedef std::vector<ScMyColumnStyleVec> ScMyColumnVectorVec;
    ScMyColumnVectorVec             aTables;

public:
    ScColumnStyles();
    ~ScColumnStyles();

    virtual void AddNewTable(const sal_Int32 nTable, const sal_Int32 nFields);
    sal_Int32 GetStyleNameIndex(const sal_Int32 nTable, const sal_Int32 nField,
        bool& bIsVisible);
    void AddFieldStyleName(const sal_Int32 nTable, const sal_Int32 nField, const sal_Int32 nStringIndex, const bool bIsVisible);
    virtual rtl::OUString* GetStyleName(const sal_Int32 nTable, const sal_Int32 nField);
};

class ScRowStyles : public ScColumnRowStylesBase
{
    typedef ::mdds::flat_segment_tree<sal_Int32, sal_Int32> StylesType;
    ::boost::ptr_vector<StylesType> aTables;
    struct Cache
    {
        sal_Int32 mnTable;
        sal_Int32 mnStart;
        sal_Int32 mnEnd;
        sal_Int32 mnStyle;
        Cache();

        bool hasCache(sal_Int32 nTable, sal_Int32 nField) const;
    };
    Cache maCache;

public:
    ScRowStyles();
    ~ScRowStyles();

    virtual void AddNewTable(const sal_Int32 nTable, const sal_Int32 nFields);
    sal_Int32 GetStyleNameIndex(const sal_Int32 nTable, const sal_Int32 nField);
    void AddFieldStyleName(const sal_Int32 nTable, const sal_Int32 nField, const sal_Int32 nStringIndex);
    void AddFieldStyleName(const sal_Int32 nTable, const sal_Int32 nStartField, const sal_Int32 nStringIndex, const sal_Int32 nEndField);
    virtual rtl::OUString* GetStyleName(const sal_Int32 nTable, const sal_Int32 nField);
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
