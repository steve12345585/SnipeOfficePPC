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

#ifndef _SV_DOCKWIN_HXX
#define _SV_DOCKWIN_HXX

#include <tools/solar.h>
#include <vcl/dllapi.h>
#include <vcl/floatwin.hxx>
#include <vector>

// data to be sent with docking events
struct DockingData
{
    Point       maMousePos;     // in
    Rectangle   maTrackRect;    // in/out
    sal_Bool        mbFloating;     // out
    sal_Bool        mbLivemode;     // in
    sal_Bool        mbInteractive;  // in

    DockingData() {};
    DockingData( const Point& rPt, const Rectangle& rRect, sal_Bool b) :
        maMousePos( rPt ), maTrackRect( rRect ), mbFloating( b ), mbLivemode( sal_False ), mbInteractive( sal_True )
        {};
};

struct EndDockingData
{
    Rectangle   maWindowRect;    // in
    sal_Bool        mbFloating;      // in
    sal_Bool        mbCancelled;     // in

    EndDockingData() {};
    EndDockingData( const Rectangle& rRect, sal_Bool b, sal_Bool bCancelled ) :
        maWindowRect( rRect ), mbFloating( b ), mbCancelled( bCancelled )
        {};
};

struct EndPopupModeData
{
    Point       maFloatingPos;    // in
    sal_Bool        mbTearoff;        // in

    EndPopupModeData() {};
    EndPopupModeData( const Point& rPos, sal_Bool bTearoff ) :
        maFloatingPos( rPos ), mbTearoff( bTearoff )
        {};
};

/** ImplDockingWindowWrapper
 *
 *  ImplDockingWindowWrapper obsoletes the DockingWindow class.
 *  It is better because it can make a "normal window" dockable.
 *  All DockingWindows should be converted the new class.
 */

class ImplDockingWindowWrapper
{
    friend class Window;
    friend class DockingManager;
    friend class DockingWindow;

private:

    // the original 'Docking'window
    Window *mpDockingWindow;

    // the original DockingWindow members
    FloatingWindow* mpFloatWin;
    Window*         mpOldBorderWin;
    Window*         mpParent;
    Point           maFloatPos;
    Point           maDockPos;
    Point           maMouseOff;
    Point           maMouseStart;
    Size            maRollUpOutSize;
    Size            maMinOutSize;
    Size            maMaxOutSize;
    Rectangle       maDragArea;
    long            mnTrackX;
    long            mnTrackY;
    long            mnTrackWidth;
    long            mnTrackHeight;
    sal_Int32       mnDockLeft;
    sal_Int32       mnDockTop;
    sal_Int32       mnDockRight;
    sal_Int32       mnDockBottom;
    WinBits         mnFloatBits;
    sal_Bool            mbDockCanceled:1,
                    mbFloatPrevented:1,
                    mbDockable:1,
                    mbDocking:1,
                    mbDragFull:1,
                    mbLastFloatMode:1,
                    mbStartFloat:1,
                    mbTrackDock:1,
                    mbPined:1,
                    mbRollUp:1,
                    mbDockBtn:1,
                    mbHideBtn:1,
                    mbStartDockingEnabled:1,
                    mbLocked:1;

    void            ImplInitData();

                    DECL_LINK( PopupModeEnd, void* );
    void            ImplEnableStartDocking( sal_Bool bEnable = sal_True )  { mbStartDockingEnabled = bEnable; }
    sal_Bool            ImplStartDockingEnabled()               { return mbStartDockingEnabled; }

public:
    ImplDockingWindowWrapper( const Window *pWindow );
    virtual ~ImplDockingWindowWrapper();

    Window*         GetWindow()     { return mpDockingWindow; }
    sal_Bool            ImplStartDocking( const Point& rPos );

    // those methods actually call the corresponding handlers
    void            StartDocking( const Point& rPos, Rectangle& rRect );
    sal_Bool            Docking( const Point& rPos, Rectangle& rRect );
    void            EndDocking( const Rectangle& rRect, sal_Bool bFloatMode );
    sal_Bool            PrepareToggleFloatingMode();
    void            ToggleFloatingMode();

    void            SetDragArea( const Rectangle& rRect );
    Rectangle       GetDragArea() const;

    void            Lock();
    void            Unlock();
    sal_Bool            IsLocked() const;

    void            StartPopupMode( ToolBox* pParentToolBox, sal_uLong nPopupModeFlags );
    sal_Bool            IsInPopupMode() const;

    void            TitleButtonClick( sal_uInt16 nButton );
    void            Pin();
    void            Roll();
    void            PopupModeEnd();
    void            Resizing( Size& rSize );
    sal_Bool            Close();
    void            Tracking( const TrackingEvent& rTEvt );
    long            Notify( NotifyEvent& rNEvt );

