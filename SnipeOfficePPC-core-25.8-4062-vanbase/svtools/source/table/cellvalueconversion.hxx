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

#ifndef SVTOOLS_CELLVALUECONVERSION_HXX
#define SVTOOLS_CELLVALUECONVERSION_HXX

#include <com/sun/star/uno/Any.hxx>

#include <boost/scoped_ptr.hpp>

namespace comphelper
{
    class ComponentContext;
}

//......................................................................................................................
namespace svt
{
//......................................................................................................................

    //==================================================================================================================
    //= CellValueConversion
    //==================================================================================================================
    struct CellValueConversion_Data;
    class CellValueConversion
    {
    public:
        CellValueConversion( ::comphelper::ComponentContext const & i_context );
        ~CellValueConversion();

        ::rtl::OUString convertToString( const ::com::sun::star::uno::Any& i_cellValue );

    private:
        ::boost::scoped_ptr< CellValueConversion_Data > m_pData;
    };

//......................................................................................................................
} // namespace svt
//......................................................................................................................

#endif // SVTOOLS_CELLVALUECONVERSION_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
