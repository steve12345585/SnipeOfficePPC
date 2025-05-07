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


#include "docrecovery.hxx"
#include "osl/file.hxx"
#include "osl/process.h"
#include "rtl/bootstrap.hxx"
#include "rtl/strbuf.hxx"
#include "tools/appendunixshellword.hxx"
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>

#define RCFILE ".crash_reportrc"

using namespace ::std;

static const char *get_home_dir()
{
    struct passwd *ppwd = getpwuid( getuid() );

    return ppwd ? (ppwd->pw_dir ? ppwd->pw_dir : "/") : "/";
}

static bool read_line( FILE *fp, string& rLine )
{
    char szBuffer[1024];
    bool bSuccess = false;
    bool bEOL = false;
    string  line;


    while ( !bEOL && fgets( szBuffer, sizeof(szBuffer), fp ) )
    {
        int len = strlen(szBuffer);

        bSuccess = true;

        while ( len && szBuffer[len - 1] == '\n' )
        {
            szBuffer[--len] = 0;
            bEOL = true;
        }

        line.append( szBuffer );
    }

    rLine = line;
    return bSuccess;
}

static string trim_string( const string& rString )
{
    string temp = rString;

    while ( temp.length() && (temp[0] == ' ' || temp[0] == '\t') )
        temp.erase( 0, 1 );

    string::size_type   len = temp.length();

    while ( len && (temp[len-1] == ' ' || temp[len-1] == '\t') )
    {
        temp.erase( len - 1, 1 );
        len = temp.length();
    }

    return temp;
}

static string get_profile_string( const char *pFileName, const char *pSectionName, const char *pKeyName, const char *pDefault = NULL )
{
    FILE    *fp = fopen( pFileName, "r" );
    string  retValue = pDefault ? pDefault : "";

    if ( fp )
    {
        string line;
        string section;

        while ( read_line( fp, line ) )
        {
            line = trim_string( line );

            if ( line.length() && line[0] == '[' )
            {
                line.erase( 0, 1 );
                string::size_type end = line.find( ']', 0 );

                if ( string::npos != end )
                    section = trim_string( line.substr( 0, end ) );
            }
            else
            {

                string::size_type iEqualSign = line.find( '=', 0 );

                if ( iEqualSign != string::npos )
                {
                    string  keyname = line.substr( 0, iEqualSign );
                    keyname = trim_string( keyname );

                    string  value = line.substr( iEqualSign + 1, string::npos );
                    value = trim_string( value );

                    if (
                        0 == strcasecmp( section.c_str(), pSectionName ) &&
                        0 == strcasecmp( keyname.c_str(), pKeyName )
                         )
                    {
                        retValue = value;
                        break;
                    }
                }
            }
        }

        fclose( fp );
    }

    return retValue;
}

static bool get_profile_bool( const char *pFileName, const char *pSectionName, const char *pKeyName )
{
    string  str = get_profile_string( pFileName, pSectionName, pKeyName );

    if ( !strcasecmp( str.c_str(), "true" ) )
        return true;
    return false;
}

static String get_profile_String( const char *pFileName, const char *pSectionName, const char *pKeyName, const char * = NULL )
{
    string  str = get_profile_string( pFileName, pSectionName, pKeyName );
    String  result( str.c_str(), RTL_TEXTENCODING_UTF8 );

    return result;
}

namespace svx{
    namespace DocRecovery{

        bool ErrorRepSendDialog::ReadParams()
        {
            string  sRCFile = get_home_dir();

            sRCFile += "/";
            sRCFile += string(RCFILE);

            maEMailAddrED.SetText( get_profile_String( sRCFile.c_str(), "Options", "ReturnAddress" ) );
            maParams.maHTTPProxyServer = get_profile_String( sRCFile.c_str(), "Options", "ProxyServer" );
            maParams.maHTTPProxyPort = get_profile_String( sRCFile.c_str(), "Options", "ProxyPort" );
            maParams.miHTTPConnectionType = get_profile_bool( sRCFile.c_str(), "Options", "UseProxy" ) ? 2 : 1;
            maContactCB.Check( get_profile_bool( sRCFile.c_str(), "Options", "AllowContact" ) );

            return true;
        }

        bool ErrorRepSendDialog::SaveParams()
        {
            bool success = false;
            string  sRCFile = get_home_dir();

            sRCFile += "/";
            sRCFile += string(RCFILE);

            FILE *fp = fopen( sRCFile.c_str(), "w" );

            if ( fp )
            {
                fprintf( fp, "[Options]\n" );
                fprintf( fp, "UseProxy=%s\n", 2 == maParams.miHTTPConnectionType ? "true" : "false" );
                fprintf( fp, "ProxyServer=%s\n", rtl::OUStringToOString( maParams.maHTTPProxyServer, RTL_TEXTENCODING_UTF8 ).getStr() );
                fprintf( fp, "ProxyPort=%s\n", rtl::OUStringToOString( maParams.maHTTPProxyPort, RTL_TEXTENCODING_UTF8 ).getStr() );
                fprintf( fp, "ReturnAddress=%s\n", rtl::OUStringToOString( GetEMailAddress(), RTL_TEXTENCODING_UTF8 ).getStr() );
                fprintf( fp, "AllowContact=%s\n", IsContactAllowed() ? "true" : "false" );
                fclose( fp );
            }

            return success;
        }

        bool ErrorRepSendDialog::SendReport()
        {
            rtl::OUString sSubEnvVar(RTL_CONSTASCII_USTRINGPARAM("ERRORREPORT_SUBJECT"));
            rtl::OUString strSubject(GetDocType());
            osl_setEnvironment(sSubEnvVar.pData, strSubject.pData);

            char szBodyFile[L_tmpnam] = "";
            FILE *fp = fopen( tmpnam( szBodyFile ), "w" );

            if ( fp )
            {
                rtl::OString strUTF8(rtl::OUStringToOString(GetUsing(), RTL_TEXTENCODING_UTF8));

                size_t nWritten = fwrite(strUTF8.getStr(), 1, strUTF8.getLength(), fp);
                OSL_VERIFY(nWritten == static_cast<size_t>(strUTF8.getLength()));
                fclose( fp );

                rtl::OUString sBodyEnvVar(RTL_CONSTASCII_USTRINGPARAM("ERRORREPORT_BODYFILE"));
                rtl::OUString strBodyFile(rtl::OStringToOUString(rtl::OString(szBodyFile),
                    osl_getThreadTextEncoding()));
                osl_setEnvironment(sBodyEnvVar.pData, strBodyFile.pData);
            }

            int ret = -1;
            rtl::OUString path1(
                RTL_CONSTASCII_USTRINGPARAM(
                    "$BRAND_BASE_DIR/program/crashrep"));
            rtl::Bootstrap::expandMacros(path1);
            rtl::OString path2;
            if ((osl::FileBase::getSystemPathFromFileURL(path1, path1) ==
                 osl::FileBase::E_None) &&
                path1.convertToString(
                    &path2, osl_getThreadTextEncoding(),
                    (RTL_UNICODETOTEXT_FLAGS_UNDEFINED_ERROR |
                     RTL_UNICODETOTEXT_FLAGS_INVALID_ERROR)))
            {
                rtl::OStringBuffer cmd;
                tools::appendUnixShellWord(&cmd, path2);
                cmd.append(RTL_CONSTASCII_STRINGPARAM(" -debug -load -send -noui"));
                ret = system(cmd.getStr());
            }

            if ( szBodyFile[0] )
            {
                unlink( szBodyFile );
            }

            return -1 != ret;
        }


    }   // namespace DocRecovery
}   // namespace svx

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
