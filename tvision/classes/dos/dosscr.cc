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
#define Uses_TVCodePage
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

#define GET_DOS_VERSION            0x3000
#define GET_GLOBAL_CODE_PAGE_TABLE 0x6601

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

 // Set the code page
 int dosCodePage=437; // United States code page, for all versions before 3.3
 // get DOS version number, in the form of a normal number
 AX=GET_DOS_VERSION;
 dosInt();
 unsigned ver=AH | ((AL & 0xff)<<8);
 if (ver>=0x31E)
   {
    AX=GET_GLOBAL_CODE_PAGE_TABLE;
    dosInt();
    dosCodePage=BX;
   }
 codePage=new TVCodePage(dosCodePage);

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

#define IDENTIFY_WinOldAp_VERSION 0x1700
#define OPEN_CLIPBOARD            0x1701
#define EMPTY_CLIPBOARD           0x1702
#define SET_CLIPBOARD_DATA        0x1703
#define GET_CLIPBOARD_DATA_SIZE   0x1704
#define GET_CLIPBOARD_DATA        0x1705
#define CLOSE_CLIPBOARD           0x1708

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

int TVDOSClipboard::Init(void)
{
 AX=IDENTIFY_WinOldAp_VERSION;
 MultiplexInt();
 Version=AX;
 isValid=AX!=IDENTIFY_WinOldAp_VERSION;
 if (!isValid)
    TVOSClipboard::error=WINOLDAP_NoPresent;
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
    TVOSClipboard::error=WINOLDAP_TooBig;
    return 0;
   }
 AH=0x48;
 BX=(size>>4)+(size & 0xF ? 1 : 0);
 MultiplexInt();
 if (r.x.flags & 1)
   {
    TVOSClipboard::error=WINOLDAP_TooBig;
    return 0;
   }
 *BaseAddress=AX<<4;
 return 1;
}

void TVDOSClipboard::FreeDOSMem(unsigned long Address)
{
 #ifdef USE_TB
 if (Address==__tb)
    return;
 #endif
 AH=0x49;
 ES=Address>>4;
 TDisplayDOS::dosInt();
}

int TVDOSClipboard::copy(int id, const char *buffer, unsigned len)
{
 if (!isValid || id!=0) return 0;

 unsigned long dataoff;
 char cleaner[32];
 int winLen;
 int ret=1;

 AX=OPEN_CLIPBOARD;
 MultiplexInt();
 if (AX==0)
   {
    TVOSClipboard::error=WINOLDAP_ClpInUse;
    return 0;
   }
 // Erase the current contents of the clipboard
 AX=EMPTY_CLIPBOARD;
 MultiplexInt();
 winLen=((len+1)+0x1F) & ~0x1F;
 memset(cleaner,0,32);
 if (AllocateDOSMem(winLen,&dataoff))
   {
    dosmemput(buffer,len,dataoff);
    dosmemput(cleaner,winLen-len,dataoff+len);
    AX=SET_CLIPBOARD_DATA;
    DX=7; // OEM text
    BX=dataoff & 0x0f;
    ES=(dataoff>>4) & 0xffff;
    SI=winLen>>16;
    CX=winLen & 0xffff;
    MultiplexInt();
    FreeDOSMem(dataoff);
    if (AX==0)
      {
       TVOSClipboard::error=WINOLDAP_WinErr;
       ret=0;
      }
   }
 AX=CLOSE_CLIPBOARD;
 MultiplexInt();
 return ret;
}

char *TVDOSClipboard::paste(int id, unsigned &len)
{
 if (!isValid || id!=0) return NULL;

 char *p=NULL;
 unsigned long BaseAddress;
 unsigned long size;

 AX=OPEN_CLIPBOARD;
 MultiplexInt();
 if (AX==0)
   {
    TVOSClipboard::error=WINOLDAP_ClpInUse;
    return NULL;
   }
 AX=GET_CLIPBOARD_DATA_SIZE;
 DX=1;
 MultiplexInt();
 size=AX+(DX<<16);
 if (size)
   {
    if (AllocateDOSMem(size,&BaseAddress))
      {
       p=new char[size];
       if (p)
         {
          DX=1;
          BX=BaseAddress & 0x0f;
          ES=(BaseAddress>>4) & 0xffff;
          AX=GET_CLIPBOARD_DATA;
          MultiplexInt();
          dosmemget(BaseAddress,size,p);
          len=strlen(p);
         }
       else
          TVOSClipboard::error=WINOLDAP_Memory;
       FreeDOSMem(BaseAddress);
      }
   }
 else
   {
    p=new char[1];
    *p=0;
   }
 AX=CLOSE_CLIPBOARD;
 MultiplexInt();
 return p;
}

#else // DJGPP

#include <tv/dos/screen.h>
#include <tv/dos/key.h>
#include <tv/dos/mouse.h>

#endif // else DJGPP

