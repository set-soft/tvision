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

// SET: Moved the standard headers here because according to DJ
// they can inconditionally declare symbols like NULL
#include <stdlib.h>

#define Uses_TScreen
#define Uses_TEvent
#define Uses_TFrame
#include <tv.h>

#include <dos.h>
#include <go32.h>
#include <dpmi.h>
#define REGS __dpmi_regs
#define INTR(nr,r) __dpmi_int(nr,&r)
#include <pc.h>

#include <conio.h>
#include <sys/farptr.h>
#include <errno.h>

static ushort Equipment;
static uchar CrtInfo;
static uchar CrtRows;

ushort * TDisplay::equipment = &Equipment;
uchar * TDisplay::crtInfo = &CrtInfo;
uchar * TDisplay::crtRows = &CrtRows;
TFont *TDisplay::font=0;

REGS r_display;

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

void TDisplay::SetPage(uchar page)
{
  Page = page;
  AH = 0x05;
  AL = page;
  videoInt();
}

void TDisplay::SetCursor(int x,int y)
{
  DH = y;
  DL = x;
  BH = GetPage();
  AH = 2;
  videoInt();
}

void TDisplay::GetCursor(int &x,int &y)
{
  AH = 3;
  BH = GetPage();
  videoInt();
  y = DH;
  x = DL;
}

ushort TDisplay::getCursorType()
{
  AH = 3;
  BH = GetPage();
  videoInt();
  return CX;
}

void TDisplay::setCursorType( ushort ct )
{
  AH = 1;
  CX = ct;
  BH = GetPage();
  videoInt();
}

void TDisplay::clearScreen( uchar , uchar )
{
  ScreenClear();
}

void TDisplay::videoInt()
{
    INTR(0x10,r_display);
}

ushort TDisplay::getRows()
{
  return ScreenRows();
}

ushort TDisplay::getCols()
{
  return ScreenCols();
}

#if 0
#include <stdio.h>
static
unsigned VBEScreenMode()
{
 unsigned mode=ScreenMode();
 fprintf(stderr,"Modo reportado por libc: %03x\n",mode);

 // Now test if VBE is available and wich mode reports
 AX=0x4F03;
 INTR(0x10,r_display);
 if (AL==0x4F && AH==0)
   {
    mode=BX & 0x3FFF;
    fprintf(stderr,"Modo reportado por VBE: %03x\n",mode);
   }
 return mode;
}
#endif

ushort TDisplay::getCrtMode()
{
  ushort mode = ScreenMode(); // VBEScreenMode();
  if (mode == smCO80)
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

extern int emulate_mouse;
/* tmode from 0 to 5 */
extern "C" int set_tweaked_text(int tmode, void (*setf)(int));

// A way to avoid problems from the C side
extern "C" void set_font_for_tweaked_C_code(int lines);

void set_font_for_tweaked_C_code(int lines)
{
 // Tweaked uses BIOS and hence have the 8/9x16 fonts loaded so avoid a
 // reload
 TDisplay::SelectFont(lines,lines==16 ? 1 : 0);
}

// SET: Added VESA support
static
void Textmode(int mode)
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
 INTR(0x10,r_display);
}

/* The following code is taken from conio.c. I had to do this,
   because I don't want to to set the video mode (crt mode) with
   clearing the screen, but the _set_scanline function does this.

   And I assume here anywhere, that there is an VGA adapter and no
   EGA!!!
*/

static int adapter_type = 2;

/* Set screen scan lines and load appropriate font.
   SCAN_LINES and FONT is the height */
static void
set_scan_lines_and_font(int scan_lines, int font)
{
  __dpmi_regs regs;

  /* Set 200/350/400 scan lines.  */
  regs.h.ah = 0x12;
  regs.h.al = scan_lines;       /* 0: 200, 1: 350, 2: 400 */
  regs.h.bl = 0x30;
  __dpmi_int(0x10, &regs);

  /* Scan lines setting only takes effect when video mode is set.  */
  regs.x.ax = 0x83;
  __dpmi_int(0x10, &regs);

  TDisplay::SelectFont(font);
}


/* Set screen scan lines and load 8x10 font.
   SCAN_LINES is as required by Int 10h function 12h. */
static void
set_scan_lines_and_8x10_font(int scan_lines)
{
  __dpmi_regs regs;

  regs.h.bl = 0x30;
  regs.h.ah = 0x12;
  regs.h.al = scan_lines;           /* 0: 200, 1: 350, 2: 400 */
  __dpmi_int(0x10, &regs);

  /* Set video mode, so that scan lines we set will take effect.  */
  regs.x.ax = 0x83;
  __dpmi_int(0x10, &regs);

  /* Load our 8x10 font and enable intensity bit.  */
  TDisplay::SelectFont(10);
  //load_8x10_font();
}

