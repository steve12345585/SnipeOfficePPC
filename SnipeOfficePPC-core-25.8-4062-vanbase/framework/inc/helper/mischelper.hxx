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

#ifndef __MISC_HELPER_HXX_
#define __MISC_HELPER_HXX_

#include <com/sun/star/linguistic2/XLanguageGuessing.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/document/XEventListener.hpp>
#include <com/sun/star/lang/XEventListener.hpp>
#include <com/sun/star/util/XChangesListener.hpp>
#include <com/sun/star/container/XContainerListener.hpp>
#include <com/sun/star/frame/XFrame.hpp>

#include <cppuhelper/implbase1.hxx>

#include <i18npool/lang.h>
#include <svl/languageoptions.hxx>
#include <rtl/ustring.hxx>
#include <fwidllapi.h>

#include <set>

class SvtLanguageTable;


// flags for script types in use within selection
#define LS_SCRIPT_LATIN     0x0001
#define LS_SCRIPT_ASIAN     0x0002
#define LS_SCRIPT_COMPLEX   0x0004


namespace framework
{

// menu ids for language status bar control
enum LangMenuIDs
{
    MID_LANG_SEL_1 = 1,     // need to start with 1 since xPopupMenu->execute will return 0 if the menu is cancelled
    MID_LANG_SEL_2,
    MID_LANG_SEL_3,
    MID_LANG_SEL_4,
    MID_LANG_SEL_5,
    MID_LANG_SEL_6,
    MID_LANG_SEL_7,
    MID_LANG_SEL_8,
    MID_LANG_SEL_9,
    MID_LANG_SEL_NONE,
    MID_LANG_SEL_RESET,
    MID_LANG_SEL_MORE,

    MID_LANG_PARA_SEPERATOR,
    MID_LANG_PARA_STRING,

    MID_LANG_PARA_1,
    MID_LANG_PARA_2,
    MID_LANG_PARA_3,
    MID_LANG_PARA_4,
    MID_LANG_PARA_5,
    MID_LANG_PARA_6,
    MID_LANG_PARA_7,
    MID_LANG_PARA_8,
    MID_LANG_PARA_9,
    MID_LANG_PARA_NONE,
    MID_LANG_PARA_RESET,
    MID_LANG_PARA_MORE,
};


inline bool IsScriptTypeMatchingToLanguage( sal_Int16 nScriptType, LanguageType nLang )
{
    return 0 != (nScriptType & SvtLanguageOptions::GetScriptTypeOfLanguage( nLang ));
}


class FWI_DLLPUBLIC LanguageGuessingHelper
{
    mutable ::com::sun::star::uno::Reference< ::com::sun::star::linguistic2::XLanguageGuessing >    m_xLanguageGuesser;
    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > m_xServiceManager;

public:
    LanguageGuessingHelper(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _xServiceManager) : m_xServiceManager(_xServiceManager){}

    ::com::sun::star::uno::Reference< ::com::sun::star::linguistic2::XLanguageGuessing >  GetGuesser() const;
};

FWI_DLLPUBLIC ::rtl::OUString RetrieveLabelFromCommand( const ::rtl::OUString& aCmdURL
            ,const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >&    _xServiceFactory
            ,::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess >&        _xUICommandLabels
            ,const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& _xFrame
            ,::rtl::OUString& _rModuleIdentifier
            ,sal_Bool& _rIni
            ,const sal_Char* _pName);

FWI_DLLPUBLIC void FillLangItems( std::set< ::rtl::OUString > &rLangItems,
        const SvtLanguageTable &rLanguageTable,
        const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame > &rxFrame,
        const LanguageGuessingHelper & rLangGuessHelper,
        sal_Int16               nScriptType,
        const ::rtl::OUString & rCurLang,
        const ::rtl::OUString & rKeyboardLang,
        const ::rtl::OUString & rGuessedTextLang );

//It's common for an object to want to create and own a Broadcaster and set
//itself as a Listener on its own Broadcaster member.
//
//However, calling addListener on a Broadcaster means that the Broadcaster adds
//a reference to the Listener leading to an ownership cycle where the Listener
//owns the Broadcaster which "owns" the Listener.
//
//The WeakContainerListener allows breaking this cycle and retrofitting
//afflicted implentations fairly easily.
//
//OriginalListener owns the Broadcaster which "owns" the WeakContainerListener
//which forwards the events to the OriginalListener without taking ownership of
//it.
class WeakContainerListener : public ::cppu::WeakImplHelper1<com::sun::star::container::XContainerListener>
{
    private:
        com::sun::star::uno::WeakReference<com::sun::star::container::XContainerListener> mxOwner;

    public:
        WeakContainerListener(com::sun::star::uno::Reference<com::sun::star::container::XContainerListener> xOwner)
            : mxOwner(xOwner)
        {
        }

        virtual ~WeakContainerListener()
        {
        }

        // container.XContainerListener
        virtual void SAL_CALL elementInserted(const com::sun::star::container::ContainerEvent& rEvent)
            throw(com::sun::star::uno::RuntimeException)
        {
            com::sun::star::uno::Reference<com::sun::star::container::XContainerListener> xOwner(mxOwner.get(),
                com::sun::star::uno::UNO_QUERY);
            if (xOwner.is())
                xOwner->elementInserted(rEvent);
        }

