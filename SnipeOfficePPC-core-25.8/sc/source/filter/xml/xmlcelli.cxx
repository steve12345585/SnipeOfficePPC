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


// INCLUDE ---------------------------------------------------------------

#include "xmlcelli.hxx"
#include "xmlimprt.hxx"
#include "xmltabi.hxx"
#include "xmlstyli.hxx"
#include "xmlannoi.hxx"
#include "global.hxx"
#include "document.hxx"
#include "cellsuno.hxx"
#include "docuno.hxx"
#include "unonames.hxx"
#include "postit.hxx"
#include "sheetdata.hxx"
#include "docsh.hxx"

#include "XMLTableShapeImportHelper.hxx"
#include "XMLTextPContext.hxx"
#include "XMLStylesImportHelper.hxx"

#include "arealink.hxx"
#include <sfx2/linkmgr.hxx>
#include "convuno.hxx"
#include "XMLConverter.hxx"
#include "scerrors.hxx"
#include "editutil.hxx"
#include "cell.hxx"

#include <xmloff/xmltkmap.hxx>
#include <xmloff/xmltoken.hxx>
#include <xmloff/nmspmap.hxx>
#include <xmloff/xmluconv.hxx>
#include <xmloff/families.hxx>
#include <xmloff/numehelp.hxx>
#include <xmloff/xmlnmspe.hxx>
#include <svl/zforlist.hxx>
#include <svx/svdocapt.hxx>
#include <editeng/outlobj.hxx>
#include <editeng/editobj.hxx>
#include <svx/unoapi.hxx>
#include <svl/languageoptions.hxx>
#include <sax/tools/converter.hxx>

#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/text/XText.hpp>
#include <com/sun/star/sheet/XSpreadsheets.hpp>
#include <com/sun/star/sheet/XSpreadsheet.hpp>
#include <com/sun/star/sheet/XCellRangeAddressable.hpp>

#include <com/sun/star/util/XMergeable.hpp>
#include <com/sun/star/sheet/XSheetCondition.hpp>
#include <com/sun/star/table/XCellRange.hpp>
#include <com/sun/star/table/CellAddress.hpp>
#include <com/sun/star/util/NumberFormat.hpp>
#include <com/sun/star/util/XNumberFormatsSupplier.hpp>
#include <com/sun/star/util/XNumberFormatTypes.hpp>
#include <com/sun/star/util/Date.hpp>
#include <com/sun/star/lang/Locale.hpp>
#include <com/sun/star/text/ControlCharacter.hpp>

#include <rtl/ustrbuf.hxx>
#include <tools/date.hxx>
#include <i18npool/lang.h>
#include <comphelper/extract.hxx>

using namespace com::sun::star;
using namespace xmloff::token;

using rtl::OUString;

//------------------------------------------------------------------

ScXMLTableRowCellContext::ScXMLTableRowCellContext( ScXMLImport& rImport,
                                      sal_uInt16 nPrfx,
                                      const ::rtl::OUString& rLName,
                                      const ::com::sun::star::uno::Reference<
                                      ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
                                      const bool bTempIsCovered,
                                      const sal_Int32 nTempRepeatedRows ) :
    SvXMLImportContext( rImport, nPrfx, rLName ),
    pContentValidationName(NULL),
    pDetectiveObjVec(NULL),
    pCellRangeSource(NULL),
    fValue(0.0),
    nMergedRows(1),
    nMergedCols(1),
    nRepeatedRows(nTempRepeatedRows),
    nCellsRepeated(1),
    rXMLImport((ScXMLImport&)rImport),
    eGrammar( formula::FormulaGrammar::GRAM_STORAGE_DEFAULT),
    nCellType(util::NumberFormat::TEXT),
    bIsMerged(false),
    bIsMatrix(false),
    bHasSubTable(false),
    bIsCovered(bTempIsCovered),
    bIsEmpty(true),
    bHasTextImport(false),
    bIsFirstTextImport(false),
    bSolarMutexLocked(false),
    bFormulaTextResult(false)
{
    rXMLImport.SetRemoveLastChar(false);
    rXMLImport.GetTables().AddColumn(bTempIsCovered);
    const sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
    rtl::OUString aLocalName;
    rtl::OUString* pStyleName = NULL;
    rtl::OUString* pCurrencySymbol = NULL;
    const SvXMLTokenMap& rTokenMap = rImport.GetTableRowCellAttrTokenMap();
    for (sal_Int16 i = 0; i < nAttrCount; ++i)
    {
        sal_uInt16 nAttrPrefix = rImport.GetNamespaceMap().GetKeyByAttrName(
            xAttrList->getNameByIndex(i), &aLocalName);

        const rtl::OUString& sValue = xAttrList->getValueByIndex(i);
        sal_uInt16 nToken = rTokenMap.Get(nAttrPrefix, aLocalName);
        switch (nToken)
        {
            case XML_TOK_TABLE_ROW_CELL_ATTR_STYLE_NAME:
                pStyleName = new rtl::OUString(sValue);
            break;
            case XML_TOK_TABLE_ROW_CELL_ATTR_CONTENT_VALIDATION_NAME:
                OSL_ENSURE(!pContentValidationName, "here should be only one Validation Name");
                pContentValidationName = new rtl::OUString(sValue);
            break;
            case XML_TOK_TABLE_ROW_CELL_ATTR_SPANNED_ROWS:
                bIsMerged = true;
                nMergedRows = sValue.toInt32();
            break;
            case XML_TOK_TABLE_ROW_CELL_ATTR_SPANNED_COLS:
                bIsMerged = true;
                nMergedCols = sValue.toInt32();
            break;
            case XML_TOK_TABLE_ROW_CELL_ATTR_SPANNED_MATRIX_COLS:
                bIsMatrix = true;
                nMatrixCols = sValue.toInt32();
            break;
            case XML_TOK_TABLE_ROW_CELL_ATTR_SPANNED_MATRIX_ROWS:
                bIsMatrix = true;
                nMatrixRows = sValue.toInt32();
            break;
            case XML_TOK_TABLE_ROW_CELL_ATTR_REPEATED:
                nCellsRepeated = std::max( sValue.toInt32(), (sal_Int32) 1 );
            break;
            case XML_TOK_TABLE_ROW_CELL_ATTR_VALUE_TYPE:
                nCellType = GetScImport().GetCellType(sValue);
                bIsEmpty = false;
            break;
            case XML_TOK_TABLE_ROW_CELL_ATTR_VALUE:
            {
                if (!sValue.isEmpty())
                {
                    ::sax::Converter::convertDouble(fValue, sValue);
                    bIsEmpty = false;
                }
            }
            break;
            case XML_TOK_TABLE_ROW_CELL_ATTR_DATE_VALUE:
            {
                if (!sValue.isEmpty() && rXMLImport.SetNullDateOnUnitConverter())
                {
                    rXMLImport.GetMM100UnitConverter().convertDateTime(fValue, sValue);
                    bIsEmpty = false;
                }
            }
            break;
            case XML_TOK_TABLE_ROW_CELL_ATTR_TIME_VALUE:
            {
                if (!sValue.isEmpty())
                {
                    ::sax::Converter::convertDuration(fValue, sValue);
                    bIsEmpty = false;
                }
            }
            break;
            case XML_TOK_TABLE_ROW_CELL_ATTR_STRING_VALUE:
            {
                if (!sValue.isEmpty())
                {
                    OSL_ENSURE(!pOUTextValue, "here should be only one string value");
                    pOUTextValue.reset(sValue);
                    bIsEmpty = false;
                }
            }
            break;
            case XML_TOK_TABLE_ROW_CELL_ATTR_BOOLEAN_VALUE:
            {
                if (!sValue.isEmpty())
                {
                    if ( IsXMLToken(sValue, XML_TRUE) )
                        fValue = 1.0;
                    else if ( IsXMLToken(sValue, XML_FALSE) )
                        fValue = 0.0;
                    else
                        ::sax::Converter::convertDouble(fValue, sValue);
                    bIsEmpty = false;
                }
            }
            break;
            case XML_TOK_TABLE_ROW_CELL_ATTR_FORMULA:
            {
                if (!sValue.isEmpty())
                {
                    OSL_ENSURE(!pOUFormula, "here should be only one formula");
                    rtl::OUString aFormula, aFormulaNmsp;
                    rXMLImport.ExtractFormulaNamespaceGrammar( aFormula, aFormulaNmsp, eGrammar, sValue );
                    pOUFormula.reset( FormulaWithNamespace( aFormula, aFormulaNmsp ) );
                }
            }
            break;
            case XML_TOK_TABLE_ROW_CELL_ATTR_CURRENCY:
                pCurrencySymbol = new rtl::OUString(sValue);
            break;
            default:
                ;
        }
    }
    if (pOUFormula)
    {
        if (nCellType == util::NumberFormat::TEXT)
            bFormulaTextResult = true;
        nCellType = util::NumberFormat::UNDEFINED;
    }
    rXMLImport.GetStylesImportHelper()->SetAttributes(pStyleName, pCurrencySymbol, nCellType);
}

