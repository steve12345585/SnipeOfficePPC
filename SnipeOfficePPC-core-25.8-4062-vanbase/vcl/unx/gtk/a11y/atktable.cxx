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

#include <com/sun/star/accessibility/XAccessibleTable.hpp>

#ifdef ENABLE_TRACING
#include <stdio.h>
#endif

using namespace ::com::sun::star;

static inline AtkObject *
atk_object_wrapper_conditional_ref( const uno::Reference< accessibility::XAccessible >& rxAccessible )
{
#ifdef ENABLE_TRACING
    fprintf( stderr, ": %p\n", rxAccessible.get() );
#endif

    if( rxAccessible.is() )
        return atk_object_wrapper_ref( rxAccessible );

    return NULL;
}

/*****************************************************************************/

// FIXME
static G_CONST_RETURN gchar *
getAsConst( rtl::OUString rString )
{
    static const int nMax = 10;
    static rtl::OString aUgly[nMax];
    static int nIdx = 0;
    nIdx = (nIdx + 1) % nMax;
    aUgly[nIdx] = rtl::OUStringToOString( rString, RTL_TEXTENCODING_UTF8 );
    return aUgly[ nIdx ].getStr();
}

/*****************************************************************************/

static accessibility::XAccessibleTable*
    getTable( AtkTable *pTable ) throw (uno::RuntimeException)
{
    AtkObjectWrapper *pWrap = ATK_OBJECT_WRAPPER( pTable );
    if( pWrap )
    {
        if( !pWrap->mpTable && pWrap->mpContext )
        {
            uno::Any any = pWrap->mpContext->queryInterface( accessibility::XAccessibleTable::static_type(NULL) );
            pWrap->mpTable = reinterpret_cast< accessibility::XAccessibleTable * > (any.pReserved);
            pWrap->mpTable->acquire();
        }

        return pWrap->mpTable;
    }

    return NULL;
}

/*****************************************************************************/

