/**[txh]********************************************************************

  DOS Display (TDisplayDOS) functions.

  Copyright (c) 1996-1998 by Robert H”hne.
  Copyright (c) 1998-2002 by Salvador E. Tropea (SET)
  Tweaked video modes by Christian Domp <chrisd@europeonline.com>

  Module: DOS Display
  Comments:
  This module implements the low level DOS screen access.@p

  Todo:@*
  * The palette map could be changed during suspend. Currently not
  suported. But is a really bizarre case and is restored if VGA state saving
  is used.
  
***************************************************************************/

#include <tv/configtv.h>

//#define Uses_stdio
#define Uses_stdlib
#define Uses_string
#define Uses_TScreen
#define Uses_TEvent
#define Uses_TFrame
#include <tv.h>

// I delay the check to generate as much dependencies as possible
#ifdef TVCompf_djgpp

#include <conio.h>
#include <dos.h>
#include <go32.h>
#include <dpmi.h>
#include <pc.h>
#include <sys/farptr.h>
#include <errno.h>
#include <sys/movedata.h>

#define TSCREEN_DEFINE_REGISTERS
#include <tv/dos/screen.h>
#include <tv/dos/mouse.h>

// In early versions Robert experimented with pages now it isn't used but
// some simple future driver could do it.
#define GetPage() 0

// This could be needed for Windows XP Professional, not sure.
// Disabled until I get positive confirmation.
//#define XP_CURSOR_SHAPE_WORKAROUND 1

// x86 registers used for BIOS calls
__dpmi_regs TDisplayDOS::rDisplay;
// Numbers of lines of each character (height in pixels)
unsigned    TDisplayDOS::charLines=16;
// Flag to indicate if the video mode should ask the mouse class to draw
// the cursor instead of letting the OS mouse driver do it.
char        TDisplayDOS::emulateMouse=0;
char        TDisplayDOS::colorsMap[17];

// Video BIOS services
#define SET_TEXT_MODE_CURSOR_SHAPE                       0x01
#define SET_CURSOR_POSITION                              0x02
#define GET_CURSOR_POSITION_AND_SIZE                     0x03
#define READ_ALL_PALETTE_REGISTERS_AND_OVERSCAN_REGISTER 0x1009
#define SET_INDIVIDUAL_DAC_REGISTER                      0x1010
#define READ_INDIVIDUAL_DAC_REGISTER                     0x1015
#define ALTERNATE_FUNCTION_SELECT                        0x12
#define SELECT_VERTICAL_RESOLUTION                       0x30

const int crtIndex=0x3B4,crtData=0x3B5;
const int mdaCursorLocationHigh=0x0E,mdaCursorLocationLow=0x0F;
const int mdaCursorStart=0x0A,mdaCursorEnd=0x0B;

static inline
unsigned char InMDA(unsigned char i)
{
 outportb(crtIndex,i);
 return inportb(crtData);
}

static inline
void OutMDA(unsigned char i,unsigned char b)
{
 outportb(crtIndex,i);
 outportb(crtData,b);
}

void TDisplayDOS::SetCursorPos(unsigned x, unsigned y)
{
 if (dual_display || TScreenDOS::screenMode==7)
   {
    unsigned loc=y*80+x;
    OutMDA(mdaCursorLocationHigh,loc>>8);
    OutMDA(mdaCursorLocationLow ,loc & 0xFF);
   }
 else
   {
    DH=y;
    DL=x;
    BH=GetPage();
    AH=SET_CURSOR_POSITION;
    videoInt();
   }
}

void TDisplayDOS::GetCursorPos(unsigned &x, unsigned &y)
{
 if (dual_display)
   {
    unsigned pos=(InMDA(mdaCursorLocationHigh)<<8) |
                 InMDA(mdaCursorLocationLow);
    y=pos/80;
    x=pos%80;
   }
 else
   {
    AH=GET_CURSOR_POSITION_AND_SIZE;
    BH=GetPage();
    videoInt();
    y=DH;
    x=DL;
   }
}

