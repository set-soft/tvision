/**[txh]********************************************************************

  Display module, Copyright 2001-2002 by Salvador E. Tropea
  Description:
  This is the base class for low level screen output.
  Most members are pointers to functions defined by each platform dependent
driver. Default members are provided.
  This is a complete new file created from concepts that José Angel Sanchez
Caso provide me and coded from scratch. The names of some members are the
same used in original Turbo Vision for compatibility purposes.
  
***************************************************************************/

//#define DEBUG_CURSOR
#ifdef DEBUG_CURSOR
 #include <stdio.h>
#endif
#define Uses_TScreen
#include <tv.h>

// Remove me please!
int TDisplay::dual_display=0;

void        (*TDisplay::clearScreen)(uchar, uchar)=TDisplay::defaultClearScreen;
ushort      (*TDisplay::getRows)()                =TDisplay::defaultGetRows;
ushort      (*TDisplay::getCols)()                =TDisplay::defaultGetCols;
void        (*TDisplay::setCrtMode)(ushort mode)  =TDisplay::defaultSetCrtMode;
ushort      (*TDisplay::getCrtMode)()             =TDisplay::defaultGetCrtMode;
void        (*TDisplay::getCursorPos)(unsigned &x, unsigned &y)
                                                  =TDisplay::defaultGetCursorShape;
void        (*TDisplay::setCursorPos)(unsigned  x, unsigned  y)
                                                  =TDisplay::defaultSetCursorShape;
void        (*TDisplay::setCursorShape)(unsigned start, unsigned end)
                                                  =TDisplay::defaultSetCursorShape;
void        (*TDisplay::getCursorShape)(unsigned &start, unsigned &end)
                                                  =TDisplay::defaultGetCursorShape;
void        (*TDisplay::setCrtModeExt)(char *mode)=TDisplay::defaultSetCrtModeExt;
int         (*TDisplay::checkForWindowSize)(void) =TDisplay::defaultCheckForWindowSize;
const char *(*TDisplay::getWindowTitle)(void)     =TDisplay::defaultGetWindowTitle;
int         (*TDisplay::setWindowTitle)(const char *name)
                                                  =TDisplay::defaultSetWindowTitle;
int         (*TDisplay::getBlinkState)()          =TDisplay::defaultGetBlinkState;
void        (*TDisplay::getDisPaletteColors)(int from, int number, TScreenColor *colors)
                                                  =TDisplay::defaultGetDisPaletteColors;
void        (*TDisplay::setDisPaletteColors)(int from, int number, TScreenColor *colors)
                                                  =TDisplay::defaultSetDisPaletteColors;
int           TDisplay::argc                      =0;
char        **TDisplay::argv                      =NULL;
char        **TDisplay::environ                   =NULL;
TScreenColor  TDisplay::OriginalPalette[16];
TScreenColor  TDisplay::ActualPalette[16];

/*****************************************************************************

 Default actions for TDisplay.
    
*****************************************************************************/

/**[txh]********************************************************************
  Description: Number of columns of the physical screen.
***************************************************************************/

ushort TDisplay::defaultGetCols()
{
 return 80;
}

/**[txh]********************************************************************
  Description: Number of rows of the physical screen.
***************************************************************************/

ushort TDisplay::defaultGetRows()
{
 return 25;
}

/**[txh]********************************************************************
  Description: Erase the screen using this width and height.
***************************************************************************/

void TDisplay::defaultClearScreen( uchar, uchar )
{
}

/**[txh]********************************************************************
  Description: Sets the cursor shape, values in percent.
***************************************************************************/

void TDisplay::defaultSetCursorShape(unsigned /*start*/, unsigned /*end*/)
{
}

/**[txh]********************************************************************
  Description: Gets the cursor shape, values in percent.
***************************************************************************/

void TDisplay::defaultGetCursorShape(unsigned &start, unsigned &end)
{
 start=end=0;
}

/**[txh]********************************************************************
  Description: Sets the video mode.
***************************************************************************/

void TDisplay::defaultSetCrtMode(ushort)
{
}

/**[txh]********************************************************************
  Description: Sets the video mode using a string. It could be an external
program or other information that doesn't fit in an ushort.
***************************************************************************/

void TDisplay::defaultSetCrtModeExt(char *)
{
}

