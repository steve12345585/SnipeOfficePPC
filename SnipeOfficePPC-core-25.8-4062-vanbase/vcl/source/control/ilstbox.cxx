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


#include <tools/debug.hxx>

#include <vcl/svapp.hxx>
#include <vcl/settings.hxx>
#include <vcl/event.hxx>
#include <vcl/scrbar.hxx>
#include <vcl/help.hxx>
#include <vcl/lstbox.h>
#include <vcl/unohelp.hxx>
#include <vcl/i18nhelp.hxx>

#include <ilstbox.hxx>
#include <controldata.hxx>
#include <svdata.hxx>

#include <com/sun/star/i18n/XCollator.hpp>
#include <com/sun/star/accessibility/XAccessible.hpp>
#include <com/sun/star/accessibility/AccessibleRole.hpp>

#include <rtl/instance.hxx>
#include <comphelper/string.hxx>
#include <comphelper/processfactory.hxx>

#define MULTILINE_ENTRY_DRAW_FLAGS ( TEXT_DRAW_WORDBREAK | TEXT_DRAW_MULTILINE | TEXT_DRAW_VCENTER )

using namespace ::com::sun::star;

// =======================================================================

void ImplInitFieldSettings( Window* pWin, sal_Bool bFont, sal_Bool bForeground, sal_Bool bBackground )
{
    const StyleSettings& rStyleSettings = pWin->GetSettings().GetStyleSettings();

    if ( bFont )
    {
        Font aFont = rStyleSettings.GetFieldFont();
        if ( pWin->IsControlFont() )
            aFont.Merge( pWin->GetControlFont() );
        pWin->SetZoomedPointFont( aFont );
    }

    if ( bFont || bForeground )
    {
        Color aTextColor = rStyleSettings.GetFieldTextColor();
        if ( pWin->IsControlForeground() )
            aTextColor = pWin->GetControlForeground();
        pWin->SetTextColor( aTextColor );
    }

    if ( bBackground )
    {
        if( pWin->IsControlBackground() )
            pWin->SetBackground( pWin->GetControlBackground() );
        else
            pWin->SetBackground( rStyleSettings.GetFieldColor() );
    }
}

// -----------------------------------------------------------------------

void ImplInitDropDownButton( PushButton* pButton )
{
    if ( pButton->GetSettings().GetStyleSettings().GetOptions() & STYLE_OPTION_SPINUPDOWN )
        pButton->SetSymbol( SYMBOL_SPIN_UPDOWN );
    else
        pButton->SetSymbol( SYMBOL_SPIN_DOWN );

    if ( pButton->IsNativeControlSupported(CTRL_LISTBOX, PART_ENTIRE_CONTROL)
            && ! pButton->IsNativeControlSupported(CTRL_LISTBOX, PART_BUTTON_DOWN) )
        pButton->SetBackground();
}

// =======================================================================

ImplEntryList::ImplEntryList( Window* pWindow )
{
    mpWindow = pWindow;
    mnLastSelected = LISTBOX_ENTRY_NOTFOUND;
    mnSelectionAnchor = LISTBOX_ENTRY_NOTFOUND;
    mnImages = 0;
    mbCallSelectionChangedHdl = sal_True;

    mnMRUCount = 0;
    mnMaxMRUCount = 0;
}

// -----------------------------------------------------------------------

ImplEntryList::~ImplEntryList()
{
    Clear();
}

// -----------------------------------------------------------------------

void ImplEntryList::Clear()
{
    mnImages = 0;
    maEntries.clear();
}

// -----------------------------------------------------------------------

void ImplEntryList::SelectEntry( sal_uInt16 nPos, sal_Bool bSelect )
{
    if (nPos < maEntries.size())
    {
        boost::ptr_vector<ImplEntryType>::iterator iter = maEntries.begin()+nPos;

        if ( ( iter->mbIsSelected != bSelect ) &&
           ( (iter->mnFlags & LISTBOX_ENTRY_FLAG_DISABLE_SELECTION) == 0  ) )
        {
            iter->mbIsSelected = bSelect;
            if ( mbCallSelectionChangedHdl )
                maSelectionChangedHdl.Call( (void*)sal_IntPtr(nPos) );
        }
    }
}

namespace
{
    struct theSorter
        : public rtl::StaticWithInit< comphelper::string::NaturalStringSorter, theSorter >
    {
        comphelper::string::NaturalStringSorter operator () ()
        {
            return comphelper::string::NaturalStringSorter(
                ::comphelper::getProcessComponentContext(),
                Application::GetSettings().GetLanguageTag().getLocale());
        }
    };
}

namespace vcl
{
    namespace unohelper
    {
        const comphelper::string::NaturalStringSorter& getNaturalStringSorterForAppLocale()
        {
            return theSorter::get();
        }
    }
}

sal_uInt16 ImplEntryList::InsertEntry( sal_uInt16 nPos, ImplEntryType* pNewEntry, sal_Bool bSort )
{
    if ( !!pNewEntry->maImage )
        mnImages++;

    sal_uInt16 insPos = 0;

    if ( !bSort || maEntries.empty())
    {
        if (nPos < maEntries.size())
        {
            insPos = nPos;
            maEntries.insert( maEntries.begin() + nPos, pNewEntry );
        }
        else
        {
            insPos = maEntries.size();
            maEntries.push_back(pNewEntry);
        }
    }
    else
    {
        const comphelper::string::NaturalStringSorter &rSorter = theSorter::get();

        const XubString& rStr = pNewEntry->maStr;
        sal_uLong nLow, nHigh, nMid;

        nHigh = maEntries.size();

        ImplEntryType* pTemp = GetEntry( (sal_uInt16)(nHigh-1) );

        try
        {
            // XXX even though XCollator::compareString returns a sal_Int32 the only
            // defined values are {-1, 0, 1} which is compatible with StringCompare
            StringCompare eComp = (StringCompare)rSorter.compare(rStr, pTemp->maStr);

            // Schnelles Einfuegen bei sortierten Daten
            if ( eComp != COMPARE_LESS )
            {
                insPos = maEntries.size();
                maEntries.push_back(pNewEntry);
            }
            else
            {
                nLow  = mnMRUCount;
                pTemp = (ImplEntryType*)GetEntry( (sal_uInt16)nLow );

                eComp = (StringCompare)rSorter.compare(rStr, pTemp->maStr);
                if ( eComp != COMPARE_GREATER )
                {
                    insPos = 0;
                    maEntries.insert(maEntries.begin(),pNewEntry);
                }
                else
                {
                    // Binaeres Suchen
                    nHigh--;
                    do
                    {
                        nMid = (nLow + nHigh) / 2;
                        pTemp = (ImplEntryType*)GetEntry( nMid );

                        eComp = (StringCompare)rSorter.compare(rStr, pTemp->maStr);

                        if ( eComp == COMPARE_LESS )
                            nHigh = nMid-1;
                        else
                        {
                            if ( eComp == COMPARE_GREATER )
                                nLow = nMid + 1;
                            else
                                break;
                        }
                    }
                    while ( nLow <= nHigh );

                    if ( eComp != COMPARE_LESS )
                        nMid++;

                    insPos = nMid;
                    maEntries.insert(maEntries.begin()+nMid,pNewEntry);
                }
            }
        }
        catch (uno::RuntimeException& )
        {
            // XXX this is arguable, if the exception occurred because pNewEntry is
            // garbage you wouldn't insert it. If the exception occurred because the
            // Collator implementation is garbage then give the user a chance to see
            // his stuff
            insPos = 0;
            maEntries.insert(maEntries.begin(),pNewEntry);
        }

    }

    return insPos;
}

// -----------------------------------------------------------------------

void ImplEntryList::RemoveEntry( sal_uInt16 nPos )
{
    if (nPos < maEntries.size())
    {
        boost::ptr_vector<ImplEntryType>::iterator iter = maEntries.begin()+ nPos;

        if ( !!iter->maImage )
            mnImages--;

        maEntries.erase(iter);
    }
}

// -----------------------------------------------------------------------

sal_uInt16 ImplEntryList::FindEntry( const XubString& rString, sal_Bool bSearchMRUArea ) const
{
    sal_uInt16 nEntries = maEntries.size();
    for ( sal_uInt16 n = bSearchMRUArea ? 0 : GetMRUCount(); n < nEntries; n++ )
    {
        String aComp( vcl::I18nHelper::filterFormattingChars( maEntries[n].maStr ) );
        if ( aComp == rString )
            return n;
    }
    return LISTBOX_ENTRY_NOTFOUND;
}

    // -----------------------------------------------------------------------

sal_uInt16 ImplEntryList::FindMatchingEntry( const XubString& rStr, sal_uInt16 nStart, sal_Bool bForward, sal_Bool bLazy ) const
{
    sal_uInt16  nPos = LISTBOX_ENTRY_NOTFOUND;
    sal_uInt16  nEntryCount = GetEntryCount();
    if ( !bForward )
        nStart++;   // wird sofort dekrementiert

    const vcl::I18nHelper& rI18nHelper = mpWindow->GetSettings().GetLocaleI18nHelper();
    for ( sal_uInt16 n = nStart; bForward ? ( n < nEntryCount ) : n; )
    {
        if ( !bForward )
            n--;

        ImplEntryType* pImplEntry = GetEntry( n );
        sal_Bool bMatch = bLazy ? rI18nHelper.MatchString( rStr, pImplEntry->maStr ) != 0 : ( rStr.Match( pImplEntry->maStr ) == STRING_MATCH );
        if ( bMatch )
        {
            nPos = n;
            break;
        }

        if ( bForward )
            n++;
    }

    return nPos;
}

// -----------------------------------------------------------------------

sal_uInt16 ImplEntryList::FindEntry( const void* pData ) const
{
    sal_uInt16 nPos = LISTBOX_ENTRY_NOTFOUND;
    for ( sal_uInt16 n = GetEntryCount(); n; )
    {
        ImplEntryType* pImplEntry = GetEntry( --n );
        if ( pImplEntry->mpUserData == pData )
        {
            nPos = n;
            break;
        }
    }
    return nPos;
}

// -----------------------------------------------------------------------

long ImplEntryList::GetAddedHeight( sal_uInt16 i_nEndIndex, sal_uInt16 i_nBeginIndex, long i_nBeginHeight ) const
{
    long nHeight = i_nBeginHeight;
    sal_uInt16 nStart = i_nEndIndex > i_nBeginIndex ? i_nBeginIndex : i_nEndIndex;
    sal_uInt16 nStop  = i_nEndIndex > i_nBeginIndex ? i_nEndIndex : i_nBeginIndex;
    sal_uInt16 nEntryCount = GetEntryCount();
    if( nStop != LISTBOX_ENTRY_NOTFOUND && nEntryCount != 0 )
    {
        // sanity check
        if( nStop > nEntryCount-1 )
            nStop = nEntryCount-1;
        if( nStart > nEntryCount-1 )
            nStart = nEntryCount-1;

        sal_uInt16 nIndex = nStart;
        while( nIndex != LISTBOX_ENTRY_NOTFOUND && nIndex < nStop )
        {
            nHeight += GetEntryPtr( nIndex )-> mnHeight;
            nIndex++;
        }
    }
    else
        nHeight = 0;
    return i_nEndIndex > i_nBeginIndex ? nHeight : -nHeight;
}

// -----------------------------------------------------------------------

long ImplEntryList::GetEntryHeight( sal_uInt16 nPos ) const
{
    ImplEntryType* pImplEntry = GetEntry( nPos );
    return pImplEntry ? pImplEntry->mnHeight : 0;
}

// -----------------------------------------------------------------------

XubString ImplEntryList::GetEntryText( sal_uInt16 nPos ) const
{
    XubString aEntryText;
    ImplEntryType* pImplEntry = GetEntry( nPos );
    if ( pImplEntry )
        aEntryText = pImplEntry->maStr;
    return aEntryText;
}

// -----------------------------------------------------------------------

sal_Bool ImplEntryList::HasEntryImage( sal_uInt16 nPos ) const
{
    sal_Bool bImage = sal_False;
    ImplEntryType* pImplEntry = GetEntry( nPos );
    if ( pImplEntry )
        bImage = !!pImplEntry->maImage;
    return bImage;
}

// -----------------------------------------------------------------------

Image ImplEntryList::GetEntryImage( sal_uInt16 nPos ) const
{
    Image aImage;
    ImplEntryType* pImplEntry = GetEntry( nPos );
    if ( pImplEntry )
        aImage = pImplEntry->maImage;
    return aImage;
}

// -----------------------------------------------------------------------

void ImplEntryList::SetEntryData( sal_uInt16 nPos, void* pNewData )
{
    ImplEntryType* pImplEntry = GetEntry( nPos );
    if ( pImplEntry )
        pImplEntry->mpUserData = pNewData;
}

// -----------------------------------------------------------------------

void* ImplEntryList::GetEntryData( sal_uInt16 nPos ) const
{
    ImplEntryType* pImplEntry = GetEntry( nPos );
    return pImplEntry ? pImplEntry->mpUserData : NULL;
}

// -----------------------------------------------------------------------

void ImplEntryList::SetEntryFlags( sal_uInt16 nPos, long nFlags )
{
    ImplEntryType* pImplEntry = GetEntry( nPos );
    if ( pImplEntry )
        pImplEntry->mnFlags = nFlags;
}

// -----------------------------------------------------------------------

long ImplEntryList::GetEntryFlags( sal_uInt16 nPos ) const
{
    ImplEntryType* pImplEntry = GetEntry( nPos );
    return pImplEntry ? pImplEntry->mnFlags : 0;
}

// -----------------------------------------------------------------------

sal_uInt16 ImplEntryList::GetSelectEntryCount() const
{
    sal_uInt16 nSelCount = 0;
    for ( sal_uInt16 n = GetEntryCount(); n; )
    {
        ImplEntryType* pImplEntry = GetEntry( --n );
        if ( pImplEntry->mbIsSelected )
            nSelCount++;
    }
    return nSelCount;
}

// -----------------------------------------------------------------------

XubString ImplEntryList::GetSelectEntry( sal_uInt16 nIndex ) const
{
    return GetEntryText( GetSelectEntryPos( nIndex ) );
}

// -----------------------------------------------------------------------

sal_uInt16 ImplEntryList::GetSelectEntryPos( sal_uInt16 nIndex ) const
{
    sal_uInt16 nSelEntryPos = LISTBOX_ENTRY_NOTFOUND;
    sal_uInt16 nSel = 0;
    sal_uInt16 nEntryCount = GetEntryCount();

    for ( sal_uInt16 n = 0; n < nEntryCount; n++ )
    {
        ImplEntryType* pImplEntry = GetEntry( n );
        if ( pImplEntry->mbIsSelected )
        {
            if ( nSel == nIndex )
            {
                nSelEntryPos = n;
                break;
            }
            nSel++;
        }
    }

    return nSelEntryPos;
}

// -----------------------------------------------------------------------

