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

#ifndef SVTOOLS_SOURCE_TABLE_TABLEDATAWINDOW_HXX
#define SVTOOLS_SOURCE_TABLE_TABLEDATAWINDOW_HXX

#include <vcl/window.hxx>
#include <vcl/seleng.hxx>

//........................................................................
namespace svt { namespace table
{
//........................................................................

    class TableControl_Impl;
    class TableFunctionSet;


    //====================================================================
    //= TableDataWindow
    //====================================================================
    /** the window containing the content area (including headers) of
        a table control
    */
    class TableDataWindow : public Window
    {
        friend class TableFunctionSet;
    private:
        TableControl_Impl&  m_rTableControl;
        Link                m_aSelectHdl;
        sal_uLong           m_nTipWindowHandle;

    public:
        TableDataWindow( TableControl_Impl& _rTableControl );
        ~TableDataWindow();

        inline void         SetSelectHdl( const Link& rLink )   { m_aSelectHdl = rLink; }
        inline const Link&  GetSelectHdl() const                { return m_aSelectHdl; }

        // Window overridables
        virtual void        Paint( const Rectangle& rRect );
        virtual void        MouseMove( const MouseEvent& rMEvt);
        virtual void        MouseButtonDown( const MouseEvent& rMEvt);
        virtual void        MouseButtonUp( const MouseEvent& rMEvt);
        virtual long        Notify(NotifyEvent& rNEvt);
        virtual void        SetControlBackground(const Color& rColor);
        virtual void        SetControlBackground();
        virtual void        RequestHelp( const HelpEvent& rHEvt );

        void                SetBackground(const Wallpaper& rColor);

    private:
        void    impl_hideTipWindow();
    };
//........................................................................
} } // namespace svt::table
//........................................................................

#endif // SVTOOLS_SOURCE_TABLE_TABLEDATAWINDOW_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
