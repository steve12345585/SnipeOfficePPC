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
#ifndef _REPORT_RPTUIFUNC_HXX
#define _REPORT_RPTUIFUNC_HXX

#include <vcl/timer.hxx>

class Timer;
class MouseEvent;
class Point;
class SdrTextObj;
class SdrObject;
namespace rptui
{

class OReportSection;
class OSectionView;

//============================================================================
// DlgEdFunc
//============================================================================

class DlgEdFunc /* : public LinkHdl */
{
    DlgEdFunc(const DlgEdFunc&);
    void operator =(const DlgEdFunc&);
protected:
    OReportSection* m_pParent;
    OSectionView&   m_rView;
    Timer           aScrollTimer;
    Point           m_aMDPos;
    com::sun::star::uno::Reference<com::sun::star::uno::XInterface> m_xOverlappingObj;
    SdrObject *     m_pOverlappingObj;
    sal_Int32       m_nOverlappedControlColor;
    sal_Int32       m_nOldColor;
    bool            m_bSelectionMode;
    bool            m_bUiActive;
    bool            m_bShowPropertyBrowser;

    DECL_LINK( ScrollTimeout, Timer * );
    void    ForceScroll( const Point& rPos );
    /** checks that no other object is overlapped.
    *
    * \param rMEvt
    */
    void    checkMovementAllowed(const MouseEvent& rMEvt);

    /** sets the correct mouse pointer when moving a object
    *
    * \param rMEvt
    * \return <TRUE/> when the pointer was already set.
    */
    bool    setMovementPointer(const MouseEvent& rMEvt);

    bool    isRectangleHit(const MouseEvent& rMEvt);
    /**
        returns true, as long as only customshapes in the marked list,
        custom shapes can drop every where
    */
    bool    isOnlyCustomShapeMarked();

    /** activate object if it is of type OBJ_OLE2
    */
    void    activateOle(SdrObject* _pObj);

    void checkTwoCklicks(const MouseEvent& rMEvt);

public:
    DlgEdFunc( OReportSection* pParent );
    virtual ~DlgEdFunc();

    virtual sal_Bool MouseButtonDown( const MouseEvent& rMEvt );
    virtual sal_Bool MouseButtonUp( const MouseEvent& rMEvt );
    virtual sal_Bool MouseMove( const MouseEvent& rMEvt );

    /** checks if the keycode is known by the child windows
        @param  _rCode  the keycode
        @return <TRUE/> if the keycode is handled otherwise <FALSE/>
    */
    virtual sal_Bool    handleKeyEvent(const KeyEvent& _rEvent);

    /** returns <TRUE/> if the mouse event is over an existing object
    *
    * \param rMEvt
    * \return <TRUE/> if overlapping, otherwise <FALSE/>
    */
    bool isOverlapping(const MouseEvent& rMEvt);
    void setOverlappedControlColor(sal_Int32 _nColor);
    void stopScrollTimer();

    /** deactivate all ole object
    */
    void    deactivateOle(bool _bSelect = false);

    inline bool isUiActive() const { return m_bUiActive; }
protected:
    void colorizeOverlappedObject(SdrObject* _pOverlappedObj);
    void unColorizeOverlappedObj();


};

//============================================================================
// DlgEdFuncInsert
//============================================================================

class DlgEdFuncInsert : public DlgEdFunc
{
public:
    DlgEdFuncInsert( OReportSection* pParent );
    ~DlgEdFuncInsert();

    virtual sal_Bool MouseButtonDown( const MouseEvent& rMEvt );
    virtual sal_Bool MouseButtonUp( const MouseEvent& rMEvt );
    virtual sal_Bool MouseMove( const MouseEvent& rMEvt );
};

//============================================================================
// DlgEdFuncSelect
//============================================================================

class DlgEdFuncSelect : public DlgEdFunc
{
public:
    DlgEdFuncSelect( OReportSection* pParent );
    ~DlgEdFuncSelect();

    virtual sal_Bool MouseButtonDown( const MouseEvent& rMEvt );
    virtual sal_Bool MouseButtonUp( const MouseEvent& rMEvt );
    virtual sal_Bool MouseMove( const MouseEvent& rMEvt );

    void SetInEditMode(SdrTextObj* _pTextObj,const MouseEvent& rMEvt, sal_Bool bQuickDrag);
};

}
#endif //_REPORT_RPTUIFUNC_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
