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

#ifndef _XMLOFF_SETTINGSEXPORTHELPER_HXX
#define _XMLOFF_SETTINGSEXPORTHELPER_HXX

#include "xmloff/dllapi.h"

#include <com/sun/star/awt/Rectangle.hpp>
#include <com/sun/star/formula/SymbolDescriptor.hpp>
#include <com/sun/star/util/XStringSubstitution.hpp>
#include <xmloff/xmlexp.hxx>

namespace com
{
    namespace sun { namespace star {
        namespace container { class XNameAccess; class XIndexAccess; }
        namespace util { struct DateTime; }
    } }
}

namespace xmloff
{
    class XMLSettingsExportContext;
}

class XMLOFF_DLLPUBLIC XMLSettingsExportHelper
{
    ::xmloff::XMLSettingsExportContext& m_rContext;

    ::com::sun::star::uno::Reference< ::com::sun::star::util::XStringSubstitution > mxStringSubsitution;

    const rtl::OUString msPrinterIndependentLayout;
    const rtl::OUString msColorTableURL;
    const rtl::OUString msLineEndTableURL;
    const rtl::OUString msHatchTableURL;
    const rtl::OUString msDashTableURL;
    const rtl::OUString msGradientTableURL;
    const rtl::OUString msBitmapTableURL;

    void ManipulateSetting( com::sun::star::uno::Any& rAny, const rtl::OUString& rName ) const;

    void CallTypeFunction(const com::sun::star::uno::Any& rAny,
                        const rtl::OUString& rName) const;

    void exportBool(const sal_Bool bValue, const rtl::OUString& rName) const;
    void exportByte(const sal_Int8 nValue, const rtl::OUString& rName) const;
    void exportShort(const sal_Int16 nValue, const rtl::OUString& rName) const;
    void exportInt(const sal_Int32 nValue, const rtl::OUString& rName) const;
    void exportLong(const sal_Int64 nValue, const rtl::OUString& rName) const;
    void exportDouble(const double fValue, const rtl::OUString& rName) const;
    void exportString(const rtl::OUString& sValue, const rtl::OUString& rName) const;
    void exportDateTime(const com::sun::star::util::DateTime& aValue, const rtl::OUString& rName) const;
    void exportSequencePropertyValue(
        const com::sun::star::uno::Sequence<com::sun::star::beans::PropertyValue>& aProps,
        const rtl::OUString& rName) const;
    void exportbase64Binary(
        const com::sun::star::uno::Sequence<sal_Int8>& aProps,
        const rtl::OUString& rName) const;
    void exportMapEntry(const com::sun::star::uno::Any& rAny,
                        const rtl::OUString& rName,
                        const sal_Bool bNameAccess) const;
    void exportNameAccess(
        const com::sun::star::uno::Reference<com::sun::star::container::XNameAccess>& aNamed,
        const rtl::OUString& rName) const;
    void exportIndexAccess(
        const com::sun::star::uno::Reference<com::sun::star::container::XIndexAccess> aIndexed,
        const rtl::OUString rName) const;

    void exportSymbolDescriptors(
                    const com::sun::star::uno::Sequence < com::sun::star::formula::SymbolDescriptor > &rProps,
                    const rtl::OUString rName) const;
    void exportForbiddenCharacters(
                    const com::sun::star::uno::Any &rAny,
                    const rtl::OUString rName) const;

public:
    XMLSettingsExportHelper( ::xmloff::XMLSettingsExportContext& i_rContext );
    ~XMLSettingsExportHelper();

    void exportAllSettings(
        const com::sun::star::uno::Sequence<com::sun::star::beans::PropertyValue>& aProps,
        const rtl::OUString& rName) const;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
