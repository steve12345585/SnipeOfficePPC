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


//_________________________________________________________________________________________________________________
//  my own includes
//_________________________________________________________________________________________________________________
#include "uielement/uicommanddescription.hxx"
#include <threadhelp/resetableguard.hxx>
#include "services.h"

#include "properties.h"

#include "helper/mischelper.hxx"

//_________________________________________________________________________________________________________________
//  interface includes
//_________________________________________________________________________________________________________________
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/container/XContainer.hpp>

//_________________________________________________________________________________________________________________
//  includes of other projects
//_________________________________________________________________________________________________________________
#include <rtl/ustrbuf.hxx>
#include <cppuhelper/implbase2.hxx>
#include <unotools/configmgr.hxx>
#include <tools/string.hxx>

#include <vcl/mnemonic.hxx>
#include <comphelper/sequence.hxx>
#include <rtl/logfile.hxx>

//_________________________________________________________________________________________________________________
//  Defines
//_________________________________________________________________________________________________________________

using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::beans;
using namespace com::sun::star::container;
using namespace ::com::sun::star::frame;

//_________________________________________________________________________________________________________________
//  Namespace
//_________________________________________________________________________________________________________________

struct ModuleToCommands
{
    const char* pModuleId;
    const char* pCommands;
};

static const char CONFIGURATION_ROOT_ACCESS[]           = "/org.openoffice.Office.UI.";
static const char CONFIGURATION_CMD_ELEMENT_ACCESS[]    = "/UserInterface/Commands";
static const char CONFIGURATION_POP_ELEMENT_ACCESS[]    = "/UserInterface/Popups";
static const char CONFIGURATION_PROPERTY_LABEL[]        = "Label";
static const char CONFIGURATION_PROPERTY_CONTEXT_LABEL[] = "ContextLabel";

// Property names of the resulting Property Set
static const char PROPSET_LABEL[]                       = "Label";
static const char PROPSET_NAME[]                        = "Name";
static const char PROPSET_POPUP[]                       = "Popup";
static const char PROPSET_PROPERTIES[]                  = "Properties";

// Special resource URLs to retrieve additional information
static const char PRIVATE_RESOURCE_URL[]                = "private:";

const sal_Int32   COMMAND_PROPERTY_IMAGE                = 1;
const sal_Int32   COMMAND_PROPERTY_ROTATE               = 2;
const sal_Int32   COMMAND_PROPERTY_MIRROR               = 4;

