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

#ifndef LIST_HXX_INCLUDED
#define LIST_HXX_INCLUDED

#include <tools/string.hxx>

class SwNumRule;
class SwNodes;
class SwNodeNum;

class SwListImpl;

class SwList
{
    public:
        SwList( const String sListId,
                SwNumRule& rDefaultListStyle,
                const SwNodes& rNodes );
        ~SwList();

        const String GetListId() const;

        const String GetDefaultListStyleName() const;

        void InsertListItem( SwNodeNum& rNodeNum,
                             const int nLevel );
        void RemoveListItem( SwNodeNum& rNodeNum );

        void InvalidateListTree();
        void ValidateListTree();

        void MarkListLevel( const int nListLevel,
                            const bool bValue );

        bool IsListLevelMarked( const int nListLevel ) const;

    private:
        // no copy constructor and no assignment operator
        SwList( const SwList& );
        SwList& operator=( const SwList& );

        SwListImpl* mpListImpl;
};
#endif // LIST_HXX_INCLUDED

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
