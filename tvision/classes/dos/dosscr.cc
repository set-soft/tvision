/**[txh]********************************************************************

  DOS Screen (TScreenDOS) functions.

  Copyright (c) 1998-2002 by Salvador E. Tropea (SET)
  Contains code Copyright (c) 1996-1998 by Robert H”hne.

  Description:
  This module implements the low level DOS screen access.
  
***************************************************************************/

#include <tv/configtv.h>

#ifdef TVCompf_djgpp
 #include <conio.h>
#endif

#define Uses_string
#define Uses_TScreen
#define Uses_TEvent
#define Uses_TGKey
#define Uses_TVOSClipboard
#include <tv.h>

// I delay the check to generate as much dependencies as possible
#ifdef TVCompf_djgpp

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

 TVDOSClipboard::Init();
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

/*****************************************************************************
  Windows clipboard implementation using WinOldAp API.
*****************************************************************************/

#define USE_TB

// Numbers for the errors
#define WINOLDAP_NoPresent 1
#define WINOLDAP_ClpInUse  2
#define WINOLDAP_TooBig    3
#define WINOLDAP_Memory    4
#define WINOLDAP_WinErr    5

#define WINOLDAP_Errors    5

// Strings for the errors
const char *TVDOSClipboard::dosNameError[]=
{
 NULL,
 __("Windows not present"),
 __("Clipboard in use by other application"),
 __("Clipboard too big for the transfer buffer"),
 __("Not enough memory"),
 __("Windows reports error")
};

int TVDOSClipboard::isValid=0;
int TVDOSClipboard::Version;
int TVDOSClipboard::Error=0;

int TVDOSClipboard::Init(void)
{
 __dpmi_regs r;

 r.x.ax=0x1700;
 __dpmi_int(0x2F,&r);
 Version=r.x.ax;
 isValid=r.x.ax!=0x1700;
 if (!isValid)
    Error=WINOLDAP_NoPresent;
 else
   {
    TVOSClipboard::copy=copy;
    TVOSClipboard::paste=paste;
    TVOSClipboard::available=1; // We have 1 clipboard
    TVOSClipboard::name="Windows";
    TVOSClipboard::errors=WINOLDAP_Errors;
    TVOSClipboard::nameErrors=dosNameError;
   }
 return isValid;
}

int TVDOSClipboard::AllocateDOSMem(unsigned long size,unsigned long *BaseAddress)
{
 __dpmi_regs r;
 #ifdef USE_TB
 unsigned long tbsize=_go32_info_block.size_of_transfer_buffer;

 if (size<=tbsize)
   {
    *BaseAddress=__tb;
    return 1;
   }
 #endif
 if (size>0x100000)
   {
    Error=WINOLDAP_TooBig;
    return 0;
   }
 r.h.ah=0x48;
 r.x.bx=(size>>4)+(size & 0xF ? 1 : 0);
 __dpmi_int(0x21,&r);
 if (r.x.flags & 1)
   {
    Error=WINOLDAP_TooBig;
    return 0;
   }
 *BaseAddress=r.x.ax<<4;
 return 1;
}

void TVDOSClipboard::FreeDOSMem(unsigned long Address)
{
 __dpmi_regs r;
 #ifdef USE_TB
 if (Address==__tb)
    return;
 #endif
 r.h.ah=0x49;
 r.x.es=Address>>4;
 __dpmi_int(0x21,&r);
}

int TVDOSClipboard::copy(int id, const char *buffer, unsigned len)
{
 if (!isValid || id!=0) return 0;

 __dpmi_regs r;
 unsigned long dataoff;
 char cleaner[32];
 int winLen;

 r.x.ax=0x1701;
 __dpmi_int(0x2F,&r);
 if (r.x.ax==0)
   {
    Error=WINOLDAP_ClpInUse;
    return 0;
   }
 // Erase the current contents of the clipboard
 r.x.ax=0x1702;
 __dpmi_int(0x2F,&r);
 winLen=((len+1)+0x1F) & ~0x1F;
 memset(cleaner,0,32);
 if (AllocateDOSMem(winLen,&dataoff))
   {
    dosmemput(buffer,len,dataoff);
    dosmemput(cleaner,winLen-len,dataoff+len);
    r.x.ax=0x1703;
    r.x.dx=7; // OEM text
    r.x.bx=dataoff & 0x0f;
    r.x.es=(dataoff>>4) & 0xffff;
    r.x.si=winLen>>16;
    r.x.cx=winLen & 0xffff;
    __dpmi_int(0x2F,&r);
    FreeDOSMem(dataoff);
    if (r.x.ax==0)
      {
       Error=WINOLDAP_WinErr;
       r.x.ax=0x1708;
       __dpmi_int(0x2F,&r);
       return 0;
      }
    r.x.ax=0x1708;
    __dpmi_int(0x2F,&r);
   }
 return 1;
}

char *TVDOSClipboard::paste(int id, unsigned &len)
{
 if (!isValid || id!=0) return NULL;

 char *p=NULL;
 unsigned long BaseAddress;
 unsigned long size;
 __dpmi_regs r;

 r.x.ax=0x1701;
 __dpmi_int(0x2F,&r);
 if (r.x.ax==0)
   {
    Error=WINOLDAP_ClpInUse;
    return NULL;
   }
 r.x.ax=0x1704;
 r.x.dx=1;
 __dpmi_int(0x2F,&r);
 size=r.x.ax+(r.x.dx<<16);
 if (size)
   {
    if (AllocateDOSMem(size,&BaseAddress))
      {
       p=new char[size];
       if (p)
         {
          r.x.dx=1;
          r.x.bx=BaseAddress & 0x0f;
          r.x.es=(BaseAddress>>4) & 0xffff;
          r.x.ax=0x1705;
          __dpmi_int(0x2F,&r);
          dosmemget(BaseAddress,size,p);
          len=strlen(p);
         }
       else
          Error=WINOLDAP_Memory;
       FreeDOSMem(BaseAddress);
      }
   }
 else
   {
    p=new char[1];
    *p=0;
   }
 r.x.ax=0x1708;
 __dpmi_int(0x2F,&r);
 return p;
}

#else // DJGPP

#include <tv/dos/screen.h>
#include <tv/dos/key.h>
#include <tv/dos/mouse.h>

#endif // else DJGPP

