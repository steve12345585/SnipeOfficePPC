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
#ifndef _SFXMNUMGR_HXX
#define _SFXMNUMGR_HXX

#include <vcl/menu.hxx>
#include <tools/wintypes.hxx>
#include <tools/link.hxx>
#include <com/sun/star/embed/VerbDescriptor.hpp>
#include <com/sun/star/uno/Sequence.hxx>

#include <tools/stream.hxx>
#include <tools/resid.hxx>
#include <sfx2/sfx.hrc>

class SfxVirtualMenu;
class SfxPoolItem;
class SfxBindings;
class ResMgr;
class Menu;
class PopupMenu;
class SfxBindings;
class ResId;
class Point;
class SfxObjectShell;
class SfxModule;
class SfxViewFrame;

class SfxMenuManager
{
friend class SfxPopupMenuManager;

    SfxVirtualMenu*  pMenu;     // the actual Menu
    SfxVirtualMenu*         pOldMenu;       // only while reconfiguring
    sal_Bool         bMenuBar;  // Popup or MenuBar
    SfxBindings*            pBindings;
    ResMgr*                 pResMgr;
    sal_uInt32              nType;
    sal_Bool                    bAddClipboardFuncs : 1;

    void                    Construct( SfxVirtualMenu& rMenu );

protected:
                            SfxMenuManager( Menu*, SfxBindings& );
                            ~SfxMenuManager();
    sal_uInt16                  GetItemPos( sal_uInt16 nId );
    sal_uInt32              GetType() { return nType; }
public:

    DECL_LINK( Select, Menu* );

    SfxVirtualMenu*         GetMenu() const
                            { return pMenu; }

    SfxBindings&            GetBindings() { return *pBindings; }
    const SfxBindings&      GetBindings() const { return *pBindings; }
    void                    SetResMgr(ResMgr* pMgr)  {pResMgr = pMgr; }
    ResMgr*                 GetResMgr() const { return pResMgr; }
    void                    SetPopupMenu( sal_uInt16 nId, PopupMenu *pMenu );
};

//--------------------------------------------------------------------

class SAL_DLLPUBLIC_EXPORT SfxPopupMenuManager : public SfxMenuManager
{
private:
    Menu*               pSVMenu;

    // when #i107205 gets fixed this one should be superfluous.
    // But right now we want to avoid the memory leak that would otherwise occur,
    // if we don't delete the pointer that got created in SfxPopupMenuManager::Popup
    static PopupMenu *  pStaticThesSubMenu;

    // only declared, but not defined: don't allow copying
    SfxPopupMenuManager( const SfxPopupMenuManager& );
    SfxPopupMenuManager& operator=( const SfxPopupMenuManager& );

public:
                        SfxPopupMenuManager( PopupMenu*, SfxBindings& );
                        ~SfxPopupMenuManager();
    static void         ExecutePopup( const ResId&, SfxViewFrame* pViewFrame, const Point& rPoint, Window* pWindow );
    // @deprecated!!
    // Don't use this method any longer. The whole class will be removed in the future.
    // Changing code which relies on Popup would need much more effort.
    static SfxPopupMenuManager* Popup( const ResId& rResId, SfxViewFrame* pFrame,const Point& rPoint, Window* pWindow );

    sal_uInt16              Execute( const Point& rPos, Window *pWindow );

    void                RemoveDisabledEntries();
    Menu*                   GetSVMenu();
};

#endif // #ifndef _SFXMNUMGR_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