ScXMLTableRowCellContext::~ScXMLTableRowCellContext()
{
    if (pContentValidationName)
        delete pContentValidationName;
    if (pDetectiveObjVec)
        delete pDetectiveObjVec;
    if (pCellRangeSource)
        delete pCellRangeSource;
}

void ScXMLTableRowCellContext::LockSolarMutex()
{
    if (!bSolarMutexLocked)
    {
        GetScImport().LockSolarMutex();
        bSolarMutexLocked = true;
    }
}

void ScXMLTableRowCellContext::UnlockSolarMutex()
{
    if (bSolarMutexLocked)
    {
        GetScImport().UnlockSolarMutex();
        bSolarMutexLocked = false;
    }
}

void ScXMLTableRowCellContext::SetCursorOnTextImport(const rtl::OUString& rOUTempText)
{
    com::sun::star::table::CellAddress aCellPos = rXMLImport.GetTables().GetRealCellPos();
    if (CellExists(aCellPos))
    {
        uno::Reference<table::XCellRange> xCellRange(rXMLImport.GetTables().GetCurrentXCellRange());
        if (xCellRange.is())
        {
            xBaseCell.set(xCellRange->getCellByPosition(aCellPos.Column, aCellPos.Row));
            if (xBaseCell.is())
            {
                xLockable.set(xBaseCell, uno::UNO_QUERY);
                if (xLockable.is())
                    xLockable->addActionLock();
                uno::Reference<text::XText> xText(xBaseCell, uno::UNO_QUERY);
                if (xText.is())
                {
                    uno::Reference<text::XTextCursor> xTextCursor(xText->createTextCursor());
                    if (xTextCursor.is())
                    {
                        xTextCursor->setString(rOUTempText);
                        xTextCursor->gotoEnd(false);
                        rXMLImport.GetTextImport()->SetCursor(xTextCursor);
                    }
                }
            }
        }
    }
    else
    {
        OSL_FAIL("this method should only be called for a existing cell");
    }
}

