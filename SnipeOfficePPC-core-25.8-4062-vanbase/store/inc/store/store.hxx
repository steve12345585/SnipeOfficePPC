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

#ifndef _STORE_STORE_HXX_
#define _STORE_STORE_HXX_

#include "sal/types.h"
#include "rtl/ustring.hxx"
#include "store/store.h"

namespace store
{

/*========================================================================
 *
 * OStoreStream interface.
 *
 *======================================================================*/
class OStoreStream
{
public:
    /** Construction.
     */
    inline OStoreStream (void) SAL_THROW(())
        : m_hImpl (0)
    {}

    /** Destruction.
     */
    inline ~OStoreStream (void) SAL_THROW(())
    {
        if (m_hImpl)
            (void) store_releaseHandle (m_hImpl);
    }

    /** Copy construction.
     */
    inline OStoreStream (OStoreStream const & rhs) SAL_THROW(())
        : m_hImpl (rhs.m_hImpl)
    {
        if (m_hImpl)
            (void) store_acquireHandle (m_hImpl);
    }

    /** Assignment.
     */
    inline OStoreStream & operator= (OStoreStream const & rhs) SAL_THROW(())
    {
        if (rhs.m_hImpl)
            (void) store_acquireHandle (rhs.m_hImpl);
        if (m_hImpl)
            (void) store_releaseHandle (m_hImpl);
        m_hImpl = rhs.m_hImpl;
        return *this;
    }

    /** Construction from Stream Handle.
     */
    inline explicit OStoreStream (storeStreamHandle Handle) SAL_THROW(())
        : m_hImpl (Handle)
    {
        if (m_hImpl)
            (void) store_acquireHandle (m_hImpl);
    }

    /** Conversion into Stream Handle.
     */
    inline operator storeStreamHandle (void) const SAL_THROW(())
    {
        return m_hImpl;
    }

    /** Check for a valid Stream Handle.
        @return sal_True if valid, sal_False otherwise.
     */
    inline bool isValid (void) const SAL_THROW(())
    {
        return (m_hImpl != 0);
    }

    /** Open the stream.
        @see store_openStream()
     */
    inline storeError create (
        storeFileHandle       hFile,
        rtl::OUString const & rPath,
        rtl::OUString const & rName,
        storeAccessMode       eMode) SAL_THROW(())
    {
        if (m_hImpl)
        {
            (void) store_releaseHandle (m_hImpl);
            m_hImpl = 0;
        }
        return store_openStream (hFile, rPath.pData, rName.pData, eMode, &m_hImpl);
    }

    /** Close the stream.
        @see store_closeStream()
     */
    inline void close (void) SAL_THROW(())
    {
        if (m_hImpl)
        {
            (void) store_closeStream (m_hImpl);
            m_hImpl = 0;
        }
    }

    /** Read from the stream.
        @see store_readStream()
     */
    inline storeError readAt (
        sal_uInt32   nOffset,
        void *       pBuffer,
        sal_uInt32   nBytes,
        sal_uInt32 & rnDone) SAL_THROW(())
    {
        if (!m_hImpl)
            return store_E_InvalidHandle;

        return store_readStream (m_hImpl, nOffset, pBuffer, nBytes, &rnDone);
    }

    /** Write to the stream.
        @see store_writeStream()
     */
    inline storeError writeAt (
        sal_uInt32   nOffset,
        void const * pBuffer,
        sal_uInt32   nBytes,
        sal_uInt32 & rnDone) SAL_THROW(())
    {
        if (!m_hImpl)
            return store_E_InvalidHandle;

        return store_writeStream (m_hImpl, nOffset, pBuffer, nBytes, &rnDone);
    }

    /** Flush the stream.
        @see store_flushStream()
     */
    inline storeError flush (void) const SAL_THROW(())
    {
        if (!m_hImpl)
            return store_E_InvalidHandle;

        return store_flushStream (m_hImpl);
    }

