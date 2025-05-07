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

#ifndef _SV_CURSOR_HXX
#define _SV_CURSOR_HXX

#include <tools/gen.hxx>
#include <tools/link.hxx>
#include <vcl/sv.h>
#include <vcl/dllapi.h>

class AutoTimer;
struct ImplCursorData;
class Window;

// -----------------
// - Cursor-Styles -
// -----------------

#define CURSOR_SHADOW                   ((sal_uInt16)0x0001)
#define CURSOR_DIRECTION_NONE           ((unsigned char)0x00)
#define CURSOR_DIRECTION_LTR            ((unsigned char)0x01)
#define CURSOR_DIRECTION_RTL            ((unsigned char)0x02)

// ----------
// - Cursor -
// ----------

class VCL_DLLPUBLIC Cursor
{
private:
    ImplCursorData* mpData;             // Interne Daten
    Window*         mpWindow;           // Window (only for shadow cursor)
    long            mnSlant;            // Schraegstellung
    long            mnOffsetY;          // Offset fuer Rotation
    Size            maSize;             // Groesse
    Point           maPos;              // Position
    short           mnOrientation;      // Rotation
    sal_uInt16          mnStyle;            // Style
    bool            mbVisible;          // Ist Cursor sichtbar
    unsigned char   mnDirection;        // indicates direction

public:
    SAL_DLLPRIVATE void         ImplDraw();
    SAL_DLLPRIVATE void         ImplRestore();
    DECL_DLLPRIVATE_LINK(       ImplTimerHdl, void* );
    SAL_DLLPRIVATE void         ImplShow( bool bDrawDirect = true );
    SAL_DLLPRIVATE void         ImplHide();
    SAL_DLLPRIVATE void         ImplResume( bool bRestore = false );
    SAL_DLLPRIVATE bool         ImplSuspend();
    SAL_DLLPRIVATE void         ImplNew();

public:
                    Cursor();
                    Cursor( const Cursor& rCursor );
                    ~Cursor();

    void            SetStyle( sal_uInt16 nStyle );
    sal_uInt16          GetStyle() const { return mnStyle; }

    void            Show();
    void            Hide();
    bool            IsVisible() const { return mbVisible; }

    void            SetWindow( Window* pWindow );
    Window*         GetWindow() const { return mpWindow; }

    void            SetPos( const Point& rNewPos );
    const Point&    GetPos() const { return maPos; }
    long            GetOffsetY() const { return mnOffsetY; }

    void            SetSize( const Size& rNewSize );
    const Size&     GetSize() const { return maSize; }
    void            SetWidth( long nNewWidth );
    long            GetWidth() const { return maSize.Width(); }
    long            GetHeight() const { return maSize.Height(); }

    long            GetSlant() const { return mnSlant; }

    void            SetOrientation( short nOrientation = 0 );
    short           GetOrientation() const { return mnOrientation; }

    void            SetDirection( unsigned char nDirection = 0 );
    unsigned char   GetDirection() const { return mnDirection; }

    Cursor&         operator=( const Cursor& rCursor );
    bool            operator==( const Cursor& rCursor ) const;
    bool            operator!=( const Cursor& rCursor ) const
                        { return !(Cursor::operator==( rCursor )); }

private:
    void ImplDoShow( bool bDrawDirect, bool bRestore );
    bool ImplDoHide( bool bStop );
};

#endif  // _SV_CURSOR_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
