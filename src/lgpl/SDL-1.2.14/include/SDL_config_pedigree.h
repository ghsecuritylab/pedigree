/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2009 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/

#ifndef _SDL_config_pedigree_h
#define _SDL_config_pedigree_h

/* This is a set of defines to configure the SDL features */

/* General platform specific identifiers */
#include "SDL_platform.h"

#include <stdarg.h>

/* Make sure that this isn't included by Visual C++ */
#ifdef _MSC_VER
#error You should copy include/SDL_config.h.default to include/SDL_config.h
#endif

/* Endianness */
#define SDL_BYTEORDER 1234

/* Comment this if you want to build without any C library requirements */
#define HAVE_LIBC 1
#if HAVE_LIBC

/* Useful headers */
#define HAVE_ALLOCA_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_STDIO_H 1
#define STDC_HEADERS 1
#define HAVE_STDLIB_H 1
#define HAVE_STDARG_H 1
#define HAVE_MALLOC_H 1
#undef HAVE_MEMORY_H
#define HAVE_STRING_H 1
#define HAVE_STRINGS_H 1
#define HAVE_INTTYPES_H 1
#define HAVE_STDINT_H 1
#define HAVE_CTYPE_H 1
#define HAVE_MATH_H 1
#undef HAVE_ICONV_H
#define HAVE_SIGNAL_H 1
#undef HAVE_ALTIVEC_H

/* C library functions */
#define HAVE_MALLOC 1
#define HAVE_CALLOC 1
#define HAVE_REALLOC 1
#define HAVE_FREE 1
#define HAVE_ALLOCA 1
#ifndef _WIN32 /* Don't use C runtime versions of these on Windows */
#define HAVE_GETENV 1
#define HAVE_PUTENV 1
#define HAVE_UNSETENV 1
#endif
#define HAVE_QSORT 1
#define HAVE_ABS 1
#define HAVE_BCOPY 1
#define HAVE_MEMSET 1
#define HAVE_MEMCPY 1
#define HAVE_MEMMOVE 1
#define HAVE_MEMCMP 1
#define HAVE_STRLEN 1
#define HAVE_STRLCPY 1
#define HAVE_STRLCAT 1
#define HAVE_STRDUP 1
#undef HAVE__STRREV
#undef HAVE__STRUPR
#undef HAVE__STRLWR
#define HAVE_INDEX 1
#define HAVE_RINDEX 1
#define HAVE_STRCHR 1
#define HAVE_STRRCHR 1
#define HAVE_STRSTR 1
#define HAVE_ITOA 1
#undef HAVE__LTOA
#undef HAVE__UITOA
#undef HAVE__ULTOA
#define HAVE_STRTOL 1
#define HAVE_STRTOUL 1
#undef HAVE__I64TOA
#undef HAVE__UI64TOA
#define HAVE_STRTOLL 1
#define HAVE_STRTOULL 1
#define HAVE_STRTOD 1
#define HAVE_ATOI 1
#define HAVE_ATOF 1
#define HAVE_STRCMP 1
#define HAVE_STRNCMP 1
#undef HAVE__STRICMP
#define HAVE_STRCASECMP 1
#undef HAVE__STRNICMP
#define HAVE_STRNCASECMP 1
#define HAVE_SSCANF 1
#define HAVE_SNPRINTF 1
#define HAVE_VSNPRINTF 1
#undef HAVE_ICONV
#define HAVE_SIGACTION 1
#undef HAVE_SETJMP
#define HAVE_NANOSLEEP 1
#undef HAVE_CLOCK_GETTIME
#define HAVE_GETPAGESIZE 1
#undef HAVE_MPROTECT

#else
/* We may need some replacement for stdarg.h here */
#include <stdarg.h>
#endif /* HAVE_LIBC */

/* Allow disabling of core subsystems */
#define SDL_AUDIO_DISABLED 1
#define SDL_CDROM_DISABLED 1
#define SDL_CPUINFO_DISABLED 1
#define SDL_EVENTS_DISABLED 1
#define SDL_FILE_DISABLED 1
#define SDL_JOYSTICK_DISABLED 1
#define SDL_LOADSO_DISABLED 1
#define SDL_THREADS_DISABLED 1
#define SDL_TIMERS_DISABLED 1
#define SDL_VIDEO_DISABLED 0

