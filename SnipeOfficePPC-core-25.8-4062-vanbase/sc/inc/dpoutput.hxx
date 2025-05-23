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

#ifndef SC_DPOUTPUT_HXX
#define SC_DPOUTPUT_HXX

#include <com/sun/star/sheet/XDimensionsSupplier.hpp>
#include <com/sun/star/sheet/DataResult.hpp>
#include <com/sun/star/sheet/MemberResult.hpp>
#include <com/sun/star/sheet/GeneralFunction.hpp>
#include <com/sun/star/sheet/DataPilotOutputRangeType.hpp>

#include "global.hxx"
#include "address.hxx"

#include "dpfilteredcache.hxx"
#include "dptypes.hxx"

#include <vector>

namespace com { namespace sun { namespace star { namespace sheet {
    struct DataPilotFieldFilter;
    struct DataPilotTablePositionData;
}}}}

class Rectangle;
class ScDocument;

struct ScDPOutLevelData;


struct ScDPGetPivotDataField
{
    rtl::OUString maFieldName;
    com::sun::star::sheet::GeneralFunction meFunction;

    bool   mbValIsStr;
    rtl::OUString maValStr;
    double mnValNum;

        ScDPGetPivotDataField() :
            meFunction( com::sun::star::sheet::GeneralFunction_NONE ),
            mbValIsStr( false ),
            mnValNum( 0.0 )
        {
        }
};



class ScDPOutput
{
private:
    ScDocument*             pDoc;
    com::sun::star::uno::Reference<
        com::sun::star::sheet::XDimensionsSupplier> xSource;
    ScAddress               aStartPos;
    ScDPOutLevelData*       pColFields;
    ScDPOutLevelData*       pRowFields;
    ScDPOutLevelData*       pPageFields;
    long                    nColFieldCount;
    long                    nRowFieldCount;
    long                    nPageFieldCount;
    com::sun::star::uno::Sequence<
        com::sun::star::uno::Sequence<
            com::sun::star::sheet::DataResult> > aData;
    rtl::OUString           aDataDescription;

    // Number format related parameters
    sal_uInt32*                 pColNumFmt;
    sal_uInt32*                 pRowNumFmt;
    long                    nColFmtCount;
    long                    nRowFmtCount;
    sal_uInt32                  nSingleNumFmt;

    // Output geometry related parameters
    long                    nColCount;
    long                    nRowCount;
    long                    nHeaderSize;
    SCCOL                   nTabStartCol;
    SCROW                   nTabStartRow;
    SCCOL                   nMemberStartCol;
    SCROW                   nMemberStartRow;
    SCCOL                   nDataStartCol;
    SCROW                   nDataStartRow;
    SCCOL                   nTabEndCol;
    SCROW                   nTabEndRow;
    bool                    bDoFilter:1;
    bool                    bResultsError:1;
    bool                    mbHasDataLayout:1;
    bool                    bSizesValid:1;
    bool                    bSizeOverflow:1;
    bool                    mbHeaderLayout:1;  // true : grid, false : standard

    void            DataCell( SCCOL nCol, SCROW nRow, SCTAB nTab,
                                const com::sun::star::sheet::DataResult& rData );
    void            HeaderCell( SCCOL nCol, SCROW nRow, SCTAB nTab,
                                const com::sun::star::sheet::MemberResult& rData,
                                bool bColHeader, long nLevel );

    void FieldCell(SCCOL nCol, SCROW nRow, SCTAB nTab, const ScDPOutLevelData& rData, bool bInTable);

    void            CalcSizes();

    /** Query which sub-area of the table the cell is in. See
        css.sheet.DataPilotTablePositionType for the interpretation of the
        return value. */
    sal_Int32       GetPositionType(const ScAddress& rPos);

public:
                    ScDPOutput( ScDocument* pD,
                                const com::sun::star::uno::Reference<
                                    com::sun::star::sheet::XDimensionsSupplier>& xSrc,
                                const ScAddress& rPos, bool bFilter );
                    ~ScDPOutput();

    void            SetPosition( const ScAddress& rPos );

    void            Output();           //! Refresh?
    ScRange         GetOutputRange( sal_Int32 nRegionType = ::com::sun::star::sheet::DataPilotOutputRangeType::WHOLE );
    long            GetHeaderRows();
    bool            HasError();         // range overflow or exception from source

    void            GetPositionData(const ScAddress& rPos, ::com::sun::star::sheet::DataPilotTablePositionData& rPosData);

    /** Get filtering criteria based on the position of the cell within data
        field region. */
    bool            GetDataResultPositionData(::std::vector< ::com::sun::star::sheet::DataPilotFieldFilter >& rFilters, const ScAddress& rPos);

    /**
     * @return true on success and stores the result in rTarget, or false if
     *         rFilters or rTarget describe something that is not visible.
     */
    bool GetPivotData( ScDPGetPivotDataField& rTarget,
                       const std::vector< ScDPGetPivotDataField >& rFilters );
    long            GetHeaderDim( const ScAddress& rPos, sal_uInt16& rOrient );
    bool GetHeaderDrag(
        const ScAddress& rPos, bool bMouseLeft, bool bMouseTop, long nDragDim,
        Rectangle& rPosRect, sal_uInt16& rOrient, long& rDimPos );
    bool IsFilterButton( const ScAddress& rPos );

    void GetMemberResultNames(ScDPUniqueStringSet& rNames, long nDimension);

    void            SetHeaderLayout(bool bUseGrid);
    bool            GetHeaderLayout() const;

    static void GetDataDimensionNames(
        rtl::OUString& rSourceName, rtl::OUString& rGivenName,
        const com::sun::star::uno::Reference<
            com::sun::star::uno::XInterface>& xDim );
};


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
