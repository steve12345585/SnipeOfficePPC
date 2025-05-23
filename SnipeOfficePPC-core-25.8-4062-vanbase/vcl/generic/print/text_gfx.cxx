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


#include <math.h>

#include "psputil.hxx"
#include "glyphset.hxx"

#include "generic/printergfx.hxx"
#include "vcl/fontmanager.hxx"
#include "vcl/helper.hxx"

#include "osl/thread.h"

#include "sal/alloca.h"

using namespace psp ;

namespace psp {
/*
 container for a font and its helper fonts:
 1st font is the font substitute e.g. helvetica substitutes arial on the printer
 2nd is the font itself
 3rd is a fallback font, usually a font with unicode glyph repertoir (e.g. andale)
 symbol fonts (adobe-fontspecific) may need special glyphmapping
 (symbol page vc. latin page)
*/
class Font3
{
    private:

        #define Font3Size 3

        fontID  mpFont [Font3Size];
        bool    mbSymbol;

    public:

        fontID  GetFont (int nIdx) const
                    { return nIdx < Font3Size ? mpFont[nIdx] : -1 ; }
        bool    IsSymbolFont () const
                    { return mbSymbol; }

        Font3 (const PrinterGfx &rGfx);
        ~Font3 () {}
};

Font3::Font3(const PrinterGfx &rGfx)
{
    mpFont[0] = rGfx.getFontSubstitute();
    mpFont[1] = rGfx.GetFontID();
    mpFont[2] = rGfx.getFallbackID();
    // mpFont[2] = rGfx.GetFontID();

       PrintFontManager &rMgr = PrintFontManager::get();
    mbSymbol = mpFont[1] != -1 ?
                rMgr.getFontEncoding(mpFont[1]) == RTL_TEXTENCODING_SYMBOL : false;
}

} // namespace psp

static int getVerticalDeltaAngle( sal_Unicode nChar )
{
    int nRotation = GetVerticalFlags(nChar);
    if (nRotation == GF_ROTR)
        return -900;
    if (nRotation == GF_ROTL)
        return 900;
    return 0;
}

void
PrinterGfx::PSUploadPS1Font (sal_Int32 nFontID)
{
    std::list< sal_Int32 >::iterator aFont;
    // already in the document header ?
    for (aFont = maPS1Font.begin(); aFont != maPS1Font.end(); ++aFont )
        if( nFontID == *aFont )
            return;

    // no occurrenc yet, mark for download
    // add the fontid to the list
    maPS1Font.push_back (nFontID);
}

/*
 * implement text handling printer routines,
 */

sal_uInt16
PrinterGfx::SetFont(
                    sal_Int32 nFontID,
                    sal_Int32 nHeight,
                    sal_Int32 nWidth,
                    sal_Int32 nAngle,
                    bool bVertical,
                    bool bArtItalic,
                    bool bArtBold
                    )
{
    // font and encoding will be set by drawText again immediately
    // before PSShowText
    mnFontID                          = nFontID;
    maVirtualStatus.maFont            = rtl::OString();
    maVirtualStatus.maEncoding        = RTL_TEXTENCODING_DONTKNOW;
    maVirtualStatus.mnTextHeight      = nHeight;
    maVirtualStatus.mnTextWidth       = nWidth;
    maVirtualStatus.mbArtItalic       = bArtItalic;
    maVirtualStatus.mbArtBold         = bArtBold;
    mnTextAngle                       = nAngle;
    mbTextVertical                    = bVertical;

    return 0;
}

