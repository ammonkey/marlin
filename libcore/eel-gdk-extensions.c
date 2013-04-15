/* eel-gdk-extensions.c: Graphics routines to augment what's in gdk.
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
 * Authors: Darin Adler <darin@eazel.com>, 
 *          Pavel Cisler <pavel@eazel.com>,
 *          Ramiro Estrugo <ramiro@eazel.com>
 */

#include <config.h>
#include "eel-gdk-extensions.h"

#include "eel-glib-extensions.h"
#include "eel-string.h"
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <stdlib.h>
#include <pango/pango.h>

/**
 * eel_gdk_rgba_is_dark:
 * 
 * Return true if the given color is `dark'
 */
/*gboolean
  eel_gdk_rgba_is_dark (const GdkRGBA *color)
  {
  int intensity;

  intensity = ((((int) (color->red) >> 8) * 77)
  + (((int) (color->green) >> 8) * 150)
  + (((int) (color->blue) >> 8) * 28)) >> 8;

  return intensity < 128;
  }*/

EelGdkGeometryFlags
eel_gdk_parse_geometry (const char *string, int *x_return, int *y_return,
			guint *width_return, guint *height_return)
{
    int x11_flags;
    EelGdkGeometryFlags gdk_flags;

    g_return_val_if_fail (string != NULL, EEL_GDK_NO_VALUE);
    g_return_val_if_fail (x_return != NULL, EEL_GDK_NO_VALUE);
    g_return_val_if_fail (y_return != NULL, EEL_GDK_NO_VALUE);
    g_return_val_if_fail (width_return != NULL, EEL_GDK_NO_VALUE);
    g_return_val_if_fail (height_return != NULL, EEL_GDK_NO_VALUE);

    x11_flags = XParseGeometry (string, x_return, y_return,
				width_return, height_return);

    gdk_flags = EEL_GDK_NO_VALUE;
    if (x11_flags & XValue) {
	gdk_flags |= EEL_GDK_X_VALUE;
    }
    if (x11_flags & YValue) {
	gdk_flags |= EEL_GDK_Y_VALUE;
    }
    if (x11_flags & WidthValue) {
	gdk_flags |= EEL_GDK_WIDTH_VALUE;
    }
    if (x11_flags & HeightValue) {
	gdk_flags |= EEL_GDK_HEIGHT_VALUE;
    }
    if (x11_flags & XNegative) {
	gdk_flags |= EEL_GDK_X_NEGATIVE;
    }
    if (x11_flags & YNegative) {
	gdk_flags |= EEL_GDK_Y_NEGATIVE;
    }

    return gdk_flags;
}

#if 0
void
eel_cairo_draw_layout_with_drop_shadow (cairo_t     *cr,
					GdkRGBA     *text_color,
					GdkRGBA     *shadow_color,
					int          x,
					int          y,
					PangoLayout *layout)
{
    cairo_save (cr);

    gdk_cairo_set_source_rgba (cr, shadow_color);
    cairo_move_to (cr, x+1, y+1);
    pango_cairo_show_layout (cr, layout);

    gdk_cairo_set_source_rgba (cr, text_color);
    cairo_move_to (cr, x, y);
    pango_cairo_show_layout (cr, layout);

    cairo_restore (cr);
}

#define CLAMP_COLOR(v) (t = (v), CLAMP (t, 0, 1))
#define SATURATE(v) ((1.0 - saturation) * intensity + saturation * (v))

void
eel_make_color_inactive (GdkRGBA *color)
{
    double intensity, saturation;
    gdouble t;

    saturation = 0.7;
    intensity = color->red * 0.30 + color->green * 0.59 + color->blue * 0.11;
    color->red = SATURATE (color->red);
    color->green = SATURATE (color->green);
    color->blue = SATURATE (color->blue);

    if (intensity > 0.5) {
	color->red *= 0.9;
	color->green *= 0.9;
	color->blue *= 0.9;
    } else {
	color->red *= 1.25;
	color->green *= 1.25;
	color->blue *= 1.25;
    }

    color->red = CLAMP_COLOR (color->red);
    color->green = CLAMP_COLOR (color->green);
    color->blue = CLAMP_COLOR (color->blue);
}
#endif
