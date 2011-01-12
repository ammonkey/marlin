/* helper functions for GtkUIManager stuff
 * imported from nautilus
 *
 * Copyright (C) 2004 Red Hat, Inc.
 *
 * The Gnome Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Gnome Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with the Gnome Library; see the file COPYING.LIB.  If not,
 * write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 *  Authors: Alexander Larsson <alexl@redhat.com>
 */

#include <config.h>
#include "eel-ui.h"
#include "eel-debug.h"

void
eel_ui_unmerge_ui (GtkUIManager *ui_manager,
                   guint *merge_id,
                   GtkActionGroup **action_group)
{
    if (*merge_id != 0) {
        gtk_ui_manager_remove_ui (ui_manager,
                                  *merge_id);
        *merge_id = 0;
    }
    if (*action_group != NULL) {
        gtk_ui_manager_remove_action_group (ui_manager,
                                            *action_group);
        *action_group = NULL;
    }
}

/*char *
eel_ui_file (const char *partial_path)
{
    char *path;

    path = g_build_filename (DATADIR "/marlin/ui", partial_path, NULL);
    if (g_file_test (path, G_FILE_TEST_EXISTS)) {
        return path;
    }
    g_free (path);
    return NULL;
}*/

const char *
eel_ui_string_get (const char *filename)
{
    static GHashTable *ui_cache = NULL;
    char *ui;
    char *path;

    if (ui_cache == NULL) {
        ui_cache = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
        eel_debug_call_at_shutdown_with_data ((GFreeFunc)g_hash_table_destroy, ui_cache);
    }

    ui = g_hash_table_lookup (ui_cache, filename);
    if (ui == NULL) {
        //path = eel_ui_file (filename);
        //path = UI_DIR filename;
        //path = g_build_filename (DATADIR "/marlin/ui", partial_path, NULL);
        path = g_build_filename (UI_DIR, filename, NULL);
        if (path == NULL || !g_file_get_contents (path, &ui, NULL, NULL)) {
            g_warning ("Unable to load ui file %s\n", filename); 
        } 
        g_free (path);
        g_hash_table_insert (ui_cache,
                             g_strdup (filename),
                             ui);
    }

    return ui;
}