    /** Get the stream size.
        @see store_getStreamSize()
     */
    inline storeError getSize (sal_uInt32 & rnSize) const SAL_THROW(())
    {
        if (!m_hImpl)
            return store_E_InvalidHandle;

        return store_getStreamSize (m_hImpl, &rnSize);
    }

    /** Set the stream size.
        @see store_setStreamSize()
     */
    inline storeError setSize (sal_uInt32 nSize) SAL_THROW(())
    {
        if (!m_hImpl)
            return store_E_InvalidHandle;

        return store_setStreamSize (m_hImpl, nSize);
    }

private:
    /** Representation.
     */
    storeStreamHandle m_hImpl;
};

/*========================================================================
 *
 * OStoreDirectory interface.
 *
 *======================================================================*/
class OStoreDirectory
{
public:
    /** Construction.
     */
    inline OStoreDirectory (void) SAL_THROW(())
        : m_hImpl (0)
    {}

    /** Destruction.
     */
    inline ~OStoreDirectory (void) SAL_THROW(())
    {
        if (m_hImpl)
            (void) store_releaseHandle (m_hImpl);
    }

    /** Copy construction.
     */
    inline OStoreDirectory (OStoreDirectory const & rhs) SAL_THROW(())
        : m_hImpl (rhs.m_hImpl)
    {
        if (m_hImpl)
            (void) store_acquireHandle (m_hImpl);
    }

    /** Assignment.
     */
    inline OStoreDirectory & operator= (OStoreDirectory const & rhs) SAL_THROW(())
    {
        if (rhs.m_hImpl)
            (void) store_acquireHandle (rhs.m_hImpl);
        if (m_hImpl)
            (void) store_releaseHandle (m_hImpl);
        m_hImpl = rhs.m_hImpl;
        return *this;
    }

    /** Construction from Directory Handle.
     */
    inline explicit OStoreDirectory (storeDirectoryHandle Handle) SAL_THROW(())
        : m_hImpl (Handle)
    {
        if (m_hImpl)
            (void) store_acquireHandle (m_hImpl);
    }

    /** Conversion into Directory Handle.
     */
    inline operator storeDirectoryHandle(void) const SAL_THROW(())
    {
        return m_hImpl;
    }

    /** Check for a valid Directory Handle.
        @return sal_True if valid, sal_False otherwise.
     */
    inline bool isValid (void) const SAL_THROW(())
    {
        return (m_hImpl != 0);
    }

    /** Open the directory.
        @see store_openDirectory()
     */
    inline storeError create (
        storeFileHandle       hFile,
        rtl::OUString const & rPath,
        rtl::OUString const & rName,
        storeAccessMode       eMode) SAL_THROW(())
    {
        if (m_hImpl)
        {
            (void) store_releaseHandle (m_hImpl);
            m_hImpl = 0;
        }
        return store_openDirectory (hFile, rPath.pData, rName.pData, eMode, &m_hImpl);
    }

    /** Close the directory.
        @see store_closeDirectory()
     */
    inline void close (void) SAL_THROW(())
    {
        if (m_hImpl)
        {
            (void) store_closeDirectory (m_hImpl);
            m_hImpl = 0;
        }
    }

    /** Directory iterator type.
        @see first()
        @see next()
     */
    typedef storeFindData iterator;

    /** Find first directory entry.
        @see store_findFirst()
     */
    inline storeError first (iterator& it) SAL_THROW(())
    {
        if (!m_hImpl)
            return store_E_InvalidHandle;

        return store_findFirst (m_hImpl, &it);
    }

    /** Find next directory entry.
        @see store_findNext()
     */
    inline storeError next (iterator& it) SAL_THROW(())
    {
        if (!m_hImpl)
            return store_E_InvalidHandle;

        return store_findNext (m_hImpl, &it);
    }

