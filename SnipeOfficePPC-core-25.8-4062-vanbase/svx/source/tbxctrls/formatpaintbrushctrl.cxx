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


#include "svx/formatpaintbrushctrl.hxx"

// header for class SfxBoolItem
#include <svl/eitem.hxx>

// header for define SFX_APP
#include <sfx2/app.hxx>

// header for class ToolBox
#include <vcl/toolbox.hxx>

//.............................................................................
namespace svx
{
//.............................................................................

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;

SFX_IMPL_TOOLBOX_CONTROL( FormatPaintBrushToolBoxControl, SfxBoolItem );

FormatPaintBrushToolBoxControl::FormatPaintBrushToolBoxControl( sal_uInt16 nSlotId, sal_uInt16 nId, ToolBox& rTbx )
    : SfxToolBoxControl( nSlotId, nId, rTbx )
    , m_bPersistentCopy(false)
    , m_aDoubleClickTimer()
{
    sal_uIntPtr nDblClkTime = rTbx.GetSettings().GetMouseSettings().GetDoubleClickTime();

    m_aDoubleClickTimer.SetTimeoutHdl( LINK(this, FormatPaintBrushToolBoxControl, WaitDoubleClickHdl) );
    m_aDoubleClickTimer.SetTimeout(nDblClkTime);
}

// -----------------------------------------------------------------------

FormatPaintBrushToolBoxControl::~FormatPaintBrushToolBoxControl()
{
    m_aDoubleClickTimer.Stop();
}

// -----------------------------------------------------------------------
void FormatPaintBrushToolBoxControl::impl_executePaintBrush()
{
    Sequence< PropertyValue > aArgs( 1 );
    aArgs[0].Name  = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "PersistentCopy" ));
    aArgs[0].Value = makeAny( static_cast<sal_Bool>(m_bPersistentCopy) );
    Dispatch( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ".uno:FormatPaintbrush" ))
        , aArgs );
}

// -----------------------------------------------------------------------
void FormatPaintBrushToolBoxControl::DoubleClick()
{
    m_aDoubleClickTimer.Stop();

    m_bPersistentCopy = true;
    this->impl_executePaintBrush();
}

// -----------------------------------------------------------------------
void FormatPaintBrushToolBoxControl::Click()
{
    m_bPersistentCopy = false;
    m_aDoubleClickTimer.Start();
}

// -----------------------------------------------------------------------
IMPL_LINK_NOARG(FormatPaintBrushToolBoxControl, WaitDoubleClickHdl)
{
    //there was no second click during waiting
    this->impl_executePaintBrush();
    return 0;
}

// -----------------------------------------------------------------------
void FormatPaintBrushToolBoxControl::Select( sal_Bool )
{
}

// -----------------------------------------------------------------------
void FormatPaintBrushToolBoxControl::StateChanged( sal_uInt16 nSID, SfxItemState eState,
                const SfxPoolItem* pState )
{
    if( ( eState & SFX_ITEM_SET ) == 0 )
        m_bPersistentCopy = false;
    SfxToolBoxControl::StateChanged( nSID, eState, pState );
}

//.............................................................................
} //namespace svx
//.............................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