void PrinterGfx::drawGlyphs(
                            const Point& rPoint,
                            sal_uInt32* pGlyphIds,
                            sal_Unicode* pUnicodes,
                            sal_Int16 nLen,
                            sal_Int32* pDeltaArray
                            )
{

    // draw the string
    // search for a glyph set matching the set font
    std::list< GlyphSet >::iterator aIter;
    for (aIter = maPS3Font.begin(); aIter != maPS3Font.end(); ++aIter)
        if ( ((*aIter).GetFontID()  == mnFontID)
             && ((*aIter).IsVertical() == mbTextVertical))
        {
            (*aIter).DrawGlyphs (*this, rPoint, pGlyphIds, pUnicodes, nLen, pDeltaArray);
            break;
        }

    // not found ? create a new one
    if (aIter == maPS3Font.end())
    {
        maPS3Font.push_back (GlyphSet(mnFontID, mbTextVertical));
        maPS3Font.back().DrawGlyphs (*this, rPoint, pGlyphIds, pUnicodes, nLen, pDeltaArray);
    }
}

void PrinterGfx::DrawGlyphs(
                            const Point& rPoint,
                            sal_GlyphId* pGlyphIds,
                            sal_Unicode* pUnicodes,
                            sal_Int16 nLen,
                            sal_Int32* pDeltaArray
                            )
{
    if( nLen <= 0 )
        return;

    if ( !mrFontMgr.isFontDownloadingAllowed( mnFontID ) )
    {
        LicenseWarning(rPoint, pUnicodes, nLen, pDeltaArray);
        return;
    }

    if( mrFontMgr.getFontType( mnFontID ) != fonttype::TrueType )
    {
        DrawText( rPoint, pUnicodes, nLen, pDeltaArray );
        return;
    }

    // move and rotate the user coordinate system
    // avoid the gsave/grestore for the simple cases since it allows
    // reuse of the current font if it hasn't changed
    sal_Int32 nCurrentTextAngle = mnTextAngle;
    Point aPoint( rPoint );

    if (nCurrentTextAngle != 0)
    {
        PSGSave ();
        PSTranslate (rPoint);
        PSRotate (nCurrentTextAngle);
        mnTextAngle = 0;
        aPoint = Point( 0, 0 );
    }

    if( mbTextVertical )
    {
        // vertical glyphs can have an additional rotation ... sigh.
        // so break up text in chunks of normal glyphs and print out
        // specially rotated glyphs extra
        sal_uInt32* pTempGlyphIds = (sal_uInt32*)alloca(sizeof(sal_Int32)*nLen);
        sal_Int32* pTempDelta = (sal_Int32*)alloca(sizeof(sal_Int32)*nLen);
        sal_Unicode* pTempUnicodes = (sal_Unicode*)alloca(sizeof(sal_Unicode)*nLen);
        sal_Int16 nTempLen = 0;
        sal_Int32 nTempFirstDelta = 0;
        Point aRotPoint;
        sal_Int32 nTextHeight = maVirtualStatus.mnTextHeight;
        sal_Int32 nTextWidth  = maVirtualStatus.mnTextWidth ? maVirtualStatus.mnTextWidth : maVirtualStatus.mnTextHeight;
        sal_Int32 nAscend = mrFontMgr.getFontAscend( mnFontID );
        sal_Int32 nDescend = mrFontMgr.getFontDescend( mnFontID );

        nDescend = nDescend * nTextHeight / 1000;
        nAscend = nAscend * nTextHeight / 1000;

        for( sal_Int16 i = 0; i < nLen; i++ )
        {
            const sal_GlyphId nRot = pGlyphIds[i] & GF_ROTMASK;
            if( nRot == GF_NONE )
            {
                pTempUnicodes[nTempLen] = pUnicodes[i];
                pTempGlyphIds[nTempLen] = pGlyphIds[i];
                if( nTempLen > 0 )
                    pTempDelta[nTempLen-1]  = pDeltaArray[i-1]-nTempFirstDelta;
                else
                {
                    // the first element in pDeltaArray shows
                    // the offset of the second character
                    // so if the first glyph is normal
                    // then we do not need to move the delta indices
                    // else we have to move them down by one and
                    // recalculate aPoint and all deltas
                    if( i != 0 )
                        nTempFirstDelta = pDeltaArray[ i-1 ];
                }
                nTempLen++;
            }
            else
            {
                sal_Int32 nOffset = i > 0 ? pDeltaArray[i-1] : 0;
                sal_Int32 nRotAngle = 0;
                switch( nRot )
                {
                    case GF_ROTR:
                        nRotAngle = 2700;
                        aRotPoint = Point( -nAscend*nTextWidth/nTextHeight, -nDescend*nTextWidth/nTextHeight - nOffset );
                        break;
                    case GF_VERT:
                        nRotAngle = 1800;
                        aRotPoint = Point( -nOffset, (nAscend+nDescend) );
                        break;
                    case GF_ROTL:
                        nRotAngle = 900;
                        aRotPoint = Point( -nDescend*nTextWidth/nTextHeight, nOffset + nAscend*nTextWidth/nTextHeight );
                        break;
                }
                sal_GlyphId nRotGlyphId     = pGlyphIds[i];
                sal_Unicode nRotUnicode     = pUnicodes[i];
                sal_Int32 nRotDelta         = 0;

                // transform matrix to new individual direction
                PSGSave ();
                GraphicsStatus aSaveStatus = maVirtualStatus;
                if( nRot != 2 ) // switch font aspect
                {
                    maVirtualStatus.mnTextWidth = nTextHeight;
                    maVirtualStatus.mnTextHeight = nTextWidth;
                }
                if( aPoint.X() || aPoint.Y() )
                    PSTranslate( aPoint );
                PSRotate (nRotAngle);
                // draw the rotated glyph
                drawGlyphs( aRotPoint, &nRotGlyphId, &nRotUnicode, 1, &nRotDelta );

                // restore previous state
                maVirtualStatus = aSaveStatus;
                PSGRestore();
            }
        }

        pGlyphIds = pTempGlyphIds;
        pUnicodes = pTempUnicodes;
        pDeltaArray = pTempDelta;
        nLen = nTempLen;

        aPoint.X() += nTempFirstDelta;
    }

    if( nLen > 0 )
        drawGlyphs( aPoint, pGlyphIds, pUnicodes, nLen, pDeltaArray );

    // restore the user coordinate system
    if (nCurrentTextAngle != 0)
    {
        PSGRestore ();
        mnTextAngle = nCurrentTextAngle;
    }
}

