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

#include <rtl/unload.h>

#include "targetdragcontext.hxx"

extern rtl_StandardModuleCount g_moduleCount;
TargetDragContext::TargetDragContext( DropTarget* p)
{
    g_moduleCount.modCnt.acquire( &g_moduleCount.modCnt );
    m_pDropTarget= p;
    p->acquire();
}

TargetDragContext::~TargetDragContext()
{
    m_pDropTarget->release();
    g_moduleCount.modCnt.release( &g_moduleCount.modCnt );
}

void SAL_CALL TargetDragContext::acceptDrag( sal_Int8 dragOperation )
    throw( RuntimeException)
{
    m_pDropTarget->_acceptDrag( dragOperation, static_cast<XDropTargetDragContext*>( this) );

}
void SAL_CALL TargetDragContext::rejectDrag( )
    throw( RuntimeException)
{
    m_pDropTarget->_rejectDrag( static_cast<XDropTargetDragContext*>( this) );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
