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
#ifndef _SVX_SRCHDLG_HXX
#define _SVX_SRCHDLG_HXX

#include <svtools/stdctrl.hxx>
#include <vcl/combobox.hxx>
#include <vcl/edit.hxx>
#include <vcl/lstbox.hxx>
#include <vcl/button.hxx>
#include <vcl/group.hxx>
#include <vcl/fixed.hxx>
#include <vcl/dialog.hxx>
#include <sfx2/childwin.hxx>
#include <sfx2/basedlgs.hxx>
#include <svl/svarray.hxx>
#include <svl/svstdarr.hxx>
#include <svtools/svmedit.hxx>
#include <svl/srchdefs.hxx>
#include "svx/svxdllapi.h"

class SvxSearchItem;
class MoreButton;
class SfxStyleSheetBasePool;
class SvxJSearchOptionsPage;
class SvxSearchController;

struct SearchDlg_Impl;

#ifndef NO_SVX_SEARCH

// struct SearchAttrItem -------------------------------------------------

struct SearchAttrItem
{
    sal_uInt16          nSlot;
    SfxPoolItem*    pItem;
};

// class SearchAttrItemList ----------------------------------------------

SV_DECL_VARARR_VISIBILITY(SrchAttrItemList, SearchAttrItem, 8, SVX_DLLPUBLIC)

class SVX_DLLPUBLIC SearchAttrItemList : private SrchAttrItemList
{
public:
    SearchAttrItemList() {}
    SearchAttrItemList( const SearchAttrItemList& rList );
    ~SearchAttrItemList();

    void            Put( const SfxItemSet& rSet );
    SfxItemSet&     Get( SfxItemSet& rSet );
    void            Clear();
    sal_uInt16          Count() const { return SrchAttrItemList::Count(); }
    SearchAttrItem& operator[](sal_uInt16 nPos) const
                        { return SrchAttrItemList::operator[]( nPos ); }
    SearchAttrItem& GetObject( sal_uInt16 nPos ) const
                        { return SrchAttrItemList::GetObject( nPos ); }

    // the pointer to the item is not being copierd, so don't delete
    void Insert( const SearchAttrItem& rItem )
        { SrchAttrItemList::Insert( rItem, SrchAttrItemList::Count() ); }
    // deletes the pointer to the items
    void Remove( sal_uInt16 nPos, sal_uInt16 nLen = 1 );
};

#ifndef SV_NODIALOG

// class SvxSearchDialogWrapper ------------------------------------------

class SvxSearchDialog;
class SVX_DLLPUBLIC SvxSearchDialogWrapper : public SfxChildWindow
{
    SvxSearchDialog *dialog;
public:
    SvxSearchDialogWrapper( Window*pParent, sal_uInt16 nId,
                            SfxBindings* pBindings, SfxChildWinInfo* pInfo );

    ~SvxSearchDialogWrapper ();
    SvxSearchDialog *getDialog ();
    SFX_DECL_CHILDWINDOW(SvxSearchDialogWrapper);
};

// class SvxSearchDialog -------------------------------------------------

/*
    {k:\svx\prototyp\dialog\svx/srchdlg.hxx}

    [Description]
    In this modeless dialog the attributes for a search are configured
    and a search is started from it. Several search types
    (search, search all, replace, replace all) are possible.

    [Items]
    <SvxSearchItem><SID_ATTR_SEARCH>
*/

class SvxSearchDialog : public SfxModelessDialog
{
friend class SvxSearchController;
friend class SvxSearchDialogWrapper;
friend class SvxJSearchOptionsDialog;

public:
    SvxSearchDialog( Window* pParent, SfxChildWindow* pChildWin, SfxBindings& rBind );
    ~SvxSearchDialog();

    virtual sal_Bool    Close();

    // Window
    virtual void    Activate();

    const SearchAttrItemList*   GetSearchItemList() const
                                    { return pSearchList; }
    const SearchAttrItemList*   GetReplaceItemList() const
                                    { return pReplaceList; }

    inline sal_Bool     HasSearchAttributes() const;
    inline sal_Bool     HasReplaceAttributes() const;

    PushButton&     GetReplaceBtn() { return aReplaceBtn; }

    sal_Int32           GetTransliterationFlags() const;

    void            SetSaveToModule(bool b);

private:
    FixedText       aSearchText;
    ComboBox        aSearchLB;
    ListBox         aSearchTmplLB;
    FixedInfo       aSearchAttrText;

    FixedText       aReplaceText;
    ComboBox        aReplaceLB;
    ListBox         aReplaceTmplLB;
    FixedInfo       aReplaceAttrText;

