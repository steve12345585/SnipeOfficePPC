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


#include <comphelper/string.hxx>
#include <sfx2/linkmgr.hxx>
#include <doc.hxx>
#include <editsh.hxx>
#include <ndtxt.hxx>
#include <fmtfld.hxx>
#include <txtfld.hxx>
#include <ddefld.hxx>
#include <swtable.hxx>
#include <swbaslnk.hxx>
#include <swddetbl.hxx>
#include <unofldmid.h>
#include <hints.hxx>

using rtl::OUString;
using namespace ::com::sun::star;

#define DDE_TXT_ENCODING    osl_getThreadTextEncoding()

/*--------------------------------------------------------------------
    Beschreibung: Globale Variablen
 --------------------------------------------------------------------*/

class SwIntrnlRefLink : public SwBaseLink
{
    SwDDEFieldType& rFldType;
public:
    SwIntrnlRefLink( SwDDEFieldType& rType, sal_uInt16 nUpdateType, sal_uInt16 nFmt )
        : SwBaseLink( nUpdateType, nFmt ),
        rFldType( rType )
    {}

    virtual void Closed();
    virtual ::sfx2::SvBaseLink::UpdateResult DataChanged(
        const String& rMimeType, const ::com::sun::star::uno::Any & rValue );

    virtual const SwNode* GetAnchor() const;
    virtual sal_Bool IsInRange( sal_uLong nSttNd, sal_uLong nEndNd, xub_StrLen nStt = 0,
                            xub_StrLen nEnd = STRING_NOTFOUND ) const;
};


::sfx2::SvBaseLink::UpdateResult SwIntrnlRefLink::DataChanged( const String& rMimeType,
                                const uno::Any & rValue )
{
    switch( SotExchange::GetFormatIdFromMimeType( rMimeType ) )
    {
    case FORMAT_STRING:
        if( !IsNoDataFlag() )
        {
            uno::Sequence< sal_Int8 > aSeq;
            rValue >>= aSeq;
            String sStr( (sal_Char*)aSeq.getConstArray(), static_cast<xub_StrLen>(aSeq.getLength()),
                               DDE_TXT_ENCODING  );

            // CR-LF am Ende entfernen, ist ueberfluessig!
            xub_StrLen n = sStr.Len();
            while( n && 0 == sStr.GetChar( n-1 ) )
                --n;
            if( n && 0x0a == sStr.GetChar( n-1 ) )
                --n;
            if( n && 0x0d == sStr.GetChar( n-1 ) )
                --n;

            sal_Bool bDel = n != sStr.Len();
            if( bDel )
                sStr.Erase( n );

            rFldType.SetExpansion( sStr );
            // erst Expansion setzen! (sonst wird das Flag geloescht!)
            rFldType.SetCRLFDelFlag( bDel );
        }
        break;

    // weitere Formate ...
    default:
        return SUCCESS;
    }

    OSL_ENSURE( rFldType.GetDoc(), "Kein pDoc" );

    // keine Abhaengigen mehr?
    if( rFldType.GetDepends() && !rFldType.IsModifyLocked() && !ChkNoDataFlag() )
    {
        ViewShell* pSh;
        SwEditShell* pESh = rFldType.GetDoc()->GetEditShell( &pSh );

        // dann suchen wir uns mal alle Felder. Wird kein gueltiges
        // gefunden, dann Disconnecten wir uns!
        SwMsgPoolItem aUpdateDDE( RES_UPDATEDDETBL );
        int bCallModify = sal_False;
        rFldType.LockModify();

        SwClientIter aIter( rFldType );     // TODO
        SwClient * pLast = aIter.GoStart();
        if( pLast )     // konnte zum Anfang gesprungen werden ??
            do {
                // eine DDE-Tabelle oder ein DDE-FeldAttribut im Text
                if( !pLast->IsA( TYPE( SwFmtFld ) ) ||
                    ((SwFmtFld*)pLast)->GetTxtFld() )
                {
                    if( !bCallModify )
                    {
                        if( pESh )
                            pESh->StartAllAction();
                        else if( pSh )
                            pSh->StartAction();
                    }
                    pLast->ModifyNotification( 0, &aUpdateDDE );
                    bCallModify = sal_True;
                }
            } while( 0 != ( pLast = aIter++ ));

        rFldType.UnlockModify();

        if( bCallModify )
        {
            if( pESh )
                pESh->EndAllAction();
            else if( pSh )
                pSh->EndAction();

            if( pSh )
                pSh->GetDoc()->SetModified();
        }
    }

    return SUCCESS;
}

void SwIntrnlRefLink::Closed()
{
    if( rFldType.GetDoc() && !rFldType.GetDoc()->IsInDtor() )
    {
        // Advise verabschiedet sich, alle Felder in Text umwandeln ?
        ViewShell* pSh;
        SwEditShell* pESh = rFldType.GetDoc()->GetEditShell( &pSh );
        if( pESh )
        {
            pESh->StartAllAction();
            pESh->FieldToText( &rFldType );
            pESh->EndAllAction();
        }
        else
        {
            pSh->StartAction();
            // am Doc aufrufen ??
            pSh->EndAction();
        }
    }
    SvBaseLink::Closed();
}

