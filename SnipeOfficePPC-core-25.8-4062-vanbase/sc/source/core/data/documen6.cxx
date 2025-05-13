/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
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

#include "scitems.hxx"
#include <editeng/scripttypeitem.hxx>

#include <com/sun/star/i18n/BreakIterator.hpp>
#include <com/sun/star/i18n/ScriptType.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <comphelper/processfactory.hxx>

#include "document.hxx"
#include "cell.hxx"
#include "cellform.hxx"
#include "patattr.hxx"
#include "scrdata.hxx"
#include "poolhelp.hxx"
#include "attrib.hxx"

using namespace com::sun::star;

//
//  this file is compiled with exceptions enabled
//  put functions here that need exceptions!
//

// -----------------------------------------------------------------------

const uno::Reference< i18n::XBreakIterator >& ScDocument::GetBreakIterator()
{
    if ( !pScriptTypeData )
        pScriptTypeData = new ScScriptTypeData;
    if ( !pScriptTypeData->xBreakIter.is() )
    {
        pScriptTypeData->xBreakIter = i18n::BreakIterator::create( comphelper::getComponentContext(xServiceManager) );
    }
    return pScriptTypeData->xBreakIter;
}

bool ScDocument::HasStringWeakCharacters( const rtl::OUString& rString )
{
    if (!rString.isEmpty())
    {
        uno::Reference<i18n::XBreakIterator> xBreakIter = GetBreakIterator();
        if ( xBreakIter.is() )
        {
            sal_Int32 nLen = rString.getLength();

            sal_Int32 nPos = 0;
            do
            {
                sal_Int16 nType = xBreakIter->getScriptType( rString, nPos );
                if ( nType == i18n::ScriptType::WEAK )
                    return true;                            // found

                nPos = xBreakIter->endOfScript( rString, nPos, nType );
            }
            while ( nPos >= 0 && nPos < nLen );
        }
    }

    return false;       // none found
}

sal_uInt8 ScDocument::GetStringScriptType( const rtl::OUString& rString )
{

    sal_uInt8 nRet = 0;
    if (!rString.isEmpty())
    {
        uno::Reference<i18n::XBreakIterator> xBreakIter = GetBreakIterator();
        if ( xBreakIter.is() )
        {
            sal_Int32 nLen = rString.getLength();

            sal_Int32 nPos = 0;
            do
            {
                sal_Int16 nType = xBreakIter->getScriptType( rString, nPos );
                switch ( nType )
                {
                    case i18n::ScriptType::LATIN:
                        nRet |= SCRIPTTYPE_LATIN;
                        break;
                    case i18n::ScriptType::ASIAN:
                        nRet |= SCRIPTTYPE_ASIAN;
                        break;
                    case i18n::ScriptType::COMPLEX:
                        nRet |= SCRIPTTYPE_COMPLEX;
                        break;
                    // WEAK is ignored
                }
                nPos = xBreakIter->endOfScript( rString, nPos, nType );
            }
            while ( nPos >= 0 && nPos < nLen );
        }
    }
    return nRet;
}

sal_uInt8 ScDocument::GetCellScriptType( ScBaseCell* pCell, sal_uLong nNumberFormat )
{
    if ( !pCell )
        return 0;       // empty

    sal_uInt8 nStored = pCell->GetScriptType();
    if ( nStored != SC_SCRIPTTYPE_UNKNOWN )         // stored value valid?
        return nStored;                             // use stored value

    rtl::OUString aStr;
    Color* pColor;
    ScCellFormat::GetString( pCell, nNumberFormat, aStr, &pColor, *xPoolHelper->GetFormTable() );

    sal_uInt8 nRet = GetStringScriptType( aStr );

    pCell->SetScriptType( nRet );       // store for later calls

    return nRet;
}

sal_uInt8 ScDocument::GetScriptType( SCCOL nCol, SCROW nRow, SCTAB nTab, ScBaseCell* pCell )
{
    // if cell is not passed, take from document

    if (!pCell)
    {
        pCell = GetCell( ScAddress( nCol, nRow, nTab ) );
        if ( !pCell )
            return 0;       // empty
    }

    // if script type is set, don't have to get number formats

    sal_uInt8 nStored = pCell->GetScriptType();
    if ( nStored != SC_SCRIPTTYPE_UNKNOWN )         // stored value valid?
        return nStored;                             // use stored value

    // include number formats from conditional formatting

    const ScPatternAttr* pPattern = GetPattern( nCol, nRow, nTab );
    if (!pPattern) return 0;
    const SfxItemSet* pCondSet = NULL;
    if ( !((const ScCondFormatItem&)pPattern->GetItem(ATTR_CONDITIONAL)).GetCondFormatData().empty() )
        pCondSet = GetCondResult( nCol, nRow, nTab );

    sal_uLong nFormat = pPattern->GetNumberFormat( xPoolHelper->GetFormTable(), pCondSet );
    return GetCellScriptType( pCell, nFormat );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
