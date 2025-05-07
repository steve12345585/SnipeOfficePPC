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



#ifdef WNT
#include <windows.h>
#endif

#include <l10ntools/directory.hxx>
#include <rtl/ustring.hxx>
#include <iostream>
#include <vector>
#include <algorithm>

namespace transex
{

Directory::Directory( const rtl::OUString sFullPath , const rtl::OUString sEntry )
{
    sFullName       = sFullPath;
    sDirectoryName  = sEntry;
}

bool Directory::lessDir ( const Directory& rKey1, const Directory& rKey2 )
{
    rtl::OUString sName1( ( static_cast< Directory >( rKey1 ) ).getDirectoryName() );
    rtl::OUString sName2( ( static_cast< Directory >( rKey2 ) ).getDirectoryName() );

    return sName1.compareTo( sName2 ) < 0 ;
}


void Directory::dump()
{

    for( std::vector< transex::File >::iterator iter = aFileVec.begin() ; iter != aFileVec.end() ; ++iter )
    {
        std::cout << "FILE " << rtl::OUStringToOString( (*iter).getFullName().getStr() , RTL_TEXTENCODING_UTF8 , (*iter).getFullName().getLength() ).getStr() << "\n";
    }

    for( std::vector< transex::Directory >::iterator iter = aDirVec.begin() ; iter != aDirVec.end() ; ++iter )
    {
        std::cout << "DIR " << rtl::OUStringToOString( (*iter).getFullName().getStr() , RTL_TEXTENCODING_UTF8 , (*iter).getFullName().getLength() ).getStr() << "\n";
    }

}

void Directory::scanSubDir( int nLevels )
{
    readDirectory( sFullName );
    dump();
    if( nLevels > 0 ) {
        for( std::vector< transex::Directory >::iterator iter = aDirVec.begin() ; iter != aDirVec.end() || nLevels > 0 ; ++iter , nLevels-- )
        {
            ( *iter ).scanSubDir();
        }
    }
}

#ifdef WNT

void Directory::readDirectory ( const rtl::OUString& sFullpath )
{
    sal_Bool            fFinished;
    HANDLE          hList;
    TCHAR           szDir[MAX_PATH+1];
    TCHAR           szSubDir[MAX_PATH+1];
    WIN32_FIND_DATA FileData;

    rtl::OString sFullpathext = rtl::OUStringToOString( sFullpath , RTL_TEXTENCODING_UTF8 , sFullpath.getLength() );
    const char *dirname = sFullpathext.getStr();

    // Get the proper directory path
    sprintf(szDir, "%s\\*", dirname);

    // Get the first file
    hList = FindFirstFile(szDir, &FileData);
    if (hList == INVALID_HANDLE_VALUE)
    {
        //FindClose(hList);
        //printf("No files found %s\n", szDir ); return;
    }
    else
    {
        fFinished = sal_False;
        while (!fFinished)
        {

            sprintf(szSubDir, "%s\\%s", dirname, FileData.cFileName);
            rtl::OString myfile( FileData.cFileName );
            rtl::OString mydir( szSubDir );

            if (FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if ( (strcmp(FileData.cFileName, ".") != 0 ) &&
                     (strcmp(FileData.cFileName, "..") != 0 ) )
                {
                    //sprintf(szSubDir, "%s\\%s", dirname, FileData.cFileName);
                    transex::Directory aDir(    rtl::OStringToOUString( mydir , RTL_TEXTENCODING_UTF8 , mydir.getLength() ),
                                                rtl::OStringToOUString( myfile , RTL_TEXTENCODING_UTF8 , myfile.getLength() ) );
                    aDirVec.push_back( aDir );
                }
            }
            else
            {
                transex::File aFile(    rtl::OStringToOUString( mydir , RTL_TEXTENCODING_UTF8 , mydir.getLength() ),
                                        rtl::OStringToOUString( myfile , RTL_TEXTENCODING_UTF8 , myfile.getLength() ) );
                aFileVec.push_back( aFile );
            }
            if (!FindNextFile(hList, &FileData))
            {
                if (GetLastError() == ERROR_NO_MORE_FILES)
                {
                    fFinished = sal_True;
                }
            }
        }
    }

    FindClose(hList);

    ::std::sort( aFileVec.begin() , aFileVec.end() , File::lessFile );
    ::std::sort( aDirVec.begin()  , aDirVec.end()  , Directory::lessDir  );
}

#else

class dirholder
{
private:
    DIR *mpDir;
public:
    dirholder(DIR *pDir) : mpDir(pDir) {}
    int close() { int nRet = mpDir ? closedir(mpDir) : 0; mpDir = NULL; return nRet; }
    ~dirholder() { close(); }
};

void Directory::readDirectory( const rtl::OUString& sFullpath )
{
    struct stat     statbuf;
    struct stat     statbuf2;
    struct dirent   *dirp;
    DIR             *dir;

    if(sFullpath.isEmpty()) return;

    rtl::OString   sFullpathext = rtl::OUStringToOString( sFullpath , RTL_TEXTENCODING_UTF8 );

    // stat
    if( stat( sFullpathext.getStr(), &statbuf ) < 0 )
    {
        printf("warning: Cannot stat %s \n" , sFullpathext.getStr() );
        return;
    }

    if( S_ISDIR(statbuf.st_mode ) == 0 )
        return;

    if( (dir = opendir( sFullpathext.getStr() ) ) == NULL  )
    {
        printf("read error 2 in %s \n",sFullpathext.getStr());
        return;
    }

    dirholder aHolder(dir);

    const rtl::OString sDot ( "." ) ;
    const rtl::OString sDDot( ".." );

    if ( chdir( sFullpathext.getStr() ) == -1 )
    {
        printf("chdir error in %s \n",sFullpathext.getStr());
        return;
    }

    sFullpathext += rtl::OString( "/" );

    while(  ( dirp = readdir( dir ) ) != NULL )
    {
        rtl::OString sEntryName(  dirp->d_name );

        if( sEntryName.equals( sDot )  || sEntryName.equals( sDDot ) )
            continue;

        // add dir entry
        rtl::OString sEntity = sFullpathext;
        sEntity += sEntryName;

        // stat new entry
        if( lstat( sEntity.getStr() , &statbuf2 ) < 0 )
        {
            printf("error on entry %s\n" , sEntity.getStr() ) ;
            continue;
        }

        // add file / dir to vector
        switch( statbuf2.st_mode & S_IFMT )
        {
            case S_IFREG:
                        {
                            transex::File aFile( rtl::OStringToOUString( sEntity , RTL_TEXTENCODING_UTF8 , sEntity.getLength() ) ,
                                                 rtl::OStringToOUString( sEntryName , RTL_TEXTENCODING_UTF8 , sEntryName.getLength() )
                                               );

                            aFileVec.push_back( aFile ) ;
                            break;
                         }
            case S_IFLNK:
            case S_IFDIR:
                        {
                            transex::Directory aDir(
                                                     rtl::OStringToOUString( sEntity , RTL_TEXTENCODING_UTF8 , sEntity.getLength() ) ,
                                                     rtl::OStringToOUString( sEntryName , RTL_TEXTENCODING_UTF8 , sEntryName.getLength() )
                                                   ) ;
                            aDirVec.push_back( aDir ) ;
                            break;
                         }
        }
    }
    if ( chdir( ".." ) == -1 )
    {
        printf("chdir error in .. \n");
        return;
    }

    if ( aHolder.close() < 0 )
        return;

    std::sort( aFileVec.begin() , aFileVec.end() , File::lessFile );
    std::sort( aDirVec.begin()  , aDirVec.end()  , Directory::lessDir  );

}

#endif
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
