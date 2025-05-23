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

#include "sal/config.h"

#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/Sequence.hxx"
#include "com/sun/star/xml/dom/XElement.hpp"
#include "com/sun/star/xml/dom/XNodeList.hpp"
#include "rtl/bootstrap.hxx"
#include "rtl/string.h"
#include "rtl/ustring.h"
#include "rtl/ustring.hxx"
#include "sal/types.h"
#include "tools/resid.hxx"
#include "unotools/configmgr.hxx"

#include "deployment.hrc"
#include "dp_resource.h"

#include "dp_dependencies.hxx"
#include "dp_descriptioninfoset.hxx"
#include "dp_version.hxx"

namespace {

static char const namespaceLibreOffice[] =
    "http://snipeoffice.org/extensions/description/2011";

static char const namespaceOpenOfficeOrg[] =
    "http://openoffice.org/extensions/description/2006";

static char const minimalVersionLibreOffice[] = "LibreOffice-minimal-version";

static char const minimalVersionOpenOfficeOrg[] =
    "OpenOffice.org-minimal-version";

static char const maximalVersionOpenOfficeOrg[] =
    "OpenOffice.org-maximal-version";

rtl::OUString getLibreOfficeMajorMinorMicro() {
    return utl::ConfigManager::getAboutBoxProductVersion();
}

rtl::OUString getReferenceOpenOfficeOrgMajorMinor() {
    rtl::OUString v(
        RTL_CONSTASCII_USTRINGPARAM(
            "${$BRAND_BASE_DIR/program/" SAL_CONFIGFILE("version")
            ":Version:ReferenceOOoMajorMinor}"));
    rtl::Bootstrap::expandMacros(v); //TODO: check for failure
    return v;
}

bool satisfiesMinimalVersion(
    rtl::OUString const & actual, rtl::OUString const & specified)
{
    return dp_misc::compareVersions(actual, specified) != dp_misc::LESS;
}

bool satisfiesMaximalVersion(
    rtl::OUString const & actual, rtl::OUString const & specified)
{
    return dp_misc::compareVersions(actual, specified) != dp_misc::GREATER;
}

rtl::OUString produceErrorText(
    rtl::OUString const & reason, rtl::OUString const & version)
{
    return reason.replaceFirst("%VERSION",
        (version.isEmpty()
         ?  dp_misc::getResId(RID_DEPLOYMENT_DEPENDENCIES_UNKNOWN).toString()
         : version));
}

}

namespace dp_misc {

namespace Dependencies {

css::uno::Sequence< css::uno::Reference< css::xml::dom::XElement > >
check(dp_misc::DescriptionInfoset const & infoset) {
    css::uno::Reference< css::xml::dom::XNodeList > deps(
        infoset.getDependencies());
    sal_Int32 n = deps->getLength();
    css::uno::Sequence< css::uno::Reference< css::xml::dom::XElement > >
        unsatisfied(n);
    sal_Int32 unsat = 0;
    for (sal_Int32 i = 0; i < n; ++i) {
        css::uno::Reference< css::xml::dom::XElement > e(
            deps->item(i), css::uno::UNO_QUERY_THROW);
        bool sat = false;
        if ( e->getNamespaceURI() == namespaceOpenOfficeOrg && e->getTagName() == minimalVersionOpenOfficeOrg )
        {
            sat = satisfiesMinimalVersion(
                getReferenceOpenOfficeOrgMajorMinor(),
                e->getAttribute(
                    rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("value"))));
        } else if ( e->getNamespaceURI() == namespaceOpenOfficeOrg && e->getTagName() == maximalVersionOpenOfficeOrg )
        {
            sat = satisfiesMaximalVersion(
                getReferenceOpenOfficeOrgMajorMinor(),
                e->getAttribute(
                    rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("value"))));
        } else if (e->getNamespaceURI() == namespaceLibreOffice && e->getTagName() == minimalVersionLibreOffice )
        {
            sat = satisfiesMinimalVersion(
                getLibreOfficeMajorMinorMicro(),
                e->getAttribute(
                    rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("value"))));
        } else if (e->hasAttributeNS(
                       rtl::OUString(
                           RTL_CONSTASCII_USTRINGPARAM(namespaceOpenOfficeOrg)),
                       rtl::OUString(
                           RTL_CONSTASCII_USTRINGPARAM(
                               minimalVersionOpenOfficeOrg))))
        {
            sat = satisfiesMinimalVersion(
                getReferenceOpenOfficeOrgMajorMinor(),
                e->getAttributeNS(
                    rtl::OUString(
                        RTL_CONSTASCII_USTRINGPARAM(namespaceOpenOfficeOrg)),
                    rtl::OUString(
                        RTL_CONSTASCII_USTRINGPARAM(
                            minimalVersionOpenOfficeOrg))));
        }
        if (!sat) {
            unsatisfied[unsat++] = e;
        }
    }
    unsatisfied.realloc(unsat);
    return unsatisfied;
}

rtl::OUString getErrorText(
    css::uno::Reference< css::xml::dom::XElement > const & dependency)
{
    OSL_ASSERT(dependency.is());
    if ( dependency->getNamespaceURI() == namespaceOpenOfficeOrg && dependency->getTagName() == minimalVersionOpenOfficeOrg )
    {
        return produceErrorText(
                dp_misc::getResId(RID_DEPLOYMENT_DEPENDENCIES_OOO_MIN).toString(),
            dependency->getAttribute(
                rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("value"))));
    } else if (dependency->getNamespaceURI() == namespaceOpenOfficeOrg && dependency->getTagName() == maximalVersionOpenOfficeOrg )
    {
        return produceErrorText(
                dp_misc::getResId(RID_DEPLOYMENT_DEPENDENCIES_OOO_MAX).toString(),
            dependency->getAttribute(
                rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("value"))));
    } else if (dependency->getNamespaceURI() == namespaceLibreOffice && dependency->getTagName() == minimalVersionLibreOffice )
    {
        return produceErrorText(
                dp_misc::getResId(RID_DEPLOYMENT_DEPENDENCIES_LO_MIN).toString(),
            dependency->getAttribute(
                rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("value"))));
    } else if (dependency->hasAttributeNS(
                   rtl::OUString(
                       RTL_CONSTASCII_USTRINGPARAM(namespaceOpenOfficeOrg)),
                   rtl::OUString(
                       RTL_CONSTASCII_USTRINGPARAM(
                           minimalVersionOpenOfficeOrg))))
    {
        return produceErrorText(
                dp_misc::getResId(RID_DEPLOYMENT_DEPENDENCIES_OOO_MIN).toString(),
            dependency->getAttributeNS(
                rtl::OUString(
                    RTL_CONSTASCII_USTRINGPARAM(namespaceOpenOfficeOrg)),
                rtl::OUString(
                    RTL_CONSTASCII_USTRINGPARAM(minimalVersionOpenOfficeOrg))));
    } else {
        return dp_misc::getResId(RID_DEPLOYMENT_DEPENDENCIES_UNKNOWN).toString();
    }
}

}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
