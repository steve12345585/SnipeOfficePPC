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

package com.sun.star.script.framework.io;

import java.net.*;
import java.io.*;
import java.util.*;
import java.util.zip.*;

import com.sun.star.uno.XComponentContext;
import com.sun.star.ucb.XSimpleFileAccess;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.lang.XMultiComponentFactory;
import com.sun.star.io.XInputStream;
import com.sun.star.io.XOutputStream;
import com.sun.star.io.XTruncate;

import com.sun.star.script.framework.log.LogUtils;
import com.sun.star.script.framework.provider.PathUtils;

public class UCBStreamHandler extends URLStreamHandler {

    public final static String separator = "/ucb/";

    private XComponentContext m_xContext = null;
    private XMultiComponentFactory m_xMultiComponentFactory = null;
    private XSimpleFileAccess m_xSimpleFileAccess = null;
    private HashMap m_jarStreamMap = new HashMap(12);
    public static String m_ucbscheme;

    public UCBStreamHandler( XComponentContext ctxt, String scheme, XSimpleFileAccess xSFA )
    {
        LogUtils.DEBUG( "UCBStreamHandler ctor, scheme = " + scheme );
        this.m_xContext = ctxt;
        this.m_ucbscheme = scheme;
        this.m_xSimpleFileAccess = xSFA;
    }

    public void parseURL(URL url, String spec, int start, int limit) {
        LogUtils.DEBUG("**XUCBStreamHandler, parseURL: " + url + " spec: " + spec + " start: " + start + " limit: " + limit );

        String file = url.getFile();
        if (file == null)
            file =  spec.substring(start, limit);
        else
            file += spec.substring(start, limit);

        LogUtils.DEBUG("**For scheme = " + m_ucbscheme );
        LogUtils.DEBUG("**Setting path = " + file );
        setURL(url, m_ucbscheme, null, -1, null, null, file, null, null);
    }

    public URLConnection openConnection(URL u) throws IOException {
        return new UCBConnection(u);
    }

    private class UCBConnection extends URLConnection {

        public UCBConnection(URL url) {
            super(url);
        }

        public void connect() {
        }

        public InputStream getInputStream() throws IOException {
            LogUtils.DEBUG("UCBConnectionHandler GetInputStream on " + url );
            String sUrl = url.toString();
            if (sUrl.lastIndexOf(separator) == -1) {
                LogUtils.DEBUG("getInputStream straight file load" );
                return getFileStreamFromUCB(sUrl);
            }
            else {
                String path = sUrl.substring(0, sUrl.lastIndexOf(separator) );
                String file = sUrl.substring(
                    sUrl.lastIndexOf(separator) + separator.length());
                LogUtils.DEBUG("getInputStream, load of file from another file eg. " + file + " from " + path );
                return getUCBStream(file, path);
            }
        }
        public OutputStream getOutputStream() throws IOException {
            LogUtils.DEBUG("UCBConnectionHandler getOutputStream on " + url );
            OutputStream os = null;
            try
            {
                String sUrl = url.toString();
                if ( !( sUrl.lastIndexOf(separator) == -1) ) {
                    String path = sUrl.substring(0, sUrl.lastIndexOf(separator));
                    String file = sUrl.substring(
                        sUrl.lastIndexOf(separator) + separator.length());

                    if ( m_xSimpleFileAccess.isReadOnly( path ) )
                    {
                        throw new java.io.IOException("File is read only");
                    }

                    LogUtils.DEBUG("getOutputStream, create o/p  stream  for file eg. " + path  );

                    // we will only deal with simple file write
                    XOutputStream xos = m_xSimpleFileAccess.openFileWrite( path );
                    XTruncate xtrunc = ( XTruncate ) UnoRuntime.queryInterface( XTruncate.class, xos );
                    if ( xtrunc != null )
                    {
                        xtrunc.truncate();
                    }
                    os = new XOutputStreamWrapper( xos );
                }
                if ( os == null )
                {
                    throw new IOException("Failed to get OutputStream for " + sUrl );
                }
            }
            catch ( com.sun.star.ucb.CommandAbortedException cae )
            {
                LogUtils.DEBUG("caught exception: " + cae.toString() + " getting writable stream from " + url );
                throw new IOException( cae.toString() );
            }
            catch ( com.sun.star.uno.Exception e )
            {
                LogUtils.DEBUG("caught unknown exception: " + e.toString() + " getting writable stream from " + url );
                throw new IOException( e.toString() );
            }
            return os;
        }
    }


