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

#ifndef __SERIALIZATION_APP_XML_HXX
#define __SERIALIZATION_APP_XML_HXX

#include <com/sun/star/io/XPipe.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>

#include "serialization.hxx"

#include <comphelper/componentcontext.hxx>

class CSerializationAppXML : public CSerialization
{
private:
    CSS::uno::Reference< CSS::lang::XMultiServiceFactory > m_xFactory;
    CSS::uno::Reference< CSS::io::XPipe > m_xBuffer;

    void serialize_node(const CSS::uno::Reference< CSS::xml::dom::XNode >& aNode);
    void serialize_nodeset();

public:
    CSerializationAppXML();

    virtual void serialize();
    virtual CSS::uno::Reference< CSS::io::XInputStream > getInputStream();
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
