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


#include "SchXMLImport.hxx"
#include "SchXMLChartContext.hxx"
#include "contexts.hxx"
#include "XMLChartPropertySetMapper.hxx"
#include "SchXMLTools.hxx"

#include <tools/debug.hxx>
#include <rtl/ustrbuf.hxx>
#include <comphelper/processfactory.hxx>
#include "xmloff/xmlnmspe.hxx"
#include <xmloff/xmltoken.hxx>
#include <xmloff/xmluconv.hxx>
#include <xmloff/nmspmap.hxx>
#include <xmloff/xmlictxt.hxx>
#include <xmloff/xmlstyle.hxx>
#include <com/sun/star/task/XStatusIndicatorSupplier.hpp>
#include <com/sun/star/chart/XChartDocument.hpp>
#include <com/sun/star/chart/ChartDataRowSource.hpp>
#include <com/sun/star/container/XChild.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/chart2/data/XDataReceiver.hpp>
#include <com/sun/star/chart2/data/XDataProvider.hpp>
#include <com/sun/star/chart2/XChartDocument.hpp>
#include <com/sun/star/chart2/XCoordinateSystemContainer.hpp>
#include <com/sun/star/chart2/XChartTypeContainer.hpp>
#include <com/sun/star/chart2/XDataSeriesContainer.hpp>

#include <com/sun/star/document/XDocumentProperties.hpp>
#include <com/sun/star/document/XDocumentPropertiesSupplier.hpp>

#include <typeinfo>

using namespace com::sun::star;
using namespace ::xmloff::token;

using ::rtl::OUString;
using ::rtl::OUStringBuffer;
using ::rtl::OUStringToOString;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Sequence;

namespace
{
class lcl_MatchesChartType : public ::std::unary_function< Reference< chart2::XChartType >, bool >
{
public:
    explicit lcl_MatchesChartType( const OUString & aChartTypeName ) :
            m_aChartTypeName( aChartTypeName )
    {}

    bool operator () ( const Reference< chart2::XChartType > & xChartType ) const
    {
        return (xChartType.is() &&
                xChartType->getChartType().equals( m_aChartTypeName ));
    }

private:
    OUString m_aChartTypeName;
};
} // anonymous namespace

/* ----------------------------------------
   TokenMaps for distinguishing different
   tokens in different contexts
   ----------------------------------------*/

// ----------------------------------------
// element maps
// ----------------------------------------







// ----------------------------------------
// attribute maps
// ----------------------------------------









// ========================================

SchXMLImportHelper::SchXMLImportHelper() :
        mpAutoStyles( 0 ),

        mpChartDocElemTokenMap( 0 ),
        mpTableElemTokenMap( 0 ),
        mpChartElemTokenMap( 0 ),
        mpPlotAreaElemTokenMap( 0 ),
        mpSeriesElemTokenMap( 0 ),

        mpChartAttrTokenMap( 0 ),
        mpPlotAreaAttrTokenMap( 0 ),
        mpAutoStyleAttrTokenMap( 0 ),
        mpCellAttrTokenMap( 0 ),
        mpSeriesAttrTokenMap( 0 ),
        mpRegEquationAttrTokenMap( 0 )
{
}

SchXMLImportHelper::~SchXMLImportHelper()
{
        // delete token maps
    if( mpChartDocElemTokenMap )
        delete mpChartDocElemTokenMap;
    if( mpTableElemTokenMap )
        delete mpTableElemTokenMap;
    if( mpChartElemTokenMap )
        delete mpChartElemTokenMap;
    if( mpPlotAreaElemTokenMap )
        delete mpPlotAreaElemTokenMap;
    if( mpSeriesElemTokenMap )
        delete mpSeriesElemTokenMap;

    if( mpChartAttrTokenMap )
        delete mpChartAttrTokenMap;
    if( mpPlotAreaAttrTokenMap )
        delete mpPlotAreaAttrTokenMap;
    if( mpAutoStyleAttrTokenMap )
        delete mpAutoStyleAttrTokenMap;
    if( mpCellAttrTokenMap )
        delete mpCellAttrTokenMap;
    if( mpSeriesAttrTokenMap )
        delete mpSeriesAttrTokenMap;
}

