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
#ifndef INCLUDED_OOXML_PROPERTY_SET_HXX
#define INCLUDED_OOXML_PROPERTY_SET_HXX

#include <resourcemodel/WW8ResourceModel.hxx>

namespace writerfilter {
namespace ooxml
{

class OOXMLProperty : public Sprm
{
public:
    typedef boost::shared_ptr<OOXMLProperty> Pointer_t;

    virtual ~OOXMLProperty();

    virtual sal_uInt32 getId() const = 0;
    virtual Value::Pointer_t getValue() = 0;
    virtual writerfilter::Reference<BinaryObj>::Pointer_t getBinary() = 0;
    virtual writerfilter::Reference<Stream>::Pointer_t getStream() = 0;
    virtual writerfilter::Reference<Properties>::Pointer_t getProps() = 0;
    virtual string getName() const = 0;
    virtual string toString() const = 0;
    virtual void resolve(Properties & rProperties) = 0;

    virtual Sprm * clone() = 0;
};

class OOXMLPropertySet : public writerfilter::Reference<Properties>
{
public:
    typedef boost::shared_ptr<OOXMLPropertySet> Pointer_t;

    virtual ~OOXMLPropertySet();

    virtual void resolve(Properties & rHandler) = 0;
    virtual string getType() const = 0;
    virtual void add(OOXMLProperty::Pointer_t pProperty) = 0;
    virtual void add(OOXMLPropertySet::Pointer_t pPropertySet) = 0;
    virtual OOXMLPropertySet * clone() const = 0;
    virtual void setType(const string & rsType) = 0;

    virtual string toString() = 0;
};

class OOXMLTable : public writerfilter::Reference<Table>
{
public:
    virtual ~OOXMLTable();

    virtual void resolve(Table & rTable) = 0;
    virtual OOXMLTable * clone() const = 0;
};
}}

#endif // INCLUDED_OOXML_PROPERTY_SET_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
