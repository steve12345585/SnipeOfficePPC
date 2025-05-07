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
#ifndef CHART_TITLE_HXX
#define CHART_TITLE_HXX

#include "ServiceMacros.hxx"
#include "ModifyListenerHelper.hxx"
#include "OPropertySet.hxx"
#include "MutexContainer.hxx"
#include <cppuhelper/implbase5.hxx>
#include <comphelper/uno3.hxx>
#include <com/sun/star/chart2/XTitle.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/util/XCloneable.hpp>

namespace chart
{

namespace impl
{
typedef ::cppu::WeakImplHelper5<
    ::com::sun::star::chart2::XTitle,
    ::com::sun::star::lang::XServiceInfo,
    ::com::sun::star::util::XCloneable,
    ::com::sun::star::util::XModifyBroadcaster,
    ::com::sun::star::util::XModifyListener >
    Title_Base;
}

class Title :
    public MutexContainer,
    public impl::Title_Base,
    public ::property::OPropertySet
{
public:
    Title( ::com::sun::star::uno::Reference<
           ::com::sun::star::uno::XComponentContext > const & xContext );
    virtual ~Title();

    /// establish methods for factory instatiation
    APPHELPER_SERVICE_FACTORY_HELPER( Title )

    /// XServiceInfo declarations
    APPHELPER_XSERVICEINFO_DECL()

    /// merge XInterface implementations
     DECLARE_XINTERFACE()
    /// merge XTypeProvider implementations
     DECLARE_XTYPEPROVIDER()

protected:
    explicit Title( const Title & rOther );

    // ____ OPropertySet ____
    virtual ::com::sun::star::uno::Any GetDefaultValue( sal_Int32 nHandle ) const
        throw(::com::sun::star::beans::UnknownPropertyException);

    // ____ OPropertySet ____
    virtual ::cppu::IPropertyArrayHelper & SAL_CALL getInfoHelper();

    // ____ XPropertySet ____
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL
        getPropertySetInfo()
        throw (::com::sun::star::uno::RuntimeException);

    // ____ XTitle ____
    virtual ::com::sun::star::uno::Sequence<
        ::com::sun::star::uno::Reference<
        ::com::sun::star::chart2::XFormattedString > > SAL_CALL getText()
        throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setText( const ::com::sun::star::uno::Sequence<
                                   ::com::sun::star::uno::Reference<
                                   ::com::sun::star::chart2::XFormattedString > >& Strings )
        throw (::com::sun::star::uno::RuntimeException);

    // ____ XCloneable ____
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::util::XCloneable > SAL_CALL createClone()
        throw (::com::sun::star::uno::RuntimeException);

    // ____ XModifyBroadcaster ____
    virtual void SAL_CALL addModifyListener(
        const ::com::sun::star::uno::Reference< ::com::sun::star::util::XModifyListener >& aListener )
        throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removeModifyListener(
        const ::com::sun::star::uno::Reference< ::com::sun::star::util::XModifyListener >& aListener )
        throw (::com::sun::star::uno::RuntimeException);

    // ____ XModifyListener ____
    virtual void SAL_CALL modified(
        const ::com::sun::star::lang::EventObject& aEvent )
        throw (::com::sun::star::uno::RuntimeException);

    // ____ XEventListener (base of XModifyListener) ____
    virtual void SAL_CALL disposing(
        const ::com::sun::star::lang::EventObject& Source )
        throw (::com::sun::star::uno::RuntimeException);

    // ____ OPropertySet ____
    virtual void firePropertyChangeEvent();
    using OPropertySet::disposing;

    void fireModifyEvent();

private:
    ::com::sun::star::uno::Sequence<
        ::com::sun::star::uno::Reference<
            ::com::sun::star::chart2::XFormattedString > > m_aStrings;

    ::com::sun::star::uno::Reference< ::com::sun::star::util::XModifyListener > m_xModifyEventForwarder;
};

} //  namespace chart

// CHART_TITLE_HXX
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