sal_Bool ImplEntryList::IsEntryPosSelected( sal_uInt16 nIndex ) const
{
    ImplEntryType* pImplEntry = GetEntry( nIndex );
    return pImplEntry ? pImplEntry->mbIsSelected : sal_False;
}

// -----------------------------------------------------------------------

bool ImplEntryList::IsEntrySelectable( sal_uInt16 nPos ) const
{
    ImplEntryType* pImplEntry = GetEntry( nPos );
    return pImplEntry ? ((pImplEntry->mnFlags & LISTBOX_ENTRY_FLAG_DISABLE_SELECTION) == 0) : true;
}

// -----------------------------------------------------------------------

sal_uInt16 ImplEntryList::FindFirstSelectable( sal_uInt16 nPos, bool bForward /* = true */ )
{
    if( IsEntrySelectable( nPos ) )
        return nPos;

    if( bForward )
    {
        for( nPos = nPos + 1; nPos < GetEntryCount(); nPos++ )
        {
            if( IsEntrySelectable( nPos ) )
                return nPos;
        }
    }
    else
    {
        while( nPos )
        {
            nPos--;
            if( IsEntrySelectable( nPos ) )
                return nPos;
        }
    }

    return LISTBOX_ENTRY_NOTFOUND;
}

void ImplEntryList::take_properties(ImplEntryList &rOther)
{
    mnLastSelected = rOther.mnLastSelected;
    mnSelectionAnchor = rOther.mnSelectionAnchor;
    mnImages = rOther.mnImages;
    mnMRUCount = rOther.mnMRUCount;
    mnMaxMRUCount = rOther.mnMaxMRUCount;
    maEntries.swap(rOther.maEntries);
}

// =======================================================================

ImplListBoxWindow::ImplListBoxWindow( Window* pParent, WinBits nWinStyle ) :
    Control( pParent, 0 ),
    maQuickSelectionEngine( *this )
{
    mpEntryList         = new ImplEntryList( this );

    mnTop               = 0;
    mnLeft              = 0;
    mnBorder            = 1;
    mnSelectModifier    = 0;
    mnUserDrawEntry     = LISTBOX_ENTRY_NOTFOUND;
    mbTrack             = sal_False;
    mbImgsDiffSz        = sal_False;
    mbTravelSelect      = sal_False;
    mbTrackingSelect    = sal_False;
    mbSelectionChanged  = sal_False;
    mbMouseMoveSelect   = sal_False;
    mbMulti             = sal_False;
    mbStackMode         = sal_False;
    mbGrabFocus         = sal_False;
    mbUserDrawEnabled   = sal_False;
    mbInUserDraw        = sal_False;
    mbReadOnly          = sal_False;
    mbHasFocusRect      = sal_False;
    mbRight             = ( nWinStyle & WB_RIGHT )      ? sal_True : sal_False;
    mbCenter            = ( nWinStyle & WB_CENTER )     ? sal_True : sal_False;
    mbSimpleMode        = ( nWinStyle & WB_SIMPLEMODE ) ? sal_True : sal_False;
    mbSort              = ( nWinStyle & WB_SORT )       ? sal_True : sal_False;

    // pb: #106948# explicit mirroring for calc
    mbMirroring         = sal_False;

    mnCurrentPos            = LISTBOX_ENTRY_NOTFOUND;
    mnTrackingSaveSelection = LISTBOX_ENTRY_NOTFOUND;
    mnSeparatorPos          = LISTBOX_ENTRY_NOTFOUND;
    meProminentType         = PROMINENT_TOP;

    SetLineColor();
    SetTextFillColor();
    SetBackground( Wallpaper( GetSettings().GetStyleSettings().GetFieldColor() ) );

    ImplInitSettings( sal_True, sal_True, sal_True );
    ImplCalcMetrics();
}

void ImplListBoxWindow::take_properties(Window &rOther)
{
    Control::take_properties(rOther);

    ImplListBoxWindow &rOtherListBoxWindow = static_cast<ImplListBoxWindow&>(rOther);
    mpEntryList->take_properties(*rOtherListBoxWindow.mpEntryList);

    maFocusRect = rOtherListBoxWindow.maFocusRect;
    maUserItemSize = rOtherListBoxWindow.maUserItemSize;
    mnMaxTxtHeight = rOtherListBoxWindow.mnMaxTxtHeight;
    mnMaxTxtWidth = rOtherListBoxWindow.mnMaxTxtWidth;
    mnMaxImgTxtWidth = rOtherListBoxWindow.mnMaxImgTxtWidth;
    mnMaxImgWidth = rOtherListBoxWindow.mnMaxImgWidth;
    mnMaxImgHeight = rOtherListBoxWindow.mnMaxImgHeight;
    mnMaxWidth = rOtherListBoxWindow.mnMaxWidth;
    mnMaxHeight = rOtherListBoxWindow.mnMaxHeight;
    mnCurrentPos = rOtherListBoxWindow.mnCurrentPos;
    mnTrackingSaveSelection = rOtherListBoxWindow.mnTrackingSaveSelection;
    mnSeparatorPos = rOtherListBoxWindow.mnSeparatorPos;
    mnUserDrawEntry = rOtherListBoxWindow.mnUserDrawEntry;
    mnTop = rOtherListBoxWindow.mnTop;
    mnLeft = rOtherListBoxWindow.mnLeft;
    mnBorder = rOtherListBoxWindow.mnBorder;
    mnTextHeight = rOtherListBoxWindow.mnTextHeight;
    meProminentType = rOtherListBoxWindow.meProminentType;
    mnSelectModifier = rOtherListBoxWindow.mnSelectModifier;
    mbHasFocusRect = rOtherListBoxWindow.mbHasFocusRect;
    mbSort = rOtherListBoxWindow.mbSort;
    mbTrack = rOtherListBoxWindow.mbTrack;
    mbMulti = rOtherListBoxWindow.mbMulti;
    mbStackMode = rOtherListBoxWindow.mbStackMode;
    mbSimpleMode = rOtherListBoxWindow.mbSimpleMode;
    mbImgsDiffSz = rOtherListBoxWindow.mbImgsDiffSz;
    mbTravelSelect = rOtherListBoxWindow.mbTravelSelect;
    mbTrackingSelect = rOtherListBoxWindow.mbTrackingSelect;
    mbSelectionChanged = rOtherListBoxWindow.mbSelectionChanged;
    mbMouseMoveSelect = rOtherListBoxWindow.mbMouseMoveSelect;
    mbGrabFocus = rOtherListBoxWindow.mbGrabFocus;
    mbUserDrawEnabled = rOtherListBoxWindow.mbUserDrawEnabled;
    mbInUserDraw = rOtherListBoxWindow.mbInUserDraw;
    mbReadOnly = rOtherListBoxWindow.mbReadOnly;
    mbMirroring = rOtherListBoxWindow.mbMirroring;
    mbRight = rOtherListBoxWindow.mbRight;
    mbCenter = rOtherListBoxWindow.mbCenter;
}

// -----------------------------------------------------------------------

ImplListBoxWindow::~ImplListBoxWindow()
{
    delete mpEntryList;
}

// -----------------------------------------------------------------------

void ImplListBoxWindow::ImplInitSettings( sal_Bool bFont, sal_Bool bForeground, sal_Bool bBackground )
{
    ImplInitFieldSettings( this, bFont, bForeground, bBackground );
}

// -----------------------------------------------------------------------

void ImplListBoxWindow::ImplCalcMetrics()
{
    mnMaxWidth      = 0;
    mnMaxTxtWidth   = 0;
    mnMaxImgWidth   = 0;
    mnMaxImgTxtWidth= 0;
    mnMaxImgHeight  = 0;

    mnTextHeight = (sal_uInt16)GetTextHeight();
    mnMaxTxtHeight = mnTextHeight + mnBorder;
    mnMaxHeight = mnMaxTxtHeight;

    if ( maUserItemSize.Height() > mnMaxHeight )
        mnMaxHeight = (sal_uInt16) maUserItemSize.Height();
    if ( maUserItemSize.Width() > mnMaxWidth )
        mnMaxWidth= (sal_uInt16) maUserItemSize.Width();

    for ( sal_uInt16 n = mpEntryList->GetEntryCount(); n; )
    {
        ImplEntryType* pEntry = mpEntryList->GetMutableEntryPtr( --n );
        ImplUpdateEntryMetrics( *pEntry );
    }

    if( mnCurrentPos != LISTBOX_ENTRY_NOTFOUND )
    {
        Size aSz( GetOutputSizePixel().Width(), mpEntryList->GetEntryPtr( mnCurrentPos )->mnHeight );
        maFocusRect.SetSize( aSz );
    }
}

// -----------------------------------------------------------------------

void ImplListBoxWindow::Clear()
{
    mpEntryList->Clear();

    mnMaxHeight     = mnMaxTxtHeight;
    mnMaxWidth      = 0;
    mnMaxTxtWidth   = 0;
    mnMaxImgTxtWidth= 0;
    mnMaxImgWidth   = 0;
    mnMaxImgHeight  = 0;
    mnTop           = 0;
    mnLeft          = 0;
    mbImgsDiffSz    = sal_False;
    ImplClearLayoutData();

    mnCurrentPos = LISTBOX_ENTRY_NOTFOUND;
    maQuickSelectionEngine.Reset();

    Invalidate();
}

void ImplListBoxWindow::SetUserItemSize( const Size& rSz )
{
    ImplClearLayoutData();
    maUserItemSize = rSz;
    ImplCalcMetrics();
}

// -----------------------------------------------------------------------

struct ImplEntryMetrics
{
    sal_Bool    bText;
    sal_Bool    bImage;
    long    nEntryWidth;
    long    nEntryHeight;
    long    nTextWidth;
    long    nImgWidth;
    long    nImgHeight;
};

// -----------------------------------------------------------------------

void ImplListBoxWindow::ImplUpdateEntryMetrics( ImplEntryType& rEntry )
{
    ImplEntryMetrics aMetrics;
    aMetrics.bText = rEntry.maStr.Len() ? sal_True : sal_False;
    aMetrics.bImage = !!rEntry.maImage;
    aMetrics.nEntryWidth = 0;
    aMetrics.nEntryHeight = 0;
    aMetrics.nTextWidth = 0;
    aMetrics.nImgWidth = 0;
    aMetrics.nImgHeight = 0;

    if ( aMetrics.bText )
    {
        if( (rEntry.mnFlags & LISTBOX_ENTRY_FLAG_MULTILINE) )
        {
            // multiline case
            Size aCurSize( PixelToLogic( GetSizePixel() ) );
            // set the current size to a large number
            // GetTextRect should shrink it to the actual size
            aCurSize.Height() = 0x7fffff;
            Rectangle aTextRect( Point( 0, 0 ), aCurSize );
            aTextRect = GetTextRect( aTextRect, rEntry.maStr, TEXT_DRAW_WORDBREAK | TEXT_DRAW_MULTILINE );
            aMetrics.nTextWidth = aTextRect.GetWidth();
            if( aMetrics.nTextWidth > mnMaxTxtWidth )
                mnMaxTxtWidth = aMetrics.nTextWidth;
            aMetrics.nEntryWidth = mnMaxTxtWidth;
            aMetrics.nEntryHeight = aTextRect.GetHeight() + mnBorder;
        }
        else
        {
            // normal single line case
            aMetrics.nTextWidth = (sal_uInt16)GetTextWidth( rEntry.maStr );
            if( aMetrics.nTextWidth > mnMaxTxtWidth )
                mnMaxTxtWidth = aMetrics.nTextWidth;
            aMetrics.nEntryWidth = mnMaxTxtWidth;
            aMetrics.nEntryHeight = mnTextHeight + mnBorder;
        }
    }
    if ( aMetrics.bImage )
    {
        Size aImgSz = rEntry.maImage.GetSizePixel();
        aMetrics.nImgWidth  = (sal_uInt16) CalcZoom( aImgSz.Width() );
        aMetrics.nImgHeight = (sal_uInt16) CalcZoom( aImgSz.Height() );

        if( mnMaxImgWidth && ( aMetrics.nImgWidth != mnMaxImgWidth ) )
            mbImgsDiffSz = sal_True;
        else if ( mnMaxImgHeight && ( aMetrics.nImgHeight != mnMaxImgHeight ) )
            mbImgsDiffSz = sal_True;

        if( aMetrics.nImgWidth > mnMaxImgWidth )
            mnMaxImgWidth = aMetrics.nImgWidth;
        if( aMetrics.nImgHeight > mnMaxImgHeight )
            mnMaxImgHeight = aMetrics.nImgHeight;

        mnMaxImgTxtWidth = Max( mnMaxImgTxtWidth, aMetrics.nTextWidth );
        aMetrics.nEntryHeight = Max( aMetrics.nImgHeight, aMetrics.nEntryHeight );

    }
    if ( IsUserDrawEnabled() || aMetrics.bImage )
    {
        aMetrics.nEntryWidth = Max( aMetrics.nImgWidth, maUserItemSize.Width() );
        if ( aMetrics.bText )
            aMetrics.nEntryWidth += aMetrics.nTextWidth + IMG_TXT_DISTANCE;
        aMetrics.nEntryHeight = Max( Max( mnMaxImgHeight, maUserItemSize.Height() ) + 2,
                                     aMetrics.nEntryHeight );
    }

    if ( !aMetrics.bText && !aMetrics.bImage && !IsUserDrawEnabled() )
    {
        // entries which have no (aka an empty) text, and no image, and are not user-drawn, should be
        // shown nonetheless
        aMetrics.nEntryHeight = mnTextHeight + mnBorder;
    }

    if ( aMetrics.nEntryWidth > mnMaxWidth )
        mnMaxWidth = aMetrics.nEntryWidth;
    if ( aMetrics.nEntryHeight > mnMaxHeight )
        mnMaxHeight = aMetrics.nEntryHeight;

    rEntry.mnHeight = aMetrics.nEntryHeight;
}

// -----------------------------------------------------------------------

void ImplListBoxWindow::ImplCallSelect()
{
    if ( !IsTravelSelect() && GetEntryList()->GetMaxMRUCount() )
    {
        // Insert the selected entry as MRU, if not allready first MRU
        sal_uInt16 nSelected = GetEntryList()->GetSelectEntryPos( 0 );
        sal_uInt16 nMRUCount = GetEntryList()->GetMRUCount();
        String aSelected = GetEntryList()->GetEntryText( nSelected );
        sal_uInt16 nFirstMatchingEntryPos = GetEntryList()->FindEntry( aSelected, sal_True );
        if ( nFirstMatchingEntryPos || !nMRUCount )
        {
            sal_Bool bSelectNewEntry = sal_False;
            if ( nFirstMatchingEntryPos < nMRUCount )
            {
                RemoveEntry( nFirstMatchingEntryPos );
                nMRUCount--;
                if ( nFirstMatchingEntryPos == nSelected )
                    bSelectNewEntry = sal_True;
            }
            else if ( nMRUCount == GetEntryList()->GetMaxMRUCount() )
            {
                RemoveEntry( nMRUCount - 1 );
                nMRUCount--;
            }

            ImplClearLayoutData();

            ImplEntryType* pNewEntry = new ImplEntryType( aSelected );
            pNewEntry->mbIsSelected = bSelectNewEntry;
            GetEntryList()->InsertEntry( 0, pNewEntry, sal_False );
            ImplUpdateEntryMetrics( *pNewEntry );
            GetEntryList()->SetMRUCount( ++nMRUCount );
            SetSeparatorPos( nMRUCount ? nMRUCount-1 : 0 );
            maMRUChangedHdl.Call( NULL );
        }
    }

    maSelectHdl.Call( NULL );
    mbSelectionChanged = sal_False;
}