void TDisplayDOS::GetCursorShape(unsigned &start, unsigned &end)
{
 #if XP_CURSOR_SHAPE_WORKAROUND
 static int searchedXP=0, detectedXP;
 #endif

 if (dual_display || TScreenDOS::screenMode==7)
   {
    start=InMDA(mdaCursorStart)*100/charLines;
    end  =InMDA(mdaCursorEnd)*100/charLines;
    return;
   }

 #if XP_CURSOR_SHAPE_WORKAROUND
 if (!searchedXP)
   {
    searchedXP=1;
    char *OS=getenv("OS");
    detectedXP=(OS && strcmp(OS,"Windows_NT")==0);
   }
 #endif

 AH=GET_CURSOR_POSITION_AND_SIZE;
 BH=GetPage();
 videoInt();

 if (CX==0x2000)
   {
    start=end=0;
    return;
   }

 #if XP_CURSOR_SHAPE_WORKAROUND
 /* Andris reported:
    "Noticed big nuisance under WinXP Pro: videoInt at
     start returns 0x0F0F and as result there is no normal
     cursor in TVision applications later. This workarounds
     the problem. AP. 26/09/2004)" */
 if (detectedXP && CX==0x0F0F)
    CH--;
 #endif

 start=CH*100/charLines;
 end  =CL*100/charLines;
}

void TDisplayDOS::SetCursorShape(unsigned start, unsigned end)
{
 if (start>=end && getShowCursorEver())
    return;

 unsigned lStart=(start*charLines+50)/100;
 unsigned lEnd=(end*charLines+50)/100;

 if (dual_display || TScreenDOS::screenMode==7)
   {
    if (start<=end) // cursor off
      {// Is that really needed?
       OutMDA(mdaCursorStart,0x01);
       OutMDA(mdaCursorEnd,0x00);
      }
    else
     {
      OutMDA(mdaCursorStart,lStart);
      OutMDA(mdaCursorEnd,lEnd);
     }
    return;
   }
 AH=SET_TEXT_MODE_CURSOR_SHAPE;
 BH=GetPage();
 if (start>=end)
    CX=0x2000;
 else
   {
    CH=lStart;
    CL=lEnd;
   }
 videoInt();
}

// This isn't correctly implemented, but is OK
void TDisplayDOS::ClearScreen(uchar , uchar)
{
 ScreenClear();
}

ushort TDisplayDOS::GetRows()
{
 return dual_display ? 25 : ScreenRows();
}

ushort TDisplayDOS::GetCols()
{
 return dual_display ? 80 : ScreenCols();
}

ushort TDisplayDOS::GetCrtMode()
{
 ushort mode=ScreenMode();
 if (mode==smCO80)
   {
    // SET: I changed it because was wrong. With STM I can set 108x34 (25% more
    // resolution, same aspect) but it reports 3
    int res=getRows()+getCols()*100;

    switch (res)
      {
       case 8025:
            break;
       case 8028:
            mode+=0x100;
            break;
       case 8035:
            mode+=0x200;
            break;
       case 8040:
            mode+=0x300;
            break;
       case 8043:
            mode+=0x400;
            break;
       case 8050:
            mode+=0x500;
            break;
       case 8030:
            mode+=0x700;
            break;
       case 8034:
            mode+=0x800;
            break;
       case 9030:
            mode+=0x900;
            break;
       case 9034:
            mode+=0xA00;
            break;
       case 9430:
            mode+=0xB00;
            break;
       case 9434:
            mode+=0xC00;
            break;
       case 8225:
            mode+=0xD00;
            break;
       default: // We can reproduce it but isn't mode 3 so if the user_mode
                // is 3 we MUST do a mode change.
            mode+=0xFF00;
      }
   }
 return mode;
}


/*****************************************************************************
 Helper routines
*****************************************************************************/

void TDisplayDOS::setVideoModeInt()
{
 // Windows NT workaround, looks like is something that happends in my
 // machine but happends.
 // I tested Windows NT 4.0 spanish, spanish+SP3 and english running in a
 // TXProII motherboard with a M571 SYS integrated video chip using AMI
 // BIOS and a K6 233 CPU. The fact is that NT clobbers the time after
 // setting the video mode, the value that appears is around 512 hours and
 // looks like that's enough to produce a "divide overflow" exception in the
 // emulated DOS, it kills the djgpp application.
 unsigned long v1=_farpeekl(_dos_ds,0x46C);

 videoInt();
 
 unsigned long v2=_farpeekl(_dos_ds,0x46C);
 if (v2-v1>65536)
   { // Come on, switching video modes can't take 1 hour ;-)))
    _farpokel(_dos_ds,0x46C,v1+5);
   }
}

int TDisplayDOS::getCharLines()
{
 AX=0x1130;
 BL=0;
 videoInt();
 return CL;
 #if 0
 // Some parts of the code used it, but I switched to BIOS version.
 // I guess here we can trust BIOS, we'll see.
 outp(0x3D4,9); // CRTC 9
 return (inp(0x3D5) & 0x1F)+1; // lines per char
 #endif
}

