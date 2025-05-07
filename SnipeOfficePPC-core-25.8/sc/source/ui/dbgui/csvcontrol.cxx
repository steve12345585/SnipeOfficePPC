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



// ============================================================================
#include "csvcontrol.hxx"
#include <vcl/svapp.hxx>
#include "AccessibleCsvControl.hxx"


// ============================================================================

ScCsvLayoutData::ScCsvLayoutData() :
    mnPosCount( 1 ),
    mnPosOffset( 0 ),
    mnWinWidth( 1 ),
    mnHdrWidth( 0 ),
    mnCharWidth( 1 ),
    mnLineCount( 1 ),
    mnLineOffset( 0 ),
    mnWinHeight( 1 ),
    mnHdrHeight( 0 ),
    mnLineHeight( 1 ),
    mnPosCursor( CSV_POS_INVALID ),
    mnColCursor( 0 ),
    mnNoRepaint( 0 ),
    mbAppRTL( !!Application::GetSettings().GetLayoutRTL() )
{
}

ScCsvDiff ScCsvLayoutData::GetDiff( const ScCsvLayoutData& rData ) const
{
    ScCsvDiff nRet = CSV_DIFF_EQUAL;
    if( mnPosCount != rData.mnPosCount )        nRet |= CSV_DIFF_POSCOUNT;
    if( mnPosOffset != rData.mnPosOffset )      nRet |= CSV_DIFF_POSOFFSET;
    if( mnHdrWidth != rData.mnHdrWidth )        nRet |= CSV_DIFF_HDRWIDTH;
    if( mnCharWidth != rData.mnCharWidth )      nRet |= CSV_DIFF_CHARWIDTH;
    if( mnLineCount != rData.mnLineCount )      nRet |= CSV_DIFF_LINECOUNT;
    if( mnLineOffset != rData.mnLineOffset )    nRet |= CSV_DIFF_LINEOFFSET;
    if( mnHdrHeight != rData.mnHdrHeight )      nRet |= CSV_DIFF_HDRHEIGHT;
    if( mnLineHeight != rData.mnLineHeight )    nRet |= CSV_DIFF_LINEHEIGHT;
    if( mnPosCursor != rData.mnPosCursor )      nRet |= CSV_DIFF_RULERCURSOR;
    if( mnColCursor != rData.mnColCursor )      nRet |= CSV_DIFF_GRIDCURSOR;
    return nRet;
}


// ============================================================================

ScCsvControl::ScCsvControl( ScCsvControl& rParent ) :
    Control( &rParent, WB_TABSTOP | WB_NODIALOGCONTROL ),
    mrData( rParent.GetLayoutData() ),
    mpAccessible( NULL ),
    mbValidGfx( false )
{
}

ScCsvControl::ScCsvControl( Window* pParent, const ScCsvLayoutData& rData, WinBits nStyle ) :
    Control( pParent, nStyle ),
    mrData( rData ),
    mpAccessible( NULL ),
    mbValidGfx( false )
{
}

ScCsvControl::ScCsvControl( Window* pParent, const ScCsvLayoutData& rData, const ResId& rResId ) :
    Control( pParent, rResId ),
    mrData( rData ),
    mpAccessible( NULL ),
    mbValidGfx( false )
{
}

ScCsvControl::~ScCsvControl()
{
    if( mpAccessible )
        mpAccessible->dispose();
}


// event handling -------------------------------------------------------------

void ScCsvControl::GetFocus()
{
    Control::GetFocus();
    AccSendFocusEvent( true );
}

void ScCsvControl::LoseFocus()
{
    Control::LoseFocus();
    AccSendFocusEvent( false );
}

void ScCsvControl::AccSendFocusEvent( bool bFocused )
{
    if( mpAccessible )
        mpAccessible->SendFocusEvent( bFocused );
}

void ScCsvControl::AccSendCaretEvent()
{
    if( mpAccessible )
        mpAccessible->SendCaretEvent();
}

void ScCsvControl::AccSendVisibleEvent()
{
    if( mpAccessible )
        mpAccessible->SendVisibleEvent();
}

void ScCsvControl::AccSendSelectionEvent()
{
    if( mpAccessible )
        mpAccessible->SendSelectionEvent();
}

void ScCsvControl::AccSendTableUpdateEvent( sal_uInt32 nFirstColumn, sal_uInt32 nLastColumn, bool bAllRows )
{
    if( mpAccessible )
        mpAccessible->SendTableUpdateEvent( nFirstColumn, nLastColumn, bAllRows );
}

void ScCsvControl::AccSendInsertColumnEvent( sal_uInt32 nFirstColumn, sal_uInt32 nLastColumn )
{
    if( mpAccessible )
        mpAccessible->SendInsertColumnEvent( nFirstColumn, nLastColumn );
}

void ScCsvControl::AccSendRemoveColumnEvent( sal_uInt32 nFirstColumn, sal_uInt32 nLastColumn )
{
    if( mpAccessible )
        mpAccessible->SendRemoveColumnEvent( nFirstColumn, nLastColumn );
}


// repaint helpers ------------------------------------------------------------

void ScCsvControl::Repaint( bool bInvalidate )
{
    if( bInvalidate )
        InvalidateGfx();
    if( !IsNoRepaint() )
        Execute( CSVCMD_REPAINT );
}

void ScCsvControl::DisableRepaint()
{
    ++mrData.mnNoRepaint;
}