// -----------------------------------------------------------------------

sal_uInt16 ImplListBoxWindow::InsertEntry( sal_uInt16 nPos, ImplEntryType* pNewEntry )
{
    ImplClearLayoutData();
    sal_uInt16 nNewPos = mpEntryList->InsertEntry( nPos, pNewEntry, mbSort );

    if( (GetStyle() & WB_WORDBREAK) )
        pNewEntry->mnFlags |= LISTBOX_ENTRY_FLAG_MULTILINE;

    ImplUpdateEntryMetrics( *pNewEntry );
    return nNewPos;
}

// -----------------------------------------------------------------------

void ImplListBoxWindow::RemoveEntry( sal_uInt16 nPos )
{
    ImplClearLayoutData();
    mpEntryList->RemoveEntry( nPos );
    if( mnCurrentPos >= mpEntryList->GetEntryCount() )
        mnCurrentPos = LISTBOX_ENTRY_NOTFOUND;
    ImplCalcMetrics();
}

// -----------------------------------------------------------------------

void ImplListBoxWindow::SetEntryFlags( sal_uInt16 nPos, long nFlags )
{
    mpEntryList->SetEntryFlags( nPos, nFlags );
    ImplEntryType* pEntry = mpEntryList->GetMutableEntryPtr( nPos );
    if( pEntry )
        ImplUpdateEntryMetrics( *pEntry );
}

// -----------------------------------------------------------------------

void ImplListBoxWindow::ImplShowFocusRect()
{
    if ( mbHasFocusRect )
        HideFocus();
    ShowFocus( maFocusRect );
    mbHasFocusRect = sal_True;
}

// -----------------------------------------------------------------------

void ImplListBoxWindow::ImplHideFocusRect()
{
    if ( mbHasFocusRect )
    {
        HideFocus();
        mbHasFocusRect = sal_False;
    }
}


// -----------------------------------------------------------------------

sal_uInt16 ImplListBoxWindow::GetEntryPosForPoint( const Point& rPoint ) const
{
    long nY = mnBorder;

    sal_uInt16 nSelect = mnTop;
    const ImplEntryType* pEntry = mpEntryList->GetEntryPtr( nSelect );
    while( pEntry && rPoint.Y() > pEntry->mnHeight + nY )
    {
        nY += pEntry->mnHeight;
        pEntry = mpEntryList->GetEntryPtr( ++nSelect );
    }
    if( pEntry == NULL )
        nSelect = LISTBOX_ENTRY_NOTFOUND;

    return nSelect;
}

// -----------------------------------------------------------------------

sal_Bool ImplListBoxWindow::IsVisible( sal_uInt16 i_nEntry ) const
{
    sal_Bool bRet = sal_False;

    if( i_nEntry >= mnTop )
    {
        if( mpEntryList->GetAddedHeight( i_nEntry, mnTop ) <
            PixelToLogic( GetSizePixel() ).Height() )
        {
            bRet = sal_True;
        }
    }

    return bRet;
}

// -----------------------------------------------------------------------

sal_uInt16 ImplListBoxWindow::GetLastVisibleEntry() const
{
    sal_uInt16 nPos = mnTop;
    long nWindowHeight = GetSizePixel().Height();
    sal_uInt16 nCount = mpEntryList->GetEntryCount();
    long nDiff;
    for( nDiff = 0; nDiff < nWindowHeight && nPos < nCount; nDiff = mpEntryList->GetAddedHeight( nPos, mnTop ) )
        nPos++;

    if( nDiff > nWindowHeight && nPos > mnTop )
        nPos--;

    if( nPos >= nCount )
        nPos = nCount-1;

    return nPos;
}

// -----------------------------------------------------------------------

void ImplListBoxWindow::MouseButtonDown( const MouseEvent& rMEvt )
{
    mbMouseMoveSelect = sal_False;  // Nur bis zum ersten MouseButtonDown
    maQuickSelectionEngine.Reset();

    if ( !IsReadOnly() )
    {
        if( rMEvt.GetClicks() == 1 )
        {
            sal_uInt16 nSelect = GetEntryPosForPoint( rMEvt.GetPosPixel() );
            if( nSelect != LISTBOX_ENTRY_NOTFOUND )
            {
                if ( !mbMulti && GetEntryList()->GetSelectEntryCount() )
                    mnTrackingSaveSelection = GetEntryList()->GetSelectEntryPos( 0 );
                else
                    mnTrackingSaveSelection = LISTBOX_ENTRY_NOTFOUND;

                mnCurrentPos = nSelect;
                mbTrackingSelect = sal_True;
                SelectEntries( nSelect, LET_MBDOWN, rMEvt.IsShift(), rMEvt.IsMod1() );
                mbTrackingSelect = sal_False;
                if ( mbGrabFocus )
                    GrabFocus();

                StartTracking( STARTTRACK_SCROLLREPEAT );
            }
        }
        if( rMEvt.GetClicks() == 2 )
        {
            maDoubleClickHdl.Call( this );
        }
    }
    else // if ( mbGrabFocus )
    {
        GrabFocus();
    }
}

// -----------------------------------------------------------------------

void ImplListBoxWindow::MouseMove( const MouseEvent& rMEvt )
{
    if ( rMEvt.IsLeaveWindow() )
    {
        if ( mbStackMode && IsMouseMoveSelect() && IsReallyVisible() )
        {
            if ( rMEvt.GetPosPixel().Y() < 0 )
            {
                DeselectAll();
                mnCurrentPos = LISTBOX_ENTRY_NOTFOUND;
                SetTopEntry( 0 );
                if ( mbStackMode ) // #87072#, #92323#
                {
                    mbTravelSelect = sal_True;
                    mnSelectModifier = rMEvt.GetModifier();
                    ImplCallSelect();
                    mbTravelSelect = sal_False;
                }

            }
        }
    }
    else if ( ( ( !mbMulti && IsMouseMoveSelect() ) || mbStackMode ) && mpEntryList->GetEntryCount() )
    {
        Point aPoint;
        Rectangle aRect( aPoint, GetOutputSizePixel() );
        if( aRect.IsInside( rMEvt.GetPosPixel() ) )
        {
            if ( IsMouseMoveSelect() )
            {
                sal_uInt16 nSelect = GetEntryPosForPoint( rMEvt.GetPosPixel() );
                if( nSelect == LISTBOX_ENTRY_NOTFOUND )
                    nSelect = mpEntryList->GetEntryCount() - 1;
                nSelect = Min( nSelect, GetLastVisibleEntry() );
                nSelect = Min( nSelect, (sal_uInt16) ( mpEntryList->GetEntryCount() - 1 ) );
                // Select only visible Entries with MouseMove, otherwise Tracking...
                if ( IsVisible( nSelect ) &&
                    mpEntryList->IsEntrySelectable( nSelect ) &&
                    ( ( nSelect != mnCurrentPos ) || !GetEntryList()->GetSelectEntryCount() || ( nSelect != GetEntryList()->GetSelectEntryPos( 0 ) ) ) )
                {
                    mbTrackingSelect = sal_True;
                    if ( SelectEntries( nSelect, LET_TRACKING, sal_False, sal_False ) )
                    {
                        if ( mbStackMode ) // #87072#
                        {
                            mbTravelSelect = sal_True;
                            mnSelectModifier = rMEvt.GetModifier();
                            ImplCallSelect();
                            mbTravelSelect = sal_False;
                        }
                    }
                    mbTrackingSelect = sal_False;
                }
            }

            // Falls der DD-Button gedrueckt wurde und jemand mit gedrueckter
            // Maustaste in die ListBox faehrt...
            if ( rMEvt.IsLeft() && !rMEvt.IsSynthetic() )
            {
                if ( !mbMulti && GetEntryList()->GetSelectEntryCount() )
                    mnTrackingSaveSelection = GetEntryList()->GetSelectEntryPos( 0 );
                else
                    mnTrackingSaveSelection = LISTBOX_ENTRY_NOTFOUND;

                if ( mbStackMode && ( mpEntryList->GetSelectionAnchor() == LISTBOX_ENTRY_NOTFOUND ) )
                    mpEntryList->SetSelectionAnchor( 0 );

                StartTracking( STARTTRACK_SCROLLREPEAT );
            }
        }
    }
}

// -----------------------------------------------------------------------

void ImplListBoxWindow::DeselectAll()
{
    while ( GetEntryList()->GetSelectEntryCount() )
    {
        sal_uInt16 nS = GetEntryList()->GetSelectEntryPos( 0 );
        SelectEntry( nS, sal_False );
    }
}

// -----------------------------------------------------------------------

void ImplListBoxWindow::SelectEntry( sal_uInt16 nPos, sal_Bool bSelect )
{
    if( (mpEntryList->IsEntryPosSelected( nPos ) != bSelect) && mpEntryList->IsEntrySelectable( nPos ) )
    {
        ImplHideFocusRect();
        if( bSelect )
        {
            if( !mbMulti )
            {
                // Selektierten Eintrag deselektieren
                sal_uInt16 nDeselect = GetEntryList()->GetSelectEntryPos( 0 );
                if( nDeselect != LISTBOX_ENTRY_NOTFOUND )
                {
                    //SelectEntryPos( nDeselect, sal_False );
                    GetEntryList()->SelectEntry( nDeselect, sal_False );
                    if ( IsUpdateMode() && IsReallyVisible() )
                        ImplPaint( nDeselect, sal_True );
                }
            }
            mpEntryList->SelectEntry( nPos, sal_True );
            mnCurrentPos = nPos;
            if ( ( nPos != LISTBOX_ENTRY_NOTFOUND ) && IsUpdateMode() )
            {
                ImplPaint( nPos );
                if ( !IsVisible( nPos ) )
                {
                    ImplClearLayoutData();
                    sal_uInt16 nVisibleEntries = GetLastVisibleEntry()-mnTop;
                    if ( !nVisibleEntries || !IsReallyVisible() || ( nPos < GetTopEntry() ) )
                    {
                        Resize();
                        ShowProminentEntry( nPos );
                    }
                    else
                    {
                        ShowProminentEntry( nPos );
                    }
                }
            }
        }
        else
        {
            mpEntryList->SelectEntry( nPos, sal_False );
            ImplPaint( nPos, sal_True );
        }
        mbSelectionChanged = sal_True;
    }
}

// -----------------------------------------------------------------------

sal_Bool ImplListBoxWindow::SelectEntries( sal_uInt16 nSelect, LB_EVENT_TYPE eLET, sal_Bool bShift, sal_Bool bCtrl )
{
    sal_Bool bFocusChanged = sal_False;
    sal_Bool bSelectionChanged = sal_False;

    if( IsEnabled() && mpEntryList->IsEntrySelectable( nSelect ) )
    {
        // Hier (Single-ListBox) kann nur ein Eintrag deselektiert werden
        if( !mbMulti )
        {
            sal_uInt16 nDeselect = mpEntryList->GetSelectEntryPos( 0 );
            if( nSelect != nDeselect )
            {
                SelectEntry( nSelect, sal_True );
                mpEntryList->SetLastSelected( nSelect );
                bFocusChanged = sal_True;
                bSelectionChanged = sal_True;
            }
        }
        // MultiListBox ohne Modifier
        else if( mbSimpleMode && !bCtrl && !bShift )
        {
            sal_uInt16 nEntryCount = mpEntryList->GetEntryCount();
            for ( sal_uInt16 nPos = 0; nPos < nEntryCount; nPos++ )
            {
                sal_Bool bSelect = nPos == nSelect;
                if ( mpEntryList->IsEntryPosSelected( nPos ) != bSelect )
                {
                    SelectEntry( nPos, bSelect );
                    bFocusChanged = sal_True;
                    bSelectionChanged = sal_True;
                }
            }
            mpEntryList->SetLastSelected( nSelect );
            mpEntryList->SetSelectionAnchor( nSelect );
        }
        // MultiListBox nur mit CTRL/SHIFT oder nicht im SimpleMode
        else if( ( !mbSimpleMode /* && !bShift */ ) || ( (mbSimpleMode && ( bCtrl || bShift )) || mbStackMode ) )
        {
            // Space fuer Selektionswechsel
            if( !bShift && ( ( eLET == LET_KEYSPACE ) || ( eLET == LET_MBDOWN ) ) )
            {
                sal_Bool bSelect = ( mbStackMode && IsMouseMoveSelect() ) ? sal_True : !mpEntryList->IsEntryPosSelected( nSelect );
                if ( mbStackMode )
                {
                    sal_uInt16 n;
                    if ( bSelect )
                    {
                        // All entries before nSelect must be selected...
                        for ( n = 0; n < nSelect; n++ )
                            SelectEntry( n, sal_True );
                    }
                    if ( !bSelect )
                    {
                        for ( n = nSelect+1; n < mpEntryList->GetEntryCount(); n++ )
                            SelectEntry( n, sal_False );
                    }
                }
                SelectEntry( nSelect, bSelect );
                mpEntryList->SetLastSelected( nSelect );
                mpEntryList->SetSelectionAnchor( mbStackMode ? 0 : nSelect );
                if ( !mpEntryList->IsEntryPosSelected( nSelect ) )
                    mpEntryList->SetSelectionAnchor( LISTBOX_ENTRY_NOTFOUND );
                bFocusChanged = sal_True;
                bSelectionChanged = sal_True;
            }
            else if( ( ( eLET == LET_TRACKING ) && ( nSelect != mnCurrentPos ) ) ||
                     ( (bShift||mbStackMode) && ( ( eLET == LET_KEYMOVE ) || ( eLET == LET_MBDOWN ) ) ) )
            {
                mnCurrentPos = nSelect;
                bFocusChanged = sal_True;

                sal_uInt16 nAnchor = mpEntryList->GetSelectionAnchor();
                if( ( nAnchor == LISTBOX_ENTRY_NOTFOUND ) && ( mpEntryList->GetSelectEntryCount() || mbStackMode ) )
                {
                    nAnchor = mbStackMode ? 0 : mpEntryList->GetSelectEntryPos( mpEntryList->GetSelectEntryCount() - 1 );
                }
                if( nAnchor != LISTBOX_ENTRY_NOTFOUND )
                {
                    // Alle Eintraege vom Anchor bis nSelect muessen selektiert sein
                    sal_uInt16 nStart = Min( nSelect, nAnchor );
                    sal_uInt16 nEnd = Max( nSelect, nAnchor );
                    for ( sal_uInt16 n = nStart; n <= nEnd; n++ )
                    {
                        if ( !mpEntryList->IsEntryPosSelected( n ) )
                        {
                            SelectEntry( n, sal_True );
                            bSelectionChanged = sal_True;
                        }
                    }

                    // Ggf. muss noch was deselektiert werden...
                    sal_uInt16 nLast = mpEntryList->GetLastSelected();
                    if ( nLast != LISTBOX_ENTRY_NOTFOUND )
                    {
                        if ( ( nLast > nSelect ) && ( nLast > nAnchor ) )
                        {
                            for ( sal_uInt16 n = nSelect+1; n <= nLast; n++ )
                            {
                                if ( mpEntryList->IsEntryPosSelected( n ) )
                                {
                                    SelectEntry( n, sal_False );
                                    bSelectionChanged = sal_True;
                                }
                            }
                        }
                        else if ( ( nLast < nSelect ) && ( nLast < nAnchor ) )
                        {
                            for ( sal_uInt16 n = nLast; n < nSelect; n++ )
                            {
                                if ( mpEntryList->IsEntryPosSelected( n ) )
                                {
                                    SelectEntry( n, sal_False );
                                    bSelectionChanged = sal_True;
                                }
                            }
                        }
                    }
                    mpEntryList->SetLastSelected( nSelect );
                }
            }
            else if( eLET != LET_TRACKING )
            {
                ImplHideFocusRect();
                ImplPaint( nSelect, sal_True );
                bFocusChanged = sal_True;
            }
        }
        else if( bShift )
        {
            bFocusChanged = sal_True;
        }

        if( bSelectionChanged )
            mbSelectionChanged = sal_True;

        if( bFocusChanged )
        {
            long nHeightDiff = mpEntryList->GetAddedHeight( nSelect, mnTop, 0 );
            maFocusRect.SetPos( Point( 0, nHeightDiff ) );
            Size aSz( maFocusRect.GetWidth(),
                      mpEntryList->GetEntryHeight( nSelect ) );
            maFocusRect.SetSize( aSz );
            if( HasFocus() )
                ImplShowFocusRect();
        }
        ImplClearLayoutData();
    }
    return bSelectionChanged;
}

