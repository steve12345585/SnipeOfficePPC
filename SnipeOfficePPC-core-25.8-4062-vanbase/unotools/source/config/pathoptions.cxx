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

#include <unotools/pathoptions.hxx>
#include <unotools/configitem.hxx>
#include <unotools/configmgr.hxx>
#include <tools/urlobj.hxx>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Sequence.hxx>
#include <osl/mutex.hxx>
#include <osl/file.hxx>
#include <unotools/localfilehelper.hxx>
#include <unotools/bootstrap.hxx>

#include <unotools/ucbhelper.hxx>
#include <comphelper/processfactory.hxx>
#include <comphelper/componentcontext.hxx>
#include <com/sun/star/beans/XFastPropertySet.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/PropertyAttribute.hpp>
#include <com/sun/star/beans/XPropertySetInfo.hpp>
#include <com/sun/star/util/PathSubstitution.hpp>
#include <com/sun/star/util/XStringSubstitution.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/util/XMacroExpander.hpp>
#include <rtl/instance.hxx>

#include <itemholder1.hxx>

#include <vector>
#include <boost/unordered_map.hpp>

using namespace osl;
using namespace utl;
using namespace com::sun::star::uno;
using namespace com::sun::star::beans;
using namespace com::sun::star::util;
using namespace com::sun::star::lang;

using ::rtl::OUString;
using ::rtl::OString;
using ::rtl::OStringToOUString;

#define SEARCHPATH_DELIMITER  ';'
#define SIGN_STARTVARIABLE    OUString( "$("  )
#define SIGN_ENDVARIABLE      OUString( ")" )

// Supported variables by the old SvtPathOptions implementation
#define SUBSTITUTE_INSTPATH   "$(instpath)"
#define SUBSTITUTE_PROGPATH   "$(progpath)"
#define SUBSTITUTE_USERPATH   "$(userpath)"
#define SUBSTITUTE_PATH       "$(path)"

#define STRPOS_NOTFOUND       -1

struct OUStringHashCode
{
    size_t operator()( const ::rtl::OUString& sString ) const
    {
        return sString.hashCode();
    }
};

enum VarNameProperty
{
    VAR_NEEDS_SYSTEM_PATH,
    VAR_NEEDS_FILEURL
};

class NameToHandleMap : public ::boost::unordered_map<  ::rtl::OUString, sal_Int32, OUStringHashCode, ::std::equal_to< ::rtl::OUString > >
{
    public:
        inline void free() { NameToHandleMap().swap( *this ); }
};

class EnumToHandleMap : public ::boost::unordered_map< sal_Int32, sal_Int32, boost::hash< sal_Int32 >, std::equal_to< sal_Int32 > >
{
    public:
        inline void free() { EnumToHandleMap().swap( *this ); }
};

class VarNameToEnumMap : public ::boost::unordered_map< OUString, VarNameProperty, OUStringHashCode, ::std::equal_to< OUString > >
{
    public:
        inline void free() { VarNameToEnumMap().swap( *this ); }
};


// class SvtPathOptions_Impl ---------------------------------------------
class SvtPathOptions_Impl
{
    private:
        // Local variables to return const references
        std::vector< String >               m_aPathArray;
        Reference< XFastPropertySet >       m_xPathSettings;
        Reference< XStringSubstitution >    m_xSubstVariables;
        Reference< XMacroExpander >         m_xMacroExpander;
        mutable EnumToHandleMap             m_aMapEnumToPropHandle;
        VarNameToEnumMap                    m_aMapVarNamesToEnum;

        ::com::sun::star::lang::Locale      m_aLocale;
        String                              m_aEmptyString;
        mutable ::osl::Mutex                m_aMutex;

    public:
                        SvtPathOptions_Impl();

