/**[txh]********************************************************************

  DOS Screen (TScreenDOS) functions.

  Copyright (c) 1998-2002 by Salvador E. Tropea (SET)
  Contains code Copyright (c) 1996-1998 by Robert H”hne.

  Some code for handling 8x10 fonts is adapted from djgpp libc:
  Copyright (C) 1995-1999 DJ Delorie

  Description:
  This module implements the low level DOS screen access.

  Configuration variables:
  ScreenWidth
  ScreenHeight
  ScreenPalette
  AppCP
  ScrCP
  InpCP
  LoadSecondaryFont
  ExtProgVideoMode
  VideoMode
  VGABIOSState      0 use registers, 1 use BIOS (default)
  SaveVGAState      0 don't try to save/restore VGA chip state,
                    1 try to save it using the strategy indicated by
                    VGABIOSState (default)
  
***************************************************************************/

#include <tv/configtv.h>

#ifdef TVCompf_djgpp
 #include <conio.h>
#endif

#define Uses_string
#define Uses_unistd
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

#define GET_DOS_VERSION                      0x3000
#define GET_GLOBAL_CODE_PAGE_TABLE           0x6601
#define CHARGEN_SET_BLOCK_SPECIFIER          0x1103
#define CHARGEN_LOAD_USER_SPECIFIED_PATTERNS 0x1110
#define GET_FONT_INFORMATION                 0x1130
// 0x11xx services (Sets the BIOS Rom)
#define FONT_8x8                             0x12
#define FONT_8x14                            0x11
#define FONT_8x16                            0x14
// GET_FONT_INFORMATION values
#define GET_FONT_8x8                         0x03
#define GET_FONT_8x14                        0x02
#define GET_FONT_8x16                        0x06

int   TScreenDOS::wasBlink=0;
int   TScreenDOS::slowScreen=0;
uchar TScreenDOS::primaryFontSet=0,
      TScreenDOS::secondaryFontSet=0,
      TScreenDOS::fontsSuspended=0;
int   TScreenDOS::origCPScr,
      TScreenDOS::origCPApp,
      TScreenDOS::origCPInp;
int   TScreenDOS::fontSeg=-1;
TScreenFont256 TScreenDOS::appFonts[2]={{0,0,NULL},{0,0,NULL}};

const unsigned mdaBaseAddress=0xB0000;