namespace framework
{

//*****************************************************************************************************************
//  Configuration access class for PopupMenuControllerFactory implementation
//*****************************************************************************************************************

class ConfigurationAccess_UICommand : // Order is neccessary for right initialization!
                                        private ThreadHelpBase                           ,
                                        public  ::cppu::WeakImplHelper2<XNameAccess,XContainerListener>
{
    public:
                                  ConfigurationAccess_UICommand( const ::rtl::OUString& aModuleName, const Reference< XNameAccess >& xGenericUICommands, const Reference< XMultiServiceFactory >& rServiceManager );
        virtual                   ~ConfigurationAccess_UICommand();

        // XNameAccess
        virtual ::com::sun::star::uno::Any SAL_CALL getByName( const ::rtl::OUString& aName )
            throw (::com::sun::star::container::NoSuchElementException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getElementNames()
            throw (::com::sun::star::uno::RuntimeException);

        virtual sal_Bool SAL_CALL hasByName( const ::rtl::OUString& aName )
            throw (::com::sun::star::uno::RuntimeException);

        // XElementAccess
        virtual ::com::sun::star::uno::Type SAL_CALL getElementType()
            throw (::com::sun::star::uno::RuntimeException);

        virtual sal_Bool SAL_CALL hasElements()
            throw (::com::sun::star::uno::RuntimeException);

        // container.XContainerListener
        virtual void SAL_CALL     elementInserted( const ContainerEvent& aEvent ) throw(RuntimeException);
        virtual void SAL_CALL     elementRemoved ( const ContainerEvent& aEvent ) throw(RuntimeException);
        virtual void SAL_CALL     elementReplaced( const ContainerEvent& aEvent ) throw(RuntimeException);

        // lang.XEventListener
        virtual void SAL_CALL disposing( const EventObject& aEvent ) throw(RuntimeException);

    protected:
        virtual ::com::sun::star::uno::Any SAL_CALL getByNameImpl( const ::rtl::OUString& aName );

        struct CmdToInfoMap
        {
            CmdToInfoMap() : bPopup( false ),
                             bCommandNameCreated( false ),
                             nProperties( 0 ) {}

            rtl::OUString       aLabel;
            rtl::OUString       aContextLabel;
            rtl::OUString       aCommandName;
            bool                bPopup : 1,
                                bCommandNameCreated : 1;
            sal_Int32           nProperties;
        };

        Any                       getSequenceFromCache( const rtl::OUString& aCommandURL );
        Any                       getInfoFromCommand( const rtl::OUString& rCommandURL );
        void                      fillInfoFromResult( CmdToInfoMap& rCmdInfo, const rtl::OUString& aLabel );
        Any                       getUILabelFromCommand( const rtl::OUString& rCommandURL );
        Sequence< rtl::OUString > getAllCommands();
        sal_Bool                  fillCache();
        sal_Bool                  addGenericInfoToCache();
        void                      impl_fill(const Reference< XNameAccess >& _xConfigAccess,sal_Bool _bPopup,
                                                std::vector< ::rtl::OUString >& aImageCommandVector,
                                                std::vector< ::rtl::OUString >& aImageRotateVector,
                                                std::vector< ::rtl::OUString >& aImageMirrorVector);

    private:
        typedef ::boost::unordered_map< ::rtl::OUString,
                                 CmdToInfoMap,
                                 OUStringHashCode,
                                 ::std::equal_to< ::rtl::OUString > > CommandToInfoCache;

        sal_Bool initializeConfigAccess();

        rtl::OUString                     m_aConfigCmdAccess;
        rtl::OUString                     m_aConfigPopupAccess;
        rtl::OUString                     m_aPropUILabel;
        rtl::OUString                     m_aPropUIContextLabel;
        rtl::OUString                     m_aPropLabel;
        rtl::OUString                     m_aPropName;
        rtl::OUString                     m_aPropPopup;
        rtl::OUString                     m_aPropProperties;
        rtl::OUString                     m_aXMLFileFormatVersion;
        rtl::OUString                     m_aVersion;
        rtl::OUString                     m_aExtension;
        rtl::OUString                     m_aPrivateResourceURL;
        Reference< XNameAccess >          m_xGenericUICommands;
        Reference< XMultiServiceFactory > m_xServiceManager;
        Reference< XMultiServiceFactory > m_xConfigProvider;
        Reference< XNameAccess >          m_xConfigAccess;
        Reference< XContainerListener >   m_xConfigListener;
        Reference< XNameAccess >          m_xConfigAccessPopups;
        Reference< XContainerListener >   m_xConfigAccessListener;
        Sequence< rtl::OUString >         m_aCommandImageList;
        Sequence< rtl::OUString >         m_aCommandRotateImageList;
        Sequence< rtl::OUString >         m_aCommandMirrorImageList;
        CommandToInfoCache                m_aCmdInfoCache;
        sal_Bool                          m_bConfigAccessInitialized;
        sal_Bool                          m_bCacheFilled;
        sal_Bool                          m_bGenericDataRetrieved;
};

//*****************************************************************************************************************
//  XInterface, XTypeProvider
//*****************************************************************************************************************
ConfigurationAccess_UICommand::ConfigurationAccess_UICommand( const rtl::OUString& aModuleName, const Reference< XNameAccess >& rGenericUICommands, const Reference< XMultiServiceFactory >& rServiceManager ) :
    ThreadHelpBase(),
    m_aConfigCmdAccess( CONFIGURATION_ROOT_ACCESS ),
    m_aConfigPopupAccess( CONFIGURATION_ROOT_ACCESS ),
    m_aPropUILabel( CONFIGURATION_PROPERTY_LABEL ),
    m_aPropUIContextLabel( CONFIGURATION_PROPERTY_CONTEXT_LABEL ),
    m_aPropLabel( PROPSET_LABEL ),
    m_aPropName( PROPSET_NAME ),
    m_aPropPopup( PROPSET_POPUP ),
    m_aPropProperties( PROPSET_PROPERTIES ),
    m_aPrivateResourceURL( PRIVATE_RESOURCE_URL ),
    m_xGenericUICommands( rGenericUICommands ),
    m_xServiceManager( rServiceManager ),
    m_bConfigAccessInitialized( sal_False ),
    m_bCacheFilled( sal_False ),
    m_bGenericDataRetrieved( sal_False )
{
    // Create configuration hierachical access name
    m_aConfigCmdAccess += aModuleName;
    m_aConfigCmdAccess += rtl::OUString( CONFIGURATION_CMD_ELEMENT_ACCESS );

    m_xConfigProvider = Reference< XMultiServiceFactory >( rServiceManager->createInstance(SERVICENAME_CFGPROVIDER),UNO_QUERY );

    m_aConfigPopupAccess += aModuleName;
    m_aConfigPopupAccess += rtl::OUString( CONFIGURATION_POP_ELEMENT_ACCESS );
}

ConfigurationAccess_UICommand::~ConfigurationAccess_UICommand()
{
    // SAFE
    ResetableGuard aLock( m_aLock );
    Reference< XContainer > xContainer( m_xConfigAccess, UNO_QUERY );
    if ( xContainer.is() )
        xContainer->removeContainerListener(m_xConfigListener);
    xContainer = Reference< XContainer >( m_xConfigAccessPopups, UNO_QUERY );
    if ( xContainer.is() )
        xContainer->removeContainerListener(m_xConfigAccessListener);
}


// XNameAccess
Any SAL_CALL ConfigurationAccess_UICommand::getByNameImpl( const ::rtl::OUString& rCommandURL )
{
    static sal_Int32 nRequests  = 0;

    ResetableGuard aLock( m_aLock );
    if ( !m_bConfigAccessInitialized )
    {
        initializeConfigAccess();
        m_bConfigAccessInitialized = sal_True;
        fillCache();
    }

    if ( rCommandURL.indexOf( m_aPrivateResourceURL ) == 0 )
    {
        // special keys to retrieve information about a set of commands
        // SAFE
        addGenericInfoToCache();

        if ( rCommandURL.equalsIgnoreAsciiCaseAscii( UICOMMANDDESCRIPTION_NAMEACCESS_COMMANDIMAGELIST ))
            return makeAny( m_aCommandImageList );
        else if ( rCommandURL.equalsIgnoreAsciiCaseAscii( UICOMMANDDESCRIPTION_NAMEACCESS_COMMANDROTATEIMAGELIST ))
            return makeAny( m_aCommandRotateImageList );
        else if ( rCommandURL.equalsIgnoreAsciiCaseAscii( UICOMMANDDESCRIPTION_NAMEACCESS_COMMANDMIRRORIMAGELIST ))
            return makeAny( m_aCommandMirrorImageList );
        else
            return Any();
    }
    else
    {
        // SAFE
        ++nRequests;
        return getInfoFromCommand( rCommandURL );
    }
}

Any SAL_CALL ConfigurationAccess_UICommand::getByName( const ::rtl::OUString& rCommandURL )
throw ( NoSuchElementException, WrappedTargetException, RuntimeException)
{
    Any aRet( getByNameImpl( rCommandURL ) );
    if( !aRet.hasValue() )
        throw NoSuchElementException();

    return aRet;
}

Sequence< ::rtl::OUString > SAL_CALL ConfigurationAccess_UICommand::getElementNames()
throw ( RuntimeException )
{
    return getAllCommands();
}

sal_Bool SAL_CALL ConfigurationAccess_UICommand::hasByName( const ::rtl::OUString& rCommandURL )
throw (::com::sun::star::uno::RuntimeException)
{
    return getByNameImpl( rCommandURL ).hasValue();
}

// XElementAccess
Type SAL_CALL ConfigurationAccess_UICommand::getElementType()
throw ( RuntimeException )
{
    return( ::getCppuType( (const Sequence< PropertyValue >*)NULL ) );
}

sal_Bool SAL_CALL ConfigurationAccess_UICommand::hasElements()
throw ( RuntimeException )
{
    // There must are global commands!
    return sal_True;
}

void ConfigurationAccess_UICommand::fillInfoFromResult( CmdToInfoMap& rCmdInfo, const rtl::OUString& aLabel )
{
    String rStr( aLabel );
    rStr.SearchAndReplaceAllAscii(
        "%PRODUCTNAME", utl::ConfigManager::getProductName() );
    rCmdInfo.aLabel       = ::rtl::OUString( rStr );
    rStr.EraseTrailingChars( '.' ); // Remove "..." from string
    rCmdInfo.aCommandName = ::rtl::OUString( MnemonicGenerator::EraseAllMnemonicChars( rStr ));
    rCmdInfo.bCommandNameCreated = sal_True;
}

Any ConfigurationAccess_UICommand::getSequenceFromCache( const ::rtl::OUString& aCommandURL )
{
    CommandToInfoCache::iterator pIter = m_aCmdInfoCache.find( aCommandURL );
    if ( pIter != m_aCmdInfoCache.end() )
    {
        if ( !pIter->second.bCommandNameCreated )
            fillInfoFromResult( pIter->second, pIter->second.aLabel );

        Sequence< PropertyValue > aPropSeq( 4 );
        aPropSeq[0].Name  = m_aPropLabel;
        aPropSeq[0].Value = !pIter->second.aContextLabel.isEmpty() ?
                makeAny( pIter->second.aContextLabel ): makeAny( pIter->second.aLabel );
        aPropSeq[1].Name  = m_aPropName;
        aPropSeq[1].Value <<= pIter->second.aCommandName;
        aPropSeq[2].Name  = m_aPropPopup;
        aPropSeq[2].Value <<= pIter->second.bPopup;
        aPropSeq[3].Name  = m_aPropProperties;
        aPropSeq[3].Value <<= pIter->second.nProperties;
        return makeAny( aPropSeq );
    }

    return Any();
}
void ConfigurationAccess_UICommand::impl_fill(const Reference< XNameAccess >& _xConfigAccess,sal_Bool _bPopup,
                                                std::vector< ::rtl::OUString >& aImageCommandVector,
                                                std::vector< ::rtl::OUString >& aImageRotateVector,
                                                std::vector< ::rtl::OUString >& aImageMirrorVector)
{
    if ( _xConfigAccess.is() )
    {
        Sequence< ::rtl::OUString> aNameSeq = _xConfigAccess->getElementNames();
        const sal_Int32 nCount = aNameSeq.getLength();
        for ( sal_Int32 i = 0; i < nCount; i++ )
        {
            try
            {
                Reference< XNameAccess > xNameAccess(_xConfigAccess->getByName( aNameSeq[i] ),UNO_QUERY);
                if ( xNameAccess.is() )
                {
                    CmdToInfoMap aCmdToInfo;

                    aCmdToInfo.bPopup = _bPopup;
                    xNameAccess->getByName( m_aPropUILabel )        >>= aCmdToInfo.aLabel;
                    xNameAccess->getByName( m_aPropUIContextLabel ) >>= aCmdToInfo.aContextLabel;
                    xNameAccess->getByName( m_aPropProperties )     >>= aCmdToInfo.nProperties;

                    m_aCmdInfoCache.insert( CommandToInfoCache::value_type( aNameSeq[i], aCmdToInfo ));

                    if ( aCmdToInfo.nProperties & COMMAND_PROPERTY_IMAGE )
                        aImageCommandVector.push_back( aNameSeq[i] );
                    if ( aCmdToInfo.nProperties & COMMAND_PROPERTY_ROTATE )
                        aImageRotateVector.push_back( aNameSeq[i] );
                    if ( aCmdToInfo.nProperties & COMMAND_PROPERTY_MIRROR )
                        aImageMirrorVector.push_back( aNameSeq[i] );
                }
            }
            catch (const com::sun::star::lang::WrappedTargetException&)
            {
            }
            catch (const com::sun::star::container::NoSuchElementException&)
            {
            }
        }
    }
}
sal_Bool ConfigurationAccess_UICommand::fillCache()
{
    RTL_LOGFILE_CONTEXT( aLog, "framework (cd100003) ::ConfigurationAccess_UICommand::fillCache" );

    if ( m_bCacheFilled )
        return sal_True;

    std::vector< ::rtl::OUString > aImageCommandVector;
    std::vector< ::rtl::OUString > aImageRotateVector;
    std::vector< ::rtl::OUString > aImageMirrorVector;

    impl_fill(m_xConfigAccess,sal_False,aImageCommandVector,aImageRotateVector,aImageMirrorVector);
    impl_fill(m_xConfigAccessPopups,sal_True,aImageCommandVector,aImageRotateVector,aImageMirrorVector);
    // Create cached sequences for fast retrieving
    m_aCommandImageList       = comphelper::containerToSequence( aImageCommandVector );
    m_aCommandRotateImageList = comphelper::containerToSequence( aImageRotateVector );
    m_aCommandMirrorImageList = comphelper::containerToSequence( aImageMirrorVector );

    m_bCacheFilled = sal_True;

    return sal_True;
}

sal_Bool ConfigurationAccess_UICommand::addGenericInfoToCache()
{
    if ( m_xGenericUICommands.is() && !m_bGenericDataRetrieved )
    {
        Sequence< rtl::OUString > aCommandNameSeq;
        try
        {
            if ( m_xGenericUICommands->getByName(
                    rtl::OUString( UICOMMANDDESCRIPTION_NAMEACCESS_COMMANDROTATEIMAGELIST )) >>= aCommandNameSeq )
                m_aCommandRotateImageList = comphelper::concatSequences< rtl::OUString >( m_aCommandRotateImageList, aCommandNameSeq );
        }
        catch (const RuntimeException&)
        {
            throw;
        }
        catch (const Exception&)
        {
        }

        try
        {
            if ( m_xGenericUICommands->getByName(
                    rtl::OUString( UICOMMANDDESCRIPTION_NAMEACCESS_COMMANDMIRRORIMAGELIST )) >>= aCommandNameSeq )
                m_aCommandMirrorImageList = comphelper::concatSequences< rtl::OUString >( m_aCommandMirrorImageList, aCommandNameSeq );
        }
        catch (const RuntimeException&)
        {
            throw;
        }
        catch (const Exception&)
        {
        }

        m_bGenericDataRetrieved = sal_True;
    }

    return sal_True;
}

Any ConfigurationAccess_UICommand::getInfoFromCommand( const rtl::OUString& rCommandURL )
{
    Any a;

    try
    {
        a = getSequenceFromCache( rCommandURL );
        if ( !a.hasValue() )
        {
            // First try to ask our global commands configuration access. It also caches maybe
            // we find the entry in its cache first.
            if ( m_xGenericUICommands.is() && m_xGenericUICommands->hasByName( rCommandURL ) )
            {
                try
                {
                    return m_xGenericUICommands->getByName( rCommandURL );
                }
                catch (const com::sun::star::lang::WrappedTargetException&)
                {
                }
                catch (const com::sun::star::container::NoSuchElementException&)
                {
                }
            }
        }
    }
    catch (const com::sun::star::container::NoSuchElementException&)
    {
    }
    catch (const com::sun::star::lang::WrappedTargetException&)
    {
    }

    return a;
}

Sequence< rtl::OUString > ConfigurationAccess_UICommand::getAllCommands()
{
    // SAFE
    ResetableGuard aLock( m_aLock );

    if ( !m_bConfigAccessInitialized )
    {
        initializeConfigAccess();
        m_bConfigAccessInitialized = sal_True;
        fillCache();
    }

    if ( m_xConfigAccess.is() )
    {
        Reference< XNameAccess > xNameAccess;

        try
        {
            Sequence< ::rtl::OUString > aNameSeq = m_xConfigAccess->getElementNames();

            if ( m_xGenericUICommands.is() )
            {
                // Create concat list of supported user interface commands of the module
                Sequence< ::rtl::OUString > aGenericNameSeq = m_xGenericUICommands->getElementNames();
                sal_uInt32 nCount1 = aNameSeq.getLength();
                sal_uInt32 nCount2 = aGenericNameSeq.getLength();

                aNameSeq.realloc( nCount1 + nCount2 );
                ::rtl::OUString* pNameSeq = aNameSeq.getArray();
                const ::rtl::OUString* pGenericSeq = aGenericNameSeq.getConstArray();
                for ( sal_uInt32 i = 0; i < nCount2; i++ )
                    pNameSeq[nCount1+i] = pGenericSeq[i];
            }

            return aNameSeq;
        }
        catch (const com::sun::star::container::NoSuchElementException&)
        {
        }
        catch (const com::sun::star::lang::WrappedTargetException&)
        {
        }
    }

    return Sequence< rtl::OUString >();
}

sal_Bool ConfigurationAccess_UICommand::initializeConfigAccess()
{
    Sequence< Any > aArgs( 1 );
    PropertyValue   aPropValue;

    try
    {
        aPropValue.Name  = rtl::OUString( "nodepath" );
        aPropValue.Value <<= m_aConfigCmdAccess;
        aArgs[0] <<= aPropValue;

        m_xConfigAccess = Reference< XNameAccess >( m_xConfigProvider->createInstanceWithArguments(SERVICENAME_CFGREADACCESS,aArgs ),UNO_QUERY );
        if ( m_xConfigAccess.is() )
        {
            // Add as container listener
            Reference< XContainer > xContainer( m_xConfigAccess, UNO_QUERY );
            if ( xContainer.is() )
            {
                m_xConfigListener = new WeakContainerListener(this);
                xContainer->addContainerListener(m_xConfigListener);
            }
        }

        aPropValue.Value <<= m_aConfigPopupAccess;
        aArgs[0] <<= aPropValue;
        m_xConfigAccessPopups = Reference< XNameAccess >( m_xConfigProvider->createInstanceWithArguments(SERVICENAME_CFGREADACCESS,aArgs ),UNO_QUERY );
        if ( m_xConfigAccessPopups.is() )
        {
            // Add as container listener
            Reference< XContainer > xContainer( m_xConfigAccessPopups, UNO_QUERY );
            if ( xContainer.is() )
            {
                m_xConfigAccessListener = new WeakContainerListener(this);
                xContainer->addContainerListener(m_xConfigAccessListener);
            }
        }

        return sal_True;
    }
    catch (const WrappedTargetException&)
    {
    }
    catch (const Exception&)
    {
    }

    return sal_False;
}

// container.XContainerListener
void SAL_CALL ConfigurationAccess_UICommand::elementInserted( const ContainerEvent& ) throw(RuntimeException)
{
    ResetableGuard aLock( m_aLock );
    m_bCacheFilled = sal_False;
    fillCache();
}

void SAL_CALL ConfigurationAccess_UICommand::elementRemoved( const ContainerEvent& ) throw(RuntimeException)
{
    ResetableGuard aLock( m_aLock );
    m_bCacheFilled = sal_False;
    fillCache();
}

void SAL_CALL ConfigurationAccess_UICommand::elementReplaced( const ContainerEvent& ) throw(RuntimeException)
{
    ResetableGuard aLock( m_aLock );
    m_bCacheFilled = sal_False;
    fillCache();
}

// lang.XEventListener
void SAL_CALL ConfigurationAccess_UICommand::disposing( const EventObject& aEvent ) throw(RuntimeException)
{
    // SAFE
    // remove our reference to the config access
    ResetableGuard aLock( m_aLock );

    Reference< XInterface > xIfac1( aEvent.Source, UNO_QUERY );
    Reference< XInterface > xIfac2( m_xConfigAccess, UNO_QUERY );
    if ( xIfac1 == xIfac2 )
        m_xConfigAccess.clear();
    else
    {
        xIfac2 = Reference< XInterface >( m_xConfigAccessPopups, UNO_QUERY );
        if ( xIfac1 == xIfac2 )
            m_xConfigAccessPopups.clear();
    }
}

//*****************************************************************************************************************
//  XInterface, XTypeProvider, XServiceInfo
//*****************************************************************************************************************
DEFINE_XSERVICEINFO_ONEINSTANCESERVICE  (   UICommandDescription                    ,
                                            ::cppu::OWeakObject                     ,
                                            SERVICENAME_UICOMMANDDESCRIPTION        ,
                                            IMPLEMENTATIONNAME_UICOMMANDDESCRIPTION
                                        )

DEFINE_INIT_SERVICE                     (   UICommandDescription, {} )

UICommandDescription::UICommandDescription( const Reference< XMultiServiceFactory >& xServiceManager ) :
    ThreadHelpBase(),
    m_aPrivateResourceURL( PRIVATE_RESOURCE_URL ),
    m_xServiceManager( xServiceManager )
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "framework", "Ocke.Janssen@sun.com", "UICommandDescription::UICommandDescription" );
    Reference< XNameAccess > xEmpty;
    rtl::OUString aGenericUICommand( "GenericCommands" );
    m_xGenericUICommands = new ConfigurationAccess_UICommand( aGenericUICommand, xEmpty, xServiceManager );

