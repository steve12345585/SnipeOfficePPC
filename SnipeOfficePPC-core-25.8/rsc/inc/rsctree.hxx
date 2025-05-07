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
#ifndef _RSCTREE_HXX
#define _RSCTREE_HXX

#include <tools/link.hxx>
#include <rsctools.hxx>

/****************** C L A S S E S ****************************************/
class BiNode
{
protected:
    BiNode*     pLeft;    // left subtree
    BiNode*     pRight;   // right subtree

public:

                         // Wandelt eine doppelt verkettete Liste in
                         // einen binaeren Baum um
            BiNode *    ChangeDLListBTree( BiNode * pList );

                        BiNode();
    virtual             ~BiNode();


                        // Wandelt einen binaeren Baum in eine doppelt
                        // verkettete Liste um
                        BiNode* ChangeBTreeDLList();

            BiNode *    Left() const { return pLeft  ; };
            BiNode *    Right() const{ return pRight ; };
            void        EnumNodes( Link aLink ) const;
};

/*************************************************************************/
class NameNode : public BiNode
{
    void                SubOrderTree( NameNode * pOrderNode );

protected:
                        // pCmp ist Zeiger auf Namen
            NameNode*   Search( const void * pCmp ) const;

public:
            NameNode*   Left() const { return (NameNode *)pLeft  ; };
            NameNode*   Right() const{ return (NameNode *)pRight ; };
            NameNode*   Search( const NameNode * pName ) const;
                        // insert a new node in the b-tree
            sal_Bool        Insert( NameNode * pTN, sal_uInt32 * nDepth );
            sal_Bool        Insert( NameNode* pTN );
    virtual COMPARE     Compare( const NameNode * ) const;
    virtual COMPARE     Compare( const void * ) const;
            NameNode*   SearchParent( const NameNode * ) const;
                        // return ist neue Root
            NameNode*   Remove( NameNode * );
            void        OrderTree();
};

/*************************************************************************/
class IdNode : public NameNode
{
    virtual COMPARE Compare( const NameNode * ) const;
    virtual COMPARE Compare( const void * ) const;
protected:
    using NameNode::Search;

public:

    IdNode*         Search( sal_uInt32 nTypName ) const;
    virtual sal_uInt32  GetId() const;
};

/*************************************************************************/
class StringNode : public NameNode
{
    virtual COMPARE Compare( const NameNode * ) const;
    virtual COMPARE Compare( const void * ) const;

protected:
    using NameNode::Search;

    rtl::OString m_aName;

public:
    StringNode() {}
    StringNode(const rtl::OString& rStr) { m_aName = rStr; }

    StringNode*     Search( const char * ) const;
    rtl::OString    GetName() const { return m_aName; }
};

#endif // _RSCTREE_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
