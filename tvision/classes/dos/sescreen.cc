/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
/* Copyright 1999-2002 Salvador E. Tropea */
#include <tv/configtv.h>

/* This is needed, because get the correct textmode */
#define Uses_TScreen
#include <tv.h>

// I delay the check to generate as much dependencies as possible
#ifdef TVCompf_djgpp
/* This file is for saving/restoring the screen in graphics and/or
   text modes. Many parts and most of the very good comments came
   from SET.

   NOTE: by SET 1999-04: I think this code is broken and I check it latter
   so I'm not fixing here the VESA stuff about using the __tb.
*/
/*
  Hi Robert:

----------------------------------------------------------------------------
Chapter 1) XMode v.s. Text modes:

  I'm sure that this weekend you arrive to the same conclution that me, or
may be not:
  Your code works OK to restore Mode 3 -> X-Mode -> Mode 3 -> X-Mode, but
if I use a better text mode like 80x60 the code doesn't work, in fact works
only up to 80x50 or up to 132x25.
  I'll try to explain you my conclutions:

  The VGA controler use a linear mode when is in text mode, in this linear
mode the first char in 0xB8000 is the first char in your memory (256Kb).
  So if you set an 80x60 mode you'll use 80x60x2 bytes of the memory that's
9600 bytes and you are saving only 8192 bytes!!, so the result is that a part
of the screen isn't restored.
  May be I wasn't clear when I said that you save 8192: In planar modes you
access to the position 0 in the video memory in plane 0 (or 1 depending on
if we start to number from 0 or 1), the position 0 in the plane 1 is the
position 65536 of the memory with respect to the linear mode, so saving
32Kb you save 0-8Kb 64Kb-72Kb 128Kb-136Kb and 192Kb-200Kb (ever taking the
linear as reference).
  So in 132x60 (for example) you must save 0-15840 and you only save 0-8Kb.
  I modified your code to make that, perhapes is better if you rewrite it from
the new knowledge.
  Now, saving Cols*Rows*2 bytes of plane 0 (I called plane 1 in the code),
you'll save all but .... I saw that the plane 1 is wasted too!, so you must
save the same amount for plane 1. The plane 2 have the fonts, seems that
the BIOS uses half of the RAM (0-128Kb) for different text pages and the
rest for fonts, at least in my board the fonts are stored in plane 2, now
how much uses?, in my board there are fonts from 8x8 to 9x16, 9x16 means 32
bytes per character so with 256 chars you'll use 8192. Finally plane 3
remains untouched so we don't need to save it.
  That's (80x25*2)*2+8192=16192 => ONLY 16Kb! (and not 64Kb = 32Kb+malloc
overhead like the previous version) and in the worst case 39872 and that's
just the same amount of memory wasted but saves the right thing.
  Now the question is: Is that correct in other cards?.

  I don't know if you understood my code (the one that I sent you some weeks
ago), in my code I don't need to worry about if the mode is planar or linear,
I just setup a planar addressing mode, why you don't make that, is easier. If
you are worring about the flickering I can cure that.
-----------------------------------------------------------------------------
Chapter 2) 16 colors mode v.s. Text modes:

  Well that's almost the same as an X-mode but ... you are making some thing
very strange, I must admit that I tried for 4 hours until I get a working
routine but is the same as X-mode.
  From what I can see you don't have any documentation about the VGA
registers, is that true?, I have a nice Norton Guide for it.
  Anyways you are using the write mode 3, this mode is useful for tricks but
not for dumping memory, in mode 3 you need 2 operations by byte in memory,
you are making that with the ANDL, but if you use mode 0 you can do the same
just with a simple write, of course you must be sure that all the registers
involved will not mask your write.
  In this file I'm using the same routine for:

   320x200x16
   640x200x16
   640x350x16
   640x480x16
   320x240x256
   320x400x256
   360x480x256

-----------------------------------------------------------------------------
Chapter 3) Mode 0x13 v.s. Text modes:

  You think that this is the easier?, lamentably no.
  This mode is not linear like text mode and is not planar as the 16 colors
modes and mode X, so what is?, the bits 0 and 1 of the address sets the plane
so two adjacent pixels addressed with 1 of difference are in different planes,
that's very good for programing, the mode is very easy to handle, but isn't
good for owr purposes. I don't have enough information but seems that the
constroler uses 1/4 of the pixels to show the screen, so if the user sets
the mode 0x13 and don't make any strange thing we only need to save:

  max(8192,Cols*Rows*2)

  8192 is for the fonts, that's what the current program does, the problem is
that we are saving pixels that aren't needed and don't saving pixels that
we will touch. Until now I don't know how to address the missing pixels.

  Now pass to vga2.cc where I continue.

*/

