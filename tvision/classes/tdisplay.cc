/**[txh]********************************************************************

  Copyright 2001-2002 by Salvador E. Tropea
  This file is covered by the GPL license.
  
  Module: Display
  Include: TDisplay
  Comments:
  This is the base class for low level screen output.
  Most members are pointers to functions defined by each platform dependent
driver. Default members are provided.
  This is a complete new file created from concepts that José Angel Sanchez
Caso provide me and coded from scratch. The names of some members are the
same used in original Turbo Vision for compatibility purposes.
  
***************************************************************************/

//#define DEBUG_CURSOR
#define Uses_stdio
#define Uses_stdlib
#define Uses_TScreen
#define Uses_TVCodePage
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
int         (*TDisplay::setDisPaletteColors)(int from, int number, TScreenColor *colors)
                                                  =TDisplay::defaultSetDisPaletteColors;
int         (*TDisplay::setCrtModeRes_p)(unsigned w, unsigned h, int fW, int fH)
                                                  =TDisplay::defaultSetCrtModeRes;
Boolean     (*TDisplay::showBusyState)(Boolean state)
                                                  =TDisplay::defaultShowBusyState;
void        (*TDisplay::beep)()                   =TDisplay::defaultBeep;
int           TDisplay::argc                      =0;
char        **TDisplay::argv                      =NULL;
char        **TDisplay::environment               =NULL;
TScreenColor  TDisplay::OriginalPalette[16];
TScreenColor  TDisplay::ActualPalette[16];
char          TDisplay::paletteModified           =0;
char          TDisplay::avoidMoire                =0;
uint32        TDisplay::opts1                     =0;
TVCodePage   *TDisplay::codePage                  =NULL;
Boolean       TDisplay::busyState                 =False;
TVScreenDriverDetectCallBack TDisplay::dCB        =NULL;
TScreenResolution TDisplay::dosModesRes[TDisplayDOSModesNum]=
{
 {  80,25 },
 {  80,28 },
 {  80,30 },
 {  80,34 },
 {  80,35 },
 {  80,40 },
 {  80,43 },
 {  80,50 },
 {  80,60 },
 {  82,25 },
 {  90,30 },
 {  90,34 },
 {  94,30 },
 {  94,34 },
 { 132,25 },
 { 132,43 },
 { 132,50 },
 { 132,60 }
};
TScreenResolution TDisplay::dosModesCell[TDisplayDOSModesNum]=
{
 { 9,16 },
 { 9,14 },
 { 9,16 },
 { 9,14 },
 { 9,10 },
 { 9,10 },
 { 9, 8 },
 { 9, 8 },
 { 9, 8 },
 { 8,16 },
 { 9,16 },
 { 9,14 },
 { 9,16 },
 { 9,14 },
 { 9,14 },
 { 9,11 },
 { 9,10 },
 { 9, 8 }
};
int           TDisplay::dosModes[TDisplayDOSModesNum]=
{
 smCO80x25,
 smCO80x28,
 smCO80x30,
 smCO80x34,
 smCO80x35,
 smCO80x40,
 smCO80x43,
 smCO80x50,
 smCO80x60,
 smCO82x25,
 smCO90x30,
 smCO90x34,
 smCO94x30,
 smCO94x34,
 smCO132x25,
 smCO132x43,
 smCO132x50,
 smCO132x60
};

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
  Description:
  Returns information about a legacy DOS mode.
  Return: True if the mode is known.
***************************************************************************/

Boolean TDisplay::searchDOSModeInfo(ushort mode, unsigned &w, unsigned &h,
                                    int &fW, int &fH)
{
 int i;
 for (i=0; i<TDisplayDOSModesNum; i++)
     if (dosModes[i]==mode)
       {
        w=dosModesRes[i].x;
        h=dosModesRes[i].y;
        fW=dosModesCell[i].x;
        fH=dosModesCell[i].y;
        return True;
       }
 return False;
}

/**[txh]********************************************************************
  Description: Sets the video mode.
***************************************************************************/

void TDisplay::defaultSetCrtMode(ushort mode)
{
 unsigned w, h;
 int fW, fH;
 if (searchDOSModeInfo(mode,w,h,fW,fH))
    setCrtModeRes(w,h,fW,fH);
 setCursorShape(86,99);
}

