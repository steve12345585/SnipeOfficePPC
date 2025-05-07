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
/*
    IMPORTANT:
    Strictly adhere to the following sequence when creating a pivot table:

    pPivot->SetColFields(aColArr, aColCount)
    pPivot->SetRowFields(aRowArr, aRowCount)
    pPivot->SetDataFields(aDataArr, aDataCount)
    if (pPivot->CreateData())
    {
        pPivotDrawData();
        pPivotReleaseData();
    }

    Make sure that either ColArr or RowArr contains a PivotDataField entry.
*/


#ifndef SC_PIVOT_HXX
#define SC_PIVOT_HXX

#include "global.hxx"
#include "address.hxx"
#include "dpglobal.hxx"

#include <vector>
#include <boost/ptr_container/ptr_vector.hpp>

class SubTotal;

#define PIVOT_DATA_FIELD        (MAXCOLCOUNT)
#define PIVOT_FUNC_REF          (MAXCOLCOUNT)
#include <com/sun/star/uno/Sequence.hxx>
#include <com/sun/star/sheet/DataPilotFieldReference.hpp>
#include <com/sun/star/sheet/DataPilotFieldSortInfo.hpp>
#include <com/sun/star/sheet/DataPilotFieldLayoutInfo.hpp>
#include <com/sun/star/sheet/DataPilotFieldAutoShowInfo.hpp>

class SvStream;
class ScDocument;
class ScUserListData;
class ScProgress;

struct ScDPLabelData;

// -----------------------------------------------------------------------

struct PivotField
{
    SCCOL               nCol; /// 0-based dimension index (not source column index)
    long                mnOriginalDim; /// >= 0 for duplicated field.
    sal_uInt16          nFuncMask;
    sal_uInt8           mnDupCount;
    ::com::sun::star::sheet::DataPilotFieldReference maFieldRef;

    explicit PivotField( SCCOL nNewCol = 0, sal_uInt16 nNewFuncMask = PIVOT_FUNC_NONE );
    PivotField( const PivotField& r );

    long getOriginalDim() const;
    bool                operator==( const PivotField& r ) const;
};

// -----------------------------------------------------------------------

typedef boost::ptr_vector<ScDPLabelData> ScDPLabelDataVec;

struct ScPivotParam
{
    SCCOL           nCol;           // cursor position /
    SCROW           nRow;           // or start of destination area
    SCTAB           nTab;
    ScDPLabelDataVec maLabelArray;
    ::std::vector<PivotField> maPageFields;
    ::std::vector<PivotField> maColFields;
    ::std::vector<PivotField> maRowFields;
    ::std::vector<PivotField> maDataFields;
    bool            bIgnoreEmptyRows;
    bool            bDetectCategories;
    bool            bMakeTotalCol;
    bool            bMakeTotalRow;

    ScPivotParam();
    ScPivotParam( const ScPivotParam& r );
    ~ScPivotParam();

    ScPivotParam&   operator=       ( const ScPivotParam& r );
    bool            operator==      ( const ScPivotParam& r ) const;
    void SetLabelData(const ScDPLabelDataVec& r);
};

//------------------------------------------------------------------------

struct ScDPName
{
    ::rtl::OUString     maName;         /// Original name of the dimension.
    ::rtl::OUString     maLayoutName;   /// Layout name (display name)

    explicit ScDPName(const ::rtl::OUString& rName, const ::rtl::OUString& rLayoutName);
};

// ============================================================================

struct ScDPLabelData
{
    rtl::OUString       maName;         /// Original name of the dimension.
    rtl::OUString       maLayoutName;   /// Layout name (display name)
    rtl::OUString       maSubtotalName;
    SCCOL               mnCol;          /// 0-based field index (not the source column index)
    long                mnOriginalDim;  /// original dimension index (>= 0 for duplicated dimension)
    sal_uInt16          mnFuncMask;     /// Page/Column/Row subtotal function.
    sal_Int32           mnUsedHier;     /// Used hierarchy.
    sal_Int32           mnFlags;        /// Flags from the DataPilotSource dimension
    bool                mbShowAll:1;    /// true = Show all (also empty) results.
    bool                mbIsValue:1;    /// true = Sum or count in data field.
    bool                mbDataLayout:1;

    struct Member
    {
        ::rtl::OUString maName;
        ::rtl::OUString maLayoutName;
        bool mbVisible;
        bool mbShowDetails;

        Member();

        /**
         * return the name that should be displayed in the dp dialogs i.e.
         * when the layout name is present, use it, or else use the original
         * name.
         */
        ::rtl::OUString SC_DLLPUBLIC getDisplayName() const;
    };
    ::std::vector<Member>                               maMembers;
    ::com::sun::star::uno::Sequence< ::rtl::OUString >  maHiers;        /// Hierarchies.
    ::com::sun::star::sheet::DataPilotFieldSortInfo     maSortInfo;     /// Sorting info.
    ::com::sun::star::sheet::DataPilotFieldLayoutInfo   maLayoutInfo;   /// Layout info.
    ::com::sun::star::sheet::DataPilotFieldAutoShowInfo maShowInfo;     /// AutoShow info.

    ScDPLabelData();
    explicit ScDPLabelData(const rtl::OUString& rName, SCCOL nCol, bool bIsValue);

    /**
     * return the name that should be displayed in the dp dialogs i.e. when
     * the layout name is present, use it, or else use the original name.
     */
    ::rtl::OUString SC_DLLPUBLIC getDisplayName() const;
};

// ============================================================================

struct ScPivotField
{
    SCCOL               nCol;
    sal_uInt16          nFuncMask;
    sal_uInt16          nFuncCount;
    ::com::sun::star::sheet::DataPilotFieldReference maFieldRef;

    explicit            ScPivotField( SCCOL nNewCol = 0, sal_uInt16 nNewFuncMask = PIVOT_FUNC_NONE );

    bool                operator==( const ScPivotField& r ) const;
};

// ============================================================================

struct ScDPFuncData
{
    SCCOL               mnCol;
    long                mnOriginalDim;
    sal_uInt16          mnFuncMask;
    sal_uInt8           mnDupCount;
    ::com::sun::star::sheet::DataPilotFieldReference maFieldRef;

    explicit ScDPFuncData( SCCOL nNewCol, sal_uInt16 nNewFuncMask );
    explicit ScDPFuncData(
        SCCOL nNewCol, long nOriginalDim, sal_uInt16 nNewFuncMask, sal_uInt8 nDupCount,
        const ::com::sun::star::sheet::DataPilotFieldReference& rFieldRef );

    bool operator== (const ScDPFuncData& r) const;
};

// ============================================================================

typedef std::vector<ScDPName> ScDPNameVec;

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