void
PrinterGfx::DrawText (
                      const Point& rPoint,
                      const sal_Unicode* pStr,
                      sal_Int16 nLen,
                      const sal_Int32* pDeltaArray
                      )
{
    fontID nRestoreFont = mnFontID;

    // setup font[substitutes] and map the string into the symbol area in case of
    // symbol font
    Font3 aFont(*this);
    sal_Unicode *pEffectiveStr;
    if ( aFont.IsSymbolFont() )
    {
        pEffectiveStr = (sal_Unicode*)alloca(nLen * sizeof(pStr[0]));
        for (int i = 0; i < nLen; i++)
            pEffectiveStr[i] = pStr[i] < 256 ? pStr[i] + 0xF000 : pStr[i];
    }
    else
    {
        pEffectiveStr = const_cast<sal_Unicode*>(pStr);
    }

    fontID    *pFontMap   = (fontID*)    alloca(nLen * sizeof(fontID));
    sal_Int32 *pCharWidth = (sal_Int32*) alloca(nLen * sizeof(sal_Int32));

    for( int n = 0; n < nLen; n++ )
    {
        CharacterMetric aBBox;
        pFontMap[n]   = getCharMetric (aFont, pEffectiveStr[n], &aBBox);
        pCharWidth[n] = getCharWidth  (mbTextVertical, pEffectiveStr[n], &aBBox);
    }

    // setup a new delta array, use virtual resolution of 1000
    sal_Int32* pNewDeltaArray = (sal_Int32*)alloca( sizeof( sal_Int32 )*nLen );
    if ( pDeltaArray != 0)
    {
        for (int i = 0; i < nLen - 1; i++)
            pNewDeltaArray[i] = 1000 * pDeltaArray[i];
        pNewDeltaArray[nLen - 1] = 0;
    }
    else
    {
        pNewDeltaArray[0] = pCharWidth[0];
        for (int i = 1; i < nLen; i++)
            pNewDeltaArray[i] = pNewDeltaArray[i-1] + pCharWidth[i];
    }

    // move and rotate the user coordinate system
    // avoid the gsave/grestore for the simple cases since it allows
    // reuse of the current font if it hasn't changed
    sal_Int32 nCurrentTextAngle = mnTextAngle;
    sal_Int32 nCurrentPointX;
    sal_Int32 nCurrentPointY;

    if (nCurrentTextAngle != 0)
    {
        PSGSave ();
        PSTranslate (rPoint);
        PSRotate (nCurrentTextAngle);
        mnTextAngle = 0;

        nCurrentPointX = 0;
        nCurrentPointY = 0;
    }
    else
    {
        nCurrentPointX = rPoint.X();
        nCurrentPointY = rPoint.Y();
    }

    // draw the string
    sal_Int32 nDelta = 0;
    for (int nTo = 0; nTo < nLen; )
    {
        int    nFrom = nTo;
        fontID nFont = pFontMap[ nFrom ];

        while ((nTo < nLen) && (nFont == pFontMap[nTo]))
        {
            pNewDeltaArray[ nTo ] = (sal_Int32)(((0.5 + pNewDeltaArray[ nTo ]) / 1000.0) - nDelta);
            nTo++ ;
        }

        SetFont( nFont,
                 maVirtualStatus.mnTextHeight, maVirtualStatus.mnTextWidth,
                 mnTextAngle,
                 mbTextVertical,
                 maVirtualStatus.mbArtItalic,
                 maVirtualStatus.mbArtBold
                 );

        if (mbTextVertical)
        {
            drawVerticalizedText(
                    Point(nCurrentPointX + nDelta, nCurrentPointY),
                    pEffectiveStr + nFrom, nTo - nFrom,
                    pNewDeltaArray + nFrom );
        }
        else
        {
            drawText(
                    Point(nCurrentPointX + nDelta, nCurrentPointY),
                    pEffectiveStr + nFrom, nTo - nFrom,
                    pDeltaArray == NULL ? NULL : pNewDeltaArray + nFrom );
        }
        nDelta += pNewDeltaArray[ nTo - 1 ];
    }

    // restore the user coordinate system
    if (nCurrentTextAngle != 0)
    {
        PSGRestore ();
        mnTextAngle = nCurrentTextAngle;
    }

    // restore the original font settings
    SetFont( nRestoreFont,
             maVirtualStatus.mnTextHeight, maVirtualStatus.mnTextWidth,
             mnTextAngle, mbTextVertical,
             maVirtualStatus.mbArtItalic,
             maVirtualStatus.mbArtBold
             );
}

