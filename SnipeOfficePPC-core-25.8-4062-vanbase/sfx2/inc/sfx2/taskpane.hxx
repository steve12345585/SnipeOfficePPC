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

#ifndef SFX_TASKPANE_HXX
#define SFX_TASKPANE_HXX

#include "sfx2/dllapi.h"
#include "sfx2/childwin.hxx"
#include "sfx2/titledockwin.hxx"

#include <svtools/toolpanel/tabalignment.hxx>
#include <svtools/toolpanel/tabitemcontent.hxx>

#include <boost/scoped_ptr.hpp>
#include <boost/optional.hpp>

namespace svt
{
    class ToolPanelDeck;
}

//......................................................................................................................
namespace sfx2
{
//......................................................................................................................

    //==================================================================================================================
    //= ITaskPaneToolPanelAccess
    //==================================================================================================================
    class SAL_NO_VTABLE ITaskPaneToolPanelAccess
    {
    public:
        virtual void ActivateToolPanel( const ::rtl::OUString& i_rPanelURL ) = 0;

    protected:
        ~ITaskPaneToolPanelAccess() {}
    };

    //==================================================================================================================
    //= TaskPaneWrapper
    //==================================================================================================================
    class SFX2_DLLPUBLIC TaskPaneWrapper    :public SfxChildWindow
                                            ,public ITaskPaneToolPanelAccess
    {
    public:
        TaskPaneWrapper(
            Window* i_pParent,
            sal_uInt16 i_nId,
            SfxBindings* i_pBindings,
            SfxChildWinInfo* i_pInfo
        );

        SFX_DECL_CHILDWINDOW( TaskPaneWrapper );

        // ITaskPaneToolPanelAccess
        virtual void ActivateToolPanel( const ::rtl::OUString& i_rPanelURL );
    };

    //==================================================================================================================
    //= IToolPanelCompare
    //==================================================================================================================
    class SFX2_DLLPUBLIC SAL_NO_VTABLE IToolPanelCompare
    {
    public:
        /** compares to tool panel URLs
            @return
                <ul>
                <li>-1 if the tool panel described by i_rLHS should precede the one described by i_rRHS</li>
                <li>0 if the two panels have no particular relative order</li>
                <li>1 if the tool panel described by i_rLHS should succeed the one described by i_rRHS</li>

        */
        virtual short compareToolPanelsURLs(
                        const ::rtl::OUString& i_rLHS,
                        const ::rtl::OUString& i_rRHS
                    ) const = 0;

    protected:
        ~IToolPanelCompare() {}
    };

    //==================================================================================================================
    //= ModuleTaskPane
    //==================================================================================================================
    class ModuleTaskPane_Impl;
    /** SFX-less version of a module dependent task pane, filled with tool panels as specified in the respective
        module's configuration
    */
    class SFX2_DLLPUBLIC ModuleTaskPane : public Window
    {
    public:
        /** creates a new instance
            @param i_rParentWindow
                the parent window
            @param i_rDocumentFrame
                the frame to which the task pane belongs. Will be passed to any custom tool panels created
                via an XUIElementFactory. Also, it is used to determine the module which the task pane is
                responsible for, thus controlling which tool panels are actually available.
        */
        ModuleTaskPane(
            Window& i_rParentWindow,
            const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& i_rDocumentFrame
        );
        /** creates a new instance
            @param i_rParentWindow
                the parent window
            @param i_rDocumentFrame
                the frame to which the task pane belongs. Will be passed to any custom tool panels created
                via an XUIElementFactory. Also, it is used to determine the module which the task pane is
                responsible for, thus controlling which tool panels are actually available.
            @param i_rCompare
                a comparator for tool panel URLs, which allows controlling the order in which the panels are
                added to the tool panel deck.
        */
        ModuleTaskPane(
            Window& i_rParentWindow,
            const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& i_rDocumentFrame,
            const IToolPanelCompare& i_rCompare
        );
        ~ModuleTaskPane();

        /** determines whether a given module has any registered tool panels
        */
        static bool ModuleHasToolPanels( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& i_rDocumentFrame );

        /** provides access to the Window aspect of the PanelDeck

            Be careful with this method. For instance, you're not allowed to insert arbitrary IToolPanel implementations
            into the deck, as the ModuleTaskPane has certain assumptions about the panel implementations. However,
            you're allowed to remove and re-insert panels, which have originally been created by the ModuleTaskPane
            itself.
        */
              ::svt::ToolPanelDeck& GetPanelDeck();
        const ::svt::ToolPanelDeck& GetPanelDeck() const;

        /** returns the position of the panel with the given resource URL
        */
        ::boost::optional< size_t >
                    GetPanelPos( const ::rtl::OUString& i_rResourceURL );

        /** returns the resource URL of the panel at the specified position
        */
        ::rtl::OUString
                    GetPanelResourceURL( const size_t i_nPanelPos ) const;

        /// sets the "classical" layout of the tool panel deck, using drawers
        void    SetDrawersLayout();
        /// sets the new layout of the tool panel deck, using tabs
        void    SetTabsLayout( const ::svt::TabAlignment i_eTabAlignment, const ::svt::TabItemContent i_eTabContent );

    protected:
        // Window overridables
        virtual void Resize();
        virtual void GetFocus();

    private:
        ::boost::scoped_ptr< ModuleTaskPane_Impl >  m_pImpl;
    };

    //==================================================================================================================
    //= TaskPaneController
    //==================================================================================================================
    class TaskPaneController_Impl;
    /** is a helper class for connecting a ModuleTaskPane and a TitledDockingWindow, for clients of the ModuleTaskPane
        which do not use the TaskPaneDockingWindow

        The controller will add a drop down menu to the docking window which contains one item for each panel in the
        panel deck, and allows toggling their visibility.
    */
    class SFX2_DLLPUBLIC TaskPaneController
    {
    public:
        TaskPaneController(
            ModuleTaskPane& i_rTaskPane,
            TitledDockingWindow& i_rDockingWindow
        );
        ~TaskPaneController();

        /// activates the panel with the given URL
        void    ActivateToolPanel( const ::rtl::OUString& i_rPanelURL );

    private:
        ::boost::scoped_ptr< TaskPaneController_Impl >  m_pImpl;
    };

    //==================================================================================================================
    //= TaskPaneDockingWindow
    //==================================================================================================================
    class TaskPaneDockingWindow :public TitledDockingWindow
                                ,public ITaskPaneToolPanelAccess
    {
    public:
        TaskPaneDockingWindow( SfxBindings* i_pBindings, TaskPaneWrapper& i_rWrapper,
            Window* i_pParent, WinBits i_nBits );

        // ITaskPaneToolPanelAccess
        virtual void    ActivateToolPanel( const ::rtl::OUString& i_rPanelURL );

    protected:
        // Window overridables
        virtual void        GetFocus();

        // TitledDockingWindow overridables
        virtual void onLayoutDone();

    private:
        ModuleTaskPane      m_aTaskPane;
        TaskPaneController  m_aPaneController;
    };

//......................................................................................................................
} // namespace sfx2
//......................................................................................................................

#endif // SFX_TASKPANE_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