/**[txh]********************************************************************
  Description: Sets the video mode using a string. It could be an external
program or other information that doesn't fit in an ushort.
***************************************************************************/

void TDisplay::defaultSetCrtModeExt(char *command)
{
 setCursorShape(86,99);
 system(command);
}

/**[txh]********************************************************************
  Description: Selects the mode that's closest to the sepcified width and
height. The optional font size can be specified.
  Return: 0 no change done, 1 change done with all the requested parameters,
2 change done but just to get closer.
***************************************************************************/

int TDisplay::defaultSetCrtModeRes(unsigned /*w*/, unsigned /*h*/, int /*fW*/,
                                   int /*fH*/)
{
 return 0;
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

/*****************************************************************************
  Description: Shows/hides something to indicate the application is busy.
  Return: the previous state.
*****************************************************************************/

Boolean TDisplay::defaultShowBusyState(Boolean state)
{
 Boolean ret=busyState;
 busyState=state;
 return ret;
}

/**[txh]********************************************************************
  Description:
  Makes an audible indication.
***************************************************************************/

void TDisplay::defaultBeep()
{
 puts("\x7");
}

TDisplay::TDisplay()
{
}

TDisplay::TDisplay(const TDisplay&)
{
}

TDisplay::~TDisplay()
{
 if (codePage)
   {
    delete codePage;
    codePage=NULL;
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
 environment=aEnvir;
}

Boolean TDisplay::searchClosestRes(TScreenResolution *res, unsigned x,
                                   unsigned y, unsigned cant, unsigned &pos)
{
 unsigned i, minDif, indexMin, dif;
 int firstXMatch=-1;
 // Look for an exact match of width
 for (i=0; i<cant && res[i].x<x; i++)
    {
     if (res[i].x==x)
       {
        if (firstXMatch==-1) firstXMatch=i;
        if (res[i].y==y)
          {// Exact match
           pos=i;
           return True;
          }
       }
    }
 if (firstXMatch!=-1)
   {// Return the closest y that match x
    i=indexMin=firstXMatch;
    minDif=abs(res[i].y-y);
    while (++i<cant && res[i].x==x)
      {
       dif=abs(res[i].y-y);
       if (dif<minDif)
         {
          minDif=dif;
          indexMin=i;
         }
      }
    pos=indexMin;
    return False;
   }
 // No x match, looks the one with minimum differences
 indexMin=0;
 minDif=abs(res[0].y-y)+abs(res[0].x-x);
 i=1;
 while (i<cant)
   {
    dif=abs(res[i].y-y)+abs(res[i].x-x);
    if (dif<minDif)
      {
       minDif=dif;
       indexMin=i;
      }
    i++;
   }
 pos=indexMin;
 return False;
}

/*****************************************************************************

  These should set/get the palette values at low level. The TScreen driver
must indicate if they work. The dummies help to know the PC BIOS palette.

*****************************************************************************/

// Default PC BIOS palette
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

// This is the palette parsed from the tvrc file or the application
TScreenColor TDisplay::UserStartPalette[16];

void TDisplay::defaultGetDisPaletteColors(int from, int number, TScreenColor *colors)
{
 while (number-- && from<16)
    *(colors++)=PC_BIOSPalette[from++];
}

int TDisplay::defaultSetDisPaletteColors(int , int number, TScreenColor *)
{
 return number;
}

TVScreenDriverDetectCallBack TDisplay::setDetectCallBack(TVScreenDriverDetectCallBack aCB)
{
 TVScreenDriverDetectCallBack ret=dCB;
 dCB=aCB;
 return ret;
}

void TDisplay::SetDefaultCodePages(int idScr, int idApp, int idInp)
{
 TVCodePage::SetDefaultCodePages(idScr,idApp,idInp);
}

/*****************************************************************************

  Options routines, they are created to isolate the internal aspects.

*****************************************************************************/

Boolean TDisplay::setShowCursorEver(Boolean value)
{
 Boolean ret=getShowCursorEver();
 if (value)
    opts1|=ShowCursorEver;
 else
    opts1&= ~ShowCursorEver;
 return ret;
}

Boolean TDisplay::setDontMoveHiddenCursor(Boolean value)
{
 Boolean ret=getDontMoveHiddenCursor();
 if (value)
    opts1|=DontMoveHiddenCursor;
 else
    opts1&= ~DontMoveHiddenCursor;
 return ret;
}