    /** Directory traversal helper.
        @see travel()
     */
    class traveller
    {
    public:
        /** Directory traversal callback.
            @param  it [in] current directory entry.
            @return sal_True to continue iteration, sal_False to stop.
         */
        virtual sal_Bool visit (const iterator& it) = 0;

    protected:
        ~traveller() {}
    };

    /** Directory traversal.
        @see store_findFirst()
        @see store_findNext()

        @param  rTraveller [in] the traversal callback.
        @return store_E_NoMoreFiles upon end of iteration.
     */
    inline storeError travel (traveller & rTraveller) const
    {
        storeError eErrCode = store_E_InvalidHandle;
        if (m_hImpl)
        {
            iterator it;
            eErrCode = store_findFirst (m_hImpl, &it);
            while ((eErrCode == store_E_None) && rTraveller.visit(it))
                eErrCode = store_findNext (m_hImpl, &it);
        }
        return eErrCode;
    }

private:
    /** Representation.
     */
    storeDirectoryHandle m_hImpl;
};

/*========================================================================
 *
 * OStoreFile interface.
 *
 *======================================================================*/
class OStoreFile
{
public:
    /** Construction.
     */
    inline OStoreFile (void) SAL_THROW(())
        : m_hImpl (0)
    {}

    /** Destruction.
     */
    inline ~OStoreFile (void) SAL_THROW(())
    {
        if (m_hImpl)
            (void) store_releaseHandle (m_hImpl);
    }

    /** Copy construction.
     */
    inline OStoreFile (OStoreFile const & rhs) SAL_THROW(())
        : m_hImpl (rhs.m_hImpl)
    {
        if (m_hImpl)
            (void) store_acquireHandle (m_hImpl);
    }

    /** Assignment.
     */
    inline OStoreFile & operator= (OStoreFile const & rhs) SAL_THROW(())
    {
        if (rhs.m_hImpl)
            (void) store_acquireHandle (rhs.m_hImpl);
        if (m_hImpl)
            (void) store_releaseHandle (m_hImpl);
        m_hImpl = rhs.m_hImpl;
        return *this;
    }

    /** Construction from File Handle.
     */
    inline explicit OStoreFile (storeFileHandle Handle) SAL_THROW(())
        : m_hImpl (Handle)
    {
        if (m_hImpl)
            (void) store_acquireHandle (m_hImpl);
    }

    /** Conversion into File Handle.
     */
    inline operator storeFileHandle (void) const SAL_THROW(())
    {
        return m_hImpl;
    }

    /** Check for a valid File Handle.
        @return sal_True if valid, sal_False otherwise.
     */
    inline bool isValid (void) const SAL_THROW(())
    {
        return (m_hImpl != 0);
    }

    /** Open the file.
        @see store_openFile()
     */
    inline storeError create (
        rtl::OUString const & rFilename,
        storeAccessMode       eAccessMode,
        sal_uInt16            nPageSize = STORE_DEFAULT_PAGESIZE) SAL_THROW(())
    {
        if (m_hImpl)
        {
            (void) store_releaseHandle (m_hImpl);
            m_hImpl = 0;
        }
        return store_openFile (rFilename.pData, eAccessMode, nPageSize, &m_hImpl);
    }

    /** Open the temporary file in memory.
        @see store_createMemoryFile()
     */
    inline storeError createInMemory (
        sal_uInt16 nPageSize = STORE_DEFAULT_PAGESIZE) SAL_THROW(())
    {
        if (m_hImpl)
        {
            (void) store_releaseHandle (m_hImpl);
            m_hImpl = 0;
        }
        return store_createMemoryFile (nPageSize, &m_hImpl);
    }

    /** Close the file.
        @see store_closeFile()
     */
    inline void close (void) SAL_THROW(())
    {
        if (m_hImpl)
        {
            (void) store_closeFile (m_hImpl);
            m_hImpl = 0;
        }
    }

