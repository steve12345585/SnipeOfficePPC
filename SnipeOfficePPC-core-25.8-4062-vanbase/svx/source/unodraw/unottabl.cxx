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

#include <com/sun/star/awt/Gradient.hpp>
#include <svl/itempool.hxx>
#include <svl/itemset.hxx>
#include <svx/xgrad.hxx>
#include <svx/xflftrit.hxx>

#include <svx/svdmodel.hxx>
#include <svx/xflhtit.hxx>
#include "svx/unofill.hxx"
#include <svx/unomid.hxx>
#include "UnoNameItemTable.hxx"

using namespace ::com::sun::star;
using namespace ::rtl;
using namespace ::cppu;

class SvxUnoTransGradientTable : public SvxUnoNameItemTable
{
public:
    SvxUnoTransGradientTable( SdrModel* pModel ) throw();
    virtual ~SvxUnoTransGradientTable() throw();

    virtual NameOrIndex* createItem() const throw();

    // XServiceInfo
    virtual OUString SAL_CALL getImplementationName(  ) throw( uno::RuntimeException );
    virtual uno::Sequence<  OUString > SAL_CALL getSupportedServiceNames(  ) throw( uno::RuntimeException);

    // XElementAccess
    virtual uno::Type SAL_CALL getElementType(  ) throw( uno::RuntimeException);
};

SvxUnoTransGradientTable::SvxUnoTransGradientTable( SdrModel* pModel ) throw()
: SvxUnoNameItemTable( pModel, XATTR_FILLFLOATTRANSPARENCE, MID_FILLGRADIENT )
{
}

SvxUnoTransGradientTable::~SvxUnoTransGradientTable() throw()
{
}

OUString SAL_CALL SvxUnoTransGradientTable::getImplementationName() throw( uno::RuntimeException )
{
    return OUString( RTL_CONSTASCII_USTRINGPARAM("SvxUnoTransGradientTable") );
}

uno::Sequence< OUString > SAL_CALL SvxUnoTransGradientTable::getSupportedServiceNames(  )
    throw( uno::RuntimeException )
{
    uno::Sequence< OUString > aSNS( 1 );
    aSNS.getArray()[0] = OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.drawing.TransparencyGradientTable" ));
    return aSNS;
}

NameOrIndex* SvxUnoTransGradientTable::createItem() const throw()
{
    XFillFloatTransparenceItem* pNewItem = new XFillFloatTransparenceItem();
    pNewItem->SetEnabled( sal_True );
    return pNewItem;
}

// XElementAccess
uno::Type SAL_CALL SvxUnoTransGradientTable::getElementType(  )
    throw( uno::RuntimeException )
{
    return ::getCppuType((const struct awt::Gradient*)0);
}

/**
 * Create a hatchtable
 */
uno::Reference< uno::XInterface > SAL_CALL SvxUnoTransGradientTable_createInstance( SdrModel* pModel )
{
    return *new SvxUnoTransGradientTable(pModel);
}



/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
