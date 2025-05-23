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

#ifndef ERROBJECT_HXX
#define ERROBJECT_HXX
#include "sbunoobj.hxx"
#include <ooo/vba/XErrObject.hpp>


class SbxErrObject : public SbUnoObject
{
    class ErrObject* m_pErrObject;
    com::sun::star::uno::Reference< ooo::vba::XErrObject > m_xErr;

    SbxErrObject( const OUString& aName_, const com::sun::star::uno::Any& aUnoObj_ );
    ~SbxErrObject();

    class ErrObject* getImplErrObject( void )
        { return m_pErrObject; }

public:
    static SbxVariableRef getErrObject();
    static com::sun::star::uno::Reference< ooo::vba::XErrObject > getUnoErrObject();

    void setNumberAndDescription( ::sal_Int32 _number, const OUString& _description )
        throw (com::sun::star::uno::RuntimeException);
};
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
