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
#ifndef SC_IANYREFDIALOG_HXX_INCLUDED
#define SC_IANYREFDIALOG_HXX_INCLUDED

#include <formula/IControlReferenceHandler.hxx>

class ScRange;
class ScDocument;
class ScTabViewShell;
class SfxObjectShell;
namespace formula
{
    class RefEdit;
    class RefButton;
}
class SAL_NO_VTABLE IAnyRefDialog : public formula::IControlReferenceHandler
{
public:
    virtual ~IAnyRefDialog(){}

    virtual void SetReference( const ScRange& rRef, ScDocument* pDoc ) = 0;
    virtual void RefInputStart( formula::RefEdit* pEdit, formula::RefButton* pButton = NULL ) = 0;
    virtual void RefInputDone( sal_Bool bForced = false ) = 0;
    virtual sal_Bool IsTableLocked() const = 0;
    virtual sal_Bool IsRefInputMode() const = 0;

    virtual sal_Bool IsDocAllowed( SfxObjectShell* pDocSh ) const = 0;
    virtual void AddRefEntry() = 0;
    virtual void SetActive() = 0;
    virtual void ViewShellChanged() = 0;
};

#endif // SC_IANYREFDIALOG_HXX_INCLUDED

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
