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

#ifndef SVX_FMSCRIPTINGENV_HXX
#define SVX_FMSCRIPTINGENV_HXX

#include <com/sun/star/script/XEventAttacherManager.hpp>
#include <rtl/ref.hxx>

class FmFormModel;
//........................................................................
namespace svxform
{
//........................................................................

    //====================================================================
    //= IFormScriptingEnvironment
    //====================================================================
    /** describes the interface implemented by a component which handles scripting requirements
        in a form/control environment.
    */
    class SAL_NO_VTABLE IFormScriptingEnvironment : public ::rtl::IReference
    {
    public:
        /** registers an XEventAttacherManager whose events should be monitored and handled

            @param _rxManager
                the XEventAttacherManager to monitor. Must not be <NULL/>.

            @throws ::com::sun::star::lang::IllegalArgumentException
                if <arg>_rxManager</arg> is <NULL/>
            @throws ::com::sun::star::lang::DisposedException
                if the instance is already disposed
            @throws ::com::sun::star::uno::RuntimeException
                if attaching as script listener to the manager fails with a RuntimeException itself
        */
        virtual void registerEventAttacherManager(
            const ::com::sun::star::uno::Reference< ::com::sun::star::script::XEventAttacherManager >& _rxManager ) = 0;

        /** registers an XEventAttacherManager whose events should not be monitored and handled anymore

            @param _rxManager
                the XEventAttacherManager which was previously registered. Must not ne <NULL/>.

            @throws ::com::sun::star::lang::IllegalArgumentException
                if <arg>_rxManager</arg> is <NULL/>
            @throws ::com::sun::star::lang::DisposedException
                if the instance is already disposed
            @throws ::com::sun::star::uno::RuntimeException
                if removing as script listener from the manager fails with a RuntimeException itself
        */
        virtual void revokeEventAttacherManager(
            const ::com::sun::star::uno::Reference< ::com::sun::star::script::XEventAttacherManager >& _rxManager ) = 0;

        /** disposes the scripting environment instance
        */
        virtual void dispose() = 0;

        virtual ~IFormScriptingEnvironment();
    };
    typedef ::rtl::Reference< IFormScriptingEnvironment >   PFormScriptingEnvironment;

    //====================================================================
    /** creates a default component implementing the IFormScriptingEnvironment interface
    */
    PFormScriptingEnvironment   createDefaultFormScriptingEnvironment( FmFormModel& _rFormModel );

//........................................................................
} // namespace svxform
//........................................................................

#endif // SVX_FMSCRIPTINGENV_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
