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
#ifndef _SWDDETBL_HXX
#define _SWDDETBL_HXX

#include "swtable.hxx"

class SwDDEFieldType;

class SwDDETable : public SwTable
{
    SwDepend aDepend;
public:
    TYPEINFO();

    // Ctor moves all lines/boxes from SwTable to it.
    // After that SwTable is empty and has to be deleted.
    SwDDETable( SwTable& rTable, SwDDEFieldType* pDDEType,
                sal_Bool bUpdate = sal_True );
    ~SwDDETable();

    void ChangeContent();
    sal_Bool  NoDDETable();

    SwDDEFieldType* GetDDEFldType();
    inline const SwDDEFieldType* GetDDEFldType() const;
protected:
    virtual void Modify( const SfxPoolItem*, const SfxPoolItem* );
    virtual void SwClientNotify( const SwModify&, const SfxHint& );
};


// ----------- Inlines -----------------------------

inline const SwDDEFieldType* SwDDETable::GetDDEFldType() const
{
    return ((SwDDETable*)this)->GetDDEFldType();
}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
