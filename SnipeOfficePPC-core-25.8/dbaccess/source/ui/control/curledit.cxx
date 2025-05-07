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


#include "curledit.hxx"
#include <vcl/svapp.hxx>
#include <osl/diagnose.h>

//.........................................................................
namespace dbaui
{
//.........................................................................
    DBG_NAME(OConnectionURLEdit)
//=========================================================================
//= OConnectionURLEdit
//=========================================================================
OConnectionURLEdit::OConnectionURLEdit(Window* _pParent, const ResId& _rResId,sal_Bool _bShowPrefix)
    :Edit(_pParent, _rResId)
    ,m_pTypeCollection(NULL)
    ,m_pForcedPrefix(NULL)
    ,m_bShowPrefix(_bShowPrefix)
{
    DBG_CTOR(OConnectionURLEdit ,NULL);
}

//-------------------------------------------------------------------------
OConnectionURLEdit::~OConnectionURLEdit()
{
    DBG_DTOR(OConnectionURLEdit ,NULL);
    // delete my sub controls
    Edit* pSubEdit = GetSubEdit();
    SetSubEdit(NULL);
    delete pSubEdit;
    delete m_pForcedPrefix;
}

//-------------------------------------------------------------------------
void OConnectionURLEdit::SetTextNoPrefix(const String& _rText)
{
    OSL_ENSURE(GetSubEdit(), "OConnectionURLEdit::SetTextNoPrefix: have no current type, not changing the text!");
    if (GetSubEdit())
        GetSubEdit()->SetText(_rText);
}

//-------------------------------------------------------------------------
String OConnectionURLEdit::GetTextNoPrefix() const
{
    if (GetSubEdit())
        return GetSubEdit()->GetText();
    return GetText();
}

//-------------------------------------------------------------------------
void OConnectionURLEdit::SetText(const String& _rStr)
{
    Selection aNoSelection(0,0);
    SetText(_rStr, aNoSelection);
}

//-------------------------------------------------------------------------
void OConnectionURLEdit::SetText(const String& _rStr, const Selection& /*_rNewSelection*/)
{
    // create new sub controls, if necessary
    if (!GetSubEdit())
        SetSubEdit(new Edit(this, 0));
    if ( !m_pForcedPrefix )
    {
        m_pForcedPrefix = new FixedText(this, WB_VCENTER);

        // we use a gray background for the fixed text
        StyleSettings aSystemStyle = Application::GetSettings().GetStyleSettings();
        m_pForcedPrefix->SetBackground(Wallpaper(aSystemStyle.GetDialogColor()));
    }

    m_pForcedPrefix->Show(m_bShowPrefix);

    sal_Bool bIsEmpty = 0 == _rStr.Len();
    // calc the prefix
    String sPrefix;
    if (!bIsEmpty)
    {
        // determine the type of the new URL described by the new text
        sPrefix = m_pTypeCollection->getPrefix(_rStr);
    }

    // the fixed text gets the prefix
    if ( m_pForcedPrefix )
        m_pForcedPrefix->SetText(sPrefix);

    // both subs have to be resized according to the text len of the prefix
    Size aMySize = GetSizePixel();
    sal_Int32 nTextWidth = 0;
    if ( m_pForcedPrefix && m_bShowPrefix)
    {
        nTextWidth = m_pForcedPrefix->GetTextWidth(sPrefix) + 2;
        m_pForcedPrefix->SetPosSizePixel(Point(0, -2), Size(nTextWidth, aMySize.Height()));
    }
    GetSubEdit()->SetPosSizePixel(Point(nTextWidth, -2), Size(aMySize.Width() - nTextWidth - 4, aMySize.Height()));
        // -2 because the edit has a frame which is 2 pixel wide ... should not be necessary, but I don't fully understand this ....

    // show the sub controls (in case they were just created)
    GetSubEdit()->Show();

    // do the real SetTex
//  Edit::SetText(bIsEmpty ? _rStr : m_pTypeCollection->cutPrefix(_rStr), _rNewSelection);
    String sNewText( _rStr );
    if ( !bIsEmpty )
        sNewText  = m_pTypeCollection->cutPrefix( _rStr );
    Edit::SetText( sNewText );
}

//-------------------------------------------------------------------------
String OConnectionURLEdit::GetText() const
{
    if ( m_pForcedPrefix )
        return m_pForcedPrefix->GetText() += Edit::GetText();
    return Edit::GetText();
}
// -----------------------------------------------------------------------------
void OConnectionURLEdit::ShowPrefix(sal_Bool _bShowPrefix)
{
    m_bShowPrefix = _bShowPrefix;
    if ( m_pForcedPrefix )
        m_pForcedPrefix->Show(m_bShowPrefix);
}
//.........................................................................
}   // namespace dbaui
//.........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
