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

#ifndef COMPHELPER_DOCUMENTINFO_HXX
#define COMPHELPER_DOCUMENTINFO_HXX

#include "comphelper/comphelperdllapi.h"

#include <com/sun/star/frame/XModel.hpp>

//........................................................................
namespace comphelper {
//........................................................................

    //====================================================================
    //= DocumentInfo
    //====================================================================
    class COMPHELPER_DLLPUBLIC DocumentInfo
    {
    public:
        /** retrieves the UI title of the given document
        */
        static ::rtl::OUString  getDocumentTitle( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel >& _rxDocument );

    private:
        DocumentInfo(); // never implemented
    };

//........................................................................
}   // namespace comphelper
//........................................................................

#endif // COMPHELPER_DOCUMENTINFO_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