        // get the paths, not const because of using a mutex
        const String&   GetPath( SvtPathOptions::Paths );
        const String&   GetAddinPath() { return GetPath( SvtPathOptions::PATH_ADDIN ); }
        const String&   GetAutoCorrectPath() { return GetPath( SvtPathOptions::PATH_AUTOCORRECT ); }
        const String&   GetAutoTextPath() { return GetPath( SvtPathOptions::PATH_AUTOTEXT ); }
        const String&   GetBackupPath() { return GetPath( SvtPathOptions::PATH_BACKUP ); }
        const String&   GetBasicPath() { return GetPath( SvtPathOptions::PATH_BASIC ); }
        const String&   GetBitmapPath() { return GetPath( SvtPathOptions::PATH_BITMAP ); }
        const String&   GetConfigPath() { return GetPath( SvtPathOptions::PATH_CONFIG ); }
        const String&   GetDictionaryPath() { return GetPath( SvtPathOptions::PATH_DICTIONARY ); }
        const String&   GetFavoritesPath() { return GetPath( SvtPathOptions::PATH_FAVORITES ); }
        const String&   GetFilterPath() { return GetPath( SvtPathOptions::PATH_FILTER ); }
        const String&   GetGalleryPath() { return GetPath( SvtPathOptions::PATH_GALLERY ); }
        const String&   GetGraphicPath() { return GetPath( SvtPathOptions::PATH_GRAPHIC ); }
        const String&   GetHelpPath() { return GetPath( SvtPathOptions::PATH_HELP ); }
        const String&   GetLinguisticPath() { return GetPath( SvtPathOptions::PATH_LINGUISTIC ); }
        const String&   GetModulePath() { return GetPath( SvtPathOptions::PATH_MODULE ); }
        const String&   GetPalettePath() { return GetPath( SvtPathOptions::PATH_PALETTE ); }
        const String&   GetPluginPath() { return GetPath( SvtPathOptions::PATH_PLUGIN ); }
        const String&   GetStoragePath() { return GetPath( SvtPathOptions::PATH_STORAGE ); }
        const String&   GetTempPath() { return GetPath( SvtPathOptions::PATH_TEMP ); }
        const String&   GetTemplatePath() { return GetPath( SvtPathOptions::PATH_TEMPLATE ); }
        const String&   GetUserConfigPath() { return GetPath( SvtPathOptions::PATH_USERCONFIG ); }
        const String&   GetWorkPath() { return GetPath( SvtPathOptions::PATH_WORK ); }
        const String&   GetUIConfigPath() { return GetPath( SvtPathOptions::PATH_UICONFIG ); }
        const String&   GetFingerprintPath() { return GetPath( SvtPathOptions::PATH_FINGERPRINT ); }

        // set the paths
        void            SetPath( SvtPathOptions::Paths, const String& rNewPath );
        void            SetAddinPath( const String& rPath ) { SetPath( SvtPathOptions::PATH_ADDIN, rPath ); }
        void            SetAutoCorrectPath( const String& rPath ) { SetPath( SvtPathOptions::PATH_AUTOCORRECT, rPath ); }
        void            SetAutoTextPath( const String& rPath ) { SetPath( SvtPathOptions::PATH_AUTOTEXT, rPath ); }
        void            SetBackupPath( const String& rPath ) { SetPath( SvtPathOptions::PATH_BACKUP, rPath ); }
        void            SetBasicPath( const String& rPath ) { SetPath( SvtPathOptions::PATH_BASIC, rPath ); }
        void            SetBitmapPath( const String& rPath ) { SetPath( SvtPathOptions::PATH_BITMAP, rPath ); }
        void            SetConfigPath( const String& rPath ) { SetPath( SvtPathOptions::PATH_CONFIG, rPath ); }
        void            SetDictionaryPath( const String& rPath ) { SetPath( SvtPathOptions::PATH_DICTIONARY, rPath ); }
        void            SetFavoritesPath( const String& rPath ) { SetPath( SvtPathOptions::PATH_FAVORITES, rPath ); }
        void            SetFilterPath( const String& rPath ) { SetPath( SvtPathOptions::PATH_FILTER, rPath ); }
        void            SetGalleryPath( const String& rPath ) { SetPath( SvtPathOptions::PATH_GALLERY, rPath ); }
        void            SetGraphicPath( const String& rPath ) { SetPath( SvtPathOptions::PATH_GRAPHIC, rPath ); }
        void            SetHelpPath( const String& rPath ) { SetPath( SvtPathOptions::PATH_HELP, rPath ); }
        void            SetLinguisticPath( const String& rPath ) { SetPath( SvtPathOptions::PATH_LINGUISTIC, rPath ); }
        void            SetModulePath( const String& rPath ) { SetPath( SvtPathOptions::PATH_MODULE, rPath ); }
        void            SetPalettePath( const String& rPath ) { SetPath( SvtPathOptions::PATH_PALETTE, rPath ); }
        void            SetPluginPath( const String& rPath ) { SetPath( SvtPathOptions::PATH_PLUGIN, rPath ); }
        void            SetStoragePath( const String& rPath ) { SetPath( SvtPathOptions::PATH_STORAGE, rPath ); }
        void            SetTempPath( const String& rPath ) { SetPath( SvtPathOptions::PATH_TEMP, rPath ); }
        void            SetTemplatePath( const String& rPath ) { SetPath( SvtPathOptions::PATH_TEMPLATE, rPath ); }
        void            SetUserConfigPath( const String& rPath ) { SetPath( SvtPathOptions::PATH_USERCONFIG, rPath ); }
        void            SetWorkPath( const String& rPath ) { SetPath( SvtPathOptions::PATH_WORK, rPath ); }