SvXMLImportContext* SchXMLImportHelper::CreateChartContext(
    SvXMLImport& rImport,
    sal_uInt16 nPrefix, const OUString& rLocalName,
    const Reference< frame::XModel > xChartModel,
    const Reference< xml::sax::XAttributeList >& )
{
    SvXMLImportContext* pContext = 0;

    Reference< chart::XChartDocument > xDoc( xChartModel, uno::UNO_QUERY );
    if( xDoc.is())
    {
        mxChartDoc = xDoc;
        pContext = new SchXMLChartContext( *this, rImport, rLocalName );
    }
    else
    {
        OSL_FAIL( "No valid XChartDocument given as XModel" );
        pContext = new SvXMLImportContext( rImport, nPrefix, rLocalName );
    }

    return pContext;
}

/* ----------------------------------------
   get various token maps
   ----------------------------------------*/

const SvXMLTokenMap& SchXMLImportHelper::GetDocElemTokenMap()
{
    if( ! mpChartDocElemTokenMap )
    {
        static SvXMLTokenMapEntry aDocElemTokenMap[] =
        {
            { XML_NAMESPACE_OFFICE, XML_AUTOMATIC_STYLES,   XML_TOK_DOC_AUTOSTYLES  },
            { XML_NAMESPACE_OFFICE, XML_STYLES,             XML_TOK_DOC_STYLES  },
            { XML_NAMESPACE_OFFICE, XML_META,               XML_TOK_DOC_META    },
            { XML_NAMESPACE_OFFICE, XML_BODY,               XML_TOK_DOC_BODY    },
            XML_TOKEN_MAP_END
        };

        mpChartDocElemTokenMap = new SvXMLTokenMap( aDocElemTokenMap );
    } // if( ! mpChartDocElemTokenMap )

    return *mpChartDocElemTokenMap;
}

const SvXMLTokenMap& SchXMLImportHelper::GetTableElemTokenMap()
{
    if( ! mpTableElemTokenMap )
    {
        static SvXMLTokenMapEntry aTableElemTokenMap[] =
    {
        { XML_NAMESPACE_TABLE,  XML_TABLE_HEADER_COLUMNS,   XML_TOK_TABLE_HEADER_COLS   },
        { XML_NAMESPACE_TABLE,  XML_TABLE_COLUMNS,          XML_TOK_TABLE_COLUMNS       },
        { XML_NAMESPACE_TABLE,  XML_TABLE_COLUMN,           XML_TOK_TABLE_COLUMN        },
        { XML_NAMESPACE_TABLE,  XML_TABLE_HEADER_ROWS,      XML_TOK_TABLE_HEADER_ROWS   },
        { XML_NAMESPACE_TABLE,  XML_TABLE_ROWS,             XML_TOK_TABLE_ROWS          },
        { XML_NAMESPACE_TABLE,  XML_TABLE_ROW,              XML_TOK_TABLE_ROW           },
        XML_TOKEN_MAP_END
    };

        mpTableElemTokenMap = new SvXMLTokenMap( aTableElemTokenMap );
    } // if( ! mpTableElemTokenMap )

    return *mpTableElemTokenMap;
}

const SvXMLTokenMap& SchXMLImportHelper::GetChartElemTokenMap()
{
    if( ! mpChartElemTokenMap )
    {
        static SvXMLTokenMapEntry aChartElemTokenMap[] =
        {
            { XML_NAMESPACE_CHART,  XML_PLOT_AREA,              XML_TOK_CHART_PLOT_AREA     },
            { XML_NAMESPACE_CHART,  XML_TITLE,                  XML_TOK_CHART_TITLE         },
            { XML_NAMESPACE_CHART,  XML_SUBTITLE,               XML_TOK_CHART_SUBTITLE      },
            { XML_NAMESPACE_CHART,  XML_LEGEND,             XML_TOK_CHART_LEGEND        },
            { XML_NAMESPACE_TABLE,  XML_TABLE,                  XML_TOK_CHART_TABLE         },
            XML_TOKEN_MAP_END
        };

        mpChartElemTokenMap = new SvXMLTokenMap( aChartElemTokenMap );
    } // if( ! mpChartElemTokenMap )

    return *mpChartElemTokenMap;
}

