/* nautilus-file-utilities.h - interface for file manipulation routines.
 *
 *  Copyright (C) 1999, 2000, 2001 Eazel, Inc.
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
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: John Sullivan <sullivan@eazel.com>
 */

#ifndef MARLIN_FILE_UTILITIES_H
#define MARLIN_FILE_UTILITIES_H

#include <gio/gio.h>
#include <gtk/gtk.h>

//char *  marlin_get_xdg_dir                      (const char *type);
char	*marlin_get_accel_map_file		(void);

void    marlin_restore_files_from_trash         (GList *files, GtkWindow *parent_window);

#endif /* MARLIN_FILE_UTILITIES_H */
