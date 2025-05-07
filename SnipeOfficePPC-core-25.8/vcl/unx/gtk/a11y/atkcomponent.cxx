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


#include "atkwrapper.hxx"

#include <com/sun/star/accessibility/XAccessibleComponent.hpp>

#ifdef ENABLE_TRACING
#include <stdio.h>
#endif

using namespace ::com::sun::star;

static accessibility::XAccessibleComponent*
    getComponent( AtkComponent *pComponent ) throw (uno::RuntimeException)
{
    AtkObjectWrapper *pWrap = ATK_OBJECT_WRAPPER( pComponent );
    if( pWrap )
    {
        if( !pWrap->mpComponent && pWrap->mpContext )
        {
            uno::Any any = pWrap->mpContext->queryInterface( accessibility::XAccessibleComponent::static_type(NULL) );
            pWrap->mpComponent = reinterpret_cast< accessibility::XAccessibleComponent * > (any.pReserved);
            pWrap->mpComponent->acquire();
        }

        return pWrap->mpComponent;
    }

    return NULL;
}

/*****************************************************************************/

static awt::Point
translatePoint( accessibility::XAccessibleComponent *pComponent,
                gint x, gint y, AtkCoordType t)
{
    awt::Point aOrigin( 0, 0 );
    if( t == ATK_XY_SCREEN )
        aOrigin = pComponent->getLocationOnScreen();

#ifdef ENABLE_TRACING
    fprintf(stderr, "coordinates ( %u, %u ) translated to: ( %u, %u )\n",
        x, y, x - aOrigin.X, y - aOrigin.Y);
#endif

    return awt::Point( x - aOrigin.X, y - aOrigin.Y );
}

/*****************************************************************************/