        rtl::OUString   SubstVar( const rtl::OUString& rVar ) const;
        rtl::OUString   ExpandMacros( const rtl::OUString& rPath ) const;
        rtl::OUString   UsePathVariables( const rtl::OUString& rPath ) const;

        ::com::sun::star::lang::Locale  GetLocale() const { return m_aLocale; }
};

// global ----------------------------------------------------------------

static SvtPathOptions_Impl* pOptions = NULL;
static sal_Int32 nRefCount = 0;

// functions -------------------------------------------------------------
struct PropertyStruct
{
    const char*             pPropName;  // The ascii name of the Office path
    SvtPathOptions::Paths   ePath;      // The enum value used by SvtPathOptions
};

struct VarNameAttribute
{
    const char*             pVarName;       // The name of the path variable
    VarNameProperty         eVarProperty;   // Which return value is needed by this path variable
};

static PropertyStruct aPropNames[] =
{
    { "Addin",          SvtPathOptions::PATH_ADDIN          },
    { "AutoCorrect",    SvtPathOptions::PATH_AUTOCORRECT    },
    { "AutoText",       SvtPathOptions::PATH_AUTOTEXT       },
    { "Backup",         SvtPathOptions::PATH_BACKUP         },
    { "Basic",          SvtPathOptions::PATH_BASIC          },
    { "Bitmap",         SvtPathOptions::PATH_BITMAP         },
    { "Config",         SvtPathOptions::PATH_CONFIG         },
    { "Dictionary",     SvtPathOptions::PATH_DICTIONARY     },
    { "Favorite",       SvtPathOptions::PATH_FAVORITES      },
    { "Filter",         SvtPathOptions::PATH_FILTER         },
    { "Gallery",        SvtPathOptions::PATH_GALLERY        },
    { "Graphic",        SvtPathOptions::PATH_GRAPHIC        },
    { "Help",           SvtPathOptions::PATH_HELP           },
    { "Linguistic",     SvtPathOptions::PATH_LINGUISTIC     },
    { "Module",         SvtPathOptions::PATH_MODULE         },
    { "Palette",        SvtPathOptions::PATH_PALETTE        },
    { "Plugin",         SvtPathOptions::PATH_PLUGIN         },
    { "Storage",        SvtPathOptions::PATH_STORAGE        },
    { "Temp",           SvtPathOptions::PATH_TEMP           },
    { "Template",       SvtPathOptions::PATH_TEMPLATE       },
    { "UserConfig",     SvtPathOptions::PATH_USERCONFIG     },
    { "Work",           SvtPathOptions::PATH_WORK           },
    { "UIConfig",       SvtPathOptions::PATH_UICONFIG       },
    { "Fingerprint",    SvtPathOptions::PATH_FINGERPRINT    }
};

static VarNameAttribute aVarNameAttribute[] =
{
    { SUBSTITUTE_INSTPATH,  VAR_NEEDS_SYSTEM_PATH },    // $(instpath)
    { SUBSTITUTE_PROGPATH,  VAR_NEEDS_SYSTEM_PATH },    // $(progpath)
    { SUBSTITUTE_USERPATH,  VAR_NEEDS_SYSTEM_PATH },    // $(userpath)
    { SUBSTITUTE_PATH,      VAR_NEEDS_SYSTEM_PATH },    // $(path)
};

// class SvtPathOptions_Impl ---------------------------------------------

const String& SvtPathOptions_Impl::GetPath( SvtPathOptions::Paths ePath )
{
    if ( ePath >= SvtPathOptions::PATH_COUNT )
        return m_aEmptyString;

    ::osl::MutexGuard aGuard( m_aMutex );

    try
    {
        OUString    aPathValue;
        OUString    aResult;
        sal_Int32   nHandle = m_aMapEnumToPropHandle[ (sal_Int32)ePath ];

        // Substitution is done by the service itself using the substition service
        Any         a = m_xPathSettings->getFastPropertyValue( nHandle );
        a >>= aPathValue;
        if( ePath == SvtPathOptions::PATH_ADDIN     ||
            ePath == SvtPathOptions::PATH_FILTER    ||
            ePath == SvtPathOptions::PATH_HELP      ||
            ePath == SvtPathOptions::PATH_MODULE    ||
            ePath == SvtPathOptions::PATH_PLUGIN    ||
            ePath == SvtPathOptions::PATH_STORAGE
          )
        {
            // These office paths have to be converted to system pates
            utl::LocalFileHelper::ConvertURLToPhysicalName( aPathValue, aResult );
            aPathValue = aResult;
        }

        m_aPathArray[ ePath ] = aPathValue;
        return m_aPathArray[ ePath ];
    }
    catch (UnknownPropertyException &)
    {
    }

    return m_aEmptyString;
}

