/* X11 display routines.
   Copyright (c) 2001 by Salvador E. Tropea (SET)
   Covered by the GPL license. */
#include <tv/configtv.h>

#if defined(TVOS_UNIX) && defined(HAVE_X11)
#define Uses_TDisplay
#define Uses_TScreen
#include <tv.h>

// X11 defines their own values
#undef True
#undef False
#include <X11/Xlib.h>

#include <tv/x11scr.h>

/*****************************************************************************

  TDisplayX11 display stuff, that's very easy because we control all the
  variables involved

*****************************************************************************/

int       TDisplayX11::cursorX;
int       TDisplayX11::cursorY;
int       TDisplayX11::cursorPX;
int       TDisplayX11::cursorPY;
char      TDisplayX11::cShapeFrom;
char      TDisplayX11::cShapeTo;
char      TDisplayX11::windowSizeChanged=0;
int       TDisplayX11::maxX;
int       TDisplayX11::maxY;
unsigned  TDisplayX11::fontW;
unsigned  TDisplayX11::fontH;

TDisplayX11::~TDisplayX11() {}

void TDisplayX11::Init()
{
 setCursorPos=SetCursorPos;
 getCursorPos=GetCursorPos;
 getCursorShape=GetCursorShape;
 setCursorShape=SetCursorShape;
 getRows=GetRows;
 getCols=GetCols;
 checkForWindowSize=CheckForWindowSize;
}

void TDisplayX11::SetCursorPos(unsigned x, unsigned y)
{
 cursorX=x; cursorY=y;
 cursorPX=x*fontW; cursorPY=y*fontH;
}

void TDisplayX11::GetCursorPos(unsigned &x, unsigned &y)
{
 x=cursorX; y=cursorY;
}

void TDisplayX11::GetCursorShape(unsigned &start, unsigned &end)
{
 start=100*cShapeFrom/fontH;
 end  =100*cShapeTo/fontH;
}

void TDisplayX11::SetCursorShape(unsigned start, unsigned end)
{
 cShapeFrom=start*fontH/100;
 cShapeTo  =end*fontH/100;
 if ((unsigned)cShapeFrom>fontH) cShapeFrom=fontH;
 if ((unsigned)cShapeTo>fontH)   cShapeTo  =fontH;
}

ushort TDisplayX11::GetRows()
{
 return maxY;
}

ushort TDisplayX11::GetCols()
{
 return maxX;
}

int TDisplayX11::CheckForWindowSize(void)
{
 int ret=windowSizeChanged!=0;
 windowSizeChanged=0;
 return ret;
}
#endif // defined(TVOS_UNIX) && defined(HAVE_X11)