    impl_fillElements("ooSetupFactoryCommandConfigRef");

    // insert generic commands
    UICommandsHashMap::iterator pIter = m_aUICommandsHashMap.find( aGenericUICommand );
    if ( pIter != m_aUICommandsHashMap.end() )
        pIter->second = m_xGenericUICommands;
}
UICommandDescription::UICommandDescription( const Reference< XMultiServiceFactory >& xServiceManager,bool ) :
    ThreadHelpBase(),
    m_xServiceManager( xServiceManager )
{
}
UICommandDescription::~UICommandDescription()
{
    ResetableGuard aLock( m_aLock );
    m_aModuleToCommandFileMap.clear();
    m_aUICommandsHashMap.clear();
    m_xGenericUICommands.clear();
}
void UICommandDescription::impl_fillElements(const sal_Char* _pName)
{
    m_xModuleManager.set( m_xServiceManager->createInstance( SERVICENAME_MODULEMANAGER ),UNO_QUERY );
    Reference< XNameAccess > xNameAccess( m_xModuleManager, UNO_QUERY_THROW );
    Sequence< rtl::OUString > aElementNames = xNameAccess->getElementNames();
    Sequence< PropertyValue > aSeq;
    ::rtl::OUString                  aModuleIdentifier;

    for ( sal_Int32 i = 0; i < aElementNames.getLength(); i++ )
    {
        aModuleIdentifier = aElementNames[i];
        if ( xNameAccess->getByName( aModuleIdentifier ) >>= aSeq )
        {
            ::rtl::OUString aCommandStr;
            for ( sal_Int32 y = 0; y < aSeq.getLength(); y++ )
            {
                if ( aSeq[y].Name.equalsAscii(_pName) )
                {
                    aSeq[y].Value >>= aCommandStr;
                    break;
                }
            }

            // Create first mapping ModuleIdentifier ==> Command File
            m_aModuleToCommandFileMap.insert( ModuleToCommandFileMap::value_type( aModuleIdentifier, aCommandStr ));

            // Create second mapping Command File ==> commands instance
            UICommandsHashMap::iterator pIter = m_aUICommandsHashMap.find( aCommandStr );
            if ( pIter == m_aUICommandsHashMap.end() )
                m_aUICommandsHashMap.insert( UICommandsHashMap::value_type( aCommandStr, Reference< XNameAccess >() ));
        }
    } // for ( sal_Int32 i = 0; i < aElementNames.getLength(); i++ )
}
Reference< XNameAccess > UICommandDescription::impl_createConfigAccess(const ::rtl::OUString& _sName)
{
    return new ConfigurationAccess_UICommand( _sName,m_xGenericUICommands,m_xServiceManager );
}