void SvtPathOptions_Impl::SetPath( SvtPathOptions::Paths ePath, const String& rNewPath )
{
    ::osl::MutexGuard aGuard( m_aMutex );

    if ( ePath < SvtPathOptions::PATH_COUNT )
    {
        OUString    aResult;
        OUString    aNewValue;
        Any         a;

        switch ( ePath )
        {
            case SvtPathOptions::PATH_ADDIN:
            case SvtPathOptions::PATH_FILTER:
            case SvtPathOptions::PATH_HELP:
            case SvtPathOptions::PATH_MODULE:
            case SvtPathOptions::PATH_PLUGIN:
            case SvtPathOptions::PATH_STORAGE:
            {
                // These office paths have to be convert back to UCB-URL's
                utl::LocalFileHelper::ConvertPhysicalNameToURL( rNewPath, aResult );
                aNewValue = aResult;
            }
            break;

            default:
                aNewValue = rNewPath;
        }

        // Resubstitution is done by the service itself using the substition service
        a <<= aNewValue;
        try
        {
            m_xPathSettings->setFastPropertyValue( m_aMapEnumToPropHandle[ (sal_Int32)ePath], a );
        }
        catch (const Exception&)
        {
        }
    }
}

//-------------------------------------------------------------------------

OUString SvtPathOptions_Impl::ExpandMacros( const rtl::OUString& rPath ) const
{
    ::rtl::OUString sExpanded( rPath );

    const INetURLObject aParser( rPath );
    if ( aParser.GetProtocol() == INET_PROT_VND_SUN_STAR_EXPAND )
        sExpanded = m_xMacroExpander->expandMacros( aParser.GetURLPath( INetURLObject::DECODE_WITH_CHARSET ) );

    return sExpanded;
}

//-------------------------------------------------------------------------

OUString SvtPathOptions_Impl::UsePathVariables( const OUString& rPath ) const
{
    return m_xSubstVariables->reSubstituteVariables( rPath );
}

// -----------------------------------------------------------------------

OUString SvtPathOptions_Impl::SubstVar( const OUString& rVar ) const
{
    // Don't work at parameter-string directly. Copy it.
    OUString aWorkText = rVar;

    // Convert the returned path to system path!
    sal_Bool bConvertLocal = sal_False;

    // Search for first occure of "$(...".
    sal_Int32 nPosition = aWorkText.indexOf( SIGN_STARTVARIABLE );  // = first position of "$(" in string
    sal_Int32 nLength   = 0;                                        // = count of letters from "$(" to ")" in string

    // Have we found any variable like "$(...)"?
    if ( nPosition != STRPOS_NOTFOUND )
    {
        // Yes; Get length of found variable.
        // If no ")" was found - nLength is set to 0 by default! see before.
        sal_Int32 nEndPosition = aWorkText.indexOf( SIGN_ENDVARIABLE, nPosition );
        if ( nEndPosition != STRPOS_NOTFOUND )
            nLength = nEndPosition - nPosition + 1;
    }

    // Is there another path variable?
    while ( ( nPosition != STRPOS_NOTFOUND ) && ( nLength > 0 ) )
    {
        // YES; Get the next variable for replace.
        OUString aSubString = aWorkText.copy( nPosition, nLength );
        aSubString = aSubString.toAsciiLowerCase();

        // Look for special variable that needs a system path.
        VarNameToEnumMap::const_iterator pIter = m_aMapVarNamesToEnum.find( aSubString );
        if ( pIter != m_aMapVarNamesToEnum.end() )
            bConvertLocal = sal_True;

        nPosition += nLength;

        // We must control index in string before call something at OUString!
        // The OUString-implementation don't do it for us :-( but the result is not defined otherwise.
        if ( nPosition + 1 > aWorkText.getLength() )
        {
            // Position is out of range. Break loop!
            nPosition = STRPOS_NOTFOUND;
            nLength = 0;
        }
        else
        {
            // Else; Position is valid. Search for next variable.
            nPosition = aWorkText.indexOf( SIGN_STARTVARIABLE, nPosition );
            // Have we found any variable like "$(...)"?
            if ( nPosition != STRPOS_NOTFOUND )
            {
                // Yes; Get length of found variable. If no ")" was found - nLength must set to 0!
                nLength = 0;
                sal_Int32 nEndPosition = aWorkText.indexOf( SIGN_ENDVARIABLE, nPosition );
                if ( nEndPosition != STRPOS_NOTFOUND )
                    nLength = nEndPosition - nPosition + 1;
            }
        }
    }

    aWorkText = m_xSubstVariables->substituteVariables( rVar, sal_False );

    if ( bConvertLocal )
    {
        // Convert the URL to a system path for special path variables
        rtl::OUString aReturn;
        utl::LocalFileHelper::ConvertURLToPhysicalName( aWorkText, aReturn );
        return aReturn;
    }

    return aWorkText;
}

