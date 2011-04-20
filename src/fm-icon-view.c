/*
 * Copyright (C) 2010 ammonkey
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * version 3.0 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3.0 for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Author: ammonkey <am.monkeyd@gmail.com>
 */

#include "fm-icon-view.h"
#include "fm-list-model.h"
#include "fm-directory-view.h"
#include "marlin-global-preferences.h"
#include "eel-i18n.h"
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>

//#include "gof-directory-async.h"
#include "nautilus-cell-renderer-text-ellipsized.h"
#include "eel-glib-extensions.h"
#include "eel-gtk-extensions.h"
#include "marlin-tags.h"
#include "marlin-enum-types.h"

struct FMIconViewDetails {
    GList       *selection;
    GtkTreePath *new_selection_path;   /* Path of the new selection after removing a file */

    GtkCellEditable     *editable_widget;
    GtkTreeViewColumn   *file_name_column;
    GtkCellRendererText *file_name_cell;
    char                *original_name;

    GOFFile     *renaming_file;
    gboolean    rename_done;
};

/* Wait for the rename to end when activating a file being renamed */
#define WAIT_FOR_RENAME_ON_ACTIVATE 200

//static gchar *col_title[4] = { _("Filename"), _("Size"), _("Type"), _("Modified") };

G_DEFINE_TYPE (FMIconView, fm_icon_view, FM_TYPE_DIRECTORY_VIEW);

#define parent_class fm_icon_view_parent_class

/*struct SelectionForeachData {
  GList *list;
  GtkTreeSelection *selection;
  };*/

/* Property identifiers */
enum
{
    PROP_0,
    PROP_TEXT_BESIDE_ICONS,
};



static void
fm_icon_view_set_property (GObject      *object,
                           guint         prop_id,
                           const GValue *value,
                           GParamSpec   *pspec);

/* Declaration Prototypes */
static GList    *fm_icon_view_get_selection (FMDirectoryView *view);
static GList    *get_selection (FMIconView *view);
//static void     fm_icon_view_clear (FMIconView *view);
static void     fm_icon_view_zoom_level_changed (FMDirectoryView *view);

/*static void
  show_selected_files (GOFFile *file)
  {
  log_printf (LOG_LEVEL_UNDEFINED, "selected: %s\n", file->name);
  }*/

static void
fm_icon_view_selection_changed (GtkTreeSelection *selection, gpointer user_data)
{
    GtkTreeIter iter;
    GOFFile *file = NULL;
    FMIconView *view = FM_ICON_VIEW (user_data);

    /*GList *paths = gtk_tree_selection_get_selected_rows (selection, NULL);
      if (paths!=NULL && gtk_tree_model_get_iter (GTK_TREE_MODEL(view->model), &iter, paths->data))   
      {
      gtk_tree_model_get (GTK_TREE_MODEL (view->model), &iter,
      FM_LIST_MODEL_FILE_COLUMN, &file,
      -1);
    //if (file != NULL) 
    }*/
    if (view->details->selection != NULL)
        gof_file_list_free (view->details->selection);
    view->details->selection = get_selection(view);

    if (view->details->selection != NULL) 
        file = view->details->selection->data;

    fm_directory_view_notify_selection_changed (FM_DIRECTORY_VIEW (view), file);

    //fm_directory_view_notify_selection_changed (FM_DIRECTORY_VIEW (view), file);
    /*g_list_foreach (paths, (GFunc) gtk_tree_path_free, NULL);
      g_list_free (paths);*/
}

static void
fm_icon_view_item_activated (ExoIconView *exo_icon, GtkTreePath *path, FMIconView *view)
{
    log_printf (LOG_LEVEL_UNDEFINED, "%s\n", G_STRFUNC);
    //activate_selected_items (view);
    fm_directory_view_activate_selected_items (FM_DIRECTORY_VIEW (view));    
}

