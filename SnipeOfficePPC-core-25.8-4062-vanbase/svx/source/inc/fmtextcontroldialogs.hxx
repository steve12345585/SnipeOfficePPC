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

#ifndef SVX_SOURCE_FORM_FMTEXTCONTROLDIALOGS_HXX
#define SVX_SOURCE_FORM_FMTEXTCONTROLDIALOGS_HXX

#include <sfx2/sfxsids.hrc>
#include <sfx2/tabdlg.hxx>
#include <editeng/flstitem.hxx>

//........................................................................
namespace svx
{
//........................................................................

    //====================================================================
    //= TextControlCharAttribDialog
    //====================================================================
    class TextControlCharAttribDialog : public SfxTabDialog
    {
    private:
        SvxFontListItem m_aFontList;

    public:
        TextControlCharAttribDialog( Window* pParent, const SfxItemSet& _rCoreSet, const SvxFontListItem& _rFontList );
        ~TextControlCharAttribDialog();

    protected:
        virtual void PageCreated( sal_uInt16 _nId, SfxTabPage& _rPage );
    };

    //====================================================================
    //= TextControlParaAttribDialog
    //====================================================================
    class TextControlParaAttribDialog : public SfxTabDialog
    {
    public:
        TextControlParaAttribDialog( Window* _pParent, const SfxItemSet& _rCoreSet );
        ~TextControlParaAttribDialog();
    };
//........................................................................
} // namespace svx
//........................................................................

#endif // SVX_SOURCE_FORM_FMTEXTCONTROLDIALOGS_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
