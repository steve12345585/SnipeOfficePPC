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

#ifndef _GLOBALS_HXX
#define _GLOBALS_HXX

#include <hash.hxx>

#ifdef ICC
#undef _Export
#endif

class SvIdlWorkingBase;
class SvClassManager;
struct SvGlobalHashNames
{
    SvStringHashEntryRef MM_Name;
    SvStringHashEntryRef MM_module;
    SvStringHashEntryRef MM_interface;
    SvStringHashEntryRef MM_in;
    SvStringHashEntryRef MM_out;
    SvStringHashEntryRef MM_inout;
    SvStringHashEntryRef MM_String;
    SvStringHashEntryRef MM_UCHAR;
    SvStringHashEntryRef MM_USHORT;
    SvStringHashEntryRef MM_uuid;
    SvStringHashEntryRef MM_HelpContext;
    SvStringHashEntryRef MM_HelpText;
    SvStringHashEntryRef MM_void;
    SvStringHashEntryRef MM_shell;
    SvStringHashEntryRef MM_Get;
    SvStringHashEntryRef MM_Set;
    SvStringHashEntryRef MM_SlotId;
    SvStringHashEntryRef MM_HasCoreId;
    SvStringHashEntryRef MM_Cachable;
    SvStringHashEntryRef MM_Volatile;
    SvStringHashEntryRef MM_Toggle;
    SvStringHashEntryRef MM_AutoUpdate;
    SvStringHashEntryRef MM_Synchron;
    SvStringHashEntryRef MM_Asynchron;
    SvStringHashEntryRef MM_RecordPerSet;
    SvStringHashEntryRef MM_RecordPerItem;
    SvStringHashEntryRef MM_RecordManual;
    SvStringHashEntryRef MM_NoRecord;
    SvStringHashEntryRef MM_RecordAbsolute;
    SvStringHashEntryRef MM_enum;
    SvStringHashEntryRef MM_UINT16;
    SvStringHashEntryRef MM_INT16;
    SvStringHashEntryRef MM_UINT32;
    SvStringHashEntryRef MM_INT32;
    SvStringHashEntryRef MM_int;
    SvStringHashEntryRef MM_BOOL;
    SvStringHashEntryRef MM_char;
    SvStringHashEntryRef MM_BYTE;
    SvStringHashEntryRef MM_float;
    SvStringHashEntryRef MM_double;
    SvStringHashEntryRef MM_item;
    SvStringHashEntryRef MM_PseudoSlots;
    SvStringHashEntryRef MM_map;
    SvStringHashEntryRef MM_Default;
    SvStringHashEntryRef MM_HelpFile;
    SvStringHashEntryRef MM_Version;
    SvStringHashEntryRef MM_import;
    SvStringHashEntryRef MM_SlotIdFile;
    SvStringHashEntryRef MM_SvName;
    SvStringHashEntryRef MM_SbxName;
    SvStringHashEntryRef MM_ItemName;
    SvStringHashEntryRef MM_OdlName;
    SvStringHashEntryRef MM_include;
    SvStringHashEntryRef MM_ExecMethod;
    SvStringHashEntryRef MM_StateMethod;
    SvStringHashEntryRef MM_GroupId;
    SvStringHashEntryRef MM_HasDialog;
    SvStringHashEntryRef MM_TypeLibFile;
    SvStringHashEntryRef MM_Export;
    SvStringHashEntryRef MM_Automation;
    SvStringHashEntryRef MM_PseudoPrefix;
    SvStringHashEntryRef MM_define;
    SvStringHashEntryRef MM_MenuConfig;
    SvStringHashEntryRef MM_ToolBoxConfig;
    SvStringHashEntryRef MM_StatusBarConfig;
    SvStringHashEntryRef MM_AccelConfig;
    SvStringHashEntryRef MM_AllConfig;
    SvStringHashEntryRef MM_FastCall;
    SvStringHashEntryRef MM_SbxObject;
    SvStringHashEntryRef MM_Container;
    SvStringHashEntryRef MM_ImageRotation;
    SvStringHashEntryRef MM_ImageReflection;
    SvStringHashEntryRef MM_IsCollection;
    SvStringHashEntryRef MM_ReadOnlyDoc;
    SvStringHashEntryRef MM_ConfigName;
    SvStringHashEntryRef MM_union;
    SvStringHashEntryRef MM_struct;
    SvStringHashEntryRef MM_typedef;
    SvStringHashEntryRef MM_Readonly;
    SvStringHashEntryRef MM_SlotType;
    SvStringHashEntryRef MM_ModulePrefix;
    SvStringHashEntryRef MM_DisableFlags;
    SvStringHashEntryRef MM_Hidden;
    SvStringHashEntryRef MM_Description;
    SvStringHashEntryRef MM_UnoName;

