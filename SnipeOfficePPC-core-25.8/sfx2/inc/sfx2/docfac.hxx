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
#ifndef _SFX_OBJFAC_HXX
#define _SFX_OBJFAC_HXX

#include "sal/config.h"
#include "sfx2/dllapi.h"
#include <sal/types.h>
#include <tools/solar.h>
#include <tools/rtti.hxx>

// SFX_IMPL_MODULE_LIB
#include <osl/module.hxx>
#include <rtl/ustring.hxx>

#include <sfx2/objsh.hxx>
#include <sfx2/sfxdefs.hxx>

class SfxMedium;
class SfxFilter;
class SfxViewFactory;
struct SfxObjectFactory_Impl;
class SfxFilterContainer;
class SfxBindings;

//====================================================================

typedef void (*SfxVoidFunc)();

#define SDT_SDT_DOCFACTPRIO     10      // Explorer
#define SDT_SW_DOCFACTPRIO      20      // Text Document
#define SDT_SWW_DOCFACTPRIO     21      // Web Document
#define SDT_SC_DOCFACTPRIO      30      // Table Document
#define SDT_SI_DOCFACTPRIO      40      // Impress Document
#define SDT_SD_DOCFACTPRIO      41      // Draw Document
#define SDT_SCH_DOCFACTPRIO     50      // Chart Document
#define SDT_SMA_DOCFACTPRIO     60      // Math Document
#define SDT_SIM_DOCFACTPRIO     70      // Image Document
#define SDT_FRM_DOCFACTPRIO    100      // Frame Document
#define SDT_MSG_DOCFACTPRIO    110      // Message Document
#define SDT_SDB_DOCFACTPRIO    200      // Database Document

//====================================================================

class SFX2_DLLPUBLIC SfxObjectFactory
{
private:
    const char*             pShortName;
    SfxObjectFactory_Impl*  pImpl;      // Additional Data
    SfxObjectShellFlags     nFlags;
    SAL_DLLPRIVATE void Construct();

public:
    SfxObjectFactory( const SvGlobalName &rName, SfxObjectShellFlags nFlags, const char* pShortName );
    ~SfxObjectFactory();

    const SvGlobalName& GetClassId() const;
    SfxObjectShellFlags GetFlags() { return nFlags; }
    const char*     GetShortName() const { return pShortName; }
    String          GetFactoryURL() const;  // shortcut for "private:factory/GetShortName()"
    String          GetFactoryName() const { return String::CreateFromAscii( pShortName ); }
    String          GetModuleName() const;
    SfxFilterContainer *GetFilterContainer( sal_Bool bForceLoad = sal_True) const;

    // Views
    void            RegisterViewFactory(SfxViewFactory &rFactory);
    sal_uInt16          GetViewFactoryCount() const;
    SfxViewFactory& GetViewFactory(sal_uInt16 i = 0) const;

    /// returns the view factory whose GetAPIViewName or GetLegacyViewName delivers the requested logical name
    SfxViewFactory* GetViewFactoryByViewName( const String& i_rViewName ) const;

    // Filter
    const SfxFilter* GetTemplateFilter() const;
    static String   GetStandardTemplate( const String& rServiceName );
    static void     SetStandardTemplate( const String& rServiceName, const String& rTemplateName );
    static void     SetSystemTemplate( const String& rServiceName, const String& rTemplateName );

    void            SetDocumentServiceName( const rtl::OUString& rServiceName );
    const rtl::OUString&    GetDocumentServiceName() const;

    SfxModule*      GetModule() const;

    SAL_DLLPRIVATE void SetModule_Impl( SfxModule* );
    SAL_DLLPRIVATE static void UpdateFilterContainers_Impl();
    SAL_DLLPRIVATE sal_uInt16 GetViewNo_Impl( const sal_uInt16 i_nViewId, const sal_uInt16 i_nFallback ) const;

private:
    // Kopieren verboten
    SAL_DLLPRIVATE SfxObjectFactory(const SfxObjectFactory&);
    SAL_DLLPRIVATE const SfxObjectFactory& operator=(const SfxObjectFactory &);
};

//=========================================================================

#define SFX_DECL_OBJECTFACTORY()                                            \
public:                                                                     \
    static SfxObjectFactory&    Factory();                                  \
    virtual SfxObjectFactory&   GetFactory() const { return Factory(); }

#define SFX_IMPL_OBJECTFACTORY(ClassName,GlobName,Flags,ShortName)          \
    SfxObjectFactory& ClassName::Factory()                                  \
    {                                                                       \
        static SfxObjectFactory aObjectFactory(GlobName, Flags, ShortName); \
        return aObjectFactory;                                              \
    }
#endif // #ifndef _SFX_OBJFAC_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