#if 0
static void
fm_icon_view_colorize_selected_items (FMDirectoryView *view, int ncolor)
{
    GList *file_list;
    GOFFile *file;
    char *uri;

    file_list = fm_icon_view_get_selection (view);
    /*guint array_length = MIN (g_list_length (file_list)*sizeof(char), 30);
      char **array = malloc(array_length + 1);
      char **l = array;*/
    for (; file_list != NULL; file_list=file_list->next)
    {
        file = file_list->data;
        //log_printf (LOG_LEVEL_UNDEFINED, "colorize %s %d\n", file->name, ncolor);
        file->color = tags_colors[ncolor];
        uri = g_file_get_uri(file->location);
        //*array = uri;
        marlin_view_tags_set_color (tags, uri, ncolor, NULL, NULL);
        g_free (uri);
    }
    /**array = NULL;
      marlin_view_tags_uris_set_color (tags, l, array_length, ncolor, NULL);*/
    /*for (; *l != NULL; l=l++)
      log_printf (LOG_LEVEL_UNDEFINED, "array uri: %s\n", *l);*/
    //g_strfreev(l);
}

static void
fm_icon_view_rename_callback (GOFFile *file,
                              GFile *result_location,
                              GError *error,
                              gpointer callback_data)
{
    FMIconView *view = FM_ICON_VIEW (callback_data);

    printf ("%s\n", G_STRFUNC);
    if (view->details->renaming_file) {
        view->details->rename_done = TRUE;

        if (error != NULL) {
            /* If the rename failed (or was cancelled), kill renaming_file.
             * We won't get a change event for the rename, so otherwise
             * it would stay around forever.
             */
            gof_file_unref (view->details->renaming_file);
            view->details->renaming_file = NULL;
        }
    }

    g_object_unref (view);
}

static void
editable_focus_out_cb (GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    FMIconView *view = user_data;

    //TODO
    //nautilus_view_unfreeze_updates (NAUTILUS_VIEW (view));
    view->details->editable_widget = NULL;
}

static void
cell_renderer_editing_started_cb (GtkCellRenderer *renderer,
                                  GtkCellEditable *editable,
                                  const gchar *path_str,
                                  FMIconView *list_view)
{
    GtkEntry *entry;

    entry = GTK_ENTRY (editable);
    list_view->details->editable_widget = editable;

    /* Free a previously allocated original_name */
    g_free (list_view->details->original_name);

    list_view->details->original_name = g_strdup (gtk_entry_get_text (entry));

    g_signal_connect (entry, "focus-out-event",
                      G_CALLBACK (editable_focus_out_cb), list_view);

    //TODO
    /*nautilus_clipboard_set_up_editable
      (GTK_EDITABLE (entry),
      nautilus_view_get_ui_manager (NAUTILUS_VIEW (list_view)),
      FALSE);*/
}

static void
cell_renderer_editing_canceled (GtkCellRendererText *cell,
                                FMIconView          *view)
{
    view->details->editable_widget = NULL;

    //TODO
    //nautilus_view_unfreeze_updates (NAUTILUS_VIEW (view));
}

static void
cell_renderer_edited (GtkCellRendererText *cell,
                      const char          *path_str,
                      const char          *new_text,
                      FMIconView          *view)
{
    GtkTreePath *path;
    GOFFile *file;
    GtkTreeIter iter;

    printf ("%s\n", G_STRFUNC);
    view->details->editable_widget = NULL;

    /* Don't allow a rename with an empty string. Revert to original 
     * without notifying the user.
     */
    if (new_text[0] == '\0') {
        g_object_set (G_OBJECT (view->details->file_name_cell),
                      "editable", FALSE, NULL);
        //nautilus_view_unfreeze_updates (FM_DIRECTORY_VIEW (view));
        return;
    }

    path = gtk_tree_path_new_from_string (path_str);

    gtk_tree_model_get_iter (GTK_TREE_MODEL (view->model), &iter, path);

    gtk_tree_path_free (path);

    gtk_tree_model_get (GTK_TREE_MODEL (view->model), &iter,
                        FM_LIST_MODEL_FILE_COLUMN, &file, -1);

    /* Only rename if name actually changed */
    if (strcmp (new_text, view->details->original_name) != 0) {
        view->details->renaming_file = gof_file_ref (file);
        view->details->rename_done = FALSE;
        gof_file_rename (file, new_text, fm_icon_view_rename_callback, g_object_ref (view));

        g_free (view->details->original_name);
        view->details->original_name = g_strdup (new_text);
    }

    gof_file_unref (file);

    /*We're done editing - make the filename-cells readonly again.*/
    g_object_set (G_OBJECT (view->details->file_name_cell),
                  "editable", FALSE, NULL);

    //TODO
    //nautilus_view_unfreeze_updates (NAUTILUS_VIEW (view));
}

