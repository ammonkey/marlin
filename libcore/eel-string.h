/* eel-string.h: String routines to augment <string.h>.
 *
 * Copyright (C) 2000 Eazel, Inc.
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
 */

#ifndef EEL_STRING_H
#define EEL_STRING_H

#include <glib.h>
#include <string.h>
#include <stdarg.h>

/* We use the "str" abbrevation to mean char * string, since
 * "string" usually means g_string instead. We use the "istr"
 * abbreviation to mean a case-insensitive char *.
 */

#if 0
/* NULL is allowed for all the str parameters to these functions. */

/* Versions of basic string functions that allow NULL, and handle
 * cases that the standard ones get a bit wrong for our purposes.
 */
size_t   eel_strlen                        (const char    *str);
char *   eel_strchr                        (const char    *haystack,
                                            char           needle);
#endif
int      eel_strcmp                        (const char    *str_a,
                                            const char    *str_b);
#if 0
int      eel_strcasecmp                    (const char    *str_a,
                                            const char    *str_b);
int      eel_strcmp_case_breaks_ties       (const char    *str_a,
                                            const char    *str_b);
#endif

/* Other basic string operations. */
gboolean eel_str_is_empty                  (const char    *str_or_null);
gboolean eel_str_is_equal                  (const char    *str_a,
                                            const char    *str_b);
#if 0
gboolean eel_istr_is_equal                 (const char    *str_a,
                                            const char    *str_b);
gboolean eel_str_has_prefix                (const char    *target,
                                            const char    *prefix);
char *   eel_str_get_prefix                (const char    *source,
                                            const char    *delimiter);
gboolean eel_istr_has_prefix               (const char    *target,
                                            const char    *prefix);
gboolean eel_str_has_suffix                (const char    *target,
                                            const char    *suffix);
gboolean eel_istr_has_suffix               (const char    *target,
                                            const char    *suffix);

/* Conversions to and from strings. */
gboolean eel_str_to_int                    (const char    *str,
                                            int           *integer);
#endif
/* Escape function for '_' character. */
char *   eel_str_double_underscores        (const char    *str);

#if 0
/* Capitalize a string */
char *   eel_str_capitalize                (const char    *str);
#endif

/* Middle truncate a string to a maximum of truncate_length characters.
 * The resulting string will be truncated in the middle with a "..."
 * delimiter.
 */
char *   eel_str_middle_truncate           (const char    *str,
                                            guint          truncate_length);

#if 0
/* Remove all characters after the passed-in substring. */
char *   eel_str_strip_substring_and_after (const char    *str,
                                            const char    *substring);

/* Replace all occurrences of substring with replacement. */
char *   eel_str_replace_substring         (const char    *str,
                                            const char    *substring,
                                            const char    *replacement);

typedef char * eel_ref_str;

eel_ref_str eel_ref_str_new        (const char  *string);
eel_ref_str eel_ref_str_get_unique (const char  *string);
eel_ref_str eel_ref_str_ref        (eel_ref_str  str);
void        eel_ref_str_unref      (eel_ref_str  str);

#define eel_ref_str_peek(__str) ((const char *)(__str))
#endif

typedef struct {
    char character;
    char *(*to_string) (char *format, va_list va);
    void (*skip) (va_list *va);
} EelPrintfHandler;

#if 0
char *eel_strdup_printf_with_custom (EelPrintfHandler *handlers,
                                     const char *format,
                                     ...);
#endif
char *eel_strdup_vprintf_with_custom (EelPrintfHandler *custom,
                                      const char *format,
                                      va_list va);

#endif /* EEL_STRING_H */
