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
#ifndef SC_SCCOMMANDS_HRC
#define SC_SCCOMMANDS_HRC

#define CMD_FID_CHG_ACCEPT                          ".uno:AcceptChanges"
#define CMD_FID_ADD_NAME                            ".uno:AddName"
#define CMD_SID_ADD_PRINTAREA                       ".uno:AddPrintArea"
#define CMD_SID_DETECTIVE_DEL_ALL                   ".uno:ClearArrows"
#define CMD_FID_COL_WIDTH                           ".uno:ColumnWidth"
#define CMD_FID_USE_NAME                            ".uno:CreateNames"
#define CMD_SID_CREATE_SW_DRAWVIEW                  ".uno:CreateSWDrawView"
#define CMD_SID_OPENDLG_CONSOLIDATE                 ".uno:DataConsolidate"
#define CMD_SID_OPENDLG_PIVOTTABLE                  ".uno:DataDataPilotRun"
#define CMD_SID_SPECIAL_FILTER                      ".uno:DataFilterSpecialFilter"
#define CMD_SID_FILTER                              ".uno:DataFilterStandardFilter"
#define CMD_SID_DATA_SELECT                         ".uno:DataSelect"
#define CMD_SID_DEFINE_DBNAME                       ".uno:DefineDBName"
#define CMD_FID_DEFINE_NAME                         ".uno:DefineName"
#define CMD_SID_DEFINE_PRINTAREA                    ".uno:DefinePrintArea"
#define CMD_FID_DELETE_CELL                         ".uno:DeleteCell"
#define CMD_FID_DEL_MANUALBREAKS                    ".uno:DeleteAllBreaks"
#define CMD_SID_DEL_COLS                            ".uno:DeleteColumns"
#define CMD_SID_PIVOT_KILL                          ".uno:DeletePivotTable"
#define CMD_SID_DELETE_PRINTAREA                    ".uno:DeletePrintArea"
#define CMD_SID_DEL_ROWS                            ".uno:DeleteRows"
#define CMD_SID_FILL_NONE                           ".uno:FillModeEnd"
#define CMD_SID_FILL_DEL_PRED                       ".uno:FillModeRemovePredescessor"
#define CMD_SID_FILL_DEL_SUCC                       ".uno:FillModeRemoveSuccessor"
#define CMD_SID_FILL_ADD_PRED                       ".uno:FillModeTracePredescessor"
#define CMD_SID_FILL_ADD_SUCC                       ".uno:FillModeTraceSuccessor"
#define CMD_FID_FILL_SERIES                         ".uno:FillSeries"
#define CMD_FID_CELL_FORMAT                         ".uno:FormatCellDialog"
#define CMD_SID_OPENDLG_SOLVE                       ".uno:GoalSeekDialog"
#define CMD_SID_OPENDLG_OPTSOLVER                   ".uno:SolverDialog"
#define CMD_FID_TABLE_HIDE                          ".uno:Hide"
#define CMD_FID_COL_HIDE                            ".uno:HideColumn"
#define CMD_FID_ROW_HIDE                            ".uno:HideRow"
#define CMD_FID_INS_TABLE                           ".uno:Insert"
#define CMD_FID_INS_CELL                            ".uno:InsertCell"
#define CMD_FID_INS_COLBRK                          ".uno:InsertColumnBreak"
#define CMD_FID_INS_COLUMN                          ".uno:InsertColumns"
#define CMD_FID_INS_CELL_CONTENTS                   ".uno:InsertContents"
#define CMD_FID_INSERT_NAME                         ".uno:InsertName"
#define CMD_FID_INS_ROWBRK                          ".uno:InsertRowBreak"
#define CMD_FID_INS_ROW                             ".uno:InsertRows"
#define CMD_FID_MERGE_ON                            ".uno:MergeCells"
#define CMD_FID_MERGE_OFF                           ".uno:SplitCell"
#define CMD_SID_OBJECT_MIRROR                       ".uno:Mirror"
#define CMD_FID_TAB_MOVE                            ".uno:Move"
#define CMD_SID_PREVIEW_NEXT                        ".uno:NextPage"
#define CMD_FID_NOTE_VISIBLE                        ".uno:NoteVisible"
#define CMD_SID_DELETE_NOTE                         ".uno:DeleteNote"
#define CMD_SID_MIRROR_HORIZONTAL                   ".uno:ObjectMirrorHorizontal"
#define CMD_SID_MIRROR_VERTICAL                     ".uno:ObjectMirrorVertical"
#define CMD_SID_ORIGINALSIZE                        ".uno:OriginalSize"
#define CMD_SID_FORMATPAGE                          ".uno:PageFormatDialog"
#define CMD_SID_PREVIEW_PREVIOUS                    ".uno:PreviousPage"
#define CMD_FID_PROTECT_TABLE                       ".uno:Protect"
#define CMD_SID_PIVOT_RECALC                        ".uno:RecalcPivotTable"
#define CMD_FID_DELETE_TABLE                        ".uno:Remove"
#define CMD_SID_TITLE_DESCRIPTION_OBJECT            ".uno:ObjectTitleDescription"
#define CMD_SID_RENAME_OBJECT                       ".uno:RenameObject"
#define CMD_SID_CELL_FORMAT_RESET                   ".uno:ResetAttributes"
#define CMD_FID_RESET_PRINTZOOM                     ".uno:ResetPrintZoom"
#define CMD_FID_ROW_HEIGHT                          ".uno:RowHeight"
#define CMD_SID_SELECT_DB                           ".uno:SelectDB"
#define CMD_SID_ANCHOR_CELL                         ".uno:SetAnchorToCell"
#define CMD_SID_ANCHOR_PAGE                         ".uno:SetAnchorToPage"
#define CMD_FID_COL_OPT_WIDTH                       ".uno:SetOptimalColumnWidth"
#define CMD_FID_ROW_OPT_HEIGHT                      ".uno:SetOptimalRowHeight"
#define CMD_FID_TABLE_SHOW                          ".uno:Show"
#define CMD_FID_COL_SHOW                            ".uno:ShowColumn"
#define CMD_FID_ROW_SHOW                            ".uno:ShowRow"
#define CMD_SID_TEXT_STANDARD                       ".uno:StandardTextAttributes"
#define CMD_SID_OPENDLG_TABOP                       ".uno:TableOperationDialog"
#define CMD_FID_TAB_MENU_RENAME                     ".uno:RenameTable"
#define CMD_FID_TAB_SELECTALL                       ".uno:TableSelectAll"
#define CMD_FID_TAB_DESELECTALL                     ".uno:TableDeselectAll"
#define CMD_SID_DRAWTEXT_ATTR_DLG                   ".uno:TextAttributes"
#define CMD_SID_ASSIGNMACRO                         ".uno:AssignMacro"
#define CMD_SID_ULINE_VAL_DOUBLE                    ".uno:UnderlineDouble"
#define CMD_SID_DP_FILTER                           ".uno:DataPilotFilter"
#define CMD_FID_TAB_RTL                             ".uno:SheetRightToLeft"
#define CMD_SID_SHARE_DOC                           ".uno:ShareDocument"
#define CMD_FID_TAB_EVENTS                          ".uno:TableEvents"
#define CMD_FID_TAB_MENU_SET_TAB_BG_COLOR           ".uno:SetTabBgColor"
#define CMD_FID_TAB_SET_TAB_BG_COLOR                ".uno:TabBgColor"
#define CMD_SID_MANAGE_XML_SOURCE                   ".uno:ManageXMLSource"

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