    SvGlobalHashNames();
};

class IdlDll
{
public:
    SvStringHashTable * pHashTable;
    SvClassManager    * pClassMgr;
    SvGlobalHashNames * pGlobalNames;

                IdlDll();
                ~IdlDll();
};

IdlDll * GetIdlApp();
#define IDLAPP GetIdlApp()

#define SV_GLOBAL_HASH_ACCESS( Name )                           \
    if( !IDLAPP->pGlobalNames )                                 \
        IDLAPP->pGlobalNames = new SvGlobalHashNames();         \
    return IDLAPP->pGlobalNames->MM_##Name;

#define HASH_INLINE( Name )                                             \
inline SvStringHashEntry * SvHash_##Name() { SV_GLOBAL_HASH_ACCESS( Name ) }

HASH_INLINE(Name)
HASH_INLINE(module)
HASH_INLINE(interface)
HASH_INLINE(in)
HASH_INLINE(out)
HASH_INLINE(inout)
HASH_INLINE(String)
HASH_INLINE(UCHAR)
HASH_INLINE(USHORT)
HASH_INLINE(uuid)
HASH_INLINE(HelpContext)
HASH_INLINE(HelpText)
HASH_INLINE(void)
HASH_INLINE(shell)
HASH_INLINE(Get)
HASH_INLINE(Set)
HASH_INLINE(SlotId)
HASH_INLINE(HasCoreId)
HASH_INLINE(Cachable)
HASH_INLINE(Volatile)
HASH_INLINE(Toggle)
HASH_INLINE(AutoUpdate)
HASH_INLINE(Synchron)
HASH_INLINE(Asynchron)
HASH_INLINE(RecordPerItem)
HASH_INLINE(RecordPerSet)
HASH_INLINE(RecordManual)
HASH_INLINE(NoRecord)
HASH_INLINE(RecordAbsolute)
HASH_INLINE(enum)
HASH_INLINE(UINT16)
HASH_INLINE(INT16)
HASH_INLINE(UINT32)
HASH_INLINE(INT32)
HASH_INLINE(int)
HASH_INLINE(BOOL)
HASH_INLINE(char)
HASH_INLINE(BYTE)
HASH_INLINE(item)
HASH_INLINE(PseudoSlots)
HASH_INLINE(map)
HASH_INLINE(Default)
HASH_INLINE(HelpFile)
HASH_INLINE(Version)
HASH_INLINE(import)
HASH_INLINE(SlotIdFile)
HASH_INLINE(SvName)
HASH_INLINE(SbxName)
HASH_INLINE(ItemName)
HASH_INLINE(OdlName)
HASH_INLINE(include)
HASH_INLINE(ExecMethod)
HASH_INLINE(StateMethod)
HASH_INLINE(GroupId)
HASH_INLINE(float)
HASH_INLINE(double)
HASH_INLINE(HasDialog)
HASH_INLINE(TypeLibFile)
HASH_INLINE(Export)
HASH_INLINE(Automation)
HASH_INLINE(PseudoPrefix)
HASH_INLINE(define)
HASH_INLINE(MenuConfig)
HASH_INLINE(ToolBoxConfig)
HASH_INLINE(StatusBarConfig)
HASH_INLINE(AccelConfig)
HASH_INLINE(AllConfig)
HASH_INLINE(FastCall)
HASH_INLINE(SbxObject)
HASH_INLINE(Container)
HASH_INLINE(ImageRotation)
HASH_INLINE(ImageReflection)
HASH_INLINE(IsCollection)
HASH_INLINE(ReadOnlyDoc)
HASH_INLINE(ConfigName)
HASH_INLINE(union)
HASH_INLINE(struct)
HASH_INLINE(typedef)
HASH_INLINE(Readonly)
HASH_INLINE(SlotType)
HASH_INLINE(ModulePrefix)
HASH_INLINE(DisableFlags)
HASH_INLINE(Hidden)
HASH_INLINE(Description)
HASH_INLINE(UnoName)


#endif // _GLOBALS_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
