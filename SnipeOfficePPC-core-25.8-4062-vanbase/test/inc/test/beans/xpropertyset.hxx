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
 *   Copyright (C) 2012 Kohei Yoshida <kohei.yoshida@suse.com>
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

#ifndef __TEST_BEANS_XPROPERTYSET_HXX__
#define __TEST_BEANS_XPROPERTYSET_HXX__

#include <com/sun/star/uno/Reference.hxx>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/XPropertySetInfo.hpp>

#include "test/testdllapi.hxx"

#include <vector>

using namespace com::sun::star;

namespace apitest {

class OOO_DLLPUBLIC_TEST XPropertySet
{
public:
    virtual ~XPropertySet();

    virtual uno::Reference<uno::XInterface> init() = 0;

    void testGetPropertySetInfo();
    void testAddPropertyChangeListener();
    void testAddVetoableChangeListener();
    void testSetPropertyValue();
    void testGetPropertyValue();
    void testRemovePropertyChangeListener();
    void testRemoveVetoableChangeListener();

protected:
    virtual bool isPropertyValueChangeable(const rtl::OUString& rName);

private:
    void fillPropsToTest(const uno::Reference<beans::XPropertySetInfo>& xPropInfo);
    static bool getSinglePropertyValue(
        const uno::Reference<beans::XPropertySet>& xPropSet, const rtl::OUString& rName);

    struct OOO_DLLPUBLIC_TEST PropsToTest
    {
        std::vector<rtl::OUString> constrained;
        std::vector<rtl::OUString> bound;
        std::vector<rtl::OUString> normal;
        std::vector<rtl::OUString> readonly;

        bool initialized;

        PropsToTest();
    };

    PropsToTest maPropsToTest;
};

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
