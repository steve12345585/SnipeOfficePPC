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

#ifndef _SVTREEBOX_HXX
#define _SVTREEBOX_HXX

#include "svtools/svtdllapi.h"
#include <svtools/svlbox.hxx>
#include <svtools/svlbitm.hxx>

// forward and defines ---------------------------------------------------

class SvImpLBox;
class TabBar;

#define SV_TAB_BORDER 8

#define SV_LISTBOX_ID_TREEBOX 1   // fuer SvLBox::IsA()
#define SV_ENTRYHEIGHTOFFS_PIXEL 2

#define TREEFLAG_CHKBTN         0x0001
#define TREEFLAG_USESEL         0x0002
#define TREEFLAG_MANINS         0x0004
#define TREEFLAG_RECALCTABS     0x0008

typedef sal_Int64   ExtendedWinBits;

// disable the behavior of automatically selecting a "CurEntry" upon painting the control
#define EWB_NO_AUTO_CURENTRY        0x00000001

// class SvTreeListBox ---------------------------------------------------

class SVT_DLLPUBLIC SvTreeListBox : public SvLBox
{
    friend class SvImpLBox;
    friend class ImpLBSelEng;
    friend class TreeControlPeer;

    SvImpLBox*      pImp;
    Link            aCheckButtonHdl;
    Link            aScrolledHdl;
    Accelerator     aInpEditAcc;
    Image           aPrevInsertedExpBmp;
    Image           aPrevInsertedColBmp;
    Image           aCurInsertedExpBmp;
    Image           aCurInsertedColBmp;

    short           nContextBmpWidthMax;
    sal_uInt16          nFirstSelTab, nLastSelTab;
    short           nEntryHeight;
    short           nEntryHeightOffs;
    short           nIndent;
    short           nFocusWidth;
    sal_uInt16          aContextBmpMode;

    long mnCheckboxItemWidth;

#ifdef _SVTREEBX_CXX
    DECL_DLLPRIVATE_LINK( CheckButtonClick, SvLBoxButtonData * );

    SVT_DLLPRIVATE short        GetHeightOffset( const Image& rBmp, Size& rLogicSize);
    SVT_DLLPRIVATE short        GetHeightOffset( const Font& rFont, Size& rLogicSize);

    SVT_DLLPRIVATE void         SetEntryHeight( SvLBoxEntry* pEntry );
    SVT_DLLPRIVATE void         AdjustEntryHeight( const Image& rBmp );
    SVT_DLLPRIVATE void         AdjustEntryHeight( const Font& rFont );

    SVT_DLLPRIVATE void         ImpEntryInserted( SvLBoxEntry* pEntry );
    SVT_DLLPRIVATE long         PaintEntry1( SvLBoxEntry*, long nLine,
                                sal_uInt16 nTabFlagMask=0xffff,
                                sal_Bool bHasClipRegion=sal_False );

    SVT_DLLPRIVATE void         InitTreeView();
    SVT_DLLPRIVATE SvLBoxItem*  GetItem_Impl( SvLBoxEntry*, long nX, SvLBoxTab** ppTab,
                        sal_uInt16 nEmptyWidth );
    SVT_DLLPRIVATE void         ImplInitStyle();

#endif

protected:
    SvLBoxButtonData*   pCheckButtonData;
    sal_uInt16              nTreeFlags;

    SvLBoxEntry*        pEdEntry;
    SvLBoxItem*         pEdItem;

    void            EditItemText( SvLBoxEntry* pEntry, SvLBoxString* pItem,
                        const Selection& );
    void            EditedText( const XubString& );
    void            EditingRequest( SvLBoxEntry* pEntry, SvLBoxItem* pItem,
                        const Point& rMousePos );

    SvPtrarr            aTabs;

    // berechnet abhaengig von TreeList-Style & Bitmap-Groessen
    // alle Tabulatoren neu; wird beim Einfuegen/Austauschen von
    // Bitmaps, beim Wechsel des Models usw. automatisch gerufen
    virtual void    SetTabs();
    void            SetTabs_Impl();
    void            AddTab( long nPos,sal_uInt16 nFlags=SV_LBOXTAB_ADJUST_LEFT,
                        void* pUserData = 0 );
    sal_uInt16          TabCount() const { return aTabs.Count(); }
    SvLBoxTab*      GetFirstDynamicTab() const;
    SvLBoxTab*      GetFirstDynamicTab( sal_uInt16& rTabPos ) const;
    SvLBoxTab*      GetFirstTab( sal_uInt16 nFlagMask, sal_uInt16& rTabPos );
    SvLBoxTab*      GetLastTab( sal_uInt16 nFlagMask, sal_uInt16& rTabPos );
    SvLBoxTab*      GetTab( SvLBoxEntry*, SvLBoxItem* ) const;
    void            ClearTabList();