/*
  Well that's the second part:

  As I can't address the right memory in the 0x13 mode I placed my old
methode again here, so now I save 16Kb for 80x25 and 39Kb (64Kb) for
132x60.

  Now I tested this file with:

Card: Trident TGUI9440AGi
Text modes:
Mode  Tx.Resol.  Font Pixels
0x50  80x30     (8x16 640x480)
0x51  80x43     (8x11 640x473)
0x52  80x60     (8x8  640x480)
0x53  132x25    (8x14 1056x350)
0x54  132x30    (8x16 1056x480)
0x55  132x43    (8x11 1056x473)
0x56  132x60    (8x8  1056x480)
0x57  132x25    (9x14 1188x350)
0x58  132x30    (9x16 1188x480)
0x59  132x43    (9x11 1188x473)
0x5A  132x60    (9x8  1188x480)
0x108 80x60     (Vesa 1)
0x109 132x25    (Vesa 1)
0x10A 132x43    (Vesa 1)
0x10B 132x60    (Vesa 1) <--- Must be x50 but Trident ...
0x10C 132x60    (Vesa 1)

To Graphic modes:
   320x200x16   (0x0D)
   640x200x16   (0x0E)
   640x350x16   (0x10)
   640x480x2    (0x11) Partially tested.
   640x480x16   (0x12)
   320x200x256  (0x13)
   320x240x256  X-Mode
   320x400x256  X-Mode
   360x480x256  X-Mode

*/

/*
  Well ...

Chapter 4) VESA v.s. Text modes:

  Here I have some problems, the 16 color modes can be handled in the same
way, and as we never need to handle more that 132*60*2=15840 bytes per plane
we don't need any bank switch, so I used the same trick that in VGA to skip
a plane. May be is wrong assumption, but all the boards that I know have
32Kb, 64Kb and 128Kb windows.
  The 256 color modes are harder, I can only guess here, in my board seems
to be like a mode 0x13 but ALL the pixels are visible (not 1/4), so saving
32Kb like you tried doesn't work for the 132*60 because this mode uses
15840 bytes in the first bank, so if you want to save the memory in a linear
way you must save 132*60*2*4=63360 and that can envolve a bank switch in some
cards, so I let your bank switch code, but for this case.
  The routines seems to be OK for text modes, the vga.cc is not the best
for text modes so I guess that is OK to handle VESA text modes with this
routines and make a simple routine for non-VESA text modes (just the old
thing in TScreen).
  Why you don't use packed structures?, that will never be taked out from
GCC and VESA is PC specific.

  I tried switching from 1024x768x256 and 1024x768x16 to 132x60 and works OK.

*/
  
/*

  Here is a version with Allegro, works in my machine, I tested Vesa 1.x,
Vesa 2.0 banked, Vesa 2.0 linear and Trident and all seems to work OK. I]
guess because even in Vesa 2.0 linear the Vesa 1.x functions still
working.

*/
  
/*

  That's the unified version, is supposed to work with all the modes ... ;-)
  I joined the VGA routines, the VESA routines and routines to save the text
modes, as the BIOS don't have any routine to retrieve the fonts I'm not
restoring the original fonts but a fresh copy from the ROM. That can produce
problems in mode too much twisted where the fonts aren't 8x8, 8x14 or 8x16.
  There are examples with Allegro, GRX20 and conio.

I changed some things:

1) In save_vesa():

  if (WinBAttributes & 0x3) // exist and readable
  {
>>>>>>>>> The condition is wrong you can get here if Win B exists and is
>>>>>>>>> write-only!
    window_r = 1;
    win_seg_r = WinBSegment;
    // I assume here, that now winb is writable. Is that correct??
>>>>>>>>> May be is correct but, why take the risk?
    window_w = 0;
    win_seg_w = WinASegment;
  }
  So I rewritted that.

2) I don't like to use DOS memory but lamentably you'll include it in the
debugger so we can't use the transfer buffer :-(.

3) In save_vesa():
   I added bank_increment (see in the function) because WinGranularity can
be different to WinSize.

4) In restore_vesa():
   I putted out the call to service 2 (set mode) because restore_vesa_state
must be enough.

5) The 16 colors VESA modes are using the same core routine that the VGA
modes now.

*/

#include <stdlib.h>
#include <sys/movedata.h>
#include <sys/farptr.h>
#include <dpmi.h>
#include <go32.h>
#include <pc.h>

#include <tv/video.h>
extern int user_mode;

static void *buffer = NULL;
static int buffer_size = 0;
static int saved_size;
static int window_r;
static int window_w;
static int win_size;
static unsigned win_seg_r;
static unsigned win_seg_w;
static int bank_increment;
static int is_plane_mode = 0;
static int TextModeXres;
static int TextModeYres;

/*****************************************************************************

***************************  VESA BIOS ROUTINES ******************************

*****************************************************************************/
/*****************************************************************************

    Function: int GetVesaModeInfo(int mode)

    Description: Gets the information of a VESA mode.
                 0x4F01 service of VESA 1.x

    Parameters:
    int mode: The mode to ask to BIOS.

    Return:
    0 fail.
    1 OK.

    Note: Uses 256 bytes of DOS memory.

*****************************************************************************/

static int mode_info_segment = -1;
static int mode_info_selector;

static int GetVesaModeInfo(int mode)
{
  __dpmi_regs r;
  if (mode_info_segment == -1)
    mode_info_segment = __dpmi_allocate_dos_memory(16,&mode_info_selector);
  if (mode_info_segment == -1)
    return 0;
  r.x.ax = 0x4f01;
  r.x.es = mode_info_segment;
  r.x.di = 0;
  r.x.cx = mode;
  __dpmi_int(0x10,&r);
  if (r.x.ax != 0x004f)
    return 0;
  return 1;
}