extern "C" {

static gboolean
component_wrapper_grab_focus (AtkComponent *component)
{
    try
    {
        accessibility::XAccessibleComponent* pComponent = getComponent( component );
        if( pComponent )
        {
            pComponent->grabFocus();
            return TRUE;
        }
    }
    catch( const uno::Exception &e )
    {
        g_warning( "Exception in grabFocus()" );
    }

    return FALSE;
}

/*****************************************************************************/

static gboolean
component_wrapper_contains (AtkComponent *component,
                            gint          x,
                            gint          y,
                            AtkCoordType  coord_type)
{
    try
    {
        accessibility::XAccessibleComponent* pComponent = getComponent( component );
        if( pComponent )
            return pComponent->containsPoint( translatePoint( pComponent, x, y, coord_type ) );
    }
    catch( const uno::Exception &e )
    {
        g_warning( "Exception in containsPoint()" );
    }

    return FALSE;
}

/*****************************************************************************/

static AtkObject *
component_wrapper_ref_accessible_at_point (AtkComponent *component,
                                           gint          x,
                                           gint          y,
                                           AtkCoordType  coord_type)
{
    try
    {
        accessibility::XAccessibleComponent* pComponent = getComponent( component );

        if( pComponent )
        {
            uno::Reference< accessibility::XAccessible > xAccessible;
            xAccessible = pComponent->getAccessibleAtPoint(
                translatePoint( pComponent, x, y, coord_type ) );

#ifdef ENABLE_TRACING
            fprintf(stderr, "getAccessibleAtPoint( %u, %u ) returned %p\n",
              x, y, xAccessible.get());

            uno::Reference< accessibility::XAccessibleComponent > xComponent(
                xAccessible->getAccessibleContext(), uno::UNO_QUERY );

            if( xComponent.is() )
            {
                awt::Rectangle rect = xComponent->getBounds();
                fprintf(stderr, "%p->getBounds() returned: ( %u, %u, %u, %u )\n",
                    xAccessible.get(), rect.X, rect.Y, rect.Width, rect.Height );
            }
#endif

            return atk_object_wrapper_ref( xAccessible );
        }
    }
    catch( const uno::Exception &e )
    {
        g_warning( "Exception in getAccessibleAtPoint()" );
    }

    return NULL;
}

/*****************************************************************************/

static void
component_wrapper_get_position (AtkComponent   *component,
                                gint           *x,
                                gint           *y,
                                AtkCoordType   coord_type)
{
    try
    {
        accessibility::XAccessibleComponent* pComponent = getComponent( component );
        if( pComponent )
        {
            awt::Point aPos;

            if( coord_type == ATK_XY_SCREEN )
                aPos = pComponent->getLocationOnScreen();
            else
                aPos = pComponent->getLocation();

            *x = aPos.X;
            *y = aPos.Y;

#ifdef ENABLE_TRACING
            fprintf(stderr, "getLocation[OnScreen]() returned: ( %u, %u )\n", *x, *y );
#endif
        }
    }
    catch( const uno::Exception &e )
    {
        g_warning( "Exception in getLocation[OnScreen]()" );
    }
}

/*****************************************************************************/

static void
component_wrapper_get_size (AtkComponent   *component,
                            gint           *width,
                            gint           *height)
{
    try
    {
        accessibility::XAccessibleComponent* pComponent = getComponent( component );
        if( pComponent )
        {
            awt::Size aSize = pComponent->getSize();
            *width = aSize.Width;
            *height = aSize.Height;

#ifdef ENABLE_TRACING
            fprintf(stderr, "getSize() returned: ( %u, %u )\n", *width, *height );
#endif
        }
    }
    catch( const uno::Exception &e )
    {
        g_warning( "Exception in getSize()" );
    }
}

/*****************************************************************************/

static void
component_wrapper_get_extents (AtkComponent *component,
                               gint         *x,
                               gint         *y,
                               gint         *width,
                               gint         *height,
                               AtkCoordType  coord_type)
{
    component_wrapper_get_position( component, x, y, coord_type );
    component_wrapper_get_size( component, width, height );
}

/*****************************************************************************/

static gboolean
component_wrapper_set_extents (AtkComponent *, gint, gint, gint, gint, AtkCoordType)
{
    g_warning( "AtkComponent::set_extents unimplementable" );
    return FALSE;
}

/*****************************************************************************/

static gboolean
component_wrapper_set_position (AtkComponent *, gint, gint, AtkCoordType)
{
    g_warning( "AtkComponent::set_position unimplementable" );
    return FALSE;
}

/*****************************************************************************/

static gboolean
component_wrapper_set_size (AtkComponent *, gint, gint)
{
    g_warning( "AtkComponent::set_size unimplementable" );
    return FALSE;
}

/*****************************************************************************/

static AtkLayer
component_wrapper_get_layer (AtkComponent   *component)
{
    AtkRole role = atk_object_get_role( ATK_OBJECT( component ) );
    AtkLayer layer = ATK_LAYER_WIDGET;

    switch (role)
    {
        case ATK_ROLE_POPUP_MENU:
        case ATK_ROLE_MENU_ITEM:
        case ATK_ROLE_CHECK_MENU_ITEM:
        case ATK_ROLE_SEPARATOR:
        case ATK_ROLE_LIST_ITEM:
            layer = ATK_LAYER_POPUP;
            break;
        case ATK_ROLE_MENU:
            {
                AtkObject * parent = atk_object_get_parent( ATK_OBJECT( component ) );
                if( atk_object_get_role( parent ) != ATK_ROLE_MENU_BAR )
                    layer = ATK_LAYER_POPUP;
            }
            break;

        case ATK_ROLE_LIST:
            {
                AtkObject * parent = atk_object_get_parent( ATK_OBJECT( component ) );
                if( atk_object_get_role( parent ) == ATK_ROLE_COMBO_BOX )
                    layer = ATK_LAYER_POPUP;
            }
            break;

        default:
            ;
    }

    return layer;
}

/*****************************************************************************/

static gint
component_wrapper_get_mdi_zorder (AtkComponent   *)
{
    // only needed for ATK_LAYER_MDI (not used) or ATK_LAYER_WINDOW (inherited from GAIL)
    return G_MININT;
}

/*****************************************************************************/

// This code is mostly stolen from libgail ..

static guint
component_wrapper_add_focus_handler (AtkComponent    *component,
                                     AtkFocusHandler  handler)
{
    GSignalMatchType match_type;
    gulong ret;
    guint signal_id;

    match_type = (GSignalMatchType) (G_SIGNAL_MATCH_ID | G_SIGNAL_MATCH_FUNC);
    signal_id = g_signal_lookup( "focus-event", ATK_TYPE_OBJECT );

    ret = g_signal_handler_find( component, match_type, signal_id, 0, NULL,
                                 (gpointer) &handler, NULL);
    if (!ret)
    {
        return g_signal_connect_closure_by_id (component,
                                               signal_id, 0,
                                               g_cclosure_new (
                                               G_CALLBACK (handler), NULL,
                                               (GClosureNotify) NULL),
                                               FALSE);
    }
    else
    {
        return 0;
    }
}

/*****************************************************************************/

static void
component_wrapper_remove_focus_handler (AtkComponent  *component,
                                        guint         handler_id)
{
    g_signal_handler_disconnect (component, handler_id);
}

/*****************************************************************************/

} // extern "C"

void
componentIfaceInit (AtkComponentIface *iface)
{
  g_return_if_fail (iface != NULL);

  iface->add_focus_handler = component_wrapper_add_focus_handler;
  iface->contains = component_wrapper_contains;
  iface->get_extents = component_wrapper_get_extents;
  iface->get_layer = component_wrapper_get_layer;
  iface->get_mdi_zorder = component_wrapper_get_mdi_zorder;
  iface->get_position = component_wrapper_get_position;
  iface->get_size = component_wrapper_get_size;
  iface->grab_focus = component_wrapper_grab_focus;
  iface->ref_accessible_at_point = component_wrapper_ref_accessible_at_point;
  iface->remove_focus_handler = component_wrapper_remove_focus_handler;
  iface->set_extents = component_wrapper_set_extents;
  iface->set_position = component_wrapper_set_position;
  iface->set_size = component_wrapper_set_size;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
