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

#include "privsplt.hxx"

/*************************************************************************
#*  Member:     ScPrivatSplit
#*------------------------------------------------------------------------
#*
#*  Klasse:     MD_Test
#*
#*  Funktion:   Konstruktor der Klasse ScPrivatSplit
#*
#*  Input:      ---
#*
#*  Output:     ---
#*
#************************************************************************/

ScPrivatSplit::ScPrivatSplit( Window* pParent, const ResId& rResId,
                             SC_SPLIT_DIRECTION eSplit):
                        Control( pParent, rResId )
{
    Point aPos=GetPosPixel();
    nOldX=(short)aPos.X();
    nOldY=(short)aPos.Y();
    nNewX=(short)aPos.X();
    nNewY=(short)aPos.Y();
    eScSplit=eSplit;
    aXMovingRange.Min()=nNewX;
    aXMovingRange.Max()=nNewX;
    aYMovingRange.Min()=nNewY;
    aYMovingRange.Max()=nNewY;

    aWinPointer=GetPointer();

    aMovingFlag=false;
    if(eScSplit==SC_SPLIT_HORZ)
    {
        aWinPointer=Pointer(POINTER_HSPLIT);
    }
    else
    {
        aWinPointer=Pointer(POINTER_VSPLIT);
    }
    SetPointer(aWinPointer);
}


/*************************************************************************
#*  Member:     MouseButtonDown
#*------------------------------------------------------------------------
#*
#*  Klasse:     ScPrivatSplit
#*
#*  Funktion:   Reagiert auf einen einzelnen Mouse-Event. Nach Aufruf
#*              werden alle Mauseingaben an dieses Control weitergeleitet.
#*
#*  Input:      ---
#*
#*  Output:     ---
#*
#************************************************************************/

void ScPrivatSplit::MouseButtonDown( const MouseEvent& rMEvt )
{
    Point aPos=LogicToPixel(rMEvt.GetPosPixel());

    nOldX=(short)aPos.X();
    nOldY=(short)aPos.Y();

    CaptureMouse();
}

/*************************************************************************
#*  Member:     MouseButtonUp
#*------------------------------------------------------------------------
#*
#*  Klasse:     ScPrivatSplit
#*
#*  Funktion:   Ende einer Benutzeraktion mit der Maus. Es werden
#*              die aktuelle Maus- Koordinaten ermittelt und fuer
#*              die Verschiebung des Fensters verwendet.
#*
#*  Input:      ---
#*
#*  Output:     ---
#*
#************************************************************************/

void ScPrivatSplit::MouseButtonUp( const MouseEvent& rMEvt )
{
    ReleaseMouse();

    Point aPos=LogicToPixel(rMEvt.GetPosPixel());
    Point a2Pos=GetPosPixel();
    Point a3Pos=a2Pos;

    if(eScSplit==SC_SPLIT_HORZ)
    {
        nNewX=(short)aPos.X();
        nDeltaX=nNewX-nOldX;
        a2Pos.X()+=nDeltaX;
        if(a2Pos.X()<aXMovingRange.Min())
        {
            nDeltaX=(short)(aXMovingRange.Min()-a3Pos.X());
            a2Pos.X()=aXMovingRange.Min();
        }
        else if(a2Pos.X()>aXMovingRange.Max())
        {
            nDeltaX=(short)(aXMovingRange.Max()-a3Pos.X());
            a2Pos.X()=aXMovingRange.Max();
        }
    }
    else
    {
        nNewY=(short)aPos.Y();
        nDeltaY=nNewY-nOldY;
        a2Pos.Y()+=nDeltaY;
        if(a2Pos.Y()<aYMovingRange.Min())
        {
            nDeltaY=(short)(aYMovingRange.Min()-a3Pos.Y());
            a2Pos.Y()=aYMovingRange.Min();
        }
        else if(a2Pos.Y()>aYMovingRange.Max())
        {
            nDeltaY=(short)(aYMovingRange.Max()-a3Pos.Y());
            a2Pos.Y()=aYMovingRange.Max();
        }
    }
    SetPosPixel(a2Pos);
    Invalidate();
    Update();
    CtrModified();
}

