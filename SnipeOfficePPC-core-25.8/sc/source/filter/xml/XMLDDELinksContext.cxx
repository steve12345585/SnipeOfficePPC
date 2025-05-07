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
#include "XMLDDELinksContext.hxx"
#include "xmlimprt.hxx"
#include "document.hxx"
#include "scmatrix.hxx"
#include <xmloff/xmltoken.hxx>
#include <xmloff/xmlnmspe.hxx>
#include <xmloff/nmspmap.hxx>
#include <sax/tools/converter.hxx>

using namespace com::sun::star;
using namespace xmloff::token;
using ::rtl::OUString;

using rtl::OUString;

//------------------------------------------------------------------

ScXMLDDELinksContext::ScXMLDDELinksContext( ScXMLImport& rImport,
                                      sal_uInt16 nPrfx,
                                      const ::rtl::OUString& rLName,
                                      const ::com::sun::star::uno::Reference<
                                      ::com::sun::star::xml::sax::XAttributeList>& /* xAttrList */ ) :
    SvXMLImportContext( rImport, nPrfx, rLName )
{
    // here are no attributes
    rImport.LockSolarMutex();
}

ScXMLDDELinksContext::~ScXMLDDELinksContext()
{
    GetScImport().UnlockSolarMutex();
}

SvXMLImportContext *ScXMLDDELinksContext::CreateChildContext( sal_uInt16 nPrefix,
                                            const ::rtl::OUString& rLName,
                                            const ::com::sun::star::uno::Reference<
                                          ::com::sun::star::xml::sax::XAttributeList>& xAttrList )
{
    SvXMLImportContext *pContext = 0;

    if ((nPrefix == XML_NAMESPACE_TABLE) && IsXMLToken(rLName, XML_DDE_LINK))
        pContext = new ScXMLDDELinkContext(GetScImport(), nPrefix, rLName, xAttrList);

    if( !pContext )
        pContext = new SvXMLImportContext( GetImport(), nPrefix, rLName );

    return pContext;
}

void ScXMLDDELinksContext::EndElement()
{
}

ScXMLDDELinkContext::ScXMLDDELinkContext( ScXMLImport& rImport,
                                      sal_uInt16 nPrfx,
                                      const ::rtl::OUString& rLName,
                                      const ::com::sun::star::uno::Reference<
                                      ::com::sun::star::xml::sax::XAttributeList>& /* xAttrList */ ) :
    SvXMLImportContext( rImport, nPrfx, rLName ),
    aDDELinkTable(),
    aDDELinkRow(),
    sApplication(),
    sTopic(),
    sItem(),
    nPosition(-1),
    nColumns(0),
    nRows(0),
    nMode(SC_DDE_DEFAULT)
{
    // here are no attributes
}

ScXMLDDELinkContext::~ScXMLDDELinkContext()
{
}

SvXMLImportContext *ScXMLDDELinkContext::CreateChildContext( sal_uInt16 nPrefix,
                                            const ::rtl::OUString& rLName,
                                            const ::com::sun::star::uno::Reference<
                                          ::com::sun::star::xml::sax::XAttributeList>& xAttrList )
{
    SvXMLImportContext *pContext = 0;

    if ((nPrefix == XML_NAMESPACE_OFFICE) && IsXMLToken(rLName, XML_DDE_SOURCE))
        pContext = new ScXMLDDESourceContext(GetScImport(), nPrefix, rLName, xAttrList, this);
    else if ((nPrefix == XML_NAMESPACE_TABLE) && IsXMLToken(rLName, XML_TABLE))
        pContext = new ScXMLDDETableContext(GetScImport(), nPrefix, rLName, xAttrList, this);

    if( !pContext )
        pContext = new SvXMLImportContext( GetImport(), nPrefix, rLName );

    return pContext;
}

void ScXMLDDELinkContext::CreateDDELink()
{
    if (GetScImport().GetDocument() &&
        !sApplication.isEmpty() &&
        !sTopic.isEmpty() &&
        !sItem.isEmpty())
    {
        String sAppl(sApplication);
        String sTop(sTopic);
        String sIt(sItem);
        GetScImport().GetDocument()->CreateDdeLink(sAppl, sTop, sIt, nMode, ScMatrixRef());
        sal_uInt16 nPos;
        if(GetScImport().GetDocument()->FindDdeLink(sAppl, sTop, sIt, nMode, nPos))
            nPosition = nPos;
        else
            nPosition = -1;
        OSL_ENSURE(nPosition > -1, "DDE Link not inserted");
    }
}

void ScXMLDDELinkContext::AddCellToRow(const ScDDELinkCell& aCell)
{
    aDDELinkRow.push_back(aCell);
}

