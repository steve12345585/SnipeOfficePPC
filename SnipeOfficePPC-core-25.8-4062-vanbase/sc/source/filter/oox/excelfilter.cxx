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

#include "excelfilter.hxx"

#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>
#include "oox/helper/binaryinputstream.hxx"
#include "biffinputstream.hxx"
#include "excelchartconverter.hxx"
#include "excelvbaproject.hxx"
#include "stylesbuffer.hxx"
#include "themebuffer.hxx"
#include "workbookfragment.hxx"

namespace oox {
namespace xls {

// ============================================================================

using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::sheet;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::xml::sax;
using namespace ::oox::core;

using ::rtl::OUString;
using ::oox::drawingml::table::TableStyleListPtr;

// ============================================================================

ExcelFilterBase::ExcelFilterBase() :
    mpBookGlob( 0 )
{
}

ExcelFilterBase::~ExcelFilterBase()
{
    OSL_ENSURE( !mpBookGlob, "ExcelFilterBase::~ExcelFilterBase - workbook data not cleared" );
}

void ExcelFilterBase::registerWorkbookGlobals( WorkbookGlobals& rBookGlob )
{
    mpBookGlob = &rBookGlob;
}

WorkbookGlobals& ExcelFilterBase::getWorkbookGlobals() const
{
    OSL_ENSURE( mpBookGlob, "ExcelFilterBase::getWorkbookGlobals - missing workbook data" );
    return *mpBookGlob;
}

void ExcelFilterBase::unregisterWorkbookGlobals()
{
    mpBookGlob = 0;
}

// ============================================================================

OUString SAL_CALL ExcelFilter_getImplementationName() throw()
{
    return OUString( "com.sun.star.comp.oox.xls.ExcelFilter" );
}

Sequence< OUString > SAL_CALL ExcelFilter_getSupportedServiceNames() throw()
{
    Sequence< OUString > aSeq( 2 );
    aSeq[ 0 ] = "com.sun.star.document.ImportFilter";
    aSeq[ 1 ] = "com.sun.star.document.ExportFilter";
    return aSeq;
}

Reference< XInterface > SAL_CALL ExcelFilter_createInstance(
        const Reference< XComponentContext >& rxContext ) throw( Exception )
{
    return static_cast< ::cppu::OWeakObject* >( new ExcelFilter( rxContext ) );
}

// ----------------------------------------------------------------------------

ExcelFilter::ExcelFilter( const Reference< XComponentContext >& rxContext ) throw( RuntimeException ) :
    XmlFilterBase( rxContext )
{
}

ExcelFilter::~ExcelFilter()
{
}

bool ExcelFilter::importDocument() throw()
{
    /*  To activate the XLSX/XLSB dumper, insert the full path to the file
        file:///<path-to-oox-module>/source/dump/xlsbdumper.ini
        into the environment variable OOO_XLSBDUMPER and start the office with
        this variable (nonpro only). */
    //OOX_DUMP_FILE( ::oox::dump::xlsb::Dumper );

    OUString aWorkbookPath = getFragmentPathFromFirstType( CREATE_OFFICEDOC_RELATION_TYPE( "officeDocument" ) );
    if( aWorkbookPath.isEmpty() )
        return false;

    /*  Construct the WorkbookGlobals object referred to by every instance of
        the class WorkbookHelper, and execute the import filter by constructing
        an instance of WorkbookFragment and loading the file. */
    WorkbookGlobalsRef xBookGlob = WorkbookHelper::constructGlobals( *this );
    if ( xBookGlob.get() && importFragment( new WorkbookFragment( *xBookGlob, aWorkbookPath ) ) )
    {
        try
        {
            importDocumentProperties();
        }
        catch( const Exception& e )
        {
            SAL_WARN("sc", "exception when importing document properties " << e.Message);
        }
        return true;
    }
    return false;
}

bool ExcelFilter::exportDocument() throw()
{
    return false;
}

const ::oox::drawingml::Theme* ExcelFilter::getCurrentTheme() const
{
    return &WorkbookHelper( getWorkbookGlobals() ).getTheme();
}

::oox::vml::Drawing* ExcelFilter::getVmlDrawing()
{
    return 0;
}

const TableStyleListPtr ExcelFilter::getTableStyles()
{
    return TableStyleListPtr();
}

::oox::drawingml::chart::ChartConverter* ExcelFilter::getChartConverter()
{
    return WorkbookHelper( getWorkbookGlobals() ).getChartConverter();
}

GraphicHelper* ExcelFilter::implCreateGraphicHelper() const
{
    return new ExcelGraphicHelper( getWorkbookGlobals() );
}

::oox::ole::VbaProject* ExcelFilter::implCreateVbaProject() const
{
    return new ExcelVbaProject( getComponentContext(), Reference< XSpreadsheetDocument >( getModel(), UNO_QUERY ) );
}


sal_Bool SAL_CALL ExcelFilter::filter( const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& rDescriptor ) throw( ::com::sun::star::uno::RuntimeException )
{
    if ( XmlFilterBase::filter( rDescriptor ) )
        return true;

    if ( isExportFilter() )
    {
        Reference< XExporter > xExporter( getServiceFactory()->createInstance( "com.sun.star.comp.oox.ExcelFilterExport" ), UNO_QUERY );

        if ( xExporter.is() )
        {
            Reference< XComponent > xDocument( getModel(), UNO_QUERY );
            Reference< XFilter > xFilter( xExporter, UNO_QUERY );

            if ( xFilter.is() )
            {
                xExporter->setSourceDocument( xDocument );
                if ( xFilter->filter( rDescriptor ) )
                    return true;
            }
        }
    }

    return false;
}

OUString ExcelFilter::implGetImplementationName() const
{
    return ExcelFilter_getImplementationName();
}

} // namespace xls
} // namespace oox

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