/*************************************************************************
#*  Member:     MouseMove
#*------------------------------------------------------------------------
#*
#*  Klasse:     ScPrivatSplit
#*
#*  Funktion:   Reagiert kontinuierlich auf Mausbewegungen. Es werden
#*              die aktuelle Maus- Koordinaten ermittelt und fuer
#*              die Verschiebung des Fensters verwendet.
#*
#*  Input:      ---
#*
#*  Output:     ---
#*
#************************************************************************/

void ScPrivatSplit::MouseMove( const MouseEvent& rMEvt )
{
    Point aPos=LogicToPixel(rMEvt.GetPosPixel());
    Point a2Pos=GetPosPixel();
    Point a3Pos=a2Pos;
    if(rMEvt.IsLeft())
    {
        if(eScSplit==SC_SPLIT_HORZ)
        {
            nNewX=(short)aPos.X();
            nDeltaX=nNewX-nOldX;
            a2Pos.X()+=nDeltaX;

            if(a2Pos.X()<aXMovingRange.Min())
            {
                nDeltaX=(short)(aXMovingRange.Min()-a3Pos.X());
                a2Pos.X()=aXMovingRange.Min();
            }
            else if(a2Pos.X()>aXMovingRange.Max())
            {
                nDeltaX=(short)(aXMovingRange.Max()-a3Pos.X());
                a2Pos.X()=aXMovingRange.Max();
            }
        }
        else
        {
            nNewY=(short)aPos.Y();
            nDeltaY=nNewY-nOldY;
            a2Pos.Y()+=nDeltaY;
            if(a2Pos.Y()<aYMovingRange.Min())
            {
                nDeltaY=(short)(aYMovingRange.Min()-a3Pos.Y());
                a2Pos.Y()=aYMovingRange.Min();
            }
            else if(a2Pos.Y()>aYMovingRange.Max())
            {
                nDeltaY=(short)(aYMovingRange.Max()-a3Pos.Y());
                a2Pos.Y()=aYMovingRange.Max();
            }
        }

        SetPosPixel(a2Pos);

        CtrModified();
        Invalidate();
        Update();
    }
}

/*************************************************************************
#*  Member:     SetYRange
#*------------------------------------------------------------------------
#*
#*  Klasse:     ScPrivatSplit
#*
#*  Funktion:   Setzt den Range fuer die Y- Verschiebung
#*
#*  Input:      neuer Bereich
#*
#*  Output:     ---
#*
#************************************************************************/
void ScPrivatSplit::SetYRange(Range cRgeY)
{
    aYMovingRange=cRgeY;
}



/*************************************************************************
#*  Member:     GetDeltaY
#*------------------------------------------------------------------------
#*
#*  Klasse:     ScPrivatSplit
#*
#*  Funktion:   Liefert die relative x-Verschiebung zurueck
#*
#*  Input:      ---
#*
#*  Output:     ---
#*
#************************************************************************/
short ScPrivatSplit::GetDeltaX()
{
    return nDeltaX;
}

/*************************************************************************
#*  Member:     GetDeltaY
#*------------------------------------------------------------------------
#*
#*  Klasse:     ScPrivatSplit
#*
#*  Funktion:   Liefert die relative y-Verschiebung zurueck
#*
#*  Input:      ---
#*
#*  Output:     ---
#*
#************************************************************************/
short ScPrivatSplit::GetDeltaY()
{
    return nDeltaY;
}