const SwNode* SwIntrnlRefLink::GetAnchor() const
{
    // hier sollte irgend ein Anchor aus dem normalen Nodes-Array reichen
    const SwNode* pNd = 0;
    SwClientIter aIter( rFldType );     // TODO
    SwClient * pLast = aIter.GoStart();
    if( pLast )     // konnte zum Anfang gesprungen werden ??
        do {
            // eine DDE-Tabelle oder ein DDE-FeldAttribut im Text
            if( !pLast->IsA( TYPE( SwFmtFld ) ))
            {
                SwDepend* pDep = (SwDepend*)pLast;
                SwDDETable* pDDETbl = (SwDDETable*)pDep->GetToTell();
                pNd = pDDETbl->GetTabSortBoxes()[0]->GetSttNd();
            }
            else if( ((SwFmtFld*)pLast)->GetTxtFld() )
                pNd = ((SwFmtFld*)pLast)->GetTxtFld()->GetpTxtNode();

            if( pNd && &rFldType.GetDoc()->GetNodes() == &pNd->GetNodes() )
                break;
            pNd = 0;
        } while( 0 != ( pLast = aIter++ ));

    return pNd;
}

sal_Bool SwIntrnlRefLink::IsInRange( sal_uLong nSttNd, sal_uLong nEndNd,
                                xub_StrLen nStt, xub_StrLen nEnd ) const
{
    // hier sollte irgend ein Anchor aus dem normalen Nodes-Array reichen
    SwNodes* pNds = &rFldType.GetDoc()->GetNodes();
    SwClientIter aIter( rFldType );         // TODO
    SwClient * pLast = aIter.GoStart();
    if( pLast )     // konnte zum Anfang gesprungen werden ??
        do {
            // eine DDE-Tabelle oder ein DDE-FeldAttribut im Text
            if( !pLast->IsA( TYPE( SwFmtFld ) ))
            {
                SwDepend* pDep = (SwDepend*)pLast;
                SwDDETable* pDDETbl = (SwDDETable*)pDep->GetToTell();
                const SwTableNode* pTblNd = pDDETbl->GetTabSortBoxes()[0]->
                                GetSttNd()->FindTableNode();
                if( pTblNd->GetNodes().IsDocNodes() &&
                    nSttNd < pTblNd->EndOfSectionIndex() &&
                    nEndNd > pTblNd->GetIndex() )
                    return sal_True;
            }
            else if( ((SwFmtFld*)pLast)->GetTxtFld() )
            {
                const SwTxtFld* pTFld = ((SwFmtFld*)pLast)->GetTxtFld();
                const SwTxtNode* pNd = pTFld->GetpTxtNode();
                if( pNd && pNds == &pNd->GetNodes() )
                {
                    sal_uLong nNdPos = pNd->GetIndex();
                    if( nSttNd <= nNdPos && nNdPos <= nEndNd &&
                        ( nNdPos != nSttNd || *pTFld->GetStart() >= nStt ) &&
                        ( nNdPos != nEndNd || *pTFld->GetStart() < nEnd ))
                        return sal_True;
                }
            }
        } while( 0 != ( pLast = aIter++ ));

    return sal_False;
}

SwDDEFieldType::SwDDEFieldType(const String& rName,
                                const String& rCmd, sal_uInt16 nUpdateType )
    : SwFieldType( RES_DDEFLD ),
    aName( rName ), pDoc( 0 ), nRefCnt( 0 )
{
    bCRLFFlag = bDeleted = sal_False;
    refLink = new SwIntrnlRefLink( *this, nUpdateType, FORMAT_STRING );
    SetCmd( rCmd );
}

SwDDEFieldType::~SwDDEFieldType()
{
    if( pDoc && !pDoc->IsInDtor() )
        pDoc->GetLinkManager().Remove( refLink );
    refLink->Disconnect();
}

SwFieldType* SwDDEFieldType::Copy() const
{
    SwDDEFieldType* pType = new SwDDEFieldType( aName, GetCmd(), GetType() );
    pType->aExpansion = aExpansion;
    pType->bCRLFFlag = bCRLFFlag;
    pType->bDeleted = bDeleted;
    pType->SetDoc( pDoc );
    return pType;
}

const rtl::OUString& SwDDEFieldType::GetName() const
{
    return aName;
}

void SwDDEFieldType::SetCmd( const String& rStr )
{
    String sCmd( rStr );
    xub_StrLen nPos;
    while( STRING_NOTFOUND != (nPos = sCmd.SearchAscii( "  " )) )
        sCmd.Erase( nPos, 1 );
    refLink->SetLinkSourceName( sCmd );
}

String SwDDEFieldType::GetCmd() const
{
    return refLink->GetLinkSourceName();
}

