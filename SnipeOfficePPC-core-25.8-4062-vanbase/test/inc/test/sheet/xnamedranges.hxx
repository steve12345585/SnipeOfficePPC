/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * Version: MPL 1.1 / GPLv3+ / LGPLv3+
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License or as specified alternatively below. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * Major Contributor(s):
 * Copyright (C) 2011 Laurent Godard lgodard.libre@laposte.net (initial developer)
 *
 * All Rights Reserved.
 *
 * For minor contributions see the git repository.
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 3 or later (the "GPLv3+"), or
 * the GNU Lesser General Public License Version 3 or later (the "LGPLv3+"),
 * in which case the provisions of the GPLv3+ or the LGPLv3+ are applicable
 * instead of those above.
 */

#include <rtl/ustring.hxx>
#include <com/sun/star/uno/Reference.hxx>
#include <com/sun/star/sheet/XSpreadsheet.hpp>
#include <test/testdllapi.hxx>

using namespace com::sun::star;

namespace apitest {

class OOO_DLLPUBLIC_TEST XNamedRanges
{
public:
    // remove default entry
    XNamedRanges();
    // removes given entry
    XNamedRanges(const rtl::OUString& rNameToRemove);

    virtual ~XNamedRanges();

    virtual uno::Reference< uno::XInterface > init(sal_Int32 nSheets = 0) = 0;

    // XNamedRanges
    void testAddNewByName();
    void testAddNewFromTitles();
    void testRemoveByName();
    void testOutputList();

protected:
    uno::Reference< sheet::XSpreadsheet > xSheet;

private:
    rtl::OUString maNameToRemove;
};

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