Any SAL_CALL UICommandDescription::getByName( const ::rtl::OUString& aName )
throw (::com::sun::star::container::NoSuchElementException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "framework", "Ocke.Janssen@sun.com", "UICommandDescription::getByName" );
    Any a;

    ResetableGuard aLock( m_aLock );

    ModuleToCommandFileMap::const_iterator pM2CIter = m_aModuleToCommandFileMap.find( aName );
    if ( pM2CIter != m_aModuleToCommandFileMap.end() )
    {
        ::rtl::OUString aCommandFile( pM2CIter->second );
        UICommandsHashMap::iterator pIter = m_aUICommandsHashMap.find( aCommandFile );
        if ( pIter != m_aUICommandsHashMap.end() )
        {
            if ( pIter->second.is() )
                a <<= pIter->second;
            else
            {
                Reference< XNameAccess > xUICommands;
                ConfigurationAccess_UICommand* pUICommands = new ConfigurationAccess_UICommand( aCommandFile,
                                                                                               m_xGenericUICommands,
                                                                                               m_xServiceManager );
                xUICommands = Reference< XNameAccess >( static_cast< cppu::OWeakObject* >( pUICommands ),UNO_QUERY );
                pIter->second = xUICommands;
                a <<= xUICommands;
            }
        }
    }
    else if ( !m_aPrivateResourceURL.isEmpty() && aName.indexOf( m_aPrivateResourceURL ) == 0 )
    {
        // special keys to retrieve information about a set of commands
        return m_xGenericUICommands->getByName( aName );
    }
    else
    {
        throw NoSuchElementException();
    }

    return a;
}

