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

#include <stdio.h>
#include <tools/rcid.h>

#include <cstdarg>
#include <sfx2/module.hxx>
#include <sfx2/app.hxx>
#include "arrdecl.hxx"
#include "sfx2/sfxresid.hxx"
#include <sfx2/msgpool.hxx>
#include <sfx2/tbxctrl.hxx>
#include "sfx2/stbitem.hxx"
#include <sfx2/mnuitem.hxx>
#include <sfx2/childwin.hxx>
#include <sfx2/mnumgr.hxx>
#include <sfx2/docfac.hxx>
#include <sfx2/objface.hxx>
#include <sfx2/viewfrm.hxx>
#include <svl/intitem.hxx>
#include "sfx2/taskpane.hxx"
#include <tools/diagnose_ex.h>
#include <rtl/strbuf.hxx>
#include <sal/log.hxx>

#define SfxModule
#include "sfxslots.hxx"

static SfxModuleArr_Impl* pModules=0;

class SfxModule_Impl
{
public:

    SfxSlotPool*                pSlotPool;
    SfxTbxCtrlFactArr_Impl*     pTbxCtrlFac;
    SfxStbCtrlFactArr_Impl*     pStbCtrlFac;
    SfxMenuCtrlFactArr_Impl*    pMenuCtrlFac;
    SfxChildWinFactArr_Impl*    pFactArr;
    ImageList*                  pImgListSmall;
    ImageList*                  pImgListBig;

                                SfxModule_Impl();
                                ~SfxModule_Impl();
    ImageList*                  GetImageList( ResMgr* pResMgr, bool bBig );
};

SfxModule_Impl::SfxModule_Impl()
 : pSlotPool(0)
{
}

SfxModule_Impl::~SfxModule_Impl()
{
    delete pSlotPool;
    delete pTbxCtrlFac;
    delete pStbCtrlFac;
    delete pMenuCtrlFac;
    delete pFactArr;
    delete pImgListSmall;
    delete pImgListBig;
}

ImageList* SfxModule_Impl::GetImageList( ResMgr* pResMgr, bool bBig )
{
    ImageList*& rpList = bBig ? pImgListBig : pImgListSmall;
    if ( !rpList )
    {
        ResId aResId( bBig ? ( RID_DEFAULTIMAGELIST_LC ) : ( RID_DEFAULTIMAGELIST_SC ), *pResMgr );

        aResId.SetRT( RSC_IMAGELIST );

        DBG_ASSERT( pResMgr->IsAvailable(aResId), "No default ImageList!" );

        if ( pResMgr->IsAvailable(aResId) )
            rpList = new ImageList( aResId );
        else
            rpList = new ImageList();
    }

    return rpList; }

TYPEINIT1(SfxModule, SfxShell);

//=========================================================================

SFX_IMPL_INTERFACE(SfxModule,SfxShell,SfxResId(0))
{
}

//====================================================================

ResMgr* SfxModule::GetResMgr()
{
    return pResMgr;
}

//====================================================================

SfxModule::SfxModule( ResMgr* pMgrP, sal_Bool bDummyP,
                      SfxObjectFactory* pFactoryP, ... )
    : pResMgr( pMgrP ), bDummy( bDummyP ), pImpl(0L)
{
    Construct_Impl();
    va_list pVarArgs;
    va_start( pVarArgs, pFactoryP );
    for ( SfxObjectFactory *pArg = pFactoryP; pArg;
         pArg = va_arg( pVarArgs, SfxObjectFactory* ) )
        pArg->SetModule_Impl( this );
    va_end(pVarArgs);
}

void SfxModule::Construct_Impl()
{
    if( !bDummy )
    {
        SfxApplication *pApp = SFX_APP();
        SfxModuleArr_Impl& rArr = GetModules_Impl();
        SfxModule* pPtr = (SfxModule*)this;
        rArr.push_back( pPtr );
        pImpl = new SfxModule_Impl;
        pImpl->pSlotPool = new SfxSlotPool( &pApp->GetAppSlotPool_Impl(), pResMgr );

        pImpl->pTbxCtrlFac=0;
        pImpl->pStbCtrlFac=0;
        pImpl->pMenuCtrlFac=0;
        pImpl->pFactArr=0;
        pImpl->pImgListSmall=0;
        pImpl->pImgListBig=0;

        SetPool( &pApp->GetPool() );
    }
}

//====================================================================

