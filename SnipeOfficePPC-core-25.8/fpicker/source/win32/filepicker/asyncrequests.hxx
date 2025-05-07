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

#ifndef FPICKER_WIN32_VISTA_ASYNCREQUESTS_HXX
#define FPICKER_WIN32_VISTA_ASYNCREQUESTS_HXX

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include <cppuhelper/basemutex.hxx>
#include <comphelper/sequenceashashmap.hxx>
#include <osl/conditn.hxx>
#include <osl/thread.hxx>
#include <osl/time.h>
#include <queue>
#include <boost/shared_ptr.hpp>

//-----------------------------------------------------------------------------
// namespace
//-----------------------------------------------------------------------------

namespace css = ::com::sun::star;

namespace fpicker{
namespace win32{
namespace vista{

//-----------------------------------------------------------------------------
/** @todo document me
 */
class Request
{
    //-------------------------------------------------------------------------
    public:

        static const ::sal_Int32 WAIT_INFINITE = 0;

    //-------------------------------------------------------------------------
    // interface
    //-------------------------------------------------------------------------

    public:

        //---------------------------------------------------------------------
        explicit Request()
            : m_aJoiner   (  )
            , m_nRequest  (-1)
            , m_lArguments(  )
        {
            m_aJoiner.reset();
        }

        //---------------------------------------------------------------------
        virtual ~Request() {};

        //---------------------------------------------------------------------
        void setRequest(::sal_Int32 nRequest)
        {
            m_nRequest = nRequest;
        }

        //---------------------------------------------------------------------
        ::sal_Int32 getRequest()
        {
            return m_nRequest;
        }

        //---------------------------------------------------------------------
        void clearArguments()
        {
            m_lArguments.clear();
        }

        //---------------------------------------------------------------------
        template< class TArgumentType >
        void setArgument(const ::rtl::OUString& sName ,
                         const TArgumentType&   aValue)
        {
            m_lArguments[sName] <<= aValue;
        }

        //---------------------------------------------------------------------
        template< class TArgumentType >
        TArgumentType getArgumentOrDefault(const ::rtl::OUString& sName   ,
                                           const TArgumentType&   aDefault)
        {
            return m_lArguments.getUnpackedValueOrDefault(sName, aDefault);
        }

        //---------------------------------------------------------------------
        void wait(::sal_Int32 nMilliSeconds = WAIT_INFINITE);

        void waitProcessMessages();

        //---------------------------------------------------------------------
        void notify();

    //-------------------------------------------------------------------------
    // member
    //-------------------------------------------------------------------------

    private:

        ::osl::Condition m_aJoiner;
        ::sal_Int32 m_nRequest;
        ::comphelper::SequenceAsHashMap m_lArguments;
};

typedef ::boost::shared_ptr< Request > RequestRef;
typedef ::std::queue< RequestRef >  RequestQueue;

//-----------------------------------------------------------------------------
class RequestHandler
{
    public:
        virtual void before() = 0;
        virtual void doRequest(const RequestRef& rRequest) = 0;
        virtual void after() = 0;
};

typedef ::boost::shared_ptr< RequestHandler > RequestHandlerRef;

//-----------------------------------------------------------------------------
/** @todo docuemnt me
 */
class AsyncRequests : private ::cppu::BaseMutex
                    , public  ::osl::Thread
{
    public:
        static const ::sal_Int16  PROCESS_MESSAGES = 2;
        static const ::sal_Int16  BLOCKED       = 1;
        static const ::sal_Int16  NON_BLOCKED   = 0;

        //---------------------------------------------------------------------
        /** creates the new asynchronous request executor.
         */
        explicit AsyncRequests(const RequestHandlerRef& rHandler);

        void setHandler(const RequestHandlerRef& rHandler)
        {
            m_rHandler = rHandler;
        }

        //---------------------------------------------------------------------
        /** does nothing special / excepting to make sure our class wont be inline .-)
         */
        virtual ~AsyncRequests();

        //---------------------------------------------------------------------
        /** @todo document me
         */
        void triggerRequestProcessMessages (const RequestRef& rRequest);

        //---------------------------------------------------------------------
        /** @todo document me
         */
        void triggerRequestBlocked(const RequestRef& rRequest);

        //---------------------------------------------------------------------
        /** @todo document me
         */
        void triggerRequestNonBlocked(const RequestRef& rRequest);

        //---------------------------------------------------------------------
        /** @todo document me
         */
        void triggerRequestDirectly(const RequestRef& rRequest);

        //---------------------------------------------------------------------
        /** @todo document me
         */
        void triggerRequestThreadAware(const RequestRef& rRequest,
                                             ::sal_Int16  nWait   );

    private:

        //---------------------------------------------------------------------
        /** our STA .-)
         *  Will run between start() & finish(). Internaly it runs a loop ...
         *  waiting for requests. Every request will be executed synchronously
         *  in blocked mode.
         */
        virtual void SAL_CALL run();

    private:

        ::sal_Bool m_bFinish;
        RequestHandlerRef m_rHandler;
        RequestQueue m_lRequests;
};

} // namespace vista
} // namespace win32
} // namespace fpicker

#endif // FPICKER_WIN32_VISTA_ASYNCREQUESTS_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
