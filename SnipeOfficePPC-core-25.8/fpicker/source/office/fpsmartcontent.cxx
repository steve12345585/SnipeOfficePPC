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

#include "fpsmartcontent.hxx"

/** === begin UNO includes === **/
#include <com/sun/star/container/XChild.hpp>
#include <com/sun/star/ucb/ContentInfo.hpp>
#include <com/sun/star/ucb/ContentInfoAttribute.hpp>
#include <com/sun/star/ucb/XContent.hpp>
/** === end UNO includes === **/

#include <comphelper/processfactory.hxx>
#include <ucbhelper/commandenvironment.hxx>
#include <tools/solar.h>
#include <tools/debug.hxx>

//........................................................................
namespace svt
{
//........................................................................

    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::task;
    using namespace ::com::sun::star::ucb;
    using namespace ::com::sun::star::lang;
    using namespace ::com::sun::star::container;

    //====================================================================
    //= SmartContent
    //====================================================================
    //--------------------------------------------------------------------
    SmartContent::SmartContent()
        :m_pContent( NULL )
        ,m_eState( NOT_BOUND )
        ,m_pOwnInteraction( NULL )
    {
    }

    //--------------------------------------------------------------------
    SmartContent::SmartContent( const ::rtl::OUString& _rInitialURL )
        :m_pContent( NULL )
        ,m_eState( NOT_BOUND )
    {
        bindTo( _rInitialURL );
    }

    //--------------------------------------------------------------------
    SmartContent::~SmartContent()
    {
        /* This destructor originally contained the following blurb: "Do
           not delete the content. Because the content will be used by
           the cache." This is just plain silly, because it relies on
           the provider caching created contents (which is done by
           ucbhelper::ContentProviderImplHelper, but we do not actually
           expect all providers to use that, right?) Otherwise we are
           just leaking memory.

           TODO: If there is real need for caching the content, it must
           be done here.
        */
        delete m_pContent;
    }

