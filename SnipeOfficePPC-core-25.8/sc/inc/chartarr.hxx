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

#ifndef SC_CHARTARR_HXX
#define SC_CHARTARR_HXX

// -----------------------------------------------------------------------

#include "rangelst.hxx"
#include "chartpos.hxx"

#include <boost/ptr_container/ptr_vector.hpp>

class ScDocument;

// ScMemChart is a stripped-down SchMemChart from old chart,
// used only to transport a rectangular data array for the UNO API,
// contains only column/row header text and data values.

class ScMemChart
{
    short           nRowCnt;
    short           nColCnt;
    double*         pData;
    ::rtl::OUString* pColText;
    ::rtl::OUString* pRowText;

    ScMemChart(const ScMemChart& rMemChart);      // not implemented

public:
    ScMemChart(short nCols, short nRows);
    ~ScMemChart();

    short GetColCount() const { return nColCnt; }
    short GetRowCount() const { return nRowCnt; }
    const ::rtl::OUString& GetColText(short nCol) const { return pColText[nCol]; }
    const ::rtl::OUString& GetRowText(short nRow) const { return pRowText[nRow]; }
    double GetData(short nCol, short nRow) const { return pData[nCol * nRowCnt + nRow]; }
    void SetData(short nCol, short nRow, const double& rVal) { pData[nCol * nRowCnt + nRow] = rVal; }
    void SetColText(short nCol, const ::rtl::OUString& rText) { pColText[nCol] = rText; }
    void SetRowText(short nRow, const ::rtl::OUString& rText) { pRowText[nRow] = rText; }
};

class SC_DLLPUBLIC ScChartArray             // only parameter-struct
{
    ::rtl::OUString aName;
    ScDocument* pDocument;
    ScChartPositioner aPositioner;
    bool        bValid;             // for creation out of SchMemChart

private:
    ScMemChart* CreateMemChartSingle();
    ScMemChart* CreateMemChartMulti();
public:
    ScChartArray( ScDocument* pDoc, SCTAB nTab,
                  SCCOL nStartColP, SCROW nStartRowP,
                  SCCOL nEndColP, SCROW nEndRowP,
                  const ::rtl::OUString& rChartName );
    ScChartArray( ScDocument* pDoc, const ScRangeListRef& rRangeList,
                  const ::rtl::OUString& rChartName );
    ScChartArray( const ScChartArray& rArr );
    ~ScChartArray();

    const ScRangeListRef&   GetRangeList() const { return aPositioner.GetRangeList(); }
    void    SetRangeList( const ScRangeListRef& rNew ) { aPositioner.SetRangeList(rNew); }
    void    SetRangeList( const ScRange& rNew ) { aPositioner.SetRangeList(rNew); }
    const   ScChartPositionMap* GetPositionMap() { return aPositioner.GetPositionMap(); }

    void    SetHeaders(bool bCol, bool bRow) { aPositioner.SetHeaders(bCol, bRow); }
    bool    HasColHeaders() const { return aPositioner.HasColHeaders(); }
    bool    HasRowHeaders() const { return aPositioner.HasRowHeaders(); }
    bool IsValid() const { return bValid; }
    void SetName(const ::rtl::OUString& rNew) { aName = rNew; }
    const ::rtl::OUString& GetName() const { return aName; }

    bool operator==(const ScChartArray& rCmp) const;

    ScMemChart* CreateMemChart();
};

class ScChartCollection
{
    typedef ::boost::ptr_vector<ScChartArray> DataType;
    DataType maData;
public:
    ScChartCollection();
    ScChartCollection(const ScChartCollection& rColl);

    SC_DLLPUBLIC void push_back(ScChartArray* p);
    void clear();
    size_t size() const;
    bool empty() const;
    ScChartArray* operator[](size_t nIndex);
    const ScChartArray* operator[](size_t nIndex) const;

    bool operator==(const ScChartCollection& rCmp) const;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
