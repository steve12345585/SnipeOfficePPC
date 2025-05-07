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

#ifndef _GLOSBIB_HXX
#define _GLOSBIB_HXX

#include <vcl/edit.hxx>
#include <svx/stddlg.hxx>

#include <vcl/lstbox.hxx>
#include <svtools/svtabbx.hxx>

#include <vcl/button.hxx>
#include <vcl/fixed.hxx>

#include <vector>

class SwGlossaryHdl;

class FEdit : public Edit
{
    public:
        FEdit(Window * pParent, const ResId& rResId) :
            Edit(pParent, rResId){}

    virtual void KeyInput( const KeyEvent& rKEvent );
};

struct GlosBibUserData
{
    String sPath;
    String sGroupName;
    String sGroupTitle;
};

class SwGlossaryGroupTLB : public SvTabListBox
{
public:
    SwGlossaryGroupTLB(Window* pParent, const ResId& rResId) :
        SvTabListBox(pParent, rResId) {}

    virtual void    RequestHelp( const HelpEvent& rHEvt );
};

class SwGlossaryGroupDlg : public SvxStandardDialog
{
    FixedText           aBibFT;
    FEdit               aNameED;
    FixedText           aPathFT;
    ListBox             aPathLB;
    FixedText           aSelectFT;
    SwGlossaryGroupTLB  aGroupTLB;

    OKButton        aOkPB;
    CancelButton    aCancelPB;
    HelpButton      aHelpPB;
    PushButton      aNewPB;
    PushButton      aDelPB;
    PushButton      aRenamePB;

    typedef std::vector< ::rtl::OUString> OUVector_t;
    OUVector_t m_RemovedArr;
    OUVector_t m_InsertedArr;
    OUVector_t m_RenamedArr;

    SwGlossaryHdl   *pGlosHdl;

    String          sCreatedGroup;

    sal_Bool            IsDeleteAllowed(const String &rGroup);

protected:
    virtual void Apply();
    DECL_LINK( SelectHdl, SvTabListBox* );
    DECL_LINK(NewHdl, void *);
    DECL_LINK( DeleteHdl, Button*  );
    DECL_LINK(ModifyHdl, void *);
    DECL_LINK(RenameHdl, void *);

public:
    SwGlossaryGroupDlg(Window * pParent,
                        std::vector<String> const& rPathArr,
                        SwGlossaryHdl *pGlosHdl);
    ~SwGlossaryGroupDlg();

    const String&       GetCreatedGroupName() const {return sCreatedGroup;}
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
