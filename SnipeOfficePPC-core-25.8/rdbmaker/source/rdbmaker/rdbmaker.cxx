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

#include <stdio.h>
#include <osl/file.hxx>
#include <osl/process.h>
#include <codemaker/typemanager.hxx>
#include <codemaker/dependency.hxx>

#include <rtl/strbuf.hxx>

#if defined(SAL_W32)
#include <io.h>
#include <direct.h>
#include <errno.h>
#endif

#ifdef UNX
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#endif

#include "specialtypemanager.hxx"
#include "rdboptions.hxx"
#include "rdbtype.hxx"

#define PATH_DELEMITTER '/'

using namespace osl;

using ::rtl::OUString;
using ::rtl::OString;
using ::rtl::OStringBuffer;
using ::rtl::OUStringToOString;
using ::rtl::OStringToOUString;

FileStream          listFile;
RegistryKey         rootKey;
Registry            regFile;
sal_Bool            useSpecial;
TypeManager*        pTypeMgr = NULL;
StringList          dirEntries;
StringSet           filterTypes;

OString getFullNameOfApplicatRdb()
{
    OUString bootReg;
    OUString uTmpStr;
    if( osl_getExecutableFile(&uTmpStr.pData) == osl_Process_E_None )
    {
        sal_uInt32  lastIndex = uTmpStr.lastIndexOf(PATH_DELEMITTER);
        OUString    tmpReg;

        if ( lastIndex > 0 )
        {
            tmpReg =uTmpStr.copy(0, lastIndex + 1);
        }

        tmpReg += OUString( RTL_CONSTASCII_USTRINGPARAM("applicat.rdb") );

        FileBase::getSystemPathFromFileURL(tmpReg, bootReg);
    }

    return OUStringToOString(bootReg, RTL_TEXTENCODING_ASCII_US);
}

void initFilterTypes(RdbOptions* pOptions)
{
    if (pOptions->isValid("-FT"))
    {
        OString fOption(pOptions->getOption("-FT"));
        sal_Int32 nIndex = 0;
        do
        {
            filterTypes.insert( fOption.getToken( 0, ';', nIndex ).replace('.', '/') );
        }
        while ( nIndex >= 0 );
    }
    if (pOptions->isValid("-F"))
    {
        FILE  *f = fopen(pOptions->getOption("-F").getStr(), "r");

        if (f)
        {
            sal_Char buffer[1024+1];
            sal_Char *pBuf = fgets(buffer, 1024, f);
            sal_Char *s = NULL;
            sal_Char *p = NULL;
            while ( pBuf && !feof(f))
            {
                p = pBuf;
                if (*p != '\n' && *p != '\r')
                {
                    while (*p == ' ' && *p =='\t')
                        p++;

                    s = p;
                    while (*p != '\n' && *p != '\r' && *p != ' ' && *p != '\t')
                        p++;

                    *p = '\0';
                    filterTypes.insert( OString(s).replace('.', '/') );
                }

                pBuf = fgets(buffer, 1024, f);
            }

            fclose(f);
        }
    }
}

sal_Bool checkFilterTypes(const OString& type)
{
    StringSet::iterator iter = filterTypes.begin();
    while ( iter != filterTypes.end() )
    {
        if ( type.indexOf( *iter ) == 0 )
        {
            return sal_True;
        }

        ++iter;
    }

    return sal_False;
}

void cleanUp( sal_Bool bError)
{
    if ( pTypeMgr )
    {
        delete pTypeMgr;
    }
    if (useSpecial)
    {
        pTypeMgr = new SpecialTypeManager();
    }else
    {
        pTypeMgr = new RegistryTypeManager();
    }

    if ( rootKey.isValid() )
    {
        rootKey.closeKey();
    }
    if ( regFile.isValid() )
    {
        if ( bError )
        {
            regFile.destroy(OUString());
        } else
        {
            regFile.close();
        }
    }
    if ( listFile.isValid() )
    {
        listFile.close();
        unlink(listFile.getName().getStr());
    }

    StringList::reverse_iterator iter = dirEntries.rbegin();
    while ( iter != dirEntries.rend() )
    {
        if (rmdir((char*)(*iter).getStr()) == -1)
        {
            break;
        }

        ++iter;
    }
}

