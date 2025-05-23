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

#include "oox/drawingml/table/tablestylelistfragmenthandler.hxx"
#include "oox/drawingml/table/tablestylecontext.hxx"

using ::rtl::OUString;
using namespace ::oox::core;

using rtl::OUString;
using namespace ::com::sun::star;
using namespace ::oox::core;
using namespace ::oox::drawingml;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::xml::sax;

namespace oox {
namespace drawingml {
namespace table {

// ============================================================================

TableStyleListFragmentHandler::TableStyleListFragmentHandler( XmlFilterBase& rFilter, const OUString& rFragmentPath, TableStyleList& rTableStyleList ):
FragmentHandler2( rFilter, rFragmentPath ),
mrTableStyleList( rTableStyleList )
{
}

TableStyleListFragmentHandler::~TableStyleListFragmentHandler()
{
}

// CT_TableStyleList
Reference< XFastContextHandler > TableStyleListFragmentHandler::createFastChildContext(
    sal_Int32 aElementToken, const Reference< XFastAttributeList >& xAttribs )
        throw ( SAXException, RuntimeException )
{
    Reference< XFastContextHandler > xRet;
    switch( aElementToken )
    {
        case A_TOKEN( tblStyleLst ):    // CT_TableStyleList
            mrTableStyleList.getDefaultStyleId() = xAttribs->getOptionalValue( XML_def );
            break;
        case A_TOKEN( tblStyle ):       // CT_TableStyle
            std::vector< TableStyle >& rTableStyles = mrTableStyleList.getTableStyles();
            rTableStyles.resize( rTableStyles.size() + 1 );
            xRet = new TableStyleContext( *this, xAttribs, rTableStyles.back() );
            break;
    }
    if ( !xRet.is() )
        xRet = getFastContextHandler();
    return xRet;
}

// ============================================================================

} // namespace table
} // namespace drawingml
} // namespace oox

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