SvXMLImportContext *ScXMLTableRowCellContext::CreateChildContext( sal_uInt16 nPrefix,
                                            const ::rtl::OUString& rLName,
                                            const ::com::sun::star::uno::Reference<
                                          ::com::sun::star::xml::sax::XAttributeList>& xAttrList )
{
    SvXMLImportContext *pContext = 0;

    const SvXMLTokenMap& rTokenMap = rXMLImport.GetTableRowCellElemTokenMap();
    bool bTextP(false);
    switch( rTokenMap.Get( nPrefix, rLName ) )
    {
    case XML_TOK_TABLE_ROW_CELL_P:
        {
            bIsEmpty = false;
            bTextP = true;
            com::sun::star::table::CellAddress aCellPos = rXMLImport.GetTables().GetRealCellPos();
            if( ((nCellType == util::NumberFormat::TEXT) || bFormulaTextResult) &&
                !rXMLImport.GetTables().IsPartOfMatrix(static_cast<SCCOL>(aCellPos.Column), static_cast<SCROW>(aCellPos.Row)) )
            {
                if (!bHasTextImport)
                {
                    bIsFirstTextImport = true;
                    bHasTextImport = true;
                    pContext = new ScXMLTextPContext(rXMLImport, nPrefix, rLName, xAttrList, this);
                }
                else
                {
                    // com::sun::star::table::CellAddress aCellPos = rXMLImport.GetTables().GetRealCellPos();
                    if (CellExists(aCellPos))
                    {
                        if (bIsFirstTextImport && !rXMLImport.GetRemoveLastChar())
                        {
                            if (pOUTextContent)
                            {
                                SetCursorOnTextImport(*pOUTextContent);
                                pOUTextContent.reset();
                            }
                            else
                                SetCursorOnTextImport(rtl::OUString());
                            rXMLImport.SetRemoveLastChar(true);
                            uno::Reference < text::XTextCursor > xTextCursor(rXMLImport.GetTextImport()->GetCursor());
                            if (xTextCursor.is())
                            {
                                uno::Reference < text::XText > xText (xTextCursor->getText());
                                uno::Reference < text::XTextRange > xTextRange (xTextCursor, uno::UNO_QUERY);
                                if (xText.is() && xTextRange.is())
                                    xText->insertControlCharacter(xTextRange, text::ControlCharacter::PARAGRAPH_BREAK, false);
                            }
                        }
                        pContext = rXMLImport.GetTextImport()->CreateTextChildContext(
                            rXMLImport, nPrefix, rLName, xAttrList);
                        bIsFirstTextImport = false;
                    }
                }
            }
        }
        break;
    case XML_TOK_TABLE_ROW_CELL_TABLE:
        {
            const sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
            rtl::OUString aLocalName;
            for( sal_Int16 i=0; i < nAttrCount; i++ )
            {
                sal_uInt16 nAttrPrefix = rXMLImport.GetNamespaceMap().GetKeyByAttrName(
                                                    xAttrList->getNameByIndex( i ), &aLocalName );
                if (    nAttrPrefix == XML_NAMESPACE_TABLE
                    &&  IsXMLToken(aLocalName, XML_IS_SUB_TABLE))
                {
                    bHasSubTable = IsXMLToken(xAttrList->getValueByIndex( i ), XML_TRUE);
                }
            }
            OSL_ENSURE(bHasSubTable, "it should be a subtable");
            pContext = new ScXMLTableContext( rXMLImport , nPrefix,
                                                        rLName, xAttrList,
                                                        true, nMergedCols);
            nMergedCols = 1;
            bIsMerged = false;
        }
        break;
    case XML_TOK_TABLE_ROW_CELL_ANNOTATION:
        {
            bIsEmpty = false;
            OSL_ENSURE( !mxAnnotationData.get(), "ScXMLTableRowCellContext::CreateChildContext - multiple annotations in one cell" );
            mxAnnotationData.reset( new ScXMLAnnotationData );
            pContext = new ScXMLAnnotationContext( rXMLImport, nPrefix, rLName,
                                                    xAttrList, *mxAnnotationData, this);
        }
        break;
    case XML_TOK_TABLE_ROW_CELL_DETECTIVE:
        {
            bIsEmpty = false;
            if (!pDetectiveObjVec)
                pDetectiveObjVec = new ScMyImpDetectiveObjVec();
            pContext = new ScXMLDetectiveContext(
                rXMLImport, nPrefix, rLName, pDetectiveObjVec );
        }
        break;
    case XML_TOK_TABLE_ROW_CELL_CELL_RANGE_SOURCE:
        {
            bIsEmpty = false;
            if (!pCellRangeSource)
                pCellRangeSource = new ScMyImpCellRangeSource();
            pContext = new ScXMLCellRangeSourceContext(
                rXMLImport, nPrefix, rLName, xAttrList, pCellRangeSource );
        }
        break;
    }

    if (!pContext && !bTextP)
    {
        com::sun::star::table::CellAddress aCellPos = rXMLImport.GetTables().GetRealCellPos();
        uno::Reference<drawing::XShapes> xShapes (rXMLImport.GetTables().GetCurrentXShapes());
        if (xShapes.is())
        {
            if (aCellPos.Column > MAXCOL)
                aCellPos.Column = MAXCOL;
            if (aCellPos.Row > MAXROW)
                aCellPos.Row = MAXROW;
            XMLTableShapeImportHelper* pTableShapeImport = (XMLTableShapeImportHelper*)rXMLImport.GetShapeImport().get();
            pTableShapeImport->SetOnTable(false);
            pTableShapeImport->SetCell(aCellPos);
            pContext = rXMLImport.GetShapeImport()->CreateGroupChildContext(
                rXMLImport, nPrefix, rLName, xAttrList, xShapes);
            if (pContext)
            {
                bIsEmpty = false;
                rXMLImport.ProgressBarIncrement(false);
            }
        }
    }

    if( !pContext )
        pContext = new SvXMLImportContext( GetImport(), nPrefix, rLName );

    return pContext;
}

void ScXMLTableRowCellContext::DoMerge( const ScAddress& rScAddress, const SCCOL nCols, const SCROW nRows )
{
    SCCOL mergeToCol = rScAddress.Col() + nCols;
    SCROW mergeToRow = rScAddress.Row() + nRows;
    bool bInBounds = rScAddress.Col() <= MAXCOL && rScAddress.Row() <= MAXROW &&
                       mergeToCol <= MAXCOL && mergeToRow <= MAXROW;
    if( bInBounds )
    {
        rXMLImport.GetDocument()->DoMerge( rScAddress.Tab(),
            rScAddress.Col(), rScAddress.Row(), mergeToCol, mergeToRow );
    }
}

