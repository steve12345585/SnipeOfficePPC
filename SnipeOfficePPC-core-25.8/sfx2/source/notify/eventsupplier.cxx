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


//--------------------------------------------------------------------------------------------------------
#include <com/sun/star/beans/PropertyValue.hpp>

#include <com/sun/star/util/URL.hpp>

#include <com/sun/star/util/XURLTransformer.hpp>
#include <tools/urlobj.hxx>
#include <svl/macitem.hxx>
#include <sfx2/appuno.hxx>
#include <sfx2/objsh.hxx>
#include <sfx2/sfxbasemodel.hxx>
#include <sfx2/evntconf.hxx>
#include <unotools/eventcfg.hxx>

#include <unotools/securityoptions.hxx>
#include <comphelper/processfactory.hxx>
#include <comphelper/namedvaluecollection.hxx>
#include "eventsupplier.hxx"

#include <sfx2/app.hxx>
#include "sfx2/sfxresid.hxx"

#include <sfx2/sfxsids.hrc>
#include "sfxlocal.hrc"
#include <sfx2/docfile.hxx>
#include <sfx2/viewfrm.hxx>
#include <sfx2/frame.hxx>

//--------------------------------------------------------------------------------------------------------

#define MACRO_PRFIX         "macro://"
#define MACRO_POSTFIX       "()"

//--------------------------------------------------------------------------------------------------------

#define PROPERTYVALUE       ::com::sun::star::beans::PropertyValue
#define UNO_QUERY           ::com::sun::star::uno::UNO_QUERY

namespace css = ::com::sun::star;
using ::com::sun::star::uno::Sequence;
using ::com::sun::star::beans::PropertyValue;

//--------------------------------------------------------------------------------------------------------
    //  --- XNameReplace ---
//--------------------------------------------------------------------------------------------------------
void SAL_CALL SfxEvents_Impl::replaceByName( const rtl::OUString & aName, const ANY & rElement )
                                throw( ILLEGALARGUMENTEXCEPTION, NOSUCHELEMENTEXCEPTION,
                                       WRAPPEDTARGETEXCEPTION, RUNTIMEEXCEPTION )
{
    ::osl::MutexGuard aGuard( maMutex );

    // find the event in the list and replace the data
    long nCount = maEventNames.getLength();
    for ( long i=0; i<nCount; i++ )
    {
        if ( maEventNames[i] == aName )
        {
            // check for correct type of the element
            if ( !::comphelper::NamedValueCollection::canExtractFrom( rElement ) )
                throw ILLEGALARGUMENTEXCEPTION();
            ::comphelper::NamedValueCollection const aEventDescriptor( rElement );

            // create Configuration at first, creation might call this method also and that would overwrite everything
            // we might have stored before!
            if ( mpObjShell && !mpObjShell->IsLoading() )
                mpObjShell->SetModified( sal_True );

            ::comphelper::NamedValueCollection aNormalizedDescriptor;
            NormalizeMacro( aEventDescriptor, aNormalizedDescriptor, mpObjShell );

            ::rtl::OUString sType;
            if  (   ( aNormalizedDescriptor.size() == 1 )
                &&  ( aNormalizedDescriptor.has( PROP_EVENT_TYPE) == 0 )
                &&  ( aNormalizedDescriptor.get( PROP_EVENT_TYPE ) >>= sType )
                &&  ( sType.isEmpty() )
                )
            {
                // An empty event type means no binding. Therefore reset data
                // to reflect that state.
                // (that's for compatibility only. Nowadays, the Tools/Customize dialog should
                // set an empty sequence to indicate the request for resetting the assignment.)
                OSL_ENSURE( false, "legacy event assignment format detected" );
                aNormalizedDescriptor.clear();
            }

            if ( !aNormalizedDescriptor.empty() )
            {
                maEventData[i] <<= aNormalizedDescriptor.getPropertyValues();
            }
            else
            {
                maEventData[i].clear();
            }
            return;
        }
    }

    throw NOSUCHELEMENTEXCEPTION();
}

