/* Emacs style mode select   -*- C -*-
 *-----------------------------------------------------------------------------
 *
 *
 *  PrBoom: a Doom port merged with LxDoom and LSDLDoom
 *  based on BOOM, a modified and improved DOOM engine
 *  Copyright (C) 1999 by
 *  id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman
 *  Copyright (C) 1999-2006 by
 *  Jess Haas, Nicolas Kalkhof, Colin Phipps, Florian Schulze
 *  Copyright 2005, 2006 by
 *  Florian Schulze, Colin Phipps, Neil Stevens, Andrey Budko
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 *  02111-1307, USA.
 *
 * DESCRIPTION:
 *      Simple basic typedefs, isolated here to make it easier
 *       separating modules.
 *
 *-----------------------------------------------------------------------------*/

#ifndef __DOOMTYPE__
#define __DOOMTYPE__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdbool.h>
typedef int dboolean;

typedef unsigned char byte;

//e6y
#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif
#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef BETWEEN
#define BETWEEN(l,u,x) ((l)>(x)?(l):(x)>(u)?(u):(x))
#endif

#define LIKELY(x)    __builtin_expect(!!(x), 1)
#define UNLIKELY(x)  __builtin_expect(!!(x), 0)

#include <inttypes.h>
#include <limits.h>

#ifdef _MSC_VER
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif

#ifndef PATH_MAX
  #ifdef MAX_PATH
    #define PATH_MAX MAX_PATH
  #else
    #define PATH_MAX 1024
  #endif
#endif

#ifdef __GNUC__
#define CONSTFUNC __attribute__((const))
#define PUREFUNC __attribute__((pure))
#define NORETURN __attribute__ ((noreturn))
#else
#define CONSTFUNC
#define PUREFUNC
#define NORETURN
#endif

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
  #define NORETURNC11 _Noreturn
#else
  #define NORETURNC11
#endif

// Definition of PACKEDATTR from Chocolate Doom
#ifdef __GNUC__
  #if defined(_WIN32) && !defined(__clang__)
    #define PACKEDATTR __attribute__((packed,gcc_struct))
  #else
    #define PACKEDATTR __attribute__((packed))
  #endif
#else
  #define PACKEDATTR
#endif

#ifdef WIN32
#define C_DECL __cdecl
#else
#define C_DECL
#endif

#ifdef _MSC_VER
  #define INLINE __forceinline /* use __forceinline (VC++ specific) */
#else
  #define INLINE inline        /* use standard inline */
#endif

typedef enum {
  doom_12_compatibility,                 /* Doom v1.2 */
  doom_1666_compatibility,               /* Doom v1.666 */
  doom2_19_compatibility,                /* Doom & Doom 2 v1.9 */
  ultdoom_compatibility,                 /* Ultimate Doom and Doom95 */
  finaldoom_compatibility,               /* Final Doom */
  dosdoom_compatibility,                 /* DosDoom 0.47 */
  tasdoom_compatibility,                 /* TASDoom */
  boom_compatibility_compatibility,      /* Boom's compatibility mode */
  boom_201_compatibility,                /* Boom v2.01 */
  boom_202_compatibility,                /* Boom v2.02 */
  lxdoom_1_compatibility,                /* LxDoom v1.3.2+ */
  mbf_compatibility,                     /* MBF */
  prboom_1_compatibility,                /* PrBoom 2.03beta? */
  prboom_2_compatibility,                /* PrBoom 2.1.0-2.1.1 */
  prboom_3_compatibility,                /* PrBoom 2.2.x */
  prboom_4_compatibility,                /* PrBoom 2.3.x */
  prboom_5_compatibility,                /* PrBoom 2.4.0 */
  prboom_6_compatibility,                /* Latest PrBoom */
  placeholder_18_compatibility,
  placeholder_19_compatibility,
  placeholder_20_compatibility,
  mbf21_compatibility,                   /* MBF21 */
  MAX_COMPATIBILITY_LEVEL,               /* Must be last entry */
  /* Aliases follow */
  boom_compatibility = boom_201_compatibility, /* Alias used by G_Compatibility */
  best_compatibility = mbf21_compatibility
} complevel_t_e;
typedef int complevel_t;

/* cph - from v_video.h, needed by gl_struct.h */
#define VPT_ALIGN_MASK 0xf
#define VPT_STRETCH_MASK 0x1f
enum patch_translation_e {
  // e6y: wide-res
  VPT_ALIGN_LEFT         = 1,
  VPT_ALIGN_RIGHT        = 2,
  VPT_ALIGN_TOP          = 3,
  VPT_ALIGN_LEFT_TOP     = 4,
  VPT_ALIGN_RIGHT_TOP    = 5,
  VPT_ALIGN_BOTTOM       = 6,
  VPT_ALIGN_WIDE         = 7,
  VPT_ALIGN_LEFT_BOTTOM  = 8,
  VPT_ALIGN_RIGHT_BOTTOM = 9,
  VPT_ALIGN_MAX          = 10,
  VPT_STRETCH            = 16, // Stretch to compensate for high-res
  VPT_EX_TEXT            = 32,

  VPT_NONE    = 128, // Normal
  VPT_FLIP    = 256, // Flip image horizontally
  VPT_TRANS   = 512, // Translate image via a translation table
  VPT_NOOFFSET = 1024,
  VPT_STRETCH_REAL       = 2048, // [XA] VPT_STRETCH in gld_fillRect means "tile", rather than "stretch"... these flags probably need a rename.
};

extern int global_patch_top_offset;

#define BOTTOM_ALIGNMENT(x) ((x) == VPT_ALIGN_BOTTOM || \
                             (x) == VPT_ALIGN_LEFT_BOTTOM || \
                             (x) == VPT_ALIGN_RIGHT_BOTTOM)

#define TOP_ALIGNMENT(x) ((x) == VPT_ALIGN_TOP || \
                          (x) == VPT_ALIGN_LEFT_TOP || \
                          (x) == VPT_ALIGN_RIGHT_TOP)

#define arrlen(array) (sizeof(array) / sizeof(*array))

#endif
