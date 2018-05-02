/*
 * Copyright (c) 2008-2014, Pedigree Developers
 *
 * Please see the CONTRIB file in the root of the source tree for a full
 * list of contributors.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _ARGZ_H_
#define _ARGZ_H_

#include <errno.h>
#include <sys/types.h>

#include "_ansi.h"

_BEGIN_STD_C

/* The newlib implementation of these functions assumes that sizeof(char) == 1. */
error_t argz_create (char *const argv[], char **argz, size_t *argz_len);
error_t argz_create_sep (const char *string, int sep, char **argz, size_t *argz_len);
size_t argz_count (const char *argz, size_t argz_len);
void argz_extract (char *argz, size_t argz_len, char **argv);
void argz_stringify (char *argz, size_t argz_len, int sep);
error_t argz_add (char **argz, size_t *argz_len, const char *str);
error_t argz_add_sep (char **argz, size_t *argz_len, const char *str, int sep);
error_t argz_append (char **argz, size_t *argz_len, const char *buf, size_t buf_len);
error_t argz_delete (char **argz, size_t *argz_len, char *entry);
error_t argz_insert (char **argz, size_t *argz_len, char *before, const char *entry);
char * argz_next (char *argz, size_t argz_len, const char *entry);
error_t argz_replace (char **argz, size_t *argz_len, const char *str, const char *with, unsigned *replace_count);

_END_STD_C

#endif /* _ARGZ_H_ */