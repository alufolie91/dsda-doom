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
 *-----------------------------------------------------------------------------*/

#if (R_DRAWCOLUMN_PIPELINE & RDC_TRANSLATED)
#define GETCOL_MAPPED(col) (translation[(col)])
#else
#define GETCOL_MAPPED(col) (col)
#endif

#if (R_DRAWCOLUMN_PIPELINE & RDC_NOCOLMAP)
  #define GETCOL_DEPTH(col) GETCOL_MAPPED(col)
#else
  #define GETCOL_DEPTH(col) colormap[GETCOL_MAPPED(col)]
#endif

#define GETCOL(frac) GETCOL_DEPTH(source[(frac)>>FRACBITS])

#if (R_DRAWCOLUMN_PIPELINE & RDC_TRANSLUCENT)
#define COLTYPE (COL_TRANS)
#elif (R_DRAWCOLUMN_PIPELINE & RDC_FUZZ)
#define COLTYPE (COL_FUZZ)
#else
#define COLTYPE (COL_OPAQUE)
#endif

static void R_DRAWCOLUMN_FUNCNAME(draw_column_vars_t *dcvars)
{
  intptr_t         count;

#if (!(R_DRAWCOLUMN_PIPELINE & RDC_FUZZ))
  byte* __restrict dest;            // killough
  intptr_t         frac;
  const intptr_t   fracstep = dcvars->iscale;
  byte* __restrict tempbuf = temp_dcvars.buf.get();
#endif

#if (R_DRAWCOLUMN_PIPELINE & RDC_FUZZ)
  // Adjust borders. Low...
  if (!dcvars->yl)
    dcvars->yl = 1;

  // .. and high.
  if (dcvars->yh == viewheight-1)
    dcvars->yh = viewheight - 2;
#endif

  // leban 1/17/99:
  // removed the + 1 here, adjusted the if test, and added an increment
  // later.  this helps a compiler pipeline a bit better.  the x86
  // assembler also does this.

  count = dcvars->yh - dcvars->yl;

  // leban 1/17/99:
  // this case isn't executed too often.  depending on how many instructions
  // there are between here and the second if test below, this case could
  // be moved down and might save instructions overall.  since there are
  // probably different wads that favor one way or the other, i'll leave
  // this alone for now.
  if (count < 0)    // Zero length, column does not exceed a pixel.
    return;

#ifdef RANGECHECK
  if (dcvars->x >= SCREENWIDTH
      || dcvars->yl < 0
      || dcvars->yh >= SCREENHEIGHT)
    I_Error("R_DrawColumn: %i to %i at %i", dcvars->yl, dcvars->yh, dcvars->x);
#endif

#if (!(R_DRAWCOLUMN_PIPELINE & RDC_FUZZ))
  if (dcvars->flags & DRAW_COLUMN_ISPATCH)
    frac = ((dcvars->yl - dcvars->dy) * fracstep) & 0xFFFF;
  else
    frac = dcvars->texturemid + (dcvars->yl-centery)*fracstep;
#endif

  // Framebuffer destination address.
   // SoM: MAGIC
   {
      // haleyjd: reordered predicates
      if(temp_dcvars.x == 4 ||
         (temp_dcvars.x && (temp_dcvars.type != COLTYPE || temp_dcvars.x + temp_dcvars.startx != dcvars->x)))
         R_FlushColumns();

      if(!temp_dcvars.x)
      {
         temp_dcvars.startx = dcvars->x;
         temp_dcvars.yl[0] = temp_dcvars.commontop = dcvars->yl;
         temp_dcvars.yh[0] = temp_dcvars.commonbot = dcvars->yh;
         temp_dcvars.type = COLTYPE;
#if (R_DRAWCOLUMN_PIPELINE & RDC_TRANSLUCENT)
         temp_dcvars.tranmap = tranmap;
#elif (R_DRAWCOLUMN_PIPELINE & RDC_FUZZ)
         temp_dcvars.fuzzmap = fullcolormap; // SoM 7-28-04: Fix the fuzz problem.
#endif
         R_FlushWholeColumns = R_FLUSHWHOLE_FUNCNAME;
         R_FlushHTColumns    = R_FLUSHHEADTAIL_FUNCNAME;
         R_FlushQuadColumn   = R_FLUSHQUAD_FUNCNAME;
#if (!(R_DRAWCOLUMN_PIPELINE & RDC_FUZZ))
         dest = &tempbuf[dcvars->yl << 2];
#endif
      } else {
         temp_dcvars.yl[temp_dcvars.x] = dcvars->yl;
         temp_dcvars.yh[temp_dcvars.x] = dcvars->yh;

         if(dcvars->yl > temp_dcvars.commontop)
            temp_dcvars.commontop = dcvars->yl;
         if(dcvars->yh < temp_dcvars.commonbot)
            temp_dcvars.commonbot = dcvars->yh;
#if (!(R_DRAWCOLUMN_PIPELINE & RDC_FUZZ))
         dest = &tempbuf[(dcvars->yl << 2) + temp_dcvars.x];
#endif
      }
      temp_dcvars.x += 1;
   }

// do nothing else when drawin fuzz columns
#if (!(R_DRAWCOLUMN_PIPELINE & RDC_FUZZ))
  {
    const byte          * __restrict source = dcvars->source;

#if (!(R_DRAWCOLUMN_PIPELINE & RDC_NOCOLMAP))
    const lighttable_t  * __restrict colormap = dcvars->colormap;
#endif

#if (R_DRAWCOLUMN_PIPELINE & RDC_TRANSLATED)
    const byte          * __restrict translation = dcvars->translation;
#endif

    count++;

    // Inner loop that does the actual texture mapping,
    //  e.g. a DDA-lile scaling.
    // This is as fast as it gets.       (Yeah, right!!! -- killough)
    //
    // killough 2/1/98: more performance tuning

    if (dcvars->texheight == 128) {
      #define FIXEDT_128MASK ((127<<FRACBITS)|0xffff)
      while(count--) {
        *dest = GETCOL(frac & FIXEDT_128MASK);
        dest += 4;
        frac += fracstep;
      }
    } else if (dcvars->texheight == 0) {
      /* cph - another special case */
      while (count--) {
        *dest = GETCOL(frac);
        dest += 4;
        frac += fracstep;
      }
    } else {
      uintptr_t heightmask = dcvars->texheight-1; // CPhipps - specify type
      if (! (dcvars->texheight & heightmask) ) { // power of 2 -- killough
        intptr_t fixedt_heightmask = (heightmask<<FRACBITS)|0xffff;
        while ((count-=2)>=0) { // texture height is a power of 2 -- killough
          *dest = GETCOL(frac & fixedt_heightmask);
          dest += 4;
          frac += fracstep;
          *dest = GETCOL(frac & fixedt_heightmask);
          dest += 4;
          frac += fracstep;
        }
        if (count & 1)
          *dest = GETCOL(frac & fixedt_heightmask);
      } else {
        heightmask++;
        heightmask <<= FRACBITS;

        if (frac < 0)
          while ((frac += heightmask) <  0);
        else
          while (frac >= (intptr_t)heightmask)
            frac -= heightmask;

        while (count--) {
          // Re-map color indices from wall texture column
          //  using a lighting/special effects LUT.

          // heightmask is the Tutti-Frutti fix -- killough

          *dest = GETCOL(frac);
          dest += 4;
          if ((frac += fracstep) >= (intptr_t)heightmask)
            frac -= heightmask;
        }
      }
    }
  }
#endif // (!(R_DRAWCOLUMN_PIPELINE & RDC_FUZZ))
}

#undef GETCOL_MAPPED
#undef GETCOL_DEPTH
#undef GETCOL
#undef COLTYPE
#undef R_DRAWCOLUMN_FUNCNAME
#undef R_DRAWCOLUMN_PIPELINE
