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

#ifndef INCLUDED_THEMETABLE_HXX
#define INCLUDED_THEMETABLE_HXX

#include <WriterFilterDllApi.hxx>
#include <resourcemodel/LoggedResources.hxx>
#include <com/sun/star/lang/XComponent.hpp>
#include <map>

namespace writerfilter {
namespace dmapper
{

struct ThemeTable_Impl;

class WRITERFILTER_DLLPRIVATE ThemeTable : public LoggedProperties, public LoggedTable
{
    ThemeTable_Impl *m_pImpl;

public:
    ThemeTable();
    virtual ~ThemeTable();

    const OUString getFontNameForTheme(const Id id) const;

 private:
    // Properties
    virtual void lcl_attribute(Id Name, Value & val);
    virtual void lcl_sprm(Sprm & sprm);

    // Table
    virtual void lcl_entry(int pos, writerfilter::Reference<Properties>::Pointer_t ref);
};
typedef boost::shared_ptr< ThemeTable >          ThemeTablePtr;
}}

#endif //

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
