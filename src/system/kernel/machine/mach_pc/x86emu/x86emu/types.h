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

#ifndef __X86EMU_TYPES_H
#define __X86EMU_TYPES_H

// include <types/types.h>
#ifndef NO_SYS_HEADERS
// include <sys/types.h>
#endif

/*
 * The following kludge is an attempt to work around typedef conflicts with
 * <sys/types.h>.
 */
#define u8 x86emuu8
#define u16 x86emuu16
#define u32 x86emuu32
#define u64 x86emuu64
#define s8 x86emus8
#define s16 x86emus16
#define s32 x86emus32
#define s64 x86emus64
#define uint x86emuuint
#define sint x86emusint

/*---------------------- Macros and type definitions ----------------------*/

/* Currently only for Linux/32bit */
#undef __HAS_LONG_LONG__
#if defined(__GNUC__) && !defined(NO_LONG_LONG)
#define __HAS_LONG_LONG__
#endif

/* Taken from Xmd.h */
#undef NUM32
#if defined(_LP64) || defined(__alpha) || defined(__alpha__) ||     \
    defined(__ia64__) || defined(ia64) || defined(__sparc64__) ||   \
    defined(__s390x__) || (defined(__hppa__) && defined(__LP64)) || \
    defined(__amd64__) || defined(amd64) ||                         \
    (defined(__sgi) && (_MIPS_SZLONG == 64))
#define NUM32 int
#else
#define NUM32 long
#endif

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned NUM32 u32;
#ifdef __HAS_LONG_LONG__
typedef unsigned long long u64;
#endif

typedef char s8;
typedef short s16;
typedef NUM32 s32;
#ifdef __HAS_LONG_LONG__
typedef long long s64;
#endif

typedef unsigned int uint;
typedef int sint;

typedef u16 X86EMU_pioAddr;

#undef NUM32

#endif /* __X86EMU_TYPES_H */
