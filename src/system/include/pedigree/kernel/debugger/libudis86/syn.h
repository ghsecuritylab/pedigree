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

#ifndef UD_SYN_H
#define UD_SYN_H

#include "pedigree/kernel/utilities/utility.h"

// include <stdio.h>
#include "pedigree/kernel/debugger/libudis86/types.h"
#include <stdarg.h>

extern const char *ud_reg_tab[];

static void mkasm(struct ud *u, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    u->insn_fill +=
        VStringFormat((char *) u->insn_buffer + u->insn_fill, fmt, ap);
    va_end(ap);
}

#endif
