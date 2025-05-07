/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
#include <vector>
#include <algorithm>
#include <rtl/ustring.hxx>

#ifdef WNT
#else
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#endif

#include <stdio.h>

#ifndef L10NTOOLS_FILE_HXX
#define L10NTOOLS_FILE_HXX
#include <l10ntools/file.hxx>
#endif

namespace transex{

class Directory
{
    private:
    rtl::OUString sDirectoryName;
    rtl::OUString sFullName;

    std::vector<Directory>  aDirVec;
    std::vector<File>       aFileVec;

    public:
    std::vector<Directory>  getSubDirectories()  { return aDirVec;        }
    std::vector<File>       getFiles()           { return aFileVec;       }

    void readDirectory( const rtl::OUString& sFullpath );
    void scanSubDir( int nLevels = 0 );

    rtl::OUString getDirectoryName()            { return sDirectoryName; }
    rtl::OUString getFullName()                 { return sFullName ;     }

    void dump();
    Directory(){}

    Directory( const rtl::OUString sFullPath , const rtl::OUString sEntry ) ;

    static bool lessDir ( const Directory& rKey1, const Directory& rKey2 ) ;
};

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