// -----------------------------------------------------------------------

void ImplListBoxWindow::Tracking( const TrackingEvent& rTEvt )
{
    Point aPoint;
    Rectangle aRect( aPoint, GetOutputSizePixel() );
    sal_Bool bInside = aRect.IsInside( rTEvt.GetMouseEvent().GetPosPixel() );

    if( rTEvt.IsTrackingCanceled() || rTEvt.IsTrackingEnded() ) // MouseButtonUp
    {
        if ( bInside && !rTEvt.IsTrackingCanceled() )
        {
            mnSelectModifier = rTEvt.GetMouseEvent().GetModifier();
            ImplCallSelect();
        }
        else
        {
            maCancelHdl.Call( NULL );
            if ( !mbMulti )
            {
                mbTrackingSelect = sal_True;
                SelectEntry( mnTrackingSaveSelection, sal_True );
                mbTrackingSelect = sal_False;
                if ( mnTrackingSaveSelection != LISTBOX_ENTRY_NOTFOUND )
                {
                    long nHeightDiff = mpEntryList->GetAddedHeight( mnCurrentPos, mnTop, 0 );
                    maFocusRect.SetPos( Point( 0, nHeightDiff ) );
                    Size aSz( maFocusRect.GetWidth(),
                              mpEntryList->GetEntryHeight( mnCurrentPos ) );
                    maFocusRect.SetSize( aSz );
                    ImplShowFocusRect();
                }
            }
        }

        mbTrack = sal_False;
    }
    else
    {
        sal_Bool bTrackOrQuickClick = mbTrack;
        if( !mbTrack )
        {
            if ( bInside )
            {
                mbTrack = sal_True;
            }

            // Folgender Fall tritt nur auf, wenn man ganz kurz die Maustaste drueckt
            if( rTEvt.IsTrackingEnded() && mbTrack )
            {
                bTrackOrQuickClick = sal_True;
                mbTrack = sal_False;
            }
        }

        if( bTrackOrQuickClick )
        {
            MouseEvent aMEvt = rTEvt.GetMouseEvent();
            Point aPt( aMEvt.GetPosPixel() );
            sal_Bool bShift = aMEvt.IsShift();
            sal_Bool bCtrl  = aMEvt.IsMod1();

            sal_uInt16 nSelect = LISTBOX_ENTRY_NOTFOUND;
            if( aPt.Y() < 0 )
            {
                if ( mnCurrentPos != LISTBOX_ENTRY_NOTFOUND )
                {
                    nSelect = mnCurrentPos ? ( mnCurrentPos - 1 ) : 0;
                    if( nSelect < mnTop )
                        SetTopEntry( mnTop-1 );
                }
            }
            else if( aPt.Y() > GetOutputSizePixel().Height() )
            {
                if ( mnCurrentPos != LISTBOX_ENTRY_NOTFOUND )
                {
                    nSelect = Min(  (sal_uInt16)(mnCurrentPos+1), (sal_uInt16)(mpEntryList->GetEntryCount()-1) );
                    if( nSelect >= GetLastVisibleEntry() )
                        SetTopEntry( mnTop+1 );
                }
            }
            else
            {
                nSelect = (sal_uInt16) ( ( aPt.Y() + mnBorder ) / mnMaxHeight ) + (sal_uInt16) mnTop;
                nSelect = Min( nSelect, GetLastVisibleEntry() );
                nSelect = Min( nSelect, (sal_uInt16) ( mpEntryList->GetEntryCount() - 1 ) );
            }

            if ( bInside )
            {
                if ( ( nSelect != mnCurrentPos ) || !GetEntryList()->GetSelectEntryCount() )
                {
                    mbTrackingSelect = sal_True;
                    if ( SelectEntries( nSelect, LET_TRACKING, bShift, bCtrl ) )
                    {
                        if ( mbStackMode ) // #87734# (#87072#)
                        {
                            mbTravelSelect = sal_True;
                            mnSelectModifier = rTEvt.GetMouseEvent().GetModifier();
                            ImplCallSelect();
                            mbTravelSelect = sal_False;
                        }
                    }
                    mbTrackingSelect = sal_False;
                }
            }
            else
            {
                if ( !mbMulti && GetEntryList()->GetSelectEntryCount() )
                {
                    mbTrackingSelect = sal_True;
                    SelectEntry( GetEntryList()->GetSelectEntryPos( 0 ), sal_False );
                    mbTrackingSelect = sal_False;
                }
                else if ( mbStackMode )
                {
                    if ( ( rTEvt.GetMouseEvent().GetPosPixel().X() > 0 )  && ( rTEvt.GetMouseEvent().GetPosPixel().X() < aRect.Right() ) )
                    {
                        if ( ( rTEvt.GetMouseEvent().GetPosPixel().Y() < 0 ) || ( rTEvt.GetMouseEvent().GetPosPixel().Y() > GetOutputSizePixel().Height() ) )
                        {
                            sal_Bool bSelectionChanged = sal_False;
                            if ( ( rTEvt.GetMouseEvent().GetPosPixel().Y() < 0 )
                                   && !mnCurrentPos )
                            {
                                if ( mpEntryList->IsEntryPosSelected( 0 ) )
                                {
                                    SelectEntry( 0, sal_False );
                                    bSelectionChanged = sal_True;
                                    nSelect = LISTBOX_ENTRY_NOTFOUND;

                                }
                            }
                            else
                            {
                                mbTrackingSelect = sal_True;
                                bSelectionChanged = SelectEntries( nSelect, LET_TRACKING, bShift, bCtrl );
                                mbTrackingSelect = sal_False;
                            }

                            if ( bSelectionChanged )
                            {
                                mbSelectionChanged = sal_True;
                                mbTravelSelect = sal_True;
                                mnSelectModifier = rTEvt.GetMouseEvent().GetModifier();
                                ImplCallSelect();
                                mbTravelSelect = sal_False;
                            }
                        }
                    }
                }
            }
            mnCurrentPos = nSelect;
            if ( mnCurrentPos == LISTBOX_ENTRY_NOTFOUND )
            {
                ImplHideFocusRect();
            }
            else
            {
                long nHeightDiff = mpEntryList->GetAddedHeight( mnCurrentPos, mnTop, 0 );
                maFocusRect.SetPos( Point( 0, nHeightDiff ) );
                Size aSz( maFocusRect.GetWidth(), mpEntryList->GetEntryHeight( mnCurrentPos ) );
                maFocusRect.SetSize( aSz );
                ImplShowFocusRect();
            }
        }
    }
}


// -----------------------------------------------------------------------

void ImplListBoxWindow::KeyInput( const KeyEvent& rKEvt )
{
    if( !ProcessKeyInput( rKEvt ) )
        Control::KeyInput( rKEvt );
}

// -----------------------------------------------------------------------

sal_Bool ImplListBoxWindow::ProcessKeyInput( const KeyEvent& rKEvt )
{
    // zu selektierender Eintrag
    sal_uInt16 nSelect = LISTBOX_ENTRY_NOTFOUND;
    LB_EVENT_TYPE eLET = LET_KEYMOVE;

    KeyCode aKeyCode = rKEvt.GetKeyCode();

    sal_Bool bShift = aKeyCode.IsShift();
    sal_Bool bCtrl  = aKeyCode.IsMod1() || aKeyCode.IsMod3();
    sal_Bool bMod2 = aKeyCode.IsMod2();
    sal_Bool bDone = sal_False;

    switch( aKeyCode.GetCode() )
    {
        case KEY_UP:
        {
            if ( IsReadOnly() )
            {
                if ( GetTopEntry() )
                    SetTopEntry( GetTopEntry()-1 );
            }
            else if ( !bMod2 )
            {
                if( mnCurrentPos == LISTBOX_ENTRY_NOTFOUND )
                {
                    nSelect = mpEntryList->FindFirstSelectable( 0, true );
                }
                else if ( mnCurrentPos )
                {
                    // search first selectable above the current position
                    nSelect = mpEntryList->FindFirstSelectable( mnCurrentPos - 1, false );
                }

                if( ( nSelect != LISTBOX_ENTRY_NOTFOUND ) && ( nSelect < mnTop ) )
                    SetTopEntry( mnTop-1 );

                bDone = sal_True;
            }
            maQuickSelectionEngine.Reset();
        }
        break;

        case KEY_DOWN:
        {
            if ( IsReadOnly() )
            {
                SetTopEntry( GetTopEntry()+1 );
            }
            else if ( !bMod2 )
            {
                if( mnCurrentPos == LISTBOX_ENTRY_NOTFOUND )
                {
                    nSelect = mpEntryList->FindFirstSelectable( 0, true );
                }
                else if ( (mnCurrentPos+1) < mpEntryList->GetEntryCount() )
                {
                    // search first selectable below the current position
                    nSelect = mpEntryList->FindFirstSelectable( mnCurrentPos + 1, true );
                }

                if( ( nSelect != LISTBOX_ENTRY_NOTFOUND ) && ( nSelect >= GetLastVisibleEntry() ) )
                    SetTopEntry( mnTop+1 );

                bDone = sal_True;
            }
            maQuickSelectionEngine.Reset();
        }
        break;

        case KEY_PAGEUP:
        {
            if ( IsReadOnly() )
            {
                sal_uInt16 nCurVis = GetLastVisibleEntry() - mnTop +1;
                SetTopEntry( ( mnTop > nCurVis ) ?
                                (mnTop-nCurVis) : 0 );
            }
            else if ( !bCtrl && !bMod2 )
            {
                if( mnCurrentPos == LISTBOX_ENTRY_NOTFOUND )
                {
                    nSelect = mpEntryList->FindFirstSelectable( 0, true );
                }
                else if ( mnCurrentPos )
                {
                    if( mnCurrentPos == mnTop )
                    {
                        sal_uInt16 nCurVis = GetLastVisibleEntry() - mnTop +1;
                        SetTopEntry( ( mnTop > nCurVis ) ? ( mnTop-nCurVis+1 ) : 0 );
                    }

                    // find first selectable starting from mnTop looking foreward
                    nSelect = mpEntryList->FindFirstSelectable( mnTop, true );
                }
                bDone = sal_True;
            }
            maQuickSelectionEngine.Reset();
        }
        break;

        case KEY_PAGEDOWN:
        {
            if ( IsReadOnly() )
            {
                SetTopEntry( GetLastVisibleEntry() );
            }
            else if ( !bCtrl && !bMod2 )
            {
                if( mnCurrentPos == LISTBOX_ENTRY_NOTFOUND )
                {
                    nSelect = mpEntryList->FindFirstSelectable( 0, true );
                }
                else if ( (mnCurrentPos+1) < mpEntryList->GetEntryCount() )
                {
                    sal_uInt16 nCount = mpEntryList->GetEntryCount();
                    sal_uInt16 nCurVis = GetLastVisibleEntry() - mnTop;
                    sal_uInt16 nTmp = Min( nCurVis, nCount );
                    nTmp += mnTop - 1;
                    if( mnCurrentPos == nTmp && mnCurrentPos != nCount - 1 )
                    {
                        long nTmp2 = Min( (long)(nCount-nCurVis), (long)((long)mnTop+(long)nCurVis-1) );
                        nTmp2 = Max( (long)0 , nTmp2 );
                        nTmp = (sal_uInt16)(nTmp2+(nCurVis-1) );
                        SetTopEntry( (sal_uInt16)nTmp2 );
                    }
                    // find first selectable starting from nTmp looking backwards
                    nSelect = mpEntryList->FindFirstSelectable( nTmp, false );
                }
                bDone = sal_True;
            }
            maQuickSelectionEngine.Reset();
        }
        break;

        case KEY_HOME:
        {
            if ( IsReadOnly() )
            {
                SetTopEntry( 0 );
            }
            else if ( !bCtrl && !bMod2 )
            {
                if ( mnCurrentPos )
                {
                    nSelect = mpEntryList->FindFirstSelectable( mpEntryList->GetEntryCount() ? 0 : LISTBOX_ENTRY_NOTFOUND, true );
                    if( mnTop != 0 )
                        SetTopEntry( 0 );

                    bDone = sal_True;
                }
            }
            maQuickSelectionEngine.Reset();
        }
        break;

        case KEY_END:
        {
            if ( IsReadOnly() )
            {
                SetTopEntry( 0xFFFF );
            }
            else if ( !bCtrl && !bMod2 )
            {
                if( mnCurrentPos == LISTBOX_ENTRY_NOTFOUND )
                {
                    nSelect = mpEntryList->FindFirstSelectable( 0, true );
                }
                else if ( (mnCurrentPos+1) < mpEntryList->GetEntryCount() )
                {
                    sal_uInt16 nCount = mpEntryList->GetEntryCount();
                    nSelect = mpEntryList->FindFirstSelectable( nCount - 1, false );
                    sal_uInt16 nCurVis = GetLastVisibleEntry() - mnTop + 1;
                    if( nCount > nCurVis )
                        SetTopEntry( nCount - nCurVis );
                }
                bDone = sal_True;
            }
            maQuickSelectionEngine.Reset();
        }
        break;

        case KEY_LEFT:
        {
            if ( !bCtrl && !bMod2 )
            {
                ScrollHorz( -HORZ_SCROLL );
                bDone = sal_True;
            }
            maQuickSelectionEngine.Reset();
        }
        break;

        case KEY_RIGHT:
        {
            if ( !bCtrl && !bMod2 )
            {
                ScrollHorz( HORZ_SCROLL );
                bDone = sal_True;
            }
            maQuickSelectionEngine.Reset();
        }
        break;

        case KEY_RETURN:
        {
            if ( !bMod2 && !IsReadOnly() )
            {
                mnSelectModifier = rKEvt.GetKeyCode().GetModifier();
                ImplCallSelect();
                bDone = sal_False;  // RETURN nicht abfangen.
            }
            maQuickSelectionEngine.Reset();
        }
        break;

        case KEY_SPACE:
        {
            if ( !bMod2 && !IsReadOnly() )
            {
                if( mbMulti && ( !mbSimpleMode || ( mbSimpleMode && bCtrl && !bShift ) || mbStackMode ) )
                {
                    nSelect = mnCurrentPos;
                    eLET = LET_KEYSPACE;
                }
                bDone = sal_True;
            }
            maQuickSelectionEngine.Reset();
        }
        break;

        case KEY_A:
        {
            if( bCtrl && mbMulti )
            {
                // paint only once
                sal_Bool bUpdates = IsUpdateMode();
                SetUpdateMode( sal_False );

                sal_uInt16 nEntryCount = mpEntryList->GetEntryCount();
                for( sal_uInt16 i = 0; i < nEntryCount; i++ )
                    SelectEntry( i, sal_True );

                // restore update mode
                SetUpdateMode( bUpdates );
                Invalidate();

                maQuickSelectionEngine.Reset();

                bDone = sal_True;
                break;
            }
        }
        // fall through intentional
        default:
        {
            if ( !IsReadOnly() )
            {
                bDone = maQuickSelectionEngine.HandleKeyEvent( rKEvt );
            }
          }
        break;
    }

    if  (   ( nSelect != LISTBOX_ENTRY_NOTFOUND )
        &&  (   ( !mpEntryList->IsEntryPosSelected( nSelect ) )
            ||  ( eLET == LET_KEYSPACE )
            )
        )
    {
        DBG_ASSERT( !mpEntryList->IsEntryPosSelected( nSelect ) || mbMulti, "ImplListBox: Selecting same Entry" );
        if( nSelect >= mpEntryList->GetEntryCount() )
            nSelect = mpEntryList->GetEntryCount()-1;
        mnCurrentPos = nSelect;
        if ( SelectEntries( nSelect, eLET, bShift, bCtrl ) )
        {
            mbTravelSelect = sal_True;
            mnSelectModifier = rKEvt.GetKeyCode().GetModifier();
            ImplCallSelect();
            mbTravelSelect = sal_False;
        }
    }

    return bDone;
}