/*****************************************************************************

    Function: void save_vesa_state()

    Description: Saves all the state of the VESA system to restore the mode
                 later. 0x4F04 Service.

    Note: Uses an unknown amount of DOS memory (2.5Kb aprox.)

*****************************************************************************/

static int vesa_state_segment = -1;
static int vesa_state_selector;

static void save_vesa_state()
{
  __dpmi_regs r;
  if (vesa_state_segment == -1)
  {
    r.x.ax = 0x4f04;
    r.h.dl = 0x00;
    r.x.cx = 0x000f;
    __dpmi_int(0x10,&r);
    if (r.x.ax != 0x004f) return;
    vesa_state_segment =
                  __dpmi_allocate_dos_memory(r.x.bx*4,&vesa_state_selector);
    if (vesa_state_segment == -1) return;
  }
  r.x.ax = 0x4f04;
  r.h.dl = 0x01;
  r.x.es = vesa_state_segment;
  r.x.bx = 0;
  r.x.cx = 0x000f;
  __dpmi_int(0x10,&r);
}

/*****************************************************************************

    Function: void restore_vesa_state(int What)

    Description: Restores part or all of the saved information about the VESA
                 state. 0x4F04 Service.

    Parameters:
    int What: What was saved, USE RESTORE_VESA_ALL EVER.

*****************************************************************************/

static void restore_vesa_state(int What)
{
  __dpmi_regs r;
  if (vesa_state_segment == -1) return;
  r.x.ax = 0x4f04;
  r.h.dl = 0x02;
  r.x.es = vesa_state_segment;
  r.x.bx = 0;
  r.x.cx = What;
  __dpmi_int(0x10,&r);
}

/*****************************************************************************

    Function: void check_for_vesa()

    Description: Checks if VESA BIOS is present.
                 0x4F00 Service.

    Notes:
    1) The result is stored in vesa_supported.
    2) Is called by the initialization routine.
    3) Uses 512 bytes of DOS memory, can be liberated.

*****************************************************************************/

static int vesa_supported = 0;

static void check_for_vesa()
{
  __dpmi_regs r;

  r.x.ax = 0x4f03;
  __dpmi_int(0x10,&r);

  /* Not supported */
  if (r.h.al != 0x4f) return;

  vesa_supported = 1;
}

/*****************************************************************************

    Function: int is_vesa_mode()

    Description: Checks if the current mode is VESA, if yes retrieves the
                 information about the mode. 0x4F03 Service.
    Return:
    0 not, VESA or fail.
    1 VESA

*****************************************************************************/

static int is_vesa_mode()
{
  __dpmi_regs r;

  r.x.ax = 0x4f03;
  __dpmi_int(0x10,&r);

  /* Not supported or not successful */
  if (r.x.ax != 0x004f) return 0;

  /* Because GetVesaModeInfo forces a screen switch under W95, I check
     the most common textmode here without calling that function */
  if (r.x.bx == 0x0003 || r.x.bx == 7)
    return 0;

  if (!GetVesaModeInfo(r.x.bx))
    return 0;

  if (!(ModeAttributes & 1))
    // not supported
    return 0;

  user_mode = r.x.bx;

  return 1;
}
/************************ END of VESA BIOS ROUTINES *************************/



/*****************************************************************************

****************************  VGA BIOS ROUTINES ******************************

*****************************************************************************/

/*****************************************************************************

    Function: void save_vga_state()

    Description: Saves all the state of the VGA system to restore the mode
                 later. 0x1C01 Service.

    Note: Uses an unknown amount of DOS memory (2.5Kb aprox.)

*****************************************************************************/

static int vga_state_segment = -1;
static int vga_state_selector;

static
void save_vga_state()
{
  __dpmi_regs r;
  if (vga_state_segment == -1)
  {
    r.x.ax = 0x1c00;
    r.x.cx = 0x0007;
    __dpmi_int(0x10,&r);
    if (r.h.al != 0x1c) return;
    vga_state_segment =
                  __dpmi_allocate_dos_memory(r.x.bx*4,&vga_state_selector);
    if (vga_state_segment == -1) return;
  }
  r.x.ax = 0x1c01;
  r.x.es = vga_state_segment;
  r.x.bx = 0;
  r.x.cx = 0x0007;
  __dpmi_int(0x10,&r);
}

/*****************************************************************************

    Function: void restore_vga_state(int What)

    Description: Restores part or all of the saved information about the VGA
                 state. 0x1C02 Service.

    Parameters:
    int What: What was saved, USE RESTORE_VGA_ALL EVER.

*****************************************************************************/

static void restore_vga_state(int What)
{
  __dpmi_regs r;
  if (vga_state_segment == -1) return;
  r.x.ax = 0x1c02;
  r.x.es = vga_state_segment;
  r.x.bx = 0;
  r.x.cx = What;
  __dpmi_int(0x10,&r);
}

/*****************************************************************************

    Function: int is_vga_mode()

    Description: Check if the current mode is VGA and can be handled.
    Return:
    0 fail
    1 OK

    Notes:
    Define FULLBIOS to check directly trough BIOS instead of using the
    BIOS Data Area (I guess is just the same).

    Supported modes:
 
    320x200x16   (0x0D)
    640x200x16   (0x0E)
    640x350x16   (0x10)
    640x480x2    (0x11) Partially tested.
    640x480x16   (0x12)
    320x200x256  (0x13)
    320x240x256  X-Mode
    320x400x256  X-Mode
    360x480x256  X-Mode
    Other X-Modes derivated from mode 0x13.

*****************************************************************************/

