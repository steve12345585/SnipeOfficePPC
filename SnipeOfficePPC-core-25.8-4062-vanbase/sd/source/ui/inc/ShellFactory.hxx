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

#ifndef SD_VIEW_SHELL_FACTORY_HXX
#define SD_VIEW_SHELL_FACTORY_HXX

#include <sal/types.h>
#include <memory>

class Window;

namespace sd {

class FrameView;

typedef sal_Int32 ShellId;
// This shell id is a reserved value and does not specify any valid shell.
static const sal_Int32 snInvalidShellId = -1;

template<class ShellType>
class ShellFactory
{
public:
    /** This abstract virtual class needs a destructor so that the
        destructors of derived classes are called.
    */
    virtual ~ShellFactory (void) {};

    /** Create a new instance of a view shell for the given id that will
        be stacked onto the given view shell base.
        @return
            Return the new view shell or NULL when a creation is not
            possible.
        */
    virtual ShellType* CreateShell (
        ShellId nId,
        ::Window* pParentWindow,
        FrameView* pFrameView = NULL) = 0;

    /** Tell the factory that a shell is no longer in use.  It may destroy
        it or put it for future use in a cache.
    */
    virtual void ReleaseShell (ShellType* pShell) = 0;
};

} // end of namespace sd

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
