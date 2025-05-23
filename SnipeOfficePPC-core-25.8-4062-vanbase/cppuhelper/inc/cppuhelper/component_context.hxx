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
#ifndef _CPPUHELPER_COMPONENT_CONTEXT_HXX_
#define _CPPUHELPER_COMPONENT_CONTEXT_HXX_

#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/lang/XSingleComponentFactory.hpp>
#include "cppuhelperdllapi.h"


namespace cppu
{

/** Context entries init struct calling createComponentContext().
*/
struct ContextEntry_Init
{
    /** late init denotes a object that will be raised when first get() is calling for it

        The context implementation expects either a ::com::sun::star::lang::XSingleComponentFactory
        object as value (to instanciate the object) or a string as value for raising
        a service via the used service manager.
    */
    bool bLateInitService;
    /** name of context value
    */
    ::rtl::OUString name;
    /** context value
    */
    ::com::sun::star::uno::Any value;

    /** Default ctor.
    */
    inline ContextEntry_Init() SAL_THROW(())
        : bLateInitService( false )
        {}
    /** Ctor.

        @param name_
               name of entry
        @param value_
               value of entry
        @param bLateInitService_
               whether this entry is a late-init named object entry
               (value is object factory or service string)
    */
    inline ContextEntry_Init(
        ::rtl::OUString const & name_,
        ::com::sun::star::uno::Any const & value_,
        bool bLateInitService_ = false ) SAL_THROW(())
            : bLateInitService( bLateInitService_ ),
              name( name_ ),
              value( value_ )
        {}
};

/** Creates a component context with the given entries.

    @param pEntries array of entries
    @param nEntries number of entries
    @param xDelegate delegation to further context, if value was not found
    @return new context object
*/
CPPUHELPER_DLLPUBLIC ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >
SAL_CALL createComponentContext(
    ContextEntry_Init const * pEntries, sal_Int32 nEntries,
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > const & xDelegate =
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >() )
    SAL_THROW(());

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
