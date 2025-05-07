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


#include <macros/registration.hxx>

/*=================================================================================================================
    Add new include and new register info to for new services.

    Example:

        #ifndef __YOUR_SERVICE_1_HXX_
        #include <service1.hxx>
        #endif

        #ifndef __YOUR_SERVICE_2_HXX_
        #include <service2.hxx>
        #endif

        COMPONENTGETFACTORY (   IFFACTORIE( Service1 )
                                 else
                                IFFACTORIE( Service2 )
                             )
=================================================================================================================*/
#include <services/urltransformer.hxx>
#include <services/desktop.hxx>
#include <services/tabwindowservice.hxx>
#include <services/frame.hxx>
#include <services/modulemanager.hxx>
#include <jobs/jobexecutor.hxx>
#include <jobs/jobdispatch.hxx>
#include <services/backingcomp.hxx>
#include <services/layoutmanager.hxx>
#include <uifactory/uielementfactorymanager.hxx>
#include <uifactory/popupmenucontrollerfactory.hxx>
#include <uielement/objectmenucontroller.hxx>
#include <uielement/controlmenucontroller.hxx>
#include <uielement/uicommanddescription.hxx>
#include <uiconfiguration/uiconfigurationmanager.hxx>
#include <uiconfiguration/moduleuicfgsupplier.hxx>
#include <uiconfiguration/moduleuiconfigurationmanager.hxx>
#include <uifactory/menubarfactory.hxx>
#include <accelerators/globalacceleratorconfiguration.hxx>
#include <accelerators/moduleacceleratorconfiguration.hxx>
#include <accelerators/documentacceleratorconfiguration.hxx>
#include <uifactory/toolboxfactory.hxx>
#include <uifactory/addonstoolboxfactory.hxx>
#include "uiconfiguration/windowstateconfiguration.hxx"
#include "uifactory/toolbarcontrollerfactory.hxx"
#include "uifactory/statusbarcontrollerfactory.hxx"
#include <services/autorecovery.hxx>
#include <helper/statusindicatorfactory.hxx>
#include <uielement/recentfilesmenucontroller.hxx>
#include <uifactory/statusbarfactory.hxx>
#include <uiconfiguration/uicategorydescription.hxx>
#include <services/sessionlistener.hxx>
#include <services/taskcreatorsrv.hxx>

#include <uielement/langselectionstatusbarcontroller.hxx>
#include <uiconfiguration/imagemanager.hxx>
#include <uifactory/windowcontentfactorymanager.hxx>
#include <services/substitutepathvars.hxx>
#include <services/pathsettings.hxx>

COMPONENTGETFACTORY ( fwk,
                        IFFACTORY( ::framework::URLTransformer                          )   else
                        IFFACTORY( ::framework::Desktop                                 )   else
                        IFFACTORY( ::framework::Frame                                   )   else
                        IFFACTORY( ::framework::JobExecutor                             )   else
                        IFFACTORY( ::framework::JobDispatch                             )   else
                        IFFACTORY( ::framework::BackingComp                             )   else
                        IFFACTORY( ::framework::LayoutManager                           )   else
                        IFFACTORY( ::framework::UIElementFactoryManager                 )   else
                        IFFACTORY( ::framework::PopupMenuControllerFactory              )   else
                        IFFACTORY( ::framework::ObjectMenuController                    )   else
                        IFFACTORY( ::framework::ControlMenuController                   )   else
                        IFFACTORY( ::framework::UICommandDescription                    )   else
                        IFFACTORY( ::framework::ModuleManager                           )   else
                        IFFACTORY( ::framework::UIConfigurationManager                  )   else
                        IFFACTORY( ::framework::ModuleUIConfigurationManagerSupplier    )   else
                        IFFACTORY( ::framework::ModuleUIConfigurationManager            )   else
                        IFFACTORY( ::framework::MenuBarFactory                          )   else
                        IFFACTORY( ::framework::GlobalAcceleratorConfiguration          )   else
                        IFFACTORY( ::framework::ModuleAcceleratorConfiguration          )   else
                        IFFACTORY( ::framework::DocumentAcceleratorConfiguration        )   else
                        IFFACTORY( ::framework::ToolBoxFactory                          )   else
                        IFFACTORY( ::framework::AddonsToolBoxFactory                    )   else
                        IFFACTORY( ::framework::WindowStateConfiguration                )   else
                        IFFACTORY( ::framework::ToolbarControllerFactory                )   else
                        IFFACTORY( ::framework::AutoRecovery                            )   else
                        IFFACTORY( ::framework::StatusIndicatorFactory                  )   else
                        IFFACTORY( ::framework::RecentFilesMenuController               )   else
                        IFFACTORY( ::framework::StatusBarFactory                        )   else
                        IFFACTORY( ::framework::UICategoryDescription                   )   else
                        IFFACTORY( ::framework::SessionListener                         )   else
                        IFFACTORY( ::framework::StatusbarControllerFactory              )   else
                        IFFACTORY( ::framework::SessionListener                         )   else
                        IFFACTORY( ::framework::TaskCreatorService                      )   else
                        IFFACTORY( ::framework::ImageManager                            )   else
                        IFFACTORY( ::framework::LangSelectionStatusbarController        )   else
                        IFFACTORY( ::framework::WindowContentFactoryManager             )   else
                        IFFACTORY( ::framework::TabWindowService                        )   else
                        IFFACTORY( ::framework::SubstitutePathVariables                 )   else
                        IFFACTORY( ::framework::PathSettings                            )
            )

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