static int is_vga_mode()
{
#ifdef FULLBIOS
  __dpmi_regs r;

  r.x.ax = 0x0f00;
  __dpmi_int(0x10,&r);

  user_mode = r.h.al;
#else
  user_mode = ScreenMode();
#endif

  if (user_mode != 0x0d &&
      user_mode != 0x0e &&
      user_mode != 0x10 &&
      user_mode != 0x11 &&
      user_mode != 0x12 &&
      user_mode != 0x13
     ) return 0;

  if (user_mode == 0x12 ||
      user_mode == 0x0d ||
      user_mode == 0x0e ||
      user_mode == 0x10
     )
    {
     is_plane_mode = 1;
    }
  else
  {
    unsigned char b;
    outportb(VGA_SEQUENCER_PORT,VGA_MEMORY_MODE_REG);
    b = inportb(VGA_SEQUENCER_DATA);
    is_plane_mode = b & 0x08 ? 0 : 1;
  }

  return 1;
}
/************************* END of VGA BIOS ROUTINES *************************/

/****************************** TXT BIOS ROUTINES ***************************/

/*****************************************************************************

    Function: int is_txt_mode()

    Description: Checks if the current mode is a text mode and can be handled.
    Return:
    0 fail
    1 OK

    Notes:
    Define FULLBIOS to check directly trough BIOS instead of using the
    BIOS Data Area (I guess is just the same).

    Supported modes:
 
    40x25 Color (0,1), Fonts 8x8, 8x14 and 8x16
    80x25 Color (2,3), Fonts 8x8, 8x14 and 8x16
    80x25 B&W   (7)    Hercules
    80x43 Color
    80x50 Color
    Other modes derivated from mode 3 with fonts of 8, 14 and 16 scan lines.

*****************************************************************************/

static int is_txt_mode()
{
#ifdef FULLBIOS
  __dpmi_regs r;

  r.x.ax = 0x0f00;
  __dpmi_int(0x10,&r);

  user_mode = r.h.al;
#else
  user_mode = TScreen::getCrtMode();
#endif

  if ((user_mode & 0xff)<4 || user_mode==7)
    {
     if (user_mode==7)
        win_seg_r=0xB0000;
     else
        win_seg_r=0xB8000;
    }
  else
    return 0;

  return 1;
}
/*************************** End of TXT BIOS ROUTINES ***********************/

/*****************************************************************************

    Function: void SetAddressingMode()

    Description: Puts the VGA controler in Write Mode 0, Read Mode 0 and if
                 the mode isn't planar in planar mode.

    The 0 modes are the better to dump the video memory.

*****************************************************************************/

static void SetAddressingMode()
{
 if (is_plane_mode)
   {
    outportb(VGA_GR_CTRL_PORT,VGA_MODE_REG);
    /* set read mode 0 (and write mode 0) */
    outportb(VGA_GR_CTRL_DATA,inportb(VGA_GR_CTRL_DATA) & 0xF4);
   }
 else
   {
    // That's select write mode 0, read mode 0 and planar addressing
    asm ("movw   $0x604,%%ax \n\t"
         "movw   $0x3C4,%%dx \n\t"
         "outw   %%ax,%%dx   \n\t"
         "movb   $0x40,%%ah  \n\t"
         "incb   %%al        \n\t"
         "movw   $0x3CE,%%dx \n\t"
         "outw   %%ax,%%dx   \n\t"
         "movb   $5,%%ah     \n\t"
         "incb   %%al        \n\t"
         "outw   %%ax,%%dx" : : : "%edx", "%eax" );
   }
}

/*****************************************************************************

    Function: int SavePlanar(int OffTarget)

    Description: Saves Cols*Rows*2 bytes of planes 0 and 1, and 8192 bytes
                 of the plane 3.
    Parameters:
    int OffTarget: Offset of the video window memory
    Return:
    The number of bytes used.

*****************************************************************************/

static int SavePlanar(int OffTarget)
{
 int offset = 0;
 int i,bs;

 // Ensure that the mode of addressing is OK
 SetAddressingMode();

 // Now save 3 planes, plane 4 is never touched by a normal text mode
 for (i=0;i<3;i++)
    {
     /* read from plane 'i' */
     outportw(VGA_GR_CTRL_PORT,((i << 8) | VGA_RD_PLANE_SEL_REG));
     if (i==2)
        bs=8192;
     else
        bs=TextModeXres*TextModeYres*2;
     movedata(_dos_ds,OffTarget,_my_ds(),(int)buffer+offset,bs);
     offset += bs;
    }

 return offset;
}

/*****************************************************************************

    Function: int RestorePlanar(int OffTarget)

    Description: Is the complementary to SavePlanar.

*****************************************************************************/

