/* QNX 4 screen routines source.
   Copyright (c) 1998-2003 by Mike Gorchak
   Covered by the BSD license. */

#include <tv/configtv.h>

#define Uses_stdio
#define Uses_stdlib
#define Uses_unistd
#define Uses_TDisplay
#define Uses_TScreen
#define Uses_string
#define Uses_ioctl
#define Uses_signal

#include <tv.h>

// I delay the check to generate as much dependencies as possible
#if defined(TVOS_UNIX) && defined(TVOSf_QNX4)

#include <tv/qnx4/screen.h>

extern "C"
{
   #include <sys/term.h>
   #include <sys/qnxterm.h>
}

#include <sys/dev.h>

ushort TDisplayQNX4::ScreenSizeX=0;
ushort TDisplayQNX4::ScreenSizeY=0;
ushort TDisplayQNX4::CursorLastX=0xFFFF;
ushort TDisplayQNX4::CursorLastY=0xFFFF;
ushort TDisplayQNX4::CursorShapeStart=0xFFFF;
ushort TDisplayQNX4::CursorShapeEnd=0xFFFF;
char   TDisplayQNX4::ConsoleMode=1;

TDisplayQNX4::~TDisplayQNX4()
{
}

void TDisplayQNX4::Init()
{
   TDisplay::setCursorPos=SetCursorPos;
   TDisplay::getCursorPos=GetCursorPos;
   TDisplay::getCursorShape=GetCursorShape;
   TDisplay::setCursorShape=SetCursorShape;
   TDisplay::getRows=GetRows;
   TDisplay::getCols=GetCols;
   TDisplay::setCrtModeExt=SetCrtModeExt;
   TDisplay::checkForWindowSize=CheckForWindowSize;
}

void TDisplayQNX4::SetCursorPos(unsigned x, unsigned y)
{
   CursorLastX=x;
   CursorLastY=y;
   term_cur(y, x);
   term_flush();
}

void TDisplayQNX4::GetCursorPos(unsigned &x, unsigned &y)
{
   x=CursorLastX;
   y=CursorLastY;
}

void TDisplayQNX4::SetCursorShape(unsigned start, unsigned end)
{
   char* CursorShapeControlString;

   if (start>=end)
   {
      CursorShapeControlString=cursor_invisible;
   }
   else
   {
      if (end-start>15)
      {
         CursorShapeControlString=cursor_visible;
      }
      else
      {
         CursorShapeControlString=cursor_normal;
      }
   }

   __putp(CursorShapeControlString);
   term_flush();

   CursorShapeStart=start;
   CursorShapeEnd=end;
}

void TDisplayQNX4::GetCursorShape(unsigned &start, unsigned &end)
{
   start=CursorShapeStart;
   end=CursorShapeEnd;
}

ushort TDisplayQNX4::GetRows()
{
   struct winsize win;

   ioctl(fileno(stdin), TIOCGWINSZ, &win);
   ScreenSizeY=win.ws_row;

   return win.ws_row;
}

ushort TDisplayQNX4::GetCols()
{
   struct winsize win;

   ioctl(fileno(stdin), TIOCGWINSZ, &win);
   ScreenSizeX=win.ws_col;

   return win.ws_col;
}

void TDisplayQNX4::SetCrtMode(ushort mode)
{
   struct winsize win;

   win.ws_col=char(mode>>8);
   win.ws_row=char(mode);
   win.ws_xpixel=0;
   win.ws_ypixel=0;
   
   ioctl(fileno(stdin), TIOCSWINSZ, &win);

   ScreenSizeX=GetCols();
   ScreenSizeY=GetRows();
   
   term_relearn_size();
   
   term_color(TERM_WHITE | TERM_BLACK_BG);
   term_clear(TERM_CLS_SCR);
   term_flush();
}

void TDisplayQNX4::SetCrtModeExt(char* mode)
{
   system(mode);
}

int TDisplayQNX4::CheckForWindowSize(void)
{
}

const char* TDisplayQNX4::GetWindowTitle(void)
{
   return NULL;
}

int TDisplayQNX4::SetWindowTitle(const char* name)
{
   return 0;
}

#else
// Here to generate the dependencies in RHIDE
#include <tv/qnx4/screen.h>
#endif // TVOS_UNIX && TVOSf_QNX4
