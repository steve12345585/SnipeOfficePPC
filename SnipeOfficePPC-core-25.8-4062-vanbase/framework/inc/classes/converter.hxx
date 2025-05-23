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

#ifndef __FRAMEWORK_CLASSES_CONVERTER_HXX_
#define __FRAMEWORK_CLASSES_CONVERTER_HXX_

#include <general.h>
#include <stdtypes.h>

#include <com/sun/star/uno/Sequence.hxx>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/beans/NamedValue.hpp>
#include <tools/datetime.hxx>
#include <fwidllapi.h>

namespace framework{

class FWI_DLLPUBLIC Converter
{
    public:
        // Seq<beans.NamedValue> <=> Seq<beans.PropertyValue>
        static css::uno::Sequence< css::beans::NamedValue >    convert_seqPropVal2seqNamedVal  ( const css::uno::Sequence< css::beans::PropertyValue >& lSource );

        // Seq<String> => Vector<String>
        static OUStringList                                    convert_seqOUString2OUStringList( const css::uno::Sequence< ::rtl::OUString >&           lSource );

        static ::rtl::OUString                                 convert_DateTime2ISO8601        ( const DateTime&                                        aSource );
};

}       // namespace framework

#endif  // #ifndef __FRAMEWORK_CLASSES_CONVERTER_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
