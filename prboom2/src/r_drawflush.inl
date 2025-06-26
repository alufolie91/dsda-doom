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

#if (R_DRAWCOLUMN_PIPELINE & RDC_TRANSLUCENT)
#define GETDESTCOLOR(col1, col2) (temp_dcvars.tranmap[((col1)<<8)+(col2)])
#else
#define GETDESTCOLOR(col) (col)
#endif

//
// R_FlushWholeOpaque
//
// Flushes the entire columns in the buffer, one at a time.
// This is used when a quad flush isn't possible.
// Opaque version -- no remapping whatsoever.
//
static void R_FLUSHWHOLE_FUNCNAME(void)
{
    const __restrict intptr_t stride = drawvars.pitch;

   // Scaled software fuzz algorithm
#if (R_DRAWCOLUMN_PIPELINE & RDC_FUZZ)
{
    intptr_t yl, yh, count, lines;
    byte * __restrict dest;

    if ((temp_dcvars.x + temp_dcvars.startx) % fuzzcellsize)
    {
        return;
    }

    yl = temp_dcvars.yl[temp_dcvars.x - 1];
    yh = temp_dcvars.yh[temp_dcvars.x - 1];

    count = yh - yl + 1;

    if (count < 0)
    {
        return;
    }

#ifdef RANGECHECK
    if ((unsigned)x >= video.width || yl < 0 || yh  >= video.height)
    {
        I_Error("R_DrawFuzzColumn: %i to %i at %i", yl, yh , x);
    }
#endif

    ++count;

    dest = drawvars.topleft + yl * stride + temp_dcvars.startx + temp_dcvars.x - fuzzcellsize;

    lines = fuzzcellsize - (yl % fuzzcellsize);

    do
    {
        intptr_t mask;
        byte fuzz;

        count -= lines;

        // if (count < 0)
        // {
        //    lines += count;
        //    count = 0;
        // }
        mask = count >> (8 * sizeof(mask) - 1);
        lines += count & mask;
        count &= ~mask;

        fuzz = fullcolormap[6 * 256 + dest[fuzzoffset[fuzzpos]]];

        do
        {
            memset(dest, fuzz, fuzzcellsize);
            dest += stride;
        } while (--lines);

        ++fuzzpos;

        // Clamp table lookup index.
        fuzzpos &= (fuzzpos - FUZZTABLE) >> (8 * sizeof(fuzzpos) - 1); // killough 1/99

        lines = fuzzcellsize;
    } while (count);
}
#else
{
   byte* __restrict source;
   byte* __restrict dest;
   intptr_t  count, yl;
   byte* __restrict tempbuf = dcvars->buf.get();

   while(--temp_dcvars.x >= 0)
   {
      yl     = temp_dcvars.yl[temp_dcvars.x];
      source = &tempbuf[dcvars->x + (yl << 2)];
      dest   = drawvars.topleft + yl*stride + temp_dcvars.startx + temp_dcvars.x;
      count  = temp_dcvars.yh[temp_dcvars.x] - yl + 1;

      while(--count >= 0)
      {
#if (R_DRAWCOLUMN_PIPELINE & RDC_TRANSLUCENT)
         *dest = GETDESTCOLOR(*dest, *source);
#else
         *dest = *source;
#endif

         source += 4;
         dest += stride;
      }
   }
}
#endif
}

//
// R_FlushHTOpaque
//
// Flushes the head and tail of columns in the buffer in
// preparation for a quad flush.
// Opaque version -- no remapping whatsoever.
//
static void R_FLUSHHEADTAIL_FUNCNAME(void)
{
    byte* __restrict source;
    byte* __restrict dest;
    intptr_t count, colnum = 0;
    intptr_t yl, yh;

   #if (R_DRAWCOLUMN_PIPELINE & RDC_FUZZ)
      // Only whole flushes are supported for fuzz
      R_FLUSHWHOLE_FUNCNAME();
      return;
   #endif

   const __restrict intptr_t stride = drawvars.pitch;
   byte* __restrict tempbuf = dcvars->buf.get();

   while(colnum < 4)
   {
      yl = temp_dcvars.yl[colnum];
      yh = temp_dcvars.yh[colnum];

      // flush column head
      if(yl < temp_dcvars.commontop)
      {
         source = &tempbuf[colnum + (yl << 2)];
         dest   = drawvars.topleft + yl*stride + temp_dcvars.startx + colnum;
         count  = temp_dcvars.commontop - yl;

         while(--count >= 0)
         {
#if (R_DRAWCOLUMN_PIPELINE & RDC_TRANSLUCENT)
            // haleyjd 09/11/04: use temptranmap here
            *dest = GETDESTCOLOR(*dest, *source);
#else
            *dest = *source;
#endif

            source += 4;
            dest += stride;
         }
      }

      // flush column tail
      if(yh > temp_dcvars.commonbot)
      {
         source = &temp_dcvars.buf[colnum + ((temp_dcvars.commonbot + 1) << 2)];
         dest   = drawvars.topleft + (temp_dcvars.commonbot + 1)*stride + temp_dcvars.startx + colnum;
         count  = yh - temp_dcvars.commonbot;

         while(--count >= 0)
         {
#if (R_DRAWCOLUMN_PIPELINE & RDC_TRANSLUCENT)
            // haleyjd 09/11/04: use temptranmap here
            *dest = GETDESTCOLOR(*dest, *source);
#else
            *dest = *source;
#endif

            source += 4;
            dest += stride;
         }
      }
      ++colnum;
   }
}

static void R_FLUSHQUAD_FUNCNAME(void)
{
    byte* __restrict source;
    byte* __restrict dest;
    intptr_t count;

   #if (R_DRAWCOLUMN_PIPELINE & RDC_FUZZ)
      // Only whole flushes are supported for fuzz
      return;
   #endif

   const __restrict intptr_t stride = drawvars.pitch;
   byte* __restrict tempbuf = dcvars->buf.get();

   source = &tempbuf[dcvars->commontop << 2];
   dest = drawvars.topleft + temp_dcvars.commontop*stride + temp_dcvars.startx;
   count = temp_dcvars.commonbot - temp_dcvars.commontop + 1;

#if (R_DRAWCOLUMN_PIPELINE & RDC_TRANSLUCENT)
   while(--count >= 0)
   {
      dest[0] = GETDESTCOLOR(dest[0], source[0]);
      dest[1] = GETDESTCOLOR(dest[1], source[1]);
      dest[2] = GETDESTCOLOR(dest[2], source[2]);
      dest[3] = GETDESTCOLOR(dest[3], source[3]);
      source += 4    * sizeof(byte);
      dest += stride * sizeof(byte);
   }
#else
   if ((sizeof(int) == 4) && (((intptr_t)source % 4) == 0) && (((intptr_t)dest % 4) == 0)) {
      while(--count >= 0)
      {
         *(int *)dest =   *(int *)source;
         source += 4      * sizeof(byte);
         dest   += stride * sizeof(byte);
      }
   } else {
      while(--count >= 0)
      {
         dest[0] = source[0];
         dest[1] = source[1];
         dest[2] = source[2];
         dest[3] = source[3];
         source += 4      * sizeof(byte);
         dest   += stride * sizeof(byte);
      }
   }
#endif
}

#undef GETDESTCOLOR
#undef R_DRAWCOLUMN_PIPELINE
#undef R_FLUSHWHOLE_FUNCNAME
#undef R_FLUSHHEADTAIL_FUNCNAME
#undef R_FLUSHQUAD_FUNCNAME
