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

// include ---------------------------------------------------------------

#include <stdio.h>

#define _CUI_CHARMAP_CXX_
#include <tools/shl.hxx>
#include <vcl/sound.hxx>
#include <vcl/svapp.hxx>
#include <svtools/colorcfg.hxx>
#include <svl/eitem.hxx>
#include <svl/intitem.hxx>
#include <svl/itempool.hxx>

#include <rtl/textenc.h>
#include <svx/ucsubset.hxx>
#include <sfx2/objsh.hxx>
#include <vcl/msgbox.hxx>
#include <svl/stritem.hxx>

#include <cuires.hrc>
#include <dialmgr.hxx>
#include "cuicharmap.hxx"
#include <sfx2/request.hxx>
#include <sfx2/sfxsids.hrc>
#include <sfx2/app.hxx>
#include <editeng/fontitem.hxx>
#include "charmap.hrc"
#include "macroass.hxx"

// class SvxCharacterMap =================================================

SvxCharacterMap::SvxCharacterMap( Window* pParent, sal_Bool bOne, const SfxItemSet* pSet ) :
    SfxModalDialog( pParent, CUI_RES( RID_SVXDLG_CHARMAP ) ),
    mpCharMapData( 0 )
{
    SFX_ITEMSET_ARG( pSet, pItem, SfxBoolItem, FN_PARAM_1, sal_False );
    if ( pItem )
        bOne = pItem->GetValue();

    mpCharMapData =  new SvxCharMapData( this, bOne, &CUI_MGR() );

    SFX_ITEMSET_ARG( pSet, pCharItem, SfxInt32Item, SID_ATTR_CHAR, sal_False );
    if ( pCharItem )
        SetChar( pCharItem->GetValue() );

    SFX_ITEMSET_ARG( pSet, pDisableItem, SfxBoolItem, FN_PARAM_2, sal_False );
    if ( pDisableItem && pDisableItem->GetValue() )
        DisableFontSelection();

    SFX_ITEMSET_ARG( pSet, pFontItem, SvxFontItem, SID_ATTR_CHAR_FONT, sal_False );
    SFX_ITEMSET_ARG( pSet, pFontNameItem, SfxStringItem, SID_FONT_NAME, sal_False );
    if ( pFontItem )
    {
        Font aFont( pFontItem->GetFamilyName(), pFontItem->GetStyleName(), GetCharFont().GetSize() );
        aFont.SetCharSet( pFontItem->GetCharSet() );
        aFont.SetPitch( pFontItem->GetPitch() );
        SetCharFont( aFont );
    }
    else if ( pFontNameItem )
    {
        Font aFont( GetCharFont() );
        aFont.SetName( pFontNameItem->GetValue() );
        SetCharFont( aFont );
    }

    FreeResource();
    CreateOutputItemSet( pSet ? *pSet->GetPool() : SFX_APP()->GetPool() );
}

// -----------------------------------------------------------------------

SvxCharacterMap::~SvxCharacterMap()
{
    delete mpCharMapData;
}

// -----------------------------------------------------------------------

const Font& SvxCharacterMap::GetCharFont() const
{
    return mpCharMapData->aFont;
}

// -----------------------------------------------------------------------

void SvxCharacterMap::SetChar( sal_UCS4 c )
{
    mpCharMapData->aShowSet.SelectCharacter( c );
}

// -----------------------------------------------------------------------

sal_UCS4 SvxCharacterMap::GetChar() const
{
    return mpCharMapData->aShowSet.GetSelectCharacter();
}

// -----------------------------------------------------------------------

String SvxCharacterMap::GetCharacters() const
{
    return mpCharMapData->aShowText.GetText();
}


// -----------------------------------------------------------------------

void SvxCharacterMap::DisableFontSelection()
{
    mpCharMapData->aFontText.Disable();
    mpCharMapData->aFontLB.Disable();
}