    virtual void InitEntry(SvLBoxEntry*,const XubString&,const Image&,const Image&,SvLBoxButtonKind);

    virtual void    NotifyBeginScroll();
    virtual void    NotifyEndScroll();
    // nLines == 0 => horizontales Scrolling
    virtual void    NotifyScrolling( long nLines );
    virtual void    NotifyScrolled();
    void            SetScrolledHdl( const Link& rLink ) { aScrolledHdl = rLink; }
    const Link&     GetScrolledHdl() const { return aScrolledHdl; }
    long            GetXOffset() const { return GetMapMode().GetOrigin().X(); }

    // wird aufgerufen, _bevor_ Bereiche im Control invalidiert werden,
    // kann zum Hiden von Elementen benutzt werden, die von aussen
    // in das Control hineingezeichnet werden
    virtual void    NotifyInvalidating();

    virtual sal_uLong   GetAscInsertionPos( SvLBoxEntry*, SvLBoxEntry* pParent );
    virtual sal_uLong   GetDescInsertionPos( SvLBoxEntry*, SvLBoxEntry* pParent );
    virtual void    Command( const CommandEvent& rCEvt );

    virtual void    RequestHelp( const HelpEvent& rHEvt );
    virtual void    CursorMoved( SvLBoxEntry* pNewCursor );
    virtual void    PreparePaint( SvLBoxEntry* );
    virtual void    DataChanged( const DataChangedEvent& rDCEvt );
    virtual void    StateChanged( StateChangedType nStateChange );

    void            InitSettings(sal_Bool bFont,sal_Bool bForeground,sal_Bool bBackground);
    sal_Bool            IsCellFocusEnabled() const;
    bool            SetCurrentTabPos( sal_uInt16 _nNewPos );
    sal_uInt16          GetCurrentTabPos() const;
    void            CallImplEventListeners(sal_uLong nEvent, void* pData);

    void            ImplEditEntry( SvLBoxEntry* pEntry );

    sal_Bool        AreChildrenTransient() const;
    void            SetChildrenNotTransient();

public:
    SvTreeListBox( Window* pParent, WinBits nWinStyle=0 );
    SvTreeListBox( Window* pParent, const ResId& rResId );
    ~SvTreeListBox();

    void                SetExtendedWinBits( ExtendedWinBits _nBits );

    void            DisconnectFromModel();

    void            EnableCheckButton( SvLBoxButtonData* );
    void            SetCheckButtonData( SvLBoxButtonData* );
    void            SetNodeBitmaps( const Image& rCollapsedNodeBmp, const Image& rExpandedNodeBmp );

    /** returns the default image which clients should use for expanded nodes, to have a consistent user
        interface experience in the whole product.
    */
    static const Image& GetDefaultExpandedNodeImage( );

    /** returns the default image which clients should use for expanded nodes, to have a consistent user
        interface experience in the whole product.
    */
    static const Image& GetDefaultCollapsedNodeImage( );

    /** sets default bitmaps for collapsed and expanded nodes.
    */
    inline  void    SetNodeDefaultImages( )
    {
        SetNodeBitmaps(
            GetDefaultCollapsedNodeImage( ),
            GetDefaultExpandedNodeImage( )
        );
    }

    virtual SvLBoxEntry*    InsertEntry( const XubString& rText, SvLBoxEntry* pParent = 0,
                                         sal_Bool bChildrenOnDemand = sal_False,
                                         sal_uLong nPos=LIST_APPEND, void* pUserData = 0,
                                         SvLBoxButtonKind eButtonKind = SvLBoxButtonKind_enabledCheckbox );

    virtual SvLBoxEntry*    InsertEntry( const XubString& rText,
                                         const Image& rExpandedEntryBmp,
                                         const Image& rCollapsedEntryBmp,
                                         SvLBoxEntry* pParent = 0,
                                         sal_Bool bChildrenOnDemand = sal_False,
                                         sal_uLong nPos = LIST_APPEND, void* pUserData = 0,
                                         SvLBoxButtonKind eButtonKind = SvLBoxButtonKind_enabledCheckbox );

    const Image&    GetDefaultExpandedEntryBmp( ) const;
    const Image&    GetDefaultCollapsedEntryBmp( ) const;

    void            SetDefaultExpandedEntryBmp( const Image& rBmp );
    void            SetDefaultCollapsedEntryBmp( const Image& rBmp );

    void            SetCheckButtonState( SvLBoxEntry*, SvButtonState );
    SvButtonState   GetCheckButtonState( SvLBoxEntry* ) const;

    sal_Bool            IsExpandBitmapOnCursor() const  { return (sal_Bool)(aContextBmpMode & SVLISTENTRYFLAG_FOCUSED)!=0; }

