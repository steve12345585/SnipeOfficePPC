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

#include "formnavigation.hxx"
#include "urltransformer.hxx"
#include "controlfeatureinterception.hxx"
#include "frm_strings.hxx"

#include <com/sun/star/form/runtime/FormFeature.hpp>

#include <tools/debug.hxx>


//.........................................................................
namespace frm
{
//.........................................................................

    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::beans;
    using namespace ::com::sun::star::lang;
    using namespace ::com::sun::star::util;
    using namespace ::com::sun::star::frame;
    namespace FormFeature = ::com::sun::star::form::runtime::FormFeature;

    //==================================================================
    //= OFormNavigationHelper
    //==================================================================
    DBG_NAME( OFormNavigationHelper )
    //------------------------------------------------------------------
    OFormNavigationHelper::OFormNavigationHelper( const Reference< XMultiServiceFactory >& _rxORB )
        :m_xORB( _rxORB )
        ,m_nConnectedFeatures( 0 )
    {
        DBG_CTOR( OFormNavigationHelper, NULL );
        m_pFeatureInterception.reset( new ControlFeatureInterception( m_xORB ) );
    }

    //------------------------------------------------------------------
    OFormNavigationHelper::~OFormNavigationHelper()
    {
        DBG_DTOR( OFormNavigationHelper, NULL );
    }

    //------------------------------------------------------------------
    void SAL_CALL OFormNavigationHelper::dispose( ) throw( RuntimeException )
    {
        m_pFeatureInterception->dispose();
        disconnectDispatchers();
    }

    //------------------------------------------------------------------
    void OFormNavigationHelper::interceptorsChanged( )
    {
        updateDispatches();
    }

    //------------------------------------------------------------------
    void OFormNavigationHelper::featureStateChanged( sal_Int16 /*_nFeatureId*/, sal_Bool /*_bEnabled*/ )
    {
        // not interested in
    }

    //------------------------------------------------------------------
    void OFormNavigationHelper::allFeatureStatesChanged( )
    {
        // not interested in
    }

    //------------------------------------------------------------------
    void SAL_CALL OFormNavigationHelper::registerDispatchProviderInterceptor( const Reference< XDispatchProviderInterceptor >& _rxInterceptor ) throw (RuntimeException)
    {
        m_pFeatureInterception->registerDispatchProviderInterceptor( _rxInterceptor );
        interceptorsChanged();
    }

    //------------------------------------------------------------------
    void SAL_CALL OFormNavigationHelper::releaseDispatchProviderInterceptor( const Reference< XDispatchProviderInterceptor >& _rxInterceptor ) throw (RuntimeException)
    {
        m_pFeatureInterception->releaseDispatchProviderInterceptor( _rxInterceptor );
        interceptorsChanged();
    }

    //------------------------------------------------------------------
    void SAL_CALL OFormNavigationHelper::statusChanged( const FeatureStateEvent& _rState ) throw (RuntimeException)
    {
        for (   FeatureMap::iterator aFeature = m_aSupportedFeatures.begin();
                aFeature != m_aSupportedFeatures.end();
                ++aFeature
            )
        {
            if ( aFeature->second.aURL.Main == _rState.FeatureURL.Main )
            {
                if  (  ( aFeature->second.bCachedState != _rState.IsEnabled )
                    || ( aFeature->second.aCachedAdditionalState != _rState.State )
                    )
                {
                    // change the cached state
                    aFeature->second.bCachedState           = _rState.IsEnabled;
                    aFeature->second.aCachedAdditionalState = _rState.State;
                    // tell derivees what happened
                    featureStateChanged( aFeature->first, _rState.IsEnabled );
                }
                return;
            }
        }

        // unreachable
        OSL_FAIL( "OFormNavigationHelper::statusChanged: huh? An invalid/unknown URL?" );
    }

    //------------------------------------------------------------------
    void SAL_CALL OFormNavigationHelper::disposing( const EventObject& _rSource ) throw (RuntimeException)
    {
        // was it one of our external dispatchers?
        if ( m_nConnectedFeatures )
        {
            for (   FeatureMap::iterator aFeature = m_aSupportedFeatures.begin();
                    aFeature != m_aSupportedFeatures.end();
                    ++aFeature
                )
            {
                if ( aFeature->second.xDispatcher == _rSource.Source )
                {
                    aFeature->second.xDispatcher->removeStatusListener( static_cast< XStatusListener* >( this ), aFeature->second.aURL );
                    aFeature->second.xDispatcher = NULL;
                    aFeature->second.bCachedState = sal_False;
                    aFeature->second.aCachedAdditionalState.clear();
                    --m_nConnectedFeatures;

                    featureStateChanged( aFeature->first, sal_False );
                    break;
                }
            }
        }
    }

