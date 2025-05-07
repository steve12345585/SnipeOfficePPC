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

#define TRANSLITERATION_Space_ja_JP
#include <transliteration_Ignore.hxx>

namespace com { namespace sun { namespace star { namespace i18n {

OneToOneMappingTable_t ignoreSpace_ja_JP_mappingTable[] = {
    { 0x0020, 0xffff },  // SPACE
    { 0x00A0, 0xffff },  // NO-BREAK SPACE
    { 0x2002, 0xffff },  // EN SPACE
    { 0x2003, 0xffff },  // EM SPACE
    { 0x2004, 0xffff },  // THREE-PER-EM SPACE
    { 0x2005, 0xffff },  // FOUR-PER-EM SPACE
    { 0x2006, 0xffff },  // SIX-PER-EM SPACE
    { 0x2007, 0xffff },  // FIGURE SPACE
    { 0x2008, 0xffff },  // PUNCTUATION SPACE
    { 0x2009, 0xffff },  // THIN SPACE
    { 0x200A, 0xffff },  // HAIR SPACE
    { 0x200B, 0xffff },  // ZERO WIDTH SPACE
    { 0x202F, 0xffff },  // NARROW NO-BREAK SPACE
    { 0x3000, 0xffff },  // IDEOGRAPHIC SPACE
    { 0x303F, 0xffff }   // IDEOGRAPHIC HALF FILL SPACE
};


ignoreSpace_ja_JP::ignoreSpace_ja_JP()
{
        func = (TransFunc)0;
        table = new oneToOneMapping(ignoreSpace_ja_JP_mappingTable, sizeof(ignoreSpace_ja_JP_mappingTable));
        map = 0;
        transliterationName = "ignoreSpace_ja_JP";
        implementationName = "com.sun.star.i18n.Transliteration.ignoreSpace_ja_JP";
}

} } } }

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
