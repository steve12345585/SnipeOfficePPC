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
#ifndef TOOLKIT_CONTROLS_TKSIMPLEANIMATION_HXX
#define TOOLKIT_CONTROLS_TKSIMPLEANIMATION_HXX

#include <toolkit/controls/unocontrolmodel.hxx>
#include <toolkit/helper/servicenames.hxx>
#include <toolkit/controls/unocontrolbase.hxx>
#include <toolkit/helper/macros.hxx>
#include <com/sun/star/graphic/XGraphic.hpp>
#include <com/sun/star/awt/XSimpleAnimation.hpp>
#include <comphelper/uno3.hxx>
#include <cppuhelper/implbase1.hxx>

//........................................................................
namespace toolkit
{
//........................................................................

    //====================================================================
    //= UnoSimpleAnimationControlModel
    //====================================================================
    class UnoSimpleAnimationControlModel : public UnoControlModel
    {
    protected:
        ::com::sun::star::uno::Any      ImplGetDefaultValue( sal_uInt16 nPropId ) const;
        ::cppu::IPropertyArrayHelper&   SAL_CALL getInfoHelper();

    public:
                            UnoSimpleAnimationControlModel( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& i_factory );
                            UnoSimpleAnimationControlModel( const UnoSimpleAnimationControlModel& rModel ) : UnoControlModel( rModel ) {;}

        UnoControlModel*    Clone() const { return new UnoSimpleAnimationControlModel( *this ); }

        // XMultiPropertySet
        ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  ) throw(::com::sun::star::uno::RuntimeException);

        // XPersistObject
        ::rtl::OUString SAL_CALL getServiceName() throw(::com::sun::star::uno::RuntimeException);

        // XServiceInfo
        ::rtl::OUString SAL_CALL getImplementationName(  ) throw(::com::sun::star::uno::RuntimeException);
        ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames() throw(::com::sun::star::uno::RuntimeException);
    };

    //====================================================================
    //= UnoSimpleAnimationControl
    //====================================================================

    typedef ::cppu::AggImplInheritanceHelper1   <   UnoControlBase
                                                ,   ::com::sun::star::awt::XSimpleAnimation
                                                >   UnoSimpleAnimationControl_Base;

    class UnoSimpleAnimationControl : public UnoSimpleAnimationControl_Base
    {
    private:

    public:
                                    UnoSimpleAnimationControl( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& i_factory );
        ::rtl::OUString             GetComponentServiceName();

        // XSimpleAnimation
        virtual void SAL_CALL start() throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL stop() throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL setImageList( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::graphic::XGraphic > >& ImageList )
                                            throw (::com::sun::star::uno::RuntimeException);

        // XServiceInfo
        ::rtl::OUString SAL_CALL getImplementationName(  ) throw(::com::sun::star::uno::RuntimeException);
        ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames() throw(::com::sun::star::uno::RuntimeException);
    };

//........................................................................
} // namespacetoolkit
//........................................................................

#endif // TOOLKIT_CONTROLS_TKSIMPLEANIMATION_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
