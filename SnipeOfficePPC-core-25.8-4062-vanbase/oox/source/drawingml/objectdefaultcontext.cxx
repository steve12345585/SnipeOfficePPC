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

#include "oox/drawingml/objectdefaultcontext.hxx"
#include "oox/drawingml/spdefcontext.hxx"
#include "oox/drawingml/theme.hxx"

using rtl::OUString;
using namespace ::oox::core;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::xml::sax;

namespace oox { namespace drawingml {

objectDefaultContext::objectDefaultContext( ContextHandler& rParent, Theme& rTheme )
: ContextHandler( rParent )
, mrTheme( rTheme )
{
}

Reference< XFastContextHandler > objectDefaultContext::createFastChildContext( sal_Int32 aElementToken, const Reference< XFastAttributeList >& /* xAttribs */ ) throw (SAXException, RuntimeException)
{
    switch( aElementToken )
    {
        case A_TOKEN( spDef ):
            return new spDefContext( *this, mrTheme.getSpDef() );
        case A_TOKEN( lnDef ):
            return new spDefContext( *this, mrTheme.getLnDef() );
        case A_TOKEN( txDef ):
            return new spDefContext( *this, mrTheme.getTxDef() );
    }
    return 0;
}

} }

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
