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
#include "GroupExchange.hxx"
#include <sot/formats.hxx>
#include <sot/storage.hxx>

namespace rptui
{
    using namespace ::com::sun::star;
    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::beans;

    sal_uInt32 OGroupExchange::getReportGroupId()
    {
        static sal_uInt32 s_nReportFormat = (sal_uInt32)-1;
        if ( (sal_uInt32)-1 == s_nReportFormat )
        {
            s_nReportFormat = SotExchange::RegisterFormatName(rtl::OUString("application/x-openoffice;windows_formatname=\"reportdesign.GroupFormat\"" ));
            OSL_ENSURE((sal_uInt32)-1 != s_nReportFormat, "Bad exchange id!");
        }
        return s_nReportFormat;
    }
    OGroupExchange::OGroupExchange(const uno::Sequence< uno::Any >& _aGroupRow)
        : m_aGroupRow(_aGroupRow)
    {
    }
    // -----------------------------------------------------------------------------
    void OGroupExchange::AddSupportedFormats()
    {
        if ( m_aGroupRow.getLength() )
        {
            AddFormat(OGroupExchange::getReportGroupId());
        }
    }
    // -----------------------------------------------------------------------------
    sal_Bool OGroupExchange::GetData( const ::com::sun::star::datatransfer::DataFlavor& rFlavor )
    {
        sal_uLong nFormat = SotExchange::GetFormat(rFlavor);
        if(nFormat == OGroupExchange::getReportGroupId() )
        {
            return SetAny(uno::makeAny(m_aGroupRow),rFlavor);
        }
        return sal_False;
    }
    // -----------------------------------------------------------------------------
    void OGroupExchange::ObjectReleased()
    {
        m_aGroupRow.realloc(0);
    }
    // -----------------------------------------------------------------------------
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
