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

#include "TableWindowTitle.hxx"
#include "TableWindow.hxx"
#include "QueryTableView.hxx"
#include <vcl/svapp.hxx>
#include <vcl/help.hxx>
#include <vcl/menu.hxx>
#include <tools/debug.hxx>
#include "dbustrings.hrc"
#include <sfx2/cntids.hrc>
#include "TableWindowListBox.hxx"
#include "TableConnection.hxx"
#include "dbu_qry.hrc"
#include "QueryDesignView.hxx"
#include "JoinController.hxx"

#include <algorithm>

using namespace dbaui;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::uno;
//==================================================================
// class OTableWindowTitle
//==================================================================
DBG_NAME(OTableWindowTitle)
//------------------------------------------------------------------------------
OTableWindowTitle::OTableWindowTitle( OTableWindow* pParent ) :
     FixedText( pParent, WB_3DLOOK|WB_LEFT|WB_NOLABEL|WB_VCENTER )
    ,m_pTabWin( pParent )
{
    DBG_CTOR(OTableWindowTitle,NULL);
    // Hintergrund- und Textfarbe setzen
    StyleSettings aSystemStyle = Application::GetSettings().GetStyleSettings();
    SetBackground(Wallpaper(Color(aSystemStyle.GetFaceColor())));
    SetTextColor(aSystemStyle.GetButtonTextColor());

    Font aFont( GetFont() );
    aFont.SetTransparent( sal_True );
    SetFont( aFont );
}

//------------------------------------------------------------------------------
OTableWindowTitle::~OTableWindowTitle()
{
    DBG_DTOR(OTableWindowTitle,NULL);
    m_pTabWin = NULL;
}

//------------------------------------------------------------------------------
void OTableWindowTitle::GetFocus()
{
    if(m_pTabWin)
        m_pTabWin->GetFocus();
}

//------------------------------------------------------------------------------
void OTableWindowTitle::LoseFocus()
{
    m_pTabWin->LoseFocus();
}
//------------------------------------------------------------------------------
void OTableWindowTitle::RequestHelp( const HelpEvent& rHEvt )
{
    if(m_pTabWin)
    {
        String aHelpText = m_pTabWin->GetComposedName();
        if( aHelpText.Len())
        {
            // Hilfe anzeigen
            Rectangle aItemRect(Point(0,0),GetSizePixel());
            aItemRect = LogicToPixel( aItemRect );
            Point aPt = OutputToScreenPixel( aItemRect.TopLeft() );
            aItemRect.Left()   = aPt.X();
            aItemRect.Top()    = aPt.Y();
            aPt = OutputToScreenPixel( aItemRect.BottomRight() );
            aItemRect.Right()  = aPt.X();
            aItemRect.Bottom() = aPt.Y();
            if( rHEvt.GetMode() == HELPMODE_BALLOON )
                Help::ShowBalloon( this, aItemRect.Center(), aItemRect, aHelpText);
            else
                Help::ShowQuickHelp( this, aItemRect, aHelpText );
        }
    }
}

//------------------------------------------------------------------------------
void OTableWindowTitle::Command( const CommandEvent& rEvt )
{
    switch( rEvt.GetCommand() )
    {
        case COMMAND_CONTEXTMENU:
        {
            GrabFocus();
            if ( m_pTabWin )
                m_pTabWin->Command( rEvt );
            else
                Control::Command(rEvt);
        }
    }
}

//------------------------------------------------------------------------------
void OTableWindowTitle::KeyInput( const KeyEvent& rEvt )
{
    if ( m_pTabWin )
        m_pTabWin->KeyInput( rEvt );
}

//------------------------------------------------------------------------------
void OTableWindowTitle::MouseButtonDown( const MouseEvent& rEvt )
{
    if( rEvt.IsLeft() )
    {
        if( rEvt.GetClicks() == 2)
        {
            Size aSize(GetTextWidth(GetText()) + 20,
                        m_pTabWin->GetSizePixel().Height() - m_pTabWin->GetListBox()->GetSizePixel().Height());

            aSize.Height() += (m_pTabWin->GetListBox()->GetEntryCount() + 2) * m_pTabWin->GetListBox()->GetEntryHeight();
            if(m_pTabWin->GetSizePixel() != aSize)
            {
                m_pTabWin->SetSizePixel(aSize);

                OJoinTableView* pView = static_cast<OJoinTableView*>(m_pTabWin->getTableView());
                OSL_ENSURE(pView,"No OJoinTableView!");
                const ::std::vector<OTableConnection*>* pConns = pView->getTableConnections();
                ::std::for_each(pConns->begin(),
                                pConns->end(),
                                ::std::mem_fun(&OTableConnection::RecalcLines));

                pView->InvalidateConnections();
                pView->getDesignView()->getController().setModified(sal_True);
                pView->Invalidate(INVALIDATE_NOCHILDREN);
            }
        }
        else
        {
            Point aPos = rEvt.GetPosPixel();
            aPos = OutputToScreenPixel( aPos );
            OJoinTableView* pView = static_cast<OJoinTableView*>(m_pTabWin->getTableView());
            OSL_ENSURE(pView,"No OJoinTableView!");
            pView->NotifyTitleClicked( static_cast<OTableWindow*>(GetParent()), aPos );
        }
        GrabFocus();
    }
    else
        Control::MouseButtonDown( rEvt );
}


//------------------------------------------------------------------------------
void OTableWindowTitle::DataChanged(const DataChangedEvent& rDCEvt)
{
    if (rDCEvt.GetType() == DATACHANGED_SETTINGS)
    {
        // nehmen wir den worst-case an : die Farben haben sich geaendert, also
        // mich anpassen
        StyleSettings aSystemStyle = Application::GetSettings().GetStyleSettings();
        SetBackground(Wallpaper(Color(aSystemStyle.GetFaceColor())));
        SetTextColor(aSystemStyle.GetButtonTextColor());
    }
}
// -----------------------------------------------------------------------------
void OTableWindowTitle::StateChanged( StateChangedType nType )
{
    Window::StateChanged( nType );

    if ( nType == STATE_CHANGE_ZOOM )
    {
        const StyleSettings& rStyleSettings = GetSettings().GetStyleSettings();

        Font aFont = rStyleSettings.GetGroupFont();
        if ( IsControlFont() )
            aFont.Merge( GetControlFont() );
        SetZoomedPointFont( aFont );

        Resize();
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
