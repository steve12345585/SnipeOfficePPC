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


#include <tools/rcid.h>

#include <vcl/dockwin.hxx>
#include <vcl/taskpanelist.hxx>

#include <svdata.hxx>

#include <functional>
#include <algorithm>

// can't have static linkage because SUNPRO 5.2 complains
Point ImplTaskPaneListGetPos( const Window *w )
{
    Point pos;
    if( w->ImplIsDockingWindow() )
    {
        pos = ((DockingWindow*)w)->GetPosPixel();
        Window *pF = ((DockingWindow*)w)->GetFloatingWindow();
        if( pF )
            pos = pF->OutputToAbsoluteScreenPixel( pF->ScreenToOutputPixel( pos ) );
        else
            pos = w->OutputToAbsoluteScreenPixel( pos );
    }
    else
        pos = w->OutputToAbsoluteScreenPixel( w->GetPosPixel() );

    return pos;
}

// compares window pos left-to-right
struct LTRSort : public ::std::binary_function< const Window*, const Window*, bool >
{
    bool operator()( const Window* w1, const Window* w2 ) const
    {
        Point pos1(ImplTaskPaneListGetPos( w1 ));
        Point pos2(ImplTaskPaneListGetPos( w2 ));

        if( pos1.X() == pos2.X() )
            return ( pos1.Y() < pos2.Y() );
        else
            return ( pos1.X() < pos2.X() );
    }
};
struct LTRSortBackward : public ::std::binary_function< const Window*, const Window*, bool >
{
    bool operator()( const Window* w2, const Window* w1 ) const
    {
        Point pos1(ImplTaskPaneListGetPos( w1 ));
        Point pos2(ImplTaskPaneListGetPos( w2 ));

        if( pos1.X() == pos2.X() )
            return ( pos1.Y() < pos2.Y() );
        else
            return ( pos1.X() < pos2.X() );
    }
};

// --------------------------------------------------

static void ImplTaskPaneListGrabFocus( Window *pWindow )
{
    // put focus in child of floating windows which is typically a toolbar
    // that can deal with the focus
    if( pWindow->ImplIsFloatingWindow() && pWindow->GetWindow( WINDOW_FIRSTCHILD ) )
        pWindow = pWindow->GetWindow( WINDOW_FIRSTCHILD );
    pWindow->GrabFocus();
}

// --------------------------------------------------

TaskPaneList::TaskPaneList()
{
}

TaskPaneList::~TaskPaneList()
{
}

// --------------------------------------------------

void TaskPaneList::AddWindow( Window *pWindow )
{
    if( pWindow )
    {
        ::std::vector< Window* >::iterator insertionPos = mTaskPanes.end();
        for ( ::std::vector< Window* >::iterator p = mTaskPanes.begin();
              p != mTaskPanes.end();
              ++p
            )
        {
            if ( *p == pWindow )
                // avoid duplicates
                return;

            // If the new window is the child of an existing pane window, or vice versa,
            // ensure that in our pane list, *first* the child window appears, *then*
            // the ancestor window.
            // This is necessary for HandleKeyEvent: There, the list is traveled from the
            // beginning, until the first window is found which has the ChildPathFocus. Now
            // if this would be the ancestor window of another pane window, this would fudge
            // the result
            if ( pWindow->IsWindowOrChild( *p ) )
            {
                insertionPos = p + 1;
                break;
            }
            if ( (*p)->IsWindowOrChild( pWindow ) )
            {
                insertionPos = p;
                break;
            }
        }

        mTaskPanes.insert( insertionPos, pWindow );
        pWindow->ImplIsInTaskPaneList( sal_True );
    }
}

// --------------------------------------------------

void TaskPaneList::RemoveWindow( Window *pWindow )
{
    ::std::vector< Window* >::iterator p;
    p = ::std::find( mTaskPanes.begin(), mTaskPanes.end(), pWindow );
    if( p != mTaskPanes.end() )
    {
        mTaskPanes.erase( p );
        pWindow->ImplIsInTaskPaneList( sal_False );
    }
}

// --------------------------------------------------

sal_Bool TaskPaneList::IsInList( Window *pWindow )
{
    ::std::vector< Window* >::iterator p;
    p = ::std::find( mTaskPanes.begin(), mTaskPanes.end(), pWindow );
    if( p != mTaskPanes.end() )
        return sal_True;
    else
        return sal_False;
}

// --------------------------------------------------

