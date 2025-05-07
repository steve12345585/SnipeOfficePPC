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

#include <svl/intitem.hxx>
#include <editeng/editeng.hxx>
#include <editeng/editview.hxx>
#include <editeng/editdata.hxx>
#include <editeng/eerdll.hxx>
#include <editeng/lrspitem.hxx>
#include <editeng/fhgtitem.hxx>

#define _OUTLINER_CXX
#include <editeng/outliner.hxx>
#include <outleeng.hxx>
#include <paralist.hxx>
#include <editeng/editrids.hrc>
#include <svl/itemset.hxx>
#include <editeng/eeitem.hxx>
#include <editeng/editstat.hxx>
#include "outlundo.hxx"

OutlinerEditEng::OutlinerEditEng( Outliner* pEngOwner, SfxItemPool* pPool )
 : EditEngine( pPool )
{
    pOwner = pEngOwner;
}

OutlinerEditEng::~OutlinerEditEng()
{
}

void OutlinerEditEng::PaintingFirstLine( sal_uInt16 nPara, const Point& rStartPos, long nBaseLineY, const Point& rOrigin, short nOrientation, OutputDevice* pOutDev )
{
    if( GetControlWord() & EE_CNTRL_OUTLINER )
    {
        PaintFirstLineInfo aInfo( nPara, rStartPos, nBaseLineY, rOrigin, nOrientation, pOutDev );
        pOwner->maPaintFirstLineHdl.Call( &aInfo );
    }

    pOwner->PaintBullet( nPara, rStartPos, rOrigin, nOrientation, pOutDev );
}

const SvxNumberFormat* OutlinerEditEng::GetNumberFormat( sal_uInt16 nPara ) const
{
    const SvxNumberFormat* pFmt = NULL;
    if (pOwner)
        pFmt = pOwner->GetNumberFormat( nPara );
    return pFmt;
}


Rectangle OutlinerEditEng::GetBulletArea( sal_uInt16 nPara )
{
    Rectangle aBulletArea = Rectangle( Point(), Point() );
    if ( nPara < pOwner->pParaList->GetParagraphCount() )
    {
        if ( pOwner->ImplHasBullet( nPara ) )
            aBulletArea = pOwner->ImpCalcBulletArea( nPara, sal_False, sal_False );
    }
    return aBulletArea;
}

void OutlinerEditEng::ParagraphInserted( sal_uInt16 nNewParagraph )
{
    pOwner->ParagraphInserted( nNewParagraph );

    EditEngine::ParagraphInserted( nNewParagraph );
}

void OutlinerEditEng::ParagraphDeleted( sal_uInt16 nDeletedParagraph )
{
    pOwner->ParagraphDeleted( nDeletedParagraph );

    EditEngine::ParagraphDeleted( nDeletedParagraph );
}

void OutlinerEditEng::ParagraphConnected( sal_uInt16 /*nLeftParagraph*/, sal_uInt16 nRightParagraph )
{
    if( pOwner && pOwner->IsUndoEnabled() && !const_cast<EditEngine&>(pOwner->GetEditEngine()).IsInUndo() )
    {
        Paragraph* pPara = pOwner->GetParagraph( nRightParagraph );
        if( pPara && pOwner->HasParaFlag( pPara, PARAFLAG_ISPAGE ) )
        {
            pOwner->InsertUndo( new OutlinerUndoChangeParaFlags( pOwner, nRightParagraph, PARAFLAG_ISPAGE, 0 ) );
        }
    }
}


void OutlinerEditEng::StyleSheetChanged( SfxStyleSheet* pStyle )
{
    pOwner->StyleSheetChanged( pStyle );
}

void OutlinerEditEng::ParaAttribsChanged( sal_uInt16 nPara )
{
    pOwner->ParaAttribsChanged( nPara );
}

sal_Bool OutlinerEditEng::SpellNextDocument()
{
    return pOwner->SpellNextDocument();
}

sal_Bool OutlinerEditEng::ConvertNextDocument()
{
    return pOwner->ConvertNextDocument();
}

