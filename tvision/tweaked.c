/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#ifdef __DJGPP__

/* This source code is taken from Christian Domp <chrisd@europeonline.com> */
#include <stdio.h>
#include <pc.h>
#include <dpmi.h>
#include <sys/farptr.h>
#include <dos.h>
#include <go32.h>

#define NUM_TWEAKED_MODES 7
#define TW80X30		0
#define TW80X34		1
#define TW90X30		2
#define TW90X34		3
#define TW94X30		4
#define TW94X34		5
#define TW82X25		6 // That's a replacement for 80x25 but with 8 pixels
                          // fonts

static unsigned short width[]={80,80,90,90,94,94,82};
static unsigned short height[]={30,34,30,34,30,34,25};

/* 80x30 example
    0x0c11, Enable writes to index 0-7
    0x5f00, 0x5F+5 = 100 = total horizontal chars
    0x4f01, 0x4F+1 = 80  = displayed horizontal chars
    0x5002, 0x50   = 80  = start of horizontal blank
    0x8203, b7: enable access to 10 and 11 b5-b6: Horiz. delay = 0
            b0-b4 low bits of horiz blank end (2).
    0x5504, 0x55   = 85  = start of retrace.
    0x8105, b7: b5 of horiz blank end => 100010 = 34 (80+34-100=14)
    0x0b06, Vertical total (low 8 bits)
    0x3e07, b5: b9 VT, b0: b8 VT => Vertical total 1000001011=523
            b7: b9 VRS, b2: b8 VRS => 01
            b6: b9 VDE, b1: b8 VDE => 01
            b3: b8 VBS => 1
    0x4f09, b0-4: 15+1=16 lines characters b5: b9 VBS b6: b9 LCR b7: No double scan
    0xea10, Vertical retrace start 0xEA => 0x1EA = 490
    0x8c11, Disable access to 0-7, Vertical retrace ends when low bits are 0xC
    0xdf12, Vertical Display End 0xDF => 0x1DF = 479 (I think is 16*30=480 -1)
    0x2813, 0x28 = 40 => 40*2 = 80 characters per line
    0xe715, Vertical Blank Start 0xE7 => 0x1E7 = 487
    0x0416  Vertical Blank End   4
*/

static unsigned short ttweaks[][16] = {
    {   /* 80x30 */
    0x0c11,0x5f00,0x4f01,0x5002,0x8203,0x5504,0x8105,0x0b06,
    0x3e07,0x4f09,0xea10,0x8c11,0xdf12,0x2813,0xe715,0x0416
    },
    {   /* 80x34 */
    0x0c11,0x5f00,0x4f01,0x5002,0x8203,0x5504,0x8105,0x0b06,
    0x3e07,0x4d09,0xea10,0x8c11,0xdf12,0x2813,0xe715,0x0416
    },
    {   /* 90x30 */
    0x0c11,0x6d00,0x5901,0x5a02,0x9003,0x6004,0x8b05,0x0b06,
    0x3e07,0x4f09,0xea10,0x8c11,0xdf12,0x2d13,0xe715,0x0416
    },
    {   /* 90x34 */ /* Corrected for 476 lines, SET */
    0x0c11,0x6d00,0x5901,0x5a02,0x9003,0x6004,0x8b05,0x0b06,
    0x3e07,0x4d09,0xea10,0x8c11,0xdb12,0x2d13,0xe715,0x0416
    },
    {   /* 94x30 */
    0x0c11,0x6c00,0x5d01,0x5e02,0x8f03,0x6204,0x8e05,0x0b06,
    0x3e07,0x4f09,0xea10,0x8c11,0xdf12,0x2f13,0xe715,0x0416
    },
    {   /* 94x34 */ /* Corrected for 476 lines, SET */
    0x0c11,0x6c00,0x5d01,0x5e02,0x8f03,0x6204,0x8e05,0x0b06,
    0x3e07,0x4d09,0xea10,0x8c11,0xdb12,0x2f13,0xe715,0x0416
    },
    {   /* 82x25 */
    0x0c11,0x6100,0x5101,0x5202,0x8203,0x5704,0x8105,0xb406,
    0x1f07,0x4f09,0x9a10,0x8c11,0x8f12,0x2913,0x9715,0x0016
    }
};

int set_tweaked_text(int tmode, void (*set_font)(int),
                     void (*sm)(__dpmi_regs *regs))
{
  __dpmi_regs regs;
  int i;
  volatile int junk;
  
  if(tmode>=NUM_TWEAKED_MODES) return -1;
  
  regs.x.ax = 0x3 | 0x80; // do not clear the screen
  sm(&regs);

  /* load 8x14 font for 34 line modes */
  if(height[tmode] == 34)
    set_font(14);
  else
    set_font(16); /* That's redundant if TFont isn't overrided */

  /* wait for vertical retrace */
  while((inportb(0x3da) & 8) != 0);
  while((inportb(0x3da) & 8) == 0);
  disable();
  if (tmode==TW82X25)
     outportb(0x3c2,0x63); // 400 lines, 25 MHz
  else
     outportb(0x3c2,0xe7); // 480 lines, 28 MHz
  /* set sequencer clocking mode */
  outportw(0x3c4,0x101);
  /* reset data flip-flop to address mode */
  junk=inportw(0x3da);
  outportb(0x3c0,0x30);
  /* set mode control register */
  outportb(0x3c0,0x04);
  outportb(0x3c0,0x33);
  /* set horizontal pixel pan */
  outportb(0x3c0,0x00);
  for(i = 0; i < 16; i++) {
    outportw(0x3d4,ttweaks[tmode][i]);
  }
  _farsetsel(_dos_ds);
  /* update BIOS data area */
  _farnspokew(0x44a,width[tmode]);
  _farnspokeb(0x484,height[tmode] - 1);
  _farnspokew(0x44c,width[tmode] * height[tmode] * 2);
  enable();
  return 0;
}

#ifdef TEST

void set_font(int a)
{
 printf("Fonts of: %d lines\n",a);
}

int main(void)
{
  int i,mode;

  for(mode=0;mode<NUM_TWEAKED_MODES;mode++) {
    set_tweaked_text(mode,set_font);
    printf("\n%d x %d mode\n",ScreenCols(),ScreenRows());
    for(i=0;i<10;i++) printf("1234567890");
    printf("\n");
    getkey();
    for(i=50;i>0;i--) printf("%d\n",i);
    getkey();
  }
  return 0;
}
#endif

#endif // __DJGPP__

