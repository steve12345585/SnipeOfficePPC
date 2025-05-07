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

#ifndef _SFX_EVENTSUPPLIER_HXX_
#define _SFX_EVENTSUPPLIER_HXX_

#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/container/XNameReplace.hpp>
#include <com/sun/star/container/XSet.hpp>
#include <com/sun/star/document/XEventListener.hpp>
#include <com/sun/star/document/XEventBroadcaster.hpp>
#include <com/sun/star/document/XDocumentEventBroadcaster.hpp>
#include <com/sun/star/document/XDocumentEventListener.hpp>
#include <com/sun/star/document/XEventsSupplier.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/task/XJobExecutor.hpp>
#include <com/sun/star/uno/Reference.hxx>
#include <com/sun/star/uno/Type.hxx>
#include <sal/types.h>
#include <osl/mutex.hxx>
#include <cppuhelper/weak.hxx>
#include <cppuhelper/implbase1.hxx>
#include <cppuhelper/implbase2.hxx>
#include <cppuhelper/implbase3.hxx>
#include <cppuhelper/implbase4.hxx>
#include <cppuhelper/implbase7.hxx>
#include <comphelper/sequenceashashmap.hxx>
#include <comphelper/sequenceasvector.hxx>
#include <sfx2/sfxuno.hxx>

#include <cppuhelper/interfacecontainer.hxx>
#include <svl/lstner.hxx>
#include <unotools/eventcfg.hxx>

namespace comphelper
{
    class NamedValueCollection;
}

//--------------------------------------------------------------------------------------------------------

#define NOSUCHELEMENTEXCEPTION      ::com::sun::star::container::NoSuchElementException
#define XNAMEREPLACE                ::com::sun::star::container::XNameReplace
#define DOCEVENTOBJECT              ::com::sun::star::document::EventObject
#define XEVENTBROADCASTER           ::com::sun::star::document::XEventBroadcaster
#define XDOCEVENTLISTENER           ::com::sun::star::document::XEventListener
#define XEVENTSSUPPLIER             ::com::sun::star::document::XEventsSupplier
#define XJOBEXECUTOR                ::com::sun::star::task::XJobExecutor
#define EVENTOBJECT                 ::com::sun::star::lang::EventObject
#define ILLEGALARGUMENTEXCEPTION    ::com::sun::star::lang::IllegalArgumentException
#define WRAPPEDTARGETEXCEPTION      ::com::sun::star::lang::WrappedTargetException
#define ANY                         ::com::sun::star::uno::Any
#define REFERENCE                   ::com::sun::star::uno::Reference
#define WEAKREFERENCE               ::com::sun::star::uno::WeakReference
#define RUNTIMEEXCEPTION            ::com::sun::star::uno::RuntimeException
#define SEQUENCE                    ::com::sun::star::uno::Sequence
#define UNOTYPE                     ::com::sun::star::uno::Type
#define OINTERFACECONTAINERHELPER   ::cppu::OInterfaceContainerHelper

//--------------------------------------------------------------------------------------------------------

class SfxObjectShell;
class SfxBaseModel;
class SvxMacro;

//--------------------------------------------------------------------------------------------------------

class SfxEvents_Impl : public ::cppu::WeakImplHelper2< ::com::sun::star::container::XNameReplace, ::com::sun::star::document::XEventListener  >
{
    SEQUENCE< ::rtl::OUString >     maEventNames;
    SEQUENCE< ANY >                 maEventData;
    REFERENCE< XEVENTBROADCASTER >  mxBroadcaster;
    ::osl::Mutex                    maMutex;
    SfxObjectShell                 *mpObjShell;

    sal_Bool                    Warn_Impl( const String& );

public:
                                SfxEvents_Impl( SfxObjectShell* pShell,
                                                REFERENCE< XEVENTBROADCASTER > xBroadcaster );
                               ~SfxEvents_Impl();

