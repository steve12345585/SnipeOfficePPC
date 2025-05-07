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
#ifndef SC_XMLCELLI_HXX
#define SC_XMLCELLI_HXX

#include <memory>
#include "XMLDetectiveContext.hxx"
#include "XMLCellRangeSourceContext.hxx"
#include <xmloff/xmlictxt.hxx>
#include <xmloff/xmlimp.hxx>
#include <com/sun/star/table/XCell.hpp>
#include <com/sun/star/util/DateTime.hpp>
#include <com/sun/star/table/XCellRange.hpp>
#include <com/sun/star/table/CellRangeAddress.hpp>
#include <com/sun/star/table/CellAddress.hpp>
#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>
#include <com/sun/star/document/XActionLockable.hpp>

#include "formula/grammar.hxx"
#include <boost/optional.hpp>

class ScXMLImport;
struct ScXMLAnnotationData;

class ScXMLTableRowCellContext : public SvXMLImportContext
{
    typedef ::std::pair< ::rtl::OUString, ::rtl::OUString > FormulaWithNamespace;
    com::sun::star::uno::Reference<com::sun::star::table::XCell> xBaseCell;
    com::sun::star::uno::Reference<com::sun::star::document::XActionLockable> xLockable;
    ::boost::optional< rtl::OUString > pOUTextValue;
    ::boost::optional< rtl::OUString > pOUTextContent;
    ::boost::optional< FormulaWithNamespace > pOUFormula;
    rtl::OUString* pContentValidationName;
    ::std::auto_ptr< ScXMLAnnotationData > mxAnnotationData;
    ScMyImpDetectiveObjVec* pDetectiveObjVec;
    ScMyImpCellRangeSource* pCellRangeSource;
    double      fValue;
    sal_Int32   nMergedRows, nMergedCols;
    sal_Int32   nMatrixRows, nMatrixCols;
    sal_Int32   nRepeatedRows;
    sal_Int32   nCellsRepeated;
    ScXMLImport& rXMLImport;
    formula::FormulaGrammar::Grammar  eGrammar;
    sal_Int16   nCellType;
    bool        bIsMerged;
    bool        bIsMatrix;
    bool        bHasSubTable;
    bool        bIsCovered;
    bool        bIsEmpty;
    bool        bHasTextImport;
    bool        bIsFirstTextImport;
    bool        bSolarMutexLocked;
    bool        bFormulaTextResult;

    const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
    ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }

    sal_Int16 GetCellType(const rtl::OUString& sOUValue) const;

    void DoMerge(const ScAddress& rScCellPos, const SCCOL nCols, const SCROW nRows);

    void SetContentValidation(com::sun::star::uno::Reference<com::sun::star::beans::XPropertySet>& xPropSet);
    void SetCellProperties(const com::sun::star::uno::Reference<com::sun::star::table::XCellRange>& xCellRange,
                                                const com::sun::star::table::CellAddress& aCellAddress);
    void SetCellProperties(const com::sun::star::uno::Reference<com::sun::star::table::XCell>& xCell);

    void LockSolarMutex();
    void UnlockSolarMutex();

    bool CellExists(const com::sun::star::table::CellAddress& aCellPos) const
    {
        return (aCellPos.Column <= MAXCOL && aCellPos.Row <= MAXROW);
    }

public:

    ScXMLTableRowCellContext( ScXMLImport& rImport, sal_uInt16 nPrfx,
                       const ::rtl::OUString& rLName,
                       const ::com::sun::star::uno::Reference<
                                        ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
                                        const bool bIsCovered, const sal_Int32 nRepeatedRows );

    virtual ~ScXMLTableRowCellContext();

    virtual SvXMLImportContext *CreateChildContext( sal_uInt16 nPrefix,
                                     const ::rtl::OUString& rLocalName,
                                     const ::com::sun::star::uno::Reference<
                                          ::com::sun::star::xml::sax::XAttributeList>& xAttrList );

    inline void SetString(const rtl::OUString& rOUTempText) { pOUTextContent.reset(rOUTempText); }
    void SetCursorOnTextImport(const rtl::OUString& rOUTempText);

    void SetAnnotation(const ::com::sun::star::table::CellAddress& rPosition );
    void SetDetectiveObj( const ::com::sun::star::table::CellAddress& rPosition );
    void SetCellRangeSource( const ::com::sun::star::table::CellAddress& rPosition );

    virtual void EndElement();
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