void SvxCharacterMap::SetCharFont( const Font& rFont )
{
    mpCharMapData->SetCharFont( rFont );
}

short SvxCharacterMap::Execute()
{
    short nResult = SfxModalDialog::Execute();
    if ( nResult == RET_OK )
    {
        SfxItemSet* pSet = GetItemSet();
        if ( pSet )
        {
            const SfxItemPool* pPool = pSet->GetPool();
            const Font& rFont( GetCharFont() );
            pSet->Put( SfxStringItem( pPool->GetWhich(SID_CHARMAP), GetCharacters() ) );
            pSet->Put( SvxFontItem( rFont.GetFamily(), rFont.GetName(),
                rFont.GetStyleName(), rFont.GetPitch(), rFont.GetCharSet(), pPool->GetWhich(SID_ATTR_CHAR_FONT) ) );
            pSet->Put( SfxStringItem( pPool->GetWhich(SID_FONT_NAME), rFont.GetName() ) );
            pSet->Put( SfxInt32Item( pPool->GetWhich(SID_ATTR_CHAR), GetChar() ) );
        }
    }

    return nResult;
}


// class SvxShowText =====================================================

SvxShowText::SvxShowText( Window* pParent, const ResId& rResId, sal_Bool bCenter )
:   Control( pParent, rResId ),
    mbCenter( bCenter)
{}

// -----------------------------------------------------------------------

void SvxShowText::Paint( const Rectangle& )
{
    Color aTextCol = GetTextColor();

    const StyleSettings& rStyleSettings = Application::GetSettings().GetStyleSettings();
    const Color aWindowTextColor( rStyleSettings.GetDialogTextColor() );
    SetTextColor( aWindowTextColor );

    const String aText = GetText();
    const Size aSize = GetOutputSizePixel();
    Point aPoint( 2, mnY );

    // adjust position using ink boundary if possible
    Rectangle aBoundRect;
    if( !GetTextBoundRect( aBoundRect, aText ) || aBoundRect.IsEmpty() )
        aPoint.X() = (aSize.Width() - GetTextWidth( aText )) / 2;
    else
    {
        // adjust position before it gets out of bounds
        aBoundRect += aPoint;

        // shift back vertically if needed
        int nYLDelta = aBoundRect.Top();
        int nYHDelta = aSize.Height() - aBoundRect.Bottom();
        if( nYLDelta <= 0 )
            aPoint.Y() -= nYLDelta - 1;
        else if( nYHDelta <= 0 )
            aPoint.Y() += nYHDelta - 1;

        if( mbCenter )
        {
            // move glyph to middle of cell
            aPoint.X() = -aBoundRect.Left()
                       + (aSize.Width() - aBoundRect.GetWidth()) / 2;
        }
        else
        {
            // shift back horizontally if needed
            int nXLDelta = aBoundRect.Left();
            int nXHDelta = aSize.Width() - aBoundRect.Right();
            if( nXLDelta <= 0 )
                aPoint.X() -= nXLDelta - 1;
            else if( nXHDelta <= 0 )
                aPoint.X() += nXHDelta - 1;
        }
    }

    DrawText( aPoint, aText );
    SetTextColor( aTextCol );
}

// -----------------------------------------------------------------------

void SvxShowText::SetFont( const Font& rFont )
{
    long nWinHeight = GetOutputSizePixel().Height();
    Font aFont = rFont;
    aFont.SetWeight( WEIGHT_NORMAL );
    aFont.SetAlign( ALIGN_TOP );
    aFont.SetSize( PixelToLogic( Size( 0, nWinHeight/2 ) ) );
    aFont.SetTransparent( sal_True );
    Control::SetFont( aFont );
    mnY = ( nWinHeight - GetTextHeight() ) / 2;

    Invalidate();
}

// -----------------------------------------------------------------------

void SvxShowText::SetText( const String& rText )
{
    Control::SetText( rText );
    Invalidate();
}

// -----------------------------------------------------------------------