// -----------------------------------------------------------------------
namespace
{
    static ::vcl::StringEntryIdentifier lcl_getEntry( const ImplEntryList& _rList, sal_uInt16 _nPos, String& _out_entryText )
    {
        OSL_PRECOND( ( _nPos != LISTBOX_ENTRY_NOTFOUND ), "lcl_getEntry: invalid position!" );
        sal_uInt16 nEntryCount( _rList.GetEntryCount() );
        if ( _nPos >= nEntryCount )
            _nPos = 0;
        _out_entryText = _rList.GetEntryText( _nPos );

        // ::vcl::StringEntryIdentifier does not allow for 0 values, but our position is 0-based
        // => normalize
        return reinterpret_cast< ::vcl::StringEntryIdentifier >( _nPos + 1 );
    }

    static sal_uInt16 lcl_getEntryPos( ::vcl::StringEntryIdentifier _entry )
    {
        // our pos is 0-based, but StringEntryIdentifier does not allow for a NULL
        return static_cast< sal_uInt16 >( reinterpret_cast< sal_Int64 >( _entry ) ) - 1;
    }
}

// -----------------------------------------------------------------------
::vcl::StringEntryIdentifier ImplListBoxWindow::CurrentEntry( String& _out_entryText ) const
{
    return lcl_getEntry( *GetEntryList(), ( mnCurrentPos == LISTBOX_ENTRY_NOTFOUND ) ? 0 : mnCurrentPos + 1, _out_entryText );
}

// -----------------------------------------------------------------------
::vcl::StringEntryIdentifier ImplListBoxWindow::NextEntry( ::vcl::StringEntryIdentifier _currentEntry, String& _out_entryText ) const
{
    sal_uInt16 nNextPos = lcl_getEntryPos( _currentEntry ) + 1;
    return lcl_getEntry( *GetEntryList(), nNextPos, _out_entryText );
}

// -----------------------------------------------------------------------
void ImplListBoxWindow::SelectEntry( ::vcl::StringEntryIdentifier _entry )
{
    sal_uInt16 nSelect = lcl_getEntryPos( _entry );
    if ( mpEntryList->IsEntryPosSelected( nSelect ) )
    {
        // ignore that. This method is a callback from the QuickSelectionEngine, which means the user attempted
        // to select the given entry by typing its starting letters. No need to act.
        return;
    }

    // normalize
    OSL_ENSURE( nSelect < mpEntryList->GetEntryCount(), "ImplListBoxWindow::SelectEntry: how that?" );
    if( nSelect >= mpEntryList->GetEntryCount() )
        nSelect = mpEntryList->GetEntryCount()-1;

    // make visible
    ShowProminentEntry( nSelect );

    // actually select
    mnCurrentPos = nSelect;
    if ( SelectEntries( nSelect, LET_KEYMOVE, sal_False, sal_False ) )
    {
        mbTravelSelect = sal_True;
        mnSelectModifier = 0;
        ImplCallSelect();
        mbTravelSelect = sal_False;
    }
}

// -----------------------------------------------------------------------

void ImplListBoxWindow::ImplPaint( sal_uInt16 nPos, sal_Bool bErase, bool bLayout )
{
    const StyleSettings& rStyleSettings = GetSettings().GetStyleSettings();

    const ImplEntryType* pEntry = mpEntryList->GetEntryPtr( nPos );
    if( ! pEntry )
        return;

    long nWidth  = GetOutputSizePixel().Width();
    long nY = mpEntryList->GetAddedHeight( nPos, mnTop );
    Rectangle aRect( Point( 0, nY ), Size( nWidth, pEntry->mnHeight ) );

    if( ! bLayout )
    {
        if( mpEntryList->IsEntryPosSelected( nPos ) )
        {
            SetTextColor( !IsEnabled() ? rStyleSettings.GetDisableColor() : rStyleSettings.GetHighlightTextColor() );
            SetFillColor( rStyleSettings.GetHighlightColor() );
            SetTextFillColor( rStyleSettings.GetHighlightColor() );
            DrawRect( aRect );
        }
        else
        {
            ImplInitSettings( sal_False, sal_True, sal_False );
            if( !IsEnabled() )
                SetTextColor( rStyleSettings.GetDisableColor() );
            SetTextFillColor();
            if( bErase )
                Erase( aRect );
        }
    }

    if ( IsUserDrawEnabled() )
    {
        mbInUserDraw = sal_True;
        mnUserDrawEntry = nPos;
        aRect.Left() -= mnLeft;
        if ( nPos < GetEntryList()->GetMRUCount() )
            nPos = GetEntryList()->FindEntry( GetEntryList()->GetEntryText( nPos ) );
        nPos = sal::static_int_cast<sal_uInt16>(nPos - GetEntryList()->GetMRUCount());
        UserDrawEvent aUDEvt( this, aRect, nPos, 0 );
        maUserDrawHdl.Call( &aUDEvt );
        mbInUserDraw = sal_False;
    }
    else
    {
        DrawEntry( nPos, sal_True, sal_True, sal_False, bLayout );
    }
}

// -----------------------------------------------------------------------

void ImplListBoxWindow::DrawEntry( sal_uInt16 nPos, sal_Bool bDrawImage, sal_Bool bDrawText, sal_Bool bDrawTextAtImagePos, bool bLayout )
{
    const ImplEntryType* pEntry = mpEntryList->GetEntryPtr( nPos );
    if( ! pEntry )
        return;

    // Bei Aenderungen in dieser Methode ggf. auch ImplWin::DrawEntry() anpassen.

    if ( mbInUserDraw )
        nPos = mnUserDrawEntry; // real entry, not the matching entry from MRU

    long nY = mpEntryList->GetAddedHeight( nPos, mnTop );
    Size aImgSz;

    if( bDrawImage && mpEntryList->HasImages() && !bLayout )
    {
        Image aImage = mpEntryList->GetEntryImage( nPos );
        if( !!aImage )
        {
            aImgSz = aImage.GetSizePixel();
            Point aPtImg( mnBorder - mnLeft, nY + ( ( pEntry->mnHeight - aImgSz.Height() ) / 2 ) );

            // pb: #106948# explicit mirroring for calc
            if ( mbMirroring )
                // right aligned
                aPtImg.X() = mnMaxWidth + mnBorder - aImgSz.Width() - mnLeft;

            if ( !IsZoom() )
            {
                DrawImage( aPtImg, aImage );
            }
            else
            {
                aImgSz.Width() = CalcZoom( aImgSz.Width() );
                aImgSz.Height() = CalcZoom( aImgSz.Height() );
                DrawImage( aPtImg, aImgSz, aImage );
            }
        }
    }

    if( bDrawText )
    {
        MetricVector* pVector = bLayout ? &mpControlData->mpLayoutData->m_aUnicodeBoundRects : NULL;
        String* pDisplayText = bLayout ? &mpControlData->mpLayoutData->m_aDisplayText : NULL;
        XubString aStr( mpEntryList->GetEntryText( nPos ) );
        if ( aStr.Len() )
        {
            long nMaxWidth = Max( static_cast< long >( mnMaxWidth ),
                                  GetOutputSizePixel().Width() - 2*mnBorder );
            // a multiline entry should only be as wide a the window
            if( (pEntry->mnFlags & LISTBOX_ENTRY_FLAG_MULTILINE) )
                nMaxWidth = GetOutputSizePixel().Width() - 2*mnBorder;

            Rectangle aTextRect( Point( mnBorder - mnLeft, nY ),
                                 Size( nMaxWidth, pEntry->mnHeight ) );

            if( !bDrawTextAtImagePos && ( mpEntryList->HasEntryImage(nPos) || IsUserDrawEnabled() ) )
            {
                long nImageWidth = Max( mnMaxImgWidth, maUserItemSize.Width() );
                aTextRect.Left() += nImageWidth + IMG_TXT_DISTANCE;
            }

            if( bLayout )
                mpControlData->mpLayoutData->m_aLineIndices.push_back( mpControlData->mpLayoutData->m_aDisplayText.Len() );

            // pb: #106948# explicit mirroring for calc
            if ( mbMirroring )
            {
                // right aligned
                aTextRect.Left() = nMaxWidth + mnBorder - GetTextWidth( aStr ) - mnLeft;
                if ( aImgSz.Width() > 0 )
                    aTextRect.Left() -= ( aImgSz.Width() + IMG_TXT_DISTANCE );
            }

            sal_uInt16 nDrawStyle = ImplGetTextStyle();
            if( (pEntry->mnFlags & LISTBOX_ENTRY_FLAG_MULTILINE) )
                nDrawStyle |= MULTILINE_ENTRY_DRAW_FLAGS;
            if( (pEntry->mnFlags & LISTBOX_ENTRY_FLAG_DRAW_DISABLED) )
                nDrawStyle |= TEXT_DRAW_DISABLE;

            DrawText( aTextRect, aStr, nDrawStyle, pVector, pDisplayText );
        }
    }

    if( !bLayout )
    {
        if ( ( mnSeparatorPos != LISTBOX_ENTRY_NOTFOUND ) &&
             ( ( nPos == mnSeparatorPos ) || ( nPos == mnSeparatorPos+1 ) ) )
        {
            Color aOldLineColor( GetLineColor() );
            SetLineColor( ( GetBackground().GetColor() != COL_LIGHTGRAY ) ? COL_LIGHTGRAY : COL_GRAY );
            Point aStartPos( 0, nY );
            if ( nPos == mnSeparatorPos )
                aStartPos.Y() += pEntry->mnHeight-1;
            Point aEndPos( aStartPos );
            aEndPos.X() = GetOutputSizePixel().Width();
            DrawLine( aStartPos, aEndPos );
            SetLineColor( aOldLineColor );
        }
    }
}

// -----------------------------------------------------------------------

void ImplListBoxWindow::FillLayoutData() const
{
    mpControlData->mpLayoutData = new vcl::ControlLayoutData();
    const_cast<ImplListBoxWindow*>(this)->
        ImplDoPaint( Rectangle( Point( 0, 0 ), GetOutputSize() ), true );
}

// -----------------------------------------------------------------------

void ImplListBoxWindow::ImplDoPaint( const Rectangle& rRect, bool bLayout )
{
    sal_uInt16 nCount = mpEntryList->GetEntryCount();

    sal_Bool bShowFocusRect = mbHasFocusRect;
    if ( mbHasFocusRect && ! bLayout )
        ImplHideFocusRect();

    long nY = 0; // + mnBorder;
    long nHeight = GetOutputSizePixel().Height();// - mnMaxHeight + mnBorder;

    for( sal_uInt16 i = (sal_uInt16)mnTop; i < nCount && nY < nHeight + mnMaxHeight; i++ )
    {
        const ImplEntryType* pEntry = mpEntryList->GetEntryPtr( i );
        if( nY + pEntry->mnHeight >= rRect.Top() &&
            nY <= rRect.Bottom() + mnMaxHeight )
        {
            ImplPaint( i, sal_False, bLayout );
        }
        nY += pEntry->mnHeight;
    }

    long nHeightDiff = mpEntryList->GetAddedHeight( mnCurrentPos, mnTop, 0 );
    maFocusRect.SetPos( Point( 0, nHeightDiff ) );
    Size aSz( maFocusRect.GetWidth(), mpEntryList->GetEntryHeight( mnCurrentPos ) );
    maFocusRect.SetSize( aSz );
    if( HasFocus() && bShowFocusRect && !bLayout )
        ImplShowFocusRect();
}

// -----------------------------------------------------------------------

void ImplListBoxWindow::Paint( const Rectangle& rRect )
{
    ImplDoPaint( rRect );
}

// -----------------------------------------------------------------------