const SvXMLTokenMap& SchXMLImportHelper::GetPlotAreaElemTokenMap()
{
    if( ! mpPlotAreaElemTokenMap )
    {
        static SvXMLTokenMapEntry aPlotAreaElemTokenMap[] =
{
    { XML_NAMESPACE_CHART_EXT,  XML_COORDINATE_REGION,      XML_TOK_PA_COORDINATE_REGION_EXT },
    { XML_NAMESPACE_CHART,  XML_COORDINATE_REGION,      XML_TOK_PA_COORDINATE_REGION },
    { XML_NAMESPACE_CHART,  XML_AXIS,                   XML_TOK_PA_AXIS             },
    { XML_NAMESPACE_CHART,  XML_SERIES,                 XML_TOK_PA_SERIES           },
    { XML_NAMESPACE_CHART,  XML_WALL,                   XML_TOK_PA_WALL             },
    { XML_NAMESPACE_CHART,  XML_FLOOR,                  XML_TOK_PA_FLOOR            },
    { XML_NAMESPACE_DR3D,   XML_LIGHT,                  XML_TOK_PA_LIGHT_SOURCE     },
    { XML_NAMESPACE_CHART,  XML_STOCK_GAIN_MARKER,      XML_TOK_PA_STOCK_GAIN       },
    { XML_NAMESPACE_CHART,  XML_STOCK_LOSS_MARKER,      XML_TOK_PA_STOCK_LOSS       },
    { XML_NAMESPACE_CHART,  XML_STOCK_RANGE_LINE,       XML_TOK_PA_STOCK_RANGE      },
    XML_TOKEN_MAP_END
};

        mpPlotAreaElemTokenMap = new SvXMLTokenMap( aPlotAreaElemTokenMap );
    } // if( ! mpPlotAreaElemTokenMap )

    return *mpPlotAreaElemTokenMap;
}

const SvXMLTokenMap& SchXMLImportHelper::GetSeriesElemTokenMap()
{
    if( ! mpSeriesElemTokenMap )
    {
        static SvXMLTokenMapEntry aSeriesElemTokenMap[] =
{
    { XML_NAMESPACE_CHART,  XML_DATA_POINT,       XML_TOK_SERIES_DATA_POINT       },
    { XML_NAMESPACE_CHART,  XML_DOMAIN,           XML_TOK_SERIES_DOMAIN           },
    { XML_NAMESPACE_CHART,  XML_MEAN_VALUE,       XML_TOK_SERIES_MEAN_VALUE_LINE  },
    { XML_NAMESPACE_CHART,  XML_REGRESSION_CURVE, XML_TOK_SERIES_REGRESSION_CURVE },
    { XML_NAMESPACE_CHART,  XML_ERROR_INDICATOR,  XML_TOK_SERIES_ERROR_INDICATOR  },
    XML_TOKEN_MAP_END
};

        mpSeriesElemTokenMap = new SvXMLTokenMap( aSeriesElemTokenMap );
    } // if( ! mpSeriesElemTokenMap )

    return *mpSeriesElemTokenMap;
}

// ----------------------------------------

const SvXMLTokenMap& SchXMLImportHelper::GetChartAttrTokenMap()
{
    if( ! mpChartAttrTokenMap )
    {
        static SvXMLTokenMapEntry aChartAttrTokenMap[] =
{
    { XML_NAMESPACE_XLINK,  XML_HREF,                   XML_TOK_CHART_HREF          },
    { XML_NAMESPACE_CHART,  XML_CLASS,                  XML_TOK_CHART_CLASS         },
    { XML_NAMESPACE_SVG,    XML_WIDTH,                  XML_TOK_CHART_WIDTH         },
    { XML_NAMESPACE_SVG,    XML_HEIGHT,                 XML_TOK_CHART_HEIGHT        },
    { XML_NAMESPACE_CHART,  XML_STYLE_NAME,             XML_TOK_CHART_STYLE_NAME    },
    { XML_NAMESPACE_CHART,  XML_COLUMN_MAPPING,         XML_TOK_CHART_COL_MAPPING   },
    { XML_NAMESPACE_CHART,  XML_ROW_MAPPING,            XML_TOK_CHART_ROW_MAPPING   },
    XML_TOKEN_MAP_END
};

        mpChartAttrTokenMap = new SvXMLTokenMap( aChartAttrTokenMap );
    } // if( ! mpChartAttrTokenMap )

    return *mpChartAttrTokenMap;
}

