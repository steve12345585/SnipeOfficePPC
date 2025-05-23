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


#include <testshl/simpleheader.hxx>
#include <odiapi/core/Node.hxx>
#include <odiapi/props/Properties.hxx>

using namespace odiapi::core;
using namespace odiapi::props;
using namespace writerfilter;
using namespace std;

class TestCore : public CppUnit::TestFixture
{
public:
    void testCreateCore()
    {
        PropertyPool::Pointer_t pool = createPropertyPool();
        PropertyBag_Pointer_t pb = createPropertyBag();

        pb->insert(createIntegerProperty(NS_fo::LN_font_weight, 12));
        PropertyPoolHandle_Pointer_t ph = pool->insert(pb);

        Node::Pointer_t node = createNode(NS_style::LN_char, ph, "NS_style::LN_char");

        CPPUNIT_ASSERT_MESSAGE("Create node failed wrong node id", node->getId() == NS_style::LN_char);
        CPPUNIT_ASSERT_MESSAGE("Create node failed wrong text", node->getText() == "NS_style::LN_char");
        CPPUNIT_ASSERT_MESSAGE("Create node failed wrong pool handle", node->getProperties() == ph);
    }

    void testInsertSibling()
    {
        PropertyPool::Pointer_t pool = createPropertyPool();
        PropertyBag_Pointer_t pb = createPropertyBag();

        pb->insert(createIntegerProperty(NS_fo::LN_font_weight, 12));
        PropertyPoolHandle_Pointer_t ph = pool->insert(pb);

        Node::Pointer_t node1 = createNode(NS_style::LN_char, ph, "Text");
        Node::Pointer_t node2 = createNode(NS_style::LN_char, ph, "\\par");

        node1->insertSibling(node2);

        string postfixSeq = node1->getText();
        const Node* n = node1.get();
        while (n->hasNext())
        {
            n = &n->getNext();
            postfixSeq = postfixSeq + n->getText();
        }

        CPPUNIT_ASSERT_MESSAGE("Insert sibling failed", postfixSeq == "Text\\par");

        Node::Pointer_t node3 = createNode(NS_style::LN_char, ph, "\\span");

        node1->insertSibling(node3);

        postfixSeq = node1->getText();
        n = node1.get();
        while (n->hasNext())
        {
            n = &n->getNext();
            postfixSeq = postfixSeq + n->getText();
        }

        CPPUNIT_ASSERT_MESSAGE("Insert sibling failed", postfixSeq == "Text\\span\\par");
    }

    void testAppendChildren()
    {
        PropertyPool::Pointer_t pool = createPropertyPool();
        PropertyBag_Pointer_t pb = createPropertyBag();

        pb->insert(createIntegerProperty(NS_fo::LN_font_weight, 12));
        PropertyPoolHandle_Pointer_t ph = pool->insert(pb);

        Node::Pointer_t node1 = createNode(NS_style::LN_char, ph, "Text");
        const Node* pn1 = node1.get();

        Node::Pointer_t node2 = createNode(NS_style::LN_char, ph, "\\par");

        node2->appendChildren(node1);

        CPPUNIT_ASSERT_MESSAGE("Append children failed", &node2->getFirstChild() == pn1);

        const Node* n = &node2->getFirstChild();
        string postfixSeq = n->getText() + n->getNext().getText();

        CPPUNIT_ASSERT_MESSAGE("Append children failed", postfixSeq == "Text\\par");
    }

    void testCore()
    {
        PropertyPool::Pointer_t pool = createPropertyPool();
        PropertyBag_Pointer_t pb = createPropertyBag();

        pb->insert(createIntegerProperty(NS_fo::LN_font_weight, 12));
        PropertyPoolHandle_Pointer_t ph = pool->insert(pb);

        Node::Pointer_t node1 = createNode(NS_style::LN_char, ph, "A");
        Node::Pointer_t node2 = createNode(NS_style::LN_char, ph, "\\span");

        node2->appendChildren(node1);

        Node::Pointer_t node3 = createNode(NS_style::LN_char, ph, "B");
        Node::Pointer_t node4 = createNode(NS_style::LN_char, ph, "\\span");

        node4->appendChildren(node3);

        node2->insertSibling(node4);

        Node::Pointer_t node5 = createNode(NS_style::LN_char, ph, "\\par");

        node5->appendChildren(node2);

        Node::Pointer_t node6 = createNode(NS_style::LN_char, ph, "C");
        Node::Pointer_t node7 = createNode(NS_style::LN_char, ph, "\\span");

        node7->appendChildren(node6);

        node5->appendChildren(node7);


        string postfixSeq = node5->getText();
        const Node* n = node5.get();
        while (n->hasPrevious())
        {
            n = &n->getPrevious();
            postfixSeq = postfixSeq + n->getText();
        }

        CPPUNIT_ASSERT_MESSAGE("Insert sibling failed", postfixSeq == "\\par\\spanC\\spanB\\spanA");
    }

    CPPUNIT_TEST_SUITE(TestCore);
    CPPUNIT_TEST(testCreateCore);
    CPPUNIT_TEST(testInsertSibling);
    CPPUNIT_TEST(testAppendChildren);
    CPPUNIT_TEST(testCore);
    CPPUNIT_TEST_SUITE_END();
};

//#####################################
// register test suites
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestCore, "TestCore");

//NOADDITIONAL;

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
