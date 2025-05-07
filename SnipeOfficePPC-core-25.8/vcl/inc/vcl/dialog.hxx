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

#ifndef _SV_DIALOG_HXX
#define _SV_DIALOG_HXX

#include <vcl/sv.h>
#include <vcl/dllapi.h>
#include <vcl/syswin.hxx>

// parameter to pass to the dialogue constructor if really no parent is wanted
// whereas NULL chooses the deafult dialogue parent
#define DIALOG_NO_PARENT ((Window*)0xffffffff)

// ----------
// - Dialog -
// ----------

struct DialogImpl;

class VCL_DLLPUBLIC Dialog : public SystemWindow
{
private:
    Window*         mpDialogParent;
    Dialog*         mpPrevExecuteDlg;
    DialogImpl*     mpDialogImpl;
    long            mnMousePositioned;
    sal_Bool            mbInExecute;
    sal_Bool            mbOldSaveBack;
    sal_Bool            mbInClose;
    sal_Bool            mbModalMode;
    sal_Int8        mnCancelClose;  //liuchen 2009-7-22, support Excel VBA UserForm_QueryClose event

    SAL_DLLPRIVATE void    ImplInitDialogData();
    SAL_DLLPRIVATE void    ImplInitSettings();

    // Copy assignment is forbidden and not implemented.
    SAL_DLLPRIVATE         Dialog (const Dialog &);
    SAL_DLLPRIVATE         Dialog & operator= (const Dialog &);

    DECL_DLLPRIVATE_LINK( ImplAsyncCloseHdl, void* );
protected:
    using Window::ImplInit;
    SAL_DLLPRIVATE void    ImplInit( Window* pParent, WinBits nStyle );
    SAL_DLLPRIVATE void    ImplDialogRes( const ResId& rResId );

public:
    SAL_DLLPRIVATE sal_Bool    IsInClose() const { return mbInClose; }

protected:
                    Dialog( WindowType nType );
    virtual void    Draw( OutputDevice* pDev, const Point& rPos, const Size& rSize, sal_uLong nFlags );

public:
                    Dialog( Window* pParent, WinBits nStyle = WB_STDDIALOG );
    virtual         ~Dialog();

    virtual long    Notify( NotifyEvent& rNEvt );
    virtual void    StateChanged( StateChangedType nStateChange );
    virtual void    DataChanged( const DataChangedEvent& rDCEvt );

    virtual sal_Bool    Close();

    virtual short   Execute();
    sal_Bool            IsInExecute() const { return mbInExecute; }

    sal_Int8        GetCloseFlag() const { return mnCancelClose; }  //liuchen 2009-7-22, support Excel VBA UserForm_QueryClose event
    void            SetCloseFlag( sal_Int8 nCancel ) { mnCancelClose = nCancel; }  //liuchen 2009-7-22, support Excel VBA UserForm_QueryClose event

    ////////////////////////////////////////
    // Dialog::Execute replacement API
public:
    // Link impl: DECL_LINK( MyEndDialogHdl, Dialog* ); <= param is dialog just ended
    virtual void    StartExecuteModal( const Link& rEndDialogHdl );
    long            GetResult() const;
private:
    sal_Bool            ImplStartExecuteModal();
    void            ImplEndExecuteModal();
public:

    // Dialog::Execute replacement API
    ////////////////////////////////////////

    void            EndDialog( long nResult = 0 );
    static void     EndAllDialogs( Window* pParent=NULL );

    void            GetDrawWindowBorder( sal_Int32& rLeftBorder, sal_Int32& rTopBorder,
                                         sal_Int32& rRightBorder, sal_Int32& rBottomBorder ) const;

    void            SetModalInputMode( sal_Bool bModal );
    void            SetModalInputMode( sal_Bool bModal, sal_Bool bSubModalDialogs );
    sal_Bool            IsModalInputMode() const { return mbModalMode; }

    void            GrabFocusToFirstControl();
};

// ------------------
// - ModelessDialog -
// ------------------

class VCL_DLLPUBLIC ModelessDialog : public Dialog
{
    // Copy assignment is forbidden and not implemented.
    SAL_DLLPRIVATE         ModelessDialog (const ModelessDialog &);
    SAL_DLLPRIVATE         ModelessDialog & operator= (const ModelessDialog &);

public:
                    ModelessDialog( Window* pParent, const ResId& rResId );
};

// ---------------
// - ModalDialog -
// ---------------

class VCL_DLLPUBLIC ModalDialog : public Dialog
{
public:
                    ModalDialog( Window* pParent, WinBits nStyle = WB_STDMODAL );
                    ModalDialog( Window* pParent, const ResId& rResId );

private:
    using Window::Show;
    void            Show( sal_Bool bVisible = sal_True );
    using Window::Hide;
    void            Hide();

    // Copy assignment is forbidden and not implemented.
    SAL_DLLPRIVATE         ModalDialog (const ModalDialog &);
    SAL_DLLPRIVATE         ModalDialog & operator= (const ModalDialog &);
};

#endif  // _SV_DIALOG_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