extern "C" {

static AtkObject*
table_wrapper_ref_at (AtkTable *table,
                      gint      row,
                      gint      column)
{
    try {
        accessibility::XAccessibleTable* pTable = getTable( table );

#ifdef ENABLE_TRACING
        if( pTable )
            fprintf(stderr, "getAccessibleCellAt( %u, %u ) returns", row, column );

        if( column >= 255 )
            fprintf(stderr, "getAccessibleCellAt( %u, %u ) returns", row, column );

#endif

        if( pTable )
            return atk_object_wrapper_conditional_ref( pTable->getAccessibleCellAt( row, column ) );
    }

    catch(const uno::Exception& e) {
        g_warning( "Exception in getAccessibleCellAt()" );
    }

    return NULL;
}

/*****************************************************************************/

static gint
table_wrapper_get_index_at (AtkTable      *table,
                            gint          row,
                            gint          column)
{
    try {
        accessibility::XAccessibleTable* pTable = getTable( table );

#ifdef ENABLE_TRACING
        if( pTable )
            fprintf(stderr, "getAccessibleIndex( %u, %u ) returns %u\n",
                row, column, pTable->getAccessibleIndex( row, column ) );
#endif

        if( pTable )
            return pTable->getAccessibleIndex( row, column );
    }
    catch(const uno::Exception& e) {
        g_warning( "Exception in getAccessibleIndex()" );
    }

    return -1;
}

/*****************************************************************************/

static gint
table_wrapper_get_column_at_index (AtkTable      *table,
                                   gint          nIndex)
{
    try {
        accessibility::XAccessibleTable* pTable = getTable( table );

#ifdef ENABLE_TRACING
        if( pTable )
            fprintf(stderr, "getAccessibleColumn( %u ) returns %u\n",
                nIndex, pTable->getAccessibleColumn( nIndex ) );
#endif

        if( pTable )
            return pTable->getAccessibleColumn( nIndex );
    }
    catch(const uno::Exception& e) {
        g_warning( "Exception in getAccessibleColumn()" );
    }

    return -1;
}

/*****************************************************************************/

static gint
table_wrapper_get_row_at_index( AtkTable *table,
                                gint      nIndex )
{
    try {
        accessibility::XAccessibleTable* pTable = getTable( table );

#ifdef ENABLE_TRACING
        if( pTable )
            fprintf(stderr, "getAccessibleRow( %u ) returns %u\n",
                nIndex, pTable->getAccessibleRow( nIndex ) );
#endif

        if( pTable )
            return pTable->getAccessibleRow( nIndex );
    }
    catch(const uno::Exception& e) {
        g_warning( "Exception in getAccessibleRow()" );
    }

    return -1;
}

/*****************************************************************************/

static gint
table_wrapper_get_n_columns( AtkTable *table )
{
    try {
        accessibility::XAccessibleTable* pTable = getTable( table );

#ifdef ENABLE_TRACING
        if( pTable )
            fprintf(stderr, "XAccessibleTable::getAccessibleColumnCount returns %u\n",
                pTable->getAccessibleColumnCount() );
#endif

        if( pTable )
            return pTable->getAccessibleColumnCount();
    }
    catch(const uno::Exception& e) {
        g_warning( "Exception in getAccessibleColumnCount()" );
    }

    return -1;
}

/*****************************************************************************/

static gint
table_wrapper_get_n_rows( AtkTable *table )
{
    try {
        accessibility::XAccessibleTable* pTable = getTable( table );

#ifdef ENABLE_TRACING
        if( pTable )
            fprintf(stderr, "getAccessibleRowCount() returns %u\n",
                pTable->getAccessibleRowCount() );
#endif

        if( pTable )
            return pTable->getAccessibleRowCount();
    }
    catch(const uno::Exception& e) {
        g_warning( "Exception in getAccessibleRowCount()" );
    }

    return -1;
}

/*****************************************************************************/

static gint
table_wrapper_get_column_extent_at( AtkTable *table,
                                    gint      row,
                                    gint      column )
{
    try {
        accessibility::XAccessibleTable* pTable = getTable( table );

#ifdef ENABLE_TRACING
        if( pTable )
            fprintf(stderr, "getAccessibleColumnExtentAt( %u, %u ) returns %u\n",
                row, column, pTable->getAccessibleColumnExtentAt( row, column ) );
#endif

        if( pTable )
            return pTable->getAccessibleColumnExtentAt( row, column );
    }
    catch(const uno::Exception& e) {
        g_warning( "Exception in getAccessibleColumnExtentAt()" );
    }

    return -1;
}

/*****************************************************************************/

static gint
table_wrapper_get_row_extent_at( AtkTable *table,
                                 gint      row,
                                 gint      column )
{
    try {
        accessibility::XAccessibleTable* pTable = getTable( table );

#ifdef ENABLE_TRACING
        if( pTable )
            fprintf(stderr, "getAccessibleRowExtentAt( %u, %u ) returns %u\n",
                row, column, pTable->getAccessibleRowExtentAt( row, column ) );
#endif

        if( pTable )
            return pTable->getAccessibleRowExtentAt( row, column );
    }
    catch(const uno::Exception& e) {
        g_warning( "Exception in getAccessibleRowExtentAt()" );
    }

    return -1;
}

/*****************************************************************************/

static AtkObject *
table_wrapper_get_caption( AtkTable *table )
{
    try {
        accessibility::XAccessibleTable* pTable = getTable( table );

#ifdef ENABLE_TRACING
        if( pTable )
            fprintf(stderr, "getAccessibleCaption() returns" );
#endif

        if( pTable )
            return atk_object_wrapper_conditional_ref( pTable->getAccessibleCaption() );
    }

    catch(const uno::Exception& e) {
        g_warning( "Exception in getAccessibleCaption()" );
    }

    return NULL;
}

/*****************************************************************************/

static G_CONST_RETURN gchar *
table_wrapper_get_row_description( AtkTable *table,
                                   gint      row )
{
    try {
        accessibility::XAccessibleTable* pTable = getTable( table );

#ifdef ENABLE_TRACING
        if( pTable )
            fprintf(stderr, "getAccessibleRowDescription( %u ) returns %s\n",
                row, getAsConst( pTable->getAccessibleRowDescription( row ) ) );
#endif

        if( pTable )
            return getAsConst( pTable->getAccessibleRowDescription( row ) );
    }
    catch(const uno::Exception& e) {
        g_warning( "Exception in getAccessibleRowDescription()" );
    }

    return NULL;
}

/*****************************************************************************/

static G_CONST_RETURN gchar *
table_wrapper_get_column_description( AtkTable *table,
                                      gint      column )
{
    try {
        accessibility::XAccessibleTable* pTable = getTable( table );

#ifdef ENABLE_TRACING
        if( pTable )
            fprintf(stderr, "getAccessibleColumnDescription( %u ) returns %s\n",
                column, getAsConst( pTable->getAccessibleColumnDescription( column ) ) );
#endif

        if( pTable )
            return getAsConst( pTable->getAccessibleColumnDescription( column ) );
    }
    catch(const uno::Exception& e) {
        g_warning( "Exception in getAccessibleColumnDescription()" );
    }

    return NULL;
}

/*****************************************************************************/

static AtkObject *
table_wrapper_get_row_header( AtkTable *table,
                              gint      row )
{
    try {
        accessibility::XAccessibleTable* pTable = getTable( table );
        if( pTable )
        {
            uno::Reference< accessibility::XAccessibleTable > xRowHeaders( pTable->getAccessibleRowHeaders() );

#ifdef ENABLE_TRACING
            if( xRowHeaders.is() )
                fprintf(stderr, "getAccessibleRowHeader( %u )->getAccessibleCellAt( 0, %u ) returns",
                    row, row );
            else
                fprintf(stderr, "getAccessibleRowHeader( %u ) returns %p\n", row, xRowHeaders.get() );
#endif

            if( xRowHeaders.is() )
                return atk_object_wrapper_conditional_ref( xRowHeaders->getAccessibleCellAt( row, 0 ) );
        }
    }
    catch(const uno::Exception& e) {
        g_warning( "Exception in getAccessibleRowHeaders()" );
    }

    return NULL;
}

/*****************************************************************************/

static AtkObject *
table_wrapper_get_column_header( AtkTable *table,
                                 gint      column )
{
    try {
        accessibility::XAccessibleTable* pTable = getTable( table );

        if( pTable )
        {
            uno::Reference< accessibility::XAccessibleTable > xColumnHeaders( pTable->getAccessibleColumnHeaders() );

#ifdef ENABLE_TRACING
            if( xColumnHeaders.is() )
                fprintf(stderr, "getAccessibleColumnHeader( %u )->getAccessibleCellAt( 0, %u ) returns",
                    column, column );
            else
                fprintf(stderr, "getAccessibleColumnHeader( %u ) returns %p\n", column, xColumnHeaders.get() );
#endif

            if( xColumnHeaders.is() )
                return atk_object_wrapper_conditional_ref( xColumnHeaders->getAccessibleCellAt( 0, column ) );
        }
    }
    catch(const uno::Exception& e) {
        g_warning( "Exception in getAccessibleColumnHeaders()" );
    }

    return NULL;
}

/*****************************************************************************/

static AtkObject *
table_wrapper_get_summary( AtkTable *table )
{
    try {
        accessibility::XAccessibleTable* pTable = getTable( table );

#ifdef ENABLE_TRACING
        if( pTable )
            fprintf(stderr, "getAccessibleSummary() returns" );
#endif

        if( pTable )
        {
            return atk_object_wrapper_conditional_ref( pTable->getAccessibleSummary() );
        }
    }
    catch(const uno::Exception& e) {
        g_warning( "Exception in getAccessibleSummary()" );
    }

    return NULL;
}

/*****************************************************************************/

static gint
convertToGIntArray( const uno::Sequence< ::sal_Int32 >& aSequence, gint **pSelected )
{
    if( aSequence.getLength() )
    {
        *pSelected = g_new( gint, aSequence.getLength() );

        for( sal_Int32 i = 0; i < aSequence.getLength(); i++ )
            (*pSelected) [i] = aSequence[i];
    }

    return aSequence.getLength();
}

/*****************************************************************************/

static gint
table_wrapper_get_selected_columns( AtkTable      *table,
                                    gint          **pSelected )
{
    *pSelected = NULL;
    try {
        accessibility::XAccessibleTable* pTable = getTable( table );

#ifdef ENABLE_TRACING
        if( pTable )
            fprintf(stderr, "getSelectedAccessibleColumns() \n" );
#endif

        if( pTable )
            return convertToGIntArray( pTable->getSelectedAccessibleColumns(), pSelected );
    }
    catch(const uno::Exception& e) {
        g_warning( "Exception in getSelectedAccessibleColumns()" );
    }

    return 0;
}

/*****************************************************************************/

static gint
table_wrapper_get_selected_rows( AtkTable      *table,
                                 gint          **pSelected )
{
    *pSelected = NULL;
    try {
        accessibility::XAccessibleTable* pTable = getTable( table );

#ifdef ENABLE_TRACING
        if( pTable )
            fprintf(stderr, "getSelectedAccessibleRows() \n" );
#endif

        if( pTable )
            return convertToGIntArray( pTable->getSelectedAccessibleRows(), pSelected );
    }
    catch(const uno::Exception& e) {
        g_warning( "Exception in getSelectedAccessibleRows()" );
    }

    return 0;
}

/*****************************************************************************/

static gboolean
table_wrapper_is_column_selected( AtkTable      *table,
                                  gint          column )
{
    try {
        accessibility::XAccessibleTable* pTable = getTable( table );

#ifdef ENABLE_TRACING
        if( pTable )
            fprintf(stderr, "isAccessibleColumnSelected( %u ) returns %s\n",
                column, pTable->isAccessibleColumnSelected( column ) ? "true" : "false" );
#endif

        if( pTable )
            return pTable->isAccessibleColumnSelected( column );
    }
    catch(const uno::Exception& e) {
        g_warning( "Exception in isAccessibleColumnSelected()" );
    }

    return 0;
}

/*****************************************************************************/

static gboolean
table_wrapper_is_row_selected( AtkTable      *table,
                               gint          row )
{
    try {
        accessibility::XAccessibleTable* pTable = getTable( table );

#ifdef ENABLE_TRACING
        if( pTable )
            fprintf(stderr, "isAccessibleRowSelected( %u ) returns %s\n",
                row, pTable->isAccessibleRowSelected( row ) ? "true" : "false" );
#endif

        if( pTable )
            return pTable->isAccessibleRowSelected( row );
    }
    catch(const uno::Exception& e) {
        g_warning( "Exception in isAccessibleRowSelected()" );
    }

    return FALSE;
}

/*****************************************************************************/

static gboolean
table_wrapper_is_selected( AtkTable      *table,
                           gint          row,
                           gint          column )
{
    try {
        accessibility::XAccessibleTable* pTable = getTable( table );

#ifdef ENABLE_TRACING
        if( pTable )
            fprintf(stderr, "isAccessibleSelected( %u, %u ) returns %s\n",
                row, column, pTable->isAccessibleSelected( row , column ) ? "true" : "false" );
#endif

        if( pTable )
            return pTable->isAccessibleSelected( row, column );
    }
    catch(const uno::Exception& e) {
        g_warning( "Exception in isAccessibleSelected()" );
    }

    return FALSE;
}

/*****************************************************************************/

static gboolean
table_wrapper_add_row_selection( AtkTable *, gint )
{
    g_warning( "FIXME: no simple analogue for add_row_selection" );
    return 0;
}

/*****************************************************************************/

static gboolean
table_wrapper_remove_row_selection( AtkTable *, gint )
{
    g_warning( "FIXME: no simple analogue for remove_row_selection" );
    return 0;
}

/*****************************************************************************/

static gboolean
table_wrapper_add_column_selection( AtkTable *, gint )
{
    g_warning( "FIXME: no simple analogue for add_column_selection" );
    return 0;
}

/*****************************************************************************/

static gboolean
table_wrapper_remove_column_selection( AtkTable *, gint )
{
    g_warning( "FIXME: no simple analogue for remove_column_selection" );
    return 0;
}

/*****************************************************************************/

static void
table_wrapper_set_caption( AtkTable *, AtkObject * )
{ // meaningless helper
}

/*****************************************************************************/

static void
table_wrapper_set_column_description( AtkTable *, gint, const gchar * )
{ // meaningless helper
}


/*****************************************************************************/

static void
table_wrapper_set_column_header( AtkTable *, gint, AtkObject * )
{ // meaningless helper
}


/*****************************************************************************/

static void
table_wrapper_set_row_description( AtkTable *, gint, const gchar * )
{ // meaningless helper
}

/*****************************************************************************/

static void
table_wrapper_set_row_header( AtkTable *, gint, AtkObject * )
{ // meaningless helper
}

/*****************************************************************************/

static void
table_wrapper_set_summary( AtkTable *, AtkObject * )
{ // meaningless helper
}

/*****************************************************************************/

} // extern "C"

