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
#ifndef _CONNECTIVITY_SDBCX_IREFRESHABLE_HXX_
#define _CONNECTIVITY_SDBCX_IREFRESHABLE_HXX_

#include "connectivity/CommonTools.hxx"
#include "connectivity/dbtoolsdllapi.hxx"

namespace connectivity
{
    namespace sdbcx
    {
        class OOO_DLLPUBLIC_DBTOOLS SAL_NO_VTABLE IRefreshableGroups
        {
        public:
            virtual void refreshGroups() = 0;

        protected:
            ~IRefreshableGroups() {}
        };

        class OOO_DLLPUBLIC_DBTOOLS SAL_NO_VTABLE IRefreshableUsers
        {
        public:
            virtual void refreshUsers() = 0;

        protected:
            ~IRefreshableUsers() {}
        };

        class OOO_DLLPUBLIC_DBTOOLS SAL_NO_VTABLE IRefreshableColumns
        {
        public:
            virtual void refreshColumns()   = 0;

        protected:
            ~IRefreshableColumns() {}
        };
    }
}
#endif //_CONNECTIVITY_SDBCX_IREFRESHABLE_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
