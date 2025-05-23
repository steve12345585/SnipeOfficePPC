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

#ifndef _VCL_UNOHELP_HXX
#define _VCL_UNOHELP_HXX

#include <com/sun/star/uno/Reference.h>
#include <rtl/ustring.hxx>
#include <vcl/dllapi.h>

namespace com {
namespace sun {
namespace star {
namespace i18n {
    class XBreakIterator;
    class XCharacterClassification;
    class XCollator;
}}}}

namespace com {
namespace sun {
namespace star {
namespace accessibility {
    struct AccessibleEventObject;
}
}}}

namespace comphelper {
    namespace string {
        class NaturalStringSorter;
}}

namespace vcl
{
namespace unohelper
{
VCL_DLLPUBLIC ::com::sun::star::uno::Reference < ::com::sun::star::i18n::XBreakIterator > CreateBreakIterator();
VCL_DLLPUBLIC ::com::sun::star::uno::Reference < ::com::sun::star::i18n::XCharacterClassification> CreateCharacterClassification();
//Get access to singleton Natural String Sorter collating for Application::GetLocale
VCL_DLLPUBLIC const comphelper::string::NaturalStringSorter& getNaturalStringSorterForAppLocale();
VCL_DLLPUBLIC ::rtl::OUString CreateLibraryName( const sal_Char* pModName, sal_Bool bSUPD );
VCL_DLLPUBLIC void NotifyAccessibleStateEventGlobally( const ::com::sun::star::accessibility::AccessibleEventObject& rEventObject );
}}  // namespace vcl::unohelper

#endif  // _VCL_UNOHELP_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