// Extern support functions
extern void SaveScreen();
extern void SaveScreenReleaseMemory();
extern void RestoreScreen();
extern void ScreenUpdate();
extern ushort user_mode;
extern char useBIOS_VGA_State;
extern char saveVGA_State;

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
 if (dCB) dCB();

 TDisplayDOS::Init();

 TScreen::Resume=Resume;
 TScreen::Suspend=Suspend;
 TScreen::setCrtData=setCrtData;
 TScreen::clearScreen=clearScreen;
 TScreen::getCharacter=getCharacter;
 TScreen::getCharacters=getCharacters;
 TScreen::setCharacter=setCharacter;
 TScreen::setCharacters=setCharacters;
 TScreen::System_p=System;
 TScreen::getFontGeometry=GetFontGeometry;
 TScreen::setFont_p=SetFont;
 TScreen::restoreFonts=RestoreFonts;

 TVDOSClipboard::Init();
 THWMouseDOS::Init();
 TGKeyDOS::Init();

 long aux;
 if (optSearch("VGABIOSState",aux))
    useBIOS_VGA_State=aux;
 if (optSearch("SaveVGAState",aux))
    saveVGA_State=aux;

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
 // Look for user settings
 optSearch("AppCP",forcedAppCP);
 optSearch("ScrCP",forcedScrCP);
 optSearch("InpCP",forcedInpCP);
 // User settings have more priority than detected settings
 codePage=new TVCodePage(forcedAppCP!=-1 ? forcedAppCP : dosCodePage,
                         forcedScrCP!=-1 ? forcedScrCP : dosCodePage,
                         forcedInpCP!=-1 ? forcedInpCP : dosCodePage);
 SetDefaultCodePages(dosCodePage,dosCodePage,dosCodePage);

 flags0=CodePageVar | CanSetPalette | CanReadPalette | CursorShapes | UseScreenSaver |
        CanSetBFont | CanSetSBFont  | CanSetVideoSize;
 user_mode=startupMode=getCrtMode();
 SaveScreen();
 unsigned startScreenWidth=GetCols(), startScreenHeight=GetRows();

 unsigned maxX=startScreenWidth, maxY=startScreenHeight;
 if (optSearch("ScreenWidth",aux))
    maxX=aux;
 if (optSearch("ScreenHeight",aux))
    maxY=aux;
 if (frCB)
   {// Give a chance to the call back.
    primaryFontSet=1;
    appFonts[0].h=0;
    long aux=0;
    if (optSearch("LoadSecondaryFont",aux) && aux)
      {
       secondaryFontSet=1;
       appFonts[1].h=0;
      }
   }
 char *ext=optSearch("ExtProgVideoMode");
 if (optSearch("VideoMode",aux))
   {// if frCB installed it will use the application font
    setCrtMode((ushort)aux);
   }
 else if (ext)
   {
    setCrtModeExt(ext);
    if (frCB)
       SelectFont(charLines,False); // Try loading the application font
   }
 else if (maxX!=startScreenWidth || maxY!=startScreenHeight)
   {// if frCB installed it will use the application font
    setCrtModeRes(maxX,maxY);
   }
 else
   {
    if (frCB)
       SelectFont(charLines,False); // Try loading the application font
   }
 // User requested palette
 if (parseUserPalette())
    setPaletteColors(0,16,UserStartPalette);

 screenMode=getCrtMode();
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
    ResumeFonts();
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
 suspend();                 // First suspend
 SaveScreenReleaseMemory(); // Now we can release the memory
 ReleaseMemFonts();
 THWMouseDOS::DeInit();
}

void TScreenDOS::Suspend()
{
 if (!dual_display)
   {
    wasBlink=getBlinkState();
    SuspendFonts();
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

void TScreenDOS::setCharacter(unsigned offset,uint32 value)
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

int TScreenDOS::System(const char *command, pid_t *pidChild, int in,
                       int out, int err)
{
 // fork mechanism not available
 if (pidChild)
    *pidChild=0;
 // If the caller asks for redirection replace the requested handles
 if (in!=-1)
    dup2(in,STDIN_FILENO);
 if (out!=-1)
    dup2(out,STDOUT_FILENO);
 if (err!=-1)
    dup2(err,STDERR_FILENO);
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

//#define DEBUG_CLIPBOARD
#ifdef DEBUG_CLIPBOARD
 ushort messageBox( const char *msg, ushort aOptions );
 ushort messageBox( ushort aOptions, const char *fmt, ... );
 #define DBGMessage(a) messageBox(a,0x402)
 #define DBGMessage2(a,b) messageBox(0x402,a,b)
#else
 #define DBGMessage(a)
 #define DBGMessage2(a,b)
#endif

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
    DBGMessage("Error opening the clipboard");
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
       DBGMessage("Error copying to clipboard");
       TVOSClipboard::error=WINOLDAP_WinErr;
       ret=0;
      }
   }
 else
   {
    TVOSClipboard::error=WINOLDAP_Memory;
    DBGMessage("Error allocating DOS memory");
    ret=0;
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
    DBGMessage("Error at open clipboard");
    TVOSClipboard::error=WINOLDAP_ClpInUse;
    return NULL;
   }
 AX=GET_CLIPBOARD_DATA_SIZE;
 DX=1;
 MultiplexInt();
 size=AX+(DX<<16);
 DBGMessage2("Size of clipboard %d",size);
 if (size)
   {
    if (AllocateDOSMem(size,&BaseAddress))
      {
       p=new char[size];
       if (p)
         {
          DX=7;
          BX=BaseAddress & 0x0f;
          ES=(BaseAddress>>4) & 0xffff;
          AX=GET_CLIPBOARD_DATA;
          MultiplexInt();
          dosmemget(BaseAddress,size,p);
          len=strlen(p);
          DBGMessage2("Got %d bytes in string",len);
         }
       else
          TVOSClipboard::error=WINOLDAP_Memory;
       FreeDOSMem(BaseAddress);
      }
    else
      {
       TVOSClipboard::error=WINOLDAP_Memory;
       DBGMessage("Error allocating DOS memory");
      }
   }
 else
   {
    p=new char[1];
    *p=0;
    len=0;
   }
 AX=CLOSE_CLIPBOARD;
 MultiplexInt();
 return p;
}