//--------------------------------------------------------------------------------------------------------
//  --- XNameAccess ---
//--------------------------------------------------------------------------------------------------------
ANY SAL_CALL SfxEvents_Impl::getByName( const rtl::OUString& aName )
                                throw( NOSUCHELEMENTEXCEPTION, WRAPPEDTARGETEXCEPTION,
                                       RUNTIMEEXCEPTION )
{
    ::osl::MutexGuard aGuard( maMutex );

    // find the event in the list and return the data

    long nCount = maEventNames.getLength();

    for ( long i=0; i<nCount; i++ )
    {
        if ( maEventNames[i] == aName )
            return maEventData[i];
    }

    throw NOSUCHELEMENTEXCEPTION();
}

//--------------------------------------------------------------------------------------------------------
SEQUENCE< rtl::OUString > SAL_CALL SfxEvents_Impl::getElementNames() throw ( RUNTIMEEXCEPTION )
{
    return maEventNames;
}

//--------------------------------------------------------------------------------------------------------
sal_Bool SAL_CALL SfxEvents_Impl::hasByName( const rtl::OUString& aName ) throw ( RUNTIMEEXCEPTION )
{
    ::osl::MutexGuard aGuard( maMutex );

    // find the event in the list and return the data

    long nCount = maEventNames.getLength();

    for ( long i=0; i<nCount; i++ )
    {
        if ( maEventNames[i] == aName )
            return sal_True;
    }

    return sal_False;
}

//--------------------------------------------------------------------------------------------------------
//  --- XElementAccess ( parent of XNameAccess ) ---
//--------------------------------------------------------------------------------------------------------
UNOTYPE SAL_CALL SfxEvents_Impl::getElementType() throw ( RUNTIMEEXCEPTION )
{
    UNOTYPE aElementType = ::getCppuType( (const SEQUENCE < PROPERTYVALUE > *)0 );
    return aElementType;
}

//--------------------------------------------------------------------------------------------------------
sal_Bool SAL_CALL SfxEvents_Impl::hasElements() throw ( RUNTIMEEXCEPTION )
{
    ::osl::MutexGuard aGuard( maMutex );

    if ( maEventNames.getLength() )
        return sal_True;
    else
        return sal_False;
}

static void Execute( ANY& aEventData, const css::document::DocumentEvent& aTrigger, SfxObjectShell* pDoc )
{
    SEQUENCE < PROPERTYVALUE > aProperties;
    if ( aEventData >>= aProperties )
    {
        rtl::OUString aType;
        rtl::OUString aScript;
        rtl::OUString aLibrary;
        rtl::OUString aMacroName;

        sal_Int32 nCount = aProperties.getLength();

        if ( !nCount )
            return;

        sal_Int32 nIndex = 0;
        while ( nIndex < nCount )
        {
            if ( aProperties[ nIndex ].Name == PROP_EVENT_TYPE )
                aProperties[ nIndex ].Value >>= aType;
            else if ( aProperties[ nIndex ].Name == PROP_SCRIPT )
                aProperties[ nIndex ].Value >>= aScript;
            else if ( aProperties[ nIndex ].Name == PROP_LIBRARY )
                aProperties[ nIndex ].Value >>= aLibrary;
            else if ( aProperties[ nIndex ].Name == PROP_MACRO_NAME )
                aProperties[ nIndex ].Value >>= aMacroName;
            else {
                OSL_FAIL("Unknown property value!");
            }
            nIndex += 1;
        }

        if (aType.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM(STAR_BASIC)) && !aScript.isEmpty())
        {
            com::sun::star::uno::Any aAny;
            SfxMacroLoader::loadMacro( aScript, aAny, pDoc );
        }
        else if (aType.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM("Service")) ||
                  aType.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM("Script")))
        {
            if ( !aScript.isEmpty() )
            {
                SfxViewFrame* pView = pDoc ?
                    SfxViewFrame::GetFirst( pDoc ) :
                    SfxViewFrame::Current();

                ::com::sun::star::uno::Reference
                    < ::com::sun::star::util::XURLTransformer > xTrans(
                        ::comphelper::getProcessServiceFactory()->createInstance(
                            rtl::OUString(
                                "com.sun.star.util.URLTransformer" ) ),
                        UNO_QUERY );

                ::com::sun::star::util::URL aURL;
                aURL.Complete = aScript;
                xTrans->parseStrict( aURL );

                ::com::sun::star::uno::Reference
                    < ::com::sun::star::frame::XDispatchProvider > xProv;

                if ( pView != NULL )
                {
                    xProv = ::com::sun::star::uno::Reference
                        < ::com::sun::star::frame::XDispatchProvider > (
                            pView->GetFrame().GetFrameInterface(), UNO_QUERY );
                }
                else
                {
                    xProv = ::com::sun::star::uno::Reference
                        < ::com::sun::star::frame::XDispatchProvider > (
                            ::comphelper::getProcessServiceFactory()->createInstance(
                                rtl::OUString(
                                    "com.sun.star.frame.Desktop" ) ),
                            UNO_QUERY );
                }

                ::com::sun::star::uno::Reference < ::com::sun::star::frame::XDispatch > xDisp;
                if ( xProv.is() )
                    xDisp = xProv->queryDispatch( aURL, ::rtl::OUString(), 0 );

                if ( xDisp.is() )
                {

                    css::beans::PropertyValue aEventParam;
                    aEventParam.Value <<= aTrigger;
                    css::uno::Sequence< css::beans::PropertyValue > aDispatchArgs( &aEventParam, 1 );
                    xDisp->dispatch( aURL, aDispatchArgs );
                }
            }
        }
        else if ( aType.isEmpty() )
        {
            // Empty type means no active binding for the event. Just ignore do nothing.
        }
        else
        {
            SAL_WARN( "sfx2.notify", "notifyEvent(): Unsupported event type" );
        }
    }
}

