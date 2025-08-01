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
 *      Refresh module, drawing LineSegs from BSP.
 *
 *-----------------------------------------------------------------------------*/

#ifndef __R_SEGS__
#define __R_SEGS__

#ifdef __cplusplus
extern "C" {
#endif

void R_RenderMaskedSegRange(drawseg_t *ds, int x1, int x2);
void R_StoreWallRange(const int start, const int stop);

int R_TopLightLevel(side_t *side, int base_lightlevel);
int R_MidLightLevel(side_t *side, int base_lightlevel);
int R_BottomLightLevel(side_t *side, int base_lightlevel);
void R_AddContrast(seg_t *seg, int *base_lightlevel);

typedef enum
{
  FAKE_CONTRAST_MODE_OFF,
  FAKE_CONTRAST_MODE_ON,
  FAKE_CONTRAST_MODE_SMOOTH
} fake_contrast_mode_t;

extern fake_contrast_mode_t fake_contrast_mode;

#ifdef __cplusplus
} // extern "C"
#endif

#endif
