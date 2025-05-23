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

#ifndef OOX_XLS_CONDFORMATCONTEXT_HXX
#define OOX_XLS_CONDFORMATCONTEXT_HXX

#include "condformatbuffer.hxx"
#include "excelhandlers.hxx"

namespace oox {
namespace xls {

// ============================================================================
//
class CondFormatContext;

class ColorScaleContext : public WorksheetContextBase
{
public:
    explicit ColorScaleContext( CondFormatContext& rFragment, CondFormatRuleRef xRule );

    virtual ::oox::core::ContextHandlerRef onCreateContext( sal_Int32 nElement, const AttributeList& rAttribs );
    virtual void        onStartElement( const AttributeList& rAttribs );

private:
    CondFormatRuleRef mxRule;
};

class DataBarContext : public WorksheetContextBase
{
public:
    explicit DataBarContext( CondFormatContext& rFormat, CondFormatRuleRef xRule );

    virtual ::oox::core::ContextHandlerRef onCreateContext( sal_Int32 nElement, const AttributeList& rAttribs );
    virtual void        onStartElement( const AttributeList& rAttribs );

private:
    CondFormatRuleRef mxRule;
};

class IconSetContext : public WorksheetContextBase
{
public:
    explicit IconSetContext( CondFormatContext& rFormat, CondFormatRuleRef xRule );

    virtual oox::core::ContextHandlerRef onCreateContext( sal_Int32 nElement, const AttributeList& rAttribs );
    virtual void onStartElement( const AttributeList& rAttribs );
private:
    CondFormatRuleRef mxRule;
};

class CondFormatContext : public WorksheetContextBase
{
public:
    explicit            CondFormatContext( WorksheetFragmentBase& rFragment );

protected:
    virtual ::oox::core::ContextHandlerRef onCreateContext( sal_Int32 nElement, const AttributeList& rAttribs );
    virtual void        onStartElement( const AttributeList& rAttribs );
    virtual void        onCharacters( const ::rtl::OUString& rChars );
    virtual void        onEndElement();

    virtual ::oox::core::ContextHandlerRef onCreateRecordContext( sal_Int32 nRecId, SequenceInputStream& rStrm );
    virtual void        onStartRecord( SequenceInputStream& rStrm );
    virtual void        onEndRecord();

private:
    CondFormatRef       mxCondFmt;
    CondFormatRuleRef   mxRule;
};

// ============================================================================

} // namespace xls
} // namespace oox

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
