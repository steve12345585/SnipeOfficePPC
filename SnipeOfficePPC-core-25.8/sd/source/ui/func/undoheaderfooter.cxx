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



#include <sfx2/dispatch.hxx>
#include <sfx2/viewfrm.hxx>

#include "app.hrc"
#include "DrawViewShell.hxx"
#include "drawview.hxx"
#include "undoheaderfooter.hxx"

TYPEINIT1(SdHeaderFooterUndoAction, SdUndoAction);

SdHeaderFooterUndoAction::SdHeaderFooterUndoAction( SdDrawDocument* pDoc, SdPage* pPage, const sd::HeaderFooterSettings& rNewSettings )
:   SdUndoAction(pDoc),
    mpPage(pPage),
    maOldSettings(pPage->getHeaderFooterSettings()),
    maNewSettings(rNewSettings)
{
}

SdHeaderFooterUndoAction::~SdHeaderFooterUndoAction()
{
}

void SdHeaderFooterUndoAction::Undo()
{
    mpPage->setHeaderFooterSettings( maOldSettings );
    SfxViewFrame::Current()->GetDispatcher()->Execute( SID_SWITCHPAGE, SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD );
}

void SdHeaderFooterUndoAction::Redo()
{
    mpPage->setHeaderFooterSettings( maNewSettings );
    SfxViewFrame::Current()->GetDispatcher()->Execute( SID_SWITCHPAGE, SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
