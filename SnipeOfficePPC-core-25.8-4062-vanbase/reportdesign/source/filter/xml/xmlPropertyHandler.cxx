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

#include "xmlPropertyHandler.hxx"
#include "xmlfilter.hxx"
#include <xmloff/xmltoken.hxx>
#include <xmloff/txtimp.hxx>
#include <xmloff/nmspmap.hxx>

//--------------------------------------------------------------------------
namespace rptxml
{
//--------------------------------------------------------------------------
    using namespace ::com::sun::star;
    using namespace xmloff;
    using namespace ::com::sun::star::report;
    using namespace ::com::sun::star::uno;
DBG_NAME( rpt_OXMLRptPropHdlFactory )

OXMLRptPropHdlFactory::OXMLRptPropHdlFactory()
{
    DBG_CTOR( rpt_OXMLRptPropHdlFactory,NULL);
}
// -----------------------------------------------------------------------------
OXMLRptPropHdlFactory::~OXMLRptPropHdlFactory()
{
    DBG_DTOR( rpt_OXMLRptPropHdlFactory,NULL);
}
// -----------------------------------------------------------------------------
}// rptxml
// -----------------------------------------------------------------------------

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