SfxModule::~SfxModule()
{
    if( !bDummy )
    {
        if ( SFX_APP()->Get_Impl() )
        {
            // The module will be destroyed before the Deinitialize,
            // so remove from the array
            SfxModuleArr_Impl& rArr = GetModules_Impl();
            for( sal_uInt16 nPos = rArr.size(); nPos--; )
            {
                if( rArr[ nPos ] == this )
                {
                    rArr.erase( rArr.begin() + nPos );
                    break;
                }
            }

            delete pImpl;
        }

        delete pResMgr;
    }
}

//-------------------------------------------------------------------------

SfxSlotPool* SfxModule::GetSlotPool() const
{
    return pImpl->pSlotPool;
}

//-------------------------------------------------------------------------

void SfxModule::RegisterChildWindow(SfxChildWinFactory *pFact)
{
    DBG_ASSERT( pImpl, "No real Module!" );

    if (!pImpl->pFactArr)
        pImpl->pFactArr = new SfxChildWinFactArr_Impl;

    for (sal_uInt16 nFactory=0; nFactory<pImpl->pFactArr->size(); ++nFactory)
    {
        if (pFact->nId ==  (*pImpl->pFactArr)[nFactory]->nId)
        {
            pImpl->pFactArr->erase( pImpl->pFactArr->begin() + nFactory );
            SAL_WARN("sfx2.appl", "ChildWindow registered multiple times!");
            return;
        }
    }

    pImpl->pFactArr->push_back( pFact );
}

//-------------------------------------------------------------------------

void SfxModule::RegisterToolBoxControl( SfxTbxCtrlFactory *pFact )
{
    if (!pImpl->pTbxCtrlFac)
        pImpl->pTbxCtrlFac = new SfxTbxCtrlFactArr_Impl;

#ifdef DBG_UTIL
    for ( sal_uInt16 n=0; n<pImpl->pTbxCtrlFac->size(); n++ )
    {
        SfxTbxCtrlFactory *pF = (*pImpl->pTbxCtrlFac)[n];
        if ( pF->nTypeId && pF->nTypeId == pFact->nTypeId &&
            (pF->nSlotId == pFact->nSlotId || pF->nSlotId == 0) )
        {
            DBG_WARNING("TbxController-Registering is not clearly defined!");
        }
    }
#endif

    pImpl->pTbxCtrlFac->push_back( pFact );
}

//-------------------------------------------------------------------------

void SfxModule::RegisterStatusBarControl( SfxStbCtrlFactory *pFact )
{
    if (!pImpl->pStbCtrlFac)
        pImpl->pStbCtrlFac = new SfxStbCtrlFactArr_Impl;

#ifdef DBG_UTIL
    for ( sal_uInt16 n=0; n<pImpl->pStbCtrlFac->size(); n++ )
    {
        SfxStbCtrlFactory *pF = (*pImpl->pStbCtrlFac)[n];
        if ( pF->nTypeId && pF->nTypeId == pFact->nTypeId &&
            (pF->nSlotId == pFact->nSlotId || pF->nSlotId == 0) )
        {
            DBG_WARNING("TbxController-Registering is not clearly defined!");
        }
    }
#endif

    pImpl->pStbCtrlFac->push_back( pFact );
}

//-------------------------------------------------------------------------

void SfxModule::RegisterMenuControl( SfxMenuCtrlFactory *pFact )
{
    if (!pImpl->pMenuCtrlFac)
        pImpl->pMenuCtrlFac = new SfxMenuCtrlFactArr_Impl;

#ifdef DBG_UTIL
    for ( sal_uInt16 n=0; n<pImpl->pMenuCtrlFac->size(); n++ )
    {
        SfxMenuCtrlFactory *pF = (*pImpl->pMenuCtrlFac)[n];
        if ( pF->nTypeId && pF->nTypeId == pFact->nTypeId &&
            (pF->nSlotId == pFact->nSlotId || pF->nSlotId == 0) )
        {
            DBG_WARNING("MenuController-Registering is not clearly defined!");
        }
    }
#endif

    pImpl->pMenuCtrlFac->push_back( pFact );
}

//-------------------------------------------------------------------------

SfxTbxCtrlFactArr_Impl*  SfxModule::GetTbxCtrlFactories_Impl() const
{
    return pImpl->pTbxCtrlFac;
}

//-------------------------------------------------------------------------

SfxStbCtrlFactArr_Impl*  SfxModule::GetStbCtrlFactories_Impl() const
{
    return pImpl->pStbCtrlFac;
}

//-------------------------------------------------------------------------

SfxMenuCtrlFactArr_Impl* SfxModule::GetMenuCtrlFactories_Impl() const
{
    return pImpl->pMenuCtrlFac;
}

