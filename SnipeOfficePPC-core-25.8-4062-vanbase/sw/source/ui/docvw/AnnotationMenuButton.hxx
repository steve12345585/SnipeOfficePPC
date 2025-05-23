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

#ifndef _ANNOTATIONMENUBUTTON_HXX
#define _ANNOTATIONMENUBUTTON_HXX

#include <vcl/menubtn.hxx>

namespace sw { namespace sidebarwindows {
    class SwSidebarWin;
} }

namespace sw { namespace annotation {

class AnnotationMenuButton : public MenuButton
{
    public:
        AnnotationMenuButton( sw::sidebarwindows::SwSidebarWin& rSidebarWin );
        ~AnnotationMenuButton();

        // overloaded <MenuButton> methods
        virtual void Select();

        // overloaded <Window> methods
        virtual void MouseButtonDown( const MouseEvent& rMEvt );
        virtual void Paint( const Rectangle& rRect );
        virtual void KeyInput( const KeyEvent& rKeyEvt );

    private:
        sw::sidebarwindows::SwSidebarWin& mrSidebarWin;
};

} } // end of namespace sw::annotation

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
