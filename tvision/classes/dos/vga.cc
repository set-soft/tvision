/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <tv/configtv.h>
#include "vgastate.h"

#ifdef TVCompf_djgpp

/* 
   This file is used to save/restore the graphics screen when a
   VGA mode is set.
   I have tested it with all the modes, which are available on my
   et4000 card.

   320x200x16
   640x200x16
   640x350x16
   640x480x16
   320x200x256
   320x240x256
   320x400x256
   360x480x256

The function save_vga assumes, that is_vga_mode called before and
was successfull. There is no other check for validity.
And the function restore_vga assumes that save_vga was called
before.
*/

#include <stdlib.h>
#include <sys/movedata.h>
#include <sys/farptr.h>
#include <dpmi.h>
#include <go32.h>

#define SIZE 0x8000

extern int user_mode;

static void *buffer = NULL;
static int buffer_size = 0;
static int saved_size;
static int win_size;
static unsigned win_seg;
static int is_plane_mode = 0;
static int mode_x;

// For BIOS save state
static int vga_state_segment = -1;
static int vga_state_selector;
// For low level save state
static void *vgaState=0;

char useBIOS_VGA_State=1;
char saveVGA_State=0;

void rh_save_vga_state()
{
 if (!saveVGA_State) return;
 if (useBIOS_VGA_State)
   {// SET: It fails for some Matrox boards.
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
 else
   {
    if (!vgaState)
       vgaState=malloc(VGAStateGetBufferSize());
    VGAStateSave(vgaState);
   }
}

void rh_restore_vga_state()
{
 if (!saveVGA_State) return;
 if (useBIOS_VGA_State)
   {// SET: It fails for some Matrox boards.
    __dpmi_regs r;
    if (vga_state_segment == -1) return;
    r.x.ax = 0x1c02;
    r.x.es = vga_state_segment;
    r.x.bx = 0;
    r.x.cx = 0x0007;
    __dpmi_int(0x10,&r);
   }
 else
    VGAStateLoad(vgaState);
}

/*
 * Sequencer port and frequently used register indices
 */
#define VGA_SEQUENCER_PORT	0x3c4
#define VGA_SEQUENCER_DATA	0x3c5

#define VGA_WRT_PLANE_ENB_REG	2
#define VGA_MEMORY_MODE_REG     4

/*
 * color plane operations
 */
#define VGA_FUNC_SET	0
#define VGA_FUNC_AND	8
#define VGA_FUNC_OR	16
#define VGA_FUNC_XOR	24

/*
 * Graphics controller port and frequently used registers
 */
#define VGA_GR_CTRL_PORT	0x3ce
#define VGA_GR_CTRL_DATA	0x3cf

#define VGA_SET_RESET_REG	0
#define VGA_SET_RESET_ENB_REG	1
#define VGA_COLOR_COMP_REG	2
#define VGA_ROT_FN_SEL_REG	3
#define VGA_RD_PLANE_SEL_REG	4
#define VGA_MODE_REG		5
#define VGA_MISC_REG		6
#define VGA_COLOR_DONTC_REG	7
#define VGA_BIT_MASK_REG	8

#include <pc.h>

int rh_is_vga_mode()
{
  __dpmi_regs r;

  r.x.ax = 0x0f00;
  __dpmi_int(0x10,&r);

  if (r.h.al != 0x12 &&
      r.h.al != 0x13 &&
      r.h.al != 0x0d &&
      r.h.al != 0x0e &&
      r.h.al != 0x10
     ) return 0;

  user_mode = r.h.al;

  mode_x = 0;

  if (user_mode == 0x12 ||
      user_mode == 0x0d ||
      user_mode == 0x0e ||
      user_mode == 0x10
     )
    is_plane_mode = 1;
  else
  {
    unsigned char b;
    outportb(VGA_SEQUENCER_PORT,VGA_MEMORY_MODE_REG);
    b = inportb(VGA_SEQUENCER_DATA);
    is_plane_mode = b & 0x08 ? 0 : 1;
    mode_x = is_plane_mode;
  }

  return 1;
}

void rh_save_vga()
{
  int offset = 0;
  rh_save_vga_state();
  win_seg = 0xa0000;

  win_size = 64 * 1024;

  saved_size = SIZE;

  if (saved_size < win_size)
    win_size = saved_size;

  if (saved_size > buffer_size)
  {
    buffer = realloc(buffer,saved_size);
    buffer_size = saved_size;
  }

  while (offset < saved_size)
  {
    if (is_plane_mode)
    {
      int i;
      int bs = win_size / 4;

      int modereg;
      outportb(VGA_GR_CTRL_PORT,VGA_MODE_REG);
      modereg = inportb(VGA_GR_CTRL_DATA) & 0xf0;
      /* now set to read mode 0 */
      outportw(VGA_GR_CTRL_PORT,(modereg << 8) | VGA_MODE_REG);
      for (i=0;i<4;i++)
      {
        /* read from plane 'plane' */
        outportw(VGA_GR_CTRL_PORT,((i << 8) | VGA_RD_PLANE_SEL_REG));

        movedata(_dos_ds,win_seg,_my_ds(),(int)buffer+offset,bs);
        offset += bs;
      }

#if 0
      /* restore the registers */
      rh_restore_vga_state();
#endif
    }
    else
    {
      movedata(_dos_ds,win_seg,_my_ds(),(int)buffer+offset,win_size);
      offset += win_size;
    }
  }
}

__inline__ void
_farnsandl(unsigned long offset,
	 unsigned long value)
{
  __asm__ __volatile__ (".byte 0x64\n"
      "	andl %k0,(%k1)"
      :
      : "ri" (value), "r" (offset));
}

__inline__ void
_farfill_l(unsigned short sel,unsigned long offset,unsigned long count,
	 unsigned long value)
{
  __asm__ __volatile__ (
      "pushw %%es\n"
      "movw %w0,%%es\n"
      "cld\n"
      "rep\n"
      "stosl\n"
      "popw %%es"
      :
      : "rm" (sel), "a" (value), "D" (offset), "c" (count));
}

void rh_restore_vga()
{
  int offset = 0;
  __dpmi_regs r;
  r.x.ax = (user_mode & 0x7f) | 0x80;
  __dpmi_int(0x10,&r);
  rh_restore_vga_state();

  while (offset < saved_size)
  {
    if (is_plane_mode)
    {
      int i,j;
      int bs = win_size / 4;

      outportw(VGA_GR_CTRL_PORT,(VGA_FUNC_SET << 8) | VGA_ROT_FN_SEL_REG);
      /* enable all planes for writing */
      outportw(VGA_SEQUENCER_PORT,((0x0f << 8) | VGA_WRT_PLANE_ENB_REG));
      /* eanable all planes for set/reset */
      outportw(VGA_GR_CTRL_PORT,((0x00 << 8) | VGA_SET_RESET_ENB_REG));
      /* set all planes for reset */
      outportw(VGA_GR_CTRL_PORT,((0x00 << 8) | VGA_SET_RESET_REG));
#if 0
      /* set write mode 3 */
      outportw(VGA_GR_CTRL_PORT,((8+3) << 8) | VGA_MODE_REG);
#endif
      /* clear all */
      if (!mode_x)
      {
        _farfill_l(_dos_ds,win_seg,bs/4,0xffffffffL);
      }
      unsigned long *_l = (unsigned long *)buffer;
      for (i=0;i<4;i++)
      {
#if 1
        /* enable plane 'i' for writing */
        outportw(VGA_SEQUENCER_PORT,(((1 << i) << 8) | VGA_WRT_PLANE_ENB_REG));
#endif
        if (!mode_x)
        {
          /* eanable plane 'i' for set/reset */
          outportw(VGA_GR_CTRL_PORT,(((1 << i) << 8) | VGA_SET_RESET_ENB_REG));
          /* enable plane 'i' for set */
          outportw(VGA_GR_CTRL_PORT,(((1 << i) << 8) | VGA_SET_RESET_REG));
        }
#if 0
        /* set write mode 3 */
        outportw(VGA_GR_CTRL_PORT,((8+3) << 8) | VGA_MODE_REG);
#endif
        if (mode_x)
        {
          movedata(_my_ds(),(int)_l,_dos_ds,win_seg,bs);
          _l += bs/4;
        }
        else
        {
          _farsetsel(_dos_ds);
          for (j=0;j<bs;j+=4)
          {
            _farnsandl(win_seg+j,*_l++);
          }
        }
        offset+=bs;
      }
      rh_restore_vga_state();
    }
    else
    {
      movedata(_my_ds(),(int)buffer+offset,_dos_ds,win_seg,win_size);
      offset += win_size;
    }
  }
}

#ifdef TEST

#include <grx20.h>
#include <stdio.h>
#include <conio.h>
#include <pc.h>

#ifdef USE_TV

#define getch()

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
  void Resume() { resume(); Redraw(); }
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

#else

#define INIT() textmode(3)
#define RESUME() rh_is_vga_mode(); rh_save_vga(); textmode(0x83)
#define SUSPEND() rh_restore_vga()

#endif

int main()
{
  int i;
#ifdef USE_TV
  extern int screen_saving;
  screen_saving = 1;
#endif
  INIT();
  getch();
  GrSetMode(GR_width_height_color_graphics,360,480,256);
  GrClearScreen(GrBlack());
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
  getch();
  GrTextXY(0,0,"This is a sample text",GrWhite(),GrNOCOLOR);
  RESUME();
  getch();
  SUSPEND();
  getch();
  textmode(3);
  return 0;
}
#endif

#endif // DJGPP

