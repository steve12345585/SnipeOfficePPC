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
#ifndef SC_XMLSUBTI_HXX
#define SC_XMLSUBTI_HXX

#include <xmloff/xmlictxt.hxx>
#include <xmloff/xmlimp.hxx>
#include <com/sun/star/sheet/XSpreadsheet.hpp>
#include <com/sun/star/drawing/XDrawPage.hpp>
#include <com/sun/star/table/CellAddress.hpp>
#include <com/sun/star/table/XCellRange.hpp>
#include <com/sun/star/table/CellRangeAddress.hpp>
#include <com/sun/star/frame/XModel.hpp>

#include "XMLTableShapeResizer.hxx"
#include "formula/grammar.hxx"
#include "tabprotection.hxx"
#include "rangelst.hxx"

class ScXMLImport;

struct ScXMLTabProtectionData
{
    ::rtl::OUString maPassword;
    ScPasswordHash  meHash1;
    ScPasswordHash  meHash2;
    bool            mbProtected;
    bool            mbSelectProtectedCells;
    bool            mbSelectUnprotectedCells;

    ScXMLTabProtectionData();
};

class ScMyTables
{
private:
    ScXMLImport&                        rImport;

    ScMyOLEFixer                        aFixupOLEs;

    ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSpreadsheet > xCurrentSheet;
    ::com::sun::star::uno::Reference< ::com::sun::star::table::XCellRange > xCurrentCellRange;
    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XDrawPage > xDrawPage;
    ::com::sun::star::uno::Reference < ::com::sun::star::drawing::XShapes > xShapes;
    rtl::OUString                       sCurrentSheetName;
    ScAddress                           maCurrentCellPos;
    ScRangeList                         maMatrixRangeList;
    ScXMLTabProtectionData              maProtectionData;
    sal_Int32                           nCurrentColCount;
    sal_Int16                           nCurrentDrawPage;
    sal_Int16                           nCurrentXShapes;

    void                                NewRow();
    void                                NewColumn(bool bIsCovered);

    void                                SetTableStyle(const rtl::OUString& sStyleName);
public:
                                        ScMyTables(ScXMLImport& rImport);
                                        ~ScMyTables();
    void                                NewSheet(const rtl::OUString& sTableName, const rtl::OUString& sStyleName,
                                                 const ScXMLTabProtectionData& rProtectData);
    void                                AddRow();
    void                                SetRowStyle(const rtl::OUString& rCellStyleName);
    void                                AddColumn(bool bIsCovered);
    void                                FixupOLEs() { aFixupOLEs.FixupOLEs(); }
    bool                                IsOLE(com::sun::star::uno::Reference< com::sun::star::drawing::XShape >& rShape) const
                                            { return ScMyOLEFixer::IsOLE(rShape); }
    void                                DeleteTable();
    ScAddress                           GetCurrentCellPos() const { return maCurrentCellPos; };
    void                                AddColStyle(const sal_Int32 nRepeat, const rtl::OUString& rCellStyleName);
    ScXMLTabProtectionData&             GetCurrentProtectionData() { return maProtectionData; }
    rtl::OUString                       GetCurrentSheetName() const { return sCurrentSheetName; }
    SCTAB                               GetCurrentSheet() const { return (maCurrentCellPos.Tab() >= 0) ? maCurrentCellPos.Tab() : 0; }
    SCCOL                               GetCurrentColCount() const { return std::min<sal_Int32>(nCurrentColCount, MAXCOL); }
    SCROW                               GetCurrentRow() const { return (maCurrentCellPos.Row() >= 0) ? maCurrentCellPos.Row() : 0; }
    ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSpreadsheet >
                                        GetCurrentXSheet() const { return xCurrentSheet; }
    ::com::sun::star::uno::Reference< ::com::sun::star::table::XCellRange >
                                        GetCurrentXCellRange() const { return xCurrentCellRange; }
    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XDrawPage >
                                        GetCurrentXDrawPage();
    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >
                                        GetCurrentXShapes();
    bool                                HasDrawPage();
    bool                                HasXShapes();
    void                                AddOLE(com::sun::star::uno::Reference <com::sun::star::drawing::XShape>& rShape,
                                               const rtl::OUString &rRangeList);

    void                                AddMatrixRange( const SCCOL nStartColumn,
                                            const SCROW nStartRow,
                                            const SCCOL nEndColumn,
                                            const SCROW nEndRow,
                                            const rtl::OUString& rFormula,
                                            const rtl::OUString& rFormulaNmsp,
                                            const formula::FormulaGrammar::Grammar );
    bool                                IsPartOfMatrix( const ScAddress& rScAddress) const;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
