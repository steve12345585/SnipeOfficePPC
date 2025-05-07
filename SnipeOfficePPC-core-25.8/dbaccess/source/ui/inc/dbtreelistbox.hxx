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
#ifndef DBAUI_DBTREELISTBOX_HXX
#define DBAUI_DBTREELISTBOX_HXX

#include "ScrollHelper.hxx"
#include "moduledbu.hxx"

#include <com/sun/star/lang/XMultiServiceFactory.hpp>

#include <svtools/svtreebx.hxx>
#include <vcl/timer.hxx>

#include <set>


namespace dbaui
{
    struct DBTreeEditedEntry
    {
        SvLBoxEntry*    pEntry;
        XubString       aNewText;
    };

    class IEntryFilter
    {
    public:
        virtual bool    includeEntry( SvLBoxEntry* _pEntry ) const = 0;

    protected:
        ~IEntryFilter() {}
    };

    //========================================================================
    class IControlActionListener;
    class IContextMenuProvider;
    class DBTreeListBox     :public SvTreeListBox
    {
        OModuleClient               m_aModuleClient;
        OScrollHelper               m_aScrollHelper;
        Timer                       m_aTimer; // is needed for table updates
        Point                       m_aMousePos;
        ::std::set< SvListEntry* >  m_aSelectedEntries;
        SvLBoxEntry*                m_pDragedEntry;
        IControlActionListener*     m_pActionListener;
        IContextMenuProvider*
                                    m_pContextMenuProvider;

        Link                        m_aPreExpandHandler;    // handler to be called before a node is expanded
        Link                        m_aSelChangeHdl;        // handlet to be called (asynchronously) when the selection changes in any way
        Link                        m_aCutHandler;          // called when someone press CTRL+X
        Link                        m_aCopyHandler;         // called when someone press CTRL+C
        Link                        m_aPasteHandler;        // called when someone press CTRL+V
        Link                        m_aDeleteHandler;       // called when someone press DELETE Key
        Link                        m_aEditingHandler;      // called before someone will edit an entry
        Link                        m_aEditedHandler;       // called after someone edited an entry
        Link                        m_aEnterKeyHdl;


        sal_Bool                    m_bHandleEnterKey;

    protected:
        ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > m_xORB;

    private:
        void init();
        DECL_LINK( OnTimeOut, void* );
        DECL_LINK( OnResetEntry, SvLBoxEntry* );
        DECL_LINK( ScrollUpHdl, SvTreeListBox* );
        DECL_LINK( ScrollDownHdl, SvTreeListBox* );

    public:
        DBTreeListBox( Window* pParent
            ,const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxORB
            ,WinBits nWinStyle=0
            ,sal_Bool _bHandleEnterKey = sal_False);
        DBTreeListBox( Window* pParent
            ,const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxORB
            ,const ResId& rResId
            ,sal_Bool _bHandleEnterKey = sal_False);
        ~DBTreeListBox();

        void                    setControlActionListener( IControlActionListener* _pListener ) { m_pActionListener = _pListener; }
        void                    setContextMenuProvider( IContextMenuProvider* _pContextMenuProvider ) { m_pContextMenuProvider = _pContextMenuProvider; }

        inline void setORB(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _xORB) { m_xORB = _xORB; }


        void    SetPreExpandHandler(const Link& _rHdl)  { m_aPreExpandHandler = _rHdl; }
        void    SetSelChangeHdl( const Link& _rHdl )    { m_aSelChangeHdl = _rHdl; }
        void    setCutHandler(const Link& _rHdl)        { m_aCutHandler = _rHdl; }
        void    setCopyHandler(const Link& _rHdl)       { m_aCopyHandler = _rHdl; }
        void    setPasteHandler(const Link& _rHdl)      { m_aPasteHandler = _rHdl; }
        void    setDeleteHandler(const Link& _rHdl)     { m_aDeleteHandler = _rHdl; }
        void    setEditingHandler(const Link& _rHdl)    { m_aEditingHandler = _rHdl; }
        void    setEditedHandler(const Link& _rHdl)     { m_aEditedHandler = _rHdl; }

        // modified the given entry so that the expand handler is called whenever the entry is expanded
        // (normally, the expand handler is called only once)
        void            EnableExpandHandler(SvLBoxEntry* _pEntry);

        SvLBoxEntry*    GetEntryPosByName( const String& aName, SvLBoxEntry* pStart = NULL, const IEntryFilter* _pFilter = NULL ) const;
        virtual void    RequestingChildren( SvLBoxEntry* pParent );
        virtual void    SelectHdl();
        virtual void    DeselectHdl();
        // Window
        virtual void    KeyInput( const KeyEvent& rKEvt );

        virtual void    StateChanged( StateChangedType nStateChange );
        virtual void    InitEntry( SvLBoxEntry* pEntry, const XubString& aStr, const Image& aCollEntryBmp, const Image& aExpEntryBmp, SvLBoxButtonKind eButtonKind);

        // enable editing for tables/views and queries
        virtual sal_Bool    EditingEntry( SvLBoxEntry* pEntry, Selection& );
        virtual sal_Bool    EditedEntry( SvLBoxEntry* pEntry, const rtl::OUString& rNewText );

        virtual sal_Bool    DoubleClickHdl();

        virtual PopupMenu* CreateContextMenu( void );
        virtual void    ExcecuteContextMenuAction( sal_uInt16 nSelectedPopupEntry );

        void            SetEnterKeyHdl(const Link& rNewHdl) {m_aEnterKeyHdl = rNewHdl;}

        void            clearCurrentSelection() { m_aSelectedEntries.clear(); }

    protected:
        virtual void        MouseButtonDown( const MouseEvent& rMEvt );
        virtual void        RequestHelp( const HelpEvent& rHEvt );

        // DragSourceHelper overridables
        virtual void        StartDrag( sal_Int8 nAction, const Point& rPosPixel );
        // DropTargetHelper overridables
        virtual sal_Int8    AcceptDrop( const AcceptDropEvent& _rEvt );
        virtual sal_Int8    ExecuteDrop( const ExecuteDropEvent& _rEvt );

        virtual void        ModelHasRemoved( SvListEntry* pEntry );
        virtual void        ModelHasEntryInvalidated( SvListEntry* pEntry );

        void                implStopSelectionTimer();
        void                implStartSelectionTimer();

    protected:
        using SvTreeListBox::ExecuteDrop;
    };
}

#endif // DBAUI_DBTREELISTBOX_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