    //------------------------------------------------------------------
    void OFormNavigationHelper::updateDispatches()
    {
        if ( !m_nConnectedFeatures )
        {   // we don't have any dispatchers yet -> do the initial connect
            connectDispatchers();
            return;
        }

        initializeSupportedFeatures();

        m_nConnectedFeatures = 0;

        Reference< XDispatch >  xNewDispatcher;
        Reference< XDispatch >  xCurrentDispatcher;

        for (   FeatureMap::iterator aFeature = m_aSupportedFeatures.begin();
                aFeature != m_aSupportedFeatures.end();
                ++aFeature
            )
        {
            xNewDispatcher = queryDispatch( aFeature->second.aURL );
            xCurrentDispatcher = aFeature->second.xDispatcher;
            if ( xNewDispatcher != xCurrentDispatcher )
            {
                // the dispatcher for this particular URL changed
                if ( xCurrentDispatcher.is() )
                    xCurrentDispatcher->removeStatusListener( static_cast< XStatusListener* >( this ), aFeature->second.aURL );

                xCurrentDispatcher = aFeature->second.xDispatcher = xNewDispatcher;

                if ( xCurrentDispatcher.is() )
                    xCurrentDispatcher->addStatusListener( static_cast< XStatusListener* >( this ), aFeature->second.aURL );
            }

            if ( xCurrentDispatcher.is() )
                ++m_nConnectedFeatures;
            else
                aFeature->second.bCachedState = sal_False;
        }

        // notify derivee that (potentially) all features changed their state
        allFeatureStatesChanged( );
    }

    //------------------------------------------------------------------
    void OFormNavigationHelper::connectDispatchers()
    {
        if ( m_nConnectedFeatures )
        {   // already connected -> just do an update
            updateDispatches();
            return;
        }

        initializeSupportedFeatures();

        m_nConnectedFeatures = 0;

        for (   FeatureMap::iterator aFeature = m_aSupportedFeatures.begin();
                aFeature != m_aSupportedFeatures.end();
                ++aFeature
            )
        {
            aFeature->second.bCachedState = sal_False;
            aFeature->second.aCachedAdditionalState.clear();
            aFeature->second.xDispatcher = queryDispatch( aFeature->second.aURL );
            if ( aFeature->second.xDispatcher.is() )
            {
                ++m_nConnectedFeatures;
                aFeature->second.xDispatcher->addStatusListener( static_cast< XStatusListener* >( this ), aFeature->second.aURL );
            }
        }

        // notify derivee that (potentially) all features changed their state
        allFeatureStatesChanged( );
    }

    //------------------------------------------------------------------
    void OFormNavigationHelper::disconnectDispatchers()
    {
        if ( m_nConnectedFeatures )
        {
            for (   FeatureMap::iterator aFeature = m_aSupportedFeatures.begin();
                    aFeature != m_aSupportedFeatures.end();
                    ++aFeature
                )
            {
                if ( aFeature->second.xDispatcher.is() )
                    aFeature->second.xDispatcher->removeStatusListener( static_cast< XStatusListener* >( this ), aFeature->second.aURL );

                aFeature->second.xDispatcher = NULL;
                aFeature->second.bCachedState = sal_False;
                aFeature->second.aCachedAdditionalState.clear();
            }

            m_nConnectedFeatures = 0;
        }

        // notify derivee that (potentially) all features changed their state
        allFeatureStatesChanged( );
    }