        virtual void SAL_CALL elementRemoved(const com::sun::star::container::ContainerEvent& rEvent)
            throw(com::sun::star::uno::RuntimeException)
        {
            com::sun::star::uno::Reference<com::sun::star::container::XContainerListener> xOwner(mxOwner.get(),
                com::sun::star::uno::UNO_QUERY);
            if (xOwner.is())
                xOwner->elementRemoved(rEvent);
        }

        virtual void SAL_CALL elementReplaced(const com::sun::star::container::ContainerEvent& rEvent)
            throw(com::sun::star::uno::RuntimeException)
        {
            com::sun::star::uno::Reference<com::sun::star::container::XContainerListener> xOwner(mxOwner.get(),
                com::sun::star::uno::UNO_QUERY);
            if (xOwner.is())
                xOwner->elementReplaced(rEvent);
        }

        // lang.XEventListener
        virtual void SAL_CALL disposing(const com::sun::star::lang::EventObject& rEvent)
            throw(com::sun::star::uno::RuntimeException)
        {
            com::sun::star::uno::Reference<com::sun::star::container::XContainerListener> xOwner(mxOwner.get(),
                com::sun::star::uno::UNO_QUERY);
            if (xOwner.is())
                xOwner->disposing(rEvent);

        }
};

class WeakChangesListener : public ::cppu::WeakImplHelper1<com::sun::star::util::XChangesListener>
{
    private:
        com::sun::star::uno::WeakReference<com::sun::star::util::XChangesListener> mxOwner;

    public:
        WeakChangesListener(com::sun::star::uno::Reference<com::sun::star::util::XChangesListener> xOwner)
            : mxOwner(xOwner)
        {
        }

        virtual ~WeakChangesListener()
        {
        }

        // util.XChangesListener
        virtual void SAL_CALL changesOccurred(const com::sun::star::util::ChangesEvent& rEvent)
            throw(com::sun::star::uno::RuntimeException)
        {
            com::sun::star::uno::Reference<com::sun::star::util::XChangesListener> xOwner(mxOwner.get(),
                com::sun::star::uno::UNO_QUERY);
            if (xOwner.is())
                xOwner->changesOccurred(rEvent);
        }

        // lang.XEventListener
        virtual void SAL_CALL disposing(const com::sun::star::lang::EventObject& rEvent)
            throw(com::sun::star::uno::RuntimeException)
        {
            com::sun::star::uno::Reference<com::sun::star::util::XChangesListener> xOwner(mxOwner.get(),
                com::sun::star::uno::UNO_QUERY);
            if (xOwner.is())
                xOwner->disposing(rEvent);

        }
};

class WeakEventListener : public ::cppu::WeakImplHelper1<com::sun::star::lang::XEventListener>
{
    private:
        com::sun::star::uno::WeakReference<com::sun::star::lang::XEventListener> mxOwner;

    public:
        WeakEventListener(com::sun::star::uno::Reference<com::sun::star::lang::XEventListener> xOwner)
            : mxOwner(xOwner)
        {
        }

        virtual ~WeakEventListener()
        {
        }

        // lang.XEventListener
        virtual void SAL_CALL disposing(const com::sun::star::lang::EventObject& rEvent)
            throw(com::sun::star::uno::RuntimeException)
        {
            com::sun::star::uno::Reference<com::sun::star::lang::XEventListener> xOwner(mxOwner.get(),
                com::sun::star::uno::UNO_QUERY);
            if (xOwner.is())
                xOwner->disposing(rEvent);

        }
};

class WeakDocumentEventListener : public ::cppu::WeakImplHelper1<com::sun::star::document::XEventListener>
{
    private:
        com::sun::star::uno::WeakReference<com::sun::star::document::XEventListener> mxOwner;

    public:
        WeakDocumentEventListener(com::sun::star::uno::Reference<com::sun::star::document::XEventListener> xOwner)
            : mxOwner(xOwner)
        {
        }

        virtual ~WeakDocumentEventListener()
        {
        }

        virtual void SAL_CALL notifyEvent(const com::sun::star::document::EventObject& rEvent)
            throw(com::sun::star::uno::RuntimeException)
        {
            com::sun::star::uno::Reference<com::sun::star::document::XEventListener> xOwner(mxOwner.get(),
                com::sun::star::uno::UNO_QUERY);
            if (xOwner.is())
                xOwner->notifyEvent(rEvent);

        }

        // lang.XEventListener
        virtual void SAL_CALL disposing(const com::sun::star::lang::EventObject& rEvent)
            throw(com::sun::star::uno::RuntimeException)
        {
            com::sun::star::uno::Reference<com::sun::star::document::XEventListener> xOwner(mxOwner.get(),
                com::sun::star::uno::UNO_QUERY);
            if (xOwner.is())
                xOwner->disposing(rEvent);

        }
};


} // namespace framework

#endif // __MISC_HELPER_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
