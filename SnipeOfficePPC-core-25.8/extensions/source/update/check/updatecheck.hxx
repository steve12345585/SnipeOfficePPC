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

#include <com/sun/star/beans/NamedValue.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/task/XInteractionHandler.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>

#include <osl/conditn.hxx>
#include <osl/thread.hxx>
#include <rtl/instance.hxx>
#include <salhelper/refobj.hxx>

#include "updateinfo.hxx"
#include "updatecheckconfiglistener.hxx"
#include "actionlistener.hxx"
#include "updatehdl.hxx"
#include "download.hxx"


class UpdateCheck;
class UpdateCheckConfig;

class UpdateCheckInitData {

public:
    inline rtl::Reference< UpdateCheck > SAL_CALL operator() () const;
};

class WorkerThread : public osl::Thread
{
public:
    virtual void SAL_CALL cancel() = 0;
};

class UpdateCheck :
    public UpdateCheckConfigListener,
    public IActionListener,
    public DownloadInteractionHandler,
    public salhelper::ReferenceObject,
    public rtl::StaticWithInit< rtl::Reference< UpdateCheck >, UpdateCheckInitData >
{
    SAL_DLLPUBLIC_EXPORT UpdateCheck();

    virtual ~UpdateCheck();

public:
    inline SAL_CALL operator rtl::Reference< UpdateCheckConfigListener > ()
        { return static_cast< UpdateCheckConfigListener * > (this); }

    void initialize(const com::sun::star::uno::Sequence<com::sun::star::beans::NamedValue>& rValues,
                    const com::sun::star::uno::Reference<com::sun::star::uno::XComponentContext>& xContext);

    /* Returns an instance of the specified service obtained from the specified
     * component context
     */

    static com::sun::star::uno::Reference< com::sun::star::uno::XInterface > createService(
        const rtl::OUString& aServiceName,
        const com::sun::star::uno::Reference<com::sun::star::uno::XComponentContext>& xContext);

    // Update internal update info member
    void setUpdateInfo(const UpdateInfo& aInfo);

    /* This method turns on the menubar icon, triggers the bubble window or
     * updates the dialog text when appropriate
     */
    void setUIState(UpdateState eState, bool suppressBubble = false);

    // Returns the UI state that matches rInfo best
    static UpdateState getUIState(const UpdateInfo& rInfo);

    // Check for updates failed
    void setCheckFailedState();

    // Executes the update check dialog for manual checks and downloads interaction
    void showDialog(bool forceCheck = false);

    // Returns true if the update dialog is currently showing
    bool isDialogShowing() const;
    bool shouldShowExtUpdDlg() const { return ( m_bShowExtUpdDlg && m_bHasExtensionUpdate ); }
    void showExtensionDialog();
    void setHasExtensionUpdates( bool bHasUpdates ) { m_bHasExtensionUpdate = bHasUpdates; }
    bool hasOfficeUpdate() const { return (m_aUpdateInfo.BuildId.getLength() > 0); }

    // DownloadInteractionHandler
    virtual bool downloadTargetExists(const rtl::OUString& rFileName);
    virtual void downloadStalled(const rtl::OUString& rErrorMessage);
    virtual void downloadProgressAt(sal_Int8 nProcent);
    virtual void downloadStarted(const rtl::OUString& rLocalFileName, sal_Int64 nFileSize);
    virtual void downloadFinished(const rtl::OUString& rLocalFileName);
    // checks if the download target already exists and asks user what to do next
    virtual bool checkDownloadDestination( const rtl::OUString& rFile );

    // Cancels the download action (and resumes checking if enabled)
    void cancelDownload();

    // Returns the XInteractionHandler of the UpdateHandler instance if present (and visible)
    SAL_DLLPUBLIC_EXPORT com::sun::star::uno::Reference< com::sun::star::task::XInteractionHandler > getInteractionHandler() const;

    // UpdateCheckConfigListener
    virtual void autoCheckStatusChanged(bool enabled);
    virtual void autoCheckIntervalChanged();

    // IActionListener
    void cancel();
    void download();
    void install();
    void pause();
    void resume();
    void closeAfterFailure();

    // rtl::IReference
    virtual oslInterlockedCount SAL_CALL acquire() SAL_THROW(());
    virtual oslInterlockedCount SAL_CALL release() SAL_THROW(());

private:

    // Schedules or cancels next automatic check for updates
    void enableAutoCheck(bool enable);

    // Starts/resumes or stops a download
    void enableDownload(bool enable, bool paused=false);

    // Shuts down the currently running thread
    void shutdownThread(bool join);

    // Returns the update handler instance
    rtl::Reference<UpdateHandler> getUpdateHandler();

    // Open the given URL in a browser
    void showReleaseNote(const rtl::OUString& rURL) const;

    // stores the release note url on disk to be used by setup app
    static bool storeReleaseNote(sal_Int8 nNum, const rtl::OUString &rURL);

    /* This method turns on the menubar icon and triggers the bubble window
     */
    void handleMenuBarUI( rtl::Reference< UpdateHandler > rUpdateHandler,
                          UpdateState& eState, bool suppressBubble );
    enum State {
        NOT_INITIALIZED,
        DISABLED,
        CHECK_SCHEDULED,
        DOWNLOADING,
        DOWNLOAD_PAUSED
    };

    State m_eState;
    UpdateState m_eUpdateState;

    mutable osl::Mutex m_aMutex;
    WorkerThread *m_pThread;
    osl::Condition m_aCondition;

    UpdateInfo m_aUpdateInfo;
    rtl::OUString m_aImageName;
    bool m_bHasExtensionUpdate;
    bool m_bShowExtUpdDlg;

    rtl::Reference<UpdateHandler> m_aUpdateHandler;
    com::sun::star::uno::Reference<com::sun::star::beans::XPropertySet> m_xMenuBarUI;
    com::sun::star::uno::Reference<com::sun::star::uno::XComponentContext> m_xContext;

    friend class UpdateCheckInitData;
};

inline rtl::Reference< UpdateCheck > SAL_CALL
UpdateCheckInitData::operator() () const
{
    return rtl::Reference< UpdateCheck > (new UpdateCheck());
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
