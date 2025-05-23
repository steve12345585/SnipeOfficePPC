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

#ifndef SD_OUTLINE_BULLET_DLG_HXX
#define SD_OUTLINE_BULLET_DLG_HXX

#include <sfx2/tabdlg.hxx>

#include "dlgolbul.hrc"

namespace sd {

class View;

/*************************************************************************
|*
|* Bullet-Tab-Dialog
|*
\************************************************************************/
class OutlineBulletDlg
    : public SfxTabDialog
{
public:
    OutlineBulletDlg (
        ::Window* pParent,
        const SfxItemSet* pAttr,
        ::sd::View* pView );
    virtual ~OutlineBulletDlg (void);

    const SfxItemSet* GetOutputItemSet() const;

protected:
    virtual void PageCreated( sal_uInt16 nId, SfxTabPage &rPage );

private:
    using SfxTabDialog::GetOutputItemSet;

    SfxItemSet  aInputSet;
    SfxItemSet  *pOutputSet;
    sal_Bool        bTitle;
    ::sd::View      *pSdView;
};

} // end of namespace sd

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