static void
fm_icon_view_start_renaming_file (FMDirectoryView *view,
                                  GOFFile *file,
                                  gboolean select_all)
{
    FMIconView *list_view;
    GtkTreeIter iter;
    GtkTreePath *path;
    gint start_offset, end_offset;

    list_view = FM_ICON_VIEW (view);

    /* Select all if we are in renaming mode already */
    if (list_view->details->file_name_column && list_view->details->editable_widget) {
        gtk_editable_select_region (GTK_EDITABLE (list_view->details->editable_widget),
                                    0, -1);
        return;
    }

    if (!fm_list_model_get_first_iter_for_file (list_view->model, file, &iter)) {
        return;
    }

    /* Freeze updates to the view to prevent losing rename focus when the tree view updates */
    //TODO
    //nautilus_view_freeze_updates (NAUTILUS_VIEW (view));

    path = gtk_tree_model_get_path (GTK_TREE_MODEL (list_view->model), &iter);

    /* Make filename-cells editable. */
    g_object_set (G_OBJECT (list_view->details->file_name_cell),
                  "editable", TRUE, NULL);

    gtk_tree_view_scroll_to_cell (list_view->tree, NULL,
                                  list_view->details->file_name_column,
                                  TRUE, 0.0, 0.0);
    /* set cursor also triggers editing-started, where we save the editable widget */
    /*gtk_tree_view_set_cursor (list_view->tree, path,
      list_view->details->file_name_column, TRUE);*/
    /* sound like set_cursor is not enought to trigger editing-started, we use cursor_on_cell instead */
    gtk_tree_view_set_cursor_on_cell (list_view->tree, path,
                                      list_view->details->file_name_column,
                                      (GtkCellRenderer *) list_view->details->file_name_cell,
                                      TRUE);

    if (list_view->details->editable_widget != NULL) {
        eel_filename_get_rename_region (list_view->details->original_name,
                                        &start_offset, &end_offset);

        gtk_editable_select_region (GTK_EDITABLE (list_view->details->editable_widget),
                                    start_offset, end_offset);
    }

    gtk_tree_path_free (path);
}
#endif

static void
fm_icon_view_sync_selection (FMDirectoryView *view)
{
    FMIconView *icon_view = FM_ICON_VIEW (view);
    GOFFile *file;

    if (icon_view->details->selection != NULL) 
        file = icon_view->details->selection->data;

    fm_directory_view_notify_selection_changed (view, file);
}


/*static void
  do_popup_menu (GtkWidget *widget, FMIconView *view, GdkEventButton *event)
  {
  if (tree_view_has_selection (GTK_TREE_VIEW (widget))) {
//fm_directory_view_pop_up_selection_context_menu (FM_DIRECTORY_VIEW (view), event);
printf ("popup_selection_menu\n");
} else {
//fm_directory_view_pop_up_background_context_menu (FM_DIRECTORY_VIEW (view), event);
printf ("popup_background_menu\n");
}
}*/

