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

#include "svx/xmlexchg.hxx"
#include <sot/formats.hxx>
#include <sot/exchange.hxx>
#include <tools/debug.hxx>

//........................................................................
namespace svx
{
//........................................................................

    using namespace ::com::sun::star::datatransfer;

    //====================================================================
    //= OXFormsTransferable
    //====================================================================
    //--------------------------------------------------------------------
    OXFormsTransferable::OXFormsTransferable( const OXFormsDescriptor &rhs ) :
        m_aDescriptor(rhs)
    {
    }
    //--------------------------------------------------------------------
    void OXFormsTransferable::AddSupportedFormats()
    {
        AddFormat( SOT_FORMATSTR_ID_XFORMS );
    }
    //--------------------------------------------------------------------
    sal_Bool OXFormsTransferable::GetData( const DataFlavor& _rFlavor )
    {
        const sal_uInt32 nFormatId = SotExchange::GetFormat( _rFlavor );
        if ( SOT_FORMATSTR_ID_XFORMS == nFormatId )
        {
            return SetString(rtl::OUString("XForms-Transferable"), _rFlavor);
        }
        return sal_False;
    }
    //--------------------------------------------------------------------
    const OXFormsDescriptor &OXFormsTransferable::extractDescriptor( const TransferableDataHelper &_rData ) {

        using namespace ::com::sun::star::uno;
        using namespace ::com::sun::star::datatransfer;
        Reference<XTransferable> &transfer = const_cast<Reference<XTransferable> &>(_rData.GetTransferable());
        XTransferable *pInterface = transfer.get();
        OXFormsTransferable *pThis = dynamic_cast<OXFormsTransferable *>(pInterface);
        DBG_ASSERT(pThis,"XTransferable is NOT an OXFormsTransferable???");
        return pThis->m_aDescriptor;
    }


//........................................................................
}   // namespace svx
//........................................................................


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