/*****************************************************************************
  Fonts routines
*****************************************************************************/

int TScreenDOS::GetFontGeometry(unsigned &w, unsigned &h)
{
 w=8;
 h=charLines;
 return 1;
}

/**[txh]********************************************************************

  Description:
  That selects what fonts are used for no/intense foreground colors.@p
  The bits of the call are a little tweaked because it's EGA compatible:@*
---VGA---@*
 0,1,4 block selected by characters with attribute bit 3 clear@*
 2,3,5 block selected by characters with attribute bit 3 set@*

***************************************************************************/

void TScreenDOS::SetDualCharacter(int b1, int b2)
{
 int value;
 // No intense foreground font
 value=b1 & 0x3;
 if (b1 & 0x4)
    value|=0x10;
 // intense foreground font
 value|=(b2 & 0x3)<<2;
 if (b2 & 0x4)
    value|=0x20;

 AX=CHARGEN_SET_BLOCK_SPECIFIER;
 BL=value;
 videoInt();
}

void TScreenDOS::SetFontBIOS(int which, unsigned height, uchar *data,
                             int modeRecalculate)
{
 dosmemput(data,256*height,__tb);
 ES=__tb>>4;    /* pass pointer to our font in ES:BP */
 BP=__tb & 0xF;
 DX=0;          /* 1st char: ASCII 0 */
 CX=256;        /* 256 chars */
 BH=height;     /* points per char */
 BL=which;      /* block */
 AX=CHARGEN_LOAD_USER_SPECIFIED_PATTERNS;
 if (!modeRecalculate)
    AL&=0xF;    /* force full mode recalculate, service 0x1100 */
 videoInt();
}

void TScreenDOS::ReleaseMemFonts()
{
 if (appFonts[0].data)
   {
    free(appFonts[0].data);
    appFonts[0].data=NULL;
   }
 if (appFonts[1].data)
   {
    free(appFonts[1].data);
    appFonts[1].data=NULL;
   }
}

int TScreenDOS::MemorizeFont(int which, TScreenFont256 *font)
{
 // Keep a copy of the user font
 appFonts[which].w=font->w;
 appFonts[which].h=font->h;
 if (appFonts[which].data)
    free(appFonts[which].data);
 unsigned size=font->h*256;
 appFonts[which].data=(uchar *)malloc(size);
 if (!appFonts[which].data)
    return 0;
 memcpy(appFonts[which].data,font->data,size);
 return 1;
}

void TScreenDOS::SuspendFonts()
{
 if (fontsSuspended)
    return;
 fontsSuspended=1;
 if (!primaryFontSet && !secondaryFontSet)
    return; // Nothing to do
 DisableDualFont();
 SelectRomFont(charLines,0,0);
}

void TScreenDOS::ResumeFonts()
{
 if (!fontsSuspended)
    return;
 fontsSuspended=0;
 if (!primaryFontSet && !secondaryFontSet)
    return; // Nothing to do
 if (primaryFontSet)
    SetFontBIOS(0,charLines,appFonts[0].data,0);
 if (secondaryFontSet)
   {
    SetFontBIOS(1,charLines,appFonts[1].data,0);
    EnableDualFont();
   }
}

