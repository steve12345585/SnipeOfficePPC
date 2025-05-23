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
#ifndef _DBTABLEPREVIEWDIALOG_HXX
#define _DBTABLEPREVIEWDIALOG_HXX

#include <sfx2/basedlgs.hxx>
#include <svtools/stdctrl.hxx>
#include <vcl/button.hxx>
#include <com/sun/star/uno/Sequence.h>

namespace com{ namespace sun{ namespace star{
    namespace beans{  struct PropertyValue; }
    namespace frame{ class XFrame;     }
    }}}

class SwDBTablePreviewDialog : public SfxModalDialog
{
    FixedInfo       m_aDescriptionFI;
    Window*         m_pBeamerWIN;
    OKButton        m_aOK;

    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >         m_xFrame;
public:
    SwDBTablePreviewDialog(Window* pParent,
            ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue>& rValues  );
    ~SwDBTablePreviewDialog();

};
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
