/* eel-vfs-extensions.c - gnome-vfs extensions.  Its likely some of these will 
 *                        be part of gnome-vfs in the future.
 *
 * Copyright (C) 1999, 2000 Eazel, Inc.
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
 * Authors: Darin Adler <darin@eazel.com>
 *	    Pavel Cisler <pavel@eazel.com>
 *	    Mike Fleming  <mfleming@eazel.com>
 *          John Sullivan <sullivan@eazel.com>
 */

#ifndef EEL_VFS_EXTENSIONS_H
#define EEL_VFS_EXTENSIONS_H

#include <glib.h>

G_BEGIN_DECLS

/*#define	EEL_TRASH_URI "trash:"
#define EEL_DESKTOP_URI "x-nautilus-desktop:"
#define EEL_SEARCH_URI "x-nautilus-search:"*/

char *             eel_make_valid_utf8                   (const char           *name);

char *             eel_filename_strip_extension          (const char           *filename);
void               eel_filename_get_rename_region        (const char           *filename,
                                                          int                  *start_offset,
                                                          int                  *end_offset);

G_END_DECLS

#endif /* EEL_VFS_EXTENSIONS_H */
