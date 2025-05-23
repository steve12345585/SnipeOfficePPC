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

#include <com/sun/star/i18n/XOrdinalSuffix.hpp>
#include <cppuhelper/implbase2.hxx> // helper for implementations
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/Locale.hpp>


namespace com { namespace sun { namespace star { namespace i18n {

class OrdinalSuffix : public cppu::WeakImplHelper2
<
    XOrdinalSuffix,
    com::sun::star::lang::XServiceInfo
>
{
    public:
        OrdinalSuffix(const com::sun::star::uno::Reference < com::sun::star::lang::XMultiServiceFactory >& rxMSF);
        virtual ~OrdinalSuffix();

        // XOrdinalSuffix
        virtual com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getOrdinalSuffix( sal_Int32 nNumber, const com::sun::star::lang::Locale &rLocale ) throw(com::sun::star::uno::RuntimeException);

        // XServiceInfo
        virtual rtl::OUString SAL_CALL getImplementationName() throw(com::sun::star::uno::RuntimeException);
        virtual sal_Bool SAL_CALL supportsService(const rtl::OUString& ServiceName) throw(com::sun::star::uno::RuntimeException);
        virtual com::sun::star::uno::Sequence < rtl::OUString > SAL_CALL getSupportedServiceNames() throw(com::sun::star::uno::RuntimeException);

    private:
        com::sun::star::uno::Reference < com::sun::star::lang::XMultiServiceFactory > _xServiceManager;
};
} } } }

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
