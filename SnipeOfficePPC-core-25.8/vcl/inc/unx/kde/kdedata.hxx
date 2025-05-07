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

#ifndef _VCL_KDEDATA_HXX
#define _VCL_KDEDATA_HXX

#include <unx/saldisp.hxx>
#include <unx/saldata.hxx>
#include <unx/salframe.h>

class KDEData : public X11SalData
{
public:
    KDEData( SalInstance *pInstance ) : X11SalData( SAL_DATA_KDE3, pInstance ) {}
    virtual ~KDEData();

    virtual void Init();
    virtual void initNWF();
    virtual void deInitNWF();
};

class SalKDEDisplay : public SalX11Display
{
public:
    SalKDEDisplay( Display* pDisp );
    virtual ~SalKDEDisplay();
};

class KDESalFrame : public X11SalFrame
{
    static const int nMaxGraphics = 2;

    struct GraphicsHolder
    {
        X11SalGraphics*     pGraphics;
        bool                bInUse;
        GraphicsHolder()
                : pGraphics( NULL ),
                  bInUse( false )
        {}
        ~GraphicsHolder();
    };
    GraphicsHolder m_aGraphics[ nMaxGraphics ];

public:
    KDESalFrame( SalFrame* pParent, sal_uLong );
    virtual ~KDESalFrame();

    virtual SalGraphics* GetGraphics();
    virtual void ReleaseGraphics( SalGraphics *pGraphics );
    virtual void updateGraphics( bool bClear );
    virtual void UpdateSettings( AllSettings& rSettings );
    virtual void Show( sal_Bool bVisible, sal_Bool bNoActivate );
};

class KDESalInstance : public X11SalInstance
{
public:
    KDESalInstance( SalYieldMutex* pMutex )
            : X11SalInstance( pMutex ) {}
    virtual ~KDESalInstance() {}
    virtual SalFrame* CreateFrame( SalFrame* pParent, sal_uLong nStyle );

    virtual bool hasNativeFileSelection() const { return true; }

    virtual com::sun::star::uno::Reference< com::sun::star::ui::dialogs::XFilePicker2 >
        createFilePicker( const com::sun::star::uno::Reference<
                              com::sun::star::uno::XComponentContext >& );
};

class KDEXLib : public SalXLib
{
    bool            m_bStartupDone;
    void*           m_pApplication;
    char**          m_pFreeCmdLineArgs;
    char**          m_pAppCmdLineArgs;
    int             m_nFakeCmdLineArgs;
public:
    KDEXLib() : SalXLib(),
        m_bStartupDone( false ),
        m_pApplication( NULL ),
        m_pFreeCmdLineArgs( NULL ),
        m_pAppCmdLineArgs( NULL ),
        m_nFakeCmdLineArgs( 0 )
        {}
    virtual ~KDEXLib();
    virtual void Init();

    void doStartup();
};

#endif // _VCL_KDEDATA_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