// -----------------------------------------------------------------------

SvtPathOptions_Impl::SvtPathOptions_Impl() :
    m_aPathArray( (sal_Int32)SvtPathOptions::PATH_COUNT )
{
    Reference< XMultiServiceFactory > xSMgr = comphelper::getProcessServiceFactory();

    // Create necessary services
    m_xPathSettings = Reference< XFastPropertySet >( xSMgr->createInstance(
                                                    ::rtl::OUString( "com.sun.star.util.PathSettings" )),
                                                UNO_QUERY );
    if ( !m_xPathSettings.is() )
    {
        // #112719#: check for existance
        OSL_FAIL( "SvtPathOptions_Impl::SvtPathOptions_Impl(): #112719# happened again!" );
        throw RuntimeException(
            ::rtl::OUString( "Service com.sun.star.util.PathSettings cannot be created" ),
            Reference< XInterface >() );
    }

    ::comphelper::ComponentContext aContext( xSMgr );
    m_xSubstVariables.set( PathSubstitution::create(aContext.getUNOContext()) );
    m_xMacroExpander.set( aContext.getSingleton( "com.sun.star.util.theMacroExpander" ), UNO_QUERY_THROW );

    // Create temporary hash map to have a mapping between property names and property handles
    Reference< XPropertySet > xPropertySet = Reference< XPropertySet >( m_xPathSettings, UNO_QUERY );
    Reference< XPropertySetInfo > xPropSetInfo = xPropertySet->getPropertySetInfo();
    Sequence< Property > aPathPropSeq = xPropSetInfo->getProperties();

    NameToHandleMap aTempHashMap;
    for ( sal_Int32 n = 0; n < aPathPropSeq.getLength(); n++ )
    {
        const com::sun::star::beans::Property& aProperty = aPathPropSeq[n];
        aTempHashMap.insert( NameToHandleMap::value_type( aProperty.Name, aProperty.Handle ));
    }

    // Create mapping between internal enum (SvtPathOptions::Paths) and property handle
    sal_Int32 nCount = sizeof( aPropNames ) / sizeof( PropertyStruct );
    sal_Int32 i;
    for ( i = 0; i < nCount; i++ )
    {
        NameToHandleMap::const_iterator pIter =
            aTempHashMap.find( rtl::OUString::createFromAscii( aPropNames[i].pPropName ));

        if ( pIter != aTempHashMap.end() )
        {
            sal_Int32 nHandle   = pIter->second;
            sal_Int32 nEnum     = aPropNames[i].ePath;
            m_aMapEnumToPropHandle.insert( EnumToHandleMap::value_type( nEnum, nHandle ));
        }
    }

    // Create hash map for path variables that need a system path as a return value!
    nCount = sizeof( aVarNameAttribute ) / sizeof( VarNameAttribute );
    for ( i = 0; i < nCount; i++ )
    {
        m_aMapVarNamesToEnum.insert( VarNameToEnumMap::value_type(
                OUString::createFromAscii( aVarNameAttribute[i].pVarName ),
                aVarNameAttribute[i].eVarProperty ));
    }

    // Set language type!
    OUString aLocaleStr( ConfigManager::getLocale() );
    sal_Int32 nIndex = 0;
    m_aLocale.Language = aLocaleStr.getToken(0, '-', nIndex );
    m_aLocale.Country = aLocaleStr.getToken(0, '-', nIndex );
    m_aLocale.Variant = aLocaleStr.getToken(0, '-', nIndex );
}

// -----------------------------------------------------------------------

// class SvtPathOptions --------------------------------------------------

namespace { struct lclMutex : public rtl::Static< ::osl::Mutex, lclMutex > {}; }

SvtPathOptions::SvtPathOptions()
{
    // Global access, must be guarded (multithreading)
    ::osl::MutexGuard aGuard( lclMutex::get() );
    if ( !pOptions )
    {
        pOptions = new SvtPathOptions_Impl;
        ItemHolder1::holdConfigItem(E_PATHOPTIONS);
    }
    ++nRefCount;
    pImp = pOptions;
}

