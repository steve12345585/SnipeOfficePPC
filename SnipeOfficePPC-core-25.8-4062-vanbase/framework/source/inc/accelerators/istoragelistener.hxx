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

#ifndef __FRAMEWORK_ACCELERATORS_ISTORAGELISTENER_HXX_
#define __FRAMEWORK_ACCELERATORS_ISTORAGELISTENER_HXX_

#include <general.h>
#include <stdtypes.h>


//__________________________________________
// definition

namespace framework
{

//__________________________________________
/**
    TODO document me
 */
class IStorageListener
{
    public:

        //--------------------------------------
        /** @short  TODO */
        virtual void changesOccurred(const ::rtl::OUString& sPath) = 0;

    protected:
        ~IStorageListener() {}
};

} // namespace framework

#endif // __FRAMEWORK_ACCELERATORS_ISTORAGELISTENER_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
