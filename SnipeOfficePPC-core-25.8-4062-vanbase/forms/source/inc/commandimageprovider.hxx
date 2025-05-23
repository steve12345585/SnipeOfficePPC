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

#ifndef COMMANDIMAGEPROVIDER_HXX
#define COMMANDIMAGEPROVIDER_HXX

#include <com/sun/star/frame/XModel.hpp>

#include <comphelper/componentcontext.hxx>
#include <vcl/image.hxx>

#include <boost/shared_ptr.hpp>

//........................................................................
namespace frm
{
//........................................................................

    //=====================================================================
    //= ICommandImageProvider
    //=====================================================================
    typedef ::rtl::OUString                                 CommandURL;
    typedef ::com::sun::star::uno::Sequence< CommandURL >   CommandURLs;
    typedef ::std::vector< Image >                          CommandImages;

    class SAL_NO_VTABLE ICommandImageProvider
    {
    public:
        virtual CommandImages   getCommandImages(
                                    const CommandURLs& _rCommandURLs,
                                    const bool _bLarge
                                ) const = 0;

        virtual ~ICommandImageProvider() { }
    };

    typedef ::boost::shared_ptr< const ICommandImageProvider >  PCommandImageProvider;

    //=====================================================================
    //= factory
    //=====================================================================
    PCommandImageProvider
        createDocumentCommandImageProvider(
            const ::comphelper::ComponentContext& _rContext,
            const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel >& _rxDocument
        );

//........................................................................
} // namespace frm
//........................................................................

#endif // COMMANDIMAGEPROVIDER_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