static gboolean
button_press_callback (GtkTreeView *tree_view, GdkEventButton *event, FMIconView *view)
{
    GtkTreeSelection    *selection;
    GtkTreePath         *path;
    GtkTreeIter         iter;
    GtkAction           *action;

    /* check if the event is for the bin window */
    if (G_UNLIKELY (event->window != gtk_tree_view_get_bin_window (tree_view)))
        return FALSE;

    /* we unselect all selected items if the user clicks on an empty
     * area of the treeview and no modifier key is active.
     */
    if ((event->state & gtk_accelerator_get_default_mod_mask ()) == 0
        && !gtk_tree_view_get_path_at_pos (tree_view, event->x, event->y, NULL, NULL, NULL, NULL))
    {
        selection = gtk_tree_view_get_selection (tree_view);
        gtk_tree_selection_unselect_all (selection);
    }

    /* open the context menu on right clicks */
    if (event->type == GDK_BUTTON_PRESS && event->button == 3)
    {
        selection = gtk_tree_view_get_selection (tree_view);
        if (gtk_tree_view_get_path_at_pos (tree_view, event->x, event->y, &path, NULL, NULL, NULL))
        {
            /* select the path on which the user clicked if not selected yet */
            if (!gtk_tree_selection_path_is_selected (selection, path))
            {
                /* we don't unselect all other items if Control is active */
                if ((event->state & GDK_CONTROL_MASK) == 0)
                    gtk_tree_selection_unselect_all (selection);
                gtk_tree_selection_select_path (selection, path);
            }
            gtk_tree_path_free (path);

            /* queue the menu popup */
            fm_directory_view_queue_popup (FM_DIRECTORY_VIEW (view), event);
        }
        else
        {
            /* open the context menu */
            fm_directory_view_context_menu (view, event->button, event);
        }

        return TRUE;
    }
    else if ((event->type == GDK_BUTTON_PRESS || event->type == GDK_2BUTTON_PRESS) && event->button == 2)
    {
        /* determine the path to the item that was middle-clicked */
        if (gtk_tree_view_get_path_at_pos (tree_view, event->x, event->y, &path, NULL, NULL, NULL))
        {
            /* select only the path to the item on which the user clicked */
            selection = gtk_tree_view_get_selection (tree_view);
            gtk_tree_selection_unselect_all (selection);
            gtk_tree_selection_select_path (selection, path);

            /* if the event was a double-click or we are in single-click mode, then
             * we'll open the file or folder (folder's are opened in new windows)
             */
            if (G_LIKELY (event->type == GDK_2BUTTON_PRESS || exo_tree_view_get_single_click (EXO_TREE_VIEW (tree_view))))
            {
                printf ("activate selected ??\n");
#if 0
                /* determine the file for the path */
                gtk_tree_model_get_iter (GTK_TREE_MODEL (view->model), &iter, path);
                file = thunar_list_model_get_file (view->model, &iter);
                if (G_LIKELY (file != NULL))
                {
                    /* determine the action to perform depending on the type of the file */
                    /*action = thunar_gtk_ui_manager_get_action_by_name (THUNAR_STANDARD_VIEW (view)->ui_manager,
                      thunar_file_is_directory (file) ? "open-in-new-window" : "open");*/
                    printf ("open or open-in-new-window\n");

                    /* emit the action */
                    /*if (G_LIKELY (action != NULL))
                      gtk_action_activate (action);*/

                    /* release the file reference */
                    g_object_unref (G_OBJECT (file));
                }
#endif
            }

            /* cleanup */
            gtk_tree_path_free (path);
        }

        return TRUE;
    }

    return FALSE;
}

/*static gboolean
  popup_menu_callback (GtkWidget *widget, gpointer callback_data)
  {
  FMIconView *view;

  view = FM_ICON_VIEW (callback_data);

  do_popup_menu (widget, view, NULL);

  return TRUE;
  }*/

static gboolean
key_press_callback (GtkWidget *widget, GdkEventKey *event, gpointer callback_data)
{
    FMIconView *view;
    //GdkEventButton button_event = { 0 };
    gboolean handled;
    GtkTreeView *tree_view;
    GtkTreePath *path;

    tree_view = GTK_TREE_VIEW (widget);

    view = FM_ICON_VIEW (callback_data);
    handled = FALSE;

    switch (event->keyval) {
        /*case GDK_F10:
          if (event->state & GDK_CONTROL_MASK) {
          fm_directory_view_pop_up_background_context_menu (view, &button_event);
          handled = TRUE;
          }
          break;*/
    case GDK_KEY_space:
        if (event->state & GDK_CONTROL_MASK) {
            handled = FALSE;
            break;
        }
        if (!gtk_widget_has_focus (GTK_WIDGET (tree_view))) {
            handled = FALSE;
            break;
        }
        /*if ((event->state & GDK_SHIFT_MASK) != 0) {
          activate_selected_items_alternate (FM_ICON_VIEW (view), NULL, TRUE);
          } else {*/
        activate_selected_items (FM_ICON_VIEW (view));
        //}
        handled = TRUE;
        break;
    case GDK_KEY_Return:
    case GDK_KEY_KP_Enter:
        /*if ((event->state & GDK_SHIFT_MASK) != 0) {
          activate_selected_items_alternate (FM_ICON_VIEW (view), NULL, TRUE);
          } else {*/
        activate_selected_items (view);
        //}
        handled = TRUE;
        break;

    default:
        handled = FALSE;
    }

    return handled;
}

#if 0
static void
fm_icon_view_notify_model (ExoIconView *exo_icon, GParamSpec *pspec, FMIconView *view)
{
    /* We need to set the search column here, as ExoIconView resets it
     * whenever a new model is set.
     */
    exo_icon_view_set_search_column (exo_icon, FM_LIST_MODEL_FILENAME);
}
#endif