const SvXMLTokenMap& SchXMLImportHelper::GetPlotAreaAttrTokenMap()
{
    if( ! mpPlotAreaAttrTokenMap )
    {
        static SvXMLTokenMapEntry aPlotAreaAttrTokenMap[] =
{
    { XML_NAMESPACE_SVG,    XML_X,                      XML_TOK_PA_X                 },
    { XML_NAMESPACE_SVG,    XML_Y,                      XML_TOK_PA_Y                 },
    { XML_NAMESPACE_SVG,    XML_WIDTH,                  XML_TOK_PA_WIDTH             },
    { XML_NAMESPACE_SVG,    XML_HEIGHT,                 XML_TOK_PA_HEIGHT            },
    { XML_NAMESPACE_CHART,  XML_STYLE_NAME,             XML_TOK_PA_STYLE_NAME        },
    { XML_NAMESPACE_TABLE,  XML_CELL_RANGE_ADDRESS,     XML_TOK_PA_CHART_ADDRESS     },
    { XML_NAMESPACE_CHART,  XML_DATA_SOURCE_HAS_LABELS, XML_TOK_PA_DS_HAS_LABELS     },
    { XML_NAMESPACE_DR3D,   XML_TRANSFORM,              XML_TOK_PA_TRANSFORM         },
    { XML_NAMESPACE_DR3D,   XML_VRP,                    XML_TOK_PA_VRP               },
    { XML_NAMESPACE_DR3D,   XML_VPN,                    XML_TOK_PA_VPN               },
    { XML_NAMESPACE_DR3D,   XML_VUP,                    XML_TOK_PA_VUP               },
    { XML_NAMESPACE_DR3D,   XML_PROJECTION,             XML_TOK_PA_PROJECTION        },
    { XML_NAMESPACE_DR3D,   XML_DISTANCE,               XML_TOK_PA_DISTANCE          },
    { XML_NAMESPACE_DR3D,   XML_FOCAL_LENGTH,           XML_TOK_PA_FOCAL_LENGTH      },
    { XML_NAMESPACE_DR3D,   XML_SHADOW_SLANT,           XML_TOK_PA_SHADOW_SLANT      },
    { XML_NAMESPACE_DR3D,   XML_SHADE_MODE,             XML_TOK_PA_SHADE_MODE        },
    { XML_NAMESPACE_DR3D,   XML_AMBIENT_COLOR,          XML_TOK_PA_AMBIENT_COLOR     },
    { XML_NAMESPACE_DR3D,   XML_LIGHTING_MODE,          XML_TOK_PA_LIGHTING_MODE     },
    XML_TOKEN_MAP_END
};

        mpPlotAreaAttrTokenMap = new SvXMLTokenMap( aPlotAreaAttrTokenMap );
    } // if( ! mpPlotAreaAttrTokenMap )

    return *mpPlotAreaAttrTokenMap;
}

const SvXMLTokenMap& SchXMLImportHelper::GetCellAttrTokenMap()
{
    if( ! mpCellAttrTokenMap )
    {
        static SvXMLTokenMapEntry aCellAttrTokenMap[] =
{
    { XML_NAMESPACE_OFFICE, XML_VALUE_TYPE,             XML_TOK_CELL_VAL_TYPE       },
    { XML_NAMESPACE_OFFICE, XML_VALUE,                  XML_TOK_CELL_VALUE          },
    XML_TOKEN_MAP_END
};

        mpCellAttrTokenMap = new SvXMLTokenMap( aCellAttrTokenMap );
    } // if( ! mpCellAttrTokenMap )

    return *mpCellAttrTokenMap;
}

const SvXMLTokenMap& SchXMLImportHelper::GetSeriesAttrTokenMap()
{
    if( ! mpSeriesAttrTokenMap )
    {
        static SvXMLTokenMapEntry aSeriesAttrTokenMap[] =
{
    { XML_NAMESPACE_CHART,  XML_VALUES_CELL_RANGE_ADDRESS,  XML_TOK_SERIES_CELL_RANGE    },
    { XML_NAMESPACE_CHART,  XML_LABEL_CELL_ADDRESS,         XML_TOK_SERIES_LABEL_ADDRESS },
    { XML_NAMESPACE_CHART,  XML_ATTACHED_AXIS,              XML_TOK_SERIES_ATTACHED_AXIS },
    { XML_NAMESPACE_CHART,  XML_STYLE_NAME,                 XML_TOK_SERIES_STYLE_NAME    },
    { XML_NAMESPACE_CHART,  XML_CLASS,                      XML_TOK_SERIES_CHART_CLASS   },
    XML_TOKEN_MAP_END
};

        mpSeriesAttrTokenMap = new SvXMLTokenMap( aSeriesAttrTokenMap );
    } // if( ! mpSeriesAttrTokenMap )

    return *mpSeriesAttrTokenMap;
}