void PrinterGfx::drawVerticalizedText(
                                      const Point& rPoint,
                                      const sal_Unicode* pStr,
                                      sal_Int16 nLen,
                                      const sal_Int32* pDeltaArray
                                      )
{
    sal_Int32* pDelta = (sal_Int32*)alloca( nLen * sizeof(sal_Int32) );

    int nTextScale   = maVirtualStatus.mnTextWidth ? maVirtualStatus.mnTextWidth : maVirtualStatus.mnTextHeight;
    int nNormalAngle = mnTextAngle;
    int nDeltaAngle, nLastPos = 0;

    double fSin = sin( -2.0*M_PI*nNormalAngle/3600 );
    double fCos = cos( -2.0*M_PI*nNormalAngle/3600 );

    PrintFontManager &rMgr = PrintFontManager::get();
    PrintFontInfo aInfo;
    rMgr.getFontInfo( mnFontID, aInfo );

    bool* pGsubFlags = (bool*)alloca( nLen * sizeof(bool) );
    rMgr.hasVerticalSubstitutions( mnFontID, pStr, nLen, pGsubFlags );

    Point aPoint( rPoint );
    for( int i = 0; i < nLen; )
    {
        while( ( nDeltaAngle = getVerticalDeltaAngle( pStr[i] ) ) == 0 && i < nLen )
            i++;
        if( i <= nLen && i > nLastPos )
        {
            for( int n = nLastPos; n < i; n++ )
                pDelta[n] = pDeltaArray[n] - (aPoint.X() - rPoint.X() );

            SetFont( mnFontID,
                     maVirtualStatus.mnTextHeight, maVirtualStatus.mnTextWidth,
                     nNormalAngle, mbTextVertical,
                     maVirtualStatus.mbArtItalic,
                     maVirtualStatus.mbArtBold );
            drawText( aPoint, pStr + nLastPos, i - nLastPos, pDelta + nLastPos );

            aPoint.X() = (sal_Int32)(rPoint.X() + ((double)pDeltaArray[i-1] * fCos));
            aPoint.Y() = (sal_Int32)(rPoint.Y() + ((double)pDeltaArray[i-1] * fSin));
        }
        if( i < nLen )
        {
            int nOldWidth   = maVirtualStatus.mnTextWidth;
            int nOldHeight  = maVirtualStatus.mnTextHeight;
            SetFont( mnFontID,
                     nTextScale,
                     maVirtualStatus.mnTextHeight,
                     nNormalAngle + nDeltaAngle,
                     mbTextVertical,
                     maVirtualStatus.mbArtItalic,
                     maVirtualStatus.mbArtBold );

            double nA = nTextScale * aInfo.m_nAscend / 1000.0;
            double nD = nTextScale * aInfo.m_nDescend / 1000.0;
            double fStretch = (double)maVirtualStatus.mnTextWidth / maVirtualStatus.mnTextHeight;
            if( !pGsubFlags[i] )
                nD *= fStretch;

            Point aPos( aPoint );
            switch( nDeltaAngle )
            {
                case +900:
                    aPos.X() += (sal_Int32)(+nA * fCos + nD * fSin);
                    aPos.Y() += (sal_Int32)(-nA * fSin + nD * fCos);
                    break;
                case -900:
                    aPos.X() += (sal_Int32)(+nA * fSin + nD * fCos);
                    aPos.Y() += (sal_Int32)(-(nTextScale*fStretch - nD) * fCos);
                    break;
            }
            drawText( aPos, pStr+i, 1, NULL );
            if( i < nLen-1 && pDeltaArray )
            {
                aPoint.X() = (sal_Int32)(rPoint.X() + ((double)pDeltaArray[i] * fCos));
                aPoint.Y() = (sal_Int32)(rPoint.Y() + ((double)pDeltaArray[i] * fSin));
            }

            // swap text width/height again
            SetFont( mnFontID,
                     nOldHeight,
                     nOldWidth,
                     nNormalAngle,
                     mbTextVertical,
                     maVirtualStatus.mbArtItalic,
                     maVirtualStatus.mbArtBold );
        }
        i++;
        nLastPos = i;
    }
    mnTextAngle = nNormalAngle;
}