static void
create_and_set_up_icon_view (FMIconView *view)
{
    view->model = FM_DIRECTORY_VIEW (view)->model;
    g_object_set (G_OBJECT (view->model), "has-child", FALSE, NULL);

    view->icons = EXO_ICON_VIEW (exo_icon_view_new());
    //view->icons = GTK_ICON_VIEW (gtk_icon_view_new());
    //g_object_set (G_OBJECT (view->icons), "model", GTK_TREE_MODEL (view->model), NULL);
    exo_icon_view_set_model (view->icons, GTK_TREE_MODEL (view->model));

    /*exo_icon_view_set_text_column (view->icons, FM_LIST_MODEL_FILENAME);
      exo_icon_view_set_pixbuf_column (view->icons, FM_LIST_MODEL_ICON);*/

    /*g_signal_connect (G_OBJECT (view->icons), "notify::model", G_CALLBACK (fm_icon_view_notify_model), view);*/
    g_signal_connect (G_OBJECT (view->icons), "item-activated", G_CALLBACK (fm_icon_view_item_activated), view);
    g_signal_connect (G_OBJECT (view->icons), "selection-changed", G_CALLBACK (fm_icon_view_selection_changed), view);


    exo_icon_view_set_selection_mode (view->icons, GTK_SELECTION_MULTIPLE);
    /*exo_icon_view_set_enable_search (view->icons, TRUE);*/

    /*g_object_set (G_OBJECT (view->icons), "text-column", FM_LIST_MODEL_FILENAME, 
      "pixbuf-column", FM_LIST_MODEL_ICON, NULL);*/

    //#if 0
    /* add the abstract icon renderer */
    g_object_set (G_OBJECT (FM_DIRECTORY_VIEW (view)->icon_renderer), "follow-state", TRUE, NULL);
    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (view->icons), FM_DIRECTORY_VIEW (view)->icon_renderer, FALSE);
    gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT (view->icons), FM_DIRECTORY_VIEW (view)->icon_renderer, "file", FM_LIST_MODEL_FILE_COLUMN);

    /* add the name renderer */
    g_object_set (G_OBJECT (FM_DIRECTORY_VIEW (view)->name_renderer), "follow-state", TRUE, NULL);
    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (view->icons), FM_DIRECTORY_VIEW (view)->name_renderer, TRUE);
    gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT (view->icons), FM_DIRECTORY_VIEW (view)->name_renderer, "text", FM_LIST_MODEL_FILENAME);
    //#endif




#if 0
    /*exo_tree_view_set_single_click (EXO_TREE_VIEW (view->tree), TRUE);
      exo_tree_view_set_single_click_timeout (EXO_TREE_VIEW (view->tree), 350);*/

    g_signal_connect_object (view->tree, "button-press-event",
                             G_CALLBACK (button_press_callback), view, 0);
    g_signal_connect_object (view->tree, "key_press_event",
                             G_CALLBACK (key_press_callback), view, 0);
    g_signal_connect_object (view->tree, "row-activated",
                             G_CALLBACK (row_activated_callback), view, 0);

#endif
    gtk_widget_show (GTK_WIDGET (view->icons));
    gtk_container_add (GTK_CONTAINER (view), GTK_WIDGET (view->icons));

}

static void
fm_icon_view_add_file (FMDirectoryView *view, GOFFile *file, GOFDirectoryAsync *directory)
{
    FMListModel *model;

    model = FM_ICON_VIEW (view)->model;
    fm_list_model_add_file (model, file, directory);
}

//TODO move this to fm_directory_view
static void
fm_icon_view_remove_file (FMDirectoryView *view, GOFFile *file, GOFDirectoryAsync *directory)
{
    FMIconView *icon_view = FM_ICON_VIEW (view);
        
    fm_list_model_remove_file (icon_view->model, file, directory);
}