XubString OutlinerEditEng::GetUndoComment( sal_uInt16 nUndoId ) const
{
    switch( nUndoId )
    {
        case OLUNDO_DEPTH:
            return EE_RESSTR(RID_OUTLUNDO_DEPTH);

        case OLUNDO_EXPAND:
            return EE_RESSTR(RID_OUTLUNDO_EXPAND);

        case OLUNDO_COLLAPSE:
            return EE_RESSTR(RID_OUTLUNDO_COLLAPSE);

        case OLUNDO_ATTR:
            return EE_RESSTR(RID_OUTLUNDO_ATTR);

        case OLUNDO_INSERT:
            return EE_RESSTR(RID_OUTLUNDO_INSERT);

        default:
            return EditEngine::GetUndoComment( nUndoId );
    }
}

void OutlinerEditEng::DrawingText( const Point& rStartPos, const XubString& rText, sal_uInt16 nTextStart, sal_uInt16 nTextLen,
    const sal_Int32* pDXArray, const SvxFont& rFont, sal_uInt16 nPara, sal_uInt16 nIndex, sal_uInt8 nRightToLeft,
    const EEngineData::WrongSpellVector* pWrongSpellVector,
    const SvxFieldData* pFieldData,
    bool bEndOfLine,
    bool bEndOfParagraph,
    bool bEndOfBullet,
    const ::com::sun::star::lang::Locale* pLocale,
    const Color& rOverlineColor,
    const Color& rTextLineColor)
{
    pOwner->DrawingText(rStartPos,rText,nTextStart,nTextLen,pDXArray,rFont,nPara,nIndex,nRightToLeft,
        pWrongSpellVector, pFieldData, bEndOfLine, bEndOfParagraph, bEndOfBullet, pLocale, rOverlineColor, rTextLineColor);
}

void OutlinerEditEng::DrawingTab( const Point& rStartPos, long nWidth, const String& rChar,
    const SvxFont& rFont, sal_uInt16 nPara, xub_StrLen nIndex, sal_uInt8 nRightToLeft,
    bool bEndOfLine, bool bEndOfParagraph,
    const Color& rOverlineColor, const Color& rTextLineColor)
{
    pOwner->DrawingTab(rStartPos, nWidth, rChar, rFont, nPara, nIndex, nRightToLeft,
            bEndOfLine, bEndOfParagraph, rOverlineColor, rTextLineColor );
}

void OutlinerEditEng::FieldClicked( const SvxFieldItem& rField, sal_uInt16 nPara, sal_uInt16 nPos )
{
    EditEngine::FieldClicked( rField, nPara, nPos );    // If URL
    pOwner->FieldClicked( rField, nPara, nPos );
}

void OutlinerEditEng::FieldSelected( const SvxFieldItem& rField, sal_uInt16 nPara, sal_uInt16 nPos )
{
    pOwner->FieldSelected( rField, nPara, nPos );
}

XubString OutlinerEditEng::CalcFieldValue( const SvxFieldItem& rField, sal_uInt16 nPara, sal_uInt16 nPos, Color*& rpTxtColor, Color*& rpFldColor )
{
    return pOwner->CalcFieldValue( rField, nPara, nPos, rpTxtColor, rpFldColor );
}

void OutlinerEditEng::SetParaAttribs( sal_uInt16 nPara, const SfxItemSet& rSet )
{
    Paragraph* pPara = pOwner->pParaList->GetParagraph( nPara );
    if( pPara )
    {
        if ( !IsInUndo() && IsUndoEnabled() )
            pOwner->UndoActionStart( OLUNDO_ATTR );

        EditEngine::SetParaAttribs( (sal_uInt16)nPara, rSet );

        pOwner->ImplCheckNumBulletItem( (sal_uInt16)nPara );
        // #i100014#
        // It is not a good idea to substract 1 from a count and cast the result
        // to USHORT without check, if the count is 0.
        pOwner->ImplCheckParagraphs( (sal_uInt16)nPara, (sal_uInt16) (pOwner->pParaList->GetParagraphCount()) );

        if ( !IsInUndo() && IsUndoEnabled() )
            pOwner->UndoActionEnd( OLUNDO_ATTR );
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