    /** Flush the file.
        @see store_flushFile()
     */
    inline storeError flush (void) const SAL_THROW(())
    {
        if (!m_hImpl)
            return store_E_InvalidHandle;

        return store_flushFile (m_hImpl);
    }

    /** Get the number of referers to the file.
        @see store_getFileRefererCount()
     */
    inline storeError getRefererCount (sal_uInt32 & rnRefCount) const SAL_THROW(())
    {
        if (!m_hImpl)
            return store_E_InvalidHandle;

        return store_getFileRefererCount (m_hImpl, &rnRefCount);
    }

    /** Get the file size.
        @see store_getFileSize()
     */
    inline storeError getSize (sal_uInt32 & rnSize) const SAL_THROW(())
    {
        if (!m_hImpl)
            return store_E_InvalidHandle;

        return store_getFileSize (m_hImpl, &rnSize);
    }

    /** Set attributes of a file entry.
        @see store_attrib()
     */
    inline storeError attrib (
        rtl::OUString const & rPath,
        rtl::OUString const & rName,
        sal_uInt32            nMask1,
        sal_uInt32            nMask2,
        sal_uInt32          & rnAttrib) SAL_THROW(())
    {
        if (!m_hImpl)
            return store_E_InvalidHandle;

        return store_attrib (m_hImpl, rPath.pData, rName.pData, nMask1, nMask2, &rnAttrib);
    }

    /** Set attributes of a file entry.
        @see store_attrib()
     */
    inline storeError attrib (
        rtl::OUString const & rPath,
        rtl::OUString const & rName,
        sal_uInt32            nMask1,
        sal_uInt32            nMask2) SAL_THROW(())
    {
        if (!m_hImpl)
            return store_E_InvalidHandle;

        return store_attrib (m_hImpl, rPath.pData, rName.pData, nMask1, nMask2, NULL);
    }

    /** Insert a file entry as 'hard link' to another file entry.
        @see store_link()
     */
    inline storeError link (
        rtl::OUString const & rSrcPath, rtl::OUString const & rSrcName,
        rtl::OUString const & rDstPath, rtl::OUString const & rDstName) SAL_THROW(())
    {
        if (!m_hImpl)
            return store_E_InvalidHandle;

        return store_link (
            m_hImpl, rSrcPath.pData, rSrcName.pData, rDstPath.pData, rDstName.pData);
    }

    /** Insert a file entry as 'symbolic link' to another file entry.
        @see store_symlink()
     */
    inline storeError symlink (
        rtl::OUString const & rSrcPath, rtl::OUString const & rSrcName,
        rtl::OUString const & rDstPath, rtl::OUString const & rDstName) SAL_THROW(())
    {
        if (!m_hImpl)
            return store_E_InvalidHandle;

        return store_symlink (m_hImpl, rSrcPath.pData, rSrcName.pData, rDstPath.pData, rDstName.pData);
    }

    /** Rename a file entry.
        @see store_rename()
     */
    inline storeError rename (
        rtl::OUString const & rSrcPath, rtl::OUString const & rSrcName,
        rtl::OUString const & rDstPath, rtl::OUString const & rDstName) SAL_THROW(())
    {
        if (!m_hImpl)
            return store_E_InvalidHandle;

        return store_rename (m_hImpl, rSrcPath.pData, rSrcName.pData, rDstPath.pData, rDstName.pData);
    }

    /** Remove a file entry.
        @see store_remove()
     */
    inline storeError remove (
        rtl::OUString const & rPath, rtl::OUString const & rName) SAL_THROW(())
    {
        if (!m_hImpl)
            return store_E_InvalidHandle;

        return store_remove (m_hImpl, rPath.pData, rName.pData);
    }

private:
    /** Representation.
     */
    storeFileHandle m_hImpl;
};

/*========================================================================
 *
 * The End.
 *
 *======================================================================*/

} // namespace store

#endif /* !_STORE_STORE_HXX_ */




/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