void
PrinterGfx::LicenseWarning(const Point& rPoint, const sal_Unicode* pStr,
                           sal_Int16 nLen, const sal_Int32* pDeltaArray)
{
    // treat it like a builtin font in case a user has that font also in the
    // printer. This is not so unlikely as it may seem; no print embedding
    // licensed fonts are often used (or so they say) in companies:
    // they are installed on displays and printers, but get not embedded in
    // they are installed on displays and printers, but get not embedded in
    // print files or documents because they are not licensed for use outside
    // the company.
    rtl::OString aMessage( "The font " );
    aMessage += rtl::OUStringToOString( mrFontMgr.getPSName(mnFontID),
            RTL_TEXTENCODING_ASCII_US );
    aMessage += " could not be downloaded\nbecause its license does not allow for that";
    PSComment( aMessage.getStr() );

    rtl::OString aFontName = rtl::OUStringToOString(
            mrFontMgr.getPSName(mnFontID),
            RTL_TEXTENCODING_ASCII_US);
    PSSetFont (aFontName, RTL_TEXTENCODING_ISO_8859_1);

    sal_Size  nSize    = 4 * nLen;
    sal_uChar* pBuffer = (sal_uChar*)alloca (nSize* sizeof(sal_uChar));

    ConverterFactory &rCvt = GetConverterFactory ();
    nSize = rCvt.Convert (pStr, nLen, pBuffer, nSize, RTL_TEXTENCODING_ISO_8859_1);

    PSMoveTo (rPoint);
    PSShowText (pBuffer, nLen, nSize, pDeltaArray);
}