//--------------------------------------------------------------------------------------------------------
// --- ::document::XEventListener ---
//--------------------------------------------------------------------------------------------------------
void SAL_CALL SfxEvents_Impl::notifyEvent( const DOCEVENTOBJECT& aEvent ) throw( RUNTIMEEXCEPTION )
{
    ::osl::ClearableMutexGuard aGuard( maMutex );

    // get the event name, find the coresponding data, execute the data

    rtl::OUString aName   = aEvent.EventName;
    long        nCount  = maEventNames.getLength();
    long        nIndex  = 0;
    sal_Bool    bFound  = sal_False;

    while ( !bFound && ( nIndex < nCount ) )
    {
        if ( maEventNames[nIndex] == aName )
            bFound = sal_True;
        else
            nIndex += 1;
    }

    if ( !bFound )
        return;

    ANY aEventData = maEventData[ nIndex ];
    aGuard.clear();
    Execute( aEventData, css::document::DocumentEvent(aEvent.Source, aEvent.EventName, NULL, css::uno::Any()), mpObjShell );
}

//--------------------------------------------------------------------------------------------------------
// --- ::lang::XEventListener ---
//--------------------------------------------------------------------------------------------------------
void SAL_CALL SfxEvents_Impl::disposing( const EVENTOBJECT& /*Source*/ ) throw( RUNTIMEEXCEPTION )
{
    ::osl::MutexGuard aGuard( maMutex );

    if ( mxBroadcaster.is() )
    {
        mxBroadcaster->removeEventListener( this );
        mxBroadcaster = NULL;
    }
}

//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
SfxEvents_Impl::SfxEvents_Impl( SfxObjectShell* pShell,
                                REFERENCE< XEVENTBROADCASTER > xBroadcaster )
{
    // get the list of supported events and store it
    if ( pShell )
        maEventNames = pShell->GetEventNames();
    else
        maEventNames = GlobalEventConfig().getElementNames();

    maEventData = SEQUENCE < ANY > ( maEventNames.getLength() );

    mpObjShell      = pShell;
    mxBroadcaster   = xBroadcaster;

    if ( mxBroadcaster.is() )
        mxBroadcaster->addEventListener( this );
}

//--------------------------------------------------------------------------------------------------------
SfxEvents_Impl::~SfxEvents_Impl()
{
}

