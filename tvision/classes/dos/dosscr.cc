/**[txh]********************************************************************

  DOS Screen (TScreenDOS) functions.

  Copyright (c) 1996-1998 by Robert H”hne.
  Copyright (c) 1998-2002 by Salvador E. Tropea (SET)

  Description:
  This module implements the low level DOS screen access.
  
***************************************************************************/

#include <tv/configtv.h>

#ifdef TVCompf_djgpp

#include <conio.h>

#define Uses_TScreen
#define Uses_TEvent
#define Uses_TGKey
#include <tv.h>

#include <dos.h>
#include <malloc.h>
#include <go32.h>
#include <dpmi.h>
#include <sys/farptr.h>
#include <sys/movedata.h>

#include <pc.h>
#include <stdio.h>
#include <sys/mono.h>

#define TSCREEN_DEFINE_REGISTERS
#include <tv/dos/screen.h>
#include <tv/dos/key.h>
#include <tv/dos/mouse.h>

int TScreenDOS::wasBlink=0;
int TScreenDOS::slowScreen=0;

const unsigned mdaBaseAddress=0xB0000;

// Extern support functions
extern void SaveScreen();
extern void SaveScreenReleaseMemory();
extern void RestoreScreen();
extern void ScreenUpdate();
extern ushort user_mode;

void setBlinkState();
void setIntenseState();
int  getBlinkState();

TScreen *TV_DOSDriverCheck()
{
 TScreenDOS *drv=new TScreenDOS();
 if (!TScreen::initialized)
   {
    delete drv;
    return 0;
   }
 return drv;
}

TScreenDOS::TScreenDOS()
{
 // Currently initialization never fails
 initialized=1;

 TDisplayDOS::Init();

 TScreen::Resume=Resume;
 TScreen::Suspend=Suspend;
 TScreen::setCrtData=setCrtData;
 TScreen::clearScreen=clearScreen;
 TScreen::getCharacter=getCharacter;
 TScreen::getCharacters=getCharacters;
 TScreen::setCharacter=setCharacter;
 TScreen::setCharacters=setCharacters;
 TScreen::System=System;

 THWMouseDOS::Init();
 TGKeyDOS::Init();

 flags0=CodePageVar | CanSetPalette | CanReadPalette | CursorShapes | UseScreenSaver;
 user_mode=screenMode=startupMode=getCrtMode();
 SaveScreen();
 setCrtData();
 // We use the video memory as buffer, not a malloced buffer
 screenBuffer=(uint16 *)-1;
 suspended=0;
}

void TScreenDOS::Resume()
{
 if (!dual_display)
   {
    GetDisPaletteColors(0,16,OriginalPalette);
    SaveScreen();
    //if (screenMode == 0xffff)
    //  screenMode = getCrtMode();
    if (screenMode!=user_mode)
       setCrtMode(screenMode);
    if (wasBlink)
       setBlinkState();
    else
       setIntenseState();
    SetDisPaletteColors(0,16,ActualPalette);
   }
 else
   {
    THWMouseDOS::setEmulation(1);
   }
 setCrtData();
}

TScreenDOS::~TScreenDOS()
{
 SaveScreenReleaseMemory();
 THWMouseDOS::DeInit();
//  setCrtMode(startupMode);
//  textmode(startupMode);
//  if (startupMode == user_mode)
//    ScreenUpdate();
}

void TScreenDOS::Suspend()
{
 if (!dual_display)
   {
    wasBlink=getBlinkState();
    RestoreScreen();
    SetDisPaletteColors(0,16,OriginalPalette);
   }
}

void TScreenDOS::setCrtData()
{
 if (dual_display)
   {
    screenMode  =7;
    screenWidth =80;
    screenHeight=25;
    cursorLines =0x0b0c;
   }
 else
   {
    screenMode  =getCrtMode();
    screenWidth =getCols();
    screenHeight=getRows();
    hiResScreen =Boolean(screenHeight>25);
    if (screenMode==7)
       cursorLines=0x0b0c; // ?
    else
       cursorLines=getCursorType();
    setCursorType(0);
   }
}

void TScreenDOS::clearScreen()
{
 if (dual_display)
    _mono_clear(); // djgpp's libc
 else
    TDisplay::clearScreen(screenWidth,screenHeight);
}

ushort TScreenDOS::getCharacter(unsigned offset)
{
 return _farpeekw(_dos_ds,(dual_display ? mdaBaseAddress : ScreenPrimary)+offset*2);
}

void TScreenDOS::getCharacters(unsigned offset,ushort *buf,unsigned count)
{
 if (slowScreen)
   {
    _farsetsel(_dos_ds);
    int ofs=(dual_display ? mdaBaseAddress : ScreenPrimary)+offset*2;
    while (count--)
      {
       *buf++=_farnspeekw(ofs);
       ofs+=2;
      }
   }
 else
   {
    movedata(_dos_ds,(dual_display ? mdaBaseAddress : ScreenPrimary)+offset*2,
             _my_ds(),(int)buf,count*2);
   }
}

void TScreenDOS::setCharacter(unsigned offset,ushort value)
{
 _farpokew(_dos_ds,(dual_display ? mdaBaseAddress : ScreenPrimary)+offset*2,value);
}

void TScreenDOS::setCharacters(unsigned offset,ushort *values,unsigned count)
{
 if (slowScreen)
   {
    _farsetsel(_dos_ds);
    int ofs = dual_display ? mdaBaseAddress : ScreenPrimary+offset*2;
    while (count--)
      {
       _farnspokew(ofs,*values++);
       ofs += 2;
      }
   }
 else
   {
    movedata(_my_ds(),(int)values,_dos_ds,
             (dual_display ? mdaBaseAddress : ScreenPrimary)+offset*2,count*2);
   }
}

int TScreenDOS::System(const char *command, pid_t *pidChild)
{
 // fork mechanism not available
 if (pidChild)
    *pidChild=0;
 return system(command);
}

#endif // DJGPP

