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

#ifndef __FORMULA_H__
#define __FORMULA_H__

// DVO: remove DEBUG dependency
// #ifndef DEBUG
#include <com/sun/star/xml/sax/XDocumentHandler.hpp>
#include "attributes.hxx"
// DVO: remove DEBUG dependency
// #endif

class Node;

// DVO: remove DEBUG dependency
// #ifndef DEBUG
using namespace ::com::sun::star::xml::sax;
// #endif

class Formula
{
public:
    Formula(char *_eq, int _ishwpeq = 1)
        : pList(NULL)
    {
        eq = _eq;
        isHwpEQ = _ishwpeq;
        trim();
    }
    virtual ~Formula(){ }

    void setDocumentHandler(Reference < XDocumentHandler > xHandler )
    {
          m_rxDocumentHandler = xHandler;
    }
    void setAttributeListImpl( AttributeListImpl *p )
    {
        pList = p;
        rList = (XAttributeList *) pList;
    }
    int parse();
private:
     void trim();
     void makeMathML(Node *res);
     void makeLines(Node *res);
     void makeLine(Node *res);
     void makeExprList(Node *res);
     void makeExpr(Node *res);
     void makePrimary(Node *res);
     void makeIdentifier(Node *res);
     void makeSubSup(Node *res);
     void makeFraction(Node *res);
     void makeDecoration(Node *res);
     void makeFunction(Node *res);
     void makeRoot(Node *res);
     void makeArrow(Node *res);
     void makeAccent(Node *res);
     void makeParenth(Node *res);
     void makeFence(Node *res);
     void makeBracket(Node *res);
     void makeBlock(Node *res);
     void makeBegin(Node *res);
     void makeEnd(Node *res);

private:
     Reference< XDocumentHandler >  m_rxDocumentHandler;
     Reference< XAttributeList > rList;
     AttributeListImpl *pList;
     char *eq;
     int isHwpEQ;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