static int RestorePlanar(int OffTarget)
{
 int offset = 0;
 int i;
 int bs;

 // Ensure that the mode of addressing is OK
 SetAddressingMode();

 // Let pass all bits
 outportw(VGA_GR_CTRL_PORT,0xFF00 | VGA_BIT_MASK_REG);
 // Just put the value, don't and,xor,etc.
 outportw(VGA_GR_CTRL_PORT,VGA_ROT_FN_SEL_REG);
 // Don't use reset masks
 outportw(VGA_GR_CTRL_PORT,VGA_SET_RESET_ENB_REG);
 outportw(VGA_GR_CTRL_PORT,VGA_SET_RESET_REG);

 unsigned long *_l = (unsigned long *)buffer;
 for (i=0;i<3;i++)
    {
     /* enable plane 'i' for writing */
     outportw(VGA_SEQUENCER_PORT,(((1 << i) << 8) | VGA_WRT_PLANE_ENB_REG));

     if (i==2)
        bs=8192;
     else
        bs=TextModeXres*TextModeYres*2;

     movedata(_my_ds(),(int)_l,_dos_ds,OffTarget,bs);
     _l += bs/4;

     offset+=bs;
    }

 return offset;
}

/*****************************************************************************

    Function: void save_vesa(void)

    Description:
    Saves:
    1) The VESA registers.
    2) The BIOS data.
    3) The palette.
    4) Depending on the mode:
       a) Text modes: (XRes*YRes*2) * 4.
       b) 256 colors modes: XRes*YRes*2.
       c) 16 colors modes: XRes*YRes*2 from planes 1 and 2 and 8192 from
                           plane 3.
       where XRes and YRes are the resolution of the text mode.

*****************************************************************************/

static void save_vesa()
{
  int offset = 0;
  int bank = 0;
  __dpmi_regs r;
  save_vesa_state();

  // Window selection and address
#if 0
  // About, that hmmmm... isn't a good idea
  if (WinBAttributes & 0x3) /* exist and readable */
  {
    window_r = 1;
    win_seg_r = WinBSegment;
    /* I assume here, that now winb is writable. Is that correct?? */
    window_w = 0;
    win_seg_w = WinASegment;
  }
  else
  {
    window_w = window_r = 0;
    win_seg_r = win_seg_w = WinASegment;
  }
#else
  // That's more reliable
  if ((WinBAttributes & 0x3)==0x3)
    { // If B exists and can be readed use B to read
     window_r = 1;
     win_seg_r = WinBSegment;
    }
  else
    { // Else use A, one of the 2 must be readable
     window_r = 0;
     win_seg_r = WinASegment;
    }
  if ((WinAAttributes & 0x5)==0x5)
    { // If A exists and can be writed use A to write
     window_w = 0;
     win_seg_w = WinASegment;
    }
  else
    { // Else use B, one of the 2 must be writable
     window_w = 0;
     win_seg_w = WinBSegment;
    }
#endif
  win_seg_r <<= 4;
  win_seg_w <<= 4;


  // Window size
  win_size = WinSize * 1024;

  // Normally granularity is == to win size but ....
  // I assume that are multiples, for example,
  // WinSize=128, WinGranularity=64
  bank_increment = win_size / (WinGranularity*1024);

  // Size to save, that's isn't a fixed value:
  if ((ModeAttributes & 0x10) && (NumberOfPlanes > 1))
    {
     saved_size =
     (TextModeXres*TextModeYres*2) // The amount of memory destroyed in plane 1
     *2                            // I don't know why but the same for plane 2
     +8192     // The fonts in plane 3, is that the worst case?
               // there are 9x16 fonts, so I guess that's 32 bytes per
               // character then 32*256=8192
     ;
    }
  else
    {
     if (ModeAttributes & 0x10)
        saved_size = (TextModeXres*TextModeYres*2) * 4;
     else
        // Here I don't save the fonts used memory, what do you think?
        saved_size = TextModeXres*TextModeYres*2;
    }

  // Just an adjust for the methode used
  if (saved_size < win_size)
     win_size = saved_size;

  // Memory allocation
  if (saved_size > buffer_size)
  {
    buffer = realloc(buffer,saved_size);
    buffer_size = saved_size;
  }

  // Loop switching bank until we get all the memory needed
  while (offset < saved_size)
  {
   if (ModeAttributes & 0x10) // test for graphic mode
   { // Here the assumptions are:
     // 1) A text mode doesn't need a bank change.
     // 2) A 16 colors mode can be restored from one bank.

     // Switch the right bank
     r.x.ax = 0x4f05;
     r.x.bx = window_r;
     r.x.dx = bank;
     __dpmi_int(0x10,&r);
  
     if (NumberOfPlanes > 1)
     { // 16 colors modes, Here I asume that all is in 1 bank
      is_plane_mode=1;
      offset+=SavePlanar(win_seg_r);
     }
   }
   // Here is for text and 256 colors modes:
   if (NumberOfPlanes == 1)
     {
      movedata(_dos_ds,win_seg_r,_my_ds(),(int)buffer+offset,win_size);
      offset += win_size;
     }
   // next bank if needed ... I don't think that this will happend
   bank+=bank_increment;
  }
}

/*****************************************************************************

    Function: restore_vesa()

    Description: Is the reverse of save_vesa().

*****************************************************************************/