int TScreenDOS::SetFont(int changeP, TScreenFont256 *fontP,
                        int changeS, TScreenFont256 *fontS,
                        int fontCP, int appCP)
{
 if (!changeP && !changeS) return 1;
 // Check for restore fonts
 if (changeP && !fontP && ((!changeS && !secondaryFontSet) || (changeS && !fontS)))
   {
    DisableDualFont();
    SelectRomFont(charLines,0,0);
    ReleaseMemFonts();
    secondaryFontSet=primaryFontSet=0;
    if (fontCP!=-1)
      {
       if (appCP==-1)
          TVCodePage::SetScreenCodePage(fontCP);
       else
          TVCodePage::SetCodePage(appCP,fontCP,-1);
      }
   return 1;
   }
 // Solve the sizes
 unsigned wP=8,hP=charLines,
          wS=8,hS=charLines;
 int newP=changeP && fontP;
 if (newP)
   {
    wP=fontP->w;
    hP=fontP->h;
   }
 int newS=changeS && fontS;
 if (newS)
   {
    wS=fontS->w;
    hS=fontS->h;
   }
 if (wP!=wS || hP!=hS) return 0;
 // Check if the size is in the range
 if (wP!=8 || hP!=charLines)
    return 0;
 // Memorize the new fonts
 if (newP && !MemorizeFont(0,fontP)) return 0;
 if (newS && !MemorizeFont(1,fontS)) return 0;

 // Change the requested fonts
 if (changeP)
   {
    if (!primaryFontSet)
       TVCodePage::GetCodePages(origCPApp,origCPScr,origCPInp);
    if (fontP)
      {
       SetFontBIOS(0,charLines,fontP->data,0);
       primaryFontSet=1;
      }
    else
      {
       SelectRomFont(charLines,0,0);
       primaryFontSet=0;
      }
   }
 if (changeS)
   {
    if (fontS)
      {
       SetFontBIOS(1,charLines,fontS->data,0);
       EnableDualFont();
       secondaryFontSet=1;
      }
    else
      {
       DisableDualFont();
       secondaryFontSet=0;
      }
   }
 if (changeP && fontCP!=-1)
   {
    if (appCP==-1)
       TVCodePage::SetScreenCodePage(fontCP);
    else
       TVCodePage::SetCodePage(appCP,fontCP,-1);
   }
 return 1;
}

void TScreenDOS::RestoreFonts()
{
 if (!primaryFontSet && !secondaryFontSet)
    return; // Protection
 SetFont(1,NULL,1,NULL,origCPScr,origCPApp);
}

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

int TScreenDOS::SelectRomFont(int height, int which, int modeRecalculate)
{
 //UseDefaultFontsNextTime=0;
 switch (height)
   {
    case 8:
         SetRomFonts(FONT_8x8,which,modeRecalculate);
         break;
    case 10:
         if (Load8x10Font(which,modeRecalculate))
            return 1;
         break;
    case 14:
         SetRomFonts(FONT_8x14,which,modeRecalculate);
         break;
    case 16:
         SetRomFonts(FONT_8x16,which,modeRecalculate);
         break;
    default:
         return 1;
   }
 return 0;
}

void TScreenDOS::SetRomFonts(int sizeFont, int which, int modeRecalculate)
{
 if (!modeRecalculate)
    sizeFont&=0xF;
 BL=which;
 AH=0x11;
 AL=sizeFont;
 videoInt();
}


/**[txh]********************************************************************

  Description: 
  Stretch a 8x8 font to the 8x10 character box.  This is required to
use 80x40 mode on a VGA or 80x35 mode on an EGA, because the character
box is 10 lines high, and the ROM BIOS doesn't have an appropriate font.
So we create one from the 8x8 font by adding an extra blank line
from each side.
  
***************************************************************************/

