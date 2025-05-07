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

#ifndef FORMS_FORM_NAVIGATION_HXX
#define FORMS_FORM_NAVIGATION_HXX

#include <com/sun/star/frame/XDispatchProviderInterception.hpp>
#include <com/sun/star/frame/XStatusListener.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/util/XURLTransformer.hpp>
#include <cppuhelper/implbase2.hxx>
#include "featuredispatcher.hxx"
#include <vector>
#include <map>
#include <memory>


//.........................................................................
namespace frm
{
//.........................................................................

    class UrlTransformer;
    class ControlFeatureInterception;

    //==================================================================
    //= OFormNavigationHelper
    //==================================================================
    typedef ::cppu::ImplHelper2 <   ::com::sun::star::frame::XDispatchProviderInterception
                                ,   ::com::sun::star::frame::XStatusListener
                                >   OFormNavigationHelper_Base;

    class OFormNavigationHelper
                        :public OFormNavigationHelper_Base
                        ,public IFeatureDispatcher
    {
    private:
        struct FeatureInfo
        {
            ::com::sun::star::util::URL                                             aURL;
            ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch >  xDispatcher;
            sal_Bool                                                                bCachedState;
            ::com::sun::star::uno::Any                                              aCachedAdditionalState;

            FeatureInfo() : bCachedState( sal_False ) { }
        };
        typedef ::std::map< sal_Int16, FeatureInfo >    FeatureMap;

    private:
        ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >
                            m_xORB;
        SAL_WNODEPRECATED_DECLARATIONS_PUSH
        ::std::auto_ptr< ControlFeatureInterception >
                            m_pFeatureInterception;
        SAL_WNODEPRECATED_DECLARATIONS_POP

        // all supported features
        FeatureMap          m_aSupportedFeatures;
        // all features which we have an external dispatcher for
        sal_Int32           m_nConnectedFeatures;

    protected:
        inline const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >&
            getORB( ) const { return m_xORB; }

    protected:
        OFormNavigationHelper( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxORB );
        virtual ~OFormNavigationHelper();

        // XComponent
        void SAL_CALL dispose(  ) throw( ::com::sun::star::uno::RuntimeException );

        // XDispatchProviderInterception
        virtual void SAL_CALL registerDispatchProviderInterceptor( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchProviderInterceptor >& Interceptor ) throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL releaseDispatchProviderInterceptor( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchProviderInterceptor >& Interceptor ) throw (::com::sun::star::uno::RuntimeException);

        // XStatusListener
        virtual void SAL_CALL statusChanged( const ::com::sun::star::frame::FeatureStateEvent& State ) throw (::com::sun::star::uno::RuntimeException);

        // XEventListener
        virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& Source ) throw (::com::sun::star::uno::RuntimeException);

        // IFeatureDispatcher
        virtual void            dispatch( sal_Int16 _nFeatureId ) const;
        virtual void            dispatchWithArgument( sal_Int16 _nFeatureId, const sal_Char* _pParamName, const ::com::sun::star::uno::Any& _rParamValue ) const;
        virtual bool            isEnabled( sal_Int16 _nFeatureId ) const;
        virtual bool            getBooleanState( sal_Int16 _nFeatureId ) const;
        virtual ::rtl::OUString getStringState( sal_Int16 _nFeatureId ) const;
        virtual sal_Int32       getIntegerState( sal_Int16 _nFeatureId ) const;

        // own overridables
        /** is called when the interceptors have.
            <p>The default implementations simply calls <member>updateDispatches</member>,
            derived classes can prevent this in certain cases, or do additional handling.</p>
        */
        virtual void    interceptorsChanged( );

        /** called when the status of a feature changed

            <p>The default implementation does nothing.</p>

            <p>If the feature in question does support more state information that just the
            enabled/disabled state, then this additional information is to be retrieved in
            a separate call.</p>

            @param _nFeatureId
                the id of the feature
            @param _bEnabled
                determines if the features is enabled or disabled
            @see getBooleanState
        */
        virtual void    featureStateChanged( sal_Int16 _nFeatureId, sal_Bool _bEnabled );

        /** notification for (potential) changes in the state of all features
            <p>The base class implementation does nothing. Derived classes could force
            their peer to update it's state, depending on the result of calls to
            <member>IFeatureDispatcher::isEnabled</member>.</p>
        */
        virtual void    allFeatureStatesChanged( );

        /** retrieves the list of supported features
            <p>To be overridden by derived classes</p>
            @param _rFeatureIds
                the array of features to support. Out parameter to fill by the derivee's implementation
            @pure
        */
        virtual void    getSupportedFeatures( ::std::vector< sal_Int16 >& /* [out] */ _rFeatureIds ) = 0;

    protected:
        /** update all our dispatches which are controlled by our dispatch interceptors
        */
        void    updateDispatches();

        /** connect to the dispatch interceptors
        */
        void    connectDispatchers();

        /** disconnect from the dispatch interceptors
        */
        void    disconnectDispatchers();

        /** queries the interceptor chain for a dispatcher for the given URL
        */
        ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch >
                queryDispatch( const ::com::sun::star::util::URL& _rURL );

        /** invalidates the set of supported features

            <p>This will invalidate all structures which are tied to the set of supported
            features. All dispatches will be disconnected.<br/>
            No automatic re-connection to potential external dispatchers is done, instead,
            you have to call updateDispatches explicitly, if necessary.</p>
        */
        void    invalidateSupportedFeaturesSet();

    private:
        /** initialize m_aSupportedFeatures, if necessary
        */
        void    initializeSupportedFeatures();
    };

    //==================================================================
    //= OFormNavigationMapper
    //==================================================================
    /** helper class mapping between feature ids and feature URLs
    */
    class OFormNavigationMapper
    {
    private:
        SAL_WNODEPRECATED_DECLARATIONS_PUSH
        ::std::auto_ptr< UrlTransformer >   m_pUrlTransformer;
        SAL_WNODEPRECATED_DECLARATIONS_POP

    public:
        OFormNavigationMapper(
            const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxORB
        );
        ~OFormNavigationMapper( );

        /** retrieves the ASCII representation of a feature URL belonging to an id

            @complexity O(log n)
            @return NULL if the given id is not a known feature id (which is a valid usage)
        */
        const char* getFeatureURLAscii( sal_Int16 _nFeatureId );

        /** retrieves the feature URL belonging to an feature id

            @complexity O(log n), with n being the number of all potentially known URLs
            @return
                <TRUE/> if and only if the given id is a known feature id
                (which is a valid usage)
        */
        bool        getFeatureURL( sal_Int16 _nFeatureId, ::com::sun::star::util::URL& /* [out] */ _rURL );

        /** retrieves the feature id belonging to an feature URL

            @complexity O(n), with n being the number of all potentially known URLs
            @return
                the id of the feature URL, or -1 if the URl is not known
                (which is a valid usage)
        */
        sal_Int16   getFeatureId( const ::rtl::OUString& _rCompleteURL );

    private:
        OFormNavigationMapper( );                                           // never implemented
        OFormNavigationMapper( const OFormNavigationMapper& );              // never implemented
        OFormNavigationMapper& operator=( const OFormNavigationMapper& );   // never implemented
    };

//.........................................................................
}   // namespace frm
//.........................................................................

#endif // FORMS_FORM_NAVIGATION_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