void ScXMLDDELinkContext::AddRowsToTable(const sal_Int32 nRowsP)
{
    for (sal_Int32 i = 0; i < nRowsP; ++i)
        aDDELinkTable.insert(aDDELinkTable.end(), aDDELinkRow.begin(), aDDELinkRow.end());
    aDDELinkRow.clear();
}

void ScXMLDDELinkContext::EndElement()
{
    if (nPosition > -1 && nColumns && nRows && GetScImport().GetDocument())
    {
        bool bSizeMatch = (static_cast<size_t>(nColumns * nRows) == aDDELinkTable.size());
        OSL_ENSURE( bSizeMatch, "ScXMLDDELinkContext::EndElement: matrix dimension doesn't match cells count");
        // Excel writes bad ODF in that it does not write the
        // table:number-columns-repeated attribute of the
        // <table:table-column> element, but apparently uses the number of
        // <table:table-cell> elements within a <table:table-row> element to
        // determine the column count instead. Be lenient ...
        if (!bSizeMatch && nColumns == 1)
        {
            nColumns = aDDELinkTable.size() / nRows;
            OSL_ENSURE( static_cast<size_t>(nColumns * nRows) == aDDELinkTable.size(),
                    "ScXMLDDELinkContext::EndElement: adapted matrix dimension doesn't match either");
        }
        ScMatrixRef pMatrix = new ScMatrix( static_cast<SCSIZE>(nColumns), static_cast<SCSIZE>(nRows) );
        sal_Int32 nCol(0);
        sal_Int32 nRow(-1);
        sal_Int32 nIndex(0);
        ScDDELinkCells::iterator aItr(aDDELinkTable.begin());
        ScDDELinkCells::iterator aEndItr(aDDELinkTable.end());
        while (aItr != aEndItr)
        {
            if (nIndex % nColumns == 0)
            {
                ++nRow;
                nCol = 0;
            }
            else
                ++nCol;

            SCSIZE nScCol( static_cast< SCSIZE >( nCol ) );
            SCSIZE nScRow( static_cast< SCSIZE >( nRow ) );
            if( aItr->bEmpty )
                pMatrix->PutEmpty( nScCol, nScRow );
            else if( aItr->bString )
                pMatrix->PutString( aItr->sValue, nScCol, nScRow );
            else
                pMatrix->PutDouble( aItr->fValue, nScCol, nScRow );

            ++nIndex;
            ++aItr;
        }

        GetScImport().GetDocument()->SetDdeLinkResultMatrix( static_cast< sal_uInt16 >( nPosition ), pMatrix );
    }
}

ScXMLDDESourceContext::ScXMLDDESourceContext( ScXMLImport& rImport,
                                      sal_uInt16 nPrfx,
                                      const ::rtl::OUString& rLName,
                                      const ::com::sun::star::uno::Reference<
                                      ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
                                        ScXMLDDELinkContext* pTempDDELink) :
    SvXMLImportContext( rImport, nPrfx, rLName ),
    pDDELink(pTempDDELink)
{
    if( !xAttrList.is() ) return;

    sal_Int16               nAttrCount      = xAttrList->getLength();

    for( sal_Int16 nIndex = 0; nIndex < nAttrCount; ++nIndex )
    {
        const rtl::OUString& sAttrName  (xAttrList->getNameByIndex( nIndex ));
        const rtl::OUString& sValue     (xAttrList->getValueByIndex( nIndex ));
        OUString aLocalName;
        sal_uInt16 nPrefix      = GetScImport().GetNamespaceMap().GetKeyByAttrName( sAttrName, &aLocalName );

        if (nPrefix == XML_NAMESPACE_OFFICE)
        {
            if (IsXMLToken(aLocalName, XML_DDE_APPLICATION))
                pDDELink->SetApplication(sValue);
            else if (IsXMLToken(aLocalName, XML_DDE_TOPIC))
                pDDELink->SetTopic(sValue);
            else if (IsXMLToken(aLocalName, XML_DDE_ITEM))
                pDDELink->SetItem(sValue);
        }
        else if ((nPrefix == XML_NAMESPACE_TABLE) && IsXMLToken(aLocalName, XML_CONVERSION_MODE))
        {
            if (IsXMLToken(sValue, XML_INTO_ENGLISH_NUMBER))
                pDDELink->SetMode(SC_DDE_ENGLISH);
            else if (IsXMLToken(sValue, XML_KEEP_TEXT))
                pDDELink->SetMode(SC_DDE_TEXT);
            else
                pDDELink->SetMode(SC_DDE_DEFAULT);
        }
    }
}

ScXMLDDESourceContext::~ScXMLDDESourceContext()
{
}

