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

#include <sot/formats.hxx>
#include <sfx2/app.hxx>
#include <sfx2/linkmgr.hxx>
#include "servobj.hxx"
#include "docsh.hxx"
#include "impex.hxx"
#include "brdcst.hxx"
#include "rangenam.hxx"
#include "sc.hrc"               // SC_HINT_AREAS_CHANGED

using namespace formula;

// -----------------------------------------------------------------------

static sal_Bool lcl_FillRangeFromName( ScRange& rRange, ScDocShell* pDocSh, const String& rName )
{
    if (pDocSh)
    {
        ScDocument* pDoc = pDocSh->GetDocument();
        ScRangeName* pNames = pDoc->GetRangeName();
        if (pNames)
        {
            const ScRangeData* pData = pNames->findByUpperName(ScGlobal::pCharClass->uppercase(rName));
            if (pData)
            {
                if ( pData->IsValidReference( rRange ) )
                    return sal_True;
            }
        }
    }
    return false;
}

ScServerObjectSvtListenerForwarder::ScServerObjectSvtListenerForwarder(
        ScServerObject* pObjP)
    : pObj(pObjP)
{
}

ScServerObjectSvtListenerForwarder::~ScServerObjectSvtListenerForwarder()
{
    //! do NOT access pObj
}

void ScServerObjectSvtListenerForwarder::Notify( SvtBroadcaster& /* rBC */, const SfxHint& rHint)
{
    pObj->Notify( aBroadcaster, rHint);
}

ScServerObject::ScServerObject( ScDocShell* pShell, const String& rItem ) :
    aForwarder( this ),
    pDocSh( pShell ),
    bRefreshListener( false )
{
    //  parse item string

    if ( lcl_FillRangeFromName( aRange, pDocSh, rItem ) )
    {
        aItemStr = rItem;               // must be parsed again on ref update
    }
    else
    {
        //  parse ref
        ScDocument* pDoc = pDocSh->GetDocument();
        SCTAB nTab = pDocSh->GetCurTab();
        aRange.aStart.SetTab( nTab );

        // For DDE link, we always must parse references using OOO A1 convention.

        if ( aRange.Parse( rItem, pDoc, FormulaGrammar::CONV_OOO ) & SCA_VALID )
        {
            // area reference
        }
        else if ( aRange.aStart.Parse( rItem, pDoc, FormulaGrammar::CONV_OOO ) & SCA_VALID )
        {
            // cell reference
            aRange.aEnd = aRange.aStart;
        }
        else
        {
            OSL_FAIL("ScServerObject: invalid item");
        }
    }

    pDocSh->GetDocument()->GetLinkManager()->InsertServer( this );
    pDocSh->GetDocument()->StartListeningArea( aRange, &aForwarder );

    StartListening(*pDocSh);        // um mitzubekommen, wenn die DocShell geloescht wird
    StartListening(*SFX_APP());     // for SC_HINT_AREAS_CHANGED
}

ScServerObject::~ScServerObject()
{
    Clear();
}

void ScServerObject::Clear()
{
    if (pDocSh)
    {
        ScDocShell* pTemp = pDocSh;
        pDocSh = NULL;

        pTemp->GetDocument()->EndListeningArea( aRange, &aForwarder );
        pTemp->GetDocument()->GetLinkManager()->RemoveServer( this );
        EndListening(*pTemp);
        EndListening(*SFX_APP());
    }
}

void ScServerObject::EndListeningAll()
{
    aForwarder.EndListeningAll();
    SfxListener::EndListeningAll();
}