void ScXMLTableRowCellContext::SetContentValidation(com::sun::star::uno::Reference<com::sun::star::beans::XPropertySet>& xPropSet)
{
    if (pContentValidationName)
    {
        ScMyImportValidation aValidation;
        aValidation.eGrammar1 = aValidation.eGrammar2 = GetScImport().GetDocument()->GetStorageGrammar();
        if (rXMLImport.GetValidation(*pContentValidationName, aValidation))
        {
            uno::Reference<beans::XPropertySet> xPropertySet(xPropSet->getPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_VALIXML))), uno::UNO_QUERY);
            if (xPropertySet.is())
            {
                if (!aValidation.sErrorMessage.isEmpty())
                    xPropertySet->setPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_ERRMESS)), uno::makeAny(aValidation.sErrorMessage));
                if (!aValidation.sErrorTitle.isEmpty())
                    xPropertySet->setPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_ERRTITLE)), uno::makeAny(aValidation.sErrorTitle));
                if (!aValidation.sImputMessage.isEmpty())
                    xPropertySet->setPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_INPMESS)), uno::makeAny(aValidation.sImputMessage));
                if (!aValidation.sImputTitle.isEmpty())
                    xPropertySet->setPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_INPTITLE)), uno::makeAny(aValidation.sImputTitle));
                xPropertySet->setPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_SHOWERR)), uno::makeAny(aValidation.bShowErrorMessage));
                xPropertySet->setPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_SHOWINP)), uno::makeAny(aValidation.bShowImputMessage));
                xPropertySet->setPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_TYPE)), uno::makeAny(aValidation.aValidationType));
                xPropertySet->setPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_IGNOREBL)), uno::makeAny(aValidation.bIgnoreBlanks));
                xPropertySet->setPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_SHOWLIST)), uno::makeAny(aValidation.nShowList));
                xPropertySet->setPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_ERRALSTY)), uno::makeAny(aValidation.aAlertStyle));
                uno::Reference<sheet::XSheetCondition> xCondition(xPropertySet, uno::UNO_QUERY);
                if (xCondition.is())
                {
                    xCondition->setFormula1(aValidation.sFormula1);
                    xCondition->setFormula2(aValidation.sFormula2);
                    xCondition->setOperator(aValidation.aOperator);
                    // source position must be set as string, because it may
                    // refer to a sheet that hasn't been loaded yet.
                    xPropertySet->setPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_SOURCESTR)), uno::makeAny(aValidation.sBaseCellAddress));
                    // Transport grammar and formula namespace
                    xPropertySet->setPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_FORMULANMSP1)), uno::makeAny(aValidation.sFormulaNmsp1));
                    xPropertySet->setPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_FORMULANMSP2)), uno::makeAny(aValidation.sFormulaNmsp2));
                    xPropertySet->setPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_GRAMMAR1)), uno::makeAny(static_cast<sal_Int32>(aValidation.eGrammar1)));
                    xPropertySet->setPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_GRAMMAR2)), uno::makeAny(static_cast<sal_Int32>(aValidation.eGrammar2)));
                }
            }
            xPropSet->setPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SC_UNONAME_VALIXML)), uno::makeAny(xPropertySet));

            // For now, any sheet with validity is blocked from stream-copying.
            // Later, the validation names could be stored along with the style names.
            ScSheetSaveData* pSheetData = ScModelObj::getImplementation(GetImport().GetModel())->GetSheetSaveData();
            pSheetData->BlockSheet( GetScImport().GetTables().GetCurrentSheet() );
        }
    }
}

void ScXMLTableRowCellContext::SetCellProperties(const uno::Reference<table::XCellRange>& xCellRange,
                                                const table::CellAddress& aCellAddress)
{
    if (CellExists(aCellAddress) && pContentValidationName && !pContentValidationName->isEmpty())
    {
        sal_Int32 nBottom = aCellAddress.Row + nRepeatedRows - 1;
        sal_Int32 nRight = aCellAddress.Column + nCellsRepeated - 1;
        if (nBottom > MAXROW)
            nBottom = MAXROW;
        if (nRight > MAXCOL)
            nRight = MAXCOL;
        uno::Reference <beans::XPropertySet> xProperties (xCellRange->getCellRangeByPosition(aCellAddress.Column, aCellAddress.Row,
                                                            nRight, nBottom), uno::UNO_QUERY);
        if (xProperties.is())
            SetContentValidation(xProperties);
    }
}

void ScXMLTableRowCellContext::SetCellProperties(const uno::Reference<table::XCell>& xCell)
{
    if (pContentValidationName && !pContentValidationName->isEmpty())
    {
        uno::Reference <beans::XPropertySet> xProperties (xCell, uno::UNO_QUERY);
        if (xProperties.is())
            SetContentValidation(xProperties);
    }
}