void
tableIfaceInit (AtkTableIface *iface)
{
  g_return_if_fail (iface != NULL);

  iface->ref_at = table_wrapper_ref_at;
  iface->get_n_rows = table_wrapper_get_n_rows;
  iface->get_n_columns = table_wrapper_get_n_columns;
  iface->get_index_at = table_wrapper_get_index_at;
  iface->get_column_at_index = table_wrapper_get_column_at_index;
  iface->get_row_at_index = table_wrapper_get_row_at_index;
  iface->is_row_selected = table_wrapper_is_row_selected;
  iface->is_selected = table_wrapper_is_selected;
  iface->get_selected_rows = table_wrapper_get_selected_rows;
  iface->add_row_selection = table_wrapper_add_row_selection;
  iface->remove_row_selection = table_wrapper_remove_row_selection;
  iface->add_column_selection = table_wrapper_add_column_selection;
  iface->remove_column_selection = table_wrapper_remove_column_selection;
  iface->get_selected_columns = table_wrapper_get_selected_columns;
  iface->is_column_selected = table_wrapper_is_column_selected;
  iface->get_column_extent_at = table_wrapper_get_column_extent_at;
  iface->get_row_extent_at = table_wrapper_get_row_extent_at;
  iface->get_row_header = table_wrapper_get_row_header;
  iface->set_row_header = table_wrapper_set_row_header;
  iface->get_column_header = table_wrapper_get_column_header;
  iface->set_column_header = table_wrapper_set_column_header;
  iface->get_caption = table_wrapper_get_caption;
  iface->set_caption = table_wrapper_set_caption;
  iface->get_summary = table_wrapper_get_summary;
  iface->set_summary = table_wrapper_set_summary;
  iface->get_row_description = table_wrapper_get_row_description;
  iface->set_row_description = table_wrapper_set_row_description;
  iface->get_column_description = table_wrapper_get_column_description;
  iface->set_column_description = table_wrapper_set_column_description;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
