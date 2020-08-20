/* X11 display routines.
   Copyright (c) 2001-2003 by Salvador E. Tropea (SET)
   Covered by the GPL license. */
#include <tv/configtv.h>

#define Uses_string
#define Uses_TDisplay
#define Uses_TScreen
#include <tv.h>

// I delay the check to generate as much dependencies as possible
#if (defined(TVOS_UNIX) || defined(TVCompf_Cygwin)) && defined(HAVE_X11)

// X11 defines their own values
#undef True
#undef False
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <tv/x11/screen.h>

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
int       TDisplayX11::newX;
int       TDisplayX11::newY;
unsigned  TDisplayX11::fontW;
unsigned  TDisplayX11::fontWb;
unsigned  TDisplayX11::fontH;
unsigned  TDisplayX11::fontSz;

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
 TScreenX11::UnDrawCursor();
 cursorX=x; cursorY=y;
 cursorPX=x*fontW; cursorPY=y*fontH;
 TScreenX11::DrawCursor();
}

void TDisplayX11::GetCursorPos(unsigned &x, unsigned &y)
{
 x=cursorX; y=cursorY;
}

void TDisplayX11::GetCursorShape(unsigned &start, unsigned &end)
{
 start=100*cShapeFrom/fontH;
 end  =100*cShapeTo/fontH;
 //fprintf(stderr,"GetCursorShape: %d,%d\n",start,end);
}

void TDisplayX11::SetCursorShape(unsigned start, unsigned end)
{
 //fprintf(stderr,"SetCursorShape: %d,%d start>=end? %d\n",start,end,start>=end);
 if (start>=end && getShowCursorEver())
    return;
 cShapeFrom=start*fontH/100;
 cShapeTo  =end*fontH/100;
 if ((unsigned)cShapeFrom>fontH) cShapeFrom=fontH;
 if ((unsigned)cShapeTo>fontH)   cShapeTo  =fontH;
 if (start>=end)
    TScreenX11::DisableCursor();
 else
    TScreenX11::EnableCursor();
 //fprintf(stderr,"Seteando X11: %d a %d\n",cShapeFrom,cShapeTo);
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
 int ret=0;
 SEMAPHORE_ON;
 if (windowSizeChanged)
   {
    // Now we can change the values because the application is aware
    maxX=newX;
    maxY=newY;
    // Reallocate the buffer cleaning it to force a deep redraw
    delete[] TScreen::screenBuffer;
    TScreen::screenBuffer=new ushort[maxX*maxY];
    memset(TScreen::screenBuffer,0,maxX*maxY*sizeof(ushort));
    windowSizeChanged=0;
    ret=1;
   }
 SEMAPHORE_OFF;
 return ret;
}

#else

#include <tv/x11/screen.h>

#endif // defined(TVOS_UNIX) && defined(HAVE_X11)