    //------------------------------------------------------------------
    void OFormNavigationHelper::initializeSupportedFeatures( )
    {
        if ( m_aSupportedFeatures.empty() )
        {
            // ask the derivee which feature ids it wants us to support
            ::std::vector< sal_Int16 > aFeatureIds;
            getSupportedFeatures( aFeatureIds );

            OFormNavigationMapper aUrlMapper( m_xORB );

            for (   ::std::vector< sal_Int16 >::const_iterator aLoop = aFeatureIds.begin();
                    aLoop != aFeatureIds.end();
                    ++aLoop
                )
            {
                FeatureInfo aFeatureInfo;

                bool bKnownId =
                    aUrlMapper.getFeatureURL( *aLoop, aFeatureInfo.aURL );
                DBG_ASSERT( bKnownId, "OFormNavigationHelper::initializeSupportedFeatures: unknown feature id!" );

                if ( bKnownId )
                    // add to our map
                    m_aSupportedFeatures.insert( FeatureMap::value_type( *aLoop, aFeatureInfo ) );
            }
        }
    }

    //------------------------------------------------------------------
    Reference< XDispatch > OFormNavigationHelper::queryDispatch( const URL& _rURL )
    {
        return m_pFeatureInterception->queryDispatch( _rURL );
    }

    //------------------------------------------------------------------
    void OFormNavigationHelper::dispatchWithArgument( sal_Int16 _nFeatureId, const sal_Char* _pParamAsciiName,
        const Any& _rParamValue ) const
    {
        FeatureMap::const_iterator aInfo = m_aSupportedFeatures.find( _nFeatureId );
        if ( m_aSupportedFeatures.end() != aInfo )
        {
            if ( aInfo->second.xDispatcher.is() )
            {
                Sequence< PropertyValue > aArgs( 1 );
                aArgs[0].Name = ::rtl::OUString::createFromAscii( _pParamAsciiName );
                aArgs[0].Value = _rParamValue;

                aInfo->second.xDispatcher->dispatch( aInfo->second.aURL, aArgs );
            }
        }
    }

    //------------------------------------------------------------------
    void OFormNavigationHelper::dispatch( sal_Int16 _nFeatureId ) const
    {
        FeatureMap::const_iterator aInfo = m_aSupportedFeatures.find( _nFeatureId );
        if ( m_aSupportedFeatures.end() != aInfo )
        {
            if ( aInfo->second.xDispatcher.is() )
            {
                Sequence< PropertyValue > aEmptyArgs;
                aInfo->second.xDispatcher->dispatch( aInfo->second.aURL, aEmptyArgs );
            }
        }
    }

    //------------------------------------------------------------------
    bool OFormNavigationHelper::isEnabled( sal_Int16 _nFeatureId ) const
    {
        FeatureMap::const_iterator aInfo = m_aSupportedFeatures.find( _nFeatureId );
        if ( m_aSupportedFeatures.end() != aInfo )
            return aInfo->second.bCachedState;

        return false;
    }

    //------------------------------------------------------------------
    bool OFormNavigationHelper::getBooleanState( sal_Int16 _nFeatureId ) const
    {
        sal_Bool bState = sal_False;

        FeatureMap::const_iterator aInfo = m_aSupportedFeatures.find( _nFeatureId );
        if ( m_aSupportedFeatures.end() != aInfo )
            aInfo->second.aCachedAdditionalState >>= bState;

        return (bool)bState;
    }

    //------------------------------------------------------------------
    ::rtl::OUString OFormNavigationHelper::getStringState( sal_Int16 _nFeatureId ) const
    {
        ::rtl::OUString sState;

        FeatureMap::const_iterator aInfo = m_aSupportedFeatures.find( _nFeatureId );
        if ( m_aSupportedFeatures.end() != aInfo )
            aInfo->second.aCachedAdditionalState >>= sState;

        return sState;
    }

    //------------------------------------------------------------------
    sal_Int32 OFormNavigationHelper::getIntegerState( sal_Int16 _nFeatureId ) const
    {
        sal_Int32 nState = 0;

        FeatureMap::const_iterator aInfo = m_aSupportedFeatures.find( _nFeatureId );
        if ( m_aSupportedFeatures.end() != aInfo )
            aInfo->second.aCachedAdditionalState >>= nState;

        return nState;
    }

    //------------------------------------------------------------------
    void OFormNavigationHelper::invalidateSupportedFeaturesSet()
    {
        disconnectDispatchers( );
        // no supported features anymore:
        FeatureMap aEmpty;
        m_aSupportedFeatures.swap( aEmpty );
    }

    //==================================================================
    //= OFormNavigationMapper
    //==================================================================
    //------------------------------------------------------------------
    OFormNavigationMapper::OFormNavigationMapper( const Reference< XMultiServiceFactory >& _rxORB )
    {
        m_pUrlTransformer.reset( new UrlTransformer( _rxORB ) );
    }

