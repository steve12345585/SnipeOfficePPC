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

#ifndef _SVTOOLS_FRAMESTATUSLISTENER_HXX
#define _SVTOOLS_FRAMESTATUSLISTENER_HXX

#include "svtools/svtdllapi.h"
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/util/XURLTransformer.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XFrameActionListener.hpp>
#include <com/sun/star/frame/XDispatch.hpp>
#include <com/sun/star/frame/XStatusListener.hpp>
#include <cppuhelper/weak.hxx>
#include <cppuhelper/interfacecontainer.hxx>
#include <comphelper/broadcasthelper.hxx>

#include <boost/unordered_map.hpp>

namespace svt
{

class SVT_DLLPUBLIC FrameStatusListener : public ::com::sun::star::frame::XStatusListener,
                            public ::com::sun::star::frame::XFrameActionListener,
                            public ::com::sun::star::lang::XComponent,
                            public ::comphelper::OBaseMutex,
                            public ::cppu::OWeakObject
{
    public:
        FrameStatusListener( const com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >& rServiceManager,
                             const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& xFrame );
        virtual ~FrameStatusListener();

        // methods to support status forwarder, known by the old sfx2 toolbox controller implementation
        void addStatusListener( const rtl::OUString& aCommandURL );
        void bindListener();
        void unbindListener();
        sal_Bool isBound() const;

        // XInterface
        virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& aType ) throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL acquire() throw ();
        virtual void SAL_CALL release() throw ();

        // XComponent
        virtual void SAL_CALL dispose() throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& xListener ) throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& aListener ) throw (::com::sun::star::uno::RuntimeException);

        // XEventListener
        virtual void SAL_CALL disposing( const com::sun::star::lang::EventObject& Source ) throw ( ::com::sun::star::uno::RuntimeException );

        // XStatusListener
        virtual void SAL_CALL statusChanged( const ::com::sun::star::frame::FeatureStateEvent& Event ) throw ( ::com::sun::star::uno::RuntimeException ) = 0;

        // XFrameActionListener
        virtual void SAL_CALL frameAction( const com::sun::star::frame::FrameActionEvent& Action ) throw ( ::com::sun::star::uno::RuntimeException );

    protected:
        struct Listener
        {
            Listener( const ::com::sun::star::util::URL& rURL, const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch >& rDispatch ) :
                aURL( rURL ), xDispatch( rDispatch ) {}

            ::com::sun::star::util::URL aURL;
            ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch > xDispatch;
        };

        typedef ::boost::unordered_map< ::rtl::OUString,
                                 com::sun::star::uno::Reference< com::sun::star::frame::XDispatch >,
                                 ::rtl::OUStringHash,
                                 ::std::equal_to< ::rtl::OUString > > URLToDispatchMap;

        sal_Bool                                                                            m_bInitialized : 1,
                                                                                            m_bDisposed : 1;
        ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >                 m_xFrame;
        ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >    m_xServiceManager;
        URLToDispatchMap                                                                    m_aListenerMap;
};

}

#endif // _SVTOOLS_FRAMESTATUSLISTENER_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