    private InputStream getUCBStream(String file, String path)
        throws IOException
    {
        InputStream is = null;
        InputStream result = null;

        try {
            if (path.endsWith(".jar")) {
                is = (InputStream)m_jarStreamMap.get(path);

                if (is == null) {
                    is = getFileStreamFromUCB(path);
                    m_jarStreamMap.put(path, is);
                }
                else {
                    try {
                        is.reset();
                    }
                    catch (IOException e) {
                        is.close();
                        is = getFileStreamFromUCB(path);
                        m_jarStreamMap.put(path, is);
                    }
                }
                result = getFileStreamFromJarStream(file, is);
            }
            else
            {
                String fileUrl = PathUtils.make_url(path,file);
                result = getFileStreamFromUCB(fileUrl);
            }
        }
        finally {
            if (is != null) {
                try {
                    is.close();
                }
                catch (IOException ioe) {
                    LogUtils.DEBUG("Caught exception closing stream: " +
                        ioe.getMessage());
                }
            }
        }
        return result;
    }

    private InputStream getFileStreamFromJarStream(String file, InputStream is)
        throws IOException
    {
        ZipInputStream zis = null;
        ZipEntry entry = null;
        boolean found = false;

        zis = new ZipInputStream(is);

        while (zis.available() != 0) {
            entry = (ZipEntry)zis.getNextEntry();

            if (entry.getName().equals(file)) {
                return zis;
            }
        }
        return null;
    }

    private InputStream getFileStreamFromUCB(String path)
        throws IOException
    {
        InputStream result = null;
        XInputStream xInputStream = null;

        try {
            LogUtils.DEBUG("Trying to read from " + path );
            xInputStream = m_xSimpleFileAccess.openFileRead(path);
            LogUtils.DEBUG("sfa appeared to read file " );
            byte[][] inputBytes = new byte[1][];

            int ln = 0;
            int sz = m_xSimpleFileAccess.getSize(path);
            // TODO don't depend on result of available() or size()
            // just read stream 'till complete
            if ( sz == 0 )
            {
                if ( xInputStream.available() > 0 )
                {
                    sz = xInputStream.available();
                }
            }
            LogUtils.DEBUG("size of file " + path  + " is " + sz );
            LogUtils.DEBUG("available = " + xInputStream.available() );
            inputBytes[0] = new byte[sz];

            ln = xInputStream.readBytes(inputBytes, sz);

            if (ln != sz) {
                throw new IOException(
                    "Failed to read " + sz + " bytes from XInputStream");
            }

            result = new ByteArrayInputStream(inputBytes[0]);
        }
        catch (com.sun.star.io.IOException ioe) {
            LogUtils.DEBUG("caught exception " + ioe );
            throw new IOException(ioe.getMessage());
        }
        catch (com.sun.star.uno.Exception e) {
            LogUtils.DEBUG("caught exception " + e );
            throw new IOException(e.getMessage());
        }
        finally
        {
            if (xInputStream != null) {
                try {
                    xInputStream.closeInput();
                }
                catch (Exception e2) {
                    LogUtils.DEBUG(
                        "Error closing XInputStream:" + e2.getMessage());
                }
            }
        }
        return result;
    }

    private String convertClassNameToFileName(String name) {
        return name.replace('.', File.separatorChar) + ".class";
    }

}