    //  --- XNameReplace ---
    virtual void SAL_CALL       replaceByName( const ::rtl::OUString & aName, const ANY & aElement )
                                    throw( ILLEGALARGUMENTEXCEPTION, NOSUCHELEMENTEXCEPTION,
                                           WRAPPEDTARGETEXCEPTION, RUNTIMEEXCEPTION );

    //  --- XNameAccess ( parent of XNameReplace ) ---
    virtual ANY SAL_CALL        getByName( const ::rtl::OUString& aName )
                                    throw( NOSUCHELEMENTEXCEPTION, WRAPPEDTARGETEXCEPTION,
                                           RUNTIMEEXCEPTION );
    virtual SEQUENCE< ::rtl::OUString > SAL_CALL getElementNames() throw ( RUNTIMEEXCEPTION );
    virtual sal_Bool SAL_CALL   hasByName( const ::rtl::OUString& aName ) throw ( RUNTIMEEXCEPTION );

    //  --- XElementAccess ( parent of XNameAccess ) ---
    virtual UNOTYPE SAL_CALL    getElementType() throw ( RUNTIMEEXCEPTION );
    virtual sal_Bool SAL_CALL   hasElements() throw ( RUNTIMEEXCEPTION );

    // --- ::document::XEventListener ---
    virtual void SAL_CALL       notifyEvent( const DOCEVENTOBJECT& aEvent )
                                    throw( RUNTIMEEXCEPTION );

    // --- ::lang::XEventListener ---
    virtual void SAL_CALL       disposing( const EVENTOBJECT& Source )
                                    throw( RUNTIMEEXCEPTION );

    static SvxMacro*            ConvertToMacro( const ANY& rElement, SfxObjectShell* pDoc, sal_Bool bNormalizeMacro );
    static void                 NormalizeMacro( const ANY& rIn, ANY& rOut, SfxObjectShell* pDoc );
    static void                 NormalizeMacro(
                                    const ::comphelper::NamedValueCollection& i_eventDescriptor,
                                    ::comphelper::NamedValueCollection& o_normalizedDescriptor,
                                    SfxObjectShell* i_document );
};

//=============================================================================
struct ModelCollectionMutexBase
{
    public:
        ::osl::Mutex m_aLock;
};

//=============================================================================
typedef ::std::vector< ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel > > TModelList;

//=============================================================================
class ModelCollectionEnumeration : public ModelCollectionMutexBase
                                 , public ::cppu::WeakImplHelper1< ::com::sun::star::container::XEnumeration >
{

    //-------------------------------------------------------------------------
    // member
    //-------------------------------------------------------------------------
    private:
        ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > m_xSMGR;
        TModelList m_lModels;
        TModelList::iterator m_pEnumerationIt;

    //-------------------------------------------------------------------------
    // native interface
    //-------------------------------------------------------------------------
    public:
        ModelCollectionEnumeration(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xSMGR);
        virtual ~ModelCollectionEnumeration();
        void setModelList(const TModelList& rList);

    //-------------------------------------------------------------------------
    // uno interface
    //-------------------------------------------------------------------------
    public:

        // css.container.XEnumeration
        virtual sal_Bool SAL_CALL hasMoreElements()
            throw(::com::sun::star::uno::RuntimeException);

        virtual ::com::sun::star::uno::Any SAL_CALL nextElement()
            throw(::com::sun::star::container::NoSuchElementException,
                  ::com::sun::star::lang::WrappedTargetException     ,
                  ::com::sun::star::uno::RuntimeException            );
};

