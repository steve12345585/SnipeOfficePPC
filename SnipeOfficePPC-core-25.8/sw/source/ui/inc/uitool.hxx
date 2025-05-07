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
#ifndef _UITOOL_HXX
#define _UITOOL_HXX

#include <tools/wintypes.hxx>
#include <vcl/field.hxx>
#include <swtypes.hxx>
#include "swdllapi.h"

class MetricFormatter;
class SfxItemSet;
class SfxMedium;
class SwPageDesc;
class SvxTabStopItem;
class SwWrtShell;
class ListBox;
class SwDocShell;
class SwFrmFmt;
class SwTabCols;
class DateTime;
class SfxViewFrame;

// switch a metric
SW_DLLPUBLIC void SetMetric(MetricFormatter& rCtrl, FieldUnit eUnit);

// fill BoxInfoAttribut
SW_DLLPUBLIC void PrepareBoxInfo(SfxItemSet& rSet, const SwWrtShell& rSh);

// SfxItemSets <-> PageDesc
void ItemSetToPageDesc( const SfxItemSet& rSet, SwPageDesc& rPageDesc );
void PageDescToItemSet( const SwPageDesc& rPageDesc, SfxItemSet& rSet);

// fill tabs with default tabs
SW_DLLPUBLIC void   MakeDefTabs(SwTwips nDefDist, SvxTabStopItem& rTabs);

// erase DefaultTabs from TabStopArray
//void  EraseDefTabs(SvxTabStopItem& rTabs);

// determine space between 1st and 2nd element
SW_DLLPUBLIC sal_uInt16     GetTabDist(const SvxTabStopItem& rTabs);

// determine whether a Sfx-PageDesc combination exists in the set
// and set this in the set and delete the transport items
// (PageBreak & PageModel) from the set
void SwToSfxPageDescAttr( SfxItemSet& rSet );
void SfxToSwPageDescAttr( const SwWrtShell& rShell, SfxItemSet& rSet );

SW_DLLPUBLIC FieldUnit  GetDfltMetric(sal_Bool bWeb);
void        SetDfltMetric(FieldUnit eMetric, sal_Bool bWeb);

SW_DLLPUBLIC sal_Bool HasCharUnit( sal_Bool bWeb );
void SetApplyCharUnit(sal_Bool bApplyChar, sal_Bool bWeb);

// ListBox mit allen Zeichenvorlagen fuellen - ausser Standard!
SW_DLLPUBLIC void FillCharStyleListBox(ListBox& rToFill, SwDocShell* pDocSh, sal_Bool bSorted = sal_False, sal_Bool bWithDefault = sal_False);

//inserts a string sorted into a ListBox,
SW_DLLPUBLIC sal_uInt16 InsertStringSorted(const String& rEntry, ListBox& rToFill, sal_uInt16 nOffset);

// Tabellenbreite und Ausrichtung ermitteln
SwTwips GetTableWidth( SwFrmFmt* pFmt, SwTabCols& rCols, sal_uInt16 *pPercent,
        SwWrtShell* pSh );

String GetAppLangDateTimeString( const DateTime& );

// search for a command string withing the menu structure and execute it
// at the dispatcher if there is one, if executed return true
bool ExecuteMenuCommand( PopupMenu& rMenu, SfxViewFrame& rViewFrame, sal_uInt16 nId );

#endif // _UITOOL_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
