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
#ifndef RPT_XMLTABLE_HXX
#define RPT_XMLTABLE_HXX

#include <xmloff/xmlictxt.hxx>
#include <com/sun/star/report/XSection.hpp>
#include <vector>

namespace rptxml
{
    class ORptFilter;
    class OXMLTable : public SvXMLImportContext
    {
    public:
        struct TCell
        {
            sal_Int32 nWidth;
            sal_Int32 nHeight;
            sal_Int32 nColSpan;
            sal_Int32 nRowSpan;
            ::std::vector< ::com::sun::star::uno::Reference< ::com::sun::star::report::XReportComponent> > xElements;
            TCell() : nWidth(0),nHeight(0),nColSpan(1),nRowSpan(1){}
        };
    private:
        ::std::vector< ::std::vector<TCell> >                                               m_aGrid;
        ::std::vector<sal_Int32>                                                            m_aHeight;
        ::std::vector<sal_Int32>                                                            m_aWidth;
        ::com::sun::star::uno::Reference< ::com::sun::star::report::XSection >              m_xSection;
        ::rtl::OUString                                                                     m_sStyleName;
        sal_Int32                                                                           m_nColSpan;
        sal_Int32                                                                           m_nRowSpan;
        sal_Int32                                                                           m_nRowIndex;
        sal_Int32                                                                           m_nColumnIndex;
        ORptFilter& GetOwnImport();

        OXMLTable(const OXMLTable&);
        void operator =(const OXMLTable&);
    public:

        OXMLTable( ORptFilter& rImport
                    ,sal_uInt16 nPrfx
                    ,const ::rtl::OUString& rLName
                    ,const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList > & xAttrList
                    ,const ::com::sun::star::uno::Reference< ::com::sun::star::report::XSection >& _xSection
                    );
        virtual ~OXMLTable();

        virtual SvXMLImportContext *CreateChildContext( sal_uInt16 nPrefix,
                    const ::rtl::OUString& rLocalName,
                    const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList > & xAttrList );

        virtual void EndElement();

        inline void addHeight(sal_Int32 _nHeight)   { m_aHeight.push_back( _nHeight ); }
        inline void addWidth(sal_Int32 _nWidth)     { m_aWidth.push_back( _nWidth ); }

        inline void setColumnSpanned(sal_Int32 _nColSpan)     { m_nColSpan = _nColSpan; }
        inline void setRowSpanned(   sal_Int32 _nRowSpan)     { m_nRowSpan = _nRowSpan; }

        void incrementRowIndex();
        inline void incrementColumnIndex()  { ++m_nColumnIndex; }

        inline sal_Int32 getRowIndex() const { return m_nRowIndex; }
        inline sal_Int32 getColumnIndex() const { return m_nColumnIndex; }

        void addCell(const ::com::sun::star::uno::Reference< ::com::sun::star::report::XReportComponent>& _xElement);

        inline ::com::sun::star::uno::Reference< ::com::sun::star::report::XSection > getSection() const { return m_xSection; }
    };
// -----------------------------------------------------------------------------
} // namespace rptxml
// -----------------------------------------------------------------------------

#endif // RPT_XMLTABLE_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