void ScCsvControl::EnableRepaint( bool bInvalidate )
{
    OSL_ENSURE( IsNoRepaint(), "ScCsvControl::EnableRepaint - invalid call" );
    --mrData.mnNoRepaint;
    Repaint( bInvalidate );
}


// command handling -----------------------------------------------------------

void ScCsvControl::Execute( ScCsvCmdType eType, sal_Int32 nParam1, sal_Int32 nParam2 )
{
    maCmd.Set( eType, nParam1, nParam2 );
    maCmdHdl.Call( this );
}


// layout helpers -------------------------------------------------------------

sal_Int32 ScCsvControl::GetVisPosCount() const
{
    return (mrData.mnWinWidth - GetHdrWidth()) / GetCharWidth();
}

sal_Int32 ScCsvControl::GetMaxPosOffset() const
{
    return Max( GetPosCount() - GetVisPosCount() + 2L, 0L );
}

bool ScCsvControl::IsValidSplitPos( sal_Int32 nPos ) const
{
    return (0 < nPos) && (nPos < GetPosCount() );
}

bool ScCsvControl::IsVisibleSplitPos( sal_Int32 nPos ) const
{
    return IsValidSplitPos( nPos ) && (GetFirstVisPos() <= nPos) && (nPos <= GetLastVisPos());
}

sal_Int32 ScCsvControl::GetHdrX() const
{
    return IsRTL() ? (mrData.mnWinWidth - GetHdrWidth()) : 0;
}

sal_Int32 ScCsvControl::GetFirstX() const
{
    return IsRTL() ? 0 : GetHdrWidth();
}

sal_Int32 ScCsvControl::GetLastX() const
{
    return mrData.mnWinWidth - (IsRTL() ? GetHdrWidth() : 0) - 1;
}

sal_Int32 ScCsvControl::GetX( sal_Int32 nPos ) const
{
    return GetFirstX() + (nPos - GetFirstVisPos()) * GetCharWidth();
}

sal_Int32 ScCsvControl::GetPosFromX( sal_Int32 nX ) const
{
    return (nX - GetFirstX() + GetCharWidth() / 2) / GetCharWidth() + GetFirstVisPos();
}

sal_Int32 ScCsvControl::GetVisLineCount() const
{
    return (mrData.mnWinHeight - GetHdrHeight() - 2) / GetLineHeight() + 1;
}

sal_Int32 ScCsvControl::GetLastVisLine() const
{
    return Min( GetFirstVisLine() + GetVisLineCount(), GetLineCount() ) - 1;
}

sal_Int32 ScCsvControl::GetMaxLineOffset() const
{
    return Max( GetLineCount() - GetVisLineCount() + 1L, 0L );
}

bool ScCsvControl::IsValidLine( sal_Int32 nLine ) const
{
    return (0 <= nLine) && (nLine < GetLineCount());
}

bool ScCsvControl::IsVisibleLine( sal_Int32 nLine ) const
{
    return IsValidLine( nLine ) && (GetFirstVisLine() <= nLine) && (nLine <= GetLastVisLine());
}

sal_Int32 ScCsvControl::GetY( sal_Int32 nLine ) const
{
    return GetHdrHeight() + (nLine - GetFirstVisLine()) * GetLineHeight();
}

sal_Int32 ScCsvControl::GetLineFromY( sal_Int32 nY ) const
{
    return (nY - GetHdrHeight()) / GetLineHeight() + GetFirstVisLine();
}


// static helpers -------------------------------------------------------------

void ScCsvControl::ImplInvertRect( OutputDevice& rOutDev, const Rectangle& rRect )
{
    rOutDev.Push( PUSH_LINECOLOR | PUSH_FILLCOLOR | PUSH_RASTEROP );
    rOutDev.SetLineColor( Color( COL_BLACK ) );
    rOutDev.SetFillColor( Color( COL_BLACK ) );
    rOutDev.SetRasterOp( ROP_INVERT );
    rOutDev.DrawRect( rRect );
    rOutDev.Pop();
}

ScMoveMode ScCsvControl::GetHorzDirection( sal_uInt16 nCode, bool bHomeEnd )
{
    switch( nCode )
    {
        case KEY_LEFT:  return MOVE_PREV;
        case KEY_RIGHT: return MOVE_NEXT;
    }
    if( bHomeEnd ) switch( nCode )
    {
        case KEY_HOME:  return MOVE_FIRST;
        case KEY_END:   return MOVE_LAST;
    }
    return MOVE_NONE;
}

ScMoveMode ScCsvControl::GetVertDirection( sal_uInt16 nCode, bool bHomeEnd )
{
    switch( nCode )
    {
        case KEY_UP:        return MOVE_PREV;
        case KEY_DOWN:      return MOVE_NEXT;
        case KEY_PAGEUP:    return MOVE_PREVPAGE;
        case KEY_PAGEDOWN:  return MOVE_NEXTPAGE;
    }
    if( bHomeEnd ) switch( nCode )
    {
        case KEY_HOME:      return MOVE_FIRST;
        case KEY_END:       return MOVE_LAST;
    }
    return MOVE_NONE;
}


// accessibility --------------------------------------------------------------

ScCsvControl::XAccessibleRef ScCsvControl::CreateAccessible()
{
    mpAccessible = ImplCreateAccessible();
    mxAccessible = mpAccessible;
    return mxAccessible;
}


// ============================================================================

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