/*****************************************************************************
 VESA and user provided video modes routines
*****************************************************************************/

void TDisplayDOS::setTextMode(int mode)
{
 if (mode>=0x100)
   { // It should be a VESA mode
    AX=0x4F02;
    BX=mode | 0x8000; // do not clear the screen
   }
 else
   {
    AX=(mode & 0xff) | 0x80; // do not clear the screen
   }
 setVideoModeInt();
 charLines=getCharLines();
}

/*****************************************************************************
 Tweaked video modes routines
*****************************************************************************/

const int numTweakedModes=7;
const int tw80x30=0, tw80x34=1, tw90x30=2, tw90x34=3, tw94x30=4, tw94x34=5,
          tw82x25=6; // that's a replacement for 80x25 but with 8 pixels fonts

// Size of the available modes
static ushort width[] ={80,80,90,90,94,94,82};
static ushort height[]={30,34,30,34,30,34,25};

/* How to interpret the data, 80x30 example:
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
static unsigned short ttweaks[][16]=
{
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

int TDisplayDOS::setTweakedMode(int tmode)
{
 int i;
 volatile int junk;
 
 if (tmode>=numTweakedModes)
    return -1;
 
 AX=0x3 | 0x80; // do not clear the screen
 setVideoModeInt();

 if (height[tmode]==34)
   {
    TScreenDOS::SelectFont(14,True); // load 8x14 font for 34 line modes
    charLines=14;
   }
 else
   {// Tweaked uses BIOS and hence have the 8/9x16 fonts loaded so avoid a
    // reload
    TScreenDOS::SelectFont(16);
    charLines=16;
   }

 // wait for vertical retrace
 while ((inportb(0x3da) & 8)!=0);
 while ((inportb(0x3da) & 8)==0);

 disable();
 if (tmode==tw82x25)
    outportb(0x3c2,0x63); // 400 lines, 25 MHz
 else
    outportb(0x3c2,0xe7); // 480 lines, 28 MHz
 // set sequencer clocking mode
 outportw(0x3c4,0x101);
 // reset data flip-flop to address mode
 junk=inportw(0x3da);
 outportb(0x3c0,0x30);
 // set mode control register
 outportb(0x3c0,0x04);
 outportb(0x3c0,0x33);
 // set horizontal pixel pan
 outportb(0x3c0,0x00);
 for(i = 0; i < 16; i++)
   outportw(0x3d4,ttweaks[tmode][i]);
 // update BIOS data area
 _farsetsel(_dos_ds);
 _farnspokew(0x44a,width[tmode]);
 _farnspokeb(0x484,height[tmode]-1);
 // We must indicate the correct height. Some BIOSes make the cursor
 // invisible if we forget it.
 _farnspokew(0x485,charLines);
 _farnspokew(0x44c,width[tmode]*height[tmode]*2);
 enable();

 return 0;
}

/*****************************************************************************
 Extended video modes routines (80x 28,35,40,43,50)
*****************************************************************************/
/*
   The following code is based on what conio.c does, Robert used it
   to avoid clearing the screen. I modified it a lot and added the
   propper fonts handling.

   Note we assumed the board is VGA and it will fail for EGA and MDA.
*/

// 0: 200, 1: 350, 2: 400
const int scl200=0, scl350=1, scl400=2;

/* Set screen scan lines and load appropriate font.
   SCAN_LINES and FONT is the height.
   SCAN_LINES is as required by Int 10h function 12h. */
void TDisplayDOS::setExtendedMode(int mode)
{
 // Default mode is almost mode 0
 int scanLines=scl400;
 charLines=16;
 Boolean Force=True;

 switch (mode)
   {
    case 0: // 25 lines
         // EGA: setScanLinesAndFont(scl350,14,1);
         Force=False;
         break;
    case 1: // 28 lines VGA only
         charLines=14;
         break;
    case 2: // 35 lines EGA or VGA
         scanLines=scl350;
         charLines=10;
         break;
    case 3: // 40 lines VGA only
         charLines=10;
         break;
    case 4: // 43 lines EGA or VGA
         scanLines=scl350;
         charLines=8;
         break;
    case 5: // 50 lines VGA only
         charLines=8;
         break;
   }
 // Set 200/350/400 scan lines.
 AH=ALTERNATE_FUNCTION_SELECT;
 AL=scanLines;
 BL=SELECT_VERTICAL_RESOLUTION;
 videoInt();

 // Scan lines setting only takes effect when video mode is set.
 AX=0x83;
 setVideoModeInt();

 TScreenDOS::SelectFont(charLines,Force);
}
/****************************************************************************/