/**[txh]********************************************************************
  Description: Returns current video mode.
***************************************************************************/

ushort TDisplay::defaultGetCrtMode()
{
 return smCO80;
}

/**[txh]********************************************************************
  Description: Returns !=0 if the screen size changed externally. Usually
when we are in a window, but isn't the only case.
***************************************************************************/

int TDisplay::defaultCheckForWindowSize(void)
{
 return 0;
}

/**[txh]********************************************************************
  Description: Gets the visible title of the screen, usually the window
title.
***************************************************************************/

const char *TDisplay::defaultGetWindowTitle(void)
{
 return newStr("");
}

/**[txh]********************************************************************
  Description: Sets the visible title of the screen, usually the window
title.
  Return: !=0 success.
***************************************************************************/

int TDisplay::defaultSetWindowTitle(const char *)
{
 return 0;
}

/**[txh]********************************************************************
  Description: Finds if the MSB of the attribute is for blinking.
  Return: 0 no, 1 yes, 2 no but is used for other thing.
***************************************************************************/

int TDisplay::defaultGetBlinkState()
{
 return 2;
}

TDisplay::TDisplay()
{
 //updateIntlChars();
}

TDisplay::TDisplay(const TDisplay&)
{
 //updateIntlChars();
}

TDisplay::~TDisplay()
{
 if (font)
   {
    delete font;
    font=0;
   }
}

/**[txh]********************************************************************

  Description:
  Sets the cursor shape. I take the TV 2.0 convention: the low 8 bits is
the start and the high 8 bits the end. Values can be between 0 and 99. To
disable the cursor a value of 0 is used.

***************************************************************************/

void TDisplay::setCursorType(ushort val)
{
 setCursorShape(val & 0xFF,val>>8);
 #ifdef DEBUG_CURSOR
 fprintf(stderr,"Seteando 0x%0X => %X %X\n",val,val & 0xFF,val>>8);
 #endif
}

ushort TDisplay::getCursorType()
{
 unsigned start,end;
 getCursorShape(start,end);
 #ifdef DEBUG_CURSOR
 fprintf(stderr,"Obteniendo: start %X end %X => 0x%0X\n",start,end,(start | (end<<8)));
 #endif
 return (ushort)(start | (end<<8));
}

void TDisplay::setArgv(int aArgc, char **aArgv, char **aEnvir)
{
 argc=aArgc;
 argv=aArgv;
 environ=aEnvir;
}

/*****************************************************************************

  I originally added it to support fonts under DOS. The idea is that the
original TFont is just a "do nothing" class but can be changed by a class
that really does the job. It should be changed to functionality inside the
TDisplay class.

*****************************************************************************/

TFont *TDisplay::font=0;

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

int TFont::UseDefaultFontsNextTime=0;

// SET: That's a dummy because I don't have an easy way to change fonts in
// Linux
int TFont::SelectFont(int , int, int, int, int )
{
 return 1;
}

/*****************************************************************************

  These should set/get the palette values at low level. The TScreen driver
must indicate if they work. The dummies help to know the PC BIOS palette.

*****************************************************************************/

TScreenColor TDisplay::PC_BIOSPalette[16]=
{
 { 0x00, 0x00, 0x00 },
 { 0x00, 0x00, 0xA8 },
 { 0x00, 0xA8, 0x00 },
 { 0x00, 0xA8, 0xA8 },
 { 0xA8, 0x00, 0x00 },
 { 0xA8, 0x00, 0xA8 },
 { 0xA8, 0x54, 0x00 },
 { 0xA8, 0xA8, 0xA8 },
 { 0x54, 0x54, 0x54 },
 { 0x54, 0x54, 0xFC },
 { 0x54, 0xFC, 0x54 },
 { 0x54, 0xFC, 0xFC },
 { 0xFC, 0x54, 0x54 },
 { 0xFC, 0x54, 0xFC },
 { 0xFC, 0xFC, 0x54 },
 { 0xFC, 0xFC, 0xFC }
};

void TDisplay::defaultGetDisPaletteColors(int from, int number, TScreenColor *colors)
{
 while (number-- && from<16)
    *(colors++)=PC_BIOSPalette[from++];
}

void TDisplay::defaultSetDisPaletteColors(int , int , TScreenColor *)
{
}
