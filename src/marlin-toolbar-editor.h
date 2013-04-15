/*
 * Copyright (C) 2010 ammonkey
 *
 * Marlin is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * Marlin is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: ammonkey <am.monkeyd@gmail.com>
 *
 */

#ifndef MARLIN_TOOLBAR_EDITOR_H
#define MARLIN_TOOLBAR_EDITOR_H

#include <glib-object.h>
#include <gtk/gtk.h>
#include "marlin-vala.h"

G_BEGIN_DECLS

void marlin_toolbar_editor_dialog_show (MarlinViewWindow *mvw);

G_END_DECLS

#endif /* MARLIN_TOOLBAR_EDITOR_H */