const SvXMLTokenMap& SchXMLImportHelper::GetRegEquationAttrTokenMap()
{
    if( ! mpRegEquationAttrTokenMap )
    {
        static SvXMLTokenMapEntry aRegressionEquationAttrTokenMap[] =
{
    { XML_NAMESPACE_CHART,  XML_STYLE_NAME,             XML_TOK_REGEQ_STYLE_NAME         },
    { XML_NAMESPACE_CHART,  XML_DISPLAY_EQUATION,       XML_TOK_REGEQ_DISPLAY_EQUATION   },
    { XML_NAMESPACE_CHART,  XML_DISPLAY_R_SQUARE,       XML_TOK_REGEQ_DISPLAY_R_SQUARE   },
    { XML_NAMESPACE_SVG,    XML_X,                      XML_TOK_REGEQ_POS_X              },
    { XML_NAMESPACE_SVG,    XML_Y,                      XML_TOK_REGEQ_POS_Y              },
    XML_TOKEN_MAP_END
};

        mpRegEquationAttrTokenMap = new SvXMLTokenMap( aRegressionEquationAttrTokenMap );
    } // if( ! mpRegEquationAttrTokenMap )

    return *mpRegEquationAttrTokenMap;
}

// ----------------------------------------

//static
void SchXMLImportHelper::DeleteDataSeries(
                    const Reference< chart2::XDataSeries > & xSeries,
                    const Reference< chart2::XChartDocument > & xDoc )
{
    if( xDoc.is() )
    try
    {
        Reference< chart2::XCoordinateSystemContainer > xCooSysCnt(
            xDoc->getFirstDiagram(), uno::UNO_QUERY_THROW );
        Sequence< Reference< chart2::XCoordinateSystem > > aCooSysSeq(
            xCooSysCnt->getCoordinateSystems());

        sal_Int32 nCooSysIndex = 0;
        for( nCooSysIndex=0; nCooSysIndex<aCooSysSeq.getLength(); nCooSysIndex++ )
        {
            Reference< chart2::XChartTypeContainer > xCTCnt( aCooSysSeq[ nCooSysIndex ], uno::UNO_QUERY_THROW );
            Sequence< Reference< chart2::XChartType > > aChartTypes( xCTCnt->getChartTypes());

            sal_Int32 nChartTypeIndex = 0;
            for( nChartTypeIndex=0; nChartTypeIndex<aChartTypes.getLength(); nChartTypeIndex++ )
            {
                Reference< chart2::XDataSeriesContainer > xSeriesCnt( aChartTypes[nChartTypeIndex], uno::UNO_QUERY_THROW );
                Sequence< Reference< chart2::XDataSeries > > aSeriesSeq( xSeriesCnt->getDataSeries());

                sal_Int32 nSeriesIndex = 0;
                for( nSeriesIndex=0; nSeriesIndex<aSeriesSeq.getLength(); nSeriesIndex++ )
                {
                    if( xSeries==aSeriesSeq[nSeriesIndex] )
                    {
                        xSeriesCnt->removeDataSeries(xSeries);
                        return;
                    }
                }
            }
        }
    }
    catch( const uno::Exception & ex )
    {
        (void)ex; // avoid warning for pro build
        OSL_FAIL( OUStringToOString(
                        OUString(  "Exception caught. Type: " ) +
                        OUString::createFromAscii( typeid( ex ).name()) +
                        OUString(  ", Message: " ) +
                        ex.Message, RTL_TEXTENCODING_ASCII_US ).getStr());
    }
}

