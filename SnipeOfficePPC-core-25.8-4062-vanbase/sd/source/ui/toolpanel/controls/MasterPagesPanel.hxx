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

#ifndef SD_TASKPANE_CONTROLS_MASTER_PAGES_PANEL_HXX
#define SD_TASKPANE_CONTROLS_MASTER_PAGES_PANEL_HXX

#include "taskpane/ScrollPanel.hxx"

namespace sd {
class ViewShellBase;
}

namespace sd { namespace toolpanel {
class ControlFactory;
class TreeNode;
class ToolPanelViewShell;
} }

namespace sd { namespace toolpanel { namespace controls {

/** The master pages panel combines three master page related panels into
    one.  This has the benefit that creation of the task pane becomes a
    little bit simpler and that common scroll bars can be displayed.
*/
class MasterPagesPanel
    : public ScrollPanel
{
public:
    MasterPagesPanel (
        ::Window& i_rParentWindow,
        ToolPanelViewShell& i_rPanelViewShell);
    virtual ~MasterPagesPanel (void);

    // TreeNode overridables
    virtual TaskPaneShellManager* GetShellManager (void);

    static std::auto_ptr<ControlFactory> CreateControlFactory (ToolPanelViewShell& i_rToolPanelShell);

private:
    void    impl_construct( ViewShellBase& rBase );

private:
    ToolPanelViewShell* m_pPanelViewShell;
};

} } } // end of namespace ::sd::toolpanel::controls

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
