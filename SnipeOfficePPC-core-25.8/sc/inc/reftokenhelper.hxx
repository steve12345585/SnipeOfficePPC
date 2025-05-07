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

#ifndef SC_REFTOKENHELPER_HXX
#define SC_REFTOKENHELPER_HXX

#include "token.hxx"

#include <vector>

namespace rtl {
    class OUString;
}

class ScDocument;
class ScRange;
class ScRangeList;

class ScRefTokenHelper
{
private:
    ScRefTokenHelper();
    ScRefTokenHelper(const ScRefTokenHelper&);
    ~ScRefTokenHelper();

public:
    /**
     * Compile an array of reference tokens from a data source range string.
     * The source range may consist of multiple ranges separated by ';'s.
     */
    static void compileRangeRepresentation(
        ::std::vector<ScTokenRef>& rRefTokens, const ::rtl::OUString& rRangeStr, ScDocument* pDoc,
        const sal_Unicode cSep, ::formula::FormulaGrammar::Grammar eGrammar);

    static bool getRangeFromToken(ScRange& rRange, const ScTokenRef& pToken, bool bExternal = false);

    static void getRangeListFromTokens(ScRangeList& rRangeList, const ::std::vector<ScTokenRef>& pTokens);

    /**
     * Create a double reference token from a range object.
     */
    static void getTokenFromRange(ScTokenRef& pToken, const ScRange& rRange);

    static void getTokensFromRangeList(::std::vector<ScTokenRef>& pTokens, const ScRangeList& rRanges);

    static bool SC_DLLPUBLIC isRef(const ScTokenRef& pToken);
    static bool SC_DLLPUBLIC isExternalRef(const ScTokenRef& pToken);

    static bool SC_DLLPUBLIC intersects(const ::std::vector<ScTokenRef>& rTokens, const ScTokenRef& pToken);

    static void SC_DLLPUBLIC join(::std::vector<ScTokenRef>& rTokens, const ScTokenRef& pToken);

    static bool getDoubleRefDataFromToken(ScComplexRefData& rData, const ScTokenRef& pToken);

    static ScTokenRef createRefToken(const ScAddress& rAddr);
    static ScTokenRef createRefToken(const ScRange& rRange);
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