sal_uInt16 ImplListBoxWindow::GetDisplayLineCount() const
{
    // FIXME: LISTBOX_ENTRY_FLAG_MULTILINE

    sal_uInt16 nCount = mpEntryList->GetEntryCount();
    long nHeight = GetOutputSizePixel().Height();// - mnMaxHeight + mnBorder;
    sal_uInt16 nEntries = static_cast< sal_uInt16 >( ( nHeight + mnMaxHeight - 1 ) / mnMaxHeight );
    if( nEntries > nCount-mnTop )
        nEntries = nCount-mnTop;

    return nEntries;
}

// -----------------------------------------------------------------------

void ImplListBoxWindow::Resize()
{
    Control::Resize();

    sal_Bool bShowFocusRect = mbHasFocusRect;
    if ( bShowFocusRect )
        ImplHideFocusRect();

    if( mnCurrentPos != LISTBOX_ENTRY_NOTFOUND )
    {
        Size aSz( GetOutputSizePixel().Width(), mpEntryList->GetEntryHeight( mnCurrentPos ) );
        maFocusRect.SetSize( aSz );
    }

    if ( bShowFocusRect )
        ImplShowFocusRect();

    ImplClearLayoutData();
}

// -----------------------------------------------------------------------

void ImplListBoxWindow::GetFocus()
{
    sal_uInt16 nPos = mnCurrentPos;
    if ( nPos == LISTBOX_ENTRY_NOTFOUND )
        nPos = 0;
    long nHeightDiff = mpEntryList->GetAddedHeight( nPos, mnTop, 0 );
    maFocusRect.SetPos( Point( 0, nHeightDiff ) );
    Size aSz( maFocusRect.GetWidth(), mpEntryList->GetEntryHeight( nPos ) );
    maFocusRect.SetSize( aSz );
    ImplShowFocusRect();
    Control::GetFocus();
}

// -----------------------------------------------------------------------

void ImplListBoxWindow::LoseFocus()
{
    ImplHideFocusRect();
    Control::LoseFocus();
}

// -----------------------------------------------------------------------

void ImplListBoxWindow::SetTopEntry( sal_uInt16 nTop )
{
    if( mpEntryList->GetEntryCount() == 0 )
        return;

    long nWHeight = PixelToLogic( GetSizePixel() ).Height();

    sal_uInt16 nLastEntry = mpEntryList->GetEntryCount()-1;
    if( nTop > nLastEntry )
        nTop = nLastEntry;
    const ImplEntryType* pLast = mpEntryList->GetEntryPtr( nLastEntry );
    while( nTop > 0 && mpEntryList->GetAddedHeight( nLastEntry, nTop-1 ) + pLast->mnHeight <= nWHeight )
        nTop--;

    if ( nTop != mnTop )
    {
        ImplClearLayoutData();
        long nDiff = mpEntryList->GetAddedHeight( mnTop, nTop, 0 );
        Update();
        ImplHideFocusRect();
        mnTop = nTop;
        Scroll( 0, nDiff );
        Update();
        if( HasFocus() )
            ImplShowFocusRect();
        maScrollHdl.Call( this );
    }
}

// -----------------------------------------------------------------------

void ImplListBoxWindow::ShowProminentEntry( sal_uInt16 nEntryPos )
{
    if( meProminentType == PROMINENT_MIDDLE )
    {
        sal_uInt16 nPos = nEntryPos;
        long nWHeight = PixelToLogic( GetSizePixel() ).Height();
        while( nEntryPos > 0 && mpEntryList->GetAddedHeight( nPos+1, nEntryPos ) < nWHeight/2 )
            nEntryPos--;
    }
    SetTopEntry( nEntryPos );
}

// -----------------------------------------------------------------------

void ImplListBoxWindow::SetLeftIndent( long n )
{
    ScrollHorz( n - mnLeft );
}

// -----------------------------------------------------------------------

void ImplListBoxWindow::ScrollHorz( long n )
{
    long nDiff = 0;
    if ( n > 0 )
    {
        long nWidth = GetOutputSizePixel().Width();
        if( ( mnMaxWidth - mnLeft + n ) > nWidth )
            nDiff = n;
    }
    else if ( n < 0 )
    {
        if( mnLeft )
        {
            long nAbs = -n;
            nDiff = - ( ( mnLeft > nAbs ) ? nAbs : mnLeft );
        }
    }

    if ( nDiff )
    {
        ImplClearLayoutData();
        mnLeft = sal::static_int_cast<sal_uInt16>(mnLeft + nDiff);
        Update();
        ImplHideFocusRect();
        Scroll( -nDiff, 0 );
        Update();
        if( HasFocus() )
            ImplShowFocusRect();
        maScrollHdl.Call( this );
    }
}

// -----------------------------------------------------------------------

Size ImplListBoxWindow::CalcSize( sal_uInt16 nMaxLines ) const
{
    // FIXME: LISTBOX_ENTRY_FLAG_MULTILINE

    Size aSz;
//  sal_uInt16 nL = Min( nMaxLines, mpEntryList->GetEntryCount() );
    aSz.Height() =  nMaxLines * mnMaxHeight;
    aSz.Width() = mnMaxWidth + 2*mnBorder;
    return aSz;
}

// -----------------------------------------------------------------------

Rectangle ImplListBoxWindow::GetBoundingRectangle( sal_uInt16 nItem ) const
{
    const ImplEntryType* pEntry = mpEntryList->GetEntryPtr( nItem );
    Size aSz( GetSizePixel().Width(), pEntry ? pEntry->mnHeight : GetEntryHeight() );
    long nY = mpEntryList->GetAddedHeight( nItem, GetTopEntry() ) - mpEntryList->GetAddedHeight( GetTopEntry() );
    Rectangle aRect( Point( 0, nY ), aSz );
    return aRect;
}


// -----------------------------------------------------------------------

void ImplListBoxWindow::StateChanged( StateChangedType nType )
{
    Control::StateChanged( nType );

    if ( nType == STATE_CHANGE_ZOOM )
    {
        ImplInitSettings( sal_True, sal_False, sal_False );
        ImplCalcMetrics();
        Invalidate();
    }
    else if ( nType == STATE_CHANGE_UPDATEMODE )
    {
        if ( IsUpdateMode() && IsReallyVisible() )
            Invalidate();
    }
    else if ( nType == STATE_CHANGE_CONTROLFONT )
    {
        ImplInitSettings( sal_True, sal_False, sal_False );
        ImplCalcMetrics();
        Invalidate();
    }
    else if ( nType == STATE_CHANGE_CONTROLFOREGROUND )
    {
        ImplInitSettings( sal_False, sal_True, sal_False );
        Invalidate();
    }
    else if ( nType == STATE_CHANGE_CONTROLBACKGROUND )
    {
        ImplInitSettings( sal_False, sal_False, sal_True );
        Invalidate();
    }
    ImplClearLayoutData();
}

// -----------------------------------------------------------------------

void ImplListBoxWindow::DataChanged( const DataChangedEvent& rDCEvt )
{
    Control::DataChanged( rDCEvt );

    if ( (rDCEvt.GetType() == DATACHANGED_FONTS) ||
         (rDCEvt.GetType() == DATACHANGED_FONTSUBSTITUTION) ||
         ((rDCEvt.GetType() == DATACHANGED_SETTINGS) &&
          (rDCEvt.GetFlags() & SETTINGS_STYLE)) )
    {
        ImplClearLayoutData();
        ImplInitSettings( sal_True, sal_True, sal_True );
        ImplCalcMetrics();
        Invalidate();
    }
}

// -----------------------------------------------------------------------

sal_uInt16 ImplListBoxWindow::ImplGetTextStyle() const
{
    sal_uInt16 nTextStyle = TEXT_DRAW_VCENTER;

    if ( mpEntryList->HasImages() )
        nTextStyle |= TEXT_DRAW_LEFT;
    else if ( mbCenter )
        nTextStyle |= TEXT_DRAW_CENTER;
    else if ( mbRight )
        nTextStyle |= TEXT_DRAW_RIGHT;
    else
        nTextStyle |= TEXT_DRAW_LEFT;

    return nTextStyle;
}

// =======================================================================

ImplListBox::ImplListBox( Window* pParent, WinBits nWinStyle ) :
    Control( pParent, nWinStyle ),
    maLBWindow( this, nWinStyle&(~WB_BORDER) )
{
    // for native widget rendering we must be able to detect this window type
    SetType( WINDOW_LISTBOXWINDOW );

    mpVScrollBar    = new ScrollBar( this, WB_VSCROLL | WB_DRAG );
    mpHScrollBar    = new ScrollBar( this, WB_HSCROLL | WB_DRAG );
    mpScrollBarBox  = new ScrollBarBox( this );

    Link aLink( LINK( this, ImplListBox, ScrollBarHdl ) );
    mpVScrollBar->SetScrollHdl( aLink );
    mpHScrollBar->SetScrollHdl( aLink );

    mbVScroll       = sal_False;
    mbHScroll       = sal_False;
    mbAutoHScroll   = ( nWinStyle & WB_AUTOHSCROLL ) ? sal_True : sal_False;

    maLBWindow.SetScrollHdl( LINK( this, ImplListBox, LBWindowScrolled ) );
    maLBWindow.SetMRUChangedHdl( LINK( this, ImplListBox, MRUChanged ) );
    maLBWindow.Show();
}

void ImplListBox::take_properties(Window &rOther)
{
    Control::take_properties(rOther);

    ImplListBox &rOtherListBoxWindow = static_cast<ImplListBox&>(rOther);
    maLBWindow.take_properties(rOtherListBoxWindow.maLBWindow);
}

// -----------------------------------------------------------------------

ImplListBox::~ImplListBox()
{
    delete mpHScrollBar;
    delete mpVScrollBar;
    delete mpScrollBarBox;
}

// -----------------------------------------------------------------------

void ImplListBox::Clear()
{
    maLBWindow.Clear();
    if ( GetEntryList()->GetMRUCount() )
    {
        maLBWindow.GetEntryList()->SetMRUCount( 0 );
        maLBWindow.SetSeparatorPos( LISTBOX_ENTRY_NOTFOUND );
    }
    mpVScrollBar->SetThumbPos( 0 );
    mpHScrollBar->SetThumbPos( 0 );
    StateChanged( STATE_CHANGE_DATA );
}

// -----------------------------------------------------------------------

sal_uInt16 ImplListBox::InsertEntry( sal_uInt16 nPos, const XubString& rStr )
{
    ImplEntryType* pNewEntry = new ImplEntryType( rStr );
    sal_uInt16 nNewPos = maLBWindow.InsertEntry( nPos, pNewEntry );
    StateChanged( STATE_CHANGE_DATA );
    return nNewPos;
}

// -----------------------------------------------------------------------

sal_uInt16 ImplListBox::InsertEntry( sal_uInt16 nPos, const Image& rImage )
{
    ImplEntryType* pNewEntry = new ImplEntryType( rImage );
    sal_uInt16 nNewPos = maLBWindow.InsertEntry( nPos, pNewEntry );
    StateChanged( STATE_CHANGE_DATA );
    return nNewPos;
}

// -----------------------------------------------------------------------

sal_uInt16 ImplListBox::InsertEntry( sal_uInt16 nPos, const XubString& rStr, const Image& rImage )
{
    ImplEntryType* pNewEntry = new ImplEntryType( rStr, rImage );
    sal_uInt16 nNewPos = maLBWindow.InsertEntry( nPos, pNewEntry );
    StateChanged( STATE_CHANGE_DATA );
    return nNewPos;
}

// -----------------------------------------------------------------------

void ImplListBox::RemoveEntry( sal_uInt16 nPos )
{
    maLBWindow.RemoveEntry( nPos );
    StateChanged( STATE_CHANGE_DATA );
}

// -----------------------------------------------------------------------

void ImplListBox::SetEntryFlags( sal_uInt16 nPos, long nFlags )
{
    maLBWindow.SetEntryFlags( nPos, nFlags );
}

// -----------------------------------------------------------------------

void ImplListBox::SelectEntry( sal_uInt16 nPos, sal_Bool bSelect )
{
    maLBWindow.SelectEntry( nPos, bSelect );
}

// -----------------------------------------------------------------------

void ImplListBox::SetNoSelection()
{
    maLBWindow.DeselectAll();
}

// -----------------------------------------------------------------------

void ImplListBox::GetFocus()
{
    maLBWindow.GrabFocus();
}

// -----------------------------------------------------------------------

Window* ImplListBox::GetPreferredKeyInputWindow()
{
    return &maLBWindow;
}

// -----------------------------------------------------------------------

void ImplListBox::Resize()
{
    Control::Resize();
    ImplResizeControls();
    ImplCheckScrollBars();
}


// -----------------------------------------------------------------------

IMPL_LINK_NOARG(ImplListBox, MRUChanged)
{
    StateChanged( STATE_CHANGE_DATA );
    return 1;
}

// -----------------------------------------------------------------------

IMPL_LINK_NOARG(ImplListBox, LBWindowScrolled)
{
    long nSet = GetTopEntry();
    if( nSet > mpVScrollBar->GetRangeMax() )
        mpVScrollBar->SetRangeMax( GetEntryList()->GetEntryCount() );
    mpVScrollBar->SetThumbPos( GetTopEntry() );

    mpHScrollBar->SetThumbPos( GetLeftIndent() );

    maScrollHdl.Call( this );

    return 1;
}

// -----------------------------------------------------------------------

IMPL_LINK( ImplListBox, ScrollBarHdl, ScrollBar*, pSB )
{
    sal_uInt16 nPos = (sal_uInt16) pSB->GetThumbPos();
    if( pSB == mpVScrollBar )
        SetTopEntry( nPos );
    else if( pSB == mpHScrollBar )
        SetLeftIndent( nPos );

    return 1;
}

// -----------------------------------------------------------------------