void TScreenDOS::MaybeCreate8x10Font(void)
{
 unsigned char *p;
 unsigned long src, dest, i, j;

 if (fontSeg!=-1)
    return;
 int buf_pm_sel;
 
 /* Allocate buffer in conventional memory. */
 fontSeg=__dpmi_allocate_dos_memory(160,&buf_pm_sel);

 if (fontSeg==-1)
    return;

 /* Get the pointer to the 8x8 font table.  */
 p=(uchar *)malloc(2560); /* 256 chars X 8x10 pixels */
 if (p==(uchar *)0)
   {
    //errno=ENOMEM;
    __dpmi_free_dos_memory(buf_pm_sel);
    fontSeg=-1;
    return;
   }
 BH=GET_FONT_8x8;
 AX=GET_FONT_INFORMATION;
 videoInt();
 src=(((unsigned)ES)<<4)+BP;
 dest=((unsigned)fontSeg)<<4;

 /* Now copy the font to our table, stretching it to 8x10. */
 _farsetsel(_dos_ds);
 for (i=0; i<256; i++)
    {
     /* Fill first extra scan line with zeroes. */
     _farnspokeb(dest++, 0);

     for (j=0; j<8; j++)
        {
         uchar val=_farnspeekb(src++);
         _farnspokeb(dest++,val);
        }

     /* Fill last extra scan line with zeroes. */
     _farnspokeb(dest++,0);
    }
}

/* Load the 8x10 font we created into character generator RAM.  */
int TScreenDOS::Load8x10Font(int which, int modeRecalculate)
{
 MaybeCreate8x10Font();         /* create if needed */
 if (fontSeg==-1)
    return 1;
 ES=fontSeg;              /* pass pointer to our font in ES:BP */
 BP=0;
 DX=0;                    /* 1st char: ASCII 0 */
 CX=256;                  /* 256 chars */
 BH=10;                   /* 10 points per char */
 BL=which;                /* block */
 AX=CHARGEN_LOAD_USER_SPECIFIED_PATTERNS;
 if (!modeRecalculate)
    AL&=0xF;
 videoInt();
 return 0;
}

/**[txh]********************************************************************

  Description:
  Used after setting a video mode to load the user font or the BIOS font.
The force parameter indicates if we must load the BIOS font or if the font
was already loaded.@p
  If a font was set the application this function tries to reuse it. If the
font doesn't match the size used by the video mode calls a call back
requesting a new font. If the routine fails to get a propper font the BIOS
font is restored.
  
***************************************************************************/

void TScreenDOS::SelectFont(unsigned height, Boolean Force)
{
 int fontWasLoaded=primaryFontSet || secondaryFontSet;

 if (primaryFontSet && !fontsSuspended)
   {
    if (height==appFonts[0].h)
       SetFontBIOS(0,height,appFonts[0].data,0);
    else
      {
       int fontOk=0;
       // The one provided by the application isn't suitable
       if (frCB)
         {// Try asking the application to provide a new one
          TScreenFont256 *font=frCB(0,8,height);
          if (font)
            {
             SetFontBIOS(0,height,font->data,0);
             MemorizeFont(0,font);
             fontOk=1;
             DeleteArray(font->data);
             delete font;
            }
         }
       if (!fontOk)
         {
          if (Force)
             SelectRomFont(height,0,1);
          primaryFontSet=0;
         }
      }
   }
 else
   {
    if (Force)
       SelectRomFont(height,0,1);
   }

 if (!fontsSuspended && secondaryFontSet)
   {
    if (height==appFonts[1].h)
       SetFontBIOS(1,height,appFonts[1].data,0);
    else
      {
       int fontOk=0;
       // The one provided by the application isn't suitable
       if (frCB)
         {// Try asking the application to provide a new one
          TScreenFont256 *font=frCB(1,8,height);
          if (font)
            {
             SetFontBIOS(1,height,font->data,0);
             MemorizeFont(1,font);
             fontOk=1;
             DeleteArray(font->data);
             delete font;
            }
         }
       if (!fontOk)
         {
          DisableDualFont();
          secondaryFontSet=0;
         }
      }
   }

 if (fontWasLoaded && !(primaryFontSet || secondaryFontSet))
    // Restore the original encoding if we forced ROM fonts
    TVCodePage::SetCodePage(origCPApp,origCPScr,origCPInp);
}


#else // DJGPP

#include <tv/dos/screen.h>
#include <tv/dos/key.h>
#include <tv/dos/mouse.h>

#endif // else DJGPP