//=============================================================================
class SfxGlobalEvents_Impl : public ModelCollectionMutexBase
                           , public ::cppu::WeakImplHelper7< ::com::sun::star::lang::XServiceInfo
                                                           , ::com::sun::star::document::XEventsSupplier
                                                           , ::com::sun::star::document::XEventBroadcaster
                                                           , ::com::sun::star::document::XDocumentEventBroadcaster
                                                           , ::com::sun::star::document::XEventListener
                                                           , ::com::sun::star::document::XDocumentEventListener
                                                           , ::com::sun::star::container::XSet >
{
    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > m_xSMGR;
    ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameReplace > m_xEvents;
    ::com::sun::star::uno::WeakReference< ::com::sun::star::document::XEventListener > m_xJobExecutorListener;
    OINTERFACECONTAINERHELPER m_aLegacyListeners;
    OINTERFACECONTAINERHELPER m_aDocumentListeners;
    TModelList m_lModels;
    GlobalEventConfig* pImp;

public:
    SfxGlobalEvents_Impl(const com::sun::star::uno::Reference < ::com::sun::star::lang::XMultiServiceFactory >& xSMGR);
    virtual ~SfxGlobalEvents_Impl();

    SFX_DECL_XSERVICEINFO

    // css.document.XEventBroadcaster
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameReplace > SAL_CALL getEvents()
        throw(::com::sun::star::uno::RuntimeException);

    virtual void SAL_CALL addEventListener(const ::com::sun::star::uno::Reference< ::com::sun::star::document::XEventListener >& xListener)
        throw(::com::sun::star::uno::RuntimeException);

    virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::document::XEventListener >& xListener)
        throw(::com::sun::star::uno::RuntimeException);

    // css.document.XDocumentEventBroadcaster
    virtual void SAL_CALL addDocumentEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::document::XDocumentEventListener >& _Listener ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removeDocumentEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::document::XDocumentEventListener >& _Listener ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL notifyDocumentEvent( const ::rtl::OUString& _EventName, const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XController2 >& _ViewController, const ::com::sun::star::uno::Any& _Supplement ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::NoSupportException, ::com::sun::star::uno::RuntimeException);

    // css.document.XEventListener
    virtual void SAL_CALL notifyEvent(const ::com::sun::star::document::EventObject& aEvent)
        throw(::com::sun::star::uno::RuntimeException);

    // css.document.XDocumentEventListener
    virtual void SAL_CALL documentEventOccured( const ::com::sun::star::document::DocumentEvent& Event ) throw (::com::sun::star::uno::RuntimeException);

    // css.container.XSet
    virtual sal_Bool SAL_CALL has(const ::com::sun::star::uno::Any& aElement)
        throw(::com::sun::star::uno::RuntimeException);

    virtual void SAL_CALL insert(const ::com::sun::star::uno::Any& aElement)
        throw(::com::sun::star::lang::IllegalArgumentException  ,
              ::com::sun::star::container::ElementExistException,
              ::com::sun::star::uno::RuntimeException           );

    virtual void SAL_CALL remove(const ::com::sun::star::uno::Any& aElement)
        throw(::com::sun::star::lang::IllegalArgumentException   ,
              ::com::sun::star::container::NoSuchElementException,
              ::com::sun::star::uno::RuntimeException            );

    // css.container.XEnumerationAccess
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XEnumeration > SAL_CALL createEnumeration()
        throw(::com::sun::star::uno::RuntimeException);

    // css.container.XElementAccess
    virtual ::com::sun::star::uno::Type SAL_CALL getElementType()
        throw(::com::sun::star::uno::RuntimeException);

    virtual sal_Bool SAL_CALL hasElements()
        throw(::com::sun::star::uno::RuntimeException);

    // css.lang.XEventListener
    virtual void SAL_CALL disposing(const ::com::sun::star::lang::EventObject& aEvent)
        throw(::com::sun::star::uno::RuntimeException);

private:

    // threadsafe
    void implts_notifyJobExecution(const ::com::sun::star::document::EventObject& aEvent);
    void implts_checkAndExecuteEventBindings(const ::com::sun::star::document::DocumentEvent& aEvent);
    void implts_notifyListener(const ::com::sun::star::document::DocumentEvent& aEvent);

    // not threadsafe
    TModelList::iterator impl_searchDoc(const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel >& xModel);
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