// -----------------------------------------------------------------------

SvtPathOptions::~SvtPathOptions()
{
    // Global access, must be guarded (multithreading)
    ::osl::MutexGuard aGuard( lclMutex::get() );
    if ( !--nRefCount )
    {
        DELETEZ( pOptions );
    }
}

// -----------------------------------------------------------------------

const String& SvtPathOptions::GetAddinPath() const
{
    return pImp->GetAddinPath();
}

// -----------------------------------------------------------------------

const String& SvtPathOptions::GetAutoCorrectPath() const
{
    return pImp->GetAutoCorrectPath();
}

// -----------------------------------------------------------------------

const String& SvtPathOptions::GetAutoTextPath() const
{
    return pImp->GetAutoTextPath();
}

// -----------------------------------------------------------------------

const String& SvtPathOptions::GetBackupPath() const
{
    return pImp->GetBackupPath();
}

// -----------------------------------------------------------------------

const String& SvtPathOptions::GetBasicPath() const
{
    return pImp->GetBasicPath();
}

// -----------------------------------------------------------------------

const String& SvtPathOptions::GetBitmapPath() const
{
    return pImp->GetBitmapPath();
}

// -----------------------------------------------------------------------

const String& SvtPathOptions::GetConfigPath() const
{
    return pImp->GetConfigPath();
}

// -----------------------------------------------------------------------

const String& SvtPathOptions::GetDictionaryPath() const
{
    return pImp->GetDictionaryPath();
}

// -----------------------------------------------------------------------

const String& SvtPathOptions::GetFavoritesPath() const
{
    return pImp->GetFavoritesPath();
}

// -----------------------------------------------------------------------

const String& SvtPathOptions::GetFilterPath() const
{
    return pImp->GetFilterPath();
}

// -----------------------------------------------------------------------

const String& SvtPathOptions::GetGalleryPath() const
{
    return pImp->GetGalleryPath();
}

// -----------------------------------------------------------------------

const String& SvtPathOptions::GetGraphicPath() const
{
    return pImp->GetGraphicPath();
}

// -----------------------------------------------------------------------

const String& SvtPathOptions::GetHelpPath() const
{
    return pImp->GetHelpPath();
}

// -----------------------------------------------------------------------

const String& SvtPathOptions::GetLinguisticPath() const
{
    return pImp->GetLinguisticPath();
}

// -----------------------------------------------------------------------

const String& SvtPathOptions::GetFingerprintPath() const
{
    return pImp->GetFingerprintPath();
}

// -----------------------------------------------------------------------

const String& SvtPathOptions::GetModulePath() const
{
    return pImp->GetModulePath();
}

// -----------------------------------------------------------------------

const String& SvtPathOptions::GetPalettePath() const
{
    return pImp->GetPalettePath();
}

// -----------------------------------------------------------------------

const String& SvtPathOptions::GetPluginPath() const
{
    return pImp->GetPluginPath();
}

// -----------------------------------------------------------------------

const String& SvtPathOptions::GetStoragePath() const
{
    return pImp->GetStoragePath();
}

// -----------------------------------------------------------------------

const String& SvtPathOptions::GetTempPath() const
{
    return pImp->GetTempPath();
}

// -----------------------------------------------------------------------

const String& SvtPathOptions::GetTemplatePath() const
{
    return pImp->GetTemplatePath();
}

// -----------------------------------------------------------------------

const String& SvtPathOptions::GetUserConfigPath() const
{
    return pImp->GetUserConfigPath();
}

const String& SvtPathOptions::GetUIConfigPath() const
{
    return pImp->GetUIConfigPath();
}

// -----------------------------------------------------------------------

const String& SvtPathOptions::GetWorkPath() const
{
    return pImp->GetWorkPath();
}

// -----------------------------------------------------------------------

void SvtPathOptions::SetAddinPath( const String& rPath )
{
    pImp->SetAddinPath( rPath );
}

// -----------------------------------------------------------------------

void SvtPathOptions::SetAutoCorrectPath( const String& rPath )
{
    pImp->SetAutoCorrectPath( rPath );
}

// -----------------------------------------------------------------------

void SvtPathOptions::SetAutoTextPath( const String& rPath )
{
    pImp->SetAutoTextPath( rPath );
}

// -----------------------------------------------------------------------

void SvtPathOptions::SetBackupPath( const String& rPath )
{
    pImp->SetBackupPath( rPath );
}

// -----------------------------------------------------------------------