OString createFileName(const OString& path)
{
    OString fileName(path);

    sal_Char token;
#ifdef SAL_UNX
    fileName = fileName.replace('\\', '/');
    token = '/';
#else
    fileName = fileName.replace('/', '\\');
    token = '\\';
#endif

    OStringBuffer nameBuffer( path.getLength() );

    sal_Int32 nIndex = 0;
    do
    {
        nameBuffer.append(fileName.getToken( 0, token, nIndex ).getStr());
        if ( nIndex == -1 ) break;

        if (nameBuffer.getLength() == 0 || OString(".") == nameBuffer.getStr())
        {
            nameBuffer.append(token);
            continue;
        }

#if defined(SAL_UNX)
        if (mkdir((char*)nameBuffer.getStr(), 0777) == -1)
#else
           if (mkdir((char*)nameBuffer.getStr()) == -1)
#endif
        {
            if ( errno == ENOENT )
                return OString();
        } else
        {
            dirEntries.push_back(nameBuffer.getStr());
        }

        nameBuffer.append(token);
    }
    while ( nIndex >= 0 );

    return fileName;
}

sal_Bool produceAllTypes(const OString& typeName,
                         TypeManager& typeMgr,
                         TypeDependency& typeDependencies,
                         RdbOptions* pOptions,
                         sal_Bool bFullScope,
                         FileStream& o,
                         RegistryKey& regKey,
                         StringSet& filterTypes2)
    throw( CannotDumpException )
{
    if (!produceType(typeName, typeMgr, typeDependencies, pOptions, o, regKey, filterTypes2))
    {
        fprintf(stderr, "%s ERROR: %s\n",
                pOptions->getProgramName().getStr(),
                OString("cannot dump Type '" + typeName + "'").getStr());
        cleanUp(sal_True);
        exit(99);
    }

    RegistryKey typeKey = typeMgr.getTypeKey(typeName);
    RegistryKeyNames subKeys;

    if (typeKey.getKeyNames(OUString(), subKeys))
        return sal_False;

    OString tmpName;
    for (sal_uInt32 i=0; i < subKeys.getLength(); i++)
    {
        tmpName = OUStringToOString(subKeys.getElement(i), RTL_TEXTENCODING_UTF8);

        if (pOptions->isValid("-B"))
            tmpName = tmpName.copy(tmpName.indexOf('/', 1) + 1);
        else
            tmpName = tmpName.copy(1);

        if (bFullScope)
        {
            if (!produceAllTypes(tmpName, typeMgr, typeDependencies, pOptions, sal_True,
                                 o, regKey, filterTypes2))
                return sal_False;
        } else
        {
            if (!produceType(tmpName, typeMgr, typeDependencies, pOptions, o, regKey, filterTypes2))
                return sal_False;
        }
    }

    return sal_True;
}


