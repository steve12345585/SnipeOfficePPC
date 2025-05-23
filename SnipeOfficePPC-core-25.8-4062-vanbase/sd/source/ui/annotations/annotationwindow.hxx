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

#ifndef _ANNOTATIONWINDOW_HXX
#define _ANNOTATIONWINDOW_HXX

#include <com/sun/star/office/XAnnotation.hpp>

#include <vcl/ctrl.hxx>
#include <vcl/lineinfo.hxx>
#include <vcl/floatwin.hxx>

#include <basegfx/polygon/b2dpolygon.hxx>

#include <svx/sdr/overlay/overlayobject.hxx>
#include <editeng/editstat.hxx>

class OutlinerView;
class Outliner;
class ScrollBar;
class MultiLineEdit;
class SvxLanguageItem;
class SdDrawDocument;

namespace sd {

class AnnotationManagerImpl;
class AnnotationWindow;
class DrawDocShell;
class View;

class AnnotationTextWindow : public Control
{
private:
    OutlinerView*       mpOutlinerView;
    AnnotationWindow*   mpAnnotationWindow;

protected:
    virtual void    Paint( const Rectangle& rRect);
    virtual void    KeyInput( const KeyEvent& rKeyEvt );
    virtual void    MouseMove( const MouseEvent& rMEvt );
    virtual void    MouseButtonDown( const MouseEvent& rMEvt );
    virtual void    MouseButtonUp( const MouseEvent& rMEvt );
    virtual void    Command( const CommandEvent& rCEvt );
    virtual void    LoseFocus();

public:
    AnnotationTextWindow( AnnotationWindow* pParent, WinBits nBits );
    ~AnnotationTextWindow();

    void SetOutlinerView( OutlinerView* pOutlinerView ) { mpOutlinerView = pOutlinerView; }

    virtual rtl::OUString GetSurroundingText() const;
    virtual Selection GetSurroundingTextSelection() const;

    virtual void    GetFocus();
};


class AnnotationWindow : public FloatingWindow
{
    private:
        AnnotationManagerImpl&  mrManager;
        DrawDocShell*           mpDocShell;
        View*                   mpView;
        SdDrawDocument*         mpDoc;

        OutlinerView*           mpOutlinerView;
        Outliner*               mpOutliner;
        ScrollBar*              mpVScrollbar;
        ::com::sun::star::uno::Reference< ::com::sun::star::office::XAnnotation > mxAnnotation;
        bool                    mbReadonly;
        bool                    mbProtected;
        bool                    mbMouseOverButton;
        AnnotationTextWindow*   mpTextWindow;
        MultiLineEdit*          mpMeta;
        Rectangle               maRectMetaButton;
        basegfx::B2DPolygon     maPopupTriangle;

    protected:
        void            SetSizePixel( const Size& rNewSize );

        DECL_LINK(ModifyHdl, void*);
        DECL_LINK(ScrollHdl, ScrollBar*);

    public:
        AnnotationWindow( AnnotationManagerImpl& rManager, DrawDocShell* pDocShell, ::Window* pParent );
        virtual ~AnnotationWindow();

        void StartEdit();

        virtual SvxLanguageItem GetLanguage(void);

        void setAnnotation( const ::com::sun::star::uno::Reference< ::com::sun::star::office::XAnnotation >& xAnnotation, bool bGrabFocus = false );
        const ::com::sun::star::uno::Reference< ::com::sun::star::office::XAnnotation >& getAnnotation() const { return mxAnnotation; }

        void ExecuteSlot( sal_uInt16 nSID );

        ScrollBar*      Scrollbar()     { return mpVScrollbar;}

        DrawDocShell*           DocShell()      { return mpDocShell; }
        OutlinerView*           getView()       { return mpOutlinerView; }
        sd::View*               DocView()       { return mpView; }
        Outliner*               Engine()        { return mpOutliner; }
        SdDrawDocument*         Doc()           { return mpDoc; }

        long            GetPostItTextHeight();

        void            InitControls();
        void            HidePostIt();
        void            DoResize();
        void            ResizeIfNeccessary(long aOldHeight, long aNewHeight);
        void            SetScrollbar();

        void            Rescale();

        bool            IsReadOnly() { return mbReadonly;}

        bool            IsProtected() { return mbProtected; }

        void            SetLanguage(const SvxLanguageItem &aNewItem);

        sal_Int32       GetScrollbarWidth();

        void            ToggleInsMode();

        DECL_LINK( OnlineSpellCallback, SpellCallbackInfo*);

        virtual void    Deactivate();
        virtual void    Paint( const Rectangle& rRect);
        virtual void    MouseMove( const MouseEvent& rMEvt );
        virtual void    MouseButtonDown( const MouseEvent& rMEvt );
        virtual void    Command( const CommandEvent& rCEvt );
        virtual void    GetFocus();

        void            SetColor();

        Color           maColor;
        Color           maColorDark;
        Color           maColorLight;
};

} // namespace sd

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
