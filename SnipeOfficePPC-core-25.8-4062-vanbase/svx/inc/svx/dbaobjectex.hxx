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

#ifndef SVX_DBAOBJECTEX_HXX
#define SVX_DBAOBJECTEX_HXX

#include <svtools/transfer.hxx>
#include <comphelper/stl_types.hxx>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/sdbc/XConnection.hpp>
#include <com/sun/star/ucb/XContent.hpp>
#include <svx/dataaccessdescriptor.hxx>
#include <com/sun/star/uno/Sequence.hxx>
#include "svx/svxdllapi.h"

//........................................................................
namespace svx
{
//........................................................................

    //====================================================================
    //= OComponentTransferable
    //====================================================================
    class SVX_DLLPUBLIC OComponentTransferable : public TransferableHelper
    {
    protected:
        ODataAccessDescriptor   m_aDescriptor;

    public:
        /** construct the transferable
        */
        OComponentTransferable(
            const ::rtl::OUString&  _rDatasourceOrLocation
            ,const ::com::sun::star::uno::Reference< ::com::sun::star::ucb::XContent>& _xContent
        );

        /** checks whether or not a component descriptor can be extracted from the data flavor vector given
            @param _rFlavors
                available flavors
        */
        static sal_Bool canExtractComponentDescriptor(const DataFlavorExVector& _rFlavors,sal_Bool _bForm );

        /** extracts a component descriptor from the transferable given
        */
        static ODataAccessDescriptor
                        extractComponentDescriptor(const TransferableDataHelper& _rData);

    protected:
        // TransferableHelper overridables
        virtual void        AddSupportedFormats();
        virtual sal_Bool    GetData( const ::com::sun::star::datatransfer::DataFlavor& rFlavor );

        static sal_uInt32   getDescriptorFormatId(sal_Bool _bExtractForm);
    };

//........................................................................
}   // namespace svx
//........................................................................

#endif // SVX_DBAOBJECTEX_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