#if 0
static void
fm_icon_view_remove_file (FMDirectoryView *view, GOFFile *file, GOFDirectoryAsync *directory)
{
    printf ("%s %s\n", G_STRFUNC, g_file_get_uri(file->location));
    
    GtkTreePath *path;
    GtkTreePath *file_path;
    GtkTreeIter iter;
    GtkTreeIter temp_iter;
    GtkTreeRowReference* row_reference;
    FMIconView *icon_view;
    GtkTreeModel* tree_model; 
    GtkTreeSelection *selection;

    path = NULL;
    row_reference = NULL;
    icon_view = FM_ICON_VIEW (view);
    tree_model = GTK_TREE_MODEL(icon_view->model);

    if (fm_list_model_get_tree_iter_from_file (icon_view->model, file, directory, &iter))
    {
        selection =  exo_icon_view_get_selected_items (icon_view->icons);
        file_path = gtk_tree_model_get_path (tree_model, &iter);

        if (exo_icon_view_path_is_selected (icon_view->icons, file_path)) {
            /* get reference for next element in the list view. If the element to be deleted is the 
             * last one, get reference to previous element. If there is only one element in view
             * no need to select anything.
             */
            temp_iter = iter;

            if (gtk_tree_model_iter_next (tree_model, &iter)) {
                path = gtk_tree_model_get_path (tree_model, &iter);
                row_reference = gtk_tree_row_reference_new (tree_model, path);
            } else {
                path = gtk_tree_model_get_path (tree_model, &temp_iter);
                if (gtk_tree_path_prev (path)) {
                    row_reference = gtk_tree_row_reference_new (tree_model, path);
                }
            }
            gtk_tree_path_free (path);
        }

        gtk_tree_path_free (file_path);

        fm_list_model_remove_file (icon_view->model, file, directory);

        if (gtk_tree_row_reference_valid (row_reference)) {
            if (icon_view->details->new_selection_path) {
                gtk_tree_path_free (icon_view->details->new_selection_path);
            }
            icon_view->details->new_selection_path = gtk_tree_row_reference_get_path (row_reference);
        }

        if (row_reference) {
            gtk_tree_row_reference_free (row_reference);
        }
    }
}
#endif


/*
   static void
   fm_icon_view_clear (FMIconView *view)
   {
   if (view->model != NULL) {
//stop_cell_editing (view);
fm_list_model_clear (view->model);
}
}*/

/*
   static void
   get_selection_foreach_func (GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data)
   {
   GList **list;
   GOFFile *file;

   list = data;

   gtk_tree_model_get (model, iter,
   FM_LIST_MODEL_FILE_COLUMN, &file,
   -1);

   if (file != NULL) {
   (* list) = g_list_prepend ((* list), file);
   }
   }*/

static GList *
get_selection (FMIconView *view)
{
    GList *lp, *selected_files;
    gint n_selected_files = 0;
    GtkTreePath *path;

    /* determine the new list of selected files (replacing GtkTreePath's with GOFFile's) */
    selected_files = exo_icon_view_get_selected_items (view->icons);
    for (lp = selected_files; lp != NULL; lp = lp->next, ++n_selected_files)
    {
        path = lp->data;
        /* replace it the path with the file */
        lp->data = fm_list_model_file_for_path (view->model, lp->data);

        /* release the tree path... */
        gtk_tree_path_free (path);
    }

    return selected_files;
}

static GList *
fm_icon_view_get_selection (FMDirectoryView *view)
{
    return FM_ICON_VIEW (view)->details->selection;
}

static GList *
fm_list_view_get_selection_for_file_transfer (FMDirectoryView *view)
{
    GList *list = g_list_copy (fm_icon_view_get_selection (view));
    g_list_foreach (list, (GFunc) gof_file_ref, NULL);

    return list;
}

/*static void
  fm_icon_view_set_selection (FMIconView *list_view, GList *selection)
  {
  GtkTreeSelection *tree_selection;
  GList *node;
  GList *iters, *l;
  GOFFile *file;

  tree_selection = gtk_tree_view_get_selection (list_view->tree);

//g_signal_handlers_block_by_func (tree_selection, list_selection_changed_callback, view);

gtk_tree_selection_unselect_all (tree_selection);
for (node = selection; node != NULL; node = node->next) {
file = node->data;
iters = fm_list_model_get_all_iters_for_file (list_view->model, file);

for (l = iters; l != NULL; l = l->next) {
gtk_tree_selection_select_iter (tree_selection,
(GtkTreeIter *)l->data);
}
//eel_g_list_free_deep (iters);
}

//g_signal_handlers_unblock_by_func (tree_selection, list_selection_changed_callback, view);
//fm_directory_view_notify_selection_changed (view);
}

static void
fm_icon_view_select_all (FMIconView *view)
{
gtk_tree_selection_select_all (gtk_tree_view_get_selection (view->tree));
}*/