void ScXMLTableRowCellContext::SetAnnotation(const table::CellAddress& aCellAddress)
{
    ScDocument* pDoc = rXMLImport.GetDocument();
    if( !pDoc || !mxAnnotationData.get() )
        return;

    LockSolarMutex();

    ScAddress aPos;
    ScUnoConversion::FillScAddress( aPos, aCellAddress );
    ScPostIt* pNote = 0;

    uno::Reference< drawing::XShapes > xShapes = rXMLImport.GetTables().GetCurrentXShapes();
    uno::Reference< container::XIndexAccess > xShapesIA( xShapes, uno::UNO_QUERY );
    sal_Int32 nOldShapeCount = xShapesIA.is() ? xShapesIA->getCount() : 0;

    OSL_ENSURE( !mxAnnotationData->mxShape.is() || mxAnnotationData->mxShapes.is(),
        "ScXMLTableRowCellContext::SetAnnotation - shape without drawing page" );
    if( mxAnnotationData->mxShape.is() && mxAnnotationData->mxShapes.is() )
    {
        OSL_ENSURE( mxAnnotationData->mxShapes.get() == xShapes.get(), "ScXMLTableRowCellContext::SetAnnotation - diffenet drawing pages" );
        SdrObject* pObject = ::GetSdrObjectFromXShape( mxAnnotationData->mxShape );
        OSL_ENSURE( pObject, "ScXMLTableRowCellContext::SetAnnotation - cannot get SdrObject from shape" );

        /*  Try to reuse the drawing object already created (but only if the
            note is visible, and the object is a caption object). */
        if( mxAnnotationData->mbShown && mxAnnotationData->mbUseShapePos )
        {
            if( SdrCaptionObj* pCaption = dynamic_cast< SdrCaptionObj* >( pObject ) )
            {
                OSL_ENSURE( !pCaption->GetLogicRect().IsEmpty(), "ScXMLTableRowCellContext::SetAnnotation - invalid caption rectangle" );
                // create the cell note with the caption object
                pNote = ScNoteUtil::CreateNoteFromCaption( *pDoc, aPos, *pCaption, true );
                // forget pointer to object (do not create note again below)
                pObject = 0;
            }
        }

        // drawing object has not been used to create a note -> use shape data
        if( pObject )
        {
            // rescue settings from drawing object before the shape is removed
            ::std::auto_ptr< SfxItemSet > xItemSet( new SfxItemSet( pObject->GetMergedItemSet() ) );
            ::std::auto_ptr< OutlinerParaObject > xOutlinerObj;
            if( OutlinerParaObject* pOutlinerObj = pObject->GetOutlinerParaObject() )
                xOutlinerObj.reset( new OutlinerParaObject( *pOutlinerObj ) );
            Rectangle aCaptionRect;
            if( mxAnnotationData->mbUseShapePos )
                aCaptionRect = pObject->GetLogicRect();
            // remove the shape from the drawing page, this invalidates pObject
            mxAnnotationData->mxShapes->remove( mxAnnotationData->mxShape );
            pObject = 0;
            // update current number of existing objects
            if( xShapesIA.is() )
                nOldShapeCount = xShapesIA->getCount();

            // an outliner object is required (empty note captions not allowed)
            if( xOutlinerObj.get() )
            {
                // create cell note with all data from drawing object
                pNote = ScNoteUtil::CreateNoteFromObjectData( *pDoc, aPos,
                    xItemSet.release(), xOutlinerObj.release(),
                    aCaptionRect, mxAnnotationData->mbShown, false );
            }
        }
    }
    else if( !mxAnnotationData->maSimpleText.isEmpty() )
    {
        // create note from simple text
        pNote = ScNoteUtil::CreateNoteFromString( *pDoc, aPos,
            mxAnnotationData->maSimpleText, mxAnnotationData->mbShown, false );
    }

    // set author and date
    if( pNote )
    {
        double fDate;
        rXMLImport.GetMM100UnitConverter().convertDateTime( fDate, mxAnnotationData->maCreateDate );
        SvNumberFormatter* pNumForm = pDoc->GetFormatTable();
        sal_uInt32 nfIndex = pNumForm->GetFormatIndex( NF_DATE_SYS_DDMMYYYY, LANGUAGE_SYSTEM );
        String aDate;
        Color* pColor = 0;
        Color** ppColor = &pColor;
        pNumForm->GetOutputString( fDate, nfIndex, aDate, ppColor );
        pNote->SetDate( aDate );
        pNote->SetAuthor( mxAnnotationData->maAuthor );
    }

    // register a shape that has been newly created in the ScNoteUtil functions
    if( xShapesIA.is() && (nOldShapeCount < xShapesIA->getCount()) )
    {
        uno::Reference< drawing::XShape > xShape;
        rXMLImport.GetShapeImport()->shapeWithZIndexAdded( xShape, xShapesIA->getCount() );
    }

    // store the style names for stream copying
    ScSheetSaveData* pSheetData = ScModelObj::getImplementation(rXMLImport.GetModel())->GetSheetSaveData();
    pSheetData->HandleNoteStyles( mxAnnotationData->maStyleName, mxAnnotationData->maTextStyle, aPos );

    std::vector<ScXMLAnnotationStyleEntry>::const_iterator aIter = mxAnnotationData->maContentStyles.begin();
    std::vector<ScXMLAnnotationStyleEntry>::const_iterator aEnd = mxAnnotationData->maContentStyles.end();
    while (aIter != aEnd)
    {
        pSheetData->AddNoteContentStyle( aIter->mnFamily, aIter->maName, aPos, aIter->maSelection );
        ++aIter;
    }
}

// core implementation
void ScXMLTableRowCellContext::SetDetectiveObj( const table::CellAddress& rPosition )
{
    if( CellExists(rPosition) && pDetectiveObjVec && pDetectiveObjVec->size() )
    {
        LockSolarMutex();
        ScDetectiveFunc aDetFunc( rXMLImport.GetDocument(), rPosition.Sheet );
        uno::Reference<container::XIndexAccess> xShapesIndex (rXMLImport.GetTables().GetCurrentXShapes(), uno::UNO_QUERY); // make draw page
        ScMyImpDetectiveObjVec::iterator aItr(pDetectiveObjVec->begin());
        ScMyImpDetectiveObjVec::iterator aEndItr(pDetectiveObjVec->end());
        while(aItr != aEndItr)
        {
            ScAddress aScAddress;
            ScUnoConversion::FillScAddress( aScAddress, rPosition );
            aDetFunc.InsertObject( aItr->eObjType, aScAddress, aItr->aSourceRange, aItr->bHasError );
            if (xShapesIndex.is())
            {
                sal_Int32 nShapes = xShapesIndex->getCount();
                uno::Reference < drawing::XShape > xShape;
                rXMLImport.GetShapeImport()->shapeWithZIndexAdded(xShape, nShapes);
            }
            ++aItr;
        }
    }
}

// core implementation
void ScXMLTableRowCellContext::SetCellRangeSource( const table::CellAddress& rPosition )
{
    if( CellExists(rPosition) && pCellRangeSource  && !pCellRangeSource->sSourceStr.isEmpty() &&
        !pCellRangeSource->sFilterName.isEmpty() && !pCellRangeSource->sURL.isEmpty() )
    {
        ScDocument* pDoc = rXMLImport.GetDocument();
        if (pDoc)
        {
            LockSolarMutex();
            ScRange aDestRange( static_cast<SCCOL>(rPosition.Column), static_cast<SCROW>(rPosition.Row), rPosition.Sheet,
                static_cast<SCCOL>(rPosition.Column + pCellRangeSource->nColumns - 1),
                static_cast<SCROW>(rPosition.Row + pCellRangeSource->nRows - 1), rPosition.Sheet );
            String sFilterName( pCellRangeSource->sFilterName );
            String sSourceStr( pCellRangeSource->sSourceStr );
            ScAreaLink* pLink = new ScAreaLink( pDoc->GetDocumentShell(), pCellRangeSource->sURL,
                sFilterName, pCellRangeSource->sFilterOptions, sSourceStr, aDestRange, pCellRangeSource->nRefresh );
            sfx2::LinkManager* pLinkManager = pDoc->GetLinkManager();
            pLinkManager->InsertFileLink( *pLink, OBJECT_CLIENT_FILE, pCellRangeSource->sURL, &sFilterName, &sSourceStr );
        }
    }
}

bool lcl_IsEmptyOrNote( ScDocument* pDoc, const table::CellAddress& rCurrentPos )
{
    ScAddress aScAddress;
    ScUnoConversion::FillScAddress( aScAddress, rCurrentPos );
    ScBaseCell* pCell = pDoc->GetCell( aScAddress );
    return ( !pCell || pCell->GetCellType() == CELLTYPE_NOTE );
}

