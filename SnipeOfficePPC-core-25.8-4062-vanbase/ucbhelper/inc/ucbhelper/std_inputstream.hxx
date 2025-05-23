/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the SnipeOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef _UCBHELPER_STD_INPUTSTREAM_HXX_
#define _UCBHELPER_STD_INPUTSTREAM_HXX_

#include <boost/shared_ptr.hpp>
#include <istream>

#include <rtl/ustring.hxx>
#include <osl/mutex.hxx>
#include <cppuhelper/weak.hxx>
#include <cppuhelper/queryinterface.hxx>
#include <com/sun/star/io/XInputStream.hpp>
#include <com/sun/star/io/XSeekable.hpp>

#include "ucbhelper/ucbhelperdllapi.h"

namespace ucbhelper
{
    /** Implements a seekable InputStream
     *  working on an std::istream
     */
    class UCBHELPER_DLLPUBLIC StdInputStream
        : public cppu::OWeakObject,
          public com::sun::star::io::XInputStream,
          public com::sun::star::io::XSeekable
    {
        public:

            StdInputStream( boost::shared_ptr< std::istream > pStream );

            ~StdInputStream();

            virtual css::uno::Any SAL_CALL queryInterface ( const css::uno::Type& rType )
                throw ( css::uno::RuntimeException );

            virtual void SAL_CALL acquire( ) throw ( );

            virtual void SAL_CALL release( ) throw ( );

            virtual sal_Int32 SAL_CALL
            readBytes ( css::uno::Sequence< sal_Int8 >& aData,
                        sal_Int32 nBytesToRead )
                throw ( css::io::NotConnectedException,
                        css::io::BufferSizeExceededException,
                        css::io::IOException,
                        css::uno::RuntimeException );

            virtual sal_Int32 SAL_CALL
            readSomeBytes ( css::uno::Sequence< sal_Int8 >& aData,
                           sal_Int32 nMaxBytesToRead )
                throw ( css::io::NotConnectedException,
                        css::io::BufferSizeExceededException,
                        css::io::IOException,
                        css::uno::RuntimeException );

            virtual void SAL_CALL
            skipBytes ( sal_Int32 nBytesToSkip )
                throw ( css::io::NotConnectedException,
                        css::io::BufferSizeExceededException,
                        css::io::IOException,
                        css::uno::RuntimeException );

            virtual sal_Int32 SAL_CALL
            available ( )
                throw ( css::io::NotConnectedException,
                        css::io::IOException,
                        css::uno::RuntimeException );

            virtual void SAL_CALL
            closeInput ( )
                throw ( css::io::NotConnectedException,
                        css::io::IOException,
                        css::uno::RuntimeException );


            /** XSeekable
             */

            virtual void SAL_CALL
            seek ( sal_Int64 location )
                throw ( css::lang::IllegalArgumentException,
                        css::io::IOException,
                        css::uno::RuntimeException );


            virtual sal_Int64 SAL_CALL
            getPosition ( )
                throw ( css::io::IOException, css::uno::RuntimeException );


            virtual sal_Int64 SAL_CALL
            getLength ( )
                throw ( css::io::IOException,
                        css::uno::RuntimeException );

        private:

            osl::Mutex m_aMutex;
            boost::shared_ptr< std::istream > m_pStream;
            sal_Int64 m_nLength;
    };

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