    //------------------------------------------------------------------
    OFormNavigationMapper::~OFormNavigationMapper( )
    {
    }

    //------------------------------------------------------------------
    bool OFormNavigationMapper::getFeatureURL( sal_Int16 _nFeatureId, URL& /* [out] */ _rURL )
    {
        // get the ascii version of the URL
        const char* pAsciiURL = getFeatureURLAscii( _nFeatureId );
        if ( pAsciiURL )
            _rURL = m_pUrlTransformer->getStrictURLFromAscii( pAsciiURL );

        return ( pAsciiURL != NULL );
    }

    //------------------------------------------------------------------
    namespace
    {
        struct FeatureURL
        {
            const sal_Int16 nFormFeature;
            const sal_Char* pAsciiURL;

            FeatureURL( const sal_Int16 _nFormFeature, const sal_Char* _pAsciiURL )
                :nFormFeature( _nFormFeature )
                ,pAsciiURL( _pAsciiURL )
            {
            }
        };
        const FeatureURL* lcl_getFeatureTable()
        {
            static const FeatureURL s_aFeatureURLs[] =
            {
                FeatureURL( FormFeature::MoveAbsolute,            URL_FORM_POSITION.ascii ),
                FeatureURL( FormFeature::TotalRecords,            URL_FORM_RECORDCOUNT.ascii ),
                FeatureURL( FormFeature::MoveToFirst,             URL_RECORD_FIRST.ascii ),
                FeatureURL( FormFeature::MoveToPrevious,          URL_RECORD_PREV.ascii ),
                FeatureURL( FormFeature::MoveToNext,              URL_RECORD_NEXT.ascii ),
                FeatureURL( FormFeature::MoveToLast,              URL_RECORD_LAST.ascii ),
                FeatureURL( FormFeature::SaveRecordChanges,       URL_RECORD_SAVE.ascii ),
                FeatureURL( FormFeature::UndoRecordChanges,       URL_RECORD_UNDO.ascii ),
                FeatureURL( FormFeature::MoveToInsertRow,         URL_RECORD_NEW.ascii ),
                FeatureURL( FormFeature::DeleteRecord,            URL_RECORD_DELETE.ascii ),
                FeatureURL( FormFeature::ReloadForm,              URL_FORM_REFRESH.ascii ),
                FeatureURL( FormFeature::RefreshCurrentControl,   URL_FORM_REFRESH_CURRENT_CONTROL.ascii ),
                FeatureURL( FormFeature::SortAscending,           URL_FORM_SORT_UP.ascii ),
                FeatureURL( FormFeature::SortDescending,          URL_FORM_SORT_DOWN.ascii ),
                FeatureURL( FormFeature::InteractiveSort,         URL_FORM_SORT.ascii ),
                FeatureURL( FormFeature::AutoFilter,              URL_FORM_AUTO_FILTER.ascii ),
                FeatureURL( FormFeature::InteractiveFilter,       URL_FORM_FILTER.ascii ),
                FeatureURL( FormFeature::ToggleApplyFilter,       URL_FORM_APPLY_FILTER.ascii ),
                FeatureURL( FormFeature::RemoveFilterAndSort,     URL_FORM_REMOVE_FILTER.ascii ),
                FeatureURL( 0, NULL )
            };
            return s_aFeatureURLs;
        }
    }

    //------------------------------------------------------------------
    const char* OFormNavigationMapper::getFeatureURLAscii( sal_Int16 _nFeatureId )
    {
        const FeatureURL* pFeatures = lcl_getFeatureTable();
        while ( pFeatures->pAsciiURL )
        {
            if ( pFeatures->nFormFeature == _nFeatureId )
                return pFeatures->pAsciiURL;
            ++pFeatures;
        }
        return NULL;
    }

    //------------------------------------------------------------------
    sal_Int16 OFormNavigationMapper::getFeatureId( const ::rtl::OUString& _rCompleteURL )
    {
        const FeatureURL* pFeatures = lcl_getFeatureTable();
        while ( pFeatures->pAsciiURL )
        {
            if ( _rCompleteURL.compareToAscii( pFeatures->pAsciiURL ) == 0 )
                return pFeatures->nFormFeature;
            ++pFeatures;
        }
        return -1;
    }

//.........................................................................
}   // namespace frm
//.........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