SvxShowText::~SvxShowText()
{}

// class SvxCharacterMap =================================================

SvxCharMapData::SvxCharMapData( SfxModalDialog* pDialog, sal_Bool bOne_, ResMgr* pResContext )
:   mpDialog( pDialog ),
    aShowSet        ( pDialog, ResId( CT_SHOWSET, *pResContext ) ),
    aShowText       ( pDialog, ResId( CT_SHOWTEXT, *pResContext ) ),
    aOKBtn          ( pDialog, ResId( BTN_CHAR_OK, *pResContext ) ),
    aCancelBtn      ( pDialog, ResId( BTN_CHAR_CANCEL, *pResContext ) ),
    aHelpBtn        ( pDialog, ResId( BTN_CHAR_HELP, *pResContext ) ),
    aDeleteBtn      ( pDialog, ResId( BTN_DELETE, *pResContext ) ),
    aFontText       ( pDialog, ResId( FT_FONT, *pResContext ) ),
    aFontLB         ( pDialog, ResId( LB_FONT, *pResContext ) ),
    aSubsetText     ( pDialog, ResId( FT_SUBSET, *pResContext ) ),
    aSubsetLB       ( pDialog, ResId( LB_SUBSET, *pResContext ) ),
    aSymbolText     ( pDialog, ResId( FT_SYMBOLE, *pResContext ) ),
    aShowChar       ( pDialog, ResId( CT_SHOWCHAR, *pResContext ), sal_True ),
    aCharCodeText   ( pDialog, ResId( FT_CHARCODE, *pResContext ) ),
    bOne( bOne_ ),
    pSubsetMap( NULL )
{
    aFont = pDialog->GetFont();
    aFont.SetTransparent( sal_True );
    aFont.SetFamily( FAMILY_DONTKNOW );
    aFont.SetPitch( PITCH_DONTKNOW );
    aFont.SetCharSet( RTL_TEXTENCODING_DONTKNOW );

    if ( bOne )
    {
        Size aDlgSize = pDialog->GetSizePixel();
        pDialog->SetSizePixel( Size( aDlgSize.Width(),
            aDlgSize.Height()-aShowText.GetSizePixel().Height() ) );
        aSymbolText.Hide();
        aShowText.Hide();
        aDeleteBtn.Hide();
    }

    rtl::OUString aDefStr( aFont.GetName() );
    String aLastName;
    int nCount = mpDialog->GetDevFontCount();
    for ( int i = 0; i < nCount; i++ )
    {
        String aFontName( mpDialog->GetDevFont( i ).GetName() );
        if ( aFontName != aLastName )
        {
            aLastName = aFontName;
            sal_uInt16 nPos = aFontLB.InsertEntry( aFontName );
            aFontLB.SetEntryData( nPos, (void*)(sal_uLong)i );
        }
    }
    // the font may not be in the list =>
    // try to find a font name token in list and select found font,
    // else select topmost entry
    bool bFound = (aFontLB.GetEntryPos( aDefStr ) == LISTBOX_ENTRY_NOTFOUND );
    if( !bFound )
    {
        sal_Int32 nIndex = 0;
        do
        {
            rtl::OUString aToken = aDefStr.getToken(0, ';', nIndex);
            if ( aFontLB.GetEntryPos( aToken ) != LISTBOX_ENTRY_NOTFOUND )
            {
                aDefStr = aToken;
                bFound = sal_True;
                break;
            }
        }
        while ( nIndex >= 0 );
    }

    if ( bFound )
        aFontLB.SelectEntry( aDefStr );
    else if ( aFontLB.GetEntryCount() )
        aFontLB.SelectEntryPos(0);
    FontSelectHdl( &aFontLB );

    aOKBtn.SetClickHdl( LINK( this, SvxCharMapData, OKHdl ) );
    aFontLB.SetSelectHdl( LINK( this, SvxCharMapData, FontSelectHdl ) );
    aSubsetLB.SetSelectHdl( LINK( this, SvxCharMapData, SubsetSelectHdl ) );
    aShowSet.SetDoubleClickHdl( LINK( this, SvxCharMapData, CharDoubleClickHdl ) );
    aShowSet.SetSelectHdl( LINK( this, SvxCharMapData, CharSelectHdl ) );
    aShowSet.SetHighlightHdl( LINK( this, SvxCharMapData, CharHighlightHdl ) );
    aShowSet.SetPreSelectHdl( LINK( this, SvxCharMapData, CharPreSelectHdl ) );
    aDeleteBtn.SetClickHdl( LINK( this, SvxCharMapData, DeleteHdl ) );

    if( SvxShowCharSet::getSelectedChar() == ' ')
        aOKBtn.Disable();
    else
        aOKBtn.Enable();

    // left align aShowText field
    int nLeftEdge = aSymbolText.GetPosPixel().X();
    nLeftEdge += aSymbolText.GetTextWidth( aSymbolText.GetText() );
    aShowText.SetPosPixel( Point( nLeftEdge+4, aShowText.GetPosPixel().Y() ) );
}