//--------------------------------------------------------------------------------------------------------
SvxMacro* SfxEvents_Impl::ConvertToMacro( const ANY& rElement, SfxObjectShell* pObjShell, sal_Bool bNormalizeMacro )
{
    SvxMacro* pMacro = NULL;
    SEQUENCE < PROPERTYVALUE > aProperties;
    ANY aAny;
    if ( bNormalizeMacro )
        NormalizeMacro( rElement, aAny, pObjShell );
    else
        aAny = rElement;

    if ( aAny >>= aProperties )
    {
        rtl::OUString aType;
        rtl::OUString aScriptURL;
        rtl::OUString aLibrary;
        rtl::OUString aMacroName;

        long nCount = aProperties.getLength();
        long nIndex = 0;

        if ( !nCount )
            return pMacro;

        while ( nIndex < nCount )
        {
            if ( aProperties[ nIndex ].Name == PROP_EVENT_TYPE )
                aProperties[ nIndex ].Value >>= aType;
            else if ( aProperties[ nIndex ].Name == PROP_SCRIPT )
                aProperties[ nIndex ].Value >>= aScriptURL;
            else if ( aProperties[ nIndex ].Name == PROP_LIBRARY )
                aProperties[ nIndex ].Value >>= aLibrary;
            else if ( aProperties[ nIndex ].Name == PROP_MACRO_NAME )
                aProperties[ nIndex ].Value >>= aMacroName;
            else {
                OSL_FAIL("Unknown propery value!");
            }
            nIndex += 1;
        }

        // Get the type
        ScriptType  eType( STARBASIC );
        if ( aType == STAR_BASIC )
            eType = STARBASIC;
        else if (aType.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM("Script")) && !aScriptURL.isEmpty())
            eType = EXTENDED_STYPE;
        else if ( aType == SVX_MACRO_LANGUAGE_JAVASCRIPT )
            eType = JAVASCRIPT;
        else {
            SAL_WARN( "sfx2.notify", "ConvertToMacro: Unknown macro type" );
        }

        if ( !aMacroName.isEmpty() )
        {
            if ( aLibrary == "application" )
                aLibrary = SFX_APP()->GetName();
            else
                aLibrary = ::rtl::OUString();
            pMacro = new SvxMacro( aMacroName, aLibrary, eType );
        }
        else if ( eType == EXTENDED_STYPE )
            pMacro = new SvxMacro( aScriptURL, aType );
    }

    return pMacro;
}

void SfxEvents_Impl::NormalizeMacro( const ANY& rEvent, ANY& rRet, SfxObjectShell* pDoc )
{
    const ::comphelper::NamedValueCollection aEventDescriptor( rEvent );
    ::comphelper::NamedValueCollection aEventDescriptorOut;

    NormalizeMacro( aEventDescriptor, aEventDescriptorOut, pDoc );

    rRet <<= aEventDescriptorOut.getPropertyValues();
}

