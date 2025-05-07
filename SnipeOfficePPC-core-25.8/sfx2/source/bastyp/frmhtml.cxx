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


#include <svtools/htmltokn.h>
#include <svtools/asynclink.hxx>

#include <sfx2/sfx.hrc>

#include <sfx2/frmhtml.hxx>
#include <sfx2/docfile.hxx>
#include <sfx2/viewfrm.hxx>
#include <sfx2/evntconf.hxx>
#include <sfx2/request.hxx>
#include <sfx2/fcontnr.hxx>
#include "sfxtypes.hxx"

static sal_Char const sHTML_SC_yes[] =  "YES";
static sal_Char const sHTML_SC_no[] =       "NO";
static sal_Char const sHTML_SC_auto[] = "AUTO";

#define HTML_O_READONLY "READONLY"
#define HTML_O_EDIT     "EDIT"

static HTMLOptionEnum const aScollingTable[] =
{
    { sHTML_SC_yes,     ScrollingYes    },
    { sHTML_SC_no,      ScrollingNo     },
    { sHTML_SC_auto,    ScrollingAuto   },
    { 0,                0               }
};

void SfxFrameHTMLParser::ParseFrameOptions(
    SfxFrameDescriptor *pFrame, const HTMLOptions& rOptions, const String& rBaseURL )
{
    // Get and set the options
    Size aMargin( pFrame->GetMargin() );

    // Netscape seems to set marginwidth to 0 as soon as
    // marginheight is set, and vice versa.
    // Netscape does however not allow for a direct
    // seting to 0, while IE4.0 does
    // We will not mimic that bug !
    sal_Bool bMarginWidth = sal_False, bMarginHeight = sal_False;

    for (size_t i = 0, n = rOptions.size(); i < n; ++i)
    {
        const HTMLOption& aOption = rOptions[i];
        switch( aOption.GetToken() )
        {
        case HTML_O_BORDERCOLOR:
            {
                Color aColor;
                aOption.GetColor( aColor );
                pFrame->SetWallpaper( Wallpaper( aColor ) );
                break;
            }
        case HTML_O_SRC:
            pFrame->SetURL(
                String(
                    INetURLObject::GetAbsURL(
                        rBaseURL, aOption.GetString())) );
            break;
        case HTML_O_NAME:
            pFrame->SetName( aOption.GetString() );
            break;
        case HTML_O_MARGINWIDTH:
            aMargin.Width() = aOption.GetNumber();

            if( !bMarginHeight )
                aMargin.Height() = 0;
            bMarginWidth = sal_True;
            break;
        case HTML_O_MARGINHEIGHT:
            aMargin.Height() = aOption.GetNumber();

            if( !bMarginWidth )
                aMargin.Width() = 0;
            bMarginHeight = sal_True;
            break;
        case HTML_O_SCROLLING:
            pFrame->SetScrollingMode(
                (ScrollingMode)aOption.GetEnum( aScollingTable,
                                                 ScrollingAuto ) );
            break;
        case HTML_O_FRAMEBORDER:
        {
            String aStr = aOption.GetString();
            sal_Bool bBorder = sal_True;
            if ( aStr.EqualsIgnoreCaseAscii("NO") ||
                 aStr.EqualsIgnoreCaseAscii("0") )
                bBorder = sal_False;
            pFrame->SetFrameBorder( bBorder );
            break;
        }
        case HTML_O_NORESIZE:
            pFrame->SetResizable( sal_False );
            break;
        default:
            if ( aOption.GetTokenString().EqualsIgnoreCaseAscii(
                                                        HTML_O_READONLY ) )
            {
                String aStr = aOption.GetString();
                sal_Bool bReadonly = sal_True;
                if ( aStr.EqualsIgnoreCaseAscii("FALSE") )
                    bReadonly = sal_False;
                pFrame->SetReadOnly( bReadonly );
            }
            else if ( aOption.GetTokenString().EqualsIgnoreCaseAscii(
                                                        HTML_O_EDIT ) )
            {
                String aStr = aOption.GetString();
                sal_Bool bEdit = sal_True;
                if ( aStr.EqualsIgnoreCaseAscii("FALSE") )
                    bEdit = sal_False;
                pFrame->SetEditable( bEdit );
            }

            break;
        }
    }

    pFrame->SetMargin( aMargin );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