    PushButton      aSearchBtn;
    PushButton      aSearchAllBtn;
    FixedLine       aSearchCmdLine;
    PushButton      aReplaceBtn;
    PushButton      aReplaceAllBtn;

    FixedLine       aSearchComponentFL;
    PushButton      aSearchComponent1PB;
    PushButton      aSearchComponent2PB;

    CheckBox        aMatchCaseCB;
    CheckBox        aWordBtn;

    FixedLine       aButtonsFL;
    MoreButton*     pMoreBtn;
    HelpButton      aHelpBtn;
    CancelButton    aCloseBtn;

    FixedLine       aOptionsFL;
    CheckBox        aSelectionBtn;
    CheckBox        aBackwardsBtn;
    CheckBox        aRegExpBtn;
    CheckBox        aSimilarityBox;
    PushButton      aSimilarityBtn;
    CheckBox        aLayoutBtn;
    CheckBox        aNotesBtn;
    CheckBox        aJapMatchFullHalfWidthCB;
    CheckBox        aJapOptionsCB;
    PushButton      aJapOptionsBtn;

    PushButton      aAttributeBtn;
    PushButton      aFormatBtn;
    PushButton      aNoFormatBtn;

    FixedLine       aCalcFL;
    FixedText       aCalcSearchInFT;
    ListBox         aCalcSearchInLB;
    FixedText       aCalcSearchDirFT;
    RadioButton     aRowsBtn;
    RadioButton     aColumnsBtn;
    CheckBox        aAllSheetsCB;

    SfxBindings&    rBindings;
    sal_Bool            bWriter;
    sal_Bool            bSearch;
    sal_Bool            bFormat;
    sal_uInt16          nOptions;
    bool            bSet;
    bool            bReadOnly;
    bool            bConstruct;
    sal_uIntPtr         nModifyFlag;
    String          aStylesStr;
    String          aLayoutStr;
    String aCalcStr;

    std::vector<rtl::OUString> aSearchStrings;
    std::vector<rtl::OUString> aReplaceStrings;

    SearchDlg_Impl*         pImpl;
    SearchAttrItemList*     pSearchList;
    SearchAttrItemList*     pReplaceList;
    SvxSearchItem*          pSearchItem;

    SvxSearchController*    pSearchController;
    SvxSearchController*    pOptionsController;
    SvxSearchController*    pFamilyController;
    SvxSearchController*    pSearchSetController;
    SvxSearchController*    pReplaceSetController;

    mutable sal_Int32           nTransliterationFlags;

#ifdef _SVX_SRCHDLG_CXX
    DECL_LINK( ModifyHdl_Impl, ComboBox* pEdit );
    DECL_LINK( FlagHdl_Impl, Control* pCtrl );
    DECL_LINK( CommandHdl_Impl, Button* pBtn );
    DECL_LINK(TemplateHdl_Impl, void *);
    DECL_LINK( FocusHdl_Impl, Control* );
    DECL_LINK(LoseFocusHdl_Impl, void *);
    DECL_LINK(FormatHdl_Impl, void *);
    DECL_LINK(NoFormatHdl_Impl, void *);
    DECL_LINK(AttributeHdl_Impl, void *);
    DECL_LINK( TimeoutHdl_Impl, Timer* );

    void            Construct_Impl();
    void            InitControls_Impl();
    void            CalculateDelta_Impl();
    void            Init_Impl( int bHasItemSet );
    void            InitAttrList_Impl( const SfxItemSet* pSSet,
                                       const SfxItemSet* pRSet );
    void            Remember_Impl( const String &rStr,sal_Bool bSearch );
    void            PaintAttrText_Impl();
    String&         BuildAttrText_Impl( String& rStr, sal_Bool bSrchFlag ) const;

    void            TemplatesChanged_Impl( SfxStyleSheetBasePool& rPool );
    void            EnableControls_Impl( const sal_uInt16 nFlags );
    void            EnableControl_Impl( Control* pCtrl );
    void            SetItem_Impl( const SvxSearchItem* pItem );

    void            SetModifyFlag_Impl( const Control* pCtrl );
    void            SaveToModule_Impl();

    void            ApplyTransliterationFlags_Impl( sal_Int32 nSettings );
#endif
};

inline sal_Bool SvxSearchDialog::HasSearchAttributes() const
{
    int bLen = aSearchAttrText.GetText().Len();
    return ( aSearchAttrText.IsEnabled() && bLen );
}

inline sal_Bool SvxSearchDialog::HasReplaceAttributes() const
{
    int bLen = aReplaceAttrText.GetText().Len();
    return ( aReplaceAttrText.IsEnabled() && bLen );
}


//////////////////////////////////////////////////////////////////////


#endif  // SV_NODIALOG
#endif  // NO_SVX_SEARCH


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
