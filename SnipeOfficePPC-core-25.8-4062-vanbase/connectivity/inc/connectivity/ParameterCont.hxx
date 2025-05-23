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
#ifndef CONNECTIVITY_PARAMETERCONTINUATION_HXX_INCLUDED
#define CONNECTIVITY_PARAMETERCONTINUATION_HXX_INCLUDED

#include <com/sun/star/sdb/XInteractionSupplyParameters.hpp>
#include <comphelper/interaction.hxx>
#include "connectivity/dbtoolsdllapi.hxx"

namespace dbtools
{
//====================================================================
    //= OParameterContinuation
    //====================================================================
    class OOO_DLLPUBLIC_DBTOOLS OParameterContinuation : public comphelper::OInteraction< ::com::sun::star::sdb::XInteractionSupplyParameters >
    {
        ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >       m_aValues;

    public:
        OParameterContinuation() { }

        ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >   getValues() const { return m_aValues; }

        // XInteractionSupplyParameters
        virtual void SAL_CALL setParameters( const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& _rValues ) throw(::com::sun::star::uno::RuntimeException);

    protected:
        virtual ~OParameterContinuation() { }
    private:
        OParameterContinuation(const OParameterContinuation&);
        void operator =(const OParameterContinuation&);
    };
} // dbtools
#endif //CONNECTIVITY_PARAMETERCONTINUATION_HXX_INCLUDED

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