// -----------------------------------------------------------------------

void SvxCharMapData::SetCharFont( const Font& rFont )
{
    // first get the underlying info in order to get font names
    // like "Times New Roman;Times" resolved
    Font aTmp( mpDialog->GetFontMetric( rFont ) );

    if ( aFontLB.GetEntryPos( aTmp.GetName() ) == LISTBOX_ENTRY_NOTFOUND )
        return;

    aFontLB.SelectEntry( aTmp.GetName() );
    aFont = aTmp;
    FontSelectHdl( &aFontLB );

    // for compatibility reasons
    mpDialog->ModalDialog::SetFont( aFont );
}

// -----------------------------------------------------------------------

IMPL_LINK_NOARG(SvxCharMapData, OKHdl)
{
    String aStr = aShowText.GetText();

    if ( !aStr.Len() )
    {
        sal_UCS4 cChar = aShowSet.GetSelectCharacter();
        // using the new UCS4 constructor
    rtl::OUString aOUStr( &cChar, 1 );
        aShowText.SetText( aOUStr );
    }
    mpDialog->EndDialog( sal_True );
    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK_NOARG(SvxCharMapData, FontSelectHdl)
{
    sal_uInt16 nPos = aFontLB.GetSelectEntryPos(),
        nFont = (sal_uInt16)(sal_uLong)aFontLB.GetEntryData( nPos );
    aFont = mpDialog->GetDevFont( nFont );
    aFont.SetWeight( WEIGHT_DONTKNOW );
    aFont.SetItalic( ITALIC_NONE );
    aFont.SetWidthType( WIDTH_DONTKNOW );
    aFont.SetPitch( PITCH_DONTKNOW );
    aFont.SetFamily( FAMILY_DONTKNOW );

    // notify children using this font
    aShowSet.SetFont( aFont );
    aShowChar.SetFont( aFont );
    aShowText.SetFont( aFont );

    // right align some fields to aShowSet
    int nRightEdge = aShowSet.GetPosPixel().X() + aShowSet.GetOutputSizePixel().Width();
    Size aNewSize = aSubsetLB.GetOutputSizePixel();
    aNewSize.setWidth( nRightEdge - aSubsetLB.GetPosPixel().X() );
    aSubsetLB.SetOutputSizePixel( aNewSize );

    // setup unicode subset listbar with font specific subsets,
    // hide unicode subset listbar for symbol fonts
    // TODO: get info from the Font once it provides it
    if( pSubsetMap)
        delete pSubsetMap;
    pSubsetMap = NULL;

    sal_Bool bNeedSubset = (aFont.GetCharSet() != RTL_TEXTENCODING_SYMBOL);
    if( bNeedSubset )
    {
        FontCharMap aFontCharMap;
        aShowSet.GetFontCharMap( aFontCharMap );
        pSubsetMap = new SubsetMap( &aFontCharMap );

        // update subset listbox for new font's unicode subsets
        aSubsetLB.Clear();
        // TODO: is it worth to improve the stupid linear search?
        bool bFirst = true;
        const Subset* s;
        while( NULL != (s = pSubsetMap->GetNextSubset( bFirst ))  )
        {
            sal_uInt16 nPos_ = aSubsetLB.InsertEntry( s->GetName() );
            aSubsetLB.SetEntryData( nPos_, (void*)s );
            // NOTE: subset must live at least as long as the selected font
            if( bFirst )
                aSubsetLB.SelectEntryPos( nPos_ );
            bFirst = false;
        }
        if( aSubsetLB.GetEntryCount() <= 1 )
            bNeedSubset = sal_False;
    }

    aSubsetText.Show( bNeedSubset);
    aSubsetLB.Show( bNeedSubset);

    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK_NOARG(SvxCharMapData, SubsetSelectHdl)
{
    sal_uInt16 nPos = aSubsetLB.GetSelectEntryPos();
    const Subset* pSubset = reinterpret_cast<const Subset*> (aSubsetLB.GetEntryData(nPos));
    if( pSubset )
    {
        sal_UCS4 cFirst = pSubset->GetRangeMin();
        aShowSet.SelectCharacter( cFirst );
    }
    aSubsetLB.SelectEntryPos( nPos );
    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK_NOARG(SvxCharMapData, CharDoubleClickHdl)
{
    mpDialog->EndDialog( sal_True );
    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK_NOARG(SvxCharMapData, CharSelectHdl)
{
    if ( !bOne )
    {
        String aText = aShowText.GetText();

        if ( aText.Len() == CHARMAP_MAXLEN )
            Sound::Beep( SOUND_WARNING );
        else
        {
            sal_UCS4 cChar = aShowSet.GetSelectCharacter();
            // using the new UCS4 constructor
            rtl::OUString aOUStr( &cChar, 1 );
            aShowText.SetText( aText + aOUStr );
        }

    }
    aOKBtn.Enable();
    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK_NOARG(SvxCharMapData, CharHighlightHdl)
{
    String aText;
    sal_UCS4 cChar = aShowSet.GetSelectCharacter();
    sal_Bool bSelect = (cChar > 0);

    // show char sample
    if ( bSelect )
    {
        // using the new UCS4 constructor
        aText = rtl::OUString( &cChar, 1 );

        const Subset* pSubset = NULL;
        if( pSubsetMap )
            pSubset = pSubsetMap->GetSubsetByUnicode( cChar );
        if( pSubset )
            aSubsetLB.SelectEntry( pSubset->GetName() );
        else
            aSubsetLB.SetNoSelection();
    }
    aShowChar.SetText( aText );
    aShowChar.Update();

    // show char code
    if ( bSelect )
    {
        char aBuf[32];
        snprintf( aBuf, sizeof(aBuf), "U+%04X", static_cast<unsigned>(cChar) );
        if( cChar < 0x0100 )
            snprintf( aBuf+6, sizeof(aBuf)-6, " (%u)", static_cast<unsigned>(cChar) );
        aText = String::CreateFromAscii( aBuf );
    }
    aCharCodeText.SetText( aText );

    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK_NOARG(SvxCharMapData, CharPreSelectHdl)
{
    // adjust subset selection
    if( pSubsetMap )
    {
        sal_UCS4 cChar = aShowSet.GetSelectCharacter();
        const Subset* pSubset = pSubsetMap->GetSubsetByUnicode( cChar );
        if( pSubset )
            aSubsetLB.SelectEntry( pSubset->GetName() );
    }

    aOKBtn.Enable();
    return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK_NOARG(SvxCharMapData, DeleteHdl)
{
    aShowText.SetText( String() );
    aOKBtn.Disable();
    return 0;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