static void restore_vesa()
{
  int offset = 0;
  int bank = 0;
  __dpmi_regs r;

#ifdef FORCE_SET_VIDEO_MODE
  // Restore the mode?, why?, In my card the restore_vesa_state is enough
  r.x.ax = 0x4f02;
  r.x.bx = user_mode | 0x8000;
  __dpmi_int(0x10,&r);
#endif

  restore_vesa_state(RESTORE_VESA_ALL);

  while (offset < saved_size)
  {
   if (ModeAttributes & 0x10) // test for graphic mode
   {
     r.x.ax = 0x4f05;
     r.x.bx = window_w;
     r.x.dx = bank;
     __dpmi_int(0x10,&r);
 
     if (NumberOfPlanes > 1)
     { // Here I asume that all is in 1 bank
      is_plane_mode=1;
      offset+=RestorePlanar(win_seg_w);
     }
   }
   if (NumberOfPlanes == 1)
   {
     movedata(_my_ds(),(int)buffer+offset,_dos_ds,win_seg_w,win_size);
     offset += win_size;
   }
   bank+=bank_increment;
  }

  /* restore again, because of the change of the registers and the bank */
  restore_vesa_state(RESTORE_VESA_ALL);
}

static void save_vga()
{
  save_vga_state();
  // Fixed for VGA graphic modes, there are a methode to know the true but ...
  win_seg_r = 0xa0000;

  // Amount to save, that's isn't a fixed value:
  saved_size =
  (TextModeXres*TextModeYres*2) // The amount of memory destroyed in plane 1
  *2                            // I don't know why but the same for plane 2
  +8192     // The fonts in plane 3, is that the worst case?
            // there are 9x16 fonts, so I guess that's 32 bytes per
            // character then 32*256=8192
  ;

  // Allocate memory
  if (saved_size > buffer_size)
    {
     buffer = realloc(buffer,saved_size);
     buffer_size = saved_size;
    }

  // Save the planes
  SavePlanar(win_seg_r);

#if 0
  // Don't needed if we'll enter in text mode just after that
   /* restore the registers */
  restore_vga_state();
#endif
}

static void restore_vga()
{
#ifdef FORCE_SET_VIDEO_MODE
  __dpmi_regs r;
  r.x.ax = (user_mode & 0x7f) | 0x80;
  __dpmi_int(0x10,&r);
#endif
  restore_vga_state(RESTORE_VGA_ALL);

  // Put the data in the correct planes
  RestorePlanar(win_seg_r);

  // Restore again because we touched a lot of things
  restore_vga_state(RESTORE_VGA_ALL);
}


static void save_txt()
{
  if (user_mode != 7)
    save_vga_state();

  // Amount to save, that's isn't a fixed value:
  saved_size = TextModeXres*TextModeYres*2;

  // Allocate memory
  if (saved_size > buffer_size)
    {
     buffer = realloc(buffer,saved_size);
     buffer_size = saved_size;
    }

  movedata(_dos_ds,win_seg_r,_my_ds(),(int)buffer,saved_size);
}

static void restore_txt()
{
  __dpmi_regs r;
#ifdef FORCE_SET_VIDEO_MODE
#if 0
  r.x.ax = (user_mode & 0x7f) | 0x80;
  __dpmi_int(0x10,&r);
#else
  TScreen::setCrtMode(user_mode);
#endif
#endif
  if (user_mode != 7)
  {
    restore_vga_state(RESTORE_VGA_ALL);

    unsigned char height;
    _dosmemgetb(0x485,1,&height);
  
    r.h.ah=0x11;
    r.x.bx=0;
    switch (height)
      {
       case 8:
            r.h.al=0x02;
            break;
       case 14:
            r.h.al=0x01;
            break;
       default:
            r.h.al=0x04;
      }
    __dpmi_int(0x10,&r);

    restore_vga_state(RESTORE_VGA_ALL);
  }

  movedata(_my_ds(),(int)buffer,_dos_ds,win_seg_r,saved_size);
}


/************************* Unified STUFF ************************************/

/*****************************************************************************

    Function: void InitSaveSystem(void)

    Description: Initialize all the needed things, must be called before all.

*****************************************************************************/

static void InitSaveSystem(void)
{
 check_for_vesa();
}

#define VGA  0
#define VESA 1
#define TXTM 2
#define NONE 255

static int type_saved;

/*****************************************************************************

    Function: void SaveVideo(int text_x,int text_y)

    Description: Saves all the information to restore the actual video state.
    Arguments:
      text_x:  The colums, which the app will overwrite in text mode
      text_y:  The rows, which the app will overwrite in text mode

*****************************************************************************/

static int inited = 0;
static void InitSaveSystem(void);

void set_SaveVideo(int text_x,int text_y)
{
 TextModeXres = text_x;
 TextModeYres = text_y;
 type_saved=NONE;
 if (!inited)
 {
   InitSaveSystem();
   inited = 1;
 }
 if (vesa_supported)
    if (is_vesa_mode())
      {
       save_vesa();
       type_saved=VESA;
       return;
      }
 if (is_vga_mode())
   {
    save_vga();
    type_saved=VGA;
    return;
   }
 if (is_txt_mode())
   {
    save_txt();
    type_saved=TXTM;
   }
 // Now what can I do ...?
}

/*****************************************************************************

    Function: void RestoreVideo(void)

    Description: Restores the saved video state.

*****************************************************************************/