SvXMLImportContext *ScXMLDDESourceContext::CreateChildContext( sal_uInt16 nPrefix,
                                            const ::rtl::OUString& rLName,
                                            const ::com::sun::star::uno::Reference<
                                        ::com::sun::star::xml::sax::XAttributeList>& /* xAttrList */ )
{
    SvXMLImportContext *pContext = new SvXMLImportContext( GetImport(), nPrefix, rLName );

    return pContext;
}

void ScXMLDDESourceContext::EndElement()
{
    pDDELink->CreateDDELink();
}

ScXMLDDETableContext::ScXMLDDETableContext( ScXMLImport& rImport,
                                      sal_uInt16 nPrfx,
                                      const ::rtl::OUString& rLName,
                                      const ::com::sun::star::uno::Reference<
                                      ::com::sun::star::xml::sax::XAttributeList>& /* xAttrList */,
                                        ScXMLDDELinkContext* pTempDDELink) :
    SvXMLImportContext( rImport, nPrfx, rLName ),
    pDDELink(pTempDDELink)
{
    // here are no attributes
}

ScXMLDDETableContext::~ScXMLDDETableContext()
{
}

SvXMLImportContext *ScXMLDDETableContext::CreateChildContext( sal_uInt16 nPrefix,
                                            const ::rtl::OUString& rLName,
                                            const ::com::sun::star::uno::Reference<
                                          ::com::sun::star::xml::sax::XAttributeList>& xAttrList )
{
    SvXMLImportContext *pContext = NULL;

    if (nPrefix == XML_NAMESPACE_TABLE)
    {
        if (IsXMLToken(rLName, XML_TABLE_COLUMN))
            pContext = new ScXMLDDEColumnContext(GetScImport(), nPrefix, rLName, xAttrList, pDDELink);
        else if (IsXMLToken(rLName, XML_TABLE_ROW))
            pContext = new ScXMLDDERowContext(GetScImport(), nPrefix, rLName, xAttrList, pDDELink);
    }

    if (!pContext)
        pContext = new SvXMLImportContext( GetImport(), nPrefix, rLName );

    return pContext;
}

void ScXMLDDETableContext::EndElement()
{
}

ScXMLDDEColumnContext::ScXMLDDEColumnContext( ScXMLImport& rImport,
                                      sal_uInt16 nPrfx,
                                      const ::rtl::OUString& rLName,
                                      const ::com::sun::star::uno::Reference<
                                      ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
                                        ScXMLDDELinkContext* pTempDDELink) :
    SvXMLImportContext( rImport, nPrfx, rLName ),
    pDDELink(pTempDDELink)
{
    if( !xAttrList.is() ) return;
    sal_Int32 nCols(1);

    sal_Int16               nAttrCount      = xAttrList->getLength();

    for( sal_Int16 nIndex = 0; nIndex < nAttrCount; ++nIndex )
    {
        const rtl::OUString& sAttrName  (xAttrList->getNameByIndex( nIndex ));
        const rtl::OUString& sValue     (xAttrList->getValueByIndex( nIndex ));
        OUString aLocalName;
        sal_uInt16 nPrefix      = GetScImport().GetNamespaceMap().GetKeyByAttrName( sAttrName, &aLocalName );

        if (nPrefix == XML_NAMESPACE_TABLE)
            if (IsXMLToken(aLocalName, XML_NUMBER_COLUMNS_REPEATED))
            {
                ::sax::Converter::convertNumber(nCols, sValue);
            }
    }
    pDDELink->AddColumns(nCols);
}

ScXMLDDEColumnContext::~ScXMLDDEColumnContext()
{
}

SvXMLImportContext *ScXMLDDEColumnContext::CreateChildContext( sal_uInt16 nPrefix,
                                            const ::rtl::OUString& rLName,
                                            const ::com::sun::star::uno::Reference<
                                        ::com::sun::star::xml::sax::XAttributeList>& /* xAttrList */ )
{
    SvXMLImportContext *pContext = new SvXMLImportContext( GetImport(), nPrefix, rLName );

    return pContext;
}

void ScXMLDDEColumnContext::EndElement()
{
}

