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
#ifndef DBAUI_QUERYTABCONNUNDOACTION_HXX
#define DBAUI_QUERYTABCONNUNDOACTION_HXX

#include "QueryDesignUndoAction.hxx"

namespace dbaui
{
    class OQueryTableConnection;
    class OQueryTableView;
    class OQueryTabConnUndoAction : public OQueryDesignUndoAction
    {
    protected:
        OQueryTableConnection*  m_pConnection;
        sal_Bool                    m_bOwnerOfConn;
            // bin ich alleiniger Eigentuemer der Connection ? (aendert sich mit jedem Redo oder Undo)

    public:
        OQueryTabConnUndoAction(OQueryTableView* pOwner, sal_uInt16 nCommentID);
        virtual ~OQueryTabConnUndoAction();

        virtual void Undo() = 0;
        virtual void Redo() = 0;

        void SetConnection(OQueryTableConnection* pConn) { m_pConnection = pConn; }
            // anschliessend bitte SetOwnership
        void SetOwnership(sal_Bool bTakeIt) { m_bOwnerOfConn = bTakeIt; }
    };
}
#endif // DBAUI_QUERYTABCONNUNDOACTION_HXX


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