    void            ShowTitleButton( sal_uInt16 nButton, sal_Bool bVisible = sal_True );

    void            SetMinOutputSizePixel( const Size& rSize );

    void            SetMaxOutputSizePixel( const Size& rSize );

    sal_Bool            IsDocking() const { return mbDocking; }
    sal_Bool            IsDockable() const { return mbDockable; }
    sal_Bool            IsDockingCanceled() const { return mbDockCanceled; }
    sal_Bool            IsFloatingPrevented() const { return mbFloatPrevented; }

    void            SetFloatingMode( sal_Bool bFloatMode = sal_False );
    sal_Bool            IsFloatingMode() const;
    FloatingWindow* GetFloatingWindow() const { return mpFloatWin; }

    void            SetFloatStyle( WinBits nWinStyle );
    WinBits         GetFloatStyle() const;

    virtual void    setPosSizePixel( long nX, long nY,
                                     long nWidth, long nHeight,
                                     sal_uInt16 nFlags = WINDOW_POSSIZE_ALL );
    void            SetPosSizePixel( const Point& rNewPos,
                                     const Size& rNewSize )
                        { mpDockingWindow->SetPosSizePixel( rNewPos, rNewSize ); }
    Point           GetPosPixel() const;
    Size            GetSizePixel() const;
};

class VCL_DLLPUBLIC DockingManager
{
protected:
    ::std::vector<ImplDockingWindowWrapper *> mDockingWindows;

public:
    DockingManager();
    ~DockingManager();

    void AddWindow( const Window *pWin );
    void RemoveWindow( const Window *pWin );

    ImplDockingWindowWrapper*  GetDockingWindowWrapper( const Window *pWin );
    sal_Bool IsDockable( const Window *pWin );

    sal_Bool IsFloating( const Window *pWin );
    void SetFloatingMode( const Window *pWin, sal_Bool bFloating );

    void Lock( const Window *pWin );
    void Unlock( const Window *pWin );
    sal_Bool IsLocked( const Window *pWin );

    void    StartPopupMode( ToolBox *pParentToolBox, const Window *pWin );
    void    StartPopupMode( ToolBox *pParentToolBox, const Window *pWin, sal_uLong nPopupModeFlags );

    sal_Bool    IsInPopupMode( const Window *pWin );
    void    EndPopupMode( const Window *pWin );

    // required because those methods are not virtual in Window (!!!) and must
    // be availbale from the toolkit
    void        SetPosSizePixel( Window *pWin, long nX, long nY,
                                long nWidth, long nHeight,
                                sal_uInt16 nFlags = WINDOW_POSSIZE_ALL );
    Rectangle   GetPosSizePixel( const Window *pWin );
};

// -----------------
// - DockingWindow -
// -----------------

class VCL_DLLPUBLIC DockingWindow : public Window
{
    class   ImplData;
private:
    FloatingWindow* mpFloatWin;
    Window*         mpOldBorderWin;
    ImplData*       mpImplData;
    Point           maFloatPos;
    Point           maDockPos;
    Point           maMouseOff;
    Point           maMouseStart;
    Size            maRollUpOutSize;
    Size            maMinOutSize;
    long            mnTrackX;
    long            mnTrackY;
    long            mnTrackWidth;
    long            mnTrackHeight;
    sal_Int32           mnDockLeft;
    sal_Int32           mnDockTop;
    sal_Int32           mnDockRight;
    sal_Int32           mnDockBottom;
    WinBits         mnFloatBits;
    sal_Bool            mbDockCanceled:1,
                    mbDockPrevented:1,
                    mbFloatPrevented:1,
                    mbDockable:1,
                    mbDocking:1,
                    mbDragFull:1,
                    mbLastFloatMode:1,
                    mbStartFloat:1,
                    mbTrackDock:1,
                    mbPined:1,
                    mbRollUp:1,
                    mbDockBtn:1,
                    mbHideBtn:1;

    SAL_DLLPRIVATE void    ImplInitDockingWindowData();

    // Copy assignment is forbidden and not implemented.
    SAL_DLLPRIVATE         DockingWindow (const DockingWindow &);
    SAL_DLLPRIVATE         DockingWindow & operator= (const DockingWindow &);

protected:
    using Window::ImplInit;
    SAL_DLLPRIVATE void    ImplInit( Window* pParent, WinBits nStyle );
    SAL_DLLPRIVATE void    ImplInitSettings();
    SAL_DLLPRIVATE void    ImplLoadRes( const ResId& rResId );

public:
    SAL_DLLPRIVATE sal_Bool    ImplStartDocking( const Point& rPos );

protected:
                    DockingWindow( WindowType nType );

public:
                    DockingWindow( Window* pParent, WinBits nStyle = WB_STDDOCKWIN );
                    DockingWindow( Window* pParent, const ResId& rResId );
                    ~DockingWindow();