void SvtPathOptions::SetBasicPath( const String& rPath )
{
    pImp->SetBasicPath( rPath );
}

// -----------------------------------------------------------------------

void SvtPathOptions::SetBitmapPath( const String& rPath )
{
    pImp->SetBitmapPath( rPath );
}

// -----------------------------------------------------------------------

void SvtPathOptions::SetConfigPath( const String& rPath )
{
    pImp->SetConfigPath( rPath );
}

// -----------------------------------------------------------------------

void SvtPathOptions::SetDictionaryPath( const String& rPath )
{
    pImp->SetDictionaryPath( rPath );
}

// -----------------------------------------------------------------------

void SvtPathOptions::SetFavoritesPath( const String& rPath )
{
    pImp->SetFavoritesPath( rPath );
}

// -----------------------------------------------------------------------

void SvtPathOptions::SetFilterPath( const String& rPath )
{
    pImp->SetFilterPath( rPath );
}

// -----------------------------------------------------------------------

void SvtPathOptions::SetGalleryPath( const String& rPath )
{
    pImp->SetGalleryPath( rPath );
}

// -----------------------------------------------------------------------

void SvtPathOptions::SetGraphicPath( const String& rPath )
{
    pImp->SetGraphicPath( rPath );
}

// -----------------------------------------------------------------------

void SvtPathOptions::SetHelpPath( const String& rPath )
{
    pImp->SetHelpPath( rPath );
}

// -----------------------------------------------------------------------

void SvtPathOptions::SetLinguisticPath( const String& rPath )
{
    pImp->SetLinguisticPath( rPath );
}

// -----------------------------------------------------------------------

void SvtPathOptions::SetModulePath( const String& rPath )
{
    pImp->SetModulePath( rPath );
}

// -----------------------------------------------------------------------

void SvtPathOptions::SetPalettePath( const String& rPath )
{
    pImp->SetPalettePath( rPath );
}

// -----------------------------------------------------------------------

void SvtPathOptions::SetPluginPath( const String& rPath )
{
    pImp->SetPluginPath( rPath );
}

// -----------------------------------------------------------------------

void SvtPathOptions::SetStoragePath( const String& rPath )
{
    pImp->SetStoragePath( rPath );
}

// -----------------------------------------------------------------------

void SvtPathOptions::SetTempPath( const String& rPath )
{
    pImp->SetTempPath( rPath );
}

// -----------------------------------------------------------------------

void SvtPathOptions::SetTemplatePath( const String& rPath )
{
    pImp->SetTemplatePath( rPath );
}

// -----------------------------------------------------------------------

void SvtPathOptions::SetUserConfigPath( const String& rPath )
{
    pImp->SetUserConfigPath( rPath );
}

// -----------------------------------------------------------------------

void SvtPathOptions::SetWorkPath( const String& rPath )
{
    pImp->SetWorkPath( rPath );
}

// -----------------------------------------------------------------------

String SvtPathOptions::SubstituteVariable( const String& rVar ) const
{
    String aRet = pImp->SubstVar( rVar );
    return aRet;
}

// -----------------------------------------------------------------------

String SvtPathOptions::ExpandMacros( const String& rPath ) const
{
    return pImp->ExpandMacros( rPath );
}

// -----------------------------------------------------------------------

String SvtPathOptions::UseVariable( const String& rPath ) const
{
    String aRet = pImp->UsePathVariables( rPath );
    return aRet;
}

// -----------------------------------------------------------------------