void
PrinterGfx::drawText(
                     const Point& rPoint,
                     const sal_Unicode* pStr,
                     sal_Int16 nLen,
                     const sal_Int32* pDeltaArray
                     )
{
    if (!(nLen > 0))
        return;

    fonttype::type   eType          = mrFontMgr.getFontType (mnFontID);

    if (eType == fonttype::Type1)
        PSUploadPS1Font (mnFontID);

    if (   eType == fonttype::TrueType
        && !mrFontMgr.isFontDownloadingAllowed(mnFontID))
    {
        LicenseWarning(rPoint, pStr, nLen, pDeltaArray);
        return;
    }

    if( mrFontMgr.getUseOnlyFontEncoding( mnFontID ) )
    {
        GlyphSet aGSet( mnFontID, mbTextVertical );
        aGSet.DrawText( *this, rPoint, pStr, nLen, pDeltaArray );
        return;
    }

    // search for a glyph set matching the set font
    std::list< GlyphSet >::iterator aIter;
    for (aIter = maPS3Font.begin(); aIter != maPS3Font.end(); ++aIter)
        if (   ((*aIter).GetFontID()  == mnFontID)
            && ((*aIter).IsVertical() == mbTextVertical))
        {
            (*aIter).DrawText (*this, rPoint, pStr, nLen, pDeltaArray);
            break;
        }

    // not found ? create a new one
    if (aIter == maPS3Font.end())
    {
        maPS3Font.push_back (GlyphSet(mnFontID, mbTextVertical));
        maPS3Font.back().DrawText (*this, rPoint, pStr, nLen, pDeltaArray);
    }
}

int
PrinterGfx::getCharWidth (sal_Bool b_vert, sal_Unicode n_char, CharacterMetric *p_bbox)
{
    b_vert = b_vert && (getVerticalDeltaAngle(n_char) != 0);
    int w = b_vert ? p_bbox->height : p_bbox->width;
    w *= maVirtualStatus.mnTextWidth ? maVirtualStatus.mnTextWidth : maVirtualStatus.mnTextHeight;
    return w;
}

fontID
PrinterGfx::getCharMetric (const Font3 &rFont, sal_Unicode n_char, CharacterMetric *p_bbox)
{
    p_bbox->width  = -1;
    p_bbox->height = -1;

    for (fontID n = 0; n < 3; n++)
    {
        fontID n_font = rFont.GetFont(n);
        if (n_font != -1)
            mrFontMgr.getMetrics( n_font, n_char, n_char, p_bbox );
        if (p_bbox->width >= 0 && p_bbox->height >= 0)
            return n_font;
    }
    if (n_char != '?')
        return getCharMetric (rFont, '?', p_bbox);

    return rFont.GetFont(0) != -1 ? rFont.GetFont(0) : rFont.GetFont(1);
}

fontID
PrinterGfx::getFontSubstitute () const
{
    if( mpFontSubstitutes )
    {
        ::boost::unordered_map< fontID, fontID >::const_iterator it =
              mpFontSubstitutes->find( mnFontID );
        if( it != mpFontSubstitutes->end() )
            return it->second;
    }

    return -1;
}

sal_Int32
PrinterGfx::GetCharWidth (sal_Unicode nFrom, sal_Unicode nTo, long *pWidthArray)
{
    Font3 aFont(*this);
    if (aFont.IsSymbolFont() && (nFrom < 256) && (nTo < 256))
    {
        nFrom += 0xF000;
        nTo   += 0xF000;
    }

    for( int n = 0; n < (nTo - nFrom + 1); n++ )
    {
        CharacterMetric aBBox;
        getCharMetric (aFont, n + nFrom, &aBBox);
        pWidthArray[n] = getCharWidth (mbTextVertical, n + nFrom, &aBBox);
    }

    // returned metrics have postscript precision
    return 1000;
}

