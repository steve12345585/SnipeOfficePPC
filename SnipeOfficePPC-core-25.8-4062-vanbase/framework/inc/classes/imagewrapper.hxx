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

#ifndef __FRAMEWORK_CLASSES_IMAGEWRAPPER_HXX_
#define __FRAMEWORK_CLASSES_IMAGEWRAPPER_HXX_

#include <threadhelp/threadhelpbase.hxx>
#include <com/sun/star/awt/XBitmap.hpp>
#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <cppuhelper/implbase2.hxx>
#include <vcl/image.hxx>
#include <framework/fwedllapi.h>

namespace framework
{

class FWE_DLLPUBLIC ImageWrapper : public ThreadHelpBase                            ,   // Struct for right initalization of mutex member! Must be first of baseclasses.
                     public ::cppu::WeakImplHelper2< ::com::sun::star::awt::XBitmap,
                                                    ::com::sun::star::lang::XUnoTunnel >
{
    public:
        ImageWrapper( const Image& aImage );
        virtual ~ImageWrapper();

        const Image&    GetImage() const
        {
            return m_aImage;
        }

        static ::com::sun::star::uno::Sequence< sal_Int8 > GetUnoTunnelId();

        // XBitmap
        virtual ::com::sun::star::awt::Size SAL_CALL getSize() throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getDIB() throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getMaskDIB() throw (::com::sun::star::uno::RuntimeException);

        // XUnoTunnel
        virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier ) throw (::com::sun::star::uno::RuntimeException);

    private:
        Image   m_aImage;
};

}

#endif // __FRAMEWORK_CLASSES_IMAGEWRAPPER_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
