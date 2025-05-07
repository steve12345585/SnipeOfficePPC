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



#include <osl/mutex.hxx>
#include <com/sun/star/accessibility/AccessibleRole.hpp>
#include <com/sun/star/accessibility/AccessibleStateType.hpp>
#include <unotools/accessiblestatesethelper.hxx>
#include <comphelper/servicehelper.hxx>
#include <vcl/svapp.hxx>
#include <hffrm.hxx>
#include "accheaderfooter.hxx"
#include "access.hrc"

using namespace ::com::sun::star;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::accessibility;
using ::rtl::OUString;

const sal_Char sServiceNameHeader[] = "com.sun.star.text.AccessibleHeaderView";
const sal_Char sServiceNameFooter[] = "com.sun.star.text.AccessibleFooterView";
const sal_Char sImplementationNameHeader[] = "com.sun.star.comp.Writer.SwAccessibleHeaderView";
const sal_Char sImplementationNameFooter[] = "com.sun.star.comp.Writer.SwAccessibleFooterView";

SwAccessibleHeaderFooter::SwAccessibleHeaderFooter(
        SwAccessibleMap* pInitMap,
        const SwHeaderFrm* pHdFrm    ) :
    SwAccessibleContext( pInitMap, AccessibleRole::HEADER, pHdFrm )
{
    SolarMutexGuard aGuard;

    OUString sArg( OUString::valueOf( (sal_Int32)pHdFrm->GetPhyPageNum() ) );
    SetName( GetResource( STR_ACCESS_HEADER_NAME, &sArg ) );
}

SwAccessibleHeaderFooter::SwAccessibleHeaderFooter(
        SwAccessibleMap* pInitMap,
        const SwFooterFrm* pFtFrm    ) :
    SwAccessibleContext( pInitMap, AccessibleRole::FOOTER, pFtFrm )
{
    SolarMutexGuard aGuard;

    OUString sArg( OUString::valueOf( (sal_Int32)pFtFrm->GetPhyPageNum() ) );
    SetName( GetResource( STR_ACCESS_FOOTER_NAME, &sArg ) );
}

SwAccessibleHeaderFooter::~SwAccessibleHeaderFooter()
{
}

OUString SAL_CALL SwAccessibleHeaderFooter::getAccessibleDescription (void)
        throw (uno::RuntimeException)
{
    SolarMutexGuard aGuard;

    CHECK_FOR_DEFUNC( XAccessibleContext )

    sal_uInt16 nResId = AccessibleRole::HEADER == GetRole()
        ? STR_ACCESS_HEADER_DESC
        : STR_ACCESS_FOOTER_DESC ;

    OUString sArg( GetFormattedPageNumber() );

    return GetResource( nResId, &sArg );
}

OUString SAL_CALL SwAccessibleHeaderFooter::getImplementationName()
        throw( RuntimeException )
{
    if( AccessibleRole::HEADER == GetRole() )
        return OUString(RTL_CONSTASCII_USTRINGPARAM(sImplementationNameHeader));
    else
        return OUString(RTL_CONSTASCII_USTRINGPARAM(sImplementationNameFooter));
}

sal_Bool SAL_CALL SwAccessibleHeaderFooter::supportsService(
        const ::rtl::OUString& sTestServiceName)
    throw (uno::RuntimeException)
{
    if( sTestServiceName.equalsAsciiL( sAccessibleServiceName,
                                       sizeof(sAccessibleServiceName)-1 ) )
        return sal_True;
    else if( AccessibleRole::HEADER == GetRole() )
        return sTestServiceName.equalsAsciiL( sServiceNameHeader, sizeof(sServiceNameHeader)-1 );
    else
        return sTestServiceName.equalsAsciiL( sServiceNameFooter, sizeof(sServiceNameFooter)-1 );

}

Sequence< OUString > SAL_CALL SwAccessibleHeaderFooter::getSupportedServiceNames()
        throw( uno::RuntimeException )
{
    Sequence< OUString > aRet(2);
    OUString* pArray = aRet.getArray();
    if( AccessibleRole::HEADER == GetRole() )
        pArray[0] = OUString( RTL_CONSTASCII_USTRINGPARAM(sServiceNameHeader) );
    else
        pArray[0] = OUString( RTL_CONSTASCII_USTRINGPARAM(sServiceNameFooter) );
    pArray[1] = OUString( RTL_CONSTASCII_USTRINGPARAM(sAccessibleServiceName) );
    return aRet;
}

namespace
{
    class theSwAccessibleHeaderFooterImplementationId : public rtl::Static< UnoTunnelIdInit, theSwAccessibleHeaderFooterImplementationId > {};
}

Sequence< sal_Int8 > SAL_CALL SwAccessibleHeaderFooter::getImplementationId()
        throw(RuntimeException)
{
    return theSwAccessibleHeaderFooterImplementationId::get().getSeq();
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