/*************************************************************************
#*  Member:     CtrModified
#*------------------------------------------------------------------------
#*
#*  Klasse:     ScPrivatSplit
#*
#*  Funktion:   Teilt einem installierten Handler mit, dass
#*              eine Veraenderung eingetreten ist.
#*
#*  Input:      ---
#*
#*  Output:     ---
#*
#************************************************************************/
void ScPrivatSplit::CtrModified()
{
    aCtrModifiedLink.Call( this );
}

void ScPrivatSplit::MoveSplitTo(Point aPos)
{
    Point a2Pos=GetPosPixel();
    nOldX=(short)a2Pos.X();
    nOldY=(short)a2Pos.Y();
    Point a3Pos=a2Pos;

    if(eScSplit==SC_SPLIT_HORZ)
    {
        nNewX=(short)aPos.X();
        nDeltaX=nNewX-nOldX;
        a2Pos.X()+=nDeltaX;
        if(a2Pos.X()<aXMovingRange.Min())
        {
            nDeltaX=(short)(aXMovingRange.Min()-a3Pos.X());
            a2Pos.X()=aXMovingRange.Min();
        }
        else if(a2Pos.X()>aXMovingRange.Max())
        {
            nDeltaX=(short)(aXMovingRange.Max()-a3Pos.X());
            a2Pos.X()=aXMovingRange.Max();
        }
    }
    else
    {
        nNewY=(short)aPos.Y();
        nDeltaY=nNewY-nOldY;
        a2Pos.Y()+=nDeltaY;
        if(a2Pos.Y()<aYMovingRange.Min())
        {
            nDeltaY=(short)(aYMovingRange.Min()-a3Pos.Y());
            a2Pos.Y()=aYMovingRange.Min();
        }
        else if(a2Pos.Y()>aYMovingRange.Max())
        {
            nDeltaY=(short)(aYMovingRange.Max()-a3Pos.Y());
            a2Pos.Y()=aYMovingRange.Max();
        }
    }
    SetPosPixel(a2Pos);
    Invalidate();
    Update();
    CtrModified();
}


void ScPrivatSplit::ImplInitSettings( sal_Bool bFont, sal_Bool bForeground, sal_Bool bBackground )
{
    const StyleSettings& rStyleSettings = GetSettings().GetStyleSettings();

    if ( bFont )
    {
        Font aFont = rStyleSettings.GetAppFont();
        if ( IsControlFont() )
            aFont.Merge( GetControlFont() );
        SetFont( aFont );
    }

    if ( bFont || bForeground )
    {
        Color aTextColor = rStyleSettings.GetButtonTextColor();
        if ( IsControlForeground() )
            aTextColor = GetControlForeground();
        SetTextColor( aTextColor );
    }

    if ( bBackground )
    {
        SetBackground( rStyleSettings.GetFaceColor());
    }
    if ( IsBackground() )
    {
        SetFillColor( GetBackground().GetColor() );
        SetBackground();
    }
    Invalidate();
}

// -----------------------------------------------------------------------

void ScPrivatSplit::StateChanged( StateChangedType nType )
{
    if ( (nType == STATE_CHANGE_ZOOM) ||
         (nType == STATE_CHANGE_CONTROLFONT) )
    {
        ImplInitSettings( sal_True, false, false );
        Invalidate();
    }
    if ( nType == STATE_CHANGE_CONTROLFOREGROUND )
    {
        ImplInitSettings( false, sal_True, false );
        Invalidate();
    }
    else if ( nType == STATE_CHANGE_CONTROLBACKGROUND )
    {
        ImplInitSettings( false, false, sal_True );
        Invalidate();
    }

    Control::StateChanged( nType );
}

// -----------------------------------------------------------------------

void ScPrivatSplit::DataChanged( const DataChangedEvent& rDCEvt )
{
    if ( (rDCEvt.GetType() == DATACHANGED_SETTINGS) &&
         (rDCEvt.GetFlags() & SETTINGS_STYLE) )
    {
        ImplInitSettings( sal_True, sal_True, sal_True );
        Invalidate();
    }
    else
        Window::DataChanged( rDCEvt );
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