/* Switch to screen lines given by NLINES.  */
void
__set_screen_lines(int nlines)
{
  switch (nlines)
    {
      __dpmi_regs regs;

      case 25:
          if (adapter_type)
            {
              /* Set 350 scan lines for EGA, 400 for VGA.  */
              regs.h.bl = 0x30;
              regs.h.ah = 0x12;
              regs.h.al = (adapter_type > 1 ? 2 : 1);
              __dpmi_int(0x10, &regs);
            }

          /* Set video mode.  */
          regs.x.ax = 0x83;
          __dpmi_int(0x10, &regs);

          // Fonts must be loaded AFTER the mode setting
          // and only if needed
          if (adapter_type>1)
             TDisplay::SelectFont(16,1);
          else
             if (adapter_type)
                TDisplay::SelectFont(14,1);
          break;
      case 28:      /* VGA only */
          if (adapter_type > 1)
            set_scan_lines_and_font(2,14);
          break;
      case 35:      /* EGA or VGA */
          if (adapter_type)
            set_scan_lines_and_8x10_font(1);
          break;
      case 40:      /* VGA only */
          if (adapter_type > 1)
            set_scan_lines_and_8x10_font(2);
          break;
      case 43:      /* EGA or VGA */
          if (adapter_type)
            set_scan_lines_and_font(1,8);
          break;
      case 50:      /* VGA only */
          if (adapter_type > 1)
            set_scan_lines_and_font(2,8);
          break;
    }

}

/* end of code from conio.c */

static
void TestForBIOSupport()
{
 _farpokeb(_dos_ds,ScreenPrimary,'!');
 AX = 0x200;
 BH = 0;
 DX = 0;
 INTR(0x10,r_display);
 AH = 0x0A;
 AL = ' ';
 BH = 0;
 CX = 1;
 INTR(0x10,r_display);
 if (_farpeekb(_dos_ds,ScreenPrimary) != ' ')
 {
   Textmode(C80);
   emulate_mouse = 0;
 }
 else
 {
   emulate_mouse = 1;
 }
}

void TDisplay::setCrtMode( ushort mode )
{
  if (TMouse::present())
    TMouse::hide();
  if ((mode & 0xFF) == smCO80)
  {
    int hmode = mode >> 8;
    if (hmode > 6 && hmode < 14)
    {
      set_tweaked_text(hmode-7,set_font_for_tweaked_C_code);
      emulate_mouse = 1;
    }
    else
    {
#if 0
      Textmode(C80);
#endif
      switch (hmode)
      {
        case 0:
          __set_screen_lines(25);
          break;
        case 1:
          __set_screen_lines(28);
          break;
        case 2:
          __set_screen_lines(35);
          break;
        case 3:
          __set_screen_lines(40);
          break;
        case 4:
          __set_screen_lines(43);
          break;
        case 5:
          __set_screen_lines(50);
          break;
      }
      emulate_mouse = 0;
    }
  }
  else
  {
    Textmode(mode);
    TestForBIOSupport();
    outp(0x3D4,9); // CRTC 9
    int lines=(inp(0x3D5) & 0x1F)+1; // lines per char
    TDisplay::SelectFont(lines,1);
  }
  if (TMouse::present())
  {
    TMouse::setRange(getCols()-1,getRows()-1);
    TMouse::show();
  }
}


void TDisplay::setCrtMode( char *command )
{
 system(command);
 TestForBIOSupport();

 outp(0x3D4,9); // CRTC 9
 int lines=(inp(0x3D5) & 0x1F)+1; // lines per char
 TDisplay::SelectFont(lines,1,0);

 if (TMouse::present())
 {
  TMouse::setRange(getCols()-1,getRows()-1);
  TMouse::show();
 }
}

static unsigned getCodePage()
{
    //  get version number, in the form of a normal number
    AX = 0x3000;
    INTR(0x21,r);
    unsigned ver = (AH) | ((AL & 0xff) << 8);
    if( ver < 0x30C )
        return 437; // United States code page, for all versions before 3.3

    AX = 0x6601;
    INTR(0x21,r);
    return BX;
}

void TDisplay::updateIntlChars()
{
    if(getCodePage() != 437 )
        TFrame::frameChars[30] = 'Í';
}

int TDisplay::SelectFont(int height, int noForce, int modeRecalculate, int width)
{
 if (!font)
    font=new TFont();
 return font->SelectFont(height,width,0,noForce,modeRecalculate);
}

void TDisplay::SetFontHandler(TFont *f)
{
 if (font) // The destructor is virtual
    delete font;
 font=f;
}

void TDisplay::RestoreDefaultFont(void)
{
 if (!font)
    font=new TFont();
 font->RestoreDefaultFont();
}
#endif // __DJGPP__

