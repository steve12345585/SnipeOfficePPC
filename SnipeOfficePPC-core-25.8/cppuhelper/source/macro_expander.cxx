/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/


#include <rtl/bootstrap.hxx>

#include <uno/mapping.hxx>

#include <cppuhelper/factory.hxx>
#include <cppuhelper/compbase2.hxx>
#include <cppuhelper/component_context.hxx>

#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/util/XMacroExpander.hpp>
#include "com/sun/star/uno/RuntimeException.hpp"

#include "macro_expander.hxx"
#include "paths.hxx"

#define OUSTR(x) ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(x) )
#define SERVICE_NAME_A "com.sun.star.lang.MacroExpander"
#define SERVICE_NAME_B "com.sun.star.lang.BootstrapMacroExpander"
#define IMPL_NAME "com.sun.star.lang.comp.cppuhelper.BootstrapMacroExpander"


using namespace ::rtl;
using namespace ::osl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

namespace cppu
{

Bootstrap const & get_unorc() SAL_THROW(())
{
    static rtlBootstrapHandle s_bstrap = 0;
    if (! s_bstrap)
    {
        OUString iniName(getUnoIniUri());
        rtlBootstrapHandle bstrap = rtl_bootstrap_args_open( iniName.pData );

        ClearableMutexGuard guard( Mutex::getGlobalMutex() );
        if (s_bstrap)
        {
            guard.clear();
            rtl_bootstrap_args_close( bstrap );
        }
        else
        {
            s_bstrap = bstrap;
        }
    }
    return *(Bootstrap const *)&s_bstrap;
}

}

namespace cppuhelper { namespace detail {

rtl::OUString expandMacros(rtl::OUString const & text) {
    rtl::OUString t(text);
    rtl_bootstrap_expandMacros_from_handle(
        cppu::get_unorc().getHandle(), &t.pData);
    return t;
}

} }

namespace
{

class ImplNames
{
private:
    Sequence<OUString> m_aNames;
public:
    ImplNames() : m_aNames(2)
    {
        m_aNames[0] = OUSTR(SERVICE_NAME_A);
        m_aNames[1] = OUSTR(SERVICE_NAME_B);
    }
    const Sequence<OUString>& getNames() const { return m_aNames; }
};

class theImplNames : public rtl::Static<ImplNames, theImplNames> {};

inline OUString s_impl_name()
{
    return OUSTR(IMPL_NAME);
}

inline Sequence< OUString > const & s_get_service_names()
{
    return theImplNames::get().getNames();
}

typedef ::cppu::WeakComponentImplHelper2<
    util::XMacroExpander, lang::XServiceInfo > t_uno_impl;

struct mutex_holder
{
    Mutex m_mutex;
};

class Bootstrap_MacroExpander : public mutex_holder, public t_uno_impl
{
protected:
    virtual void SAL_CALL disposing();

public:
    inline Bootstrap_MacroExpander() SAL_THROW(())
        : t_uno_impl( m_mutex )
        {}
    virtual ~Bootstrap_MacroExpander()
        SAL_THROW(());

    // XMacroExpander impl
    virtual OUString SAL_CALL expandMacros( OUString const & exp )
        throw (lang::IllegalArgumentException);
    // XServiceInfo impl
    virtual OUString SAL_CALL getImplementationName()
        throw (RuntimeException);
    virtual sal_Bool SAL_CALL supportsService( OUString const & serviceName )
        throw (RuntimeException);
    virtual Sequence< OUString > SAL_CALL getSupportedServiceNames()
        throw (RuntimeException);
};

//__________________________________________________________________________________________________
void Bootstrap_MacroExpander::disposing()
{}
//__________________________________________________________________________________________________
Bootstrap_MacroExpander::~Bootstrap_MacroExpander() SAL_THROW(())
{}

// XServiceInfo impl
//__________________________________________________________________________________________________
OUString Bootstrap_MacroExpander::getImplementationName()
    throw (RuntimeException)
{
    return s_impl_name();
}
//__________________________________________________________________________________________________
sal_Bool Bootstrap_MacroExpander::supportsService( OUString const & serviceName )
    throw (RuntimeException)
{
    Sequence< OUString > const & service_names = s_get_service_names();
    OUString const * p = service_names.getConstArray();
    for ( sal_Int32 nPos = service_names.getLength(); nPos--; )
    {
        if (p[ nPos ].equals( serviceName ))
            return sal_True;
    }
    return sal_False;
}
//__________________________________________________________________________________________________
Sequence< OUString > Bootstrap_MacroExpander::getSupportedServiceNames()
    throw (RuntimeException)
{
    return s_get_service_names();
}

// XMacroExpander impl
//__________________________________________________________________________________________________
OUString Bootstrap_MacroExpander::expandMacros( OUString const & exp )
    throw (lang::IllegalArgumentException)
{
    return cppuhelper::detail::expandMacros( exp );
}

//==================================================================================================
Reference< XInterface > SAL_CALL service_create(
    SAL_UNUSED_PARAMETER Reference< XComponentContext > const & )
    SAL_THROW( (RuntimeException) )
{
    return static_cast< ::cppu::OWeakObject * >( new Bootstrap_MacroExpander );
}

}

namespace cppuhelper { namespace detail {

Reference< lang::XSingleComponentFactory > create_bootstrap_macro_expander_factory() SAL_THROW(())
{
    Reference< lang::XSingleComponentFactory > free(::cppu::createSingleComponentFactory(
                                                        service_create,
                                                        s_impl_name(),
                                                        s_get_service_names() ));

    uno::Environment curr_env(Environment::getCurrent());
    uno::Environment target_env(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(CPPU_STRINGIFY(CPPU_ENV))));

    uno::Mapping target2curr(target_env, curr_env);

    return Reference<lang::XSingleComponentFactory>(
        reinterpret_cast<lang::XSingleComponentFactory *>(
            target2curr.mapInterface(free.get(), ::getCppuType(&free))),
        SAL_NO_ACQUIRE);
}

} }

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