sal_Bool TaskPaneList::HandleKeyEvent( KeyEvent aKeyEvent )
{

    // F6 cycles through everything and works always

    // MAV, #i104204#
    // The old design was the following one:
    // < Ctrl-TAB cycles through Menubar, Toolbars and Floatingwindows only and is
    // < only active if one of those items has the focus
    //
    // Since the design of Ctrl-Tab looks to be inconsistent ( non-modal dialogs are not reachable
    // and the shortcut conflicts with tab-control shortcut ), it is no more supported
    sal_Bool bSplitterOnly = sal_False;
    sal_Bool bFocusInList = sal_False;
    KeyCode aKeyCode = aKeyEvent.GetKeyCode();
    sal_Bool bForward = !aKeyCode.IsShift();
    if( aKeyCode.GetCode() == KEY_F6 && ! aKeyCode.IsMod2() ) // F6
    {
        bSplitterOnly = aKeyCode.IsMod1() && aKeyCode.IsShift();

        // is the focus in the list ?
        ::std::vector< Window* >::iterator p = mTaskPanes.begin();
        while( p != mTaskPanes.end() )
        {
            Window *pWin = *p;
            if( pWin->HasChildPathFocus( sal_True ) )
            {
                bFocusInList = sal_True;

                // Ctrl-F6 goes directly to the document
                if( !pWin->IsDialog() && aKeyCode.IsMod1() && !aKeyCode.IsShift() )
                {
                    pWin->GrabFocusToDocument();
                    return sal_True;
                }

                // activate next task pane
                Window *pNextWin = NULL;

                if( bSplitterOnly )
                    pNextWin = FindNextSplitter( *p, sal_True );
                else
                    pNextWin = FindNextFloat( *p, bForward );

                if( pNextWin != pWin )
                {
                    ImplGetSVData()->maWinData.mbNoSaveFocus = sal_True;
                    ImplTaskPaneListGrabFocus( pNextWin );
                    ImplGetSVData()->maWinData.mbNoSaveFocus = sal_False;
                }
                else
                {
                    // forward key if no splitter found
                    if( bSplitterOnly )
                        return sal_False;

                    // we did not find another taskpane, so
                    // put focus back into document
                    pWin->GrabFocusToDocument();
                }

                return sal_True;
            }
            else
                ++p;
        }

        // the focus is not in the list: activate first float if F6 was pressed
        if( !bFocusInList )
        {
            Window *pWin;
            if( bSplitterOnly )
                pWin = FindNextSplitter( NULL, sal_True );
            else
                pWin = FindNextFloat( NULL, bForward );
            if( pWin )
            {
                ImplTaskPaneListGrabFocus( pWin );
                return sal_True;
            }
        }
    }

    return sal_False;
}

// --------------------------------------------------

// returns next splitter
Window* TaskPaneList::FindNextSplitter( Window *pWindow, sal_Bool bForward )
{
    if( bForward )
        ::std::stable_sort( mTaskPanes.begin(), mTaskPanes.end(), LTRSort() );
    else
        ::std::stable_sort( mTaskPanes.begin(), mTaskPanes.end(), LTRSortBackward() );

    ::std::vector< Window* >::iterator p = mTaskPanes.begin();
    while( p != mTaskPanes.end() )
    {
        if( !pWindow || *p == pWindow )
        {
            unsigned n = mTaskPanes.size();
            while( --n )
            {
                if( pWindow )   // increment before test
                    ++p;
                if( p == mTaskPanes.end() )
                    p = mTaskPanes.begin();
                if( (*p)->ImplIsSplitter() && (*p)->IsReallyVisible() && !(*p)->IsDialog() && (*p)->GetParent()->HasChildPathFocus() )
                {
                    pWindow = *p;
                    break;
                }
                if( !pWindow )  // increment after test, otherwise first element is skipped
                    ++p;
            }
            break;
        }
        else
            ++p;
    }

    return pWindow;
}

// --------------------------------------------------

// returns first valid item (regardless of type) if pWindow==0, otherwise returns next valid float
Window* TaskPaneList::FindNextFloat( Window *pWindow, sal_Bool bForward )
{
    if( bForward )
        ::std::stable_sort( mTaskPanes.begin(), mTaskPanes.end(), LTRSort() );
    else
        ::std::stable_sort( mTaskPanes.begin(), mTaskPanes.end(), LTRSortBackward() );

    ::std::vector< Window* >::iterator p = mTaskPanes.begin();
    while( p != mTaskPanes.end() )
    {
        if( !pWindow || *p == pWindow )
        {
            while( p != mTaskPanes.end() )
            {
                if( pWindow )   // increment before test
                    ++p;
                if( p == mTaskPanes.end() )
                    break; // do not wrap, send focus back to document at end of list
                /* #i83908# do not use the menubar if it is native and invisible
                   this relies on MenuBar::ImplCreate setting the height of the menubar
                   to 0 in this case
                */
                if( (*p)->IsReallyVisible() && !(*p)->ImplIsSplitter() &&
                    ( (*p)->GetType() != WINDOW_MENUBARWINDOW || (*p)->GetSizePixel().Height() > 0 )
                    )
                {
                    pWindow = *p;
                    break;
                }
                if( !pWindow )  // increment after test, otherwise first element is skipped
                    ++p;
            }
            break;
        }
        else
            ++p;
    }

    return pWindow;
}

// --------------------------------------------------

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
