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
#ifndef _SFX_APPDATA_HXX
#define _SFX_APPDATA_HXX

#include <svl/lstner.hxx>
#include <vcl/timer.hxx>
#include <tools/string.hxx>
#include <svtools/ehdl.hxx>
#include "rtl/ref.hxx"

#include <com/sun/star/frame/XModel.hpp>

#include "bitset.hxx"
#include <vector>

class SfxApplication;
class SfxProgress;
class SfxChildWinFactArr_Impl;
class SfxDdeDocTopics_Impl;
class DdeService;
class SfxEventConfiguration;
class SfxMacroConfig;
class SfxItemPool;
class SfxFilterMatcher;
class ISfxTemplateCommon;
class SfxFilterMatcher;
class SfxStatusDispatcher;
class SfxDdeTriggerTopic_Impl;
class SfxDocumentTemplates;
class SfxFrameArr_Impl;
class SvtSaveOptions;
class SvtHelpOptions;
class SfxObjectFactory;
class SfxObjectShell;
class ResMgr;
class Window;
class SfxTbxCtrlFactArr_Impl;
class SfxStbCtrlFactArr_Impl;
class SfxMenuCtrlFactArr_Impl;
class SfxViewFrameArr_Impl;
class SfxViewShellArr_Impl;
class SfxObjectShellArr_Impl;
class ResMgr;
class SimpleResMgr;
class SfxViewFrame;
class SfxSlotPool;
class SfxResourceManager;
class SfxDispatcher;
class SfxInterface;
class BasicManager;
class SfxBasicManagerHolder;
class SfxBasicManagerCreationListener;

namespace sfx2 { namespace appl { class ImeStatusWindow; } }

typedef Link* LinkPtr;

class SfxAppData_Impl
{
public:
    IndexBitSet                         aIndexBitSet;           // for counting noname documents
    String                              aLastDir;               // for IO dialog

    // DDE stuff
    DdeService*                         pDdeService;
    SfxDdeDocTopics_Impl*               pDocTopics;
    SfxDdeTriggerTopic_Impl*            pTriggerTopic;
    DdeService*                         pDdeService2;

    // single instance classes
    SfxChildWinFactArr_Impl*            pFactArr;
    SfxFrameArr_Impl*                   pTopFrames;

    // application members
    SfxFilterMatcher*                   pMatcher;
    ResMgr*                             pBasicResMgr;
    ResMgr*                             pSvtResMgr;
#ifdef DBG_UTIL
    SimpleErrorHandler *m_pSimpleErrorHdl;
#endif
    SfxErrorHandler *m_pToolsErrorHdl;
    SfxErrorHandler *m_pSoErrorHdl;
    SfxErrorHandler *m_pSbxErrorHdl;
    SfxStatusDispatcher*                pAppDispatch;
    SfxDocumentTemplates*               pTemplates;

    // global pointers
    SfxItemPool*                        pPool;
    std::vector<sal_uInt16>*            pDisabledSlotList;
    SvtSaveOptions*                     pSaveOptions;
    SvtHelpOptions*                     pHelpOptions;

    // "current" functionality
    SfxProgress*                        pProgress;
    ISfxTemplateCommon*                 pTemplateCommon;

    sal_uInt16                              nDocModalMode;              // counts documents in modal mode
    sal_uInt16                              nAutoTabPageId;
    sal_uInt16                              nRescheduleLocks;
    sal_uInt16                              nInReschedule;

    rtl::Reference< sfx2::appl::ImeStatusWindow > m_xImeStatusWindow;

    SfxTbxCtrlFactArr_Impl*     pTbxCtrlFac;
    SfxStbCtrlFactArr_Impl*     pStbCtrlFac;
    SfxMenuCtrlFactArr_Impl*    pMenuCtrlFac;
    SfxViewFrameArr_Impl*       pViewFrames;
    SfxViewShellArr_Impl*       pViewShells;
    SfxObjectShellArr_Impl*     pObjShells;
    ResMgr*                     pSfxResManager;
    ResMgr*                     pOfaResMgr;
    SfxBasicManagerHolder*      pBasicManager;
    SfxBasicManagerCreationListener*
                                pBasMgrListener;
    SfxViewFrame*               pViewFrame;
    SfxSlotPool*                pSlotPool;
    SfxResourceManager*         pResMgr;
    SfxDispatcher*              pAppDispat;     // Dispatcher if no document
    SfxInterface**              pInterfaces;

    sal_uInt16                      nDocNo;             // current Doc-Number (AutoName)
    sal_uInt16                      nInterfaces;

    sal_Bool                        bDispatcherLocked:1;    // do nothing
    sal_Bool                        bDowning:1;   // sal_True on Exit and afterwards
    sal_Bool                        bInQuit : 1;
    sal_Bool                        bInvalidateOnUnlock : 1;
    sal_Bool                        bODFVersionWarningLater : 1;

                                SfxAppData_Impl( SfxApplication* );
                                ~SfxAppData_Impl();

    SfxDocumentTemplates*       GetDocumentTemplates();
    void                        DeInitDDE();

    /** called when the Application's BasicManager has been created. This can happen
        explicitly in SfxApplication::GetBasicManager, or implicitly if a document's
        BasicManager is created before the application's BasicManager exists.
    */
    void                        OnApplicationBasicManagerCreated( BasicManager& _rManager );
};

#endif // #ifndef _SFX_APPDATA_HXX


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