// static
Reference< chart2::XDataSeries > SchXMLImportHelper::GetNewDataSeries(
    const Reference< chart2::XChartDocument > & xDoc,
    sal_Int32 nCoordinateSystemIndex,
    const OUString & rChartTypeName,
    bool bPushLastChartType /* = false */ )
{
    Reference< chart2::XDataSeries > xResult;
    if(!xDoc.is())
        return xResult;

    try
    {
        Reference< chart2::XCoordinateSystemContainer > xCooSysCnt(
            xDoc->getFirstDiagram(), uno::UNO_QUERY_THROW );
        Sequence< Reference< chart2::XCoordinateSystem > > aCooSysSeq(
            xCooSysCnt->getCoordinateSystems());
        Reference< uno::XComponentContext > xContext(
            comphelper::getProcessComponentContext() );

        if( nCoordinateSystemIndex < aCooSysSeq.getLength())
        {
            Reference< chart2::XChartType > xCurrentType;
            {
                Reference< chart2::XChartTypeContainer > xCTCnt( aCooSysSeq[ nCoordinateSystemIndex ], uno::UNO_QUERY_THROW );
                Sequence< Reference< chart2::XChartType > > aChartTypes( xCTCnt->getChartTypes());
                // find matching chart type group
                const Reference< chart2::XChartType > * pBegin = aChartTypes.getConstArray();
                const Reference< chart2::XChartType > * pEnd = pBegin + aChartTypes.getLength();
                const Reference< chart2::XChartType > * pIt =
                    ::std::find_if( pBegin, pEnd, lcl_MatchesChartType( rChartTypeName ));
                if( pIt != pEnd )
                    xCurrentType.set( *pIt );
                // if chart type is set at series and differs from current one,
                // create a new chart type
                if( !xCurrentType.is())
                {
                    xCurrentType.set(
                        xContext->getServiceManager()->createInstanceWithContext( rChartTypeName, xContext ),
                        uno::UNO_QUERY );
                    if( xCurrentType.is())
                    {
                        if( bPushLastChartType && aChartTypes.getLength())
                        {
                            sal_Int32 nIndex( aChartTypes.getLength() - 1 );
                            aChartTypes.realloc( aChartTypes.getLength() + 1 );
                            aChartTypes[ nIndex + 1 ] = aChartTypes[ nIndex ];
                            aChartTypes[ nIndex ] = xCurrentType;
                            xCTCnt->setChartTypes( aChartTypes );
                        }
                        else
                            xCTCnt->addChartType( xCurrentType );
                    }
                }
            }

            if( xCurrentType.is())
            {
                Reference< chart2::XDataSeriesContainer > xSeriesCnt( xCurrentType, uno::UNO_QUERY_THROW );
                Sequence< Reference< chart2::XDataSeries > > aSeriesSeq( xSeriesCnt->getDataSeries());

                if( xContext.is() )
                {
                    xResult.set(
                        xContext->getServiceManager()->createInstanceWithContext(
                            OUString( "com.sun.star.chart2.DataSeries" ),
                            xContext ), uno::UNO_QUERY_THROW );
                }
                if( xResult.is() )
                    xSeriesCnt->addDataSeries( xResult );
            }
        }
    }
    catch( const uno::Exception & ex )
    {
        (void)ex; // avoid warning for pro build
        OSL_FAIL( OUStringToOString(
                        OUString(  "Exception caught. Type: " ) +
                        OUString::createFromAscii( typeid( ex ).name()) +
                        OUString(  ", Message: " ) +
                        ex.Message, RTL_TEXTENCODING_ASCII_US ).getStr());
    }
    return xResult;
}

// ========================================

// #110680#
SchXMLImport::SchXMLImport(
    const Reference< lang::XMultiServiceFactory >& xServiceFactory,
    sal_uInt16 nImportFlags ) :
        SvXMLImport( xServiceFactory, nImportFlags )
{
    GetNamespaceMap().Add( GetXMLToken(XML_NP_XLINK), GetXMLToken(XML_N_XLINK), XML_NAMESPACE_XLINK );
    GetNamespaceMap().Add( GetXMLToken(XML_NP_CHART_EXT), GetXMLToken(XML_N_CHART_EXT), XML_NAMESPACE_CHART_EXT);

    mbIsGraphicLoadOnDemandSupported = false;
}

SchXMLImport::~SchXMLImport() throw ()
{
    // stop progress view
    if( mxStatusIndicator.is())
    {
        mxStatusIndicator->end();
        mxStatusIndicator->reset();
    }

    uno::Reference< chart2::XChartDocument > xChartDoc( GetModel(), uno::UNO_QUERY );
    if( xChartDoc.is() && xChartDoc->hasControllersLocked() )
        xChartDoc->unlockControllers();
}

