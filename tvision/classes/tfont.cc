/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
/* Copyright (C) 1997 Salvador Eduardo Tropea */

/**[txh]********************************************************************

  Class: TFont
  Comments:
  It provides a place to concentrate all the PC fonts stuff. I did it
virtual because I want to override the behavior in the editor.@p
@p
SET

***************************************************************************/

#ifdef __DJGPP__

#define Uses_TScreen
#include <tv.h>

#include <dpmi.h>
#include <sys/farptr.h>
#include <stdlib.h>
#include <errno.h>
#include <go32.h>

static int load_8x10_font(int bank, int modeRecalculate);
static void SetRomFonts(int AL, int bank, int modeRecalculate);
int TFont::UseDefaultFontsNextTime=0;

/**[txh]********************************************************************

  Description:
  Selects a font of the desired size. Currently only 8x8, 8x10, 8x14 and
8x16 are supported but I did it in this way because nobody knows about the
future.@p
  If noForce is != 0 then the routine doesn't set the fonts. This option
must be used when you know that BIOS already loaded the fonts. Using it
avoids an extra load. The derived classes takes the desition according to
the selected font, so if the user selected a font the load is forced.@p
  If modeRecalculate is 1 the call to set the BIOS font is made using the
bit 4 on, that means the BIOS will recalculate some things like the number
of rows and cols in the screen. That's avoided when an external program
sets the mode because a recalculation can fail.@p

  Return:
  non-zero if fails, zero if all ok.

***************************************************************************/

int TFont::SelectFont(int height, int width, int bank, int noForce, int modeRecalculate)
{
 UseDefaultFontsNextTime=0;
#if 1
 if (noForce)
    return 0;
#endif
 if (width!=8)
    return 1;
 switch (height)
   {
    case 8:
         SetRomFonts(0x12,bank,modeRecalculate);
         break;
    case 10:
         if (load_8x10_font(bank,modeRecalculate))
            return 1;
         break;
    case 14:
         SetRomFonts(0x11,bank,modeRecalculate);
         break;
    case 16:
         SetRomFonts(0x14,bank,modeRecalculate);
         break;
    default:
         return 1;
   }
 return 0;
}

// 0x11 => 8x14
// 0x12 => 8x8
// 0x14 => 8x16
static
void SetRomFonts(int AL, int bank, int modeRecalculate)
{
 __dpmi_regs regs;

 if (!modeRecalculate)
    AL&=0xF;
 regs.h.bl = bank;
 regs.h.ah = 0x11;
 regs.h.al = AL;
 __dpmi_int(0x10, &regs);
}

static int font_seg = -1;           /* segment of DOS buffer for 8x10 font */

/* Stretch a 8x8 font to the 8x10 character box.  This is required to
   use 80x40 mode on a VGA or 80x35 mode on an EGA, because the character
   box is 10 lines high, and the ROM BIOS doesn't have an appropriate font.
   So we create one from the 8x8 font by adding an extra blank line
   from each side.  */
static void
maybe_create_8x10_font(void)
{
  unsigned char *p;
  unsigned long src, dest, i, j;

  if (font_seg == -1)
    {
      __dpmi_regs regs;
      int buf_pm_sel;
      
      /* Allocate buffer in conventional memory. */
      font_seg = __dpmi_allocate_dos_memory(160, &buf_pm_sel);

      if (font_seg == -1)
        return;

      /* Get the pointer to the 8x8 font table.  */
      p = (unsigned char *)malloc(2560); /* 256 chars X 8x10 pixels */
      if (p == (unsigned char *)0)
        {
          errno = ENOMEM;
          __dpmi_free_dos_memory(buf_pm_sel);
          font_seg = -1;
          return;
        }
      regs.h.bh = 3;
      regs.x.ax = 0x1130;
      __dpmi_int(0x10, &regs);
      src =  ( ( (unsigned)regs.x.es ) << 4 ) + regs.x.bp;
      dest = ( (unsigned)font_seg ) << 4;

      /* Now copy the font to our table, stretching it to 8x10. */
      _farsetsel(_dos_ds);
      for (i = 0; i < 256; i++)
        {
          /* Fill first extra scan line with zeroes. */
          _farnspokeb(dest++, 0);

          for (j = 0; j < 8; j++)
            {
              unsigned char val = _farnspeekb(src++);

              _farnspokeb(dest++, val);
            }

          /* Fill last extra scan line with zeroes. */
          _farnspokeb(dest++, 0);
        }
    }
}

/* Load the 8x10 font we created into character generator RAM.  */
static int
load_8x10_font(int bank,int modeRecalculate)
{
  __dpmi_regs regs;

  maybe_create_8x10_font();         /* create if needed */
  if (font_seg == -1)
    return 1;
  regs.x.es = font_seg;             /* pass pointer to our font in ES:BP */
  regs.x.bp = 0;
  regs.x.dx = 0;                    /* 1st char: ASCII 0 */
  regs.x.cx = 256;                  /* 256 chars */
  regs.h.bh = 10;                   /* 10 points per char */
  regs.h.bl = bank;                 /* block */
  regs.x.ax = 0x1110;
  if (!modeRecalculate)
     regs.h.al&=0xF;
  __dpmi_int(0x10, &regs);
  return 0;
}
#endif
