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

#ifndef _ANNOTATIONWIN_HXX
#define _ANNOTATIONWIN_HXX

#include <SidebarWin.hxx>

class PopupMenu;
class OutlinerParaObject;

namespace sw { namespace annotation {

class SwAnnotationWin : public sw::sidebarwindows::SwSidebarWin
{
    public:
        SwAnnotationWin( SwEditWin& rEditWin,
                         WinBits nBits,
                         SwPostItMgr& aMgr,
                         SwPostItBits aBits,
                         SwSidebarItem& rSidebarItem,
                         SwFmtFld* aField );
        virtual ~SwAnnotationWin();

        virtual void    UpdateData();
        virtual void    SetPostItText();
        virtual void    Delete();
        virtual void    GotoPos();

        virtual String  GetAuthor();
        virtual Date    GetDate();
        virtual Time    GetTime();

        virtual sal_uInt32 MoveCaret();

        virtual bool    CalcFollow();
        void            InitAnswer(OutlinerParaObject* pText);

        virtual bool    IsProtected();

    protected:
        virtual MenuButton* CreateMenuButton();

    private:
        virtual SvxLanguageItem GetLanguage(void);
        sal_uInt32 CountFollowing();

        SwFmtFld*       mpFmtFld;
        SwPostItField*  mpFld;
        PopupMenu*      mpButtonPopup;

};

} } // end of namespace sw::annotation
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
