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


#include <com/sun/star/beans/Property.hpp>
#include <com/sun/star/beans/PropertyAttribute.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/ucb/CommandInfo.hpp>
#include <com/sun/star/ucb/OpenCommandArgument2.hpp>
#include <com/sun/star/ucb/InsertCommandArgument.hpp>
#include <com/sun/star/util/DateTime.hpp>
#include <com/sun/star/uno/Sequence.hxx>

#include "ftpcontent.hxx"

using namespace com::sun::star;
using namespace ftp;

// virtual
uno::Sequence< beans::Property > FTPContent::getProperties(
    const uno::Reference< ucb::XCommandEnvironment > & /*xEnv*/)
{
    #define PROPS_COUNT 8

    static const beans::Property aPropsInfoTable[] =
    {
        beans::Property(
            rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ContentType" ) ),
            -1,
            getCppuType( static_cast< const rtl::OUString * >( 0 ) ),
            beans::PropertyAttribute::BOUND
            | beans::PropertyAttribute::READONLY
        ),
        beans::Property(
            rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "IsDocument" ) ),
            -1,
            getCppuBooleanType(),
            beans::PropertyAttribute::BOUND
            | beans::PropertyAttribute::READONLY
        ),
        beans::Property(
            rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "IsFolder" ) ),
            -1,
            getCppuBooleanType(),
            beans::PropertyAttribute::BOUND
            | beans::PropertyAttribute::READONLY
        ),
        beans::Property(
            rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Title" ) ),
            -1,
            getCppuType( static_cast< const rtl::OUString * >( 0 ) ),
            beans::PropertyAttribute::BOUND
            //  | beans::PropertyAttribute::READONLY
        ),
        beans::Property(
            rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Size" ) ),
            -1,
            getCppuType( static_cast< const sal_Int64 * >( 0 ) ),
            beans::PropertyAttribute::BOUND
            | beans::PropertyAttribute::READONLY
        ),
        beans::Property(
            rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "DateCreated" ) ),
            -1,
            getCppuType( static_cast< util::DateTime * >( 0 ) ),
            beans::PropertyAttribute::BOUND
            | beans::PropertyAttribute::READONLY
        ),
        beans::Property(
            rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "IsReadOnly" ) ),
            -1,
            getCppuBooleanType(),
            beans::PropertyAttribute::BOUND
            | beans::PropertyAttribute::READONLY
        ),
        beans::Property(
            rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(
                "CreatableContentsInfo" ) ),
            -1,
            getCppuType(
                static_cast< const uno::Sequence< ucb::ContentInfo > * >( 0 ) ),
            beans::PropertyAttribute::BOUND
            | beans::PropertyAttribute::READONLY
        )
    };

    return uno::Sequence< beans::Property >( aPropsInfoTable, PROPS_COUNT );
}

//=========================================================================
// virtual
uno::Sequence< ucb::CommandInfo > FTPContent::getCommands(
    const uno::Reference< ucb::XCommandEnvironment > & /*xEnv*/ )
{
// osl::MutexGuard aGuard( m_aMutex );

    //=================================================================
    //
    // Supported commands
    //
    //=================================================================

    #define COMMAND_COUNT 8

    static const ucb::CommandInfo aCommandInfoTable[] =
    {
        ///////////////////////////////////////////////////////////////
        // Required commands
        ///////////////////////////////////////////////////////////////
        ucb::CommandInfo(
            rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "getCommandInfo" ) ),
            -1,
            getCppuVoidType()
        ),
        ucb::CommandInfo(
            rtl::OUString(
                RTL_CONSTASCII_USTRINGPARAM( "getPropertySetInfo" ) ),
            -1,
            getCppuVoidType()
        ),
        ucb::CommandInfo(
            rtl::OUString(
                RTL_CONSTASCII_USTRINGPARAM( "getPropertyValues" ) ),
            -1,
            getCppuType(
                static_cast< uno::Sequence< beans::Property > * >( 0 ) )
        ),
        ucb::CommandInfo(
            rtl::OUString(
                RTL_CONSTASCII_USTRINGPARAM( "setPropertyValues" ) ),
            -1,
            getCppuType(
                static_cast< uno::Sequence< beans::PropertyValue > * >( 0 ) )
        ),
        ucb::CommandInfo(
            rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "open" ) ),
            -1,
            getCppuType(
                static_cast< ucb::OpenCommandArgument2 * >( 0 ) )
        ),
        ucb::CommandInfo(
            rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "insert" ) ),
            -1,
            getCppuType(
                static_cast< ucb::InsertCommandArgument * >( 0 ) )
        ),
        ucb::CommandInfo(
            rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "delete" ) ),
            -1,
            getCppuBooleanType()
        ),
        ucb::CommandInfo(
            rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "createNewContent" ) ),
            -1,
            getCppuType( static_cast< ucb::ContentInfo * >( 0 ) )
        )
    };

    return uno::Sequence< ucb::CommandInfo >( aCommandInfoTable, COMMAND_COUNT );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
