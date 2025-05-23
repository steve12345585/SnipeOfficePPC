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

#ifndef __FRAMEWORK_HELPER_IMAGEPRODUCER_HXX_
#define __FRAMEWORK_HELPER_IMAGEPRODUCER_HXX_

#include <sal/types.h>
#include <com/sun/star/frame/XFrame.hpp>
#include <vcl/image.hxx>
#include <rtl/ustring.hxx>
#include <framework/fwedllapi.h>

namespace framework
{

typedef Image ( *pfunc_getImage)(
    const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& rFrame,
    const ::rtl::OUString& aURL,
    bool bBig
);

pfunc_getImage FWE_DLLPUBLIC SAL_CALL SetImageProducer( pfunc_getImage pGetImageFunc );

Image FWE_DLLPUBLIC SAL_CALL GetImageFromURL(
    const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& rFrame,
    const ::rtl::OUString& aURL,
    bool bBig
);

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