/**[txh]********************************************************************

  Description: 
  Used to test if a mode that we don't know can be handled by the driver
or not. If the mode uses an unknown memory region we just assume that's
a graphics mode and revert to 80x25. It could be enhanced a lot specially to
support VESA video modes when implemented in a way that the video memory
isn't located at the usual address.
  
***************************************************************************/

void TDisplayDOS::testForSupport()
{
 // Write a ! at 0,0 position in the video memory
 // That's a guess, we don't know if that's a real text mode.
 _farpokeb(_dos_ds,ScreenPrimary,'!');
 AX=0x200; // Set cursor position
 BH=0;     // Page 0
 DX=0;     // X=Y=0
 videoInt();
 AH=0x0A;  // Write character only at cursor position
 AL=' ';   // A space to overwrite the !
 BH=0;     // Page 0
 CX=1;     // Just once
 videoInt();
 // Check if BIOS changed what we think is 0,0 coordinate
 if (_farpeekb(_dos_ds,ScreenPrimary)!=' ')
   {// Nope, BIOS can't do it.
    // Here we assume this isn't a real text mode and revert to 80x25
    setTextMode(smCO80);
    emulateMouse=0;
   }
 else
   {// As this mode could be anything strange we can't assume the mouse driver
    // can draw the mouse. That's common for VESA modes.
    // So we draw the cursor `by hand'.
    emulateMouse=1;
   }
}

/**[txh]********************************************************************

  Description:
  Sets a video mode from the video mode number. Video modes with the low
nibble equal 3 are used to encode extended and tweacked video modes. Note
that VESA 800x600x256 mode is 0x103 and here it is assumed to be extended
mode 1, that's 80x28. Also note that regular VGA 80x25 mode is the
"extended mode 0".@*
  Any other value is assumed to be a user provided video mode and is
accepted only if we are sure that's really text mode and we know how to
handle it.
  
***************************************************************************/

void TDisplayDOS::SetCrtMode(ushort mode)
{
 if (TMouse::present())
    TMouse::hide();

 if (mode==smFont8x8) // Looks like Borland TV 2.0 defined it
    mode=smCO80x50;
 // Extended and tweaked modes are numbered Mode<<8 | smCO80
 // 0-6 are extended modes.
 // 7-13 are tweaked modes.
 if ((mode & 0xFF)==smCO80)
   {
    int hmode=mode>>8;
    if (hmode>6 && hmode<14)
      {
       setTweakedMode(hmode-7);
       emulateMouse=1;
      }
    else
      {
       setExtendedMode(hmode);
       emulateMouse=0;
      }
   }
 else
   {
    setTextMode(mode);
    testForSupport();
    TScreenDOS::SelectFont(charLines);
   }

 if (TMouse::present())
   {
    THWMouseDOS::setEmulation(emulateMouse);
    TMouse::setRange(getCols()-1,getRows()-1);
    TMouse::show();
   }
 if (paletteModified)
    SetDisPaletteColors(0,16,ActualPalette);
}


void TDisplayDOS::SetCrtModeExt(char *command)
{
 // Should we hide the mouse here? [Added TV 2.0.0]
 if (TMouse::present())
    TMouse::hide();

 system(command);
 testForSupport();
 charLines=getCharLines();
 TScreenDOS::SelectFont(charLines);
 // Don't trust in the driver [Added TV 2.0.0]
 emulateMouse=1;

 if (TMouse::present())
   {
    THWMouseDOS::setEmulation(emulateMouse);
    TMouse::setRange(getCols()-1,getRows()-1);
    TMouse::show();
   }
}

int TDisplayDOS::SetCrtModeRes(unsigned w, unsigned h, int fW, int fH)
{
 if (w==TScreen::screenWidth && h==TScreen::screenHeight) return 0;
 // DOS can't do it so we just select the closest mode available
 unsigned pos;
 Boolean exact=searchClosestRes(dosModesRes,w,h,TDisplayDOSModesNum,pos);
 SetCrtMode(dosModes[pos]);
 return exact && dosModesCell[pos].x==(unsigned)fW &&
        dosModesCell[pos].y==(unsigned)fH ? 1 : 2;
}

/**[txh]********************************************************************

  Description:
  Finds the main window title if we are running under W95 and windowed.

  Return:
  A pointer to a newly allocated string (new[]). Or 0 if fail. by SET.

***************************************************************************/

