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

#ifndef SC_NAMEDLG_HXX
#define SC_NAMEDLG_HXX

#include <vcl/morebtn.hxx>
#include <vcl/combobox.hxx>
#include <vcl/group.hxx>
#include <vcl/fixed.hxx>
#include <vcl/lstbox.hxx>

#include "rangenam.hxx"
#include "anyrefdg.hxx"
#include "namemgrtable.hxx"

#include <boost/ptr_container/ptr_map.hpp>
#include <boost/ptr_container/ptr_set.hpp>

#include <stack>
#include <map>

class ScViewData;
class ScDocument;


//==================================================================

//logic behind the manage names dialog
class ScNameDlg : public ScAnyRefDlg
{
private:
    Edit            maEdName;
    formula::RefEdit        maEdAssign;
    formula::RefButton      aRbAssign;
    ListBox         maLbScope;

    DisclosureButton maBtnMore;
    CheckBox        maBtnPrintArea;
    CheckBox        maBtnColHeader;
    CheckBox        maBtnCriteria;
    CheckBox        maBtnRowHeader;

    PushButton      maBtnAdd;
    PushButton      maBtnDelete;
    HelpButton      maBtnHelp;
    PushButton      maBtnOk;
    PushButton      maBtnCancel;

    FixedText       maFtScope;
    FixedText       maFtRange;
    FixedText       maFtName;
    FixedLine       maFlDiv;
    FixedText       maFtInfo;

    ScRangeManagerCtrl maNameMgrCtrl;
    ScRangeManagerTable* mpRangeManagerTable;

    const rtl::OUString maGlobalNameStr;
    const rtl::OUString maErrInvalidNameStr;
    const rtl::OUString maErrNameInUse;
    const rtl::OUString maStrInfoDefault;
    const rtl::OUString maStrMultiSelect;

    ScViewData*     mpViewData;
    ScDocument*     mpDoc;
    const ScAddress maCursorPos;
    Selection       maCurSel;

    bool mbNeedUpdate;
    bool mbDataChanged;
    //ugly hack to call DefineNames from ManageNames
    bool mbCloseWithoutUndo;

    typedef boost::ptr_map<rtl::OUString, ScRangeName> RangeNameContainer;

    RangeNameContainer maRangeMap;

private:
    void Init();
    void UpdateChecks(ScRangeData* pData);
    void ShowOptions(const ScRangeNameLine& rLine);
    void UpdateNames();

    bool IsNameValid();
    bool IsFormulaValid();
    void CheckForEmptyTable();

    ScRangeName* GetRangeName(const rtl::OUString& rScope);

    bool AddPushed();
    void RemovePushed();
    void OKPushed();
    void CancelPushed();
    void NameSelected();
    void ScopeChanged();
    void NameModified();
    void MorePushed();

    void SelectionChanged();

    // Handler:
    DECL_LINK( OkBtnHdl, void * );
    DECL_LINK( CancelBtnHdl, void * );
    DECL_LINK( AddBtnHdl, void * );
    DECL_LINK( RemoveBtnHdl, void * );
    DECL_LINK( SelectBtnHdl, void * );
    DECL_LINK( EdModifyHdl, void * );
    DECL_LINK( AssignGetFocusHdl, void * );
    DECL_LINK( SelectionChangedHdl_Impl, void* );
    DECL_LINK( ScopeChangedHdl, void* );
    DECL_LINK( MoreBtnHdl, void* );

protected:
    virtual void    RefInputDone( sal_Bool bForced = sal_False );

public:
                    ScNameDlg( SfxBindings* pB, SfxChildWindow* pCW, Window* pParent,
                               ScViewData*      ptrViewData,
                               const ScAddress& aCursorPos, boost::ptr_map<rtl::OUString, ScRangeName>* pRangeMap = NULL );
    virtual         ~ScNameDlg();

    virtual void    SetReference( const ScRange& rRef, ScDocument* pDoc );
    virtual sal_Bool    IsRefInputMode() const;

    virtual void    SetActive();
    virtual sal_Bool    Close();

    void GetRangeNames(boost::ptr_map<rtl::OUString, ScRangeName>& rRangeMap);
    void SetEntry(const rtl::OUString& rName, const rtl::OUString& rScope);

};



#endif // SC_NAMEDLG_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