    //--------------------------------------------------------------------
    void SmartContent::enableOwnInteractionHandler(::svt::OFilePickerInteractionHandler::EInterceptedInteractions eInterceptions)
    {
        Reference< XMultiServiceFactory > xFactory = ::comphelper::getProcessServiceFactory();
        Reference< XInteractionHandler >  xGlobalInteractionHandler = Reference< XInteractionHandler >(
            xFactory->createInstance( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.task.InteractionHandler") ) ), UNO_QUERY );

        m_pOwnInteraction = new ::svt::OFilePickerInteractionHandler(xGlobalInteractionHandler);
        m_pOwnInteraction->enableInterceptions(eInterceptions);
        m_xOwnInteraction = m_pOwnInteraction;

        m_xCmdEnv = new ::ucbhelper::CommandEnvironment( m_xOwnInteraction, Reference< XProgressHandler >() );
    }

    //--------------------------------------------------------------------
    void SmartContent::enableDefaultInteractionHandler()
    {
        // Don't free the memory here! It will be done by the next
        // call automaticly - releasing of the uno reference ...
        m_pOwnInteraction = NULL;
        m_xOwnInteraction = Reference< XInteractionHandler >();

        Reference< XMultiServiceFactory > xFactory = ::comphelper::getProcessServiceFactory();
        Reference< XInteractionHandler >  xGlobalInteractionHandler = Reference< XInteractionHandler >(
            xFactory->createInstance( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.task.InteractionHandler") ) ), UNO_QUERY );
        m_xCmdEnv = new ucbhelper::CommandEnvironment( xGlobalInteractionHandler, Reference< XProgressHandler >() );
    }

    //--------------------------------------------------------------------
    ::svt::OFilePickerInteractionHandler* SmartContent::getOwnInteractionHandler() const
    {
        if (!m_xOwnInteraction.is())
            return NULL;
        return m_pOwnInteraction;
    }

    //--------------------------------------------------------------------
    SmartContent::InteractionHandlerType SmartContent::queryCurrentInteractionHandler() const
    {
        if (m_xOwnInteraction.is())
            return IHT_OWN;

        if (!m_xCmdEnv.is())
            return IHT_NONE;

        return IHT_DEFAULT;
    }

    //--------------------------------------------------------------------
    void SmartContent::disableInteractionHandler()
    {
        // Don't free the memory here! It will be done by the next
        // call automaticly - releasing of the uno reference ...
        m_pOwnInteraction = NULL;
        m_xOwnInteraction.clear();

        m_xCmdEnv.clear();
    }

    //--------------------------------------------------------------------
    void SmartContent::bindTo( const ::rtl::OUString& _rURL )
    {
        if ( getURL() == _rURL )
            // nothing to do, regardless of the state
            return;

        DELETEZ( m_pContent );
        m_eState = INVALID; // default to INVALID
        m_sURL = _rURL;

        if ( !m_sURL.isEmpty() )
        {
            try
            {
                m_pContent = new ::ucbhelper::Content( _rURL, m_xCmdEnv );
                m_eState = UNKNOWN;
                    // from now on, the state is unknown -> we cannot know for sure if the content
                    // is really valid (some UCP's only tell this when asking for properties, not upon
                    // creation)
            }
            catch( const ContentCreationException& )
            {
            }
            catch( const Exception& )
            {
                OSL_FAIL( "SmartContent::bindTo: unexpected exception caught!" );
            }
        }
        else
        {
            m_eState = NOT_BOUND;
        }


        // don't forget to reset the may internal used interaction handler ...
        // But do it only for our own specialized interaction helper!
        ::svt::OFilePickerInteractionHandler* pHandler = getOwnInteractionHandler();
        if (pHandler)
        {
            pHandler->resetUseState();
            pHandler->forgetRequest();
        }
    }

    //--------------------------------------------------------------------
    sal_Bool SmartContent::implIs( const ::rtl::OUString& _rURL, Type _eType )
    {
        // bind to this content
        bindTo( _rURL );

        // did we survive this?
        if ( isInvalid() || !isBound() )
            return sal_False;

        DBG_ASSERT( m_pContent, "SmartContent::implIs: inconsistence!" );
            // if, after an bindTo, we don't have a content, then we should be INVALID, or at least
            // NOT_BOUND (the latter happens, for example, if somebody tries to ask for an empty URL)

        sal_Bool bIs = sal_False;
        try
        {
            if ( Folder == _eType )
                bIs = m_pContent->isFolder();
            else
                bIs = m_pContent->isDocument();

            // from here on, we definately know that the content is valid
            m_eState = VALID;
        }
        catch( const Exception& )
        {
            // now we're definately invalid
            m_eState = INVALID;
        }
        return bIs;
    }

    //--------------------------------------------------------------------
    void SmartContent::getTitle( ::rtl::OUString& /* [out] */ _rTitle )
    {
        if ( !isBound() || isInvalid() )
            return;

        try
        {
            ::rtl::OUString sTitle;
            m_pContent->getPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Title" )) ) >>= sTitle;
            _rTitle =  sTitle;

            // from here on, we definately know that the content is valid
            m_eState = VALID;
        }
        catch( const ::com::sun::star::uno::Exception& )
        {
            // now we're definately invalid
            m_eState = INVALID;
        }
    }

    //--------------------------------------------------------------------
    sal_Bool SmartContent::hasParentFolder( )
    {
        if ( !isBound() || isInvalid() )
            return sal_False;

        sal_Bool bRet = sal_False;
        try
        {
            Reference< XChild > xChild( m_pContent->get(), UNO_QUERY );
            if ( xChild.is() )
            {
                Reference< XContent > xParent( xChild->getParent(), UNO_QUERY );
                if ( xParent.is() )
                {
                    const ::rtl::OUString aParentURL( xParent->getIdentifier()->getContentIdentifier() );
                    bRet = ( !aParentURL.isEmpty() && aParentURL != m_pContent->getURL() );

                    // now we're definately valid
                    m_eState = VALID;
                }
            }
        }
        catch( const Exception& )
        {
            // now we're definately invalid
            m_eState = INVALID;
        }
        return bRet;
    }

    //--------------------------------------------------------------------
    sal_Bool SmartContent::canCreateFolder( )
    {
        if ( !isBound() || isInvalid() )
            return sal_False;

        sal_Bool bRet = sal_False;
        try
        {
            Sequence< ContentInfo > aInfo = m_pContent->queryCreatableContentsInfo();
            const ContentInfo* pInfo = aInfo.getConstArray();
            sal_Int32 nCount = aInfo.getLength();
            for ( sal_Int32 i = 0; i < nCount; ++i, ++pInfo )
            {
                // Simply look for the first KIND_FOLDER...
                if ( pInfo->Attributes & ContentInfoAttribute::KIND_FOLDER )
                {
                    bRet = sal_True;
                    break;
                }
            }

            // now we're definately valid
            m_eState = VALID;
        }
        catch( const Exception& )
        {
            // now we're definately invalid
            m_eState = INVALID;
        }
        return bRet;
    }

    rtl::OUString SmartContent::createFolder( const rtl::OUString& _rTitle )
    {
        rtl::OUString aCreatedUrl;
        try
        {
            rtl::OUString sFolderType;

            Sequence< ContentInfo > aInfo = m_pContent->queryCreatableContentsInfo();
            const ContentInfo* pInfo = aInfo.getConstArray();
            sal_Int32 nCount = aInfo.getLength();
            for ( sal_Int32 i = 0; i < nCount; ++i, ++pInfo )
            {
                // Simply look for the first KIND_FOLDER...
                if ( pInfo->Attributes & ContentInfoAttribute::KIND_FOLDER )
                {
                    sFolderType = pInfo->Type;
                    break;
                }
            }

            if ( !sFolderType.isEmpty() )
            {
                ucbhelper::Content aCreated;
                Sequence< rtl::OUString > aNames( 1 );
                rtl::OUString* pNames = aNames.getArray();
                pNames[0] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Title" ) );
                Sequence< Any > aValues( 1 );
                Any* pValues = aValues.getArray();
                pValues[0] = makeAny( _rTitle );
                m_pContent->insertNewContent( sFolderType, aNames, aValues, aCreated );

                aCreatedUrl = aCreated.getURL();
            }
        }
        catch( const Exception& )
        {
        }
        return aCreatedUrl;
    }

//........................................................................
} // namespace svt
//........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