void SfxEvents_Impl::NormalizeMacro( const ::comphelper::NamedValueCollection& i_eventDescriptor,
        ::comphelper::NamedValueCollection& o_normalizedDescriptor, SfxObjectShell* i_document )
{
    SfxObjectShell* pDoc = i_document;
    if ( !pDoc )
        pDoc = SfxObjectShell::Current();

    ::rtl::OUString aType = i_eventDescriptor.getOrDefault( PROP_EVENT_TYPE, ::rtl::OUString() );
    ::rtl::OUString aScript = i_eventDescriptor.getOrDefault( PROP_SCRIPT, ::rtl::OUString() );
    ::rtl::OUString aLibrary = i_eventDescriptor.getOrDefault( PROP_LIBRARY, ::rtl::OUString() );
    ::rtl::OUString aMacroName = i_eventDescriptor.getOrDefault( PROP_MACRO_NAME, ::rtl::OUString() );

    if ( !aType.isEmpty() )
        o_normalizedDescriptor.put( PROP_EVENT_TYPE, aType );
    if ( !aScript.isEmpty() )
        o_normalizedDescriptor.put( PROP_SCRIPT, aScript );

    if ( aType == STAR_BASIC )
    {
        if ( !aScript.isEmpty() )
        {
            if ( aMacroName.isEmpty() || aLibrary.isEmpty() )
            {
                sal_Int32 nHashPos = aScript.indexOf( '/', 8 );
                sal_Int32 nArgsPos = aScript.indexOf( '(' );
                if ( ( nHashPos != STRING_NOTFOUND ) && ( nHashPos < nArgsPos ) )
                {
                    rtl::OUString aBasMgrName( INetURLObject::decode( aScript.copy( 8, nHashPos-8 ), INET_HEX_ESCAPE, INetURLObject::DECODE_WITH_CHARSET ) );
                    if ( aBasMgrName == "." )
                        aLibrary = pDoc->GetTitle();
                    else
                        aLibrary = SFX_APP()->GetName();

                    // Get the macro name
                    aMacroName = aScript.copy( nHashPos+1, nArgsPos - nHashPos - 1 );
                }
                else
                {
                    SAL_WARN( "sfx2.notify", "ConvertToMacro: Unknown macro url format" );
                }
            }
        }
        else if ( !aMacroName.isEmpty() )
        {
            aScript = rtl::OUString( MACRO_PRFIX  );
            if ( aLibrary.compareTo( SFX_APP()->GetName() ) != 0 && !aLibrary.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM("StarDesktop")) && !aLibrary.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM("application")) )
                aScript += String('.');

            aScript += String('/');
            aScript += aMacroName;
            aScript += rtl::OUString( MACRO_POSTFIX  );
        }
        else
            // wrong properties
            return;

        if (!aLibrary.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM("document")))
        {
            if ( aLibrary.isEmpty() || (pDoc && ( String(aLibrary) == pDoc->GetTitle( SFX_TITLE_APINAME ) || String(aLibrary) == pDoc->GetTitle() )) )
                aLibrary = String::CreateFromAscii("document");
            else
                aLibrary = String::CreateFromAscii("application");
        }

        o_normalizedDescriptor.put( PROP_SCRIPT, aScript );
        o_normalizedDescriptor.put( PROP_LIBRARY, aLibrary );
        o_normalizedDescriptor.put( PROP_MACRO_NAME, aMacroName );
    }
}

ModelCollectionEnumeration::ModelCollectionEnumeration(const css::uno::Reference< css::lang::XMultiServiceFactory >& xSMGR)
    : ModelCollectionMutexBase(                 )
    , m_xSMGR                 (xSMGR            )
    , m_pEnumerationIt        (m_lModels.begin())
{
}

ModelCollectionEnumeration::~ModelCollectionEnumeration()
{
}

void ModelCollectionEnumeration::setModelList(const TModelList& rList)
{
    // SAFE ->
    ::osl::ResettableMutexGuard aLock(m_aLock);
    m_lModels        = rList;
    m_pEnumerationIt = m_lModels.begin();
    aLock.clear();
    // <- SAFE
}

sal_Bool SAL_CALL ModelCollectionEnumeration::hasMoreElements()
    throw(css::uno::RuntimeException)
{
    // SAFE ->
    ::osl::ResettableMutexGuard aLock(m_aLock);
    return (m_pEnumerationIt != m_lModels.end());
    // <- SAFE
}

css::uno::Any SAL_CALL ModelCollectionEnumeration::nextElement()
    throw(css::container::NoSuchElementException,
          css::lang::WrappedTargetException     ,
          css::uno::RuntimeException            )
{
    // SAFE ->
    ::osl::ResettableMutexGuard aLock(m_aLock);
    if (m_pEnumerationIt == m_lModels.end())
        throw css::container::NoSuchElementException(
                    ::rtl::OUString("End of model enumeration reached."),
                    static_cast< css::container::XEnumeration* >(this));
    css::uno::Reference< css::frame::XModel > xModel(*m_pEnumerationIt, UNO_QUERY);
    ++m_pEnumerationIt;
    aLock.clear();
    // <- SAFE

    return css::uno::makeAny(xModel);
}

SFX_IMPL_XSERVICEINFO( SfxGlobalEvents_Impl, "com.sun.star.frame.GlobalEventBroadcaster", "com.sun.star.comp.sfx2.GlobalEventBroadcaster" )
SFX_IMPL_ONEINSTANCEFACTORY( SfxGlobalEvents_Impl );

