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


#include "atkwrapper.hxx"

#include <com/sun/star/accessibility/XAccessibleSelection.hpp>

#include <stdio.h>

using namespace ::com::sun::star;

static accessibility::XAccessibleSelection*
    getSelection( AtkSelection *pSelection ) throw (uno::RuntimeException)
{
    AtkObjectWrapper *pWrap = ATK_OBJECT_WRAPPER( pSelection );
    if( pWrap )
    {
        if( !pWrap->mpSelection && pWrap->mpContext )
        {
            uno::Any any = pWrap->mpContext->queryInterface( accessibility::XAccessibleSelection::static_type(NULL) );
            pWrap->mpSelection = reinterpret_cast< accessibility::XAccessibleSelection * > (any.pReserved);
            pWrap->mpSelection->acquire();
        }

        return pWrap->mpSelection;
    }

    return NULL;
}

extern "C" {

static gboolean
selection_add_selection( AtkSelection *selection,
                         gint          i )
{
    try {
        accessibility::XAccessibleSelection* pSelection = getSelection( selection );
        if( pSelection )
        {
            pSelection->selectAccessibleChild( i );
            return TRUE;
        }
    }
    catch(const uno::Exception& e) {
        g_warning( "Exception in selectAccessibleChild()" );
    }

    return FALSE;
}

static gboolean
selection_clear_selection( AtkSelection *selection )
{
    try {
        accessibility::XAccessibleSelection* pSelection = getSelection( selection );
        if( pSelection )
        {
            pSelection->clearAccessibleSelection();
            return TRUE;
        }
    }
    catch(const uno::Exception& e) {
        g_warning( "Exception in selectAccessibleChild()" );
    }

    return FALSE;
}

static AtkObject*
selection_ref_selection( AtkSelection *selection,
                         gint          i )
{
    try {
        accessibility::XAccessibleSelection* pSelection = getSelection( selection );
        if( pSelection )
            return atk_object_wrapper_ref( pSelection->getSelectedAccessibleChild( i ) );
    }
    catch(const uno::Exception& e) {
        g_warning( "Exception in getSelectedAccessibleChild()" );
    }

    return NULL;
}

static gint
selection_get_selection_count( AtkSelection   *selection)
{
    try {
        accessibility::XAccessibleSelection* pSelection = getSelection( selection );
        if( pSelection )
            return pSelection->getSelectedAccessibleChildCount();
    }
    catch(const uno::Exception& e) {
        g_warning( "Exception in getSelectedAccessibleChildCount()" );
    }

    return -1;
}

static gboolean
selection_is_child_selected( AtkSelection   *selection,
                              gint           i)
{
    try {
        accessibility::XAccessibleSelection* pSelection = getSelection( selection );
        if( pSelection )
            return pSelection->isAccessibleChildSelected( i );
    }
    catch(const uno::Exception& e) {
        g_warning( "Exception in getSelectedAccessibleChildCount()" );
    }

    return FALSE;
}

static gboolean
selection_remove_selection( AtkSelection *selection,
                            gint           i )
{
    try {
        accessibility::XAccessibleSelection* pSelection = getSelection( selection );
        if( pSelection )
        {
            pSelection->deselectAccessibleChild( i );
            return TRUE;
        }
    }
    catch(const uno::Exception& e) {
        g_warning( "Exception in getSelectedAccessibleChildCount()" );
    }

    return FALSE;
}

static gboolean
selection_select_all_selection( AtkSelection   *selection)
{
    try {
        accessibility::XAccessibleSelection* pSelection = getSelection( selection );
        if( pSelection )
        {
            pSelection->selectAllAccessibleChildren();
            return TRUE;
        }
    }
    catch(const uno::Exception& e) {
        g_warning( "Exception in getSelectedAccessibleChildCount()" );
    }

    return FALSE;
}

} // extern "C"

void
selectionIfaceInit( AtkSelectionIface *iface)
{
  g_return_if_fail (iface != NULL);

  iface->add_selection = selection_add_selection;
  iface->clear_selection = selection_clear_selection;
  iface->ref_selection = selection_ref_selection;
  iface->get_selection_count = selection_get_selection_count;
  iface->is_child_selected = selection_is_child_selected;
  iface->remove_selection = selection_remove_selection;
  iface->select_all_selection = selection_select_all_selection;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
