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

#ifndef _EERDLL2_HXX
#define _EERDLL2_HXX

#include <com/sun/star/linguistic2/XLanguageGuessing.hpp>
#include <editeng/forbiddencharacterstable.hxx>
#include <rtl/ref.hxx>
#include <comphelper/scoped_disposing_ptr.hxx>

class SfxPoolItem;

class GlobalEditData
{
private:
    ::com::sun::star::uno::Reference<
        ::com::sun::star::linguistic2::XLanguageGuessing >  xLanguageGuesser;
    comphelper::scoped_disposing_solar_mutex_reset_ptr<OutputDevice> m_aStdRefDevice;
    SfxPoolItem**   ppDefItems;

    rtl::Reference<SvxForbiddenCharactersTable> xForbiddenCharsTable;

public:
                    GlobalEditData();
                    ~GlobalEditData();

    SfxPoolItem**   GetDefItems();
    OutputDevice*   GetStdRefDevice();

    rtl::Reference<SvxForbiddenCharactersTable> GetForbiddenCharsTable();
    void            SetForbiddenCharsTable( rtl::Reference<SvxForbiddenCharactersTable> xForbiddenChars ) { xForbiddenCharsTable = xForbiddenChars; }
    ::com::sun::star::uno::Reference< ::com::sun::star::linguistic2::XLanguageGuessing > GetLanguageGuesser();
};


#endif //_EERDLL2_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
