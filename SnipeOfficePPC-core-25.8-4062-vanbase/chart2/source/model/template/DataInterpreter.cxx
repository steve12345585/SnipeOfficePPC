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


#include "DataInterpreter.hxx"
#include "DataSeries.hxx"
#include "DataSourceHelper.hxx"
#include "DataSeriesHelper.hxx"
#include "macros.hxx"
#include "CommonConverters.hxx"
#include "ContainerHelper.hxx"
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/chart2/data/XDataSink.hpp>

#include <vector>
#include <algorithm>
#include <iterator>

using namespace ::com::sun::star;
using namespace ::com::sun::star::chart2;
using namespace ::std;
using namespace ::chart::ContainerHelper;

using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Sequence;
using ::rtl::OUString;

#if OSL_DEBUG_LEVEL > 1
namespace
{
void lcl_ShowDataSource( const Reference< data::XDataSource > & xSource );
}
#endif

namespace chart
{

DataInterpreter::DataInterpreter(
    const Reference< uno::XComponentContext > & xContext ) :
        m_xContext( xContext )
{}

DataInterpreter::~DataInterpreter()
{}

Reference< uno::XComponentContext > DataInterpreter::GetComponentContext() const
{
    return m_xContext;
}

// ____ XDataInterpreter ____
InterpretedData SAL_CALL DataInterpreter::interpretDataSource(
    const Reference< data::XDataSource >& xSource,
    const Sequence< beans::PropertyValue >& aArguments,
    const Sequence< Reference< XDataSeries > >& aSeriesToReUse )
    throw (uno::RuntimeException)
{
    if( ! xSource.is())
        return InterpretedData();

#if OSL_DEBUG_LEVEL > 1
    lcl_ShowDataSource( xSource );
#endif

    Sequence< Reference< data::XLabeledDataSequence > > aData( xSource->getDataSequences() );

    Reference< data::XLabeledDataSequence > xCategories;
    vector< Reference< data::XLabeledDataSequence > > aSequencesVec;

    // check if we should use categories

    bool bHasCategories( HasCategories( aArguments, aData ));

    // parse data
    bool bCategoriesUsed = false;
    for( sal_Int32 i=0; i < aData.getLength(); ++i )
    {
        try
        {
            if( bHasCategories && ! bCategoriesUsed )
            {
                xCategories.set( aData[i] );
                if( xCategories.is())
                    SetRole( xCategories->getValues(), C2U("categories"));
                bCategoriesUsed = true;
            }
            else
            {
                aSequencesVec.push_back( aData[i] );
                if( aData[i].is())
                    SetRole( aData[i]->getValues(), C2U("values-y"));
            }
        }
        catch( const uno::Exception & ex )
        {
            ASSERT_EXCEPTION( ex );
        }
    }

    // create DataSeries
    vector< Reference< data::XLabeledDataSequence > >::const_iterator
          aSequencesVecIt = aSequencesVec.begin();

    sal_Int32 nSeriesIndex = 0;
    vector< Reference< XDataSeries > > aSeriesVec;
    aSeriesVec.reserve( aSequencesVec.size());

    for( ;aSequencesVecIt != aSequencesVec.end(); ++aSequencesVecIt, ++nSeriesIndex )
    {
        Sequence< Reference< data::XLabeledDataSequence > > aNewData( & (*aSequencesVecIt), 1 );
        Reference< XDataSeries > xSeries;
        if( nSeriesIndex < aSeriesToReUse.getLength())
            xSeries.set( aSeriesToReUse[nSeriesIndex] );
        else
            xSeries.set( new DataSeries( GetComponentContext() ));
        OSL_ASSERT( xSeries.is() );
        Reference< data::XDataSink > xSink( xSeries, uno::UNO_QUERY );
        OSL_ASSERT( xSink.is() );
        xSink->setData( aNewData );

        aSeriesVec.push_back( xSeries );
    }

    Sequence< Sequence< Reference< XDataSeries > > > aSeries(1);
    aSeries[0] = ContainerToSequence( aSeriesVec );
    return InterpretedData( aSeries, xCategories );
}

InterpretedData SAL_CALL DataInterpreter::reinterpretDataSeries(
    const InterpretedData& aInterpretedData )
    throw (uno::RuntimeException)
{
    InterpretedData aResult( aInterpretedData );

    sal_Int32 i=0;
    Sequence< Reference< XDataSeries > > aSeries( FlattenSequence( aInterpretedData.Series ));
    const sal_Int32 nCount = aSeries.getLength();
    for( ; i<nCount; ++i )
    {
        try
        {
            Reference< data::XDataSource > xSeriesSource( aSeries[i], uno::UNO_QUERY_THROW );
            Sequence< Reference< data::XLabeledDataSequence > > aNewSequences;

            // values-y
            Reference< data::XLabeledDataSequence > xValuesY(
                DataSeriesHelper::getDataSequenceByRole( xSeriesSource, C2U("values-y"), false ));
            // re-use values-... as values-y
            if( ! xValuesY.is())
            {
                xValuesY.set(
                    DataSeriesHelper::getDataSequenceByRole( xSeriesSource, C2U("values"), true ));
                if( xValuesY.is())
                    SetRole( xValuesY->getValues(), C2U("values-y"));
            }
            if( xValuesY.is())
            {
                aNewSequences.realloc(1);
                aNewSequences[0] = xValuesY;
            }

            Sequence< Reference< data::XLabeledDataSequence > > aSeqs( xSeriesSource->getDataSequences());
            if( aSeqs.getLength() != aNewSequences.getLength() )
            {
#if OSL_DEBUG_LEVEL > 1
                sal_Int32 j=0;
                for( ; j<aSeqs.getLength(); ++j )
                {
                    OSL_ENSURE( aSeqs[j] == xValuesY, "All sequences should be used" );
                }
#endif
                Reference< data::XDataSink > xSink( xSeriesSource, uno::UNO_QUERY_THROW );
                xSink->setData( aNewSequences );
            }
        }
        catch( const uno::Exception & ex )
        {
            ASSERT_EXCEPTION( ex );
        }
    }

    return aResult;
}

// criterion: all series must have exactly one data::XLabeledDataSequence
sal_Bool SAL_CALL DataInterpreter::isDataCompatible(
    const chart2::InterpretedData& aInterpretedData )
    throw (uno::RuntimeException)
{
    Sequence< Reference< XDataSeries > > aSeries( FlattenSequence( aInterpretedData.Series ));
    for( sal_Int32 i=0; i<aSeries.getLength(); ++i )
    {
        try
        {
            Reference< data::XDataSource > xSrc( aSeries[i], uno::UNO_QUERY_THROW );
            Sequence< Reference< data::XLabeledDataSequence > > aSeq( xSrc->getDataSequences());
            if( aSeq.getLength() != 1 )
                return sal_False;
        }
        catch( const uno::Exception & ex )
        {
            ASSERT_EXCEPTION( ex );
        }
    }

    return sal_True;
}

namespace
{

struct lcl_LabeledSequenceEquals : public std::unary_function< Reference< data::XLabeledDataSequence >, bool >
{
    lcl_LabeledSequenceEquals( const Reference< data::XLabeledDataSequence > & xLSeqToCmp ) :
            m_bHasLabels ( false ),
            m_bHasValues ( false )
    {
        if( xLSeqToCmp.is())
        {
            Reference< data::XDataSequence > xSeq( xLSeqToCmp->getValues());
            if( xSeq.is())
            {
                m_bHasValues = true;
                m_aValuesRangeRep = xSeq->getSourceRangeRepresentation();
            }

            xSeq.set( xLSeqToCmp->getLabel());
            if( xSeq.is())
            {
                m_bHasLabels = true;
                m_aLabelRangeRep = xSeq->getSourceRangeRepresentation();
            }
        }
    }

