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

#ifndef INCLUDED_DP_EXTENSIONMANAGER_H
#define INCLUDED_DP_EXTENSIONMANAGER_H

#include "dp_manager.hrc"
#include "dp_misc.h"
#include "dp_interact.h"
#include "dp_activepackages.hxx"
#include "rtl/ref.hxx"
#include "cppuhelper/compbase1.hxx"
#include "ucbhelper/content.hxx"
#include "com/sun/star/deployment/XPackageRegistry.hpp"
#include "com/sun/star/deployment/XPackageManager.hpp"
#include "osl/mutex.hxx"
#include <list>
#include <boost/unordered_map.hpp>

namespace dp_manager {

typedef ::boost::unordered_map<
    ::rtl::OUString,
    ::std::vector<css::uno::Reference<css::deployment::XPackage> >,
    ::rtl::OUStringHash > id2extensions;

class ExtensionManager : private ::dp_misc::MutexHolder,
        public ::cppu::WeakComponentImplHelper1< css::deployment::XExtensionManager >
{
public:
    ExtensionManager( css::uno::Reference< css::uno::XComponentContext >const& xContext);
    virtual     ~ExtensionManager();

    static css::uno::Sequence< ::rtl::OUString > getServiceNames();
    static ::rtl::OUString getImplName();

    void check();
    void fireModified();

public:

//    XModifyBroadcaster
    virtual void SAL_CALL addModifyListener(
       css::uno::Reference<css::util::XModifyListener> const & xListener )
       throw (css::uno::RuntimeException);
    virtual void SAL_CALL removeModifyListener(
       css::uno::Reference<css::util::XModifyListener> const & xListener )
       throw (css::uno::RuntimeException);

//XExtensionManager
    virtual css::uno::Sequence<
        css::uno::Reference<css::deployment::XPackageTypeInfo> > SAL_CALL
        getSupportedPackageTypes()
            throw (css::uno::RuntimeException);

    virtual css::uno::Reference<css::task::XAbortChannel> SAL_CALL
    createAbortChannel() throw (css::uno::RuntimeException);

    virtual css::uno::Reference<css::deployment::XPackage> SAL_CALL addExtension(
        ::rtl::OUString const & url,
        css::uno::Sequence<css::beans::NamedValue> const & properties,
        ::rtl::OUString const & repository,
        css::uno::Reference<css::task::XAbortChannel> const & xAbortChannel,
        css::uno::Reference<css::ucb::XCommandEnvironment> const & xCmdEnv )
        throw (css::deployment::DeploymentException,
               css::ucb::CommandFailedException,
               css::ucb::CommandAbortedException,
               css::lang::IllegalArgumentException,
               css::uno::RuntimeException);

    virtual void SAL_CALL removeExtension(
        ::rtl::OUString const & identifier,
        ::rtl::OUString const & filename,
        ::rtl::OUString const & repository,
        css::uno::Reference<css::task::XAbortChannel> const & xAbortChannel,
        css::uno::Reference<css::ucb::XCommandEnvironment> const & xCmdEnv )
        throw (css::deployment::DeploymentException,
            css::ucb::CommandFailedException,
            css::ucb::CommandAbortedException,
            css::lang::IllegalArgumentException,
            css::uno::RuntimeException);

    virtual void SAL_CALL enableExtension(
        css::uno::Reference<css::deployment::XPackage> const & extension,
        css::uno::Reference<css::task::XAbortChannel> const & xAbortChannel,
        css::uno::Reference<css::ucb::XCommandEnvironment> const & xCmdEnv )
        throw (css::deployment::DeploymentException,
            css::ucb::CommandFailedException,
            css::ucb::CommandAbortedException,
            css::lang::IllegalArgumentException,
            css::uno::RuntimeException);

    virtual void SAL_CALL disableExtension(
        css::uno::Reference<css::deployment::XPackage> const & extension,
        css::uno::Reference<css::task::XAbortChannel> const & xAbortChannel,
        css::uno::Reference<css::ucb::XCommandEnvironment> const & xCmdEnv )
        throw (css::deployment::DeploymentException,
            css::ucb::CommandFailedException,
            css::ucb::CommandAbortedException,
            css::lang::IllegalArgumentException,
            css::uno::RuntimeException);

    virtual sal_Int32 SAL_CALL checkPrerequisitesAndEnable(
        css::uno::Reference<css::deployment::XPackage> const & extension,
        css::uno::Reference<css::task::XAbortChannel> const & xAbortChannel,
        css::uno::Reference<css::ucb::XCommandEnvironment> const & xCmdEnv )
        throw (css::deployment::DeploymentException,
            css::ucb::CommandFailedException,
            css::ucb::CommandAbortedException,
            css::lang::IllegalArgumentException,
            css::uno::RuntimeException);

    virtual css::uno::Sequence< css::uno::Reference<css::deployment::XPackage> >
        SAL_CALL getDeployedExtensions(
        ::rtl::OUString const & repository,
        css::uno::Reference<css::task::XAbortChannel> const &,
        css::uno::Reference<css::ucb::XCommandEnvironment> const & xCmdEnv )
        throw (css::deployment::DeploymentException,
            css::ucb::CommandFailedException,
            css::ucb::CommandAbortedException,
            css::lang::IllegalArgumentException,
            css::uno::RuntimeException);

    virtual css::uno::Reference< css::deployment::XPackage>
        SAL_CALL getDeployedExtension(
        ::rtl::OUString const & repository,
        ::rtl::OUString const & identifier,
        ::rtl::OUString const & filename,
        css::uno::Reference< css::ucb::XCommandEnvironment> const & xCmdEnv )
        throw (
            css::deployment::DeploymentException,
            css::ucb::CommandFailedException,
            css::lang::IllegalArgumentException,
            css::uno::RuntimeException);

    virtual css::uno::Sequence<css::uno::Reference<css::deployment::XPackage> >
    SAL_CALL getExtensionsWithSameIdentifier(
        ::rtl::OUString const & identifier,
        ::rtl::OUString const & filename,
        css::uno::Reference< css::ucb::XCommandEnvironment> const & xCmdEnv )
        throw (
            css::deployment::DeploymentException,
            css::ucb::CommandFailedException,
            css::lang::IllegalArgumentException,
            css::uno::RuntimeException);

    virtual css::uno::Sequence< css::uno::Sequence<css::uno::Reference<css::deployment::XPackage> > >
        SAL_CALL getAllExtensions(
        css::uno::Reference<css::task::XAbortChannel> const &,
        css::uno::Reference<css::ucb::XCommandEnvironment> const & xCmdEnv )
        throw (css::deployment::DeploymentException,
            css::ucb::CommandFailedException,
            css::ucb::CommandAbortedException,
            css::lang::IllegalArgumentException,
            css::uno::RuntimeException);

    virtual void SAL_CALL reinstallDeployedExtensions(
        sal_Bool force, ::rtl::OUString const & repository,
        css::uno::Reference< css::task::XAbortChannel> const & xAbortChannel,
        css::uno::Reference< css::ucb::XCommandEnvironment> const & xCmdEnv )
        throw (
            css::deployment::DeploymentException,
            css::ucb::CommandFailedException,
            css::ucb::CommandAbortedException,
            css::lang::IllegalArgumentException,
            css::uno::RuntimeException);

    virtual sal_Bool SAL_CALL synchronize(
        css::uno::Reference<css::task::XAbortChannel> const & xAbortChannel,
        css::uno::Reference<css::ucb::XCommandEnvironment> const & xCmdEnv )
        throw (css::deployment::DeploymentException,
               css::ucb::CommandFailedException,
               css::ucb::CommandAbortedException,
               css::lang::IllegalArgumentException,
               css::uno::RuntimeException);

    virtual css::uno::Sequence<css::uno::Reference<css::deployment::XPackage> > SAL_CALL
    getExtensionsWithUnacceptedLicenses(
        ::rtl::OUString const & repository,
        css::uno::Reference<css::ucb::XCommandEnvironment> const & xCmdEnv)
        throw (css::deployment::DeploymentException,
               css::uno::RuntimeException);

    virtual sal_Bool SAL_CALL isReadOnlyRepository(::rtl::OUString const & repository)
        throw (css::uno::RuntimeException);

private:

    struct StrSyncRepository : public ::dp_misc::StaticResourceString<
        StrSyncRepository, RID_STR_SYNCHRONIZING_REPOSITORY> {};

    struct ExtensionInfos
    {
        ::rtl::OUString identifier;
        ::rtl::OUString fileName;
        ::rtl::OUString displayName;
        ::rtl::OUString version;
    };

    css::uno::Reference< css::uno::XComponentContext> m_xContext;
    css::uno::Reference<css::deployment::XPackageManagerFactory> m_xPackageManagerFactory;

    //only to be used within addExtension
    ::osl::Mutex m_addMutex;
    /* contains the names of all repositories (except tmp) in order of there
       priority. That is, the first element is "user" follod by "shared" and
       then "bundled"
     */
    ::std::list< ::rtl::OUString > m_repositoryNames;

    css::uno::Reference<css::deployment::XPackageManager> getUserRepository();
    css::uno::Reference<css::deployment::XPackageManager> getSharedRepository();
    css::uno::Reference<css::deployment::XPackageManager> getBundledRepository();
    css::uno::Reference<css::deployment::XPackageManager> getTmpRepository();
    css::uno::Reference<css::deployment::XPackageManager> getBakRepository();

    bool isUserDisabled(::rtl::OUString const & identifier,
                        ::rtl::OUString const & filename);

    bool isUserDisabled(
        css::uno::Sequence<css::uno::Reference<css::deployment::XPackage> > const & seqExtSameId);

    void activateExtension(
        ::rtl::OUString const & identifier,
        ::rtl::OUString const & fileName,
        bool bUserDisabled, bool bStartup,
        css::uno::Reference<css::task::XAbortChannel> const & xAbortChannel,
        css::uno::Reference<css::ucb::XCommandEnvironment> const & xCmdEnv);

    void activateExtension(
        css::uno::Sequence<css::uno::Reference<css::deployment::XPackage> > const & seqExt,
        bool bUserDisabled, bool bStartup,
        css::uno::Reference<css::task::XAbortChannel> const & xAbortChannel,
        css::uno::Reference<css::ucb::XCommandEnvironment> const & xCmdEnv );

    ::std::list<css::uno::Reference<css::deployment::XPackage> >
    getExtensionsWithSameId(::rtl::OUString  const & identifier,
                            ::rtl::OUString const & fileName,
                            css::uno::Reference< css::ucb::XCommandEnvironment> const & xCmdEnv =
                            css::uno::Reference< css::ucb::XCommandEnvironment>());

    css::uno::Reference<css::deployment::XPackage> backupExtension(
        ::rtl::OUString const & identifier, ::rtl::OUString const & fileName,
        css::uno::Reference<css::deployment::XPackageManager> const & xPackageManager,
        css::uno::Reference<css::ucb::XCommandEnvironment> const & xCmdEnv);

    void checkInstall(
        ::rtl::OUString const & displayName,
        css::uno::Reference<css::ucb::XCommandEnvironment> const & cmdEnv);

    void checkUpdate(
        ::rtl::OUString const & newVersion,
        ::rtl::OUString const & newDisplayName,
        css::uno::Reference<css::deployment::XPackage> const & oldExtension,
        css::uno::Reference<css::ucb::XCommandEnvironment> const & xCmdEnv);

    void addExtensionsToMap(
        id2extensions & mapExt,
        css::uno::Sequence<css::uno::Reference<css::deployment::XPackage> > const & seqExt,
        ::rtl::OUString const & repository);

    css::uno::Reference<css::deployment::XPackageManager>
    getPackageManager(::rtl::OUString const & repository)
        throw (css::lang::IllegalArgumentException);

    bool doChecksForAddExtension(
        css::uno::Reference<css::deployment::XPackageManager> const & xPackageMgr,
        css::uno::Sequence<css::beans::NamedValue> const & properties,
        css::uno::Reference<css::deployment::XPackage> const & xTmpExtension,
        css::uno::Reference<css::task::XAbortChannel> const & xAbortChannel,
        css::uno::Reference<css::ucb::XCommandEnvironment> const & xCmdEnv,
        css::uno::Reference<css::deployment::XPackage> & out_existingExtension )
        throw (css::deployment::DeploymentException,
               css::ucb::CommandFailedException,
               css::ucb::CommandAbortedException,
               css::lang::IllegalArgumentException,
               css::uno::RuntimeException);

};

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
