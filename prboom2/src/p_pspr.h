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
 *  Sprite animation.
 *
 *-----------------------------------------------------------------------------*/

#ifndef __P_PSPR__
#define __P_PSPR__

#ifdef __cplusplus
extern "C" {
#endif

/* Basic data types.
 * Needs fixed point, and BAM angles. */

#include "m_fixed.h"
#include "tables.h"

/* Needs to include the precompiled sprite animation tables.
 *
 * Header generated by multigen utility.
 * This includes all the data for thing animation,
 * i.e. the Thing Atrributes table and the Frame Sequence table.
 */

#include "info.h"

/*
 * Frame flags:
 * handles maximum brightness (torches, muzzle flare, light sources)
 */

#define FF_FULLBRIGHT   0x8000  /* flag in thing->frame */
#define FF_FRAMEMASK    0x7fff

/*
 * Overlay psprites are scaled shapes
 * drawn directly on the view screen,
 * coordinates are given for a 320*200 view screen.
 */

typedef enum
{
  ps_weapon,
  ps_flash,
  NUMPSPRITES
} psprnum_t;

typedef struct
{
  state_t *state;       /* a NULL state means not active */
  int     tics;
  fixed_t sx;
  fixed_t sy;
} pspdef_t;

enum
{
    CENTERWEAPON_OFF,
    CENTERWEAPON_HOR,
    CENTERWEAPON_HORVER,
    CENTERWEAPON_BOB,
    NUM_CENTERWEAPON,
};

int P_WeaponPreferred(int w1, int w2);

struct player_s;
int P_SwitchWeapon(struct player_s *player);
dboolean P_CheckAmmo(struct player_s *player);
void P_SubtractAmmo(struct player_s *player, int compat_amt);
void P_SetupPsprites(struct player_s *curplayer);
void P_MovePsprites(struct player_s *curplayer);
void P_DropWeapon(struct player_s *player);
int P_AmmoPercent(struct player_s *player, int weapon);

void A_Light0();
void A_WeaponReady();
void A_Lower();
void A_Raise();
void A_Punch();
void A_ReFire();
void A_FirePistol();
void A_Light1();
void A_FireShotgun();
void A_Light2();
void A_FireShotgun2();
void A_CheckReload();
void A_OpenShotgun2();
void A_LoadShotgun2();
void A_CloseShotgun2();
void A_FireCGun();
void A_GunFlash();
void A_FireMissile();
void A_Saw();
void A_FirePlasma();
void A_BFGsound();
void A_FireBFG();
void A_BFGSpray();
void A_FireOldBFG();

// [XA] New mbf21 codepointers

void A_WeaponProjectile();
void A_WeaponBulletAttack();
void A_WeaponMeleeAttack();
void A_WeaponSound();
void A_WeaponAlert();
void A_WeaponJump();
void A_ConsumeAmmo();
void A_CheckAmmo();
void A_RefireTo();
void A_GunFlashTo();

// heretic

#include "p_mobj.h"

struct player_s;

void P_RepositionMace(mobj_t * mo);
void P_ActivateBeak(struct player_s * player);
void P_PostChickenWeapon(struct player_s * player, weapontype_t weapon);
void P_SetPsprite(struct player_s * player, int position, statenum_t stnum);
void P_SetPspritePtr(struct player_s * player, pspdef_t *psp, statenum_t stnum);
void P_OpenWeapons(void);
void P_CloseWeapons(void);
void P_AddMaceSpot(const mapthing_t * mthing);
void P_DropWeapon(struct player_s * player);
void P_UpdateBeak(struct player_s * player, pspdef_t * psp);

// hexen

void P_SetPspriteNF(struct player_s * player, int position, statenum_t stnum);
void P_PostMorphWeapon(struct player_s * player, weapontype_t weapon);
void P_ActivateMorphWeapon(struct player_s * player);

#ifdef __cplusplus
} // extern "C"
#endif
#endif