//-----------------------------------------------------------------------------
SfxGlobalEvents_Impl::SfxGlobalEvents_Impl( const com::sun::star::uno::Reference < ::com::sun::star::lang::XMultiServiceFactory >& xSMGR)
    : ModelCollectionMutexBase(       )
    , m_xSMGR                 (xSMGR  )
    , m_aLegacyListeners      (m_aLock)
    , m_aDocumentListeners    (m_aLock)
    , pImp                    (0      )
{
    m_refCount++;
    SFX_APP();
    pImp                   = new GlobalEventConfig();
    m_xEvents              = pImp;
    m_xJobExecutorListener = css::uno::Reference< css::document::XEventListener >(
                        xSMGR->createInstance(::rtl::OUString("com.sun.star.task.JobExecutor")),
                        UNO_QUERY);
    m_refCount--;
}

//-----------------------------------------------------------------------------
SfxGlobalEvents_Impl::~SfxGlobalEvents_Impl()
{
}

//-----------------------------------------------------------------------------
css::uno::Reference< css::container::XNameReplace > SAL_CALL SfxGlobalEvents_Impl::getEvents()
    throw(css::uno::RuntimeException)
{
    // SAFE ->
    ::osl::ResettableMutexGuard aLock(m_aLock);
    return m_xEvents;
    // <- SAFE
}

//-----------------------------------------------------------------------------
void SAL_CALL SfxGlobalEvents_Impl::addEventListener(const css::uno::Reference< css::document::XEventListener >& xListener)
    throw(css::uno::RuntimeException)
{
    // container is threadsafe
    m_aLegacyListeners.addInterface(xListener);
}

//-----------------------------------------------------------------------------
void SAL_CALL SfxGlobalEvents_Impl::removeEventListener(const css::uno::Reference< css::document::XEventListener >& xListener)
    throw(css::uno::RuntimeException)
{
    // container is threadsafe
    m_aLegacyListeners.removeInterface(xListener);
}

//-----------------------------------------------------------------------------
void SAL_CALL SfxGlobalEvents_Impl::addDocumentEventListener( const css::uno::Reference< css::document::XDocumentEventListener >& _Listener )
    throw(css::uno::RuntimeException)
{
    m_aDocumentListeners.addInterface( _Listener );
}

//-----------------------------------------------------------------------------
void SAL_CALL SfxGlobalEvents_Impl::removeDocumentEventListener( const css::uno::Reference< css::document::XDocumentEventListener >& _Listener )
    throw(css::uno::RuntimeException)
{
    m_aDocumentListeners.removeInterface( _Listener );
}

//-----------------------------------------------------------------------------
void SAL_CALL SfxGlobalEvents_Impl::notifyDocumentEvent( const ::rtl::OUString& /*_EventName*/,
        const css::uno::Reference< css::frame::XController2 >& /*_ViewController*/, const css::uno::Any& /*_Supplement*/ )
        throw (css::lang::IllegalArgumentException, css::lang::NoSupportException, css::uno::RuntimeException)
{
    // we're a multiplexer only, no chance to generate artifical events here
    throw css::lang::NoSupportException(::rtl::OUString(), *this);
}

//-----------------------------------------------------------------------------
void SAL_CALL SfxGlobalEvents_Impl::notifyEvent(const css::document::EventObject& aEvent)
    throw(css::uno::RuntimeException)
{
    css::document::DocumentEvent aDocEvent(aEvent.Source, aEvent.EventName, NULL, css::uno::Any());
    implts_notifyJobExecution(aEvent);
    implts_checkAndExecuteEventBindings(aDocEvent);
    implts_notifyListener(aDocEvent);
}

//-----------------------------------------------------------------------------
void SAL_CALL SfxGlobalEvents_Impl::documentEventOccured( const ::css::document::DocumentEvent& _Event )
    throw (::css::uno::RuntimeException)
{
    implts_notifyJobExecution(css::document::EventObject(_Event.Source, _Event.EventName));
    implts_checkAndExecuteEventBindings(_Event);
    implts_notifyListener(_Event);
}

