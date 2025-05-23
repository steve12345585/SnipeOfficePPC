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

#ifndef SC_CELLSH_HXX
#define SC_CELLSH_HXX

#include <sfx2/shell.hxx>
#include "shellids.hxx"
#include <sfx2/module.hxx>
#include <svx/svdmark.hxx>
#include <unotools/caserotate.hxx>
#include <tools/link.hxx>
#include "formatsh.hxx"
#include "address.hxx"

class SvxClipboardFmtItem;
class TransferableDataHelper;
class TransferableClipboardListener;
class AbstractScLinkedAreaDlg;

struct CellShell_Impl
{
    TransferableClipboardListener*  m_pClipEvtLstnr;
    AbstractScLinkedAreaDlg*        m_pLinkedDlg;
    SfxRequest*                     m_pRequest;

    CellShell_Impl() :
        m_pClipEvtLstnr( NULL ),
        m_pLinkedDlg( NULL ),
        m_pRequest( NULL ) {}
};

class ScCellShell: public ScFormatShell
{
private:
    CellShell_Impl* pImpl;
    sal_Bool            bPastePossible;

    void        GetPossibleClipboardFormats( SvxClipboardFmtItem& rFormats );
    void        ExecuteExternalSource(
                    const String& _rFile, const String& _rFilter, const String& _rOptions,
                    const String& _rSource, sal_uLong _nRefresh, SfxRequest& _rRequest );

    void ExecuteDataPilotDialog();
    void ExecuteXMLSourceDialog();
    void ExecuteSubtotals(SfxRequest& rReq);

    DECL_LINK( ClipboardChanged, TransferableDataHelper* );
    DECL_LINK( DialogClosed, void* );

    RotateTransliteration m_aRotateCase;

public:

    TYPEINFO();
    SFX_DECL_INTERFACE(SCID_CELL_SHELL)

                ScCellShell(ScViewData* pData);
    virtual     ~ScCellShell();

    void        Execute(SfxRequest &);
    void        GetState(SfxItemSet &);

    void        ExecuteEdit( SfxRequest& rReq );
    void        ExecuteTrans( SfxRequest& rReq );
    void                ExecuteRotateTrans( SfxRequest& rReq );

    void        GetBlockState( SfxItemSet& rSet );
    void        GetCellState( SfxItemSet& rSet );

    void        ExecuteDB( SfxRequest& rReq );
    void        GetDBState( SfxItemSet& rSet );

    void        ExecImageMap(SfxRequest& rReq);     // ImageMap
    void        GetImageMapState(SfxItemSet& rSet);

    void        GetClipState( SfxItemSet& rSet );
    void        GetHLinkState( SfxItemSet& rSet );

    void        ExecuteCursor( SfxRequest& rReq );
    void        ExecuteCursorSel( SfxRequest& rReq );
    void        ExecutePage( SfxRequest& rReq );
    void        ExecutePageSel( SfxRequest& rReq );
    void        ExecuteMove( SfxRequest& rReq );
    void        GetStateCursor( SfxItemSet& rSet );
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
