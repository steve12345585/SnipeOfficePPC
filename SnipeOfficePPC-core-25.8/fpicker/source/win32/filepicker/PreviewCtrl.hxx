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

#ifndef _PREVIEWCTRL_HXX_
#define _PREVIEWCTRL_HXX_

//------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------

#include <sal/types.h>
#include <rtl/ustring.hxx>

#include <comdef.h>

#include <memory>

//---------------------------------------------
// declaration
//---------------------------------------------

class CDimension
{
public:
    CDimension( ) :
        m_cx( 0 ),
        m_cy( 0 )
    {
    }

    CDimension( sal_Int32 cx, sal_Int32 cy ) :
        m_cx( cx ),
        m_cy( cy )
    {
    }

    sal_Int32 m_cx;
    sal_Int32 m_cy;
};

//--------------------------------------------------
// we use OleInitialize here because we are calling
// some Ole functions to realize the picture preview
// and we expect to be called from the main thread
// so that there will be no problem calling
// OleInitialize (the main thread should be an STA)
// When OleInitialize should fail at worst the
// preview doesn't work
//--------------------------------------------------

class CAutoOleInit
{
public:

    // used to communicate ole
    // initialzation failures
    class COleInitException { };

    CAutoOleInit( )
    {
        HRESULT hr = OleInitialize( NULL );
        if ( FAILED( hr ) )
            throw COleInitException( );
    }

    ~CAutoOleInit( )
    {
        OleUninitialize( );
    }
};

//---------------------------------------------
// A simple file preview class to preview some
// common picture formats like *.gif, *jpg, etc.
// This class is not thread-safe and is
// implmented as singleton, because the class
// has only one static member to reconnect
// from callback functions
// we use a singleton-destroyer to get rid off
// the singleton instance, but this happens
// only on shutdown (unloading of the dll) -
// it's a question of taste (other solutions
// are possible)
//---------------------------------------------

class CFilePreview
{
public:
    // to ensure only one instance (singleton)
    static CFilePreview* createInstance(
        HWND aParent,
        POINT ulCorner,
        const CDimension& aSize,
        HINSTANCE hInstance,
        sal_Bool bShow = sal_True,
        sal_Bool bEnabled = sal_True );

    // sets the size of the preview window
    sal_Bool SAL_CALL setSize( const CDimension& aSize );

    // returns the CDimension of the preview
    sal_Bool SAL_CALL getSize( CDimension& theSize ) const;

    // sets the position of the upper left corner
    // of the preview window relative to the
    // upper left corner of the parent window
    sal_Bool SAL_CALL setPos( POINT ulCorner );

    // returns the current position of the preview
    // relative to the upper left corner of the
    // parent window
    sal_Bool SAL_CALL getPos( POINT& ulCorner ) const;

    // enables or disables the preview window
    // bEnable - true the window is enabled and updates its
    // view when update is called
    // bEnable - false the window shows itself in disabled
    // mode and does not update its view when update is
    // called
    void SAL_CALL enable( sal_Bool bEnable );

    // shows the preview window
    // possible values see SHOW_STATE
    sal_Bool SAL_CALL show( sal_Bool bShow );


    // if the preview is shown and enabled
    // preview of the given file will be shown
    // returns true on success or false if an error
    // occurred (the file in not there or not accessible etc.)
    virtual sal_Bool SAL_CALL update( const rtl::OUString& aFileName );

protected:
    // clients can create instances only through the static create method
    CFilePreview(
        HWND aParent,
        POINT ulCorner,
        const CDimension& aSize,
        HINSTANCE hInstance,
        sal_Bool bShow = sal_True,
        sal_Bool bEnabled = sal_True );

    // only the singleton destroyer class is allowed to delete the
    // singleton instance of this class
    virtual ~CFilePreview( );

    // we use the stl auto_ptr class as singleton destroyer
    typedef std::auto_ptr< CFilePreview > FILEPREVIEW_SINGLETON_DESTROYER_T;

protected:
    virtual void SAL_CALL onPaint( HWND hWnd, HDC hDC );

    sal_Bool loadFile( const rtl::OUString& aFileName );

private:
    CAutoOleInit m_autoOleInit;
    POINT        m_pt;
    CDimension   m_dim;
    HWND         m_hwnd;
    sal_Bool     m_bEnabled;
    IPicturePtr  m_IPicture;
    ATOM         m_atomPrevWndClass;
    HINSTANCE    m_hInstance;

    static LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

    static CFilePreview* s_FilePreviewInst;
    static FILEPREVIEW_SINGLETON_DESTROYER_T s_SingletonDestroyer;

private:
    friend FILEPREVIEW_SINGLETON_DESTROYER_T;
};


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