//-----------------------------------------------------------------------------
void SAL_CALL SfxGlobalEvents_Impl::disposing(const css::lang::EventObject& aEvent)
    throw(css::uno::RuntimeException)
{
    css::uno::Reference< css::frame::XModel > xDoc(aEvent.Source, UNO_QUERY);

    // SAFE ->
    ::osl::ResettableMutexGuard aLock(m_aLock);
    TModelList::iterator pIt = impl_searchDoc(xDoc);
    if (pIt != m_lModels.end())
        m_lModels.erase(pIt);
    aLock.clear();
    // <- SAFE
}

//-----------------------------------------------------------------------------
sal_Bool SAL_CALL SfxGlobalEvents_Impl::has(const css::uno::Any& aElement)
    throw (css::uno::RuntimeException)
{
    css::uno::Reference< css::frame::XModel > xDoc;
    aElement >>= xDoc;

    sal_Bool bHas = sal_False;

    // SAFE ->
    ::osl::ResettableMutexGuard aLock(m_aLock);
    TModelList::iterator pIt = impl_searchDoc(xDoc);
    if (pIt != m_lModels.end())
        bHas = sal_True;
    aLock.clear();
    // <- SAFE

    return bHas;
}

//-----------------------------------------------------------------------------
void SAL_CALL SfxGlobalEvents_Impl::insert( const css::uno::Any& aElement )
    throw (css::lang::IllegalArgumentException  ,
           css::container::ElementExistException,
           css::uno::RuntimeException           )
{
    css::uno::Reference< css::frame::XModel > xDoc;
    aElement >>= xDoc;
    if (!xDoc.is())
        throw css::lang::IllegalArgumentException(
                ::rtl::OUString("Cant locate at least the model parameter."),
                static_cast< css::container::XSet* >(this),
                0);

    // SAFE ->
    ::osl::ResettableMutexGuard aLock(m_aLock);
    TModelList::iterator pIt = impl_searchDoc(xDoc);
    if (pIt != m_lModels.end())
        throw css::container::ElementExistException(
                ::rtl::OUString(),
                static_cast< css::container::XSet* >(this));
    m_lModels.push_back(xDoc);
    aLock.clear();
    // <- SAFE

    css::uno::Reference< css::document::XDocumentEventBroadcaster > xDocBroadcaster(xDoc, UNO_QUERY );
    if (xDocBroadcaster.is())
        xDocBroadcaster->addDocumentEventListener(this);
    else
    {
        // try the "legacy version" of XDocumentEventBroadcaster, which is XEventBroadcaster
        css::uno::Reference< css::document::XEventBroadcaster > xBroadcaster(xDoc, UNO_QUERY);
        if (xBroadcaster.is())
            xBroadcaster->addEventListener(static_cast< css::document::XEventListener* >(this));
    }
}

//-----------------------------------------------------------------------------
void SAL_CALL SfxGlobalEvents_Impl::remove( const css::uno::Any& aElement )
    throw (css::lang::IllegalArgumentException   ,
           css::container::NoSuchElementException,
           css::uno::RuntimeException            )
{
    css::uno::Reference< css::frame::XModel > xDoc;
    aElement >>= xDoc;
    if (!xDoc.is())
        throw css::lang::IllegalArgumentException(
                ::rtl::OUString("Cant locate at least the model parameter."),
                static_cast< css::container::XSet* >(this),
                0);

    // SAFE ->
    ::osl::ResettableMutexGuard aLock(m_aLock);
    TModelList::iterator pIt = impl_searchDoc(xDoc);
    if (pIt == m_lModels.end())
        throw css::container::NoSuchElementException(
                ::rtl::OUString(),
                static_cast< css::container::XSet* >(this));
    m_lModels.erase(pIt);
    aLock.clear();
    // <- SAFE

    css::uno::Reference< css::document::XDocumentEventBroadcaster > xDocBroadcaster(xDoc, UNO_QUERY );
    if (xDocBroadcaster.is())
        xDocBroadcaster->removeDocumentEventListener(this);
    else
    {
        // try the "legacy version" of XDocumentEventBroadcaster, which is XEventBroadcaster
        css::uno::Reference< css::document::XEventBroadcaster > xBroadcaster(xDoc, UNO_QUERY);
        if (xBroadcaster.is())
            xBroadcaster->removeEventListener(static_cast< css::document::XEventListener* >(this));
    }
}

