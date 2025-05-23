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
#ifndef RPT_EXPORTDOCUMENTHANDLER_HXX_INCLUDED
#define RPT_EXPORTDOCUMENTHANDLER_HXX_INCLUDED

#include "sal/config.h"
#include "com/sun/star/uno/XComponentContext.hpp"
#include <cppuhelper/implbase3.hxx>
#include "com/sun/star/xml/sax/XDocumentHandler.hpp"
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/chart2/XChartDocument.hpp>
#include "com/sun/star/chart2/data/XDatabaseDataProvider.hpp"
#include <comphelper/uno3.hxx>

namespace rptxml
{
typedef ::cppu::WeakAggImplHelper3< ::com::sun::star::xml::sax::XDocumentHandler
                                ,   ::com::sun::star::lang::XInitialization
                                ,   ::com::sun::star::lang::XServiceInfo>   ExportDocumentHandler_BASE;

class ExportDocumentHandler : public ExportDocumentHandler_BASE
{
public:
    // XServiceInfo - static versions
    static ::rtl::OUString getImplementationName_Static(  ) throw(::com::sun::star::uno::RuntimeException);
    static ::com::sun::star::uno::Sequence< ::rtl::OUString > getSupportedServiceNames_static(  ) throw(::com::sun::star::uno::RuntimeException);
    static ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL
                    create(const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >&);
public:
    explicit ExportDocumentHandler(::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > const & context);

private:
    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName(  ) throw(::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  ) throw(::com::sun::star::uno::RuntimeException);

    DECLARE_XINTERFACE( )
    DECLARE_XTYPEPROVIDER( )

    // ::com::sun::star::xml::sax::XDocumentHandler:
    virtual void SAL_CALL startDocument() throw (::com::sun::star::uno::RuntimeException, ::com::sun::star::xml::sax::SAXException);
    virtual void SAL_CALL endDocument() throw (::com::sun::star::uno::RuntimeException, ::com::sun::star::xml::sax::SAXException);
    virtual void SAL_CALL startElement(const ::rtl::OUString & aName, const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList > & xAttribs) throw (::com::sun::star::uno::RuntimeException, ::com::sun::star::xml::sax::SAXException);
    virtual void SAL_CALL endElement(const ::rtl::OUString & aName) throw (::com::sun::star::uno::RuntimeException, ::com::sun::star::xml::sax::SAXException);
    virtual void SAL_CALL characters(const ::rtl::OUString & aChars) throw (::com::sun::star::uno::RuntimeException, ::com::sun::star::xml::sax::SAXException);
    virtual void SAL_CALL ignorableWhitespace(const ::rtl::OUString & aWhitespaces) throw (::com::sun::star::uno::RuntimeException, ::com::sun::star::xml::sax::SAXException);
    virtual void SAL_CALL processingInstruction(const ::rtl::OUString & aTarget, const ::rtl::OUString & aData) throw (::com::sun::star::uno::RuntimeException, ::com::sun::star::xml::sax::SAXException);
    virtual void SAL_CALL setDocumentLocator(const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XLocator > & xLocator) throw (::com::sun::star::uno::RuntimeException, ::com::sun::star::xml::sax::SAXException);

    virtual void SAL_CALL initialize( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments ) throw (::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);

    void exportTableRows();
private:
    ExportDocumentHandler(ExportDocumentHandler &); // not defined
    void operator =(ExportDocumentHandler &); // not defined

    virtual ~ExportDocumentHandler();

    ::osl::Mutex                                                                        m_aMutex;
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >        m_xContext;
    ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XDocumentHandler >    m_xDelegatee;
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XAggregation >             m_xProxy;
    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XTypeProvider >           m_xTypeProvider;
    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XServiceInfo >            m_xServiceInfo;
    ::com::sun::star::uno::Reference< ::com::sun::star::chart2::XChartDocument >        m_xModel;
    ::com::sun::star::uno::Reference< ::com::sun::star::chart2::data::XDatabaseDataProvider >   m_xDatabaseDataProvider;
    ::com::sun::star::uno::Sequence< ::rtl::OUString >                                  m_aColumns;
    sal_Int32                                                                           m_nCurrentCellIndex;
    sal_Int32                                                                           m_nColumnCount;
    bool m_bTableRowsStarted;
    bool m_bFirstRowExported;
    bool m_bExportChar;
    bool m_bCountColumnHeader;
};
// -----------------------------------------------------------------------------
} // namespace rptxml
// -----------------------------------------------------------------------------
#endif // RPT_EXPORTDOCUMENTHANDLER_HXX_INCLUDED

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