void set_RestoreVideo(void)
{
 switch (type_saved)
   {
    case TXTM:
         restore_txt();
         break;
    case VGA:
         restore_vga();
         break;
    case VESA:
         restore_vesa();
         break;
   }
}

/********************** end of Unified STUFF ********************************/

#ifdef TEST

// Here you can choose the methode to test

//#define ALLEGRO
#define GRX20
//#define TEXTMODES
#define USE_TV


#include <stdio.h>
#include <conio.h>
#include <pc.h>

#ifdef USE_TV

#define Uses_TScreen
#if 1
#define Uses_TEditorApp
#include <tv.h>

class IDE : public TEditorApp
{
public:
  IDE() : TProgInit( &IDE::initStatusLine,
                     &IDE::initMenuBar,
		     &IDE::initDeskTop),
	  TEditorApp()
  {}
  void Suspend() { suspend(); }
  void Resume() { resume(); redraw(); }
};

IDE *app;

#define INIT() app = new IDE(); SUSPEND()
#define SUSPEND() app->Suspend()
#define RESUME() app->Resume()
char *ExpandFileNameToThePointWhereTheProgramWasLoaded(char *x)
{
  return x;
}

#else
#include <tv.h>

#define INIT() SUSPEND()
#define SUSPEND() TScreen::suspend()
#define RESUME() TScreen::resume()
#endif

#else // USE_TV

int user_mode;

/*
  This routine sets the video text mode and gets the size because I need
  it to know how much memory is touched by the text mode.
*/
void SetTextMode(int mode)
{
 if (mode>=0x100)
    asm("int $0x10" : : "a"(0x4F02), "b"(mode | 0x8000));
 else
    textmode(mode | 0x80);
#if 0 // now defined to Screen...
 TextModeXres=ScreenCols();
 TextModeYres=ScreenRows();
#endif
}

/*
  This routine simply clears the screen and puts some thing, I'm using
  that because I'm to lazy to reproduce your changes in libtv.a
*/
void Redraw()
{
 _farfill_l(_dos_ds,0xB8000,(TextModeXres*TextModeYres*2)/4,0x07000700);
 printf("That's the screen that should be redrawed by TVision (%d,%d)\n",
        TextModeXres,TextModeYres);
}


#define MODE 0x10C
#define INIT() SetTextMode(MODE); Redraw()
#define RESUME() SaveVideo(ScrennCols(),ScreenRows()); SetTextMode(MODE); Redraw()
#define SUSPEND() RestoreVideo()

#endif // USE_TV

#ifdef ALLEGRO
#include "allegro.h"

/**************************** ALLEGRO STUFF *********************************/
int xoff, yoff;
int mode = DRAW_MODE_SOLID;
#define NUM_PATTERNS    8
#define TIME_SPEED   2
BITMAP *pattern[NUM_PATTERNS];
long tm = 0;        /* counter, incremented once a second */
int _tm = 0;

void do_it(char *msg, int clip_flag, void (*func)())
{ 
   int x1, y1, x2, y2;

   set_clip(screen, 0, 0, 0, 0);
   clear(screen);
   //message(msg);

   if (clip_flag) {
      do {
	 x1 = (rand() & 255) + 32;
	 x2 = (rand() & 255) + 32;
      } while (abs(x1-x2) < 30);
      do {
	 y1 = (rand() & 127) + 40;
	 y2 = (rand() & 127) + 40;
      } while (abs(y1-y2) < 20);
      set_clip(screen, xoff+x1, yoff+y1, xoff+x2, yoff+y2);
   }

   drawing_mode(mode, pattern[rand()%NUM_PATTERNS], 0, 0);

   (*func)();

   solid_mode();
}

int next()
{
   if (keypressed()) {
      clear_keybuf();
      return TRUE;
   }

   if (mouse_b) {
      do {
      } while (mouse_b);
      return TRUE;
   }

   return FALSE;
}

void putpix_demo()
{
   int c = 0;
   int x, y;
   long ct;

   tm = 0; _tm = 0;
   ct = 0;

   while (!next()) {

      x = (rand() & 511) + 32;
      y = (rand() & 255) + 40;
      putpixel(screen, xoff+x, yoff+y, c);

      if (mode >= DRAW_MODE_COPY_PATTERN)
	 drawing_mode(mode, pattern[rand()%NUM_PATTERNS], 0, 0);

      if (++c >= 16)
	 c = 0;

      if (ct >= 0) {
	 if (tm >= TIME_SPEED) {
	    ct = -1;
	 }
	 else
	    ct++;
      }
   }
}

void line_demo()
{
   int c = 0;
   int x1, y1, x2, y2;
   long ct;

   tm = 0; _tm = 0;
   ct = 0;

   while (!next()) {

      x1 = (rand() & 511) + 32;
      x2 = (rand() & 511) + 32;
      y1 = (rand() & 255) + 40;
      y2 = (rand() & 255) + 40;
      line(screen, xoff+x1, yoff+y1, xoff+x2, yoff+y2, c);

      if (mode >= DRAW_MODE_COPY_PATTERN)
	 drawing_mode(mode, pattern[rand()%NUM_PATTERNS], 0, 0);

      if (++c >= 16)
	 c = 0;

      if (ct >= 0) {
	 if (tm >= TIME_SPEED) {
	    ct = -1;
	 }
	 else
	    ct++;
      }
   }
}