const ::std::list< KernPair >& PrinterGfx::getKernPairs( bool bVertical ) const
{
    /*
     *  Note: this is only a 80% solution: if a font is only
     *  partially substituted in a string due to missing glyphs
     *  the results may not be perfect; the more so the more the
     *  substitution differs from the original metricwise. But
     *  vcl only asks for KernPairs for each font once and NOT
     *  in a string context this is the best we can do.
     *  In future the kerning should be done on a per string basis.
     */
    fontID nFont = mnFontID;
    if( mpFontSubstitutes )
    {
        ::boost::unordered_map< fontID, fontID >::const_iterator it =
              mpFontSubstitutes->find( mnFontID );
        if( it != mpFontSubstitutes->end() )
            nFont = it->second;
    }
    return mrFontMgr.getKernPairs( nFont, bVertical );
}

/*
 * spool the converted truetype fonts to the page header after the page body is
 * complete
 * for Type1 fonts spool additional reencoding vectors that are necessary to access the
 * whole font
 */

void
PrinterGfx::OnEndPage ()
{
}

void
PrinterGfx::OnEndJob ()
{
    maPS3Font.clear();
    maPS1Font.clear();
}

void
PrinterGfx::writeResources( osl::File* pFile, std::list< rtl::OString >& rSuppliedFonts, std::list< rtl::OString >& rNeededFonts )
{
    // write all type 1 fonts
    std::list< sal_Int32 >::iterator aFont;
    // already in the document header ?
    for (aFont = maPS1Font.begin(); aFont != maPS1Font.end(); ++aFont)
    {
        const rtl::OString& rSysPath (mrFontMgr.getFontFileSysPath(*aFont) );
        rtl::OUString aUNCPath;
        osl::File::getFileURLFromSystemPath (OStringToOUString (rSysPath, osl_getThreadTextEncoding()), aUNCPath);
        osl::File aFontFile (aUNCPath);

        // provide the pfb or pfa font as a (pfa-)font resource
        rtl::OString aPostScriptName =
            rtl::OUStringToOString ( mrFontMgr.getPSName(*aFont),
                                     RTL_TEXTENCODING_ASCII_US );

        WritePS (pFile, "%%BeginResource: font ");
        WritePS (pFile, aPostScriptName.getStr());
        WritePS (pFile, "\n");

        osl::File::RC nError = aFontFile.open(osl_File_OpenFlag_Read);
        if (nError == osl::File::E_None)
        {
            convertPfbToPfa (aFontFile, *pFile);
            aFontFile.close ();

            char lastchar = '\n';

            if (pFile->setPos(osl_Pos_Current, -1) == osl::FileBase::E_None)
            {
                sal_uInt64 uBytes(1);
                pFile->read((void *)(&lastchar), uBytes, uBytes);
            }

            if (lastchar != '\n')
                WritePS (pFile, "\n");
        }
        WritePS (pFile, "%%EndResource\n");
        rSuppliedFonts.push_back( aPostScriptName );
    }

    // write glyphsets and reencodings
    std::list< GlyphSet >::iterator aIter;
    for (aIter = maPS3Font.begin(); aIter != maPS3Font.end(); ++aIter)
    {
        if (aIter->GetFontType() == fonttype::TrueType)
        {
            aIter->PSUploadFont (*pFile, *this, mbUploadPS42Fonts ? true : false, rSuppliedFonts );
        }
        else
        // (   aIter->GetFontType() == fonttype::Type1
        //  || aIter->GetFontType() == fonttype::Builtin )
        {
            aIter->PSUploadEncoding (pFile, *this);
            if( aIter->GetFontType() == fonttype::Builtin )
                rNeededFonts.push_back(
                      rtl::OUStringToOString(
                           mrFontMgr.getPSName( aIter->GetFontID() ),
                           RTL_TEXTENCODING_ASCII_US ) );
        }
    }
}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