void ScXMLTableRowCellContext::EndElement()
{
    if (!bHasSubTable)
    {
        if (bHasTextImport && rXMLImport.GetRemoveLastChar())
        {
            UniReference< XMLTextImportHelper > aTextImport = rXMLImport.GetTextImport();
            if (aTextImport->GetCursor().is())
            {
                //aTextImport->GetCursor()->gotoEnd(false);
                if( aTextImport->GetCursor()->goLeft( 1, true ) )
                {
                    aTextImport->GetText()->insertString(
                        aTextImport->GetCursorAsRange(), rtl::OUString(),
                        true );
                }
                aTextImport->ResetCursor();
            }
        }
        ScMyTables& rTables = rXMLImport.GetTables();
        table::CellAddress aCellPos = rTables.GetRealCellPos();
        if (aCellPos.Column > 0 && nRepeatedRows > 1)
            aCellPos.Row -= (nRepeatedRows - 1);

        //duplicated for now
        ScAddress aScCellPos = rTables.GetRealScCellPos();
        if (aScCellPos.Col() > 0 && nRepeatedRows > 1)
            aScCellPos.SetRow( aScCellPos.Row() - (nRepeatedRows - 1) );

        uno::Reference<table::XCellRange> xCellRange(rTables.GetCurrentXCellRange());
        if (xCellRange.is())
        {
            if (bIsMerged)
                DoMerge(aScCellPos, static_cast<SCCOL>(nMergedCols - 1), static_cast<SCROW>(nMergedRows - 1));
            if ( !pOUFormula )
            {
                ::boost::optional< rtl::OUString > pOUText;

                if(nCellType == util::NumberFormat::TEXT)
                {
                    if (xLockable.is())
                        xLockable->removeActionLock();

                    // #i61702# The formatted text content of xBaseCell / xLockable is invalidated,
                    // so it can't be used after calling removeActionLock (getString always uses the document).

                    if (CellExists(aCellPos) && ((nCellsRepeated > 1) || (nRepeatedRows > 1)))
                    {
                        if (!xBaseCell.is())
                        {
                            try
                            {
                                xBaseCell.set(xCellRange->getCellByPosition(aCellPos.Column, aCellPos.Row));
                            }
                            catch (lang::IndexOutOfBoundsException&)
                            {
                                OSL_FAIL("It seems here are to many columns or rows");
                            }
                        }
                        uno::Reference <text::XText> xTempText (xBaseCell, uno::UNO_QUERY);
                        if (xTempText.is())
                        {
                            pOUText.reset(xTempText->getString());
                        }
                    }
                    if (!pOUTextContent && !pOUText && !pOUTextValue)
                            bIsEmpty = true;
                }
                bool bWasEmpty = bIsEmpty;
//              uno::Reference <table::XCell> xCell;
                table::CellAddress aCurrentPos( aCellPos );
                if ((pContentValidationName && !pContentValidationName->isEmpty()) ||
                    mxAnnotationData.get() || pDetectiveObjVec || pCellRangeSource)
                    bIsEmpty = false;

                for (sal_Int32 i = 0; i < nCellsRepeated; ++i)
                {
                    aCurrentPos.Column = aCellPos.Column + i;
                    if (i > 0)
                        rTables.AddColumn(false);
                    if (!bIsEmpty)
                    {
                        for (sal_Int32 j = 0; j < nRepeatedRows; ++j)
                        {
                            aCurrentPos.Row = aCellPos.Row + j;
                            if ((aCurrentPos.Column == 0) && (j > 0))
                            {
                                rTables.AddRow();
                                rTables.AddColumn(false);
                            }
                            if (CellExists(aCurrentPos))
                            {
                                // test - bypass the API
                                // if (xBaseCell.is() && (aCurrentPos == aCellPos))
                                //     xCell.set(xBaseCell);
                                // else
                                // {
                                //     try
                                //     {
                                //      xCell.set(xCellRange->getCellByPosition(aCurrentPos.Column, aCurrentPos.Row));
                                //     }
                                //     catch (lang::IndexOutOfBoundsException&)
                                //     {
                                //         OSL_FAIL("It seems here are to many columns or rows");
                                //     }
                                // }

                                // test - bypass the API
                                // if ((!(bIsCovered) || (xCell->getType() == table::CellContentType_EMPTY)))
                                if ((!(bIsCovered) || lcl_IsEmptyOrNote( rXMLImport.GetDocument(), aCurrentPos )))
                                {
                                    switch (nCellType)
                                    {
                                    case util::NumberFormat::TEXT:
                                        {
                                            bool bDoIncrement = true;
                                            if (rTables.IsPartOfMatrix(static_cast<SCCOL>(aCurrentPos.Column), static_cast<SCROW>(aCurrentPos.Row)))
                                            {
                                                LockSolarMutex();
                                                // test - bypass the API
                                                // ScCellObj* pCellObj = (ScCellObj*)ScCellRangesBase::getImplementation(xCell);
                                                // if (pCellObj)
                                                // {
                                                //     if(pOUTextValue && pOUTextValue->getLength())
                                                //         pCellObj->SetFormulaResultString(*pOUTextValue);
                                                //     else if (pOUTextContent && pOUTextContent->getLength())
                                                //         pCellObj->SetFormulaResultString(*pOUTextContent);
                                                //     else if ( i > 0 && pOUText && pOUText->getLength() )
                                                //     {
                                                //         pCellObj->SetFormulaResultString(*pOUText);
                                                //     }
                                                //     else
                                                //         bDoIncrement = false;
                                                // }
                                                // else
                                                //     bDoIncrement = false;
                                                ScAddress aScAddress;
                                                ScUnoConversion::FillScAddress( aScAddress, aCurrentPos );
                                                ScBaseCell* pCell = rXMLImport.GetDocument()->GetCell( aScAddress );
                                                bDoIncrement = ( pCell && pCell->GetCellType() == CELLTYPE_FORMULA );
                                                if ( bDoIncrement )
                                                {
                                                    ScFormulaCell* pFCell = static_cast<ScFormulaCell*>(pCell);
                                                    if (pOUTextValue && !pOUTextValue->isEmpty())
                                                        pFCell->SetHybridString( *pOUTextValue );
                                                    else if (pOUTextContent && !pOUTextContent->isEmpty())
                                                        pFCell->SetHybridString( *pOUTextContent );
                                                    else if ( i > 0 && pOUText && !pOUText->isEmpty() )
                                                        pFCell->SetHybridString( *pOUText );
                                                    else
                                                        bDoIncrement = false;
                                                }
                                            }
                                            else
                                            {
                                                // test - bypass the API
                                                // uno::Reference <text::XText> xText (xCell, uno::UNO_QUERY);
                                                // if (xText.is())
                                                // {
                                                //     if(pOUTextValue && pOUTextValue->getLength())
                                                //         xText->setString(*pOUTextValue);
                                                //     else if (pOUTextContent && pOUTextContent->getLength())
                                                //         xText->setString(*pOUTextContent);
                                                //     else if ( i > 0 && pOUText && pOUText->getLength() )
                                                //     {
                                                //         xText->setString(*pOUText);
                                                //     }
                                                //     else
                                                //         bDoIncrement = false;
                                                // }
                                                LockSolarMutex();
                                                ScBaseCell* pNewCell = NULL;
                                                ScDocument* pDoc = rXMLImport.GetDocument();
                                                if (pOUTextValue && !pOUTextValue->isEmpty())
                                                    pNewCell = ScBaseCell::CreateTextCell( *pOUTextValue, pDoc );
                                                else if (pOUTextContent && !pOUTextContent->isEmpty())
                                                    pNewCell = ScBaseCell::CreateTextCell( *pOUTextContent, pDoc );
                                                else if ( i > 0 && pOUText && !pOUText->isEmpty() )
                                                    pNewCell = ScBaseCell::CreateTextCell( *pOUText, pDoc );

                                                bDoIncrement = pNewCell != NULL;
                                                if ( bDoIncrement )
                                                {
                                                    ScAddress aScAddress;
                                                    ScUnoConversion::FillScAddress( aScAddress, aCurrentPos );
                                                    pDoc->PutCell( aScAddress, pNewCell );
                                                }
                                            }
                                            // #i56027# This is about setting simple text, not edit cells,
                                            // so ProgressBarIncrement must be called with bEditCell = FALSE.
                                            // Formatted text that is put into the cell by the child context
                                            // is handled below (bIsEmpty is true then).
                                            if (bDoIncrement || bHasTextImport)
                                                rXMLImport.ProgressBarIncrement(false);
                                        }
                                        break;
                                    case util::NumberFormat::NUMBER:
                                    case util::NumberFormat::PERCENT:
                                    case util::NumberFormat::CURRENCY:
                                    case util::NumberFormat::TIME:
                                    case util::NumberFormat::DATETIME:
                                    case util::NumberFormat::LOGICAL:
                                        {
                                            if( rTables.IsPartOfMatrix(static_cast<SCCOL>(aCurrentPos.Column), static_cast<SCROW>(aCurrentPos.Row)) )
                                            {
                                                LockSolarMutex();
                                                // test - bypass the API
                                                // ScCellObj* pCellObj = (ScCellObj*)ScCellRangesBase::getImplementation(xCell);
                                                // if (pCellObj)
                                                //     pCellObj->SetFormulaResultDouble(fValue);
                                                ScAddress aScAddress;
                                                ScUnoConversion::FillScAddress( aScAddress, aCurrentPos );
                                                ScBaseCell* pCell = rXMLImport.GetDocument()->GetCell( aScAddress );
                                                if ( pCell && pCell->GetCellType() == CELLTYPE_FORMULA )
                                                    static_cast<ScFormulaCell*>(pCell)->SetHybridDouble( fValue );
                                            }
                                            else
                                            {
                                                // test - bypass the API
                                                // xCell->setValue(fValue);
                                                LockSolarMutex();

                                                // #i62435# Initialize the value cell's script type
                                                // if the default style's number format is latin-only.
                                                // If the cell uses a different format, the script type
                                                // will be reset when the style is applied.

                                                ScBaseCell* pNewCell = new ScValueCell(fValue);
                                                if ( rXMLImport.IsLatinDefaultStyle() )
                                                    pNewCell->SetScriptType( SCRIPTTYPE_LATIN );
                                                rXMLImport.GetDocument()->PutCell(
                                                    sal::static_int_cast<SCCOL>( aCurrentPos.Column ),
                                                    sal::static_int_cast<SCROW>( aCurrentPos.Row ),
                                                    sal::static_int_cast<SCTAB>( aCurrentPos.Sheet ),
                                                    pNewCell );
                                            }
                                            rXMLImport.ProgressBarIncrement(false);
                                        }
                                        break;
                                    default:
                                        {
                                            OSL_FAIL("no cell type given");
                                        }
                                        break;
                                    }
                                }

                                SetAnnotation(aCurrentPos);
                                SetDetectiveObj( aCurrentPos );
                                SetCellRangeSource( aCurrentPos );
                            }
                            else
                            {
                                if (!bWasEmpty || mxAnnotationData.get())
                                {
                                    if (aCurrentPos.Row > MAXROW)
                                        rXMLImport.SetRangeOverflowType(SCWARN_IMPORT_ROW_OVERFLOW);
                                    else
                                        rXMLImport.SetRangeOverflowType(SCWARN_IMPORT_COLUMN_OVERFLOW);
                                }
                            }
                        }
                    }
                    else
                    {
                        // #i56027# If the child context put formatted text into the cell,
                        // bIsEmpty is true and ProgressBarIncrement has to be called
                        // with bEditCell = TRUE.
                        if (bHasTextImport)
                            rXMLImport.ProgressBarIncrement(true);
                        if ((i == 0) && (aCellPos.Column == 0))
                            for (sal_Int32 j = 1; j < nRepeatedRows; ++j)
                            {
                                rTables.AddRow();
                                rTables.AddColumn(false);
                            }
                    }
                }
                if (nCellsRepeated > 1 || nRepeatedRows > 1)
                {
                    SetCellProperties(xCellRange, aCellPos); // set now only the validation for the complete range with the given cell as start cell
                    //SetType(xCellRange, aCellPos);
                    SCCOL nStartCol(aCellPos.Column < MAXCOL ? static_cast<SCCOL>(aCellPos.Column) : MAXCOL);
                    SCROW nStartRow(aCellPos.Row < MAXROW ? static_cast<SCROW>(aCellPos.Row) : MAXROW);
                    SCCOL nEndCol(aCellPos.Column + nCellsRepeated - 1 < MAXCOL ? static_cast<SCCOL>(aCellPos.Column + nCellsRepeated - 1) : MAXCOL);
                    SCROW nEndRow(aCellPos.Row + nRepeatedRows - 1 < MAXROW ? static_cast<SCROW>(aCellPos.Row + nRepeatedRows - 1) : MAXROW);
                    ScRange aScRange( nStartCol, nStartRow, aCellPos.Sheet,
                        nEndCol, nEndRow, aCellPos.Sheet );
                    rXMLImport.GetStylesImportHelper()->AddRange(aScRange);
                }
                else if (CellExists(aCellPos))
                {
                    rXMLImport.GetStylesImportHelper()->AddCell(aCellPos);

                    // test - bypass the API
                    // SetCellProperties(xCell); // set now only the validation
                    SetCellProperties(xCellRange, aCellPos);

                    //SetType(xTempCell);
                }
            }
            else // if ( !pOUFormula )
            {
                if (CellExists(aCellPos))
                {
                    uno::Reference <table::XCell> xCell;
                    try
                    {
                        xCell.set(xCellRange->getCellByPosition(aCellPos.Column , aCellPos.Row));
                    }
                    catch (lang::IndexOutOfBoundsException&)
                    {
                        OSL_FAIL("It seems here are to many columns or rows");
                    }
                    if (xCell.is())
                    {
                        SetCellProperties(xCell); // set now only the validation
                        OSL_ENSURE(((nCellsRepeated == 1) && (nRepeatedRows == 1)), "repeated cells with formula not possible now");
                        rXMLImport.GetStylesImportHelper()->AddCell(aCellPos);
                        if (!bIsMatrix)
                        {
                            LockSolarMutex();

                            ScAddress aScAddress;
                            ScUnoConversion::FillScAddress( aScAddress, aCellPos );

                            ScDocument* pDoc = rXMLImport.GetDocument();

                            rtl::OUString aText = pOUFormula->first;
                            rtl::OUString aFormulaNmsp = pOUFormula->second;

                            ::boost::scoped_ptr<ScExternalRefManager::ApiGuard> pExtRefGuard;
                            pExtRefGuard.reset(new ScExternalRefManager::ApiGuard(pDoc));

                            ScBaseCell* pNewCell = NULL;

                            if ( !aText.isEmpty() )
                            {
                                if ( aText[0] == '=' && aText.getLength() > 1 )
                                {
                                    // temporary formula string as string tokens
                                    ScTokenArray* pCode = new ScTokenArray;
                                    pCode->AddStringXML( aText );
                                    if( (eGrammar == formula::FormulaGrammar::GRAM_EXTERNAL) && !aFormulaNmsp.isEmpty() )
                                        pCode->AddStringXML( aFormulaNmsp );

                                    pDoc->IncXMLImportedFormulaCount( aText.getLength() );
                                    pNewCell = new ScFormulaCell( pDoc, aScAddress, pCode, eGrammar, MM_NONE );
                                    delete pCode;
                                }
                                else if ( aText[0] == '\'' && aText.getLength() > 1 )
                                {
                                    //  for bEnglish, "'" at the beginning is always interpreted as text
                                    //  marker and stripped
                                    pNewCell = ScBaseCell::CreateTextCell( aText.copy( 1 ), pDoc );
                                }
                                else
                                {
                                    SvNumberFormatter* pFormatter = pDoc->GetFormatTable();
                                    sal_uInt32 nEnglish = pFormatter->GetStandardIndex(LANGUAGE_ENGLISH_US);
                                    double fVal;
                                    if ( pFormatter->IsNumberFormat( aText, nEnglish, fVal ) )
                                    {
                                        pNewCell = new ScValueCell( fVal );
                                    }
                                    else
                                        pNewCell = ScBaseCell::CreateTextCell( aText, pDoc );
                                    //  das (englische) Zahlformat wird nicht gesetzt
                                    //! passendes lokales Format suchen und setzen???
                                }

                                if (pNewCell)
                                    pDoc->PutCell( aScAddress, pNewCell );

                                ScBaseCell* pCell = rXMLImport.GetDocument()->GetCell( aScAddress );
                                if ( pCell && pCell->GetCellType() == CELLTYPE_FORMULA )
                                {
                                    if (bFormulaTextResult && pOUTextValue && !pOUTextValue->isEmpty())
                                        static_cast<ScFormulaCell*>(pCell)->SetHybridString( *pOUTextValue );
                                    else
                                        static_cast<ScFormulaCell*>(pCell)->SetHybridDouble( fValue );
                                }
                            }
                        }
                        else
                        {
                            if (nMatrixCols > 0 && nMatrixRows > 0)
                            {
                                rTables.AddMatrixRange(
                                        static_cast<SCCOL>(aCellPos.Column),
                                        static_cast<SCROW>(aCellPos.Row),
                                        static_cast<SCCOL>(aCellPos.Column + nMatrixCols - 1),
                                        static_cast<SCROW>(aCellPos.Row + nMatrixRows - 1),
                                        pOUFormula->first, pOUFormula->second, eGrammar);
                            }
                        }
                        SetAnnotation( aCellPos );
                        SetDetectiveObj( aCellPos );
                        SetCellRangeSource( aCellPos );
                        rXMLImport.ProgressBarIncrement(false);
                    }
                }
                else
                {
                    if (aCellPos.Row > MAXROW)
                        rXMLImport.SetRangeOverflowType(SCWARN_IMPORT_ROW_OVERFLOW);
                    else
                        rXMLImport.SetRangeOverflowType(SCWARN_IMPORT_COLUMN_OVERFLOW);
                }

            } // if ( !pOUFormula )
        }
        UnlockSolarMutex();
    }
    bIsMerged = false;
    bHasSubTable = false;
    nMergedCols = 1;
    nMergedRows = 1;
    nCellsRepeated = 1;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
