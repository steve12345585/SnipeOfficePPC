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

#ifndef _SVTOOLS_JAVAINTERACTION_HXX_
#define _SVTOOLS_JAVAINTERACTION_HXX_

#include "svtools/svtdllapi.h"
#include <osl/mutex.hxx>
#include <tools/gen.hxx>

#include <com/sun/star/task/XInteractionRequest.hpp>
#include <svtools/javacontext.hxx>

#define JAVA_INTERACTION_HANDLER_NAME "java-vm.interaction-handler"

namespace svt
{
// We cannot derive from  cppu::WeakImplHelper because we would export the inline
//generated class. This conflicts with other libraries if they use the same inline
//class.

class SVT_DLLPUBLIC JavaInteractionHandler:
        public com::sun::star::task::XInteractionHandler
{
public:
    JavaInteractionHandler(bool bReportErrorOnce = true);

    // XInterface
    virtual ::com::sun::star::uno::Any SAL_CALL queryInterface(
        const ::com::sun::star::uno::Type& aType )
        throw (::com::sun::star::uno::RuntimeException);

    virtual void SAL_CALL acquire() throw();

    virtual void SAL_CALL release() throw();

    // XCurrentContext
    virtual void SAL_CALL handle( const com::sun::star::uno::Reference<
                                  com::sun::star::task::XInteractionRequest >& Request )
        throw (com::sun::star::uno::RuntimeException);

private:
    oslInterlockedCount m_aRefCount;
    SVT_DLLPRIVATE JavaInteractionHandler(JavaInteractionHandler const&); //not implemented
    SVT_DLLPRIVATE JavaInteractionHandler& operator = (JavaInteractionHandler const &); //not implemented
    SVT_DLLPRIVATE virtual ~JavaInteractionHandler();
    bool m_bShowErrorsOnce;
    bool m_bJavaDisabled_Handled;
    bool m_bInvalidSettings_Handled;
    bool m_bJavaNotFound_Handled;
    bool m_bVMCreationFailure_Handled;
    bool m_bRestartRequired_Handled;
    sal_uInt16 m_nResult_JavaDisabled;
};
}

#endif // _DESKTOP_JAVAINTERACTION_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