Sequence< ::rtl::OUString > SAL_CALL UICommandDescription::getElementNames()
throw (::com::sun::star::uno::RuntimeException)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "framework", "Ocke.Janssen@sun.com", "UICommandDescription::getElementNames" );
    ResetableGuard aLock( m_aLock );

    Sequence< rtl::OUString > aSeq( m_aModuleToCommandFileMap.size() );

    sal_Int32 n = 0;
    ModuleToCommandFileMap::const_iterator pIter = m_aModuleToCommandFileMap.begin();
    while ( pIter != m_aModuleToCommandFileMap.end() )
    {
        aSeq[n] = pIter->first;
        ++pIter;
    }

    return aSeq;
}

sal_Bool SAL_CALL UICommandDescription::hasByName( const ::rtl::OUString& aName )
throw (::com::sun::star::uno::RuntimeException)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "framework", "Ocke.Janssen@sun.com", "UICommandDescription::hasByName" );
    ResetableGuard aLock( m_aLock );

    ModuleToCommandFileMap::const_iterator pIter = m_aModuleToCommandFileMap.find( aName );
    return ( pIter != m_aModuleToCommandFileMap.end() );
}

// XElementAccess
Type SAL_CALL UICommandDescription::getElementType()
throw (::com::sun::star::uno::RuntimeException)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "framework", "Ocke.Janssen@sun.com", "UICommandDescription::getElementType" );
    return( ::getCppuType( (const Reference< XNameAccess >*)NULL ) );
}

sal_Bool SAL_CALL UICommandDescription::hasElements()
throw (::com::sun::star::uno::RuntimeException)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "framework", "Ocke.Janssen@sun.com", "UICommandDescription::hasElements" );
    // generic UI commands are always available!
    return sal_True;
}

} // namespace framework

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