void ImplListBox::ImplCheckScrollBars()
{
    sal_Bool bArrange = sal_False;

    Size aOutSz = GetOutputSizePixel();
    sal_uInt16 nEntries = GetEntryList()->GetEntryCount();
    sal_uInt16 nMaxVisEntries = (sal_uInt16) (aOutSz.Height() / GetEntryHeight());

    // vert. ScrollBar
    if( nEntries > nMaxVisEntries )
    {
        if( !mbVScroll )
            bArrange = sal_True;
        mbVScroll = sal_True;

        // Ueberpruefung des rausgescrollten Bereichs
        if( GetEntryList()->GetSelectEntryCount() == 1 &&
            GetEntryList()->GetSelectEntryPos( 0 ) != LISTBOX_ENTRY_NOTFOUND )
            ShowProminentEntry( GetEntryList()->GetSelectEntryPos( 0 ) );
        else
            SetTopEntry( GetTopEntry() );   // MaxTop wird geprueft...
    }
    else
    {
        if( mbVScroll )
            bArrange = sal_True;
        mbVScroll = sal_False;
        SetTopEntry( 0 );
    }

    // horz. ScrollBar
    if( mbAutoHScroll )
    {
        long nWidth = (sal_uInt16) aOutSz.Width();
        if ( mbVScroll )
            nWidth -= mpVScrollBar->GetSizePixel().Width();

        long nMaxWidth = GetMaxEntryWidth();
        if( nWidth < nMaxWidth )
        {
            if( !mbHScroll )
                bArrange = sal_True;
            mbHScroll = sal_True;

            if ( !mbVScroll )   // ggf. brauchen wir jetzt doch einen
            {
                nMaxVisEntries = (sal_uInt16) ( ( aOutSz.Height() - mpHScrollBar->GetSizePixel().Height() ) / GetEntryHeight() );
                if( nEntries > nMaxVisEntries )
                {
                    bArrange = sal_True;
                    mbVScroll = sal_True;

                    // Ueberpruefung des rausgescrollten Bereichs
                    if( GetEntryList()->GetSelectEntryCount() == 1 &&
                        GetEntryList()->GetSelectEntryPos( 0 ) != LISTBOX_ENTRY_NOTFOUND )
                        ShowProminentEntry( GetEntryList()->GetSelectEntryPos( 0 ) );
                    else
                        SetTopEntry( GetTopEntry() );   // MaxTop wird geprueft...
                }
            }

            // Ueberpruefung des rausgescrollten Bereichs
            sal_uInt16 nMaxLI = (sal_uInt16) (nMaxWidth - nWidth);
            if ( nMaxLI < GetLeftIndent() )
                SetLeftIndent( nMaxLI );
        }
        else
        {
            if( mbHScroll )
                bArrange = sal_True;
            mbHScroll = sal_False;
            SetLeftIndent( 0 );
        }
    }

    if( bArrange )
        ImplResizeControls();

    ImplInitScrollBars();
}

// -----------------------------------------------------------------------

void ImplListBox::ImplInitScrollBars()
{
    Size aOutSz = maLBWindow.GetOutputSizePixel();

    if ( mbVScroll )
    {
        sal_uInt16 nEntries = GetEntryList()->GetEntryCount();
        sal_uInt16 nVisEntries = (sal_uInt16) (aOutSz.Height() / GetEntryHeight());
        mpVScrollBar->SetRangeMax( nEntries );
        mpVScrollBar->SetVisibleSize( nVisEntries );
        mpVScrollBar->SetPageSize( nVisEntries - 1 );
    }

    if ( mbHScroll )
    {
        mpHScrollBar->SetRangeMax( GetMaxEntryWidth() + HORZ_SCROLL );
        mpHScrollBar->SetVisibleSize( (sal_uInt16)aOutSz.Width() );
        mpHScrollBar->SetLineSize( HORZ_SCROLL );
        mpHScrollBar->SetPageSize( aOutSz.Width() - HORZ_SCROLL );
    }
}

// -----------------------------------------------------------------------

void ImplListBox::ImplResizeControls()
{
    // Hier werden die Controls nur angeordnet, ob die Scrollbars
    // sichtbar sein sollen wird bereits in ImplCheckScrollBars ermittelt.

    Size aOutSz = GetOutputSizePixel();
    long nSBWidth = GetSettings().GetStyleSettings().GetScrollBarSize();
    nSBWidth = CalcZoom( nSBWidth );

    Size aInnerSz( aOutSz );
    if ( mbVScroll )
        aInnerSz.Width() -= nSBWidth;
    if ( mbHScroll )
        aInnerSz.Height() -= nSBWidth;

    // pb: #106948# explicit mirroring for calc
    // Scrollbar on left or right side?
    sal_Bool bMirroring = maLBWindow.IsMirroring();
    Point aWinPos( bMirroring && mbVScroll ? nSBWidth : 0, 0 );
    maLBWindow.SetPosSizePixel( aWinPos, aInnerSz );

    // ScrollBarBox
    if( mbVScroll && mbHScroll )
    {
        Point aBoxPos( bMirroring ? 0 : aInnerSz.Width(), aInnerSz.Height() );
        mpScrollBarBox->SetPosSizePixel( aBoxPos, Size( nSBWidth, nSBWidth ) );
        mpScrollBarBox->Show();
    }
    else
    {
        mpScrollBarBox->Hide();
    }

    // vert. ScrollBar
    if( mbVScroll )
    {
        // Scrollbar on left or right side?
        Point aVPos( bMirroring ? 0 : aOutSz.Width() - nSBWidth, 0 );
        mpVScrollBar->SetPosSizePixel( aVPos, Size( nSBWidth, aInnerSz.Height() ) );
        mpVScrollBar->Show();
    }
    else
    {
        mpVScrollBar->Hide();
        // #107254# Don't reset top entry after resize, but check for max top entry
        SetTopEntry( GetTopEntry() );
    }

    // horz. ScrollBar
    if( mbHScroll )
    {
        Point aHPos( ( bMirroring && mbVScroll ) ? nSBWidth : 0, aOutSz.Height() - nSBWidth );
        mpHScrollBar->SetPosSizePixel( aHPos, Size( aInnerSz.Width(), nSBWidth ) );
        mpHScrollBar->Show();
    }
    else
    {
        mpHScrollBar->Hide();
        SetLeftIndent( 0 );
    }
}

// -----------------------------------------------------------------------

void ImplListBox::StateChanged( StateChangedType nType )
{
    if ( nType == STATE_CHANGE_INITSHOW )
    {
        ImplCheckScrollBars();
    }
    else if ( ( nType == STATE_CHANGE_UPDATEMODE ) || ( nType == STATE_CHANGE_DATA ) )
    {
        sal_Bool bUpdate = IsUpdateMode();
        maLBWindow.SetUpdateMode( bUpdate );
        if ( bUpdate && IsReallyVisible() )
            ImplCheckScrollBars();
    }
    else if( nType == STATE_CHANGE_ENABLE )
    {
        mpHScrollBar->Enable( IsEnabled() );
        mpVScrollBar->Enable( IsEnabled() );
        mpScrollBarBox->Enable( IsEnabled() );
        Invalidate();
    }
    else if ( nType == STATE_CHANGE_ZOOM )
    {
        maLBWindow.SetZoom( GetZoom() );
        Resize();
    }
    else if ( nType == STATE_CHANGE_CONTROLFONT )
    {
        maLBWindow.SetControlFont( GetControlFont() );
    }
    else if ( nType == STATE_CHANGE_CONTROLFOREGROUND )
    {
        maLBWindow.SetControlForeground( GetControlForeground() );
    }
    else if ( nType == STATE_CHANGE_CONTROLBACKGROUND )
    {
        maLBWindow.SetControlBackground( GetControlBackground() );
    }
    else if( nType == STATE_CHANGE_MIRRORING )
    {
        maLBWindow.EnableRTL( IsRTLEnabled() );
        mpHScrollBar->EnableRTL( IsRTLEnabled() );
        mpVScrollBar->EnableRTL( IsRTLEnabled() );
        ImplResizeControls();
    }

    Control::StateChanged( nType );
}

// -----------------------------------------------------------------------

void ImplListBox::DataChanged( const DataChangedEvent& rDCEvt )
{
        Control::DataChanged( rDCEvt );
}

// -----------------------------------------------------------------------

long ImplListBox::Notify( NotifyEvent& rNEvt )
{
    long nDone = 0;
    if ( rNEvt.GetType() == EVENT_COMMAND )
    {
        const CommandEvent& rCEvt = *rNEvt.GetCommandEvent();
        if ( rCEvt.GetCommand() == COMMAND_WHEEL )
        {
            const CommandWheelData* pData = rCEvt.GetWheelData();
            if( !pData->GetModifier() && ( pData->GetMode() == COMMAND_WHEEL_SCROLL ) )
            {
                nDone = HandleScrollCommand( rCEvt, mpHScrollBar, mpVScrollBar );
            }
        }
    }

    return nDone ? nDone : Window::Notify( rNEvt );
}

// -----------------------------------------------------------------------

const Wallpaper& ImplListBox::GetDisplayBackground() const
{
    return maLBWindow.GetDisplayBackground();
}

// -----------------------------------------------------------------------

sal_Bool ImplListBox::HandleWheelAsCursorTravel( const CommandEvent& rCEvt )
{
    sal_Bool bDone = sal_False;
    if ( rCEvt.GetCommand() == COMMAND_WHEEL )
    {
        const CommandWheelData* pData = rCEvt.GetWheelData();
        if( !pData->GetModifier() && ( pData->GetMode() == COMMAND_WHEEL_SCROLL ) )
        {
            sal_uInt16 nKey = ( pData->GetDelta() < 0 ) ? KEY_DOWN : KEY_UP;
            KeyEvent aKeyEvent( 0, KeyCode( nKey ) );
            bDone = ProcessKeyInput( aKeyEvent );
        }
    }
    return bDone;
}

// -----------------------------------------------------------------------

void ImplListBox::SetMRUEntries( const rtl::OUString& rEntries, sal_Unicode cSep )
{
    sal_Bool bChanges = GetEntryList()->GetMRUCount() ? sal_True : sal_False;

    // Remove old MRU entries
    for ( sal_uInt16 n = GetEntryList()->GetMRUCount();n; )
        maLBWindow.RemoveEntry( --n );

    sal_uInt16 nMRUCount = 0;
    sal_Int32 nIndex = 0;
    do
    {
        XubString aEntry = rEntries.getToken( 0, cSep, nIndex );
        // Accept only existing entries
        if ( GetEntryList()->FindEntry( aEntry ) != LISTBOX_ENTRY_NOTFOUND )
        {
            ImplEntryType* pNewEntry = new ImplEntryType( aEntry );
            maLBWindow.GetEntryList()->InsertEntry( nMRUCount++, pNewEntry, sal_False );
            bChanges = sal_True;
        }
    }
    while ( nIndex >= 0 );

    if ( bChanges )
    {
        maLBWindow.GetEntryList()->SetMRUCount( nMRUCount );
        SetSeparatorPos( nMRUCount ? nMRUCount-1 : 0 );
        StateChanged( STATE_CHANGE_DATA );
    }
}

// -----------------------------------------------------------------------

rtl::OUString ImplListBox::GetMRUEntries( sal_Unicode cSep ) const
{
    String aEntries;
    for ( sal_uInt16 n = 0; n < GetEntryList()->GetMRUCount(); n++ )
    {
        aEntries += GetEntryList()->GetEntryText( n );
        if( n < ( GetEntryList()->GetMRUCount() - 1 ) )
            aEntries += cSep;
    }
    return aEntries;
}

// =======================================================================

ImplWin::ImplWin( Window* pParent, WinBits nWinStyle ) :
    Control ( pParent, nWinStyle )
{
    if ( IsNativeControlSupported(CTRL_LISTBOX, PART_ENTIRE_CONTROL)
            && ! IsNativeControlSupported(CTRL_LISTBOX, PART_BUTTON_DOWN) )
        SetBackground();
    else
        SetBackground( Wallpaper( GetSettings().GetStyleSettings().GetFieldColor() ) );

    mbInUserDraw = sal_False;
    mbUserDrawEnabled = sal_False;
    mnItemPos = LISTBOX_ENTRY_NOTFOUND;
}

void ImplWin::take_properties(Window &rOther)
{
    Control::take_properties(rOther);

    ImplWin &rOtherImplWin = static_cast<ImplWin&>(rOther);
    mnItemPos = rOtherImplWin.mnItemPos;
    maString = rOtherImplWin.maString;
    maImage = rOtherImplWin.maImage;
    maFocusRect = rOtherImplWin.maFocusRect;
    maUserItemSize = rOtherImplWin.maUserItemSize;
};

// -----------------------------------------------------------------------

void ImplWin::MBDown()
{
    if( IsEnabled() )
        maMBDownHdl.Call( this );
}

// -----------------------------------------------------------------------

void ImplWin::MouseButtonDown( const MouseEvent& )
{
    if( IsEnabled() )
    {
        MBDown();
    }
}

// -----------------------------------------------------------------------

void ImplWin::FillLayoutData() const
{
    mpControlData->mpLayoutData = new vcl::ControlLayoutData();
    const_cast<ImplWin*>(this)->ImplDraw( true );
}

// -----------------------------------------------------------------------

long ImplWin::PreNotify( NotifyEvent& rNEvt )
{
    long nDone = 0;
    const MouseEvent* pMouseEvt = NULL;

    if( (rNEvt.GetType() == EVENT_MOUSEMOVE) && (pMouseEvt = rNEvt.GetMouseEvent()) != NULL )
    {
        if( pMouseEvt->IsEnterWindow() || pMouseEvt->IsLeaveWindow() )
        {
            // trigger redraw as mouse over state has changed
            if ( IsNativeControlSupported(CTRL_LISTBOX, PART_ENTIRE_CONTROL)
            && ! IsNativeControlSupported(CTRL_LISTBOX, PART_BUTTON_DOWN) )
            {
                GetParent()->GetWindow( WINDOW_BORDER )->Invalidate( INVALIDATE_NOERASE );
                GetParent()->GetWindow( WINDOW_BORDER )->Update();
            }
        }
    }

    return nDone ? nDone : Control::PreNotify(rNEvt);
}

// -----------------------------------------------------------------------

void ImplWin::ImplDraw( bool bLayout )
{
    const StyleSettings& rStyleSettings = GetSettings().GetStyleSettings();

    sal_Bool bNativeOK = sal_False;

    if( ! bLayout )
    {
        ControlState nState = CTRL_STATE_ENABLED;
        if ( IsNativeControlSupported(CTRL_LISTBOX, PART_ENTIRE_CONTROL)
            && IsNativeControlSupported(CTRL_LISTBOX, HAS_BACKGROUND_TEXTURE) )
        {
            // Repaint the (focused) area similarly to
            // ImplSmallBorderWindowView::DrawWindow() in
            // vcl/source/window/brdwin.cxx
            Window *pWin = GetParent();

            ImplControlValue aControlValue;
            if ( !pWin->IsEnabled() )
            nState &= ~CTRL_STATE_ENABLED;
            if ( pWin->HasFocus() )
            nState |= CTRL_STATE_FOCUSED;

            // The listbox is painted over the entire control including the
            // border, but ImplWin does not contain the border => correction
            // needed.
            sal_Int32 nLeft, nTop, nRight, nBottom;
            pWin->GetBorder( nLeft, nTop, nRight, nBottom );
            Point aPoint( -nLeft, -nTop );
            Rectangle aCtrlRegion( aPoint - GetPosPixel(), pWin->GetSizePixel() );

            sal_Bool bMouseOver = sal_False;
            if( GetParent() )
            {
                Window *pChild = GetParent()->GetWindow( WINDOW_FIRSTCHILD );
                while( pChild && (bMouseOver = pChild->IsMouseOver()) == sal_False )
                    pChild = pChild->GetWindow( WINDOW_NEXT );
            }

            if( bMouseOver )
                nState |= CTRL_STATE_ROLLOVER;

            // if parent has no border, then nobody has drawn the background
            // since no border window exists. so draw it here.
            WinBits nParentStyle = pWin->GetStyle();
            if( ! (nParentStyle & WB_BORDER) || (nParentStyle & WB_NOBORDER) )
            {
                Rectangle aParentRect( Point( 0, 0 ), pWin->GetSizePixel() );
                pWin->DrawNativeControl( CTRL_LISTBOX, PART_ENTIRE_CONTROL, aParentRect,
                                         nState, aControlValue, rtl::OUString() );
            }

            bNativeOK = DrawNativeControl( CTRL_LISTBOX, PART_ENTIRE_CONTROL, aCtrlRegion, nState,
                aControlValue, rtl::OUString() );
        }

        if( IsEnabled() )
        {
            if( HasFocus() )
            {
                SetTextColor( rStyleSettings.GetHighlightTextColor() );
                SetFillColor( rStyleSettings.GetHighlightColor() );
                DrawRect( maFocusRect );
            }
            else
            {
                Color aColor;
                if( ImplGetSVData()->maNWFData.mbDDListBoxNoTextArea )
                {
                    if( bNativeOK && (nState & CTRL_STATE_ROLLOVER) )
                        aColor = rStyleSettings.GetButtonRolloverTextColor();
                    else
                        aColor = rStyleSettings.GetButtonTextColor();
                }
                else
                {
                    if( bNativeOK && (nState & CTRL_STATE_ROLLOVER) )
                        aColor = rStyleSettings.GetFieldRolloverTextColor();
                    else
                        aColor = rStyleSettings.GetFieldTextColor();
                }
                if( IsControlForeground() )
                    aColor = GetControlForeground();
                SetTextColor( aColor );
                if ( !bNativeOK )
                    Erase( maFocusRect );
            }
        }
        else // Disabled
        {
            SetTextColor( rStyleSettings.GetDisableColor() );
            if ( !bNativeOK )
                Erase( maFocusRect );
        }
    }

    if ( IsUserDrawEnabled() )
    {
        mbInUserDraw = sal_True;
        UserDrawEvent aUDEvt( this, maFocusRect, mnItemPos, 0 );
        maUserDrawHdl.Call( &aUDEvt );
        mbInUserDraw = sal_False;
    }
    else
    {
        DrawEntry( sal_True, sal_True, sal_False, bLayout );
    }
}