//-----------------------------------------------------------------------------
css::uno::Reference< css::container::XEnumeration > SAL_CALL SfxGlobalEvents_Impl::createEnumeration()
    throw (css::uno::RuntimeException)
{
    // SAFE ->
    ::osl::ResettableMutexGuard aLock(m_aLock);
    ModelCollectionEnumeration* pEnum = new ModelCollectionEnumeration(m_xSMGR);
    pEnum->setModelList(m_lModels);
    css::uno::Reference< css::container::XEnumeration > xEnum(
        static_cast< css::container::XEnumeration* >(pEnum),
        UNO_QUERY);
    aLock.clear();
    // <- SAFE

    return xEnum;
}

//-----------------------------------------------------------------------------
css::uno::Type SAL_CALL SfxGlobalEvents_Impl::getElementType()
    throw (css::uno::RuntimeException)
{
    return ::getCppuType(static_cast< css::uno::Reference< css::frame::XModel >* >(NULL));
}

//-----------------------------------------------------------------------------
sal_Bool SAL_CALL SfxGlobalEvents_Impl::hasElements()
    throw (css::uno::RuntimeException)
{
    // SAFE ->
    ::osl::ResettableMutexGuard aLock(m_aLock);
    return (m_lModels.size()>0);
    // <- SAFE
}

//-----------------------------------------------------------------------------
void SfxGlobalEvents_Impl::implts_notifyJobExecution(const css::document::EventObject& aEvent)
{
    try
    {
        // SAFE ->
        ::osl::ResettableMutexGuard aLock(m_aLock);
        css::uno::Reference< css::document::XEventListener > xJobExecutor(m_xJobExecutorListener);
        aLock.clear();
        // <- SAFE
        if (xJobExecutor.is())
            xJobExecutor->notifyEvent(aEvent);
    }
    catch(const css::uno::RuntimeException&)
        { throw; }
    catch(const css::uno::Exception&)
        {}
}

//-----------------------------------------------------------------------------
void SfxGlobalEvents_Impl::implts_checkAndExecuteEventBindings(const css::document::DocumentEvent& aEvent)
{
    try
    {
        // SAFE ->
        ::osl::ResettableMutexGuard aLock(m_aLock);
        css::uno::Reference< css::container::XNameReplace > xEvents = m_xEvents;
        aLock.clear();
        // <- SAFE

        css::uno::Any aAny;
        if (xEvents.is())
            aAny = xEvents->getByName(aEvent.EventName);
        Execute(aAny, aEvent, 0);
    }
    catch(const css::uno::RuntimeException&)
        { throw; }
    catch(const css::uno::Exception&)
        {}
}

//-----------------------------------------------------------------------------
void SfxGlobalEvents_Impl::implts_notifyListener(const css::document::DocumentEvent& aEvent)
{
    // containers are threadsafe
    css::document::EventObject aLegacyEvent(aEvent.Source, aEvent.EventName);
    m_aLegacyListeners.notifyEach( &css::document::XEventListener::notifyEvent, aLegacyEvent );

    m_aDocumentListeners.notifyEach( &css::document::XDocumentEventListener::documentEventOccured, aEvent );
}

//-----------------------------------------------------------------------------
// not threadsafe ... must be locked from outside!
TModelList::iterator SfxGlobalEvents_Impl::impl_searchDoc(const css::uno::Reference< css::frame::XModel >& xModel)
{
    if (!xModel.is())
        return m_lModels.end();

    TModelList::iterator pIt;
    for (  pIt  = m_lModels.begin();
           pIt != m_lModels.end()  ;
         ++pIt                     )
    {
        css::uno::Reference< css::frame::XModel > xContainerDoc(*pIt, UNO_QUERY);
        if (xContainerDoc == xModel)
            break;
    }

    return pIt;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