//-------------------------------------------------------------------------

SfxChildWinFactArr_Impl* SfxModule::GetChildWinFactories_Impl() const
{
    return pImpl->pFactArr;
}

ImageList* SfxModule::GetImageList_Impl( sal_Bool bBig )
{
    return pImpl->GetImageList( pResMgr, bBig );
}

SfxTabPage*     SfxModule::CreateTabPage( sal_uInt16, Window*, const SfxItemSet& )
{
    return NULL;
}

SfxModuleArr_Impl& SfxModule::GetModules_Impl()
{
    if( !pModules )
        pModules = new SfxModuleArr_Impl;
    return *pModules;
};

void SfxModule::DestroyModules_Impl()
{
    if ( pModules )
    {
        SfxModuleArr_Impl& rModules = *pModules;
        for( sal_uInt16 nPos = rModules.size(); nPos--; )
        {
            SfxModule* pMod = rModules[nPos];
            delete pMod;
        }
        delete pModules, pModules = 0;
    }
}

void SfxModule::Invalidate( sal_uInt16 nId )
{
    for( SfxViewFrame* pFrame = SfxViewFrame::GetFirst(); pFrame; pFrame = SfxViewFrame::GetNext( *pFrame ) )
        if ( pFrame->GetObjectShell()->GetModule() == this )
            Invalidate_Impl( pFrame->GetBindings(), nId );
}

bool SfxModule::IsChildWindowAvailable( const sal_uInt16 i_nId, const SfxViewFrame* i_pViewFrame ) const
{
    if ( i_nId != SID_TASKPANE )
        // by default, assume it is
        return true;

    const SfxViewFrame* pViewFrame = i_pViewFrame ? i_pViewFrame : GetFrame();
    ENSURE_OR_RETURN( pViewFrame, "SfxModule::IsChildWindowAvailable: no frame to ask for the module identifier!", false );
    return ::sfx2::ModuleTaskPane::ModuleHasToolPanels( pViewFrame->GetFrame().GetFrameInterface() );
}

SfxModule* SfxModule::GetActiveModule( SfxViewFrame* pFrame )
{
    if ( !pFrame )
        pFrame = SfxViewFrame::Current();
    SfxObjectShell* pSh = 0;
    if( pFrame )
        pSh = pFrame->GetObjectShell();
    return pSh ? pSh->GetModule() : 0;
}

FieldUnit SfxModule::GetModuleFieldUnit( ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame > const & i_frame )
{
    ENSURE_OR_RETURN( i_frame.is(), "SfxModule::GetModuleFieldUnit: invalid frame!", FUNIT_100TH_MM );

    // find SfxViewFrame for the given XFrame
    SfxViewFrame* pViewFrame = SfxViewFrame::GetFirst();
    while ( pViewFrame != NULL )
    {
        if ( pViewFrame->GetFrame().GetFrameInterface() == i_frame )
            break;
        pViewFrame = SfxViewFrame::GetNext( *pViewFrame );
    }
    ENSURE_OR_RETURN( pViewFrame != NULL, "SfxModule::GetModuleFieldUnit: unable to find an SfxViewFrame for the given XFrame", FUNIT_100TH_MM );

    // find the module
    SfxModule const * pModule = GetActiveModule( pViewFrame );
    ENSURE_OR_RETURN( pModule != NULL, "SfxModule::GetModuleFieldUnit: no SfxModule for the given frame!", FUNIT_100TH_MM );
	if ( pModule )
		return pModule->GetFieldUnit();
	return FUNIT_INCH;
}

FieldUnit SfxModule::GetCurrentFieldUnit()
{
    FieldUnit eUnit = FUNIT_INCH;
    SfxModule* pModule = GetActiveModule();
    if ( pModule )
    {
        const SfxPoolItem* pItem = pModule->GetItem( SID_ATTR_METRIC );
        if ( pItem )
            eUnit = (FieldUnit)( (SfxUInt16Item*)pItem )->GetValue();
    }
    else
        SAL_WARN( "sfx2.appl", "GetModuleFieldUnit(): no module found" );
    return eUnit;
}

FieldUnit SfxModule::GetFieldUnit() const
{
    FieldUnit eUnit = FUNIT_INCH;
    const SfxPoolItem* pItem = GetItem( SID_ATTR_METRIC );
    if ( pItem )
        eUnit = (FieldUnit)( (SfxUInt16Item*)pItem )->GetValue();
    return eUnit;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
