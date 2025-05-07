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

#ifndef _PAD_HELPER_HXX_
#define _PAD_HELPER_HXX_

#include <list>
#include <tools/string.hxx>
#include <tools/resid.hxx>
#include <vcl/dialog.hxx>
#include <vcl/button.hxx>
#include <vcl/edit.hxx>
#include <vcl/fixed.hxx>
#include <vcl/lstbox.hxx>
#include <vcl/combobox.hxx>

#if defined SPA_DLLIMPLEMENTATION
#define SPA_DLLPUBLIC SAL_DLLPUBLIC_EXPORT
#else
#define SPA_DLLPUBLIC SAL_DLLPUBLIC_IMPORT
#endif


class Config;

#define PSPRINT_PPDDIR "driver"

namespace padmin
{
class DelMultiListBox : public MultiListBox
{
    Link            m_aDelPressedLink;
public:
    DelMultiListBox( Window* pParent, const ResId& rResId ) :
            MultiListBox( pParent, rResId ) {}
    ~DelMultiListBox() {}

    virtual long Notify( NotifyEvent& rEvent );

    Link setDelPressedLink( const Link& rLink )
    {
        Link aOldLink( m_aDelPressedLink );
                m_aDelPressedLink = rLink;
                return aOldLink;
    }
    const Link& getDelPressedLink() const { return m_aDelPressedLink; }
};

class DelListBox : public ListBox
{
    Link            m_aDelPressedLink;
public:
    DelListBox( Window* pParent, const ResId& rResId ) :
                ListBox( pParent, rResId ) {}
    ~DelListBox() {}

    virtual long Notify( NotifyEvent& rEvent );

    Link setDelPressedLink( const Link& rLink )
    {
        Link aOldLink( m_aDelPressedLink );
        m_aDelPressedLink = rLink;
        return aOldLink;
    }
    const Link& getDelPressedLink() const { return m_aDelPressedLink; }
};

class QueryString : public ModalDialog
{
private:
    OKButton     m_aOKButton;
    CancelButton m_aCancelButton;
    FixedText    m_aFixedText;
    Edit         m_aEdit;
    ComboBox     m_aComboBox;

    String&      m_rReturnValue;
    bool         m_bUseEdit;

    DECL_LINK( ClickBtnHdl, Button* );

public:
    QueryString( Window*, String &, String &, const ::std::list< String >& rChoices = ::std::list<String>() );
    // parent window, Query text, initial value
    ~QueryString();
};

sal_Bool AreYouSure( Window*, int nRid = -1 );

ResId PaResId( sal_uInt32 nId );

void FindFiles( const String& rDirectory, ::std::list< String >& rResult, const String& rSuffixes, bool bRecursive = false );

Config& getPadminRC();
void freePadminRC();

bool chooseDirectory( String& rInOutPath );

} // namespace padmin

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
