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
#ifndef _SWRENAMEXNAMEDDLG_HXX
#define _SWRENAMEXNAMEDDLG_HXX


#include <vcl/dialog.hxx>
#include <vcl/fixed.hxx>
#include <vcl/edit.hxx>
#include <vcl/button.hxx>
#include <actctrl.hxx>
#include <com/sun/star/frame/XController.hpp>
#include <com/sun/star/text/XTextCursor.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/awt/XControl.hpp>
#include <com/sun/star/container/XNamed.hpp>

class SwRenameXNamedDlg : public ModalDialog
{
    FixedLine       aNameFL;
    FixedText       aNewNameFT;
    NoSpaceEdit     aNewNameED;
    OKButton        aOk;
    CancelButton    aCancel;
    HelpButton      aHelp;

    String          sRemoveWarning;

    ::com::sun::star::uno::Reference< ::com::sun::star::container::XNamed > &   xNamed;
    ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess > & xNameAccess;
    ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess >   xSecondAccess;
    ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess >   xThirdAccess;

    DECL_LINK(OkHdl, void *);
    DECL_LINK(ModifyHdl, NoSpaceEdit*);

public:
    SwRenameXNamedDlg( Window* pParent,
                    ::com::sun::star::uno::Reference< ::com::sun::star::container::XNamed > & xNamed,
                    ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess > & xNameAccess );

    void    SetForbiddenChars( const String& rSet )
        { aNewNameED.SetForbiddenChars( rSet ); }

    void SetAlternativeAccess(
            ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess > & xSecond,
            ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess > & xThird )
    {
        xSecondAccess = xSecond;
        xThirdAccess = xThird;
    }
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
