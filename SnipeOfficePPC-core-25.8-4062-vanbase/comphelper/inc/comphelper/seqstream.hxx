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
#ifndef _COMPHELPER_SEQSTREAM_HXX
#define _COMPHELPER_SEQSTREAM_HXX

#include <com/sun/star/uno/Sequence.hxx>
#include <com/sun/star/uno/Reference.hxx>
#include <com/sun/star/io/XInputStream.hpp>
#include <com/sun/star/io/XOutputStream.hpp>
#include <com/sun/star/io/XSeekable.hpp>
#include <osl/mutex.hxx>
#include <cppuhelper/implbase1.hxx>
#include <cppuhelper/implbase2.hxx>
#include "comphelper/comphelperdllapi.h"

namespace comphelper
{

    typedef ::com::sun::star::uno::Sequence<sal_Int8> ByteSequence;

//==================================================================
// SequenceInputStream
// stream for reading data from a sequence of bytes
//==================================================================


class COMPHELPER_DLLPUBLIC SequenceInputStream
: public ::cppu::WeakImplHelper2< ::com::sun::star::io::XInputStream, ::com::sun::star::io::XSeekable >
{
    ::osl::Mutex    m_aMutex;
    ByteSequence    m_aData;
    sal_Int32       m_nPos;

public:
    SequenceInputStream(const ByteSequence& rData);

// com::sun::star::io::XInputStream
    virtual sal_Int32 SAL_CALL readBytes( ::com::sun::star::uno::Sequence<sal_Int8>& aData, sal_Int32 nBytesToRead )
        throw(::com::sun::star::io::NotConnectedException, ::com::sun::star::io::BufferSizeExceededException,
              ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);

    virtual sal_Int32 SAL_CALL readSomeBytes( ::com::sun::star::uno::Sequence<sal_Int8>& aData, sal_Int32 nMaxBytesToRead )
        throw(::com::sun::star::io::NotConnectedException, ::com::sun::star::io::BufferSizeExceededException,
              ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);

    virtual void SAL_CALL skipBytes( sal_Int32 nBytesToSkip )
        throw(::com::sun::star::io::NotConnectedException, ::com::sun::star::io::BufferSizeExceededException,
              ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);

    virtual sal_Int32 SAL_CALL available(  )
        throw(::com::sun::star::io::NotConnectedException, ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);

    virtual void SAL_CALL closeInput(  )
        throw(::com::sun::star::io::NotConnectedException, ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);

    virtual void SAL_CALL seek( sal_Int64 location ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);
    virtual sal_Int64 SAL_CALL getPosition(  ) throw (::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);
    virtual sal_Int64 SAL_CALL getLength(  ) throw (::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);

private:
    inline sal_Int32 avail();
};
typedef ::cppu::WeakImplHelper1< ::com::sun::star::io::XOutputStream > OSequenceOutputStream_Base;

class OSequenceOutputStream : public OSequenceOutputStream_Base
{
protected:
    ::com::sun::star::uno::Sequence< sal_Int8 >&    m_rSequence;
    double                                          m_nResizeFactor;
    sal_Int32                                       m_nMinimumResize;
    sal_Int32                                       m_nMaximumResize;
    sal_Int32                                       m_nSize;
        // the size of the virtual stream. This is not the size of the sequence, but the number of bytes written
        // into the stream at a given moment.

    sal_Bool                                        m_bConnected;
        // closeOutput has been called ?

    ::osl::Mutex                                    m_aMutex;

protected:
    ~OSequenceOutputStream() { if (m_bConnected) closeOutput(); }

public:
    /** constructs the object. Everything written into the stream through the XOutputStream methods will be forwarded
        to the sequence, reallocating it if neccessary. Writing will start at offset 0 within the sequence.
        @param      _rSeq               a reference to the sequence which will be used for output.
                                        The caller is responsible for taking care of the lifetime of the stream
                                        object and the sequence. If you're in doubt about this, use <code>closeOutput</code>
                                        before destroying the sequence
        @param      _nResizeFactor      the factor which is used for resizing the sequence when neccessary. In every
                                        resize step, the new sequence size will be calculated by multiplying the current
                                        size with this factor, rounded off to the next multiple of 4.
        @param      _nMinimumResize     the minmal number of bytes which is additionally allocated on resizing
        @param      _nMaximumResize     as the growth of the stream size is exponential, you may want to specify a
                                        maxmimum amount of memory which the sequence will grow by. If -1 is used,
                                        no limit is applied
        @see        closeOutput
    */
    OSequenceOutputStream(
        ::com::sun::star::uno::Sequence< sal_Int8 >& _rSeq,
        double _nResizeFactor = 1.3,
        sal_Int32 _nMinimumResize = 128,
        sal_Int32 _nMaximumResize = -1
        );

    /// same as XOutputStream::writeBytes (as expected :)
    virtual void SAL_CALL writeBytes( const ::com::sun::star::uno::Sequence< sal_Int8 >& aData ) throw(::com::sun::star::io::NotConnectedException, ::com::sun::star::io::BufferSizeExceededException, ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);
    /// this is a dummy in this implementation, no buffering is used
    virtual void SAL_CALL flush(  ) throw(::com::sun::star::io::NotConnectedException, ::com::sun::star::io::BufferSizeExceededException, ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);
    /** closes the output stream. In the case of this class, this means that the sequence used for writing is
        resized to the really used size and not used any further, every subsequent call to one of the XOutputStream
        methods will throw a <code>NotConnectedException</code>.
    */
    virtual void SAL_CALL closeOutput(  ) throw(::com::sun::star::io::NotConnectedException, ::com::sun::star::io::BufferSizeExceededException, ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);
};

} // namespace comphelper

#endif //_COMPHELPER_SEQSTREAM_HXX


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
