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

#include "dlg_InsertLegend.hxx"
#include "dlg_InsertLegend.hrc"
#include "res_LegendPosition.hxx"
#include "ObjectNameProvider.hxx"
#include "ResId.hxx"
#include "chartview/ChartSfxItemIds.hxx"
#include "NoWarningThisInCTOR.hxx"

// header for enum SvxChartLegendPos
#include <svx/chrtitem.hxx>
// header for class SfxItemPool
#include <svl/itempool.hxx>

//.............................................................................
namespace chart
{
//.............................................................................

using namespace ::com::sun::star;

SchLegendDlg::SchLegendDlg(Window* pWindow, const uno::Reference< uno::XComponentContext>& xCC )
    : ModalDialog(pWindow, SchResId(DLG_LEGEND))
    , m_apLegendPositionResources( new LegendPositionResources(this,xCC) )
    , aBtnOK(this, SchResId(BTN_OK))
    , aBtnCancel(this, SchResId(BTN_CANCEL))
    , aBtnHelp(this, SchResId(BTN_HELP))
{
    FreeResource();
    this->SetText( ObjectNameProvider::getName(OBJECTTYPE_LEGEND) );
}

SchLegendDlg::~SchLegendDlg()
{
}

void SchLegendDlg::init( const uno::Reference< frame::XModel >& xChartModel )
{
    m_apLegendPositionResources->writeToResources( xChartModel );
}

bool SchLegendDlg::writeToModel( const uno::Reference< frame::XModel >& xChartModel ) const
{
    m_apLegendPositionResources->writeToModel( xChartModel );
    return true;
}

//.............................................................................
} //namespace chart
//.............................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
