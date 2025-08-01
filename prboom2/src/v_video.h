/* Emacs style mode select   -*- C -*-
 *-----------------------------------------------------------------------------
 *
 *
 *  PrBoom: a Doom port merged with LxDoom and LSDLDoom
 *  based on BOOM, a modified and improved DOOM engine
 *  Copyright (C) 1999 by
 *  id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman
 *  Copyright (C) 1999-2000 by
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
 *  Gamma correction LUT.
 *  Color range translation support
 *  Functions to draw patches (by post) directly to screen.
 *  Functions to blit a block to the screen.
 *
 *-----------------------------------------------------------------------------*/

#ifndef __V_VIDEO__
#define __V_VIDEO__

#ifdef __cplusplus
extern "C" {
#endif

#include "SDL.h"

#include "doomtype.h"
#include "doomdef.h"
// Needed because we are refering to patches.
#include "r_data.h"

//
// VIDEO
//

// DWF 2012-05-10
// SetRatio sets the following global variables based on window geometry and
// user preferences. The integer ratio is hardly used anymore, so further
// simplification may be in order.
void SetRatio(int width, int height);
extern dboolean tallscreen;
extern unsigned int ratio_multiplier, ratio_scale;
extern float gl_ratio;
extern int psprite_offset; // Needed for "tallscreen" modes

#define CENTERY     (SCREENHEIGHT/2)

// Screen 0 is the screen updated by I_Update screen.
// Screen 1 is an extra buffer.

// array of pointers to color translation tables
extern const byte *colrngs[];

// symbolic indices into color translation table pointer array
typedef enum
{
  CR_DEFAULT,
  CR_BRICK,
  CR_TAN,
  CR_GRAY,
  CR_GREEN,
  CR_BROWN,
  CR_GOLD,
  CR_RED,
  CR_BLUE,
  CR_ORANGE,
  CR_YELLOW,
  CR_LIGHTBLUE,
  CR_BLACK,
  CR_PURPLE,
  CR_WHITE,
  CR_HUD_LIMIT,
  CR_DARKEN = CR_HUD_LIMIT,
  CR_DARKEN_BRICK,
  CR_DARKEN_TAN,
  CR_DARKEN_GRAY,
  CR_DARKEN_GREEN,
  CR_DARKEN_BROWN,
  CR_DARKEN_GOLD,
  CR_DARKEN_RED,
  CR_DARKEN_BLUE,
  CR_DARKEN_ORANGE,
  CR_DARKEN_YELLOW,
  CR_DARKEN_LIGHTBLUE,
  CR_DARKEN_BLACK,
  CR_DARKEN_PURPLE,
  CR_DARKEN_WHITE,
  CR_BLOOD,
  CR_BLOOD_GRAY = CR_BLOOD,
  CR_BLOOD_GREEN,
  CR_BLOOD_BLUE,
  CR_BLOOD_YELLOW,
  CR_BLOOD_BLACK,
  CR_BLOOD_PURPLE,
  CR_BLOOD_WHITE,
  CR_BLOOD_ORANGE,
  CR_LIMIT,
} crange_idx_e;
//jff 1/16/98 end palette color range additions

typedef struct {
  byte *data;          // pointer to the screen content
  dboolean not_on_heap; // if set, no malloc or free is preformed and
                       // data never set to NULL. Used i.e. with SDL doublebuffer.
  int width;           // the width of the surface
  int height;          // the height of the surface, used when mallocing
  int pitch;      // tha actual width of one line, used when mallocing
} screeninfo_t;

#define NUM_SCREENS 6
extern screeninfo_t screens[NUM_SCREENS];
extern int          usegamma;

// Varying bit-depth support -POPE
//
// For bilinear filtering, each palette color is pre-weighted and put in a
// table for fast blending operations. These macros decide how many weights
// to create for each color. The lower the number, the lower the blend
// accuracy, which can produce very bad artifacts in texture filtering.
#define VID_NUMCOLORWEIGHTS 64
#define VID_COLORWEIGHTMASK (VID_NUMCOLORWEIGHTS-1)
#define VID_COLORWEIGHTBITS 6

// [XA] size of a single palette within PLAYPAL:
//      256 colors * 3 bytes per color = 768
#define PALETTE_SIZE 768

// [XA] wonder why this was never a constant... silly id. :P
#define NUM_GAMMA_LEVELS 5

// The available bit-depth modes
typedef enum {
  VID_MODESW,
  VID_MODEGL,
  VID_MODEMAX
} video_mode_t;

extern video_mode_t current_videomode;

void V_InitMode(video_mode_t mode);

// video mode query interface
INLINE static dboolean V_IsSoftwareMode(void) {
  return current_videomode == VID_MODESW;
}

INLINE static dboolean V_IsOpenGLMode(void) {
  return current_videomode == VID_MODEGL;
}

// [XA] indexed lightmode query interface
dboolean V_IsUILightmodeIndexed(void);
dboolean V_IsAutomapLightmodeIndexed(void);
dboolean V_IsMenuLightmodeIndexed(void);

//jff 4/24/98 loads color translation lumps
void V_InitColorTranslation(void);

void V_InitFlexTranTable(void);

// Allocates buffer screens, call before R_Init.
void V_Init (void);

// V_BeginUIDraw
typedef void(*V_BeginUIDraw_f)(void);
extern V_BeginUIDraw_f V_BeginUIDraw;

// V_EndUIDraw
typedef void(*V_EndUIDraw_f)(void);
extern V_EndUIDraw_f V_EndUIDraw;

// V_BeginAutomapDraw
typedef void(*V_BeginAutomapDraw_f)(void);
extern V_BeginAutomapDraw_f V_BeginAutomapDraw;

// V_EndAutomapDraw
typedef void(*V_EndAutomapDraw_f)(void);
extern V_EndAutomapDraw_f V_EndAutomapDraw;

// V_BeginMenuDraw
typedef void(*V_BeginMenuDraw_f)(void);
extern V_BeginMenuDraw_f V_BeginMenuDraw;

// V_EndMenuDraw
typedef void(*V_EndMenuDraw_f)(void);
extern V_EndMenuDraw_f V_EndMenuDraw;

// V_CopyRect
typedef void (*V_CopyRect_f)(int srcscrn, int destscrn,
                             int x, int y,
                             int width, int height,
                             enum patch_translation_e flags);
extern V_CopyRect_f V_CopyRect;

void V_CopyScreen(int srcscrn, int destscrn);

// V_FillRect
typedef void (*V_FillRect_f)(int scrn, int x, int y,
                             int width, int height, byte colour);
extern V_FillRect_f V_FillRect;

// CPhipps - patch drawing
// Consolidated into the 3 really useful functions:

// V_DrawNumPatchGen - Draws the patch from lump num
typedef void (*V_DrawNumPatchGen_f)(int x, int y, int scrn,
                                 int lump, dboolean center, int cm,
                                 enum patch_translation_e flags);
extern V_DrawNumPatchGen_f V_DrawNumPatchGen;

typedef void (*V_DrawNumPatchGenPrecise_f)(float x, float y, int scrn,
                                 int lump, dboolean center, int cm,
                                 enum patch_translation_e flags);
extern V_DrawNumPatchGenPrecise_f V_DrawNumPatchGenPrecise;

// V_DrawNumPatch - Draws the patch from lump "num"
#define V_DrawNumPatch(x,y,s,n,t,f) V_DrawNumPatchGen(x,y,s,n,false,t,f)
#define V_DrawNumPatchPrecise(x,y,s,n,t,f) V_DrawNumPatchGenPrecise(x,y,s,n,false,t,f)

// V_DrawNamePatch - Draws the patch from lump "name"
#define V_DrawNamePatch(x,y,s,n,t,f) V_DrawNumPatchGen(x,y,s,W_GetNumForName(n),false,t,f)
#define V_DrawNamePatchPrecise(x,y,s,n,t,f) V_DrawNumPatchGenPrecise(x,y,s,W_GetNumForName(n),false,t,f)

// These functions center patches if width > 320 :
#define V_DrawNumPatchFS(x,y,s,n,t,f) V_DrawNumPatchGen(x,y,s,n,true,t,f)
#define V_DrawNumPatchPreciseFS(x,y,s,n,t,f) V_DrawNumPatchGenPrecise(x,y,s,n,true,t,f)
#define V_DrawNamePatchFS(x,y,s,n,t,f) V_DrawNumPatchGen(x,y,s,W_GetNumForName(n),true,t,f)
#define V_DrawNamePatchPreciseFS(x,y,s,n,t,f) V_DrawNumPatchGenPrecise(x,y,s,W_GetNumForName(n),true,t,f)

/* cph -
 * Functions to return width & height of a patch.
 * Doesn't really belong here, but is often used in conjunction with
 * this code.
 */
#define V_NamePatchWidth(name) R_NumPatchWidth(W_GetNumForName(name))
#define V_NamePatchHeight(name) R_NumPatchHeight(W_GetNumForName(name))

// e6y
typedef void (*V_FillFlat_f)(int lump, int scrn, int x, int y, int width, int height, enum patch_translation_e flags);
extern V_FillFlat_f V_FillFlat;
#define V_FillFlatName(flatname, scrn, x, y, width, height, flags) \
  V_FillFlat(R_FlatNumForName(flatname), (scrn), (x), (y), (width), (height), (flags))

typedef void (*V_FillPatch_f)(int lump, int scrn, int x, int y, int width, int height, enum patch_translation_e flags);
extern V_FillPatch_f V_FillPatch;
#define V_FillPatchName(name, scrn, x, y, width, height, flags) \
  V_FillPatch(W_GetNumForName(name), (scrn), (x), (y), (width), (height), (flags))


/* cphipps 10/99: function to tile a flat over the screen */
typedef void (*V_DrawBackground_f)(const char* flatname, int scrn);
extern V_DrawBackground_f V_DrawBackground;

typedef void (*V_DrawShaded_f)(int scrn, int x, int y, int width, int height, int shade);
extern V_DrawShaded_f V_DrawShaded;

// CPhipps - function to set the palette to palette number pal.
void V_TouchPalette(void);
void V_SetPalette(int pal);
void V_SetPlayPal(int playpal_index);

// Alt-Enter: fullscreen <-> windowed
void V_ToggleFullscreen(void);
void V_ChangeScreenResolution(void);

// CPhipps - function to plot a pixel

// V_PlotPixel
typedef void (*V_PlotPixel_f)(int,int,int,byte);
extern V_PlotPixel_f V_PlotPixel;

typedef struct
{
  int x, y;
  float fx, fy;
} fpoint_t;

typedef struct
{
  fpoint_t a, b;
} fline_t;

// V_DrawLine
typedef void (*V_DrawLine_f)(fline_t* fl, int color);
extern V_DrawLine_f V_DrawLine;

// V_DrawLineWu
typedef void (*V_DrawLineWu_f)(fline_t* fl, int color);
extern V_DrawLineWu_f V_DrawLineWu;

// V_PlotPixelWu
typedef void (*V_PlotPixelWu_f)(int scrn, int x, int y, byte color, int weight);
extern V_PlotPixelWu_f V_PlotPixelWu;

void V_AllocScreen(screeninfo_t *scrn);
void V_AllocScreens();
void V_FreeScreen(screeninfo_t *scrn);
void V_FreeScreens();

const unsigned char* V_GetPlaypal(void);
void V_FreePlaypal(void);

// [XA] get number of palettes in the current playpal
int V_GetPlaypalCount(void);

SDL_Color V_GetPatchColor (int lumpnum);

// e6y: wide-res
void V_ClearBorder(void);

void V_GetWideRect(int *x, int *y, int *w, int *h, enum patch_translation_e flags);

int V_BestColor(const unsigned char *palette, int r, int g, int b);

// [FG] colored blood and gibs
int V_BloodColor(int blood);

#include "gl_struct.h"

void V_FillRectVPT(int scrn, int x, int y, int width, int height, byte color, enum patch_translation_e flags);
int V_FillHeightVPT(int scrn, int y, int height, byte color, enum patch_translation_e flags);

// heretic

void V_DrawRawScreen(const char *lump_name);
void V_DrawRawScreenSection(const char *lump_name, int source_offset, int dest_y_offset, int dest_y_limit);
void V_DrawShadowedNumPatch(int x, int y, int lump);
void V_DrawShadowedNamePatch(int x, int y, const char* name);
void V_DrawTLNumPatch(int x, int y, int lump);
void V_DrawTLNamePatch(int x, int y, const char* name);
void V_DrawAltTLNumPatch(int x, int y, int lump);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
