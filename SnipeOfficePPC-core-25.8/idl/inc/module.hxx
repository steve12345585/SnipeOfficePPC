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

#ifndef _MODULE_HXX
#define _MODULE_HXX

#include <slot.hxx>
#include <object.hxx>

struct SvNamePos
{
    SvGlobalName    aUUId;
    sal_uInt32          nStmPos;
    SvNamePos( const SvGlobalName & rName, sal_uInt32 nPos )
        : aUUId( rName )
        , nStmPos( nPos ) {}
};

class SvMetaModule : public SvMetaExtern
{
    SvMetaClassMemberList       aClassList;
    SvMetaTypeMemberList        aTypeList;
    SvMetaAttributeMemberList   aAttrList;
// browser
    String                  aIdlFileName;
    SvString                aHelpFileName;
    SvString                aSlotIdFile;
    SvString                aTypeLibFile;
    SvString                aModulePrefix;

    sal_Bool                    bImported   : 1,
                            bIsModified : 1;
    SvGlobalName            aBeginName;
    SvGlobalName            aEndName;
    SvGlobalName            aNextName;
protected:
    virtual void        ReadAttributesSvIdl( SvIdlDataBase &, SvTokenStream & rInStm );
    virtual void        WriteAttributesSvIdl( SvIdlDataBase & rBase, SvStream & rOutStm, sal_uInt16 nTab );
    virtual void        ReadContextSvIdl( SvIdlDataBase &, SvTokenStream & rInStm );
    virtual void        WriteContextSvIdl( SvIdlDataBase & rBase, SvStream & rOutStm, sal_uInt16 nTab );
public:
                        SV_DECL_META_FACTORY1( SvMetaModule, SvMetaExtern, 13 )
                        SvMetaModule();

    const String &      GetIdlFileName() const { return aIdlFileName; }
    const rtl::OString& GetModulePrefix() const { return aModulePrefix.getString(); }

    virtual sal_Bool    SetName( const rtl::OString& rName, SvIdlDataBase * = NULL  );

    const rtl::OString& GetHelpFileName() const { return aHelpFileName.getString(); }
    const rtl::OString& GetTypeLibFileName() const { return aTypeLibFile.getString(); }

    const SvMetaAttributeMemberList & GetAttrList() const { return aAttrList; }
    const SvMetaTypeMemberList & GetTypeList() const { return aTypeList; }
    const SvMetaClassMemberList & GetClassList() const { return aClassList; }

                        SvMetaModule( const String & rIdlFileName,
                                      sal_Bool bImported );

    sal_Bool                FillNextName( SvGlobalName * );
    sal_Bool                IsImported() const { return bImported; }
    sal_Bool                IsModified() const { return bIsModified; }

    virtual sal_Bool        ReadSvIdl( SvIdlDataBase &, SvTokenStream & rInStm );
    virtual void        WriteSvIdl( SvIdlDataBase & rBase, SvStream & rOutStm, sal_uInt16 nTab );

    virtual void        WriteAttributes( SvIdlDataBase & rBase,
                                        SvStream & rOutStm, sal_uInt16 nTab,
                                            WriteType, WriteAttribute = 0 );
    virtual void        Write( SvIdlDataBase & rBase, SvStream & rOutStm, sal_uInt16 nTab,
                                    WriteType, WriteAttribute = 0 );
    virtual void        WriteSfx( SvIdlDataBase & rBase, SvStream & rOutStm );
    virtual void        WriteHelpIds( SvIdlDataBase & rBase, SvStream & rOutStm,
                                    HelpIdTable& rTable );
};
SV_DECL_IMPL_REF(SvMetaModule)
SV_DECL_IMPL_PERSIST_LIST(SvMetaModule,SvMetaModule *)


#endif // _MODULE_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
