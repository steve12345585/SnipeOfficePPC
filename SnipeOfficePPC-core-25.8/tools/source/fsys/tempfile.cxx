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


#include <tools/tempfile.hxx>
#include "comdep.hxx"

#include <rtl/ustring.hxx>
#include <rtl/ustrbuf.hxx>
#include <osl/file.hxx>
#include <rtl/instance.hxx>
#include <tools/time.hxx>
#include <tools/debug.hxx>
#include <stdio.h>

#ifdef UNX
#define _MAX_PATH 260
#endif

using namespace osl;

namespace { struct TempNameBase_Impl : public rtl::Static< ::rtl::OUString, TempNameBase_Impl > {}; }

struct TempFile_Impl
{
    String      aName;
    sal_Bool    bIsDirectory;
};

String GetSystemTempDir_Impl()
{
    char sBuf[_MAX_PATH];
    const char *pDir = TempDirImpl(sBuf);

    ::rtl::OString aTmpA( pDir );
    ::rtl::OUString aTmp = ::rtl::OStringToOUString( aTmpA, osl_getThreadTextEncoding() );
    rtl::OUString aRet;
    FileBase::getFileURLFromSystemPath( aTmp, aRet );
    String aName = aRet;
    if( aName.GetChar(aName.Len()-1) != '/' )
        aName += '/';
    return aName;
}

#define TMPNAME_SIZE  ( 1 + 5 + 5 + 4 + 1 )
String ConstructTempDir_Impl( const String* pParent )
{
    String aName;
    if ( pParent && pParent->Len() )
    {
        // if parent given try to use it
        rtl::OUString aTmp( *pParent );
        rtl::OUString aRet;

        // test for valid filename
        {
            ::osl::DirectoryItem aItem;
            sal_Int32 i = aRet.getLength();
            if ( aRet[i-1] == '/' )
                i--;

            if ( DirectoryItem::get( aRet.copy(0, i), aItem ) == FileBase::E_None )
                aName = aRet;
        }
    }

    if ( !aName.Len() )
    {
        // if no parent or invalid parent : use system directory
    ::rtl::OUString& rTempNameBase_Impl = TempNameBase_Impl::get();
        if ( rTempNameBase_Impl.isEmpty() )
            rTempNameBase_Impl = GetSystemTempDir_Impl();
        aName = rTempNameBase_Impl;
    }

    // Make sure that directory ends with a separator
    xub_StrLen i = aName.Len();
    if( i>0 && aName.GetChar(i-1) != '/' )
        aName += '/';

    return aName;
}

void CreateTempName_Impl( String& rName, sal_Bool bKeep, sal_Bool bDir = sal_True )
{
    // add a suitable tempname
    // Prefix can have 5 chars, leaving 3 for numbers. 26 ** 3 == 17576
    // ER 13.07.00  why not radix 36 [0-9A-Z] ?!?
    const unsigned nRadix = 26;
    String aName( rName );
    aName += String::CreateFromAscii( "sv" );

    rName.Erase();
    static unsigned long u = Time::GetSystemTicks();
    for ( unsigned long nOld = u; ++u != nOld; )
    {
        u %= (nRadix*nRadix*nRadix);
        rtl::OUString aTmp = rtl::OUStringBuffer(aName).
            append((sal_Int32)(unsigned)u, nRadix).
            append(".tmp").
            makeStringAndClear();

        if ( bDir )
        {
            FileBase::RC err = Directory::create( aTmp );
            if (  err == FileBase::E_None )
            {
                // !bKeep: only for creating a name, not a file or directory
                if ( bKeep || Directory::remove( aTmp ) == FileBase::E_None )
                    rName = aTmp;
                break;
            }
            else if ( err != FileBase::E_EXIST )
            {
                // if f.e. name contains invalid chars stop trying to create dirs
                break;
            }
        }
        else
        {
            DBG_ASSERT( bKeep, "Too expensive, use directory for creating name!" );
            File aFile( aTmp );
            FileBase::RC err = aFile.open(osl_File_OpenFlag_Create);
            if (  err == FileBase::E_None )
            {
                rName = aTmp;
                aFile.close();
                break;
            }
            else if ( err != FileBase::E_EXIST )
            {
                 // if f.e. name contains invalid chars stop trying to create files
                 break;
            }
        }
    }
}

String TempFile::CreateTempName( const String* pParent )
{
    // get correct directory
    String aName = ConstructTempDir_Impl( pParent );

    // get TempFile name with default naming scheme
    CreateTempName_Impl( aName, sal_False );

    return aName;
}

TempFile::TempFile( const String* pParent, sal_Bool bDirectory )
    : pImp( new TempFile_Impl )
    , bKillingFileEnabled( sal_False )
{
    pImp->bIsDirectory = bDirectory;

    // get correct directory
    pImp->aName = ConstructTempDir_Impl( pParent );

    // get TempFile with default naming scheme
    CreateTempName_Impl( pImp->aName, sal_True, bDirectory );
}

TempFile::TempFile( const String& rLeadingChars, const String* pExtension, const String* pParent, sal_Bool bDirectory )
    : pImp( new TempFile_Impl )
    , bKillingFileEnabled( sal_False )
{
    pImp->bIsDirectory = bDirectory;

    // get correct directory
    String aName = ConstructTempDir_Impl( pParent );

    // now use special naming scheme ( name takes leading chars and an index counting up from zero
    aName += rLeadingChars;
    for ( sal_Int32 i=0;; i++ )
    {
        rtl::OUStringBuffer aTmpBuffer(aName);
        aTmpBuffer.append(i);
        if ( pExtension )
            aTmpBuffer.append(*pExtension);
        else
            aTmpBuffer.append(".tmp");
        rtl::OUString aTmp = aTmpBuffer.makeStringAndClear();

        if ( bDirectory )
        {
            FileBase::RC err = Directory::create( aTmp );
            if ( err == FileBase::E_None )
            {
                pImp->aName = aTmp;
                break;
            }
            else if ( err != FileBase::E_EXIST )
                // if f.e. name contains invalid chars stop trying to create dirs
                break;
        }
        else
        {
            File aFile( aTmp );
            FileBase::RC err = aFile.open(osl_File_OpenFlag_Create);
            if ( err == FileBase::E_None )
            {
                pImp->aName = aTmp;
                aFile.close();
                break;
            }
            else if ( err != FileBase::E_EXIST )
                // if f.e. name contains invalid chars stop trying to create dirs
                break;
        }
    }
}

TempFile::~TempFile()
{
    if ( bKillingFileEnabled )
    {
        if ( pImp->bIsDirectory )
        {
            // at the moment no recursiv algorithm present
            Directory::remove( pImp->aName );
        }
        else
        {
            File::remove( pImp->aName );
        }
    }

    delete pImp;
}

sal_Bool TempFile::IsValid() const
{
    return pImp->aName.Len() != 0;
}

String TempFile::GetName() const
{
    rtl::OUString aTmp;
    aTmp = pImp->aName;
    return aTmp;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
