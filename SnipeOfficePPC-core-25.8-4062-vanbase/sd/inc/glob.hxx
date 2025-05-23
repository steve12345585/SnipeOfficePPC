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

#ifndef SD_GLOB_HXX
#define SD_GLOB_HXX

#include <tools/solar.h>

#include <rsc/rscsfx.hxx>

//------------------------------------------------------------------

#define SD_IF_SDAPP                     SFX_INTERFACE_SD_START + 0
#define SD_IF_SDDRAWDOCSHELL            SFX_INTERFACE_SD_START + 1
#define SD_IF_SDVIEWSHELL               SFX_INTERFACE_SD_START + 2
#define SD_IF_SDDRAWVIEWSHELL           SFX_INTERFACE_SD_START + 3
#define SD_IF_SDOUTLINEVIEWSHELL        SFX_INTERFACE_SD_START + 5
#define SD_IF_SDDRAWSTDOBJECTBAR        SFX_INTERFACE_SD_START + 6
#define SD_IF_SDDRAWTEXTOBJECTBAR       SFX_INTERFACE_SD_START + 7
#define SD_IF_SDDRAWBEZIEROBJECTBAR     SFX_INTERFACE_SD_START + 8
#define SD_IF_SDDRAWGLUEPOINTSOBJECTBAR SFX_INTERFACE_SD_START + 9
#define SD_IF_SDGRAPHICDOCSHELL         SFX_INTERFACE_SD_START + 10
#define SD_IF_SDGRAPHICVIEWSHELL        SFX_INTERFACE_SD_START + 11
#define SD_IF_SDGRAPHICSTDOBJECTBAR     SFX_INTERFACE_SD_START + 12
#define SD_IF_SDDRAWGRAFOBJECTBAR       SFX_INTERFACE_SD_START + 13
#define SD_IF_SDPRESVIEWSHELL           SFX_INTERFACE_SD_START + 14
#define SD_IF_SDPREVIEWVIEWSHELL        SFX_INTERFACE_SD_START + 15
#define SD_IF_SDVIEWSHELLBASE           SFX_INTERFACE_SD_START + 16
#define SD_IF_SD3DOBJECTBAR             SFX_INTERFACE_SD_START + 17
#define SD_IF_SDFONTWORKOBJECTBAR       SFX_INTERFACE_SD_START + 18
#define SD_IF_SDSLIDESORTERVIEWSHELL    SFX_INTERFACE_SD_START + 19
#define SD_IF_SDTASKPANEVIEWSHELL       SFX_INTERFACE_SD_START + 20
#define SD_IF_SDMASTERPAGESSELECTOR     SFX_INTERFACE_SD_START + 21
#define SD_IF_SDLAYOUTMENU              SFX_INTERFACE_SD_START + 22
#define SD_IF_SDDRAWMEDIAOBJECTBAR      SFX_INTERFACE_SD_START + 23
#define SD_IF_SDLEFTIMPRESSPANESHELL    SFX_INTERFACE_SD_START + 24
#define SD_IF_SDLEFTDRAWPANESHELL       SFX_INTERFACE_SD_START + 25
#define SD_IF_SDRIGHTPANESHELL          SFX_INTERFACE_SD_START + 26
#define SD_IF_SDDRAWTABLEOBJECTBAR      SFX_INTERFACE_SD_START + 27
#define SD_IF_SDANNOTATIONSHELL         SFX_INTERFACE_SD_START + 28
#define SD_IF_SDTOOLPANELPANESHELL      SFX_INTERFACE_SD_START + 29
#define SD_IF_SDTOOLPANELSHELL          SFX_INTERFACE_SD_START + 30

// Inventor-Id fuer StarDraw UserData
const sal_uInt32 SdUDInventor=sal_uInt32('S')*0x00000001+
                          sal_uInt32('D')*0x00000100+
                          sal_uInt32('U')*0x00010000+
                          sal_uInt32('D')*0x01000000;

// Object-Ids fuer StarDraw UserData
#define SD_ANIMATIONINFO_ID 1
#define SD_IMAPINFO_ID      2

// FamilyId der Praesentationsvorlagen
#define SD_STYLE_FAMILY_GRAPHICS        SFX_STYLE_FAMILY_PARA
#define SD_STYLE_FAMILY_PSEUDO          SFX_STYLE_FAMILY_PSEUDO
#define SD_STYLE_FAMILY_CELL            SFX_STYLE_FAMILY_FRAME
#define SD_STYLE_FAMILY_MASTERPAGE      SFX_STYLE_FAMILY_PAGE       // ex LT_FAMILY

// Trennzeichen zwischen Layoutname und Vorlagenname der Praesentationsvorlagen
#define SD_LT_SEPARATOR "~LT~"

// Optionsstream-Identifier
#define SD_OPTION_MORPHING  "Morph"
#define SD_OPTION_VECTORIZE "Vectorize"

//------------------------------------------------------------------

#endif // _SD_GLOB_HXX


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
