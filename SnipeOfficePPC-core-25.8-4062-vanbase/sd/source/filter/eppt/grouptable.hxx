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

#ifndef EPPT_GROUP_TABLE
#define EPPT_GROUP_TABLE

#include <com/sun/star/container/XIndexAccess.hpp>

struct GroupEntry
{
    sal_uInt32                  mnCurrentPos;
    sal_uInt32                  mnCount;
    ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess >           mXIndexAccess;

    GroupEntry( ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess > & rIndex )
    {
        mXIndexAccess = rIndex;
        mnCount =mXIndexAccess->getCount();
        mnCurrentPos = 0;
    };

    GroupEntry( sal_uInt32 nCount )
    {
        mnCount = nCount;
        mnCurrentPos = 0;
    };

    ~GroupEntry(){};
};

class GroupTable
{
    protected:

        sal_uInt32              mnIndex;
        sal_uInt32              mnCurrentGroupEntry;
        sal_uInt32              mnMaxGroupEntry;
        sal_uInt32              mnGroupsClosed;
        GroupEntry**            mpGroupEntry;

        void                    ImplResizeGroupTable( sal_uInt32 nEntrys );

    public:

        sal_uInt32              GetCurrentGroupIndex() const { return mnIndex; };
        sal_Int32               GetCurrentGroupLevel() const { return mnCurrentGroupEntry - 1; };
        ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess > &
                                GetCurrentGroupAccess() const { return mpGroupEntry[  mnCurrentGroupEntry - 1 ]->mXIndexAccess; };
        sal_uInt32              GetGroupsClosed();
        void                    ResetGroupTable( sal_uInt32 nCount );
        void                    ClearGroupTable();
        sal_Bool                EnterGroup( ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess > & rIndex );
        sal_Bool                GetNextGroupEntry();
                                GroupTable();
                                ~GroupTable();
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
