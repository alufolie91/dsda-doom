/* Emacs style mode select   -*- C -*-
 *-----------------------------------------------------------------------------
 *
 *
 *  PrBoom: a Doom port merged with LxDoom and LSDLDoom
 *  based on BOOM, a modified and improved DOOM engine
 *  Copyright (C) 1999 by
 *  id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman
 *  Copyright (C) 1999-2000 by
 *  Jess Haas, Nicolas Kalkhof, Colin Phipps, Florian Schulze, Andrey Budko
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
 *      Uncapped framerate stuff
 *
 *---------------------------------------------------------------------
 */

#ifndef __R_FPS__
#define __R_FPS__

#ifdef __cplusplus
extern "C" {
#endif

#include "doomstat.h"

extern int movement_smooth;
extern dboolean isExtraDDisplay;

typedef struct {
  fixed_t frac;
} tic_vars_t;

extern tic_vars_t tic_vars;

void M_ChangeUncappedFrameRate(void);

dboolean R_ViewInterpolation(void);
void R_InterpolateView(player_t *player, fixed_t frac);

extern dboolean WasRenderedInTryRunTics;

void R_ResetViewInterpolation ();
void R_UpdateInterpolations();
void R_StopAllInterpolations(void);
void R_RestoreInterpolations();
void R_ActivateSectorInterpolations();
void R_ActivateThinkerInterpolations(thinker_t *th);
void R_StopInterpolationIfNeeded(thinker_t *th);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