    bool operator() ( const Reference< data::XLabeledDataSequence > & xSeq )
    {
        if( ! xSeq.is())
            return false;

        Reference< data::XDataSequence > xSeqValues( xSeq->getValues() );
        Reference< data::XDataSequence > xSeqLabels( xSeq->getLabel() );
        bool bHasValues = xSeqValues.is();
        bool bHasLabels = xSeqLabels.is();

        return ( ( (m_bHasValues == bHasValues) &&
                   (!bHasValues || m_aValuesRangeRep.equals( xSeqValues->getSourceRangeRepresentation())) ) &&
                 ( (m_bHasLabels == bHasLabels) &&
                   (!bHasLabels || m_aLabelRangeRep.equals( xSeqLabels->getSourceRangeRepresentation())) )
            );
    }

private:
    bool m_bHasLabels;
    bool m_bHasValues;
    OUString m_aValuesRangeRep;
    OUString m_aLabelRangeRep;
};

} // anonymous namespace

Reference< data::XDataSource > SAL_CALL DataInterpreter::mergeInterpretedData(
    const InterpretedData& aInterpretedData )
    throw (uno::RuntimeException)
{
    vector< Reference< data::XLabeledDataSequence > > aResultVec;
    aResultVec.reserve( aInterpretedData.Series.getLength() +
                        1 // categories
        );

    if( aInterpretedData.Categories.is())
        aResultVec.push_back( aInterpretedData.Categories );

    Sequence< Reference< XDataSeries > > aSeries( FlattenSequence( aInterpretedData.Series ));
    for( sal_Int32 nSeriesIdx=0; nSeriesIdx<aSeries.getLength(); ++nSeriesIdx )
    {
        try
        {
            Reference< data::XDataSource > xSrc( aSeries[nSeriesIdx], uno::UNO_QUERY_THROW );
            Sequence< Reference< data::XLabeledDataSequence > > aSeq( xSrc->getDataSequences());

            // add all sequences of data series
            for( sal_Int32 nSeqIdx=0; nSeqIdx<aSeq.getLength(); ++nSeqIdx )
            {
                Reference< data::XLabeledDataSequence > xAdd( aSeq[nSeqIdx] );

                // only add if sequence is not yet in the result
                if( find_if( aResultVec.begin(), aResultVec.end(),
                             lcl_LabeledSequenceEquals( xAdd )) == aResultVec.end())
                {
                    aResultVec.push_back( xAdd );
                }
            }
        }
        catch( const uno::Exception & ex )
        {
            ASSERT_EXCEPTION( ex );
        }
    }

    return Reference< data::XDataSource >( DataSourceHelper::createDataSource( ContainerToSequence( aResultVec ) ) );
}

// convenience methods

OUString DataInterpreter::GetRole( const Reference< data::XDataSequence > & xSeq )
{
    OUString aResult;
    if( ! xSeq.is())
        return aResult;

    try
    {
        Reference< beans::XPropertySet > xProp( xSeq, uno::UNO_QUERY_THROW );
        xProp->getPropertyValue( C2U("Role")) >>= aResult;
    }
    catch( const uno::Exception & ex )
    {
        ASSERT_EXCEPTION( ex );
    }
    return aResult;
}

void DataInterpreter::SetRole( const Reference< data::XDataSequence > & xSeq, const OUString & rRole )
{
    if( ! xSeq.is())
        return;
    try
    {
        Reference< beans::XPropertySet > xProp( xSeq, uno::UNO_QUERY_THROW );
        xProp->setPropertyValue( C2U("Role"), uno::makeAny( rRole ));
    }
    catch( const uno::Exception & ex )
    {
        ASSERT_EXCEPTION( ex );
    }
}

uno::Any DataInterpreter::GetProperty(
    const Sequence< beans::PropertyValue > & aArguments,
    const OUString & rName )
{
    for( sal_Int32 i=aArguments.getLength(); i--; )
    {
        if( aArguments[i].Name.equals( rName ))
            return aArguments[i].Value;
    }
    return uno::Any();
}

bool DataInterpreter::HasCategories(
    const Sequence< beans::PropertyValue > & rArguments,
    const Sequence< Reference< data::XLabeledDataSequence > > & rData )
{
    bool bHasCategories = false;

    if( rArguments.getLength() > 0 )
        GetProperty( rArguments, C2U(("HasCategories"))) >>= bHasCategories;

    for( sal_Int32 nLSeqIdx=0; ! bHasCategories && nLSeqIdx<rData.getLength(); ++nLSeqIdx )
        bHasCategories = ( rData[nLSeqIdx].is() && GetRole( rData[nLSeqIdx]->getValues() ) == "categories");

    return bHasCategories;
}

bool DataInterpreter::UseCategoriesAsX( const Sequence< beans::PropertyValue > & rArguments )
{
    bool bUseCategoriesAsX = true;
    if( rArguments.getLength() > 0 )
        GetProperty( rArguments, C2U(("UseCategoriesAsX"))) >>= bUseCategoriesAsX;
    return bUseCategoriesAsX;
}

// ------------------------------------------------------------

Sequence< OUString > DataInterpreter::getSupportedServiceNames_Static()
{
    Sequence< OUString > aServices( 1 );
    aServices[0] = C2U( "com.sun.star.chart2.DataInterpreter" );
    return aServices;
}

// implement XServiceInfo methods basing upon getSupportedServiceNames_Static
APPHELPER_XSERVICEINFO_IMPL( DataInterpreter, C2U("com.sun.star.comp.chart2.DataInterpreter"));

} // namespace chart