static GtkTreePath*
fm_icon_view_get_path_at_pos (FMDirectoryView *view, gint x, gint y)
{
    GtkTreePath *path;

    g_return_val_if_fail (FM_IS_ICON_VIEW (view), NULL);
    //return exo_icon_view_get_path_at_pos (FM_ICON_VIEW (view)->icons, x, y);

    if (exo_icon_view_get_dest_item_at_pos  (FM_ICON_VIEW (view)->icons, x, y, &path, NULL))
        return path;

    return NULL;
}

static void
fm_icon_view_highlight_path (FMDirectoryView *view, GtkTreePath *path)
{
    g_return_if_fail (FM_IS_ICON_VIEW (view));

    exo_icon_view_set_drag_dest_item (FM_ICON_VIEW (view)->icons, path, EXO_ICON_VIEW_DROP_INTO);
}


static void
fm_icon_view_finalize (GObject *object)
{
    FMIconView *view = FM_ICON_VIEW (object);

    log_printf (LOG_LEVEL_UNDEFINED, "$$ %s\n", G_STRFUNC);

    g_free (view->details->original_name);
    view->details->original_name = NULL;

    if (view->details->new_selection_path)
        gtk_tree_path_free (view->details->new_selection_path);
    if (view->details->selection)
        gof_file_list_free (view->details->selection);

    g_signal_handlers_disconnect_by_func (marlin_icon_view_settings,
                                          fm_icon_view_zoom_level_changed, view);

    g_object_unref (view->model);
    g_free (view->details);
    G_OBJECT_CLASS (fm_icon_view_parent_class)->finalize (object); 
}

static void
fm_icon_view_init (FMIconView *view)
{
    view->details = g_new0 (FMIconViewDetails, 1);
    view->details->selection = NULL;

    create_and_set_up_icon_view (view);

    /* setup the icon renderer */
    g_object_set (G_OBJECT (FM_DIRECTORY_VIEW (view)->icon_renderer),
                  "ypad", 3u, NULL);

    /* setup the name renderer */
    g_object_set (G_OBJECT (FM_DIRECTORY_VIEW (view)->name_renderer),
                  "wrap-mode", PANGO_WRAP_WORD_CHAR, NULL);

    /* TODO */
    /* synchronize the "text-beside-icons" property with the global preference */
    /**/
    g_object_set (G_OBJECT (view), "text-beside-icons", FALSE, NULL);

    /*g_signal_connect_swapped (marlin_icon_view_settings, "changed::zoom-level",
      G_CALLBACK (zoom_level_changed), view);*/

    /*g_settings_bind (marlin_icon_view_settings, "zoom-level", 
      FM_DIRECTORY_VIEW (view)->icon_renderer, "size", 0);*/


    //TODO
    /*g_settings_bind (settings, "single-click", 
      EXO_TREE_VIEW (view->tree), "single-click", 0);
      g_settings_bind (settings, "single-click-timeout", 
      EXO_TREE_VIEW (view->tree), "single-click-timeout", 0);*/
}

static void
fm_icon_view_class_init (FMIconViewClass *klass)
{
    FMDirectoryViewClass *fm_directory_view_class;
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    //GParamSpec   *pspec;

    object_class->finalize     = fm_icon_view_finalize;
    /*object_class->get_property = _get_property;*/
    object_class->set_property = fm_icon_view_set_property;

    fm_directory_view_class = FM_DIRECTORY_VIEW_CLASS (klass);

    fm_directory_view_class->add_file = fm_icon_view_add_file;
    fm_directory_view_class->remove_file = fm_icon_view_remove_file;
    //fm_directory_view_class->colorize_selection = fm_icon_view_colorize_selected_items;        
    fm_directory_view_class->sync_selection = fm_icon_view_sync_selection;
    fm_directory_view_class->get_selection = fm_icon_view_get_selection;
    fm_directory_view_class->get_selection_for_file_transfer = fm_list_view_get_selection_for_file_transfer;

    fm_directory_view_class->get_path_at_pos = fm_icon_view_get_path_at_pos;
    fm_directory_view_class->highlight_path = fm_icon_view_highlight_path;
    //fm_directory_view_class->start_renaming_file = fm_icon_view_start_renaming_file;


    g_object_class_install_property (object_class,
                                     PROP_TEXT_BESIDE_ICONS,
                                     g_param_spec_boolean ("text-beside-icons", 
                                                           "text-beside-icons",
                                                           "text-beside-icons",
                                                           FALSE,
                                                           (G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS)));

    //eel_g_settings_add_auto_boolean (settings, "single-click", &single_click);
    //g_type_class_add_private (object_class, sizeof (GOFDirectoryAsyncPrivate));
}