ScXMLDDERowContext::ScXMLDDERowContext( ScXMLImport& rImport,
                                      sal_uInt16 nPrfx,
                                      const ::rtl::OUString& rLName,
                                      const ::com::sun::star::uno::Reference<
                                      ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
                                        ScXMLDDELinkContext* pTempDDELink) :
    SvXMLImportContext( rImport, nPrfx, rLName ),
    pDDELink(pTempDDELink),
    nRows(1)
{
    if( !xAttrList.is() ) return;

    sal_Int16               nAttrCount      = xAttrList->getLength();

    for( sal_Int16 nIndex = 0; nIndex < nAttrCount; ++nIndex )
    {
        const rtl::OUString& sAttrName  (xAttrList->getNameByIndex( nIndex ));
        const rtl::OUString& sValue     (xAttrList->getValueByIndex( nIndex ));
        OUString aLocalName;
        sal_uInt16 nPrefix      = GetScImport().GetNamespaceMap().GetKeyByAttrName( sAttrName, &aLocalName );

        if (nPrefix == XML_NAMESPACE_TABLE)
            if (IsXMLToken(aLocalName, XML_NUMBER_ROWS_REPEATED))
            {
                ::sax::Converter::convertNumber(nRows, sValue);
            }
    }
    pDDELink->AddRows(nRows);
}

ScXMLDDERowContext::~ScXMLDDERowContext()
{
}

SvXMLImportContext *ScXMLDDERowContext::CreateChildContext( sal_uInt16 nPrefix,
                                            const ::rtl::OUString& rLName,
                                            const ::com::sun::star::uno::Reference<
                                          ::com::sun::star::xml::sax::XAttributeList>& xAttrList )
{
    SvXMLImportContext *pContext = NULL;

    if (nPrefix == XML_NAMESPACE_TABLE)
        if (IsXMLToken(rLName, XML_TABLE_CELL))
            pContext = new ScXMLDDECellContext(GetScImport(), nPrefix, rLName, xAttrList, pDDELink);

    if (!pContext)
        pContext = new SvXMLImportContext( GetImport(), nPrefix, rLName );

    return pContext;
}

void ScXMLDDERowContext::EndElement()
{
    pDDELink->AddRowsToTable(nRows);
}

ScXMLDDECellContext::ScXMLDDECellContext( ScXMLImport& rImport,
                                      sal_uInt16 nPrfx,
                                      const ::rtl::OUString& rLName,
                                      const ::com::sun::star::uno::Reference<
                                      ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
                                        ScXMLDDELinkContext* pTempDDELink) :
    SvXMLImportContext( rImport, nPrfx, rLName ),
    sValue(),
    fValue(),
    nCells(1),
    bString(true),
    bString2(true),
    bEmpty(true),
    pDDELink(pTempDDELink)
{
    if( !xAttrList.is() ) return;

    sal_Int16               nAttrCount      = xAttrList->getLength();

    for( sal_Int16 nIndex = 0; nIndex < nAttrCount; ++nIndex )
    {
        const rtl::OUString& sAttrName  (xAttrList->getNameByIndex( nIndex ));
        const rtl::OUString& sTempValue (xAttrList->getValueByIndex( nIndex ));
        OUString aLocalName;
        sal_uInt16 nPrefix      = GetScImport().GetNamespaceMap().GetKeyByAttrName( sAttrName, &aLocalName );

        if (nPrefix == XML_NAMESPACE_OFFICE)
        {
            if (IsXMLToken(aLocalName, XML_VALUE_TYPE))
            {
                if (IsXMLToken(sTempValue, XML_STRING))
                    bString = true;
                else
                    bString = false;
            }
            else if (IsXMLToken(aLocalName, XML_STRING_VALUE))
            {
                sValue = sTempValue;
                bEmpty = false;
                bString2 = true;
            }
            else if (IsXMLToken(aLocalName, XML_VALUE))
            {
                ::sax::Converter::convertDouble(fValue, sTempValue);
                bEmpty = false;
                bString2 = false;
            }
        }
        else if (nPrefix == XML_NAMESPACE_TABLE)
        {
            if (IsXMLToken(aLocalName, XML_NUMBER_COLUMNS_REPEATED))
            {
                ::sax::Converter::convertNumber(nCells, sTempValue);
            }
        }
    }
}

ScXMLDDECellContext::~ScXMLDDECellContext()
{
}

SvXMLImportContext *ScXMLDDECellContext::CreateChildContext( sal_uInt16 nPrefix,
                                            const ::rtl::OUString& rLName,
                                            const ::com::sun::star::uno::Reference<
                                        ::com::sun::star::xml::sax::XAttributeList>& /* xAttrList */ )
{
    SvXMLImportContext *pContext = new SvXMLImportContext( GetImport(), nPrefix, rLName );

    return pContext;
}

void ScXMLDDECellContext::EndElement()
{
    OSL_ENSURE(bString == bString2, "something wrong with this type");
    ScDDELinkCell aCell;
    aCell.sValue = sValue;
    aCell.fValue = fValue;
    aCell.bEmpty = bEmpty;
    aCell.bString = bString2;
    for(sal_Int32 i = 0; i < nCells; ++i)
        pDDELink->AddCellToRow(aCell);
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
