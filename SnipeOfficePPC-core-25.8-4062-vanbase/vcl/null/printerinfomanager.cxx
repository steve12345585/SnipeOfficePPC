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

#include "vcl/printerinfomanager.hxx"

#include "generic/gendata.hxx"

using namespace psp;
using namespace osl;

using ::rtl::OUString;
using ::rtl::OString;
using ::rtl::OStringToOUString;
using ::rtl::OUStringHash;

PrinterInfoManager& PrinterInfoManager::get()
{
    SalData* pSalData = GetSalData();
    if( ! pSalData->m_pPIManager )
        pSalData->m_pPIManager = new PrinterInfoManager();
    return *pSalData->m_pPIManager;
}

void PrinterInfoManager::release()
{
    SalData* pSalData = GetSalData();
    delete pSalData->m_pPIManager;
    pSalData->m_pPIManager = NULL;
}

PrinterInfoManager::PrinterInfoManager( Type eType ) :
    m_pQueueInfo( NULL ),
    m_eType( eType ),
    m_bUseIncludeFeature( false ),
    m_bUseJobPatch( true ),
    m_aSystemDefaultPaper( RTL_CONSTASCII_USTRINGPARAM( "A4" ) ),
#ifdef LIBO_HEADLESS
    m_bDisableCUPS( true )
#else
    m_bDisableCUPS( false )
#endif
{
    // initSystemDefaultPaper();
}

PrinterInfoManager::~PrinterInfoManager()
{

}

bool PrinterInfoManager::checkPrintersChanged( bool /* bWait */ )
{
    return false;
}

void PrinterInfoManager::initialize()
{
    // ???
}

bool PrinterInfoManager::isCUPSDisabled() const
{
    return m_bDisableCUPS;
}

void PrinterInfoManager::setCUPSDisabled( bool /* bDisable */ )
{
    // cups is already disabled in config so do nothing
}

void PrinterInfoManager::listPrinters( ::std::list< OUString >& rList ) const
{
    rList.clear();
}

const PrinterInfo& PrinterInfoManager::getPrinterInfo( const OUString& /* rPrinter */ ) const
{
    static PrinterInfo aEmptyInfo;

    return aEmptyInfo;
}

void PrinterInfoManager::changePrinterInfo( const OUString& /* rPrinter */, const PrinterInfo& /* rNewInfo */ )
{

}

bool PrinterInfoManager::writePrinterConfig()
{
    return false;
}

bool PrinterInfoManager::addPrinter( const OUString& /* rPrinterName */, const OUString& /* rDriverName */ )
{
    return false;
}

bool PrinterInfoManager::removePrinter( const OUString& /* rPrinterName */, bool /* bCheckOnly */ )
{
    return false;
}

bool PrinterInfoManager::setDefaultPrinter( const OUString& /* rPrinterName */ )
{
    return false;
}

bool PrinterInfoManager::addOrRemovePossible() const
{
    return false;
}

void PrinterInfoManager::fillFontSubstitutions( PrinterInfo& /* rInfo */ ) const
{

}

void PrinterInfoManager::getSystemPrintCommands( std::list< OUString >& /* rCommands */ )
{

}

const std::list< PrinterInfoManager::SystemPrintQueue >& PrinterInfoManager::getSystemPrintQueues()
{
    return m_aSystemPrintQueues;
}

bool PrinterInfoManager::checkFeatureToken( const rtl::OUString& /* rPrinterName */, const char* /* pToken */ ) const
{
    return false;
}

FILE* PrinterInfoManager::startSpool( const OUString& /* rPrintername */, bool /* bQuickCommand */ )
{
    return NULL;
}

int PrinterInfoManager::endSpool( const OUString& /*rPrintername*/, const OUString& /*rJobTitle*/, FILE* /* pFile */, const JobData& /*rDocumentJobData*/, bool /*bBanner*/ )
{
    return true;
}

void PrinterInfoManager::setupJobContextData( JobData& /* rData */ )
{

}

void PrinterInfoManager::setDefaultPaper( PPDContext& /* rContext */ ) const
{

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