    void            SetEntryText(SvLBoxEntry*, const XubString& );
    void            SetExpandedEntryBmp( SvLBoxEntry* _pEntry, const Image& _rImage );
    void            SetCollapsedEntryBmp( SvLBoxEntry* _pEntry, const Image& _rImage );

    virtual String  GetEntryText( SvLBoxEntry* pEntry ) const;
    String          SearchEntryText( SvLBoxEntry* pEntry ) const;
    const Image&    GetExpandedEntryBmp(SvLBoxEntry* _pEntry ) const;
    const Image&    GetCollapsedEntryBmp(SvLBoxEntry* _pEntry ) const;

    void            SetCheckButtonHdl( const Link& rLink )  { aCheckButtonHdl=rLink; }
    Link            GetCheckButtonHdl() const { return aCheckButtonHdl; }
    virtual void    CheckButtonHdl();

    virtual SvLBoxEntry*    CloneEntry( SvLBoxEntry* pSource );

    virtual sal_uInt16  IsA();

    void            SetSublistOpenWithReturn( sal_Bool bMode = sal_True );      // open/close sublist with return/enter
    void            SetSublistOpenWithLeftRight( sal_Bool bMode = sal_True );   // open/close sublist with cursor left/right

    void            EnableInplaceEditing( sal_Bool bEnable );
    sal_Bool            IsInplaceEditingEnabled() const { return SvLBox::IsInplaceEditingEnabled(); }
    inline void     ForbidEmptyText() { SvLBox::ForbidEmptyText(); }
    // Editiert das erste StringItem des Entries, 0==Cursor
    void            EditEntry( SvLBoxEntry* pEntry = NULL );
    virtual sal_Bool    EditingEntry( SvLBoxEntry* pEntry, Selection& );
    virtual sal_Bool    EditedEntry( SvLBoxEntry* pEntry, const rtl::OUString& rNewText );
    sal_Bool            IsEditingActive() const { return SvLBox::IsEditingActive(); }
    void            EndEditing( sal_Bool bCancel = sal_False ) { SvLBox::EndEditing( bCancel ); }
    sal_Bool            EditingCanceled() const { return SvLBox::EditingCanceled(); }

    virtual void    RequestingChildren( SvLBoxEntry* pParent );

    virtual void    Paint( const Rectangle& rRect );
    virtual void    MouseButtonDown( const MouseEvent& rMEvt );
    virtual void    MouseButtonUp( const MouseEvent& rMEvt );
    virtual void    MouseMove( const MouseEvent& rMEvt );
    virtual void    KeyInput( const KeyEvent& rKEvt );
    virtual void    Resize();
    virtual void    GetFocus();
    virtual void    LoseFocus();
    void            SetUpdateMode( sal_Bool );

    using SvListView::SetModel;
    void            SetModel( SvLBoxTreeList* );
    virtual void    ModelHasCleared();
    virtual void    ModelHasInserted( SvListEntry* pEntry );
    virtual void    ModelHasInsertedTree( SvListEntry* pEntry );
    virtual void    ModelIsMoving(SvListEntry* pSource,
                        SvListEntry* pTargetParent, sal_uLong nChildPos );
    virtual void    ModelHasMoved(SvListEntry* pSource );
    virtual void    ModelIsRemoving( SvListEntry* pEntry );
    virtual void    ModelHasRemoved( SvListEntry* pEntry );
    virtual void    ModelHasEntryInvalidated( SvListEntry* pEntry );

    void            ShowTargetEmphasis( SvLBoxEntry*, sal_Bool bShow );
    using Window::GetDropTarget;
    SvLBoxEntry*    GetDropTarget( const Point& );
    void            ScrollOutputArea( short nDeltaEntries );

    short           GetEntryHeight() const  { return nEntryHeight; }
    void            SetEntryHeight( short nHeight, sal_Bool bAlways = sal_False );
    Size            GetOutputSizePixel() const;
    short           GetIndent() const { return nIndent; }
    void            SetIndent( short nIndent );
    void            SetSpaceBetweenEntries( short nSpace );
    short           GetSpaceBetweenEntries() const {return nEntryHeightOffs;}
    Point           GetEntryPosition( SvLBoxEntry* ) const;
    void            ShowEntry( SvLBoxEntry* );  // !!!OBSOLETE, use MakeVisible
    virtual void    MakeVisible( SvLBoxEntry* );
    void            MakeVisible( SvLBoxEntry*, sal_Bool bMoveToTop );

    void            SetCollapsedNodeBmp( const Image& );
    void            SetExpandedNodeBmp( const Image& );
    Image           GetExpandedNodeBmp( ) const;

