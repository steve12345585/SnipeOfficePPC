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

#include "basicmigration.hxx"
#include <tools/urlobj.hxx>
#include <unotools/bootstrap.hxx>


using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;


//.........................................................................
namespace migration
{
//.........................................................................


    #define sSourceUserBasic ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("/user/basic"))
    #define sTargetUserBasic ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("/user/__basic_80"))

    // =============================================================================
    // component operations
    // =============================================================================

    ::rtl::OUString BasicMigration_getImplementationName()
    {
        return ::rtl::OUString (RTL_CONSTASCII_USTRINGPARAM("com.sun.star.comp.desktop.migration.Basic"));
    }

    // -----------------------------------------------------------------------------

    Sequence< ::rtl::OUString > BasicMigration_getSupportedServiceNames()
    {
        Sequence< ::rtl::OUString > aNames(1);
        aNames.getArray()[0] = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.migration.Basic" ) );
        return aNames;
    }

    // =============================================================================
    // BasicMigration
    // =============================================================================

    BasicMigration::BasicMigration()
    {
    }

    // -----------------------------------------------------------------------------

    BasicMigration::~BasicMigration()
    {
    }

    // -----------------------------------------------------------------------------

    TStringVectorPtr BasicMigration::getFiles( const ::rtl::OUString& rBaseURL ) const
    {
        TStringVectorPtr aResult( new TStringVector );
        ::osl::Directory aDir( rBaseURL);

        if ( aDir.open() == ::osl::FileBase::E_None )
        {
            // iterate over directory content
            TStringVector aSubDirs;
            ::osl::DirectoryItem aItem;
            while ( aDir.getNextItem( aItem ) == ::osl::FileBase::E_None )
            {
                ::osl::FileStatus aFileStatus( osl_FileStatus_Mask_Type | osl_FileStatus_Mask_FileURL );
                if ( aItem.getFileStatus( aFileStatus ) == ::osl::FileBase::E_None )
                {
                    if ( aFileStatus.getFileType() == ::osl::FileStatus::Directory )
                        aSubDirs.push_back( aFileStatus.getFileURL() );
                    else
                        aResult->push_back( aFileStatus.getFileURL() );
                }
            }

            // iterate recursive over subfolders
            TStringVector::const_iterator aI = aSubDirs.begin();
            while ( aI != aSubDirs.end() )
            {
                TStringVectorPtr aSubResult = getFiles( *aI );
                aResult->insert( aResult->end(), aSubResult->begin(), aSubResult->end() );
                ++aI;
            }
        }

        return aResult;
    }

    // -----------------------------------------------------------------------------

    ::osl::FileBase::RC BasicMigration::checkAndCreateDirectory( INetURLObject& rDirURL )
    {
        ::osl::FileBase::RC aResult = ::osl::Directory::create( rDirURL.GetMainURL( INetURLObject::DECODE_TO_IURI ) );
        if ( aResult == ::osl::FileBase::E_NOENT )
        {
            INetURLObject aBaseURL( rDirURL );
            aBaseURL.removeSegment();
            checkAndCreateDirectory( aBaseURL );
            return ::osl::Directory::create( rDirURL.GetMainURL( INetURLObject::DECODE_TO_IURI ) );
        }
        else
        {
            return aResult;
        }
    }

    // -----------------------------------------------------------------------------

    void BasicMigration::copyFiles()
    {
        ::rtl::OUString sTargetDir;
        ::utl::Bootstrap::PathStatus aStatus = ::utl::Bootstrap::locateUserInstallation( sTargetDir );
        if ( aStatus == ::utl::Bootstrap::PATH_EXISTS )
        {
            sTargetDir += sTargetUserBasic;
            TStringVectorPtr aFileList = getFiles( m_sSourceDir );
            TStringVector::const_iterator aI = aFileList->begin();
            while ( aI != aFileList->end() )
            {
                ::rtl::OUString sLocalName = aI->copy( m_sSourceDir.getLength() );
                ::rtl::OUString sTargetName = sTargetDir + sLocalName;
                INetURLObject aURL( sTargetName );
                aURL.removeSegment();
                checkAndCreateDirectory( aURL );
                ::osl::FileBase::RC aResult = ::osl::File::copy( *aI, sTargetName );
                if ( aResult != ::osl::FileBase::E_None )
                {
                    ::rtl::OString aMsg( "BasicMigration::copyFiles: cannot copy " );
                    aMsg += ::rtl::OUStringToOString( *aI, RTL_TEXTENCODING_UTF8 ) + " to "
                         +  ::rtl::OUStringToOString( sTargetName, RTL_TEXTENCODING_UTF8 );
                    OSL_FAIL( aMsg.getStr() );
                }
                ++aI;
            }
        }
        else
        {
            OSL_FAIL( "BasicMigration::copyFiles: no user installation!" );
        }
    }

    // -----------------------------------------------------------------------------
    // XServiceInfo
    // -----------------------------------------------------------------------------

    ::rtl::OUString BasicMigration::getImplementationName() throw (RuntimeException)
    {
        return BasicMigration_getImplementationName();
    }

    // -----------------------------------------------------------------------------

    sal_Bool BasicMigration::supportsService( const ::rtl::OUString& rServiceName ) throw (RuntimeException)
    {
        Sequence< ::rtl::OUString > aNames( getSupportedServiceNames() );
        const ::rtl::OUString* pNames = aNames.getConstArray();
        const ::rtl::OUString* pEnd = pNames + aNames.getLength();
        for ( ; pNames != pEnd && !pNames->equals( rServiceName ); ++pNames )
            ;

        return pNames != pEnd;
    }

    // -----------------------------------------------------------------------------

    Sequence< ::rtl::OUString > BasicMigration::getSupportedServiceNames() throw (RuntimeException)
    {
        return BasicMigration_getSupportedServiceNames();
    }

    // -----------------------------------------------------------------------------
    // XInitialization
    // -----------------------------------------------------------------------------

    void BasicMigration::initialize( const Sequence< Any >& aArguments ) throw (Exception, RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        const Any* pIter = aArguments.getConstArray();
        const Any* pEnd = pIter + aArguments.getLength();
        for ( ; pIter != pEnd ; ++pIter )
        {
            beans::NamedValue aValue;
            *pIter >>= aValue;
            if ( aValue.Name == "UserData" )
            {
                if ( !(aValue.Value >>= m_sSourceDir) )
                {
                    OSL_FAIL( "BasicMigration::initialize: argument UserData has wrong type!" );
                }
                m_sSourceDir += sSourceUserBasic;
                break;
            }
        }
    }

    // -----------------------------------------------------------------------------
    // XJob
    // -----------------------------------------------------------------------------

    Any BasicMigration::execute( const Sequence< beans::NamedValue >& )
        throw (lang::IllegalArgumentException, Exception, RuntimeException)
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        copyFiles();

        return Any();
    }

    // =============================================================================
    // component operations
    // =============================================================================

    Reference< XInterface > SAL_CALL BasicMigration_create(
        Reference< XComponentContext > const & )
        SAL_THROW(())
    {
        return static_cast< lang::XTypeProvider * >( new BasicMigration() );
    }

    // -----------------------------------------------------------------------------

//.........................................................................
}   // namespace migration
//.........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