void Next()
{
 while (next()==FALSE);
}
/******************************* END of ALLEGRO STUFF **********************/

int main()
{
  int i;

  allegro_init();

  INIT();
  getch();

  install_keyboard();
  install_timer();
  initialise_joystick();
  install_mouse();
  set_gfx_mode(GFX_VGA, 320, 200, 0, 0);
  set_pallete(desktop_pallete);
  show_mouse(NULL);

  int card,w,h;

  if (!gfx_mode_select(&card, &w, &h))
    {
     allegro_exit();
     printf("Aborting\n\n");
     exit(1);
    }

  show_mouse(NULL);
  if (set_gfx_mode( card, w, h, 0, 0))
    {
     allegro_exit();
     printf("Error setting graphics mode\n%s\n\n", allegro_error);
     exit(1);
    }
  set_pallete(desktop_pallete);
  do_it("timing putpixel", FALSE, putpix_demo);
  Next();
  RESUME();
  Next();
  SUSPEND();
  Next();
  RESUME();
  Next();
  SUSPEND();
  do_it("timing line", FALSE, line_demo);
  Next();
  RESUME();
  Next();
  SUSPEND();
  Next();
  RESUME();
  Next();
  SUSPEND();
  Next();
  RESUME();
  Next();
  SUSPEND();
  Next();
  textmode(3);
  allegro_exit();
  return 0;
}
#endif  // ALLEGRO

#ifdef GRX20
/******************************* GRX20 STUFF *******************************/
#include <grx20.h>

int main()
{
  int i;
  char Buf[4];
  INIT();
  getch();
  GrSetMode(GR_width_height_color_graphics,640,480,16);
  //GrSetMode(GR_width_height_color_graphics,320,200,256);
  GrClearScreen(GrBlack());
  Buf[0]=32;
  Buf[1]=33;
  Buf[2]=34;
  Buf[3]=35;
  _dosmemputb(Buf,4,0xA0000);
  getch();
  RESUME();
  getch();
  SUSPEND();
  getch();
  RESUME();
  getch();
  SUSPEND();
  getch();
  for (i=0;i<16;i++)
  {
    GrFilledBox(i*21,0,i*21+20,15,i);
  }
  GrFilledEllipse(300,200,50,50,GrAllocColor(0,0,255));
  GrPlot(3,92,32);
  GrPlot(3,108,32);
  getch();
  RESUME();
  getch();
  SUSPEND();
  getch();
  RESUME();
  getch();
  SUSPEND();
  GrEllipseArc(300,200,50,50,10,10,GR_ARC_STYLE_CLOSE2,GrWhite());
  getch();
  RESUME();
  getch();
  SUSPEND();
  getch();
  RESUME();
  getch();
  SUSPEND();
  getch();
  GrClearScreen(GrBlack());
  GrFilledEllipse(300,200,50,50,GrAllocColor(0,0,255));
  getch();
  RESUME();
  getch();
  SUSPEND();
  getch();
  GrEllipseArc(300,200,50,50,10,10,GR_ARC_STYLE_CLOSE2,GrWhite());
  GrFilledEllipse(300,200,50,50,GrAllocColor(0,0,255));
  GrFilledEllipse(300,200,50,50,GrWhite());
  getch();
  RESUME();
  getch();
  SUSPEND();
  GrTextXY(0,0,"This is a sample text",GrWhite(),GrNOCOLOR);
  //GrEllipseArc(300,80,50,50,10,10,GR_ARC_STYLE_CLOSE2,GrWhite());
  getch();
  RESUME();
  getch();
  SUSPEND();
  getch();
  textmode(3);
  return 0;
}
#endif // GRX20

#ifdef TEXTMODES

#define TxtMode(var) asm("int $0x10": : "a"(var))

int main()
{
  int i;
  char Buf[4];
  INIT();
  TxtMode(3);
  textattr(0x7A);
  cprintf("Screen to restore, 80x25 color\n");
  getch();
  RESUME();
  getch();
  SUSPEND();
  getch();
  RESUME();
  getch();
  SUSPEND();
  TxtMode(0);
  textattr(0x8A);
  cprintf("Screen to restore, 40x25 color\n");
  getch();
  RESUME();
  getch();
  SUSPEND();
  getch();
  RESUME();
  getch();
  SUSPEND();
  TxtMode(3);
  _set_screen_lines(50);
  textattr(0x7B);
  gotoxy(10,10);
  cprintf("Screen to restore, 80x50 color\n");
  getch();
  RESUME();
  getch();
  SUSPEND();
  getch();
  RESUME();
  getch();
  SUSPEND();
  _set_screen_lines(43);
  textattr(0x7C);
  gotoxy(30,30);
  cprintf("Screen to restore, 80x43 color\n");
  getch();
  RESUME();
  getch();
  SUSPEND();
  getch();
  RESUME();
  getch();
  SUSPEND();
  textmode(7);
  gotoxy(30,10);
  highvideo();
  cprintf("Screen to restore, 80x25 b&w\n");
  getch();
  RESUME();
  getch();
  SUSPEND();
  getch();
  RESUME();
  getch();
  SUSPEND();
  getch();
  textmode(3);
  return 0;
}
#endif // TEXTMODES

#endif

#endif // DJGPP