    void            SetFont( const Font& rFont );

    using Window::SetCursor;
    void            SetCursor( SvLBoxEntry* pEntry, sal_Bool bForceNoSelect = sal_False );

    SvLBoxEntry*    GetEntry( const Point& rPos, sal_Bool bHit = sal_False ) const;
    SvLBoxEntry*    GetEntry( SvLBoxEntry* pParent, sal_uLong nPos ) const { return SvLBox::GetEntry(pParent,nPos); }
    SvLBoxEntry*    GetEntry( sal_uLong nRootPos ) const { return SvLBox::GetEntry(nRootPos);}

    void            PaintEntry( SvLBoxEntry* );
    long            PaintEntry( SvLBoxEntry*, long nLine,
                                sal_uInt16 nTabFlagMask=0xffff );
    virtual Rectangle GetFocusRect( SvLBoxEntry*, long nLine );
    // Beruecksichtigt Einrueckung
    virtual long    GetTabPos( SvLBoxEntry*, SvLBoxTab* );
    void            InvalidateEntry( SvLBoxEntry* );
    SvLBoxItem*     GetItem( SvLBoxEntry*, long nX, SvLBoxTab** ppTab);
    SvLBoxItem*     GetItem( SvLBoxEntry*, long nX );

    void            SetDragDropMode( DragDropMode );
    void            SetSelectionMode( SelectionMode );

    virtual sal_Bool    Expand( SvLBoxEntry* pParent );
    virtual sal_Bool    Collapse( SvLBoxEntry* pParent );
    virtual sal_Bool    Select( SvLBoxEntry* pEntry, sal_Bool bSelect=sal_True );
    virtual sal_uLong   SelectChildren( SvLBoxEntry* pParent, sal_Bool bSelect );
    virtual void    SelectAll( sal_Bool bSelect, sal_Bool bPaint = sal_True );
    virtual void    SetCurEntry( SvLBoxEntry* _pEntry );
    virtual SvLBoxEntry*
                    GetCurEntry() const;

    using Window::Invalidate;
    virtual void    Invalidate( sal_uInt16 nFlags = 0);
    virtual void    Invalidate( const Rectangle&, sal_uInt16 nFlags = 0 );

    void            SetHighlightRange(sal_uInt16 nFirstTab=0,sal_uInt16 nLastTab=0xffff);

    virtual Region  GetDragRegion() const;

    // Children des Parents werden Children des naechstoberen Parents
    void            RemoveParentKeepChildren( SvLBoxEntry* pParent );

    DECL_LINK( DefaultCompare, SvSortData* );
    virtual void    ModelNotification( sal_uInt16 nActionId, SvListEntry* pEntry1,
                        SvListEntry* pEntry2, sal_uLong nPos );

    void            EndSelection();
    void            RepaintScrollBars() const;
    ScrollBar*      GetVScroll();
    ScrollBar*      GetHScroll();
    void            EnableAsyncDrag( sal_Bool b );

    SvLBoxEntry*    GetFirstEntryInView() const;
    SvLBoxEntry*    GetNextEntryInView(SvLBoxEntry*) const;
    SvLBoxEntry*    GetLastEntryInView() const;
    void            ScrollToAbsPos( long nPos );

    void            ShowFocusRect( const SvLBoxEntry* pEntry );
    void            InitStartEntry();

    virtual PopupMenu* CreateContextMenu( void );
    virtual void    ExcecuteContextMenuAction( sal_uInt16 nSelectedPopupEntry );

    void            EnableContextMenuHandling( void );
    void            EnableContextMenuHandling( sal_Bool bEnable );
    sal_Bool            IsContextMenuHandlingEnabled( void ) const;

    void            EnableList( bool _bEnable );

    // ACCESSIBILITY ==========================================================

    /** Creates and returns the accessible object of the Box. */
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::accessibility::XAccessible > CreateAccessible();

    /** Fills the StateSet with all states (except DEFUNC, done by the accessible object). */
    virtual void FillAccessibleStateSet( ::utl::AccessibleStateSetHelper& rStateSet ) const;

    /** Fills the StateSet of one entry. */
    virtual void FillAccessibleEntryStateSet( SvLBoxEntry* pEntry, ::utl::AccessibleStateSetHelper& rStateSet ) const;

    /** Calculate and returns the bounding rectangle of an entry.
        @param pEntry
            The entry.
        @return  The bounding rectangle of an entry. */
    virtual Rectangle   GetBoundingRect( SvLBoxEntry* pEntry );

    /** Enables, that one cell of a tablistbox entry can be focused */
    void                EnableCellFocus();

protected:
    using SvListView::Expand;
    using SvListView::Collapse;
    using SvListView::Select;
    using SvListView::SelectAll;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
