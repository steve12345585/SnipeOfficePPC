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
#ifndef _UNO_CURRENT_CONTEXT_HXX_
#define _UNO_CURRENT_CONTEXT_HXX_

#include <uno/current_context.h>

#include <com/sun/star/uno/XCurrentContext.hpp>


namespace com
{
namespace sun
{
namespace star
{
namespace uno
{

/** Getting the current context.
    @attention
    Don't spread the returned interface around to other threads.  Every thread has its own
    current context.

    @return current context or null ref, if none is set
*/
inline Reference< XCurrentContext > SAL_CALL getCurrentContext()
    SAL_THROW(())
{
    Reference< XCurrentContext > xRet;
    ::rtl::OUString aEnvTypeName( RTL_CONSTASCII_USTRINGPARAM(CPPU_CURRENT_LANGUAGE_BINDING_NAME) );
    ::uno_getCurrentContext( (void **)&xRet, aEnvTypeName.pData, 0 );
    return xRet;
}
/** Setting the current context.

    @param xContext current context to be set
    @return true, if context has been successfully set
*/
inline bool SAL_CALL setCurrentContext(
    Reference< XCurrentContext > const & xContext )
    SAL_THROW(())
{
    ::rtl::OUString aEnvTypeName( RTL_CONSTASCII_USTRINGPARAM(CPPU_CURRENT_LANGUAGE_BINDING_NAME) );
    return (::uno_setCurrentContext( xContext.get(), aEnvTypeName.pData, 0 ) != sal_False);
}

/** Objects of this class are used for applying a current context until they are destructed, i.e.
    the ctor of this class saves the previous and sets the given context while the dtor restores
    the previous one upon destruction.
*/
class ContextLayer
{
    /** this C++ environment type name.
    */
    ::rtl::OUString m_aEnvTypeName;
    /** previous context
    */
    Reference< XCurrentContext > m_xPreviousContext;

public:
    /** Constructor: Saves the previous context and sets the new (given) one.

        @param xNewContext new context to be set
    */
    inline ContextLayer(
        Reference< XCurrentContext > const & xNewContext = Reference< XCurrentContext >() )
        SAL_THROW(());
    /** Destructor: restores the previous context.
    */
    inline ~ContextLayer() SAL_THROW(());

    /** Gets the previously set context.

        @return the previously set context
    */
    inline Reference< XCurrentContext > SAL_CALL getPreviousContext() const
        SAL_THROW(())
        { return m_xPreviousContext; }
};
//__________________________________________________________________________________________________
inline ContextLayer::ContextLayer( Reference< XCurrentContext > const & xNewContext )
    SAL_THROW(())
    : m_aEnvTypeName( RTL_CONSTASCII_USTRINGPARAM(CPPU_CURRENT_LANGUAGE_BINDING_NAME) )
{
    ::uno_getCurrentContext( (void **)&m_xPreviousContext, m_aEnvTypeName.pData, 0 );
    ::uno_setCurrentContext( xNewContext.get(), m_aEnvTypeName.pData, 0 );
}
//__________________________________________________________________________________________________
inline ContextLayer::~ContextLayer()
    SAL_THROW(())
{
    ::uno_setCurrentContext( m_xPreviousContext.get(), m_aEnvTypeName.pData, 0 );
}

}
}
}
}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
