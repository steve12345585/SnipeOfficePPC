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




//___________________________________________________________________
#include "XMLConsolidationContext.hxx"
#include "document.hxx"
#include "rangeutl.hxx"
#include "xmlimprt.hxx"
#include "XMLConverter.hxx"
#include <xmloff/nmspmap.hxx>
#include <xmloff/xmltoken.hxx>

using ::rtl::OUString;
using namespace ::com::sun::star;
using namespace xmloff::token;


//___________________________________________________________________

ScXMLConsolidationContext::ScXMLConsolidationContext(
        ScXMLImport& rImport,
        sal_uInt16 nPrfx,
        const OUString& rLName,
        const uno::Reference< xml::sax::XAttributeList >& xAttrList ) :
    SvXMLImportContext( rImport, nPrfx, rLName ),
    eFunction( SUBTOTAL_FUNC_NONE ),
    bLinkToSource( false ),
    bTargetAddr(false)
{
    ScXMLImport::MutexGuard aGuard(GetScImport());
    if( !xAttrList.is() ) return;

    sal_Int16               nAttrCount      = xAttrList->getLength();
    const SvXMLTokenMap&    rAttrTokenMap   = GetScImport().GetConsolidationAttrTokenMap();

    for( sal_Int16 nIndex = 0; nIndex < nAttrCount; ++nIndex )
    {
        const rtl::OUString& sAttrName  (xAttrList->getNameByIndex( nIndex ));
        const rtl::OUString& sValue     (xAttrList->getValueByIndex( nIndex ));
        OUString aLocalName;
        sal_uInt16 nPrefix      = GetScImport().GetNamespaceMap().GetKeyByAttrName( sAttrName, &aLocalName );

        switch( rAttrTokenMap.Get( nPrefix, aLocalName ) )
        {
            case XML_TOK_CONSOLIDATION_ATTR_FUNCTION:
                eFunction = ScXMLConverter::GetSubTotalFuncFromString( sValue );
            break;
            case XML_TOK_CONSOLIDATION_ATTR_SOURCE_RANGES:
                sSourceList = sValue;
            break;
            case XML_TOK_CONSOLIDATION_ATTR_TARGET_ADDRESS:
                {
                    sal_Int32 nOffset(0);
                    bTargetAddr = ScRangeStringConverter::GetAddressFromString(
                        aTargetAddr, sValue, GetScImport().GetDocument(), ::formula::FormulaGrammar::CONV_OOO, nOffset );
                }
                break;
            case XML_TOK_CONSOLIDATION_ATTR_USE_LABEL:
                sUseLabel = sValue;
            break;
            case XML_TOK_CONSOLIDATION_ATTR_LINK_TO_SOURCE:
                bLinkToSource = IsXMLToken(sValue, XML_TRUE);
            break;
        }
    }
}

ScXMLConsolidationContext::~ScXMLConsolidationContext()
{
}

SvXMLImportContext *ScXMLConsolidationContext::CreateChildContext(
        sal_uInt16 nPrefix,
        const OUString& rLName,
        const uno::Reference< xml::sax::XAttributeList>& /* xAttrList */ )
{
    return new SvXMLImportContext( GetImport(), nPrefix, rLName );
}

void ScXMLConsolidationContext::EndElement()
{
    if (bTargetAddr)
    {
        ScConsolidateParam aConsParam;
        aConsParam.nCol = aTargetAddr.Col();
        aConsParam.nRow = aTargetAddr.Row();
        aConsParam.nTab = aTargetAddr.Tab();
        aConsParam.eFunction = eFunction;

        sal_uInt16 nCount = (sal_uInt16) Min( ScRangeStringConverter::GetTokenCount( sSourceList ), (sal_Int32)0xFFFF );
        ScArea** ppAreas = nCount ? new ScArea*[ nCount ] : NULL;
        if( ppAreas )
        {
            sal_Int32 nOffset = 0;
            sal_uInt16 nIndex;
            for( nIndex = 0; nIndex < nCount; ++nIndex )
            {
                ppAreas[ nIndex ] = new ScArea;
                if ( !ScRangeStringConverter::GetAreaFromString(
                    *ppAreas[ nIndex ], sSourceList, GetScImport().GetDocument(), ::formula::FormulaGrammar::CONV_OOO, nOffset ) )
                {
                    //! handle error
                }
            }

            aConsParam.SetAreas( ppAreas, nCount );

            // array is copied in SetAreas
            for( nIndex = 0; nIndex < nCount; ++nIndex )
                delete ppAreas[nIndex];
            delete[] ppAreas;
        }

        aConsParam.bByCol = aConsParam.bByRow = false;
        if( IsXMLToken(sUseLabel, XML_COLUMN ) )
            aConsParam.bByCol = true;
        else if( IsXMLToken( sUseLabel, XML_ROW ) )
            aConsParam.bByRow = true;
        else if( IsXMLToken( sUseLabel, XML_BOTH ) )
            aConsParam.bByCol = aConsParam.bByRow = true;

        aConsParam.bReferenceData = bLinkToSource;

        ScDocument* pDoc = GetScImport().GetDocument();
        if( pDoc )
            pDoc->SetConsolidateDlgData( &aConsParam );
    }
    GetScImport().UnlockSolarMutex();
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
