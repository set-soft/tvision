/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.

 *
 *
 */

#ifdef __DJGPP__

#define Uses_TScreen
#define Uses_TEvent
#include <tv.h>

#include <dos.h>
#include <malloc.h>
#include <go32.h>
#include <dpmi.h>
#include <sys/farptr.h>
#include <sys/movedata.h>
#define REGS __dpmi_regs

#include <conio.h>
#include <pc.h>
#include <stdio.h>
#include <sys/mono.h>

ushort TScreen::startupMode = 0xFFFF;
ushort TScreen::startupCursor = 0;
ushort TScreen::screenMode = 0xFFFF;
uchar TScreen::screenWidth = 0;
uchar TScreen::screenHeight = 0;
uchar TDisplay::Page = 0;
Boolean TScreen::hiResScreen = False;
Boolean TScreen::checkSnow = True;
long TScreen::screenBuffer = -1;
ushort TScreen::cursorLines = 0;

extern REGS r_display;

#define r (r_display)
#define AL (r.h.al)
#define BL (r.h.bl)
#define DL (r.h.dl)
#define AH (r.h.ah)
#define BH (r.h.bh)
#define DH (r.h.dh)
#define AX (r.x.ax)
#define BX (r.x.bx)
#define CX (r.x.cx)
#define DX (r.x.dx)

int TScreen_suspended = 1;

void SaveScreen();
void RestoreScreen();
void ScreenUpdate();
extern ushort user_mode;

static inline
unsigned char I(unsigned char i)
{
  outportb(0x3b4,i);
  return inportb(0x3b5);
}

static inline
void O(unsigned char i,unsigned char b)
{
  outportb(0x3b4,i);
  outportb(0x3b5,b);
}

void TScreen::GetCursor(int &x,int &y)
{
  if (dual_display)
  {
  }
  else
  {
    TDisplay::GetCursor(x,y);
  }
}

void TScreen::SetCursor(int x,int y)
{
  if (dual_display || screenMode == 7)
  {
    unsigned short loc = y*80+x;
    O(0x0e,loc >> 8);
    O(0x0f,loc & 0xff);
  }
  else
  {
    TDisplay::SetCursor(x,y);
  }
}

static int was_blink = 0;
void setBlinkState();
void setIntenseState();
int getBlinkState();

TScreen::TScreen()
{
  user_mode = screenMode = startupMode = getCrtMode();
  TScreen_suspended = 1;
  resume();
}

void TScreen::resume()
{
  if (!TScreen_suspended) return;
  if (!dual_display)
  {
    SaveScreen();
    if (screenMode == 0xffff)
      screenMode = getCrtMode();
    if (screenMode != user_mode)
      setCrtMode( screenMode );
    if (was_blink)
      setBlinkState();
    else
      setIntenseState();
  }
  else
  {
    extern int emulate_mouse;
    emulate_mouse = 1;
  }
  setCrtData();
  TScreen_suspended = 0;
}

TScreen::~TScreen()
{
  suspend();
//  setCrtMode(startupMode);
//  textmode(startupMode);
//  if (startupMode == user_mode)
//    ScreenUpdate();
}

void TScreen::suspend()
{
  if (TScreen_suspended) return;
  if (!dual_display)
  {
    was_blink = getBlinkState();
    RestoreScreen();
  }
  TScreen_suspended = 1;
}

ushort TScreen::fixCrtMode( ushort mode )
{
  return mode;
}

void TScreen::setCrtData()
{
  if (dual_display)
  {
    screenMode = 7;
    screenWidth = 80;
    screenHeight = 25;
    cursorLines = 0x0b0c;
  }
  else
  {
    screenMode = getCrtMode();
    screenWidth = getCols();
    screenHeight = getRows();
    hiResScreen = Boolean(screenHeight > 25);
    if (screenMode == 7)
      cursorLines = 0x0b0c;
    else
      cursorLines = 0x0607;
    setCursorType( 0x2000 );
  }
}

void TScreen::clearScreen()
{
  if (dual_display)
    _mono_clear();
  else
    TDisplay::clearScreen( screenWidth, screenHeight );
}

void TScreen::setVideoMode( ushort mode )
{
    setCrtMode( fixCrtMode( mode ) );
    setCrtData();
}

void TScreen::setVideoMode( char *mode )
{
    setCrtMode( mode );
    setCrtData();
}

void TScreen::setCursorType(ushort ct)
{
  if (dual_display || screenMode == 7)
  {
    if (ct == 0x2000) // cursor off
    {
      O(0x0a,0x01);
      O(0x0b,0x00);
    }
    else
    {
      O(0x0a,ct >> 8);
      O(0x0b,ct & 0xff);
    }
  }
  else
  {
    TDisplay::setCursorType(ct);
  }
}

ushort TScreen::getCursorType()
{
  if (dual_display || screenMode == 7)
  {
    unsigned short ct;
    ct = (I(0x0a) << 8) | I(0x0b);
    if (!ct) ct = 0x2000;
    return ct;
  }
  else
  {
    return TDisplay::getCursorType();
  }
}

ushort TScreen::getRows()
{
  return dual_display ? 25 : TDisplay::getRows();
}

ushort TScreen::getCols()
{
  return dual_display ? 80 : TDisplay::getCols();
}

int slow_screen = 0;

ushort TScreen::getCharacter(unsigned offset)
{
  return _farpeekw(_dos_ds,(dual_display ? 0xb0000 : ScreenPrimary)+offset*2);
}

void TScreen::getCharacter(unsigned offset,ushort *buf,unsigned count)
{
  if (slow_screen)
  {
    _farsetsel(_dos_ds);
    int ofs = dual_display ? 0xb0000 : ScreenPrimary+offset*2;
    while (count--)
    {
      *buf++ = _farnspeekw(ofs);
      ofs += 2;
    }
  }
  else
  {
    movedata(_dos_ds,(dual_display ? 0xb0000 : ScreenPrimary)+offset*2,
             _my_ds(),(int)buf,count*2);
  }
}

void TScreen::setCharacter(unsigned offset,ushort value)
{
  _farpokew(_dos_ds,(dual_display ? 0xb0000 : ScreenPrimary)+offset*2,value);
}

void TScreen::setCharacter(unsigned offset,ushort *values,unsigned count)
{
  if (slow_screen)
  {
    _farsetsel(_dos_ds);
    int ofs = dual_display ? 0xb0000 : ScreenPrimary+offset*2;
    while (count--)
    {
      _farnspokew(ofs,*values++);
      ofs += 2;
    }
  }
  else
  {
    movedata(_my_ds(),(int)values,_dos_ds,
             (dual_display ? 0xb0000 : ScreenPrimary)+offset*2,count*2);
  }
}

#endif // __DJGPP__

