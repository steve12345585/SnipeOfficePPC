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

#ifndef INCLUDED_UNODEVTOOLS_OPTIONS_HXX
#define INCLUDED_UNODEVTOOLS_OPTIONS_HXX

#include <rtl/ustrbuf.hxx>

namespace com { namespace sun { namespace star { namespace uno {
class RuntimeException;
} } } }

namespace unodevtools {

//-------------------------------------------------------------------------------
sal_Bool readOption( rtl::OUString * pValue, const sal_Char * pOpt,
                     sal_uInt32 * pnIndex, const rtl::OUString & aArg)
    throw (com::sun::star::uno::RuntimeException);

//-------------------------------------------------------------------------------
sal_Bool readOption( sal_Bool * pbOpt, const sal_Char * pOpt,
                     sal_uInt32 * pnIndex, const rtl::OUString & aArg);

} // end of namespace unodevtools

#endif // INCLUDED_UNODEVTOOLS_OPTIONS_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