const char *TDisplayDOS::GetWindowTitle(void)
{
 // Winoldap Get Title.
 AX=0x168E;
 DX=2;
 // Size of transfer buffer
 unsigned long tbsize=_go32_info_block.size_of_transfer_buffer;
 CX=tbsize>0xFFFF ? 0xFFFF : tbsize; // Can tbsize be > 64Kb?
 // Address of tb
 DI=__tb & 0x0f;                     // It should be 0
 ES=(__tb>>4) & 0xffff;

 __dpmi_int(0x2F,&r);

 if (AX!=1)
    return 0;

 // Scan to get the length
 _farsetsel(_dos_ds);
 unsigned i;
 for (i=0; i<tbsize && _farnspeekb(i); i++);
 // Make a copy
 char *ret=new char[i+1];
 dosmemget(__tb,i,ret);
 ret[i]=0;
 return ret;
}

const int mxTitleSize=80; // Limited by Windows, yet another silly idea

/**[txh]********************************************************************

  Description:
  Sets the main window title if we are running under W95 and windowed.

  Return:
  1 successful. by SET.

***************************************************************************/

int TDisplayDOS::SetWindowTitle(const char *name)
{
 char title[mxTitleSize];

 // Winoldap Set Title.
 AX=0x168E;
 DX=0;
 // Address of transfer buffer
 DI=__tb & 0x0f;                     // It should be 0
 ES=(__tb>>4) & 0xffff;
 // Copy to tb
 strncpy(title,name,mxTitleSize);
 title[mxTitleSize-1]=0;
 dosmemput(title,mxTitleSize,__tb);

 __dpmi_int(0x2F,&r);

 return AX;
}

void TDisplayDOS::getPaletteMap(char *map)
{
 AX=READ_ALL_PALETTE_REGISTERS_AND_OVERSCAN_REGISTER;
 ES=__tb>>4;
 DX=__tb & 0xF;
 videoInt();
 dosmemget(__tb,17,map);
}

void TDisplayDOS::setOnePaletteIndex(int index, TScreenColor *col)
{
 AX=SET_INDIVIDUAL_DAC_REGISTER;
 BX=colorsMap[index];
 DH=col->R>>2; CH=col->G>>2; CL=col->B>>2;
 videoInt();
}

void TDisplayDOS::getOnePaletteIndex(int index, TScreenColor *col)
{
 AX=READ_INDIVIDUAL_DAC_REGISTER;
 BL=colorsMap[index];
 videoInt();
 col->R=DH<<2;
 col->G=CH<<2;
 col->B=CL<<2;
}

int TDisplayDOS::SetDisPaletteColors(int from, int number, TScreenColor *colors)
{
 int ret=number;
 while (number-- && from<16)
    setOnePaletteIndex(from++,colors++);
 return ret;
}

void TDisplayDOS::GetDisPaletteColors(int from, int number, TScreenColor *colors)
{
 while (number-- && from<16)
    getOnePaletteIndex(from++,colors++);
}

void TDisplayDOS::Beep()
{
 sound(1000);
 delay(100);
 nosound();
}

void TDisplayDOS::Init()
{// Currently we assume it doesn't change.
 getPaletteMap(colorsMap);
 // Memorize original palette
 GetDisPaletteColors(0,16,OriginalPalette);
 // That's the current
 memcpy(ActualPalette,OriginalPalette,sizeof(ActualPalette));

 TDisplay::setCursorPos=SetCursorPos;
 TDisplay::getCursorPos=GetCursorPos;
 TDisplay::getCursorShape=GetCursorShape;
 TDisplay::setCursorShape=SetCursorShape;
 TDisplay::clearScreen=ClearScreen;
 TDisplay::getRows=GetRows;
 TDisplay::getCols=GetCols;
 TDisplay::setCrtMode=SetCrtMode;
 TDisplay::getCrtMode=GetCrtMode;
 TDisplay::setCrtModeExt=SetCrtModeExt;
 TDisplay::setCrtModeRes_p=SetCrtModeRes;
 TDisplay::getWindowTitle=GetWindowTitle;
 TDisplay::setWindowTitle=SetWindowTitle;
 TDisplay::setDisPaletteColors=SetDisPaletteColors;
 TDisplay::getDisPaletteColors=GetDisPaletteColors;
 TDisplay::beep=Beep;
}

TDisplayDOS::~TDisplayDOS() {}

#else // DJGPP

#include <tv/dos/screen.h>
#include <tv/dos/mouse.h>

#endif // else DJGPP