sal_Bool ScServerObject::GetData(
        ::com::sun::star::uno::Any & rData /*out param*/,
        const String & rMimeType, sal_Bool /* bSynchron */ )
{
    if (!pDocSh)
        return false;

    // named ranges may have changed -> update aRange
    if ( aItemStr.Len() )
    {
        ScRange aNew;
        if ( lcl_FillRangeFromName( aNew, pDocSh, aItemStr ) && aNew != aRange )
        {
            aRange = aNew;
            bRefreshListener = sal_True;
        }
    }

    if ( bRefreshListener )
    {
        //  refresh the listeners now (this is called from a timer)

        EndListeningAll();
        pDocSh->GetDocument()->StartListeningArea( aRange, &aForwarder );
        StartListening(*pDocSh);
        StartListening(*SFX_APP());
        bRefreshListener = false;
    }

    String aDdeTextFmt = pDocSh->GetDdeTextFmt();
    ScDocument* pDoc = pDocSh->GetDocument();

    if( FORMAT_STRING == SotExchange::GetFormatIdFromMimeType( rMimeType ))
    {
        ScImportExport aObj( pDoc, aRange );
        if( aDdeTextFmt.GetChar(0) == 'F' )
            aObj.SetFormulas( sal_True );
        if( aDdeTextFmt.EqualsAscii( "SYLK" ) ||
            aDdeTextFmt.EqualsAscii( "FSYLK" ) )
        {
            rtl::OString aByteData;
            if( aObj.ExportByteString( aByteData, osl_getThreadTextEncoding(), SOT_FORMATSTR_ID_SYLK ) )
            {
                rData <<= ::com::sun::star::uno::Sequence< sal_Int8 >(
                                        (const sal_Int8*)aByteData.getStr(),
                                        aByteData.getLength() + 1 );
                return 1;
            }
            return 0;
        }
        if( aDdeTextFmt.EqualsAscii( "CSV" ) ||
            aDdeTextFmt.EqualsAscii( "FCSV" ) )
            aObj.SetSeparator( ',' );
        aObj.SetExportTextOptions( ScExportTextOptions( ScExportTextOptions::ToSpace, ' ', false ) );
        return aObj.ExportData( rMimeType, rData ) ? 1 : 0;
    }

    ScImportExport aObj( pDoc, aRange );
    aObj.SetExportTextOptions( ScExportTextOptions( ScExportTextOptions::ToSpace, ' ', false ) );
    if( aObj.IsRef() )
        return aObj.ExportData( rMimeType, rData ) ? 1 : 0;
    return 0;
}

void ScServerObject::Notify( SfxBroadcaster& rBC, const SfxHint& rHint )
{
    sal_Bool bDataChanged = false;

    //  DocShell can't be tested via type info, because SFX_HINT_DYING comes from the dtor
    if ( &rBC == pDocSh )
    {
        //  from DocShell, only SFX_HINT_DYING is interesting
        if ( rHint.ISA(SfxSimpleHint) && ((const SfxSimpleHint&)rHint).GetId() == SFX_HINT_DYING )
        {
            pDocSh = NULL;
            EndListening(*SFX_APP());
            //  don't access DocShell anymore for EndListening etc.
        }
    }
    else if (rBC.ISA(SfxApplication))
    {
        if ( aItemStr.Len() && rHint.ISA(SfxSimpleHint) &&
                ((const SfxSimpleHint&)rHint).GetId() == SC_HINT_AREAS_CHANGED )
        {
            //  check if named range was modified
            ScRange aNew;
            if ( lcl_FillRangeFromName( aNew, pDocSh, aItemStr ) && aNew != aRange )
                bDataChanged = sal_True;
        }
    }
    else
    {
        //  must be from Area broadcasters

        const ScHint* pScHint = PTR_CAST( ScHint, &rHint );
        if( pScHint && (pScHint->GetId() & (SC_HINT_DATACHANGED | SC_HINT_DYING)) )
            bDataChanged = sal_True;
        else if (rHint.ISA(ScAreaChangedHint))      // position of broadcaster changed
        {
            ScRange aNewRange = ((const ScAreaChangedHint&)rHint).GetRange();
            if ( aRange != aNewRange )
            {
                bRefreshListener = sal_True;
                bDataChanged = sal_True;
            }
        }
        else if (rHint.ISA(SfxSimpleHint))
        {
            sal_uLong nId = ((const SfxSimpleHint&)rHint).GetId();
            if (nId == SFX_HINT_DYING)
            {
                //  If the range is being deleted, listening must be restarted
                //  after the deletion is complete (done in GetData)
                bRefreshListener = sal_True;
                bDataChanged = sal_True;
            }
        }
    }

    if ( bDataChanged && HasDataLinks() )
        SvLinkSource::NotifyDataChanged();
}





/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