#if OSL_DEBUG_LEVEL > 1
namespace
{

void lcl_ShowDataSource( const Reference< data::XDataSource > & xSource )
{
    if( ! xSource.is())
        return;

    OSL_TRACE( "DataSource in DataInterpreter:" );
    Sequence< Reference< data::XLabeledDataSequence > > aSequences( xSource->getDataSequences());
    Reference< beans::XPropertySet > xProp;
    OUString aId;
    const sal_Int32 nMax = aSequences.getLength();
    for( sal_Int32 k = 0; k < nMax; ++k )
    {
        if( aSequences[k].is())
        {
            OUString aSourceRepr(C2U("<none>"));
            if( aSequences[k]->getValues().is())
                aSourceRepr = aSequences[k]->getValues()->getSourceRangeRepresentation();
            xProp.set( aSequences[k]->getValues(), uno::UNO_QUERY );
            if( xProp.is() &&
                ( xProp->getPropertyValue( C2U( "Role" )) >>= aId ))
            {
                OSL_TRACE( "  <data sequence %d> Role: %s, Source: %s", k, U2C( aId ), U2C( aSourceRepr ));
            }
            else
            {
                OSL_TRACE( "  <data sequence %d> unknown Role, Source: %s", k, U2C( aSourceRepr ) );
            }

            aSourceRepr = C2U("<none>");
            if( aSequences[k]->getLabel().is())
                aSourceRepr = OUString( aSequences[k]->getLabel()->getSourceRangeRepresentation());
            xProp.set( aSequences[k]->getLabel(), uno::UNO_QUERY );
            if( xProp.is() &&
                ( xProp->getPropertyValue( C2U( "Role" )) >>= aId ))
            {
                OSL_TRACE( "  <data sequence label %d> Role: %s, Source: %s", k, U2C( aId ), U2C( aSourceRepr ));
            }
            else
            {
                OSL_TRACE( "  <data sequence label %d> unknown Role, Source: %s", k, U2C( aSourceRepr ) );
            }
        }
    }
}

}
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