void SwDDEFieldType::SetDoc( SwDoc* pNewDoc )
{
    if( pNewDoc == pDoc )
        return;

    if( pDoc && refLink.Is() )
    {
        OSL_ENSURE( !nRefCnt, "wie kommen die Referenzen rueber?" );
        pDoc->GetLinkManager().Remove( refLink );
    }

    pDoc = pNewDoc;
    if( pDoc && nRefCnt )
    {
        refLink->SetVisible( pDoc->IsVisibleLinks() );
        pDoc->GetLinkManager().InsertDDELink( refLink );
    }
}


void SwDDEFieldType::_RefCntChgd()
{
    if( nRefCnt )
    {
        refLink->SetVisible( pDoc->IsVisibleLinks() );
        pDoc->GetLinkManager().InsertDDELink( refLink );
        if( pDoc->GetCurrentViewShell() )   //swmod 071108//swmod 071225
            UpdateNow();
    }
    else
    {
        Disconnect();
        pDoc->GetLinkManager().Remove( refLink );
    }
}

bool SwDDEFieldType::QueryValue( uno::Any& rVal, sal_uInt16 nWhichId ) const
{
    sal_uInt8 nPart = 0;
    switch( nWhichId )
    {
    case FIELD_PROP_PAR2:      nPart = 3; break;
    case FIELD_PROP_PAR4:      nPart = 2; break;
    case FIELD_PROP_SUBTYPE:   nPart = 1; break;
    case FIELD_PROP_BOOL1:
        {
            sal_Bool bSet = GetType() == sfx2::LINKUPDATE_ALWAYS ? sal_True : sal_False;
            rVal.setValue(&bSet, ::getBooleanCppuType());
        }
        break;
    case FIELD_PROP_PAR5:
        rVal <<= ::rtl::OUString(aExpansion);
    break;
    default:
        OSL_FAIL("illegal property");
    }
    if( nPart )
        rVal <<= OUString(GetCmd().GetToken(nPart-1, sfx2::cTokenSeperator));
    return true;
}

bool SwDDEFieldType::PutValue( const uno::Any& rVal, sal_uInt16 nWhichId )
{
    sal_uInt8 nPart = 0;
    switch( nWhichId )
    {
    case FIELD_PROP_PAR2:      nPart = 3; break;
    case FIELD_PROP_PAR4:      nPart = 2; break;
    case FIELD_PROP_SUBTYPE:   nPart = 1; break;
    case FIELD_PROP_BOOL1:
        SetType( static_cast<sal_uInt16>(*(sal_Bool*)rVal.getValue() ?
                                     sfx2::LINKUPDATE_ALWAYS :
                                     sfx2::LINKUPDATE_ONCALL ) );
        break;
    case FIELD_PROP_PAR5:
    {
        ::rtl::OUString sTemp;
        rVal >>= sTemp;
        aExpansion = sTemp;
    }
    break;
    default:
        OSL_FAIL("illegal property");
    }
    if( nPart )
    {
        String sTmp, sCmd( GetCmd() );
        while(3 > comphelper::string::getTokenCount(sCmd, sfx2::cTokenSeperator))
            sCmd += sfx2::cTokenSeperator;
        sCmd.SetToken( nPart-1, sfx2::cTokenSeperator, ::GetString( rVal, sTmp ) );
        SetCmd( sCmd );
    }
    return true;
}

SwDDEField::SwDDEField( SwDDEFieldType* pInitType )
    : SwField(pInitType)
{
}

SwDDEField::~SwDDEField()
{
    if( GetTyp()->IsLastDepend() )
        ((SwDDEFieldType*)GetTyp())->Disconnect();
}

String SwDDEField::Expand() const
{
    xub_StrLen nPos;
    String aStr(comphelper::string::remove(((SwDDEFieldType*)GetTyp())->GetExpansion(), '\r'));

    while( (nPos = aStr.Search( '\t' )) != STRING_NOTFOUND )
        aStr.SetChar( nPos, ' ' );
    while( (nPos = aStr.Search( '\n' )) != STRING_NOTFOUND )
        aStr.SetChar( nPos, '|' );
    if( aStr.Len() && ( aStr.GetChar( aStr.Len()-1 ) == '|') )
        aStr.Erase( aStr.Len()-1, 1 );
    return aStr;
}

SwField* SwDDEField::Copy() const
{
    return new SwDDEField((SwDDEFieldType*)GetTyp());
}

/*--------------------------------------------------------------------
    Beschreibung: Parameter des Typen erfragen
                  Name
 --------------------------------------------------------------------*/
const rtl::OUString& SwDDEField::GetPar1() const
{
    return ((const SwDDEFieldType*)GetTyp())->GetName();
}

/*--------------------------------------------------------------------
    Beschreibung: Parameter des Typen erfragen
                  Commando
 --------------------------------------------------------------------*/
rtl::OUString SwDDEField::GetPar2() const
{
    return ((const SwDDEFieldType*)GetTyp())->GetCmd();
}

void SwDDEField::SetPar2(const rtl::OUString& rStr)
{
    ((SwDDEFieldType*)GetTyp())->SetCmd(rStr);
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