/* Enable various audio drivers */
#undef SDL_AUDIO_DRIVER_ALSA
#undef SDL_AUDIO_DRIVER_ALSA_DYNAMIC
#undef SDL_AUDIO_DRIVER_ARTS
#undef SDL_AUDIO_DRIVER_ARTS_DYNAMIC
#undef SDL_AUDIO_DRIVER_BAUDIO
#undef SDL_AUDIO_DRIVER_BSD
#undef SDL_AUDIO_DRIVER_COREAUDIO
#undef SDL_AUDIO_DRIVER_DART
#undef SDL_AUDIO_DRIVER_DC
#undef SDL_AUDIO_DRIVER_DISK
#define SDL_AUDIO_DRIVER_DUMMY 1
#undef SDL_AUDIO_DRIVER_DMEDIA
#undef SDL_AUDIO_DRIVER_DSOUND
#undef SDL_AUDIO_DRIVER_PULSE
#undef SDL_AUDIO_DRIVER_PULSE_DYNAMIC
#undef SDL_AUDIO_DRIVER_ESD
#undef SDL_AUDIO_DRIVER_ESD_DYNAMIC
#undef SDL_AUDIO_DRIVER_MINT
#undef SDL_AUDIO_DRIVER_MMEAUDIO
#undef SDL_AUDIO_DRIVER_NAS
#undef SDL_AUDIO_DRIVER_NAS_DYNAMIC
#undef SDL_AUDIO_DRIVER_OSS
#undef SDL_AUDIO_DRIVER_OSS_SOUNDCARD_H
#undef SDL_AUDIO_DRIVER_PAUD
#undef SDL_AUDIO_DRIVER_QNXNTO
#undef SDL_AUDIO_DRIVER_SNDMGR
#undef SDL_AUDIO_DRIVER_SUNAUDIO
#undef SDL_AUDIO_DRIVER_WAVEOUT

/* Enable various cdrom drivers */
#undef SDL_CDROM_AIX
#undef SDL_CDROM_BEOS
#undef SDL_CDROM_BSDI
#undef SDL_CDROM_DC
#define SDL_CDROM_DUMMY 1
#undef SDL_CDROM_FREEBSD
#undef SDL_CDROM_LINUX
#undef SDL_CDROM_MACOS
#undef SDL_CDROM_MACOSX
#undef SDL_CDROM_MINT
#undef SDL_CDROM_OPENBSD
#undef SDL_CDROM_OS2
#undef SDL_CDROM_OSF
#undef SDL_CDROM_QNX
#undef SDL_CDROM_WIN32

/* Enable various input drivers */
#undef SDL_INPUT_LINUXEV
#undef SDL_INPUT_TSLIB
#undef SDL_JOYSTICK_BEOS
#undef SDL_JOYSTICK_DC
#define SDL_JOYSTICK_DUMMY 1
#undef SDL_JOYSTICK_IOKIT
#undef SDL_JOYSTICK_LINUX
#undef SDL_JOYSTICK_MACOS
#undef SDL_JOYSTICK_MINT
#undef SDL_JOYSTICK_OS2
#undef SDL_JOYSTICK_RISCOS
#undef SDL_JOYSTICK_WINMM
#undef SDL_JOYSTICK_USBHID
#undef SDL_JOYSTICK_USBHID_MACHINE_JOYSTICK_H

/* Enable various shared object loading systems */
#undef SDL_LOADSO_BEOS
#undef SDL_LOADSO_DLCOMPAT
#define SDL_LOADSO_DLOPEN 1
#undef SDL_LOADSO_DUMMY
#undef SDL_LOADSO_LDG
#undef SDL_LOADSO_MACOS
#undef SDL_LOADSO_OS2
#undef SDL_LOADSO_WIN32

