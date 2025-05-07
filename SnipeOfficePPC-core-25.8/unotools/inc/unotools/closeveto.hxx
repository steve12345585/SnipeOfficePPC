/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
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

#ifndef UNOTOOLS_CLOSEVETO_HXX
#define UNOTOOLS_CLOSEVETO_HXX

#include "unotools/unotoolsdllapi.h"

/** === begin UNO includes === **/
#include <com/sun/star/uno/XInterface.hpp>
/** === end UNO includes === **/

#include <boost/scoped_ptr.hpp>

//......................................................................................................................
namespace utl
{
//......................................................................................................................

    //==================================================================================================================
    //= CloseVeto
    //==================================================================================================================
    struct CloseVeto_Data;
    /** will add a XCloseListener to a given component, and veto its closing as long as the <code>CloseVeto</code>
        instance is alive.

        If closing has been requested and vetoed while the <code>CloseVeto</code> instance is alive, and the ownership
        went to the <code>CloseVeto</code> instance, then it will close the component in its dtor.
    */
    class UNOTOOLS_DLLPUBLIC CloseVeto
    {
    public:
        CloseVeto( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& i_closeable );
        ~CloseVeto();

    private:
        ::boost::scoped_ptr< CloseVeto_Data >   m_pData;
    };

//......................................................................................................................
} // namespace dbaui
//......................................................................................................................

#endif // UNOTOOLS_CLOSEVETO_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
