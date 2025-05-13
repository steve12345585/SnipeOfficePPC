/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
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

#include <swtypes.hxx>
#include <dbtablepreviewdialog.hxx>
#include <comphelper/processfactory.hxx>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/frame/XDispatchProvider.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <toolkit/unohlp.hxx>

#include <dbui.hrc>
#include <dbtablepreviewdialog.hrc>
#include <unomid.h>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::util;
using namespace ::rtl;

SwDBTablePreviewDialog::SwDBTablePreviewDialog(Window* pParent, uno::Sequence< beans::PropertyValue>& rValues ) :
    SfxModalDialog(pParent, SW_RES(DLG_MM_DBTABLEPREVIEWDIALOG)),
#ifdef MSC
#pragma warning (disable : 4355)
#endif
    m_aDescriptionFI( this, SW_RES(        FI_DESCRIPTION)),
    m_pBeamerWIN( new Window(this, SW_RES( WIN_BEAMER ))),
    m_aOK( this, SW_RES(                   PB_OK  ))
#ifdef MSC
#pragma warning (default : 4355)
#endif
{
    FreeResource();
    const beans::PropertyValue* pValues = rValues.getConstArray();
    for(sal_Int32 nValue = 0; nValue < rValues.getLength(); ++nValue        )
    {
        if ( pValues[nValue].Name == "Command" )
        {
            String sDescription = m_aDescriptionFI.GetText();
            OUString sTemp;
            pValues[nValue].Value >>= sTemp;
            sDescription.SearchAndReplaceAscii("%1", sTemp);
            m_aDescriptionFI.SetText(sDescription);
            break;
        }
    }

    try
    {
        // create a frame wrapper for myself
        uno::Reference< lang::XMultiServiceFactory >
                                    xMgr = comphelper::getProcessServiceFactory();
        m_xFrame = uno::Reference< frame::XFrame >(xMgr->createInstance(C2U("com.sun.star.frame.Frame")), uno::UNO_QUERY);
        if(m_xFrame.is())
        {
            m_xFrame->initialize( VCLUnoHelper::GetInterface ( m_pBeamerWIN ) );
        }
    }
    catch (uno::Exception const &)
    {
        m_xFrame.clear();
    }
    if(m_xFrame.is())
    {
        uno::Reference<frame::XDispatchProvider> xDP(m_xFrame, uno::UNO_QUERY);
        util::URL aURL;
        aURL.Complete = C2U(".component:DB/DataSourceBrowser");
        uno::Reference<frame::XDispatch> xD = xDP->queryDispatch(aURL,
                    C2U(""),
                    0x0C);
        if(xD.is())
        {
            xD->dispatch(aURL, rValues);
            m_pBeamerWIN->Show();
        }
    }
}

SwDBTablePreviewDialog::~SwDBTablePreviewDialog()
{
    if(m_xFrame.is())
    {
        m_xFrame->setComponent(NULL, NULL);
        m_xFrame->dispose();
    }
    else
        delete m_pBeamerWIN;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