// create the main context (subcontexts are created
// by the one created here)
SvXMLImportContext *SchXMLImport::CreateContext( sal_uInt16 nPrefix, const OUString& rLocalName,
    const Reference< xml::sax::XAttributeList >& xAttrList )
{
    SvXMLImportContext* pContext = 0;

    // accept <office:document>
    if( XML_NAMESPACE_OFFICE == nPrefix &&
        ( IsXMLToken( rLocalName, XML_DOCUMENT_STYLES) ||
          IsXMLToken( rLocalName, XML_DOCUMENT_CONTENT) ))
    {
        pContext = new SchXMLDocContext( maImportHelper, *this, nPrefix, rLocalName );
    } else if ( (XML_NAMESPACE_OFFICE == nPrefix) &&
                ( IsXMLToken(rLocalName, XML_DOCUMENT) ||
                  (IsXMLToken(rLocalName, XML_DOCUMENT_META)
                   && (getImportFlags() & IMPORT_META) )) )
    {
        uno::Reference<document::XDocumentPropertiesSupplier> xDPS(
            GetModel(), uno::UNO_QUERY);
        // mst@: right now, this seems to be not supported, so it is untested
        if (xDPS.is()) {
            pContext = (IsXMLToken(rLocalName, XML_DOCUMENT_META))
                ? new SvXMLMetaDocumentContext(*this,
                            XML_NAMESPACE_OFFICE, rLocalName,
                            xDPS->getDocumentProperties())
                // flat OpenDocument file format
                : new SchXMLFlatDocContext_Impl(
                            maImportHelper, *this, nPrefix, rLocalName,
                            xDPS->getDocumentProperties());
        } else {
            pContext = (IsXMLToken(rLocalName, XML_DOCUMENT_META))
                ? SvXMLImport::CreateContext( nPrefix, rLocalName, xAttrList )
                : new SchXMLDocContext( maImportHelper, *this,
                                        nPrefix, rLocalName );
        }
    } else {
        pContext = SvXMLImport::CreateContext( nPrefix, rLocalName, xAttrList );
    }

    return pContext;
}

SvXMLImportContext* SchXMLImport::CreateStylesContext(
    const OUString& rLocalName,
    const Reference<xml::sax::XAttributeList>& xAttrList )
{
    //#i103287# make sure that the version information is set before importing all the properties (especially stroke-opacity!)
    SchXMLTools::setBuildIDAtImportInfo( GetModel(), getImportInfo() );

    SvXMLStylesContext* pStylesCtxt =
        new SvXMLStylesContext( *(this), XML_NAMESPACE_OFFICE, rLocalName, xAttrList );

    // set context at base class, so that all auto-style classes are imported
    SetAutoStyles( pStylesCtxt );
    maImportHelper.SetAutoStylesContext( pStylesCtxt );

    return pStylesCtxt;
}

void SAL_CALL SchXMLImport::setTargetDocument( const uno::Reference< lang::XComponent >& xDoc )
    throw(lang::IllegalArgumentException, uno::RuntimeException)
{
    uno::Reference< chart2::XChartDocument > xOldDoc( GetModel(), uno::UNO_QUERY );
    if( xOldDoc.is() && xOldDoc->hasControllersLocked() )
        xOldDoc->unlockControllers();

    SvXMLImport::setTargetDocument( xDoc );

    //set data provider and number formatter
    // try to get an XDataProvider and set it
    // @todo: if we have our own data, we must not use the parent as data provider
    uno::Reference< chart2::XChartDocument > xChartDoc( GetModel(), uno::UNO_QUERY );

    if( xChartDoc.is() )
    try
    {
        //prevent rebuild of view during load ( necesarry especially if loaded not via load api, which is the case for example if binary files are loaded )
        xChartDoc->lockControllers();

        uno::Reference< container::XChild > xChild( xChartDoc, uno::UNO_QUERY );
        uno::Reference< chart2::data::XDataReceiver > xDataReceiver( xChartDoc, uno::UNO_QUERY );
        bool bHasOwnData = true;
        if( xChild.is() && xDataReceiver.is())
        {
            Reference< lang::XMultiServiceFactory > xFact( xChild->getParent(), uno::UNO_QUERY );
            if( xFact.is() )
            {
                //if the parent has a number formatter we will use the numberformatter of the parent
                Reference< util::XNumberFormatsSupplier > xNumberFormatsSupplier( xFact, uno::UNO_QUERY );
                xDataReceiver->attachNumberFormatsSupplier( xNumberFormatsSupplier );

                if ( !xChartDoc->getDataProvider().is() )
                {
                    const OUString aDataProviderServiceName( "com.sun.star.chart2.data.DataProvider");
                    const uno::Sequence< OUString > aServiceNames( xFact->getAvailableServiceNames());
                    const OUString * pBegin = aServiceNames.getConstArray();
                    const OUString * pEnd = pBegin + aServiceNames.getLength();
                    if( ::std::find( pBegin, pEnd, aDataProviderServiceName ) != pEnd )
                    {
                        Reference< chart2::data::XDataProvider > xProvider(
                            xFact->createInstance( aDataProviderServiceName ), uno::UNO_QUERY );
                        if( xProvider.is())
                        {
                            xDataReceiver->attachDataProvider( xProvider );
                            bHasOwnData = false;
                        }
                    }
                }
                else
                    bHasOwnData = false;
            }
//             else we have no parent => we have our own data

            if( bHasOwnData && ! xChartDoc->hasInternalDataProvider() )
                xChartDoc->createInternalDataProvider( sal_False );
        }
    }
    catch( const uno::Exception & rEx )
    {
#ifdef DBG_UTIL
        rtl::OString aBStr(rtl::OUStringToOString(rEx.Message, RTL_TEXTENCODING_ASCII_US));
        OSL_TRACE("SchXMLChartContext::StartElement(): Exception caught: %s", aBStr.getStr());
#else
        (void)rEx; // avoid warning for pro build
#endif
    }
}

