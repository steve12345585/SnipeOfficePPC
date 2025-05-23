/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 * This file is part of the SnipeOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef GLOACTIONGROUP_H
#define GLOACTIONGROUP_H

#include <gio/gio.h>

G_BEGIN_DECLS

#define G_TYPE_LO_ACTION_GROUP                          (g_lo_action_group_get_type ())
#define G_LO_ACTION_GROUP(inst)                         (G_TYPE_CHECK_INSTANCE_CAST ((inst),                     \
                                                         G_TYPE_LO_ACTION_GROUP, GLOActionGroup))
#define G_LO_ACTION_GROUP_CLASS(klass)                  (G_TYPE_CHECK_CLASS_CAST ((klass),                       \
                                                         G_TYPE_LO_ACTION_GROUP, GLOActionGroupClass))
#define G_IS_LO_ACTION_GROUP(inst)                      (G_TYPE_CHECK_INSTANCE_TYPE ((inst),                     \
                                                         G_TYPE_LO_ACTION_GROUP))
#define G_IS_LO_ACTION_GROUP_CLASS(klass)               (G_TYPE_CHECK_CLASS_TYPE ((klass),                       \
                                                         G_TYPE_LO_ACTION_GROUP))
#define G_LO_ACTION_GROUP_GET_CLASS(inst)               (G_TYPE_INSTANCE_GET_CLASS ((inst),                      \
                                                         G_TYPE_LO_ACTION_GROUP, GLOActionGroupClass))

typedef struct _GLOActionGroupPrivate                   GLOActionGroupPrivate;
typedef struct _GLOActionGroupClass                     GLOActionGroupClass;
typedef struct _GLOActionGroup                          GLOActionGroup;

struct _GLOActionGroup
{
    /*< private >*/
    GObject parent_instance;

    GLOActionGroupPrivate *priv;
};

struct _GLOActionGroupClass
{
    /*< private >*/
    GObjectClass parent_class;

    /*< private >*/
    gpointer padding[12];
};

GType               g_lo_action_group_get_type              (void) G_GNUC_CONST;

GLOActionGroup *    g_lo_action_group_new                   (gpointer           frame);

void                g_lo_action_group_insert                (GLOActionGroup     *group,
                                                             const gchar        *action_name,
                                                             gint                item_id,
                                                             gboolean            submenu);

void                g_lo_action_group_insert_stateful       (GLOActionGroup     *group,
                                                             const gchar        *action_name,
                                                             gint                item_id,
                                                             gboolean            submenu,
                                                             const GVariantType *parameter_type,
                                                             const GVariantType *state_type,
                                                             GVariant           *state_hint,
                                                             GVariant           *state);

void                g_lo_action_group_set_action_enabled    (GLOActionGroup     *group,
                                                             const gchar        *action_name,
                                                             gboolean            enabled);

void                g_lo_action_group_remove                (GLOActionGroup     *group,
                                                             const gchar        *action_name);

void                g_lo_action_group_clear                 (GLOActionGroup     *group);

G_END_DECLS

#endif // GLOACTIONGROUP_H

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