// -----------------------------------------------------------------------

void ImplWin::Paint( const Rectangle& )
{
    ImplDraw();
}

// -----------------------------------------------------------------------

void ImplWin::DrawEntry( sal_Bool bDrawImage, sal_Bool bDrawText, sal_Bool bDrawTextAtImagePos, bool bLayout )
{
    long nBorder = 1;
    Size aOutSz = GetOutputSizePixel();

    sal_Bool bImage = !!maImage;
    if( bDrawImage && bImage && !bLayout )
    {
        sal_uInt16 nStyle = 0;
        Size aImgSz = maImage.GetSizePixel();
        Point aPtImg( nBorder, ( ( aOutSz.Height() - aImgSz.Height() ) / 2 ) );

        // check for HC mode
        Image *pImage = &maImage;

        if ( !IsZoom() )
        {
            DrawImage( aPtImg, *pImage, nStyle );
        }
        else
        {
            aImgSz.Width() = CalcZoom( aImgSz.Width() );
            aImgSz.Height() = CalcZoom( aImgSz.Height() );
            DrawImage( aPtImg, aImgSz, *pImage, nStyle );
        }
    }

    if( bDrawText && maString.Len() )
    {
        sal_uInt16 nTextStyle = TEXT_DRAW_VCENTER;

        if ( bDrawImage && bImage && !bLayout )
            nTextStyle |= TEXT_DRAW_LEFT;
        else if ( GetStyle() & WB_CENTER )
            nTextStyle |= TEXT_DRAW_CENTER;
        else if ( GetStyle() & WB_RIGHT )
            nTextStyle |= TEXT_DRAW_RIGHT;
        else
            nTextStyle |= TEXT_DRAW_LEFT;

        Rectangle aTextRect( Point( nBorder, 0 ), Size( aOutSz.Width()-2*nBorder, aOutSz.Height() ) );

        if ( !bDrawTextAtImagePos && ( bImage || IsUserDrawEnabled() ) )
        {
            long nMaxWidth = Max( maImage.GetSizePixel().Width(), maUserItemSize.Width() );
            aTextRect.Left() += nMaxWidth + IMG_TXT_DISTANCE;
        }

        MetricVector* pVector = bLayout ? &mpControlData->mpLayoutData->m_aUnicodeBoundRects : NULL;
        String* pDisplayText = bLayout ? &mpControlData->mpLayoutData->m_aDisplayText : NULL;
        DrawText( aTextRect, maString, nTextStyle, pVector, pDisplayText );
    }

    if( HasFocus() && !bLayout )
        ShowFocus( maFocusRect );
}

// -----------------------------------------------------------------------

void ImplWin::Resize()
{
    Control::Resize();
    maFocusRect.SetSize( GetOutputSizePixel() );
    Invalidate();
}

// -----------------------------------------------------------------------

void ImplWin::GetFocus()
{
    ShowFocus( maFocusRect );
    if( ImplGetSVData()->maNWFData.mbNoFocusRects &&
        IsNativeWidgetEnabled() &&
        IsNativeControlSupported( CTRL_LISTBOX, PART_ENTIRE_CONTROL ) )
    {
        Window* pWin = GetParent()->GetWindow( WINDOW_BORDER );
        if( ! pWin )
            pWin = GetParent();
        pWin->Invalidate();
    }
    else
        Invalidate();
    Control::GetFocus();
}

// -----------------------------------------------------------------------

void ImplWin::LoseFocus()
{
    HideFocus();
    if( ImplGetSVData()->maNWFData.mbNoFocusRects &&
        IsNativeWidgetEnabled() &&
        IsNativeControlSupported( CTRL_LISTBOX, PART_ENTIRE_CONTROL ) )
    {
        Window* pWin = GetParent()->GetWindow( WINDOW_BORDER );
        if( ! pWin )
            pWin = GetParent();
        pWin->Invalidate();
    }
    else
        Invalidate();
    Control::LoseFocus();
}

// =======================================================================

ImplBtn::ImplBtn( Window* pParent, WinBits nWinStyle ) :
    PushButton(  pParent, nWinStyle ),
    mbDown  ( sal_False )
{
}

// -----------------------------------------------------------------------

void ImplBtn::MBDown()
{
    if( IsEnabled() )
       maMBDownHdl.Call( this );
}

// -----------------------------------------------------------------------

void ImplBtn::MouseButtonDown( const MouseEvent& )
{
    //PushButton::MouseButtonDown( rMEvt );
    if( IsEnabled() )
    {
        MBDown();
        mbDown = sal_True;
    }
}

// =======================================================================

ImplListBoxFloatingWindow::ImplListBoxFloatingWindow( Window* pParent ) :
    FloatingWindow( pParent, WB_BORDER | WB_SYSTEMWINDOW | WB_NOSHADOW )    // no drop shadow for list boxes
{
    mpImplLB = NULL;
    mnDDLineCount = 0;
    mbAutoWidth = sal_False;

    mnPopupModeStartSaveSelection = LISTBOX_ENTRY_NOTFOUND;

    EnableSaveBackground();

    Window * pBorderWindow = ImplGetBorderWindow();
    if( pBorderWindow )
    {
        SetAccessibleRole(accessibility::AccessibleRole::PANEL);
        pBorderWindow->SetAccessibleRole(accessibility::AccessibleRole::WINDOW);
    }
    else
    {
        SetAccessibleRole(accessibility::AccessibleRole::WINDOW);
    }

}

// -----------------------------------------------------------------------

long ImplListBoxFloatingWindow::PreNotify( NotifyEvent& rNEvt )
{
    if( rNEvt.GetType() == EVENT_LOSEFOCUS )
    {
        if( !GetParent()->HasChildPathFocus( sal_True ) )
            EndPopupMode();
    }

    return FloatingWindow::PreNotify( rNEvt );
}

// -----------------------------------------------------------------------

void ImplListBoxFloatingWindow::setPosSizePixel( long nX, long nY, long nWidth, long nHeight, sal_uInt16 nFlags )
{
    FloatingWindow::setPosSizePixel( nX, nY, nWidth, nHeight, nFlags );

    // Fix #60890# ( MBA ): um auch im aufgeklappten Zustand der Listbox die Gr"o\se einfach zu einen
    // Aufruf von Resize() "andern zu k"onnen, wird die Position hier ggf. angepa\t
    if ( IsReallyVisible() && ( nFlags & WINDOW_POSSIZE_HEIGHT ) )
    {
        Point aPos = GetParent()->GetPosPixel();
        aPos = GetParent()->GetParent()->OutputToScreenPixel( aPos );

        if ( nFlags & WINDOW_POSSIZE_X )
            aPos.X() = nX;

        if ( nFlags & WINDOW_POSSIZE_Y )
            aPos.Y() = nY;

        sal_uInt16 nIndex;
        SetPosPixel( ImplCalcPos( this, Rectangle( aPos, GetParent()->GetSizePixel() ), FLOATWIN_POPUPMODE_DOWN, nIndex ) );
    }

//  if( !IsReallyVisible() )
    {
        // Die ImplListBox erhaelt kein Resize, weil nicht sichtbar.
        // Die Fenster muessen aber ein Resize() erhalten, damit die
        // Anzahl der sichtbaren Eintraege fuer PgUp/PgDown stimmt.
        // Die Anzahl kann auch nicht von List/Combobox berechnet werden,
        // weil hierfuer auch die ggf. vorhandene vertikale Scrollbar
        // beruecksichtigt werden muss.
        mpImplLB->SetSizePixel( GetOutputSizePixel() );
        ((Window*)mpImplLB)->Resize();
        ((Window*)mpImplLB->GetMainWindow())->Resize();
    }
}

// -----------------------------------------------------------------------

void ImplListBoxFloatingWindow::Resize()
{
    mpImplLB->GetMainWindow()->ImplClearLayoutData();
    FloatingWindow::Resize();
}

// -----------------------------------------------------------------------

Size ImplListBoxFloatingWindow::CalcFloatSize()
{
    Size aFloatSz( maPrefSz );

    sal_Int32 nLeft, nTop, nRight, nBottom;
    GetBorder( nLeft, nTop, nRight, nBottom );

    sal_uInt16 nLines = mpImplLB->GetEntryList()->GetEntryCount();
    if ( mnDDLineCount && ( nLines > mnDDLineCount ) )
        nLines = mnDDLineCount;

    Size aSz = mpImplLB->CalcSize( nLines );
    long nMaxHeight = aSz.Height() + nTop + nBottom;

    if ( mnDDLineCount )
        aFloatSz.Height() = nMaxHeight;

    if( mbAutoWidth )
    {
        // AutoSize erstmal nur fuer die Breite...

        aFloatSz.Width() = aSz.Width() + nLeft + nRight;
        aFloatSz.Width() += nRight; // etwas mehr Platz sieht besser aus...

        if ( ( aFloatSz.Height() < nMaxHeight ) || ( mnDDLineCount && ( mnDDLineCount < mpImplLB->GetEntryList()->GetEntryCount() ) ) )
        {
            // dann wird noch der vertikale Scrollbar benoetigt
            long nSBWidth = GetSettings().GetStyleSettings().GetScrollBarSize();
            aFloatSz.Width() += nSBWidth;
        }

        long nDesktopWidth = GetDesktopRectPixel().getWidth();
        if (aFloatSz.Width() > nDesktopWidth)
            // Don't exceed the desktop width.
            aFloatSz.Width() = nDesktopWidth;
    }

    if ( aFloatSz.Height() > nMaxHeight )
        aFloatSz.Height() = nMaxHeight;

    // Minimale Hoehe, falls Hoehe nicht auf Float-Hoehe eingestellt wurde.
    // Der Parent vom FloatWin muss die DropDown-Combo/Listbox sein.
    Size aParentSz = GetParent()->GetSizePixel();
    if( (!mnDDLineCount || !nLines) && ( aFloatSz.Height() < aParentSz.Height() ) )
        aFloatSz.Height() = aParentSz.Height();

    // Nicht schmaler als der Parent werden...
    if( aFloatSz.Width() < aParentSz.Width() )
        aFloatSz.Width() = aParentSz.Width();

    // Hoehe auf Entries alignen...
    long nInnerHeight = aFloatSz.Height() - nTop - nBottom;
    long nEntryHeight = mpImplLB->GetEntryHeight();
    if ( nInnerHeight % nEntryHeight )
    {
        nInnerHeight /= nEntryHeight;
        nInnerHeight++;
        nInnerHeight *= nEntryHeight;
        aFloatSz.Height() = nInnerHeight + nTop + nBottom;
    }

    if (aFloatSz.Width() < aSz.Width())
    {
        // The max width of list box entries exceeds the window width.
        // Account for the scroll bar height.
        long nSBWidth = GetSettings().GetStyleSettings().GetScrollBarSize();
        aFloatSz.Height() += nSBWidth;
    }

    return aFloatSz;
}

// -----------------------------------------------------------------------

void ImplListBoxFloatingWindow::StartFloat( sal_Bool bStartTracking )
{
    if( !IsInPopupMode() )
    {
        Size aFloatSz = CalcFloatSize();

        SetSizePixel( aFloatSz );
        mpImplLB->SetSizePixel( GetOutputSizePixel() );

        sal_uInt16 nPos = mpImplLB->GetEntryList()->GetSelectEntryPos( 0 );
        mnPopupModeStartSaveSelection = nPos;

        Size aSz = GetParent()->GetSizePixel();
        Point aPos = GetParent()->GetPosPixel();
        aPos = GetParent()->GetParent()->OutputToScreenPixel( aPos );
        // FIXME: this ugly hack is for Mac/Aqua
        // should be replaced by a real mechanism to place the float rectangle
        if( ImplGetSVData()->maNWFData.mbNoFocusRects &&
            GetParent()->IsNativeWidgetEnabled() )
        {
            sal_Int32 nLeft = 4, nTop = 4, nRight = 4, nBottom = 4;
            aPos.X() += nLeft;
            aPos.Y() += nTop;
            aSz.Width() -= nLeft + nRight;
            aSz.Height() -= nTop + nBottom;
        }
        Rectangle aRect( aPos, aSz );

        // check if the control's parent is un-mirrored which is the case for form controls in a mirrored UI
        // where the document is unmirrored
        // because StartPopupMode() expects a rectangle in mirrored coordinates we have to re-mirror
        if( GetParent()->GetParent()->ImplIsAntiparallel() )
            GetParent()->GetParent()->ImplReMirror( aRect );

        StartPopupMode( aRect, FLOATWIN_POPUPMODE_DOWN );

        if( nPos != LISTBOX_ENTRY_NOTFOUND )
            mpImplLB->ShowProminentEntry( nPos );

        if( bStartTracking )
            mpImplLB->GetMainWindow()->EnableMouseMoveSelect( sal_True );

        if ( mpImplLB->GetMainWindow()->IsGrabFocusAllowed() )
            mpImplLB->GetMainWindow()->GrabFocus();

        mpImplLB->GetMainWindow()->ImplClearLayoutData();
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