// export components ========================================

// first version: everything comes from one storage

Sequence< OUString > SAL_CALL SchXMLImport_getSupportedServiceNames() throw()
{
    const OUString aServiceName(  "com.sun.star.comp.Chart.XMLOasisImporter"  );
    const Sequence< OUString > aSeq( &aServiceName, 1 );
    return aSeq;
}

OUString SAL_CALL SchXMLImport_getImplementationName() throw()
{
    return OUString(  "SchXMLImport"  );
}

Reference< uno::XInterface > SAL_CALL SchXMLImport_createInstance(const Reference< lang::XMultiServiceFactory > & rSMgr) throw( uno::Exception )
{
    // #110680#
    return (cppu::OWeakObject*)new SchXMLImport(rSMgr);
}

// ============================================================

// multiple storage version: one for content / styles / meta

Sequence< OUString > SAL_CALL SchXMLImport_Styles_getSupportedServiceNames() throw()
{
    const OUString aServiceName(  "com.sun.star.comp.Chart.XMLOasisStylesImporter"  );
    const Sequence< OUString > aSeq( &aServiceName, 1 );
    return aSeq;
}

OUString SAL_CALL SchXMLImport_Styles_getImplementationName() throw()
{
    return OUString(  "SchXMLImport.Styles"  );
}

Reference< uno::XInterface > SAL_CALL SchXMLImport_Styles_createInstance(const Reference< lang::XMultiServiceFactory > & rSMgr) throw( uno::Exception )
{
    // #110680#
    return (cppu::OWeakObject*)new SchXMLImport( rSMgr, IMPORT_STYLES );
}

// ------------------------------------------------------------

Sequence< OUString > SAL_CALL SchXMLImport_Content_getSupportedServiceNames() throw()
{
    const OUString aServiceName(  "com.sun.star.comp.Chart.XMLOasisContentImporter"  );
    const Sequence< OUString > aSeq( &aServiceName, 1 );
    return aSeq;
}

OUString SAL_CALL SchXMLImport_Content_getImplementationName() throw()
{
    return OUString(  "SchXMLImport.Content"  );
}

Reference< uno::XInterface > SAL_CALL SchXMLImport_Content_createInstance(const Reference< lang::XMultiServiceFactory > & rSMgr) throw( uno::Exception )
{
    // #110680#
    return (cppu::OWeakObject*)new SchXMLImport( rSMgr, IMPORT_CONTENT | IMPORT_AUTOSTYLES | IMPORT_FONTDECLS );
}

// ------------------------------------------------------------

Sequence< OUString > SAL_CALL SchXMLImport_Meta_getSupportedServiceNames() throw()
{
    const OUString aServiceName(  "com.sun.star.comp.Chart.XMLOasisMetaImporter"  );
    const Sequence< OUString > aSeq( &aServiceName, 1 );
    return aSeq;
}

OUString SAL_CALL SchXMLImport_Meta_getImplementationName() throw()
{
    return OUString(  "SchXMLImport.Meta"  );
}

Reference< uno::XInterface > SAL_CALL SchXMLImport_Meta_createInstance(const Reference< lang::XMultiServiceFactory > & rSMgr) throw( uno::Exception )
{
    // #110680#
    return (cppu::OWeakObject*)new SchXMLImport( rSMgr, IMPORT_META );
}

// XServiceInfo
OUString SAL_CALL SchXMLImport::getImplementationName() throw( uno::RuntimeException )
{
    switch( getImportFlags())
    {
        case IMPORT_ALL:
            return SchXMLImport_getImplementationName();
        case IMPORT_STYLES:
            return SchXMLImport_Styles_getImplementationName();
        case ( IMPORT_CONTENT | IMPORT_AUTOSTYLES | IMPORT_FONTDECLS ):
            return SchXMLImport_Content_getImplementationName();
        case IMPORT_META:
            return SchXMLImport_Meta_getImplementationName();

        case IMPORT_SETTINGS:
        // there is no settings component in chart
        default:
            return OUString( "SchXMLImport" );
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