static void
fm_icon_view_set_property (GObject      *object,
                           guint         prop_id,
                           const GValue *value,
                           GParamSpec   *pspec)
{
    FMDirectoryView *view = FM_DIRECTORY_VIEW (object);

    switch (prop_id)
    {
    case PROP_TEXT_BESIDE_ICONS:
        if (G_UNLIKELY (g_value_get_boolean (value)))
        {
            exo_icon_view_set_item_orientation (FM_ICON_VIEW (view)->icons, GTK_ORIENTATION_HORIZONTAL);
            //g_object_set (G_OBJECT (view->name_renderer), "wrap-width", 128, "yalign", 0.5f, NULL);
            g_object_set (G_OBJECT (view->name_renderer), "wrap-width", 128, "xalign", 0.0f, "yalign", 0.5f, NULL);

            /* disconnect the "zoom-level" signal handler, since we're using a fixed wrap-width here */
            g_signal_handlers_disconnect_by_func (marlin_icon_view_settings,
                                                  fm_icon_view_zoom_level_changed, view);
        }
        else
        {
            exo_icon_view_set_item_orientation (FM_ICON_VIEW (view)->icons, GTK_ORIENTATION_VERTICAL);
            g_object_set (G_OBJECT (view->name_renderer), "yalign", 0.0f, NULL);

            /* connect the "zoom-level" signal handler as the wrap-width is now synced with the "zoom-level" */
            g_signal_connect_swapped (marlin_icon_view_settings, "changed::zoom-level",
                                      G_CALLBACK (fm_icon_view_zoom_level_changed), view);
            fm_icon_view_zoom_level_changed (view);
        }
        break;

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
fm_icon_view_zoom_level_changed (FMDirectoryView *view)
{
    MarlinZoomLevel zoom_level;
    gint wrap_width;

    g_return_if_fail (FM_IS_DIRECTORY_VIEW (view));

    zoom_level = g_settings_get_enum (marlin_icon_view_settings, "zoom-level");
    /* determine the "wrap-width" depending on the "zoom-level" */
    switch (zoom_level)
    {
    case MARLIN_ZOOM_LEVEL_SMALLEST:
        wrap_width = 48;
        break;

    case MARLIN_ZOOM_LEVEL_SMALLER:
        wrap_width = 64;
        break;

    case MARLIN_ZOOM_LEVEL_SMALL:
        wrap_width = 72;
        break;

    case MARLIN_ZOOM_LEVEL_NORMAL:
        wrap_width = 112;
        break;

    default:
        wrap_width = 128;
        break;
    }

    /* set the new "wrap-width" for the text renderer */
    g_object_set (G_OBJECT (FM_DIRECTORY_VIEW (view)->name_renderer), "wrap-width", wrap_width, NULL);
    /* set the new "size" for the icon renderer */
    g_object_set (G_OBJECT (FM_DIRECTORY_VIEW (view)->icon_renderer), "size", marlin_zoom_level_to_icon_size (zoom_level), NULL);
   
    /* TODO move this to icon renderer ? */
    gint xpad, ypad;

    gtk_cell_renderer_get_padding (view->icon_renderer, &xpad, &ypad);
    gtk_cell_renderer_set_fixed_size (GTK_CELL_RENDERER (view->icon_renderer),
                                      marlin_zoom_level_to_icon_size (zoom_level)+ 2 * xpad,
                                      marlin_zoom_level_to_icon_size (zoom_level)+ 2 * ypad);
    /*exo_icon_view_set_spacing (FM_ICON_VIEW (view)->icons, 0);
    exo_icon_view_set_column_spacing (FM_ICON_VIEW (view)->icons, 0);
    exo_icon_view_set_row_spacing (FM_ICON_VIEW (view)->icons, 0);*/
    //gtk_widget_queue_draw (GTK_WIDGET (view));
    exo_icon_view_invalidate_sizes (FM_ICON_VIEW (view)->icons);
    //gtk_widget_queue_draw (GTK_WIDGET (FM_ICON_VIEW (view)->icons));
}