    virtual void    StartDocking();
    virtual sal_Bool    Docking( const Point& rPos, Rectangle& rRect );
    virtual void    EndDocking( const Rectangle& rRect, sal_Bool bFloatMode );
    virtual sal_Bool    PrepareToggleFloatingMode();
    virtual void    ToggleFloatingMode();

    virtual void    TitleButtonClick( sal_uInt16 nButton );
    virtual void    Pin();
    virtual void    Roll();
    virtual void    PopupModeEnd();
    virtual void    Resizing( Size& rSize );
    virtual sal_Bool    Close();
    virtual void    Tracking( const TrackingEvent& rTEvt );
    virtual long    Notify( NotifyEvent& rNEvt );
    virtual void    StateChanged( StateChangedType nType );
    virtual void    DataChanged( const DataChangedEvent& rDCEvt );

    void            SetPin( sal_Bool bPin );
    sal_Bool            IsPined() const;

    void            RollUp();
    void            RollDown();
    sal_Bool            IsRollUp() const;

    void            SetRollUpOutputSizePixel( const Size& rSize );
    Size            GetRollUpOutputSizePixel() const;

    void            SetMinOutputSizePixel( const Size& rSize );
    const Size&     GetMinOutputSizePixel() const;

    void            SetMaxOutputSizePixel( const Size& rSize );

    sal_Bool            IsDocking() const { return mbDocking; }
    sal_Bool            IsDockable() const { return mbDockable; }
    sal_Bool            IsDockingCanceled() const { return mbDockCanceled; }
    sal_Bool            IsDockingPrevented() const { return mbDockPrevented; }
    sal_Bool            IsFloatingPrevented() const { return mbFloatPrevented; }

    void            SetFloatingMode( sal_Bool bFloatMode = sal_False );
    sal_Bool            IsFloatingMode() const;
    FloatingWindow* GetFloatingWindow() const { return mpFloatWin; }

    void            SetFloatingPos( const Point& rNewPos );
    Point           GetFloatingPos() const;

    void            SetFloatStyle( WinBits nWinStyle );
    WinBits         GetFloatStyle() const;

    virtual void    setPosSizePixel( long nX, long nY,
                                     long nWidth, long nHeight,
                                     sal_uInt16 nFlags = WINDOW_POSSIZE_ALL );
    void            SetPosSizePixel( const Point& rNewPos,
                                     const Size& rNewSize )
                        { Window::SetPosSizePixel( rNewPos, rNewSize ); }
    Point           GetPosPixel() const;
    Size            GetSizePixel() const;
    void            SetOutputSizePixel( const Size& rNewSize );
    Size            GetOutputSizePixel() const;
};

inline void DockingWindow::SetPin( sal_Bool bPin )
{
    if ( mpFloatWin )
        mpFloatWin->SetPin( bPin );
    mbPined = bPin;
}

inline sal_Bool DockingWindow::IsPined() const
{
    if ( mpFloatWin )
        return mpFloatWin->IsPined();
    return mbPined;
}

inline void DockingWindow::RollUp()
{
    if ( mpFloatWin )
        mpFloatWin->RollUp();
    mbRollUp = sal_True;
}

inline void DockingWindow::RollDown()
{
    if ( mpFloatWin )
        mpFloatWin->RollDown();
    mbRollUp = sal_False;
}

inline sal_Bool DockingWindow::IsRollUp() const
{
    if ( mpFloatWin )
        return mpFloatWin->IsRollUp();
    return mbRollUp;
}

inline void DockingWindow::SetRollUpOutputSizePixel( const Size& rSize )
{
    if ( mpFloatWin )
        mpFloatWin->SetRollUpOutputSizePixel( rSize );
    maRollUpOutSize = rSize;
}

inline Size DockingWindow::GetRollUpOutputSizePixel() const
{
    if ( mpFloatWin )
        return mpFloatWin->GetRollUpOutputSizePixel();
    return maRollUpOutSize;
}

inline void DockingWindow::SetMinOutputSizePixel( const Size& rSize )
{
    if ( mpFloatWin )
        mpFloatWin->SetMinOutputSizePixel( rSize );
    maMinOutSize = rSize;
}

inline const Size& DockingWindow::GetMinOutputSizePixel() const
{
    if ( mpFloatWin )
        return mpFloatWin->GetMinOutputSizePixel();
    return maMinOutSize;
}

inline void DockingWindow::SetFloatingPos( const Point& rNewPos )
{
    if ( mpFloatWin )
        mpFloatWin->SetPosPixel( rNewPos );
    else
        maFloatPos = rNewPos;
}


#endif  // _SV_DOCKWIN_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
