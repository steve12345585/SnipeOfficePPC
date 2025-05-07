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



#include <unotools/moduleoptions.hxx>
#include "sddll.hxx"
#include "diactrl.hxx"
#include "tbx_ww.hxx"
#include "TextObjectBar.hxx"
#include "BezierObjectBar.hxx"
#include "GraphicObjectBar.hxx"
#include "MediaObjectBar.hxx"
#include "ImpressViewShellBase.hxx"
#include "PresentationViewShellBase.hxx"
#include "OutlineViewShell.hxx"
#include "PresentationViewShell.hxx"
#include "OutlineViewShellBase.hxx"
#include "SlideSorterViewShellBase.hxx"
#include "DrawViewShell.hxx"
#include "GraphicViewShell.hxx"
#include "GraphicViewShellBase.hxx"
#include "DrawDocShell.hxx"
#include "GraphicDocShell.hxx"
#include "SlideSorterViewShell.hxx"
#include "taskpane/ToolPanelViewShell.hxx"
#include "FactoryIds.hxx"
#include "sdmod.hxx"
#include "app.hrc"

namespace sd { namespace ui { namespace table {
    extern void RegisterInterfaces( SfxModule* pMod );
} } }


/*************************************************************************
|*
|* Register all Factorys
|*
\************************************************************************/


void SdDLL::RegisterFactorys()
{
    if (SvtModuleOptions().IsImpress())
    {
        ::sd::ImpressViewShellBase::RegisterFactory (
            ::sd::IMPRESS_FACTORY_ID);
        ::sd::SlideSorterViewShellBase::RegisterFactory (
            ::sd::SLIDE_SORTER_FACTORY_ID);
        ::sd::OutlineViewShellBase::RegisterFactory (
            ::sd::OUTLINE_FACTORY_ID);
        ::sd::PresentationViewShellBase::RegisterFactory (
            ::sd::PRESENTATION_FACTORY_ID);
    }
    if (SvtModuleOptions().IsDraw())
    {
        ::sd::GraphicViewShellBase::RegisterFactory (::sd::DRAW_FACTORY_ID);
    }
}



/*************************************************************************
|*
|* Register all Interfaces
|*
\************************************************************************/

void SdDLL::RegisterInterfaces()
{
    // Module
    SfxModule* pMod = SD_MOD();
    SdModule::RegisterInterface(pMod);

    // View shell base.
    ::sd::ViewShellBase::RegisterInterface(pMod);

    // DocShells
    ::sd::DrawDocShell::RegisterInterface(pMod);
    ::sd::GraphicDocShell::RegisterInterface(pMod);

    // Impress ViewShells
    ::sd::DrawViewShell::RegisterInterface(pMod);
    ::sd::OutlineViewShell::RegisterInterface(pMod);
    ::sd::PresentationViewShell::RegisterInterface(pMod);

    // Draw ViewShell
    ::sd::GraphicViewShell::RegisterInterface(pMod);

    // Impress ObjectShells
    ::sd::BezierObjectBar::RegisterInterface(pMod);
    ::sd::TextObjectBar::RegisterInterface(pMod);
    ::sd::GraphicObjectBar::RegisterInterface(pMod);

    // Media ObjectShell
    ::sd::MediaObjectBar::RegisterInterface(pMod);

    // Table ObjectShell
    ::sd::ui::table::RegisterInterfaces(pMod);

    // View shells for the side panes.
    ::sd::slidesorter::SlideSorterViewShell::RegisterInterface (pMod);
    ::sd::toolpanel::ToolPanelViewShell::RegisterInterface(pMod);
    // Tell the tool panel view shell to register the interfaces of its
    // controls.
    ::sd::toolpanel::ToolPanelViewShell::RegisterControls();
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