sal_Bool SvtPathOptions::SearchFile( String& rIniFile, Paths ePath )
{
    // check parameter: empty inifile name?
    if ( !rIniFile.Len() )
    {
        SAL_WARN( "unotools.config", "SvtPathOptions::SearchFile(): invalid parameter" );
        return sal_False;
    }

    rtl::OUString aIniFile = pImp->SubstVar( rIniFile );
    sal_Bool bRet = sal_False;

    switch ( ePath )
    {
        case PATH_USERCONFIG:
        {
            // path is a URL
            bRet = sal_True;
            INetURLObject aObj( GetUserConfigPath() );

            sal_Int32 nIniIndex = 0;
            do
            {
                rtl::OUString aToken = aIniFile.getToken( 0, '/', nIniIndex );
                aObj.insertName(aToken);
            }
            while ( nIniIndex >= 0 );

            if ( !::utl::UCBContentHelper::Exists( aObj.GetMainURL( INetURLObject::NO_DECODE ) ) )
            {
                aObj.SetSmartURL( GetConfigPath() );
                aObj.insertName( aIniFile );
                bRet = ::utl::UCBContentHelper::Exists( aObj.GetMainURL( INetURLObject::NO_DECODE ) );
            }

            if ( bRet )
                rIniFile = aObj.GetMainURL( INetURLObject::NO_DECODE );

            break;
        }

        default:
        {
            rtl::OUString aPath;
            switch ( ePath )
            {
                case PATH_ADDIN:        aPath = GetAddinPath();         break;
                case PATH_AUTOCORRECT:  aPath = GetAutoCorrectPath();   break;
                case PATH_AUTOTEXT:     aPath = GetAutoTextPath();      break;
                case PATH_BACKUP:       aPath = GetBackupPath();        break;
                case PATH_BASIC:        aPath = GetBasicPath();         break;
                case PATH_BITMAP:       aPath = GetBitmapPath();        break;
                case PATH_CONFIG:       aPath = GetConfigPath();        break;
                case PATH_DICTIONARY:   aPath = GetDictionaryPath();    break;
                case PATH_FAVORITES:    aPath = GetFavoritesPath();     break;
                case PATH_FILTER:       aPath = GetFilterPath();        break;
                case PATH_GALLERY:      aPath = GetGalleryPath();       break;
                case PATH_GRAPHIC:      aPath = GetGraphicPath();       break;
                case PATH_HELP:         aPath = GetHelpPath();          break;
                case PATH_LINGUISTIC:   aPath = GetLinguisticPath();    break;
                case PATH_MODULE:       aPath = GetModulePath();        break;
                case PATH_PALETTE:      aPath = GetPalettePath();       break;
                case PATH_PLUGIN:       aPath = GetPluginPath();        break;
                case PATH_STORAGE:      aPath = GetStoragePath();       break;
                case PATH_TEMP:         aPath = GetTempPath();          break;
                case PATH_TEMPLATE:     aPath = GetTemplatePath();      break;
                case PATH_WORK:         aPath = GetWorkPath();          break;
                case PATH_UICONFIG:     aPath = GetUIConfigPath();      break;
                case PATH_FINGERPRINT:  aPath = GetFingerprintPath();   break;
                case PATH_USERCONFIG:/*-Wall???*/           break;
                case PATH_COUNT: /*-Wall???*/ break;
            }

            sal_Int32 nPathIndex = 0;
            do
            {
                sal_Bool bIsURL = sal_True;
                rtl::OUString aPathToken = aPath.getToken( 0, SEARCHPATH_DELIMITER, nPathIndex );
                INetURLObject aObj( aPathToken );
                if ( aObj.HasError() )
                {
                    bIsURL = sal_False;
                    rtl::OUString aURL;
                    if ( LocalFileHelper::ConvertPhysicalNameToURL( aPathToken, aURL ) )
                        aObj.SetURL( aURL );
                }
                if ( aObj.GetProtocol() == INET_PROT_VND_SUN_STAR_EXPAND )
                {
                    ::comphelper::ComponentContext aContext( ::comphelper::getProcessServiceFactory() );
                    Reference< XMacroExpander > xMacroExpander( aContext.getSingleton( "com.sun.star.util.theMacroExpander" ), UNO_QUERY );
                    OSL_ENSURE( xMacroExpander.is(), "SvtPathOptions::SearchFile: unable to access the MacroExpander singleton!" );
                    if ( xMacroExpander.is() )
                    {
                        const ::rtl::OUString sExpandedPath = xMacroExpander->expandMacros( aObj.GetURLPath( INetURLObject::DECODE_WITH_CHARSET ) );
                        aObj.SetURL( sExpandedPath );
                    }
                }

                sal_Int32 nIniIndex = 0;
                do
                {
                    rtl::OUString aToken = aIniFile.getToken( 0, '/', nIniIndex );
                    aObj.insertName(aToken);
                }
                while ( nIniIndex >= 0 );

                bRet = ::utl::UCBContentHelper::Exists( aObj.GetMainURL( INetURLObject::NO_DECODE ) );

                if ( bRet )
                {
                    if ( !bIsURL )
                    {
                        rtl::OUString sTmp(rIniFile);
                        ::utl::LocalFileHelper::ConvertURLToPhysicalName(
                                            aObj.GetMainURL( INetURLObject::NO_DECODE ), sTmp );
                        rIniFile = sTmp;
                    }
                    else
                        rIniFile = aObj.GetMainURL( INetURLObject::NO_DECODE );
                    break;
                }
            }
            while ( nPathIndex >= 0 );
        }
    }

    return bRet;
}

// -----------------------------------------------------------------------

::com::sun::star::lang::Locale SvtPathOptions::GetLocale() const
{
    return pImp->GetLocale();
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