/* Enable various threading systems */
#undef SDL_THREAD_BEOS
#undef SDL_THREAD_DC
#undef SDL_THREAD_OS2
#undef SDL_THREAD_PTH
#define SDL_THREAD_PTHREAD 1
#undef SDL_THREAD_PTHREAD_RECURSIVE_MUTEX
#undef SDL_THREAD_PTHREAD_RECURSIVE_MUTEX_NP
#undef SDL_THREAD_SPROC
#undef SDL_THREAD_WIN32

/* Enable various timer systems */
#undef SDL_TIMER_BEOS
#undef SDL_TIMER_DC
#define SDL_TIMER_DUMMY 1
#undef SDL_TIMER_MACOS
#undef SDL_TIMER_MINT
#undef SDL_TIMER_OS2
#undef SDL_TIMER_RISCOS
#undef SDL_TIMER_UNIX
#undef SDL_TIMER_WIN32
#undef SDL_TIMER_WINCE

/* Enable various video drivers */
#undef SDL_VIDEO_DRIVER_AALIB
#undef SDL_VIDEO_DRIVER_BWINDOW
#undef SDL_VIDEO_DRIVER_CACA
#undef SDL_VIDEO_DRIVER_DC
#undef SDL_VIDEO_DRIVER_DDRAW
#undef SDL_VIDEO_DRIVER_DGA
#undef SDL_VIDEO_DRIVER_DIRECTFB
#undef SDL_VIDEO_DRIVER_DRAWSPROCKET
#undef SDL_VIDEO_DRIVER_DUMMY
#undef SDL_VIDEO_DRIVER_FBCON
#undef SDL_VIDEO_DRIVER_GAPI
#undef SDL_VIDEO_DRIVER_GEM
#undef SDL_VIDEO_DRIVER_GGI
#undef SDL_VIDEO_DRIVER_IPOD
#undef SDL_VIDEO_DRIVER_NANOX
#undef SDL_VIDEO_DRIVER_OS2FS
#undef SDL_VIDEO_DRIVER_PHOTON
#undef SDL_VIDEO_DRIVER_PICOGUI
#undef SDL_VIDEO_DRIVER_PS2GS
#undef SDL_VIDEO_DRIVER_PS3
#undef SDL_VIDEO_DRIVER_QTOPIA
#undef SDL_VIDEO_DRIVER_QUARTZ
#undef SDL_VIDEO_DRIVER_RISCOS
#undef SDL_VIDEO_DRIVER_SVGALIB
#undef SDL_VIDEO_DRIVER_TOOLBOX
#undef SDL_VIDEO_DRIVER_VGL
#undef SDL_VIDEO_DRIVER_WINDIB
#undef SDL_VIDEO_DRIVER_WSCONS
#undef SDL_VIDEO_DRIVER_X11
#undef SDL_VIDEO_DRIVER_X11_DGAMOUSE
#undef SDL_VIDEO_DRIVER_X11_DYNAMIC
#undef SDL_VIDEO_DRIVER_X11_DYNAMIC_XEXT
#undef SDL_VIDEO_DRIVER_X11_DYNAMIC_XRANDR
#undef SDL_VIDEO_DRIVER_X11_DYNAMIC_XRENDER
#undef SDL_VIDEO_DRIVER_X11_VIDMODE
#undef SDL_VIDEO_DRIVER_X11_XINERAMA
#undef SDL_VIDEO_DRIVER_X11_XME
#undef SDL_VIDEO_DRIVER_X11_XRANDR
#undef SDL_VIDEO_DRIVER_X11_XV
#undef SDL_VIDEO_DRIVER_XBIOS
#define SDL_VIDEO_DRIVER_PEDIGREE 1

/* Enable OpenGL support */
#undef SDL_VIDEO_OPENGL
#undef SDL_VIDEO_OPENGL_GLX
#undef SDL_VIDEO_OPENGL_WGL
#undef SDL_VIDEO_OPENGL_OSMESA
#undef SDL_VIDEO_OPENGL_OSMESA_DYNAMIC

/* Disable screensaver */
#define SDL_VIDEO_DISABLE_SCREENSAVER 1

/* Enable assembly routines */
#undef SDL_ASSEMBLY_ROUTINES
#undef SDL_HERMES_BLITTERS
#undef SDL_ALTIVEC_BLITTERS

#endif /* _SDL_config_h */
