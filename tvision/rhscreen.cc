/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#ifdef __DJGPP__
#define Uses_TScreen
#include <tv.h>

#include <sys/types.h>
#include <sys/movedata.h>
#include <sys/farptr.h>
#include <go32.h>
#include <dpmi.h>
#include <conio.h>
#include <pc.h>
#include <stdlib.h>
#include <stdio.h>

extern ushort user_mode;
int blink_use_bios = 0;

static int user_cursor_x;
static int user_cursor_y;
static void *user_screen = NULL;
static int user_screen_size = 0;
static int user_screen_len;
static ushort user_cursor;
static int user_blink = 1;
void rh_save_vga_state();
void rh_restore_vga_state();

int getBlinkState(void)
{
 int retval;
 if (TScreen::screenMode == 7) return 1;
 if (blink_use_bios)
 {
   retval = _farpeekb(_dos_ds,0x465) & 0x20;
 }
 else
 {
   inportb(0x3DA);
   outportb(0x3C0,0x30);
   retval = inportb(0x3C1) & 0x8;
 }
 return retval;
}

void setBlinkState(void)
{
 if (TScreen::screenMode == 7) return;
 if (getBlinkState()) return;
 if (blink_use_bios)
 {
   blinkvideo();
 }
 else
 {
   char val;
   inportb(0x3DA);
   outportb(0x3C0,0x30);
   val=inportb(0x3C1);
   outportb(0x3C0,val | 0x8);
 }
}

void setIntenseState(void)
{
 if (TScreen::screenMode == 7) return;
 if (!getBlinkState()) return;
 if (blink_use_bios)
 {
   intensevideo();
 }
 else
 {
   char val;
   inportb(0x3DA);
   outportb(0x3C0,0x30);
   val=inportb(0x3C1);
   outportb(0x3C0,val & 0xF7);
 }
}

#define SCREEN_BASE (_farpeekb(_dos_ds, 0x449) == 7 ? 0xB0000 : 0xB8000)

static
void ScreenUpdate()
{
  movedata(_my_ds(),(int)user_screen,_dos_ds,SCREEN_BASE,user_screen_len);
}


static
void ScreenRetrieve()
{
  movedata(_dos_ds,SCREEN_BASE,_my_ds(),(int)user_screen,user_screen_len);
}

void rh_save_vesa();
void rh_restore_vesa();
int rh_is_vesa_mode();
void rh_check_for_vesa();
void rh_save_vga();
void rh_restore_vga();
int rh_is_vga_mode();

extern int rh_vesa_supported;
static int vesa_checked = 0;

static int vesa_mode;
static int vga_mode;

static unsigned char palette[3*256];
int save_text_palette = 0;

static
void save_palette()
{
  int i;
  outportb(0x3c7,0);
  for (i=0;i<256;i++)
  {
    palette[i*3+0] = inportb(0x3c9);
    palette[i*3+1] = inportb(0x3c9);
    palette[i*3+2] = inportb(0x3c9);
  }
}

static
void restore_palette()
{
  int i;
  outportb(0x3c8,0);
  for (i=0;i<256;i++)
  {
    outportb(0x3c9,palette[i*3+0]);
    outportb(0x3c9,palette[i*3+1]);
    outportb(0x3c9,palette[i*3+2]);
  }
}

void rh_SaveVideo(int cols,int rows)
{
  int screensize;
  user_mode = 0;
  vesa_mode = 0;
  if (!vesa_checked)
  {
    rh_check_for_vesa();
    vesa_checked = 1;
  }
  if (rh_vesa_supported && (vesa_mode = rh_is_vesa_mode()))
  {
    rh_save_vesa();
  }
  else
  if ((vga_mode = rh_is_vga_mode()))
  {
    rh_save_vga();
  }
  else
  {
    screensize = cols*rows*2;
    TDisplay::GetCursor(user_cursor_x,user_cursor_y);
    user_cursor = TScreen::getCursorType();
    user_mode = TScreen::getCrtMode();
    user_blink = getBlinkState();
    if (user_mode != 7)
    {
      rh_save_vga_state();
      if (save_text_palette)
        save_palette();
    }
    if (screensize > user_screen_size)
    {
      user_screen = realloc(user_screen,screensize);
      user_screen_size = screensize;
    }
    user_screen_len = screensize;
    ScreenRetrieve();
  }
}

// SET: Added to release the memory of this module.
void rh_SaveVideoReleaseMemory(void)
{
 free(user_screen);
}


void rh_RestoreVideo()
{
  if (vesa_mode)
  {
    rh_restore_vesa();
  }
  else
  if (vga_mode)
  {
    rh_restore_vga();
  }
  else
  {
    if (user_mode != 7)
      rh_restore_vga_state();
    TFont::UseDefaultFontsNextTime=1;
    if (TScreen::screenMode != user_mode)
      TDisplay::setCrtMode(user_mode);
    else
      TDisplay::RestoreDefaultFont();
    if (user_blink)
      setBlinkState();
    else
      setIntenseState();
    ScreenUpdate();
    TDisplay::setCursorType(user_cursor);
    TDisplay::SetCursor(user_cursor_x,user_cursor_y);
    if (user_mode != 7 && save_text_palette)
      restore_palette();
    if (user_mode != 7)
      rh_restore_vga_state();
  }
}

#else

// SET: Linux versions moved to TScreen and thinking about integrate it to
// TDisplay.
#endif
