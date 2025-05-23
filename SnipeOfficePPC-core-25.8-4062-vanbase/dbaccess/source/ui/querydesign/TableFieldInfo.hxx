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
#ifndef DBAUI_TABLEFIELDINFO_HXX
#define DBAUI_TABLEFIELDINFO_HXX

#include "QEnumTypes.hxx"
#include <sal/types.h>

namespace dbaui
{
    class OTableFieldInfo
    {
    private:
        ETableFieldType m_eFieldType;
        sal_Int32       m_eDataType;

    public:
        OTableFieldInfo();
        ~OTableFieldInfo();

        inline ETableFieldType  GetKeyType() const { return m_eFieldType; }
        inline void             SetKey(ETableFieldType bKey=TAB_NORMAL_FIELD) { m_eFieldType = bKey; }
        inline sal_Int32        GetDataType() const { return m_eDataType; }
        inline void             SetDataType(sal_Int32 eTyp) { m_eDataType = eTyp; }
    };
}
#endif // DBAUI_TABLEFIELDINFO_HXX


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
