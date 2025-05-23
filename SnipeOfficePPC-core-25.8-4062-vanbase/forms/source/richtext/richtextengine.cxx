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

#include "richtextengine.hxx"
#include <svl/itempool.hxx>
#include <editeng/eeitem.hxx>
#include <editeng/editobj.hxx>
#define ITEMID_FONTHEIGHT   EE_CHAR_FONTHEIGHT
#include <editeng/fhgtitem.hxx>
#define ITEMID_FONT         EE_CHAR_FONTHEIGHT
#include <editeng/fontitem.hxx>
#define ITEMID_LANGUAGE     EE_CHAR_LANGUAGE
#include <editeng/langitem.hxx>
#include <vcl/svapp.hxx>
#include <tools/mapunit.hxx>
#include <vcl/mapmod.hxx>
#include <vcl/outdev.hxx>
#include <unotools/lingucfg.hxx>
#include <svl/undo.hxx>
#include <osl/mutex.hxx>

#include <algorithm>
#include <functional>

//........................................................................
namespace frm
{
//........................................................................

    //====================================================================
    //= RichTextEngine
    //====================================================================
    //--------------------------------------------------------------------
    RichTextEngine* RichTextEngine::Create()
    {
        SfxItemPool* pPool = EditEngine::CreatePool();
        pPool->FreezeIdRanges();

        RichTextEngine* pReturn = new RichTextEngine( pPool );
        OutputDevice* pOutputDevice = pReturn->GetRefDevice();
        MapMode aDeviceMapMode( pOutputDevice->GetMapMode() );

        pReturn->SetStatusEventHdl( LINK( pReturn, RichTextEngine, EditEngineStatusChanged ) );

        pPool->SetDefaultMetric(  (SfxMapUnit)( aDeviceMapMode.GetMapUnit() ) );

        // defaults
        Font aFont = Application::GetSettings().GetStyleSettings().GetAppFont();
        aFont.SetName( String( RTL_CONSTASCII_USTRINGPARAM( "Times New Roman" ) ) );
        pPool->SetPoolDefaultItem( SvxFontItem( aFont.GetFamily(), aFont.GetName(), String(), aFont.GetPitch(), aFont.GetCharSet(), EE_CHAR_FONTINFO ) );

        // 12 pt font size
        MapMode aPointMapMode( MAP_POINT );
        Size a12PointSize( OutputDevice::LogicToLogic( Size( 12, 0 ), aPointMapMode, aDeviceMapMode ) );
        pPool->SetPoolDefaultItem( SvxFontHeightItem( a12PointSize.Width(), 100, EE_CHAR_FONTHEIGHT ) );

        // font languages
        SvtLinguOptions aLinguOpt;
        pPool->SetPoolDefaultItem( SvxLanguageItem( aLinguOpt.nDefaultLanguage, EE_CHAR_LANGUAGE ) );
        pPool->SetPoolDefaultItem( SvxLanguageItem( aLinguOpt.nDefaultLanguage_CJK, EE_CHAR_LANGUAGE_CJK ) );
        pPool->SetPoolDefaultItem( SvxLanguageItem( aLinguOpt.nDefaultLanguage_CTL, EE_CHAR_LANGUAGE_CTL ) );

        return pReturn;
    }

    //--------------------------------------------------------------------
    RichTextEngine* RichTextEngine::Clone()
    {
        RichTextEngine* pClone( NULL );
        {
            SolarMutexGuard aGuard;
            EditTextObject* pMyText = CreateTextObject();
            OSL_ENSURE( pMyText, "RichTextEngine::Clone: CreateTextObject returned nonsense!" );

            pClone = Create();

            if ( pMyText )
                pClone->SetText( *pMyText );
            delete pMyText;
        }

        return pClone;
    }

    DBG_NAME(RichTextEngine)
    //--------------------------------------------------------------------
    RichTextEngine::RichTextEngine( SfxItemPool* _pPool )
        :EditEngine( _pPool )
        ,m_pEnginePool( _pPool )
    {
        DBG_CTOR(RichTextEngine,NULL);
    }

    //--------------------------------------------------------------------
    RichTextEngine::~RichTextEngine( )
    {
        DBG_DTOR(RichTextEngine,NULL);
    }

    //--------------------------------------------------------------------
    void RichTextEngine::registerEngineStatusListener( IEngineStatusListener* _pListener )
    {
        OSL_ENSURE( _pListener, "RichTextEngine::registerEngineStatusListener: invalid listener!" );
        if ( _pListener )
            m_aStatusListeners.push_back( _pListener );
    }

    //--------------------------------------------------------------------
    void RichTextEngine::revokeEngineStatusListener( IEngineStatusListener* _pListener )
    {
        ::std::vector< IEngineStatusListener* >::iterator aPos = ::std::find_if(
            m_aStatusListeners.begin(),
            m_aStatusListeners.end(),
            ::std::bind2nd( ::std::equal_to< IEngineStatusListener* >( ), _pListener )
        );
        OSL_ENSURE( aPos != m_aStatusListeners.end(), "RichTextEngine::revokeEngineStatusListener: listener not registered!" );
        if ( aPos != m_aStatusListeners.end() )
            m_aStatusListeners.erase( aPos );
    }

    //--------------------------------------------------------------------
    IMPL_LINK( RichTextEngine, EditEngineStatusChanged, EditStatus*, _pStatus )
    {
        for ( ::std::vector< IEngineStatusListener* >::const_iterator aLoop = m_aStatusListeners.begin();
              aLoop != m_aStatusListeners.end();
              ++aLoop
            )
            (*aLoop)->EditEngineStatusChanged( *_pStatus );
        return 0L;
    }

//........................................................................
}   // namespace frm
//........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