#if (defined UNX)
int main( int argc, char * argv[] )
#else
int __cdecl main( int argc, char * argv[] )
#endif
{
    RdbOptions options;

    try
    {
        if (!options.initOptions(argc, argv))
        {
            cleanUp(sal_True);
            exit(1);
        }
    }
    catch(const IllegalArgument& e)
    {
        fprintf(stderr, "Illegal option: %s\n", e.m_message.getStr());
        cleanUp(sal_True);
        exit(99);
    }

    TypeDependency  typeDependencies;

    OString bootReg;

    if ( options.isValid("-R") )
    {
        bootReg = options.getOption("-R");
    } else
    {
        if (options.getInputFiles().empty())
        {
            bootReg = getFullNameOfApplicatRdb();
        }
    }

    if ( !bootReg.isEmpty() )
    {
        pTypeMgr = new SpecialTypeManager();
        useSpecial = sal_True;
    } else
    {
        pTypeMgr = new RegistryTypeManager();
        useSpecial = sal_False;
    }

    TypeManager& typeMgr = *pTypeMgr;

    if ( useSpecial && !typeMgr.init( bootReg ) )
    {
        fprintf(stderr, "%s : init typemanager failed, check your environment for bootstrapping uno.\n", options.getProgramName().getStr());
        cleanUp(sal_True);
        exit(99);
    }
    if ( !useSpecial && !typeMgr.init(!options.isValid("-T"), options.getInputFiles()))
    {
        fprintf(stderr, "%s : init registries failed, check your registry files.\n", options.getProgramName().getStr());
        cleanUp(sal_True);
        exit(99);
    }

    initFilterTypes(&options);

    if (options.isValid("-B"))
    {
        typeMgr.setBase(options.getOption("-B"));
    }

    if ( !options.isValid("-O") )
    {
        fprintf(stderr, "%s ERROR: %s\n",
                options.getProgramName().getStr(),
                "no output file is specified.");
        cleanUp(sal_True);
        exit(99);
    }

    if ( options.generateTypeList() )
    {
        OString fileName = createFileName( options.getOption("-O") );
        listFile.open(fileName);

        if ( !listFile.isValid() )
        {
            fprintf(stderr, "%s ERROR: %s\n",
                    options.getProgramName().getStr(),
                    "could not open output file.");
            cleanUp(sal_True);
            exit(99);
        }
    } else
    {
        OUString fileName( OStringToOUString(createFileName( options.getOption("-O") ), RTL_TEXTENCODING_UTF8) );
        if ( regFile.create(fileName) )
        {
            fprintf(stderr, "%s ERROR: %s\n",
                    options.getProgramName().getStr(),
                    "could not create registry output file.");
            cleanUp(sal_True);
            exit(99);
        }


        if (options.isValid("-b"))
        {
            RegistryKey tmpKey;
            regFile.openRootKey(tmpKey);

            tmpKey.createKey( OStringToOUString(options.getOption("-b"), RTL_TEXTENCODING_UTF8), rootKey);
        } else
        {
            regFile.openRootKey(rootKey);
        }
    }

    try
    {
        if (options.isValid("-T"))
        {
            OString tOption(options.getOption("-T"));
            OString typeName, tmpName;
            sal_Bool ret = sal_False;
            sal_Int32 nIndex = 0;
            do
            {
                typeName = tOption.getToken( 0, ';', nIndex);
                sal_Int32 lastIndex = typeName.lastIndexOf('.');
                tmpName = typeName.copy( lastIndex+1 );
                if (tmpName == "*")
                {
                    if (!bootReg.isEmpty())
                    {
                        fprintf(stderr, "%s ERROR: %s\n",
                                options.getProgramName().getStr(),
                                "dumping all types of a scope is not possible if -R option is used.");
                        exit(99);
                    }
                    // produce this type and his scope, but the scope is not recursively  generated.
                    if (typeName.equals("*"))
                    {
                        tmpName = "/";
                    } else
                    {
                        tmpName = typeName.copy(0, typeName.lastIndexOf('.')).replace('.', '/');
                        if (tmpName.isEmpty())
                            tmpName = "/";
                        else
                            tmpName.replace('.', '/');
                    }
                    ret = produceAllTypes(tmpName, typeMgr, typeDependencies, &options, sal_False,
                                          listFile, rootKey, filterTypes);
                } else
                {
                    // produce only this type
                    ret = produceType(typeName.replace('.', '/'), typeMgr, typeDependencies,
                                      &options, listFile, rootKey, filterTypes);
                }

                if (!ret)
                {
                    fprintf(stderr, "%s ERROR: %s\n",
                            options.getProgramName().getStr(),
                            OString("cannot dump Type '" + typeName + "'").getStr());
                    cleanUp(sal_True);
                    exit(99);
                }
            }
            while ( nIndex >= 0 );
        } else
        if (options.isValid("-X"))
        {
        } else
        {
            if (bootReg.isEmpty())
            {
                // produce all types
                if (!produceAllTypes("/", typeMgr, typeDependencies, &options, sal_True,
                                     listFile, rootKey, filterTypes))
                {
                    fprintf(stderr, "%s ERROR: %s\n",
                            options.getProgramName().getStr(),
                            "an error occurs while dumping all types.");
                    exit(99);
                }
            } else
            {
                fprintf(stderr, "%s ERROR: %s\n",
                        options.getProgramName().getStr(),
                        "dumping all types is not possible if -R option is used.");
                exit(99);
            }
        }
    }
    catch(const CannotDumpException& e)
    {
        fprintf(stderr, "%s ERROR: %s\n",
                options.getProgramName().getStr(),
                e.m_message.getStr());
        cleanUp(sal_True);
        exit(99);
    }

    cleanUp(sal_False);
    return 0;
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
