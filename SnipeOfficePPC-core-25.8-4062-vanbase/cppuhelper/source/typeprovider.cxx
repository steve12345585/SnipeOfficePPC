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


#include <cppuhelper/typeprovider.hxx>
#include <osl/mutex.hxx>

using namespace osl;
using namespace com::sun::star::uno;

namespace cppu
{

//__________________________________________________________________________________________________
OImplementationId::~OImplementationId() SAL_THROW(())
{
    delete _pSeq;
}
//__________________________________________________________________________________________________
Sequence< sal_Int8 > OImplementationId::getImplementationId() const SAL_THROW(())
{
    if (! _pSeq)
    {
        MutexGuard aGuard( Mutex::getGlobalMutex() );
        if (! _pSeq)
        {
            Sequence< sal_Int8 > * pSeq = new Sequence< sal_Int8 >( 16 );
            ::rtl_createUuid( (sal_uInt8 *)pSeq->getArray(), 0, _bUseEthernetAddress );
            _pSeq = pSeq;
        }
    }
    return *_pSeq;
}

//--------------------------------------------------------------------------------------------------
static inline void copy( Sequence< Type > & rDest, const Sequence< Type > & rSource, sal_Int32 nOffset )
    SAL_THROW(())
{
    Type * pDest = rDest.getArray();
    const Type * pSource = rSource.getConstArray();

    for ( sal_Int32 nPos = rSource.getLength(); nPos--; )
        pDest[nOffset+ nPos] = pSource[nPos];
}

//__________________________________________________________________________________________________
OTypeCollection::OTypeCollection(
    const Type & rType1,
    const Sequence< Type > & rAddTypes )
    SAL_THROW(())
    : _aTypes( 1 + rAddTypes.getLength() )
{
    _aTypes[0] = rType1;
    copy( _aTypes, rAddTypes, 1 );
}
//__________________________________________________________________________________________________
OTypeCollection::OTypeCollection(
    const Type & rType1,
    const Type & rType2,
    const Sequence< Type > & rAddTypes )
    SAL_THROW(())
    : _aTypes( 2 + rAddTypes.getLength() )
{
    _aTypes[0] = rType1;
    _aTypes[1] = rType2;
    copy( _aTypes, rAddTypes, 2 );
}
//__________________________________________________________________________________________________
OTypeCollection::OTypeCollection(
    const Type & rType1,
    const Type & rType2,
    const Type & rType3,
    const Sequence< Type > & rAddTypes )
    SAL_THROW(())
    : _aTypes( 3 + rAddTypes.getLength() )
{
    _aTypes[0] = rType1;
    _aTypes[1] = rType2;
    _aTypes[2] = rType3;
    copy( _aTypes, rAddTypes, 3 );
}
//__________________________________________________________________________________________________
OTypeCollection::OTypeCollection(
    const Type & rType1,
    const Type & rType2,
    const Type & rType3,
    const Type & rType4,
    const Sequence< Type > & rAddTypes )
    SAL_THROW(())
    : _aTypes( 4 + rAddTypes.getLength() )
{
    _aTypes[0] = rType1;
    _aTypes[1] = rType2;
    _aTypes[2] = rType3;
    _aTypes[3] = rType4;
    copy( _aTypes, rAddTypes, 4 );
}
//__________________________________________________________________________________________________
OTypeCollection::OTypeCollection(
    const Type & rType1,
    const Type & rType2,
    const Type & rType3,
    const Type & rType4,
    const Type & rType5,
    const Sequence< Type > & rAddTypes )
    SAL_THROW(())
    : _aTypes( 5 + rAddTypes.getLength() )
{
    _aTypes[0] = rType1;
    _aTypes[1] = rType2;
    _aTypes[2] = rType3;
    _aTypes[3] = rType4;
    _aTypes[4] = rType5;
    copy( _aTypes, rAddTypes, 5 );
}
//__________________________________________________________________________________________________
OTypeCollection::OTypeCollection(
    const Type & rType1,
    const Type & rType2,
    const Type & rType3,
    const Type & rType4,
    const Type & rType5,
    const Type & rType6,
    const Sequence< Type > & rAddTypes )
    SAL_THROW(())
    : _aTypes( 6 + rAddTypes.getLength() )
{
    _aTypes[0] = rType1;
    _aTypes[1] = rType2;
    _aTypes[2] = rType3;
    _aTypes[3] = rType4;
    _aTypes[4] = rType5;
    _aTypes[5] = rType6;
    copy( _aTypes, rAddTypes, 6 );
}
//__________________________________________________________________________________________________
OTypeCollection::OTypeCollection(
    const Type & rType1,
    const Type & rType2,
    const Type & rType3,
    const Type & rType4,
    const Type & rType5,
    const Type & rType6,
    const Type & rType7,
    const Sequence< Type > & rAddTypes )
    SAL_THROW(())
    : _aTypes( 7 + rAddTypes.getLength() )
{
    _aTypes[0] = rType1;
    _aTypes[1] = rType2;
    _aTypes[2] = rType3;
    _aTypes[3] = rType4;
    _aTypes[4] = rType5;
    _aTypes[5] = rType6;
    _aTypes[6] = rType7;
    copy( _aTypes, rAddTypes, 7 );
}
//__________________________________________________________________________________________________
OTypeCollection::OTypeCollection(
    const Type & rType1,
    const Type & rType2,
    const Type & rType3,
    const Type & rType4,
    const Type & rType5,
    const Type & rType6,
    const Type & rType7,
    const Type & rType8,
    const Sequence< Type > & rAddTypes )
    SAL_THROW(())
    : _aTypes( 8 + rAddTypes.getLength() )
{
    _aTypes[0] = rType1;
    _aTypes[1] = rType2;
    _aTypes[2] = rType3;
    _aTypes[3] = rType4;
    _aTypes[4] = rType5;
    _aTypes[5] = rType6;
    _aTypes[6] = rType7;
    _aTypes[7] = rType8;
    copy( _aTypes, rAddTypes, 8 );
}
//__________________________________________________________________________________________________
OTypeCollection::OTypeCollection(
    const Type & rType1,
    const Type & rType2,
    const Type & rType3,
    const Type & rType4,
    const Type & rType5,
    const Type & rType6,
    const Type & rType7,
    const Type & rType8,
    const Type & rType9,
    const Sequence< Type > & rAddTypes )
    SAL_THROW(())
    : _aTypes( 9 + rAddTypes.getLength() )
{
    _aTypes[0] = rType1;
    _aTypes[1] = rType2;
    _aTypes[2] = rType3;
    _aTypes[3] = rType4;
    _aTypes[4] = rType5;
    _aTypes[5] = rType6;
    _aTypes[6] = rType7;
    _aTypes[7] = rType8;
    _aTypes[8] = rType9;
    copy( _aTypes, rAddTypes, 9 );
}
//__________________________________________________________________________________________________
OTypeCollection::OTypeCollection(
    const Type & rType1,
    const Type & rType2,
    const Type & rType3,
    const Type & rType4,
    const Type & rType5,
    const Type & rType6,
    const Type & rType7,
    const Type & rType8,
    const Type & rType9,
    const Type & rType10,
    const Sequence< Type > & rAddTypes )
    SAL_THROW(())
    : _aTypes( 10 + rAddTypes.getLength() )
{
    _aTypes[0] = rType1;
    _aTypes[1] = rType2;
    _aTypes[2] = rType3;
    _aTypes[3] = rType4;
    _aTypes[4] = rType5;
    _aTypes[5] = rType6;
    _aTypes[6] = rType7;
    _aTypes[7] = rType8;
    _aTypes[8] = rType9;
    _aTypes[9] = rType10;
    copy( _aTypes, rAddTypes, 10 );
}
//__________________________________________________________________________________________________
OTypeCollection::OTypeCollection(
    const Type & rType1,
    const Type & rType2,
    const Type & rType3,
    const Type & rType4,
    const Type & rType5,
    const Type & rType6,
    const Type & rType7,
    const Type & rType8,
    const Type & rType9,
    const Type & rType10,
    const Type & rType11,
    const Sequence< Type > & rAddTypes )
    SAL_THROW(())
    : _aTypes( 11 + rAddTypes.getLength() )
{
    _aTypes[0] = rType1;
    _aTypes[1] = rType2;
    _aTypes[2] = rType3;
    _aTypes[3] = rType4;
    _aTypes[4] = rType5;
    _aTypes[5] = rType6;
    _aTypes[6] = rType7;
    _aTypes[7] = rType8;
    _aTypes[8] = rType9;
    _aTypes[9] = rType10;
    _aTypes[10] = rType11;
    copy( _aTypes, rAddTypes, 11 );
}
//__________________________________________________________________________________________________
OTypeCollection::OTypeCollection(
    const Type & rType1,
    const Type & rType2,
    const Type & rType3,
    const Type & rType4,
    const Type & rType5,
    const Type & rType6,
    const Type & rType7,
    const Type & rType8,
    const Type & rType9,
    const Type & rType10,
    const Type & rType11,
    const Type & rType12,
    const Sequence< Type > & rAddTypes )
    SAL_THROW(())
    : _aTypes( 12 + rAddTypes.getLength() )
{
    _aTypes[0] = rType1;
    _aTypes[1] = rType2;
    _aTypes[2] = rType3;
    _aTypes[3] = rType4;
    _aTypes[4] = rType5;
    _aTypes[5] = rType6;
    _aTypes[6] = rType7;
    _aTypes[7] = rType8;
    _aTypes[8] = rType9;
    _aTypes[9] = rType10;
    _aTypes[10] = rType11;
    _aTypes[11] = rType12;
    copy( _aTypes, rAddTypes, 12 );
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
