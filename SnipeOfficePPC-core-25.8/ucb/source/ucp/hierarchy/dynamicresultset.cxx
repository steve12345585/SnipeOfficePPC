/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/


/**************************************************************************
                                TODO
 **************************************************************************

 - This implementation is not a dynamic result set!!! It only implements
   the necessary interfaces, but never recognizes/notifies changes!!!

 *************************************************************************/
#include "hierarchydatasupplier.hxx"
#include "dynamicresultset.hxx"

using namespace com::sun::star;
using namespace hierarchy_ucp;

//=========================================================================
//=========================================================================
//
// DynamicResultSet Implementation.
//
//=========================================================================
//=========================================================================

DynamicResultSet::DynamicResultSet(
            const uno::Reference< lang::XMultiServiceFactory >& rxSMgr,
            const rtl::Reference< HierarchyContent >& rxContent,
            const ucb::OpenCommandArgument2& rCommand )
: ResultSetImplHelper( rxSMgr, rCommand ),
  m_xContent( rxContent )
{
}

//=========================================================================
//
// Non-interface methods.
//
//=========================================================================

void DynamicResultSet::initStatic()
{
    m_xResultSet1
        = new ::ucbhelper::ResultSet(
            m_xSMgr,
            m_aCommand.Properties,
            new  HierarchyResultSetDataSupplier( m_xSMgr,
                                                 m_xContent,
                                                 m_aCommand.Mode ) );
}

//=========================================================================
void DynamicResultSet::initDynamic()
{
    m_xResultSet1
        = new ::ucbhelper::ResultSet(
            m_xSMgr,
            m_aCommand.Properties,
            new  HierarchyResultSetDataSupplier( m_xSMgr,
                                                 m_xContent,
                                                 m_aCommand.Mode ) );
    m_xResultSet2 = m_xResultSet1;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
