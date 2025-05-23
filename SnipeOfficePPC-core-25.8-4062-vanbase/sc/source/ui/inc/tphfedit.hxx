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

#ifndef SC_TPHFEDIT_HXX
#define SC_TPHFEDIT_HXX

#include <sfx2/tabdlg.hxx>
#include <svx/pageitem.hxx>
#include <svtools/stdctrl.hxx>
#include <vcl/group.hxx>
#include <vcl/lstbox.hxx>
#include <vcl/timer.hxx>
#include <vcl/virdev.hxx>
#include "scdllapi.h"
#include "scitems.hxx"          // due to enum SvxNumType
#include "popmenu.hxx"
#include <com/sun/star/accessibility/XAccessible.hpp>
#include <cppuhelper/weakref.hxx>

//===================================================================

class ScHeaderEditEngine;
class ScPatternAttr;
class EditView;
class EditTextObject;
class SvxFieldItem;
class ScAccessibleEditObject;
class ScEditWindow;

SC_DLLPUBLIC ScEditWindow* GetScEditWindow ();

enum ScEditWindowLocation
{
    Left,
    Center,
    Right
};

class SC_DLLPUBLIC ScEditWindow : public Control
{
public:
            ScEditWindow( Window* pParent, const ResId& rResId, ScEditWindowLocation eLoc );
            ~ScEditWindow();

    using Control::SetFont;
    void            SetFont( const ScPatternAttr& rPattern );
    using Control::SetText;
    void            SetText( const EditTextObject& rTextObject );
    EditTextObject* CreateTextObject();
    void            SetCharAttriutes();

    void            InsertField( const SvxFieldItem& rFld );

    void            SetNumType(SvxNumType eNumType);

    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > CreateAccessible();

    inline ScHeaderEditEngine*  GetEditEngine() const {return pEdEngine;}
protected:
    virtual void    Paint( const Rectangle& rRec );
    virtual void    MouseMove( const MouseEvent& rMEvt );
    virtual void    MouseButtonDown( const MouseEvent& rMEvt );
    virtual void    MouseButtonUp( const MouseEvent& rMEvt );
    virtual void    KeyInput( const KeyEvent& rKEvt );
    virtual void    Command( const CommandEvent& rCEvt );
    virtual void    GetFocus();
    virtual void    LoseFocus();

private:
    ScHeaderEditEngine* pEdEngine;
    EditView*           pEdView;
    ScEditWindowLocation eLocation;
    bool mbRTL;

    com::sun::star::uno::WeakReference< ::com::sun::star::accessibility::XAccessible > xAcc;
    ScAccessibleEditObject* pAcc;
};

//===================================================================
class SC_DLLPUBLIC ScExtIButton : public ImageButton
{
private:

    Timer           aTimer;
    ScPopupMenu*    pPopupMenu;
    Link            aMLink;
    sal_uInt16          nSelected;

    SC_DLLPRIVATE  DECL_LINK( TimerHdl, void*);

protected:

    virtual void    MouseButtonDown( const MouseEvent& rMEvt );
    virtual void    MouseButtonUp( const MouseEvent& rMEvt);
    virtual void    Click();

    virtual void    StartPopup();

public:

    ScExtIButton(Window* pParent, const ResId& rResId );

    void            SetPopupMenu(ScPopupMenu* pPopUp);

    sal_uInt16          GetSelected();

    void            SetMenuHdl( const Link& rLink ) { aMLink = rLink; }
    const Link&     GetMenuHdl() const { return aMLink; }

    virtual long    PreNotify( NotifyEvent& rNEvt );
};

#endif // SC_TPHFEDIT_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
