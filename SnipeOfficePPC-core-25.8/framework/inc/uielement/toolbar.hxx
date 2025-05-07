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

//_________________________________________________________________________________________________________________
//  my own includes
//_________________________________________________________________________________________________________________


#ifndef __FRAMEWORK_UIELEMENT_TOOLBAR_HXX_
#define __FRAMEWORK_UIELEMENT_TOOLBAR_HXX_

#include <vcl/toolbox.hxx>

namespace framework
{

class ToolBarManager;
class ToolBar : public ToolBox
{
    public:
                        ToolBar( Window* pParent, WinBits nWinBits );
        virtual         ~ToolBar();

        virtual void    Command ( const CommandEvent& rCEvt );
        virtual void    StateChanged( StateChangedType nType );
        virtual void    DataChanged( const DataChangedEvent& rDCEvt );

        // Toolbar manager
        void            SetToolBarManager( ToolBarManager* pTbMgr );
        ToolBarManager* GetToolBarManager() const { return m_pToolBarManager; }

        // Provide additional handlers to support external implementations
        void            SetCommandHdl( const Link& aLink ) { m_aCommandHandler = aLink; }
        const Link&     GetCommandHdl() const { return m_aCommandHandler; }
        void            SetStateChangedHdl( const Link& aLink ) { m_aStateChangedHandler = aLink; }
        const Link&     GetStateChangedHdl() const { return m_aStateChangedHandler; }
        void            SetDataChangedHdl( const Link& aLink ) { m_aDataChangedHandler = aLink; }
        const Link&     GetDataChangedHdl() { return m_aDataChangedHandler; }

    private:
        Link            m_aCommandHandler;
        Link            m_aStateChangedHandler;
        Link            m_aDataChangedHandler;
        ToolBarManager* m_pToolBarManager;
};

}

#endif // __FRAMEWORK_UIELEMENT_TOOLBAR_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
