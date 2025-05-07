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


// prevent internal compiler error with MSVC6SP3
#include <utility>

#define TRANSLITERATION_SeZe_ja_JP
#include <transliteration_Ignore.hxx>

using namespace com::sun::star::uno;
using namespace com::sun::star::lang;

namespace com { namespace sun { namespace star { namespace i18n {

static Mapping SeZe[] = {
        // SI + E --> SE
        { 0x30B7, 0x30A7, 0x30BB, sal_True },
        // SI + E --> SE
        { 0x3057, 0x3047, 0x305B, sal_True },
        // ZI + E --> ZE
        { 0x30B8, 0x30A7, 0x30BC, sal_True },
        // ZI + E --> ZE
        { 0x3058, 0x3047, 0x305C, sal_True },

        { 0, 0, 0, sal_True }
};

ignoreSeZe_ja_JP::ignoreSeZe_ja_JP()
{
        func = (TransFunc) 0;
        table = 0;
        map = SeZe;
        transliterationName = "ignoreSeZe_ja_JP";
        implementationName = "com.sun.star.i18n.Transliteration.ignoreSeZe_ja_JP";
}

} } } }

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
