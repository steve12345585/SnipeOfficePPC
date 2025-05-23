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

#ifndef _XMLOFF_XMLINDEXSPANENTRYCONTEXT_HXX_
#define _XMLOFF_XMLINDEXSPANENTRYCONTEXT_HXX_

#include "XMLIndexSimpleEntryContext.hxx"
#include <com/sun/star/uno/Reference.h>
#include <com/sun/star/uno/Sequence.h>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <rtl/ustrbuf.hxx>

namespace com { namespace sun { namespace star {
    namespace xml { namespace sax { class XAttributeList; } }
} } }
namespace rtl { class OUString; }

/**
 * Import index entry templates
 */
class XMLIndexSpanEntryContext : public XMLIndexSimpleEntryContext
{
    ::rtl::OUStringBuffer sContent;

public:

    TYPEINFO();

    XMLIndexSpanEntryContext(
        SvXMLImport& rImport,
        XMLIndexTemplateContext& rTemplate,
        sal_uInt16 nPrfx,
        const ::rtl::OUString& rLocalName );

    ~XMLIndexSpanEntryContext();

protected:

    /// Collect element contents
    virtual void Characters(const ::rtl::OUString& sString);

    /// add Text PropertyValue
    virtual void FillPropertyValues(
        ::com::sun::star::uno::Sequence<
            ::com::sun::star::beans::PropertyValue> & rValues);
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
