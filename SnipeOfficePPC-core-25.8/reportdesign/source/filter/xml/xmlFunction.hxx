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
#ifndef RPT_XMLFUNCTION_HXX
#define RPT_XMLFUNCTION_HXX

#include <xmloff/xmlictxt.hxx>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/report/XFunctionsSupplier.hpp>
#include <com/sun/star/report/XFunctions.hpp>


namespace rptxml
{
    class ORptFilter;
    class OXMLFunction : public SvXMLImportContext
    {
    protected:
        ::com::sun::star::uno::Reference< ::com::sun::star::report::XFunctions >    m_xFunctions;
        ::com::sun::star::uno::Reference< ::com::sun::star::report::XFunction >     m_xFunction;
        bool                                                                        m_bAddToReport;

        ORptFilter& GetOwnImport();

        OXMLFunction(const OXMLFunction&);
        OXMLFunction operator =(const OXMLFunction&);
    public:

        OXMLFunction( ORptFilter& rImport
                    , sal_uInt16 nPrfx
                    ,const ::rtl::OUString& rLName
                    ,const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList > & xAttrList
                    ,const ::com::sun::star::uno::Reference< ::com::sun::star::report::XFunctionsSupplier >&    _xFunctions
                    ,bool _bAddToReport = false
                    );
        virtual ~OXMLFunction();

        virtual void EndElement();
    };
// -----------------------------------------------------------------------------
} // namespace rptxml
// -----------------------------------------------------------------------------

#endif // RPT_XMLFunction_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
