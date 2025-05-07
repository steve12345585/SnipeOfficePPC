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

#include <rtl/unload.h>

#include "targetdropcontext.hxx"

using namespace ::com::sun::star::datatransfer::dnd;
using namespace ::cppu;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;

extern rtl_StandardModuleCount g_moduleCount;
TargetDropContext::TargetDropContext( DropTarget* p)
{
    g_moduleCount.modCnt.acquire( &g_moduleCount.modCnt );
    m_pDropTarget= p;
    p->acquire();
}

TargetDropContext::~TargetDropContext()
{
    m_pDropTarget->release();
    g_moduleCount.modCnt.release( &g_moduleCount.modCnt );
}

void SAL_CALL TargetDropContext::acceptDrop( sal_Int8 dropOperation )
        throw( RuntimeException)
{
    m_pDropTarget->_acceptDrop( dropOperation, static_cast<XDropTargetDropContext*>( this) );
}

void SAL_CALL TargetDropContext::rejectDrop( )
        throw( RuntimeException)
{
    m_pDropTarget->_rejectDrop(  static_cast<XDropTargetDropContext*>( this) );
}

void SAL_CALL TargetDropContext::dropComplete( sal_Bool success )
        throw( RuntimeException)
{
    m_pDropTarget->_dropComplete( success, static_cast<XDropTargetDropContext*>( this) );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
