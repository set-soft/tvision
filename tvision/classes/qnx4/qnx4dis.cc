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

#include <termios.h>

#include <tv/qnx4/screen.h>

extern "C"
{
   #include <sys/term.h>
   #include <sys/qnxterm.h>
}

#include <sys/dev.h>

ushort TDisplayQNX4::CursorLastX=0xFFFF;
ushort TDisplayQNX4::CursorLastY=0xFFFF;
ushort TDisplayQNX4::CursorShapeStart=85;
ushort TDisplayQNX4::CursorShapeEnd=100;
ushort TDisplayQNX4::ConsoleResizing=0;
ushort TDisplayQNX4::IgnoreConsoleResizing=0;
char   TDisplayQNX4::ConsoleMode=1;
struct _VideoModes TDisplayQNX4::modes[NUM_MODES]=
{
   {smCO80x25,  80,  25}, {smCO80x28,  80,  28}, {smCO80x35,  80,  35},
   {smCO80x40,  80,  40}, {smCO80x43,  80,  43}, {smCO80x50,  80,  50},
   {smCO80x30,  80,  30}, {smCO80x34,  80,  34}, {smCO90x30,  90,  30},
   {smCO90x34,  90,  34}, {smCO94x30,  94,  30}, {smCO94x34,  94,  34},
   {smCO82x25,  82,  25}, {smCO80x60,  80,  60}, {smCO132x25, 132, 25},
   {smCO132x43, 132, 34}, {smCO132x50, 132, 50}, {smCO132x60, 132, 60}
};

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
   TDisplay::getCrtMode=GetCrtMode;
   TDisplay::setCrtMode=SetCrtMode;
   TDisplay::setCrtModeRes_p=SetCrtModeRes;
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

   return win.ws_row;
}

ushort TDisplayQNX4::GetCols()
{
   struct winsize win;

   ioctl(fileno(stdin), TIOCGWINSZ, &win);

   return win.ws_col;
}

ushort TDisplayQNX4::GetCrtMode()
{
   int x=GetCols();
   int y=GetRows();
   
   for (int i=0; i<NUM_MODES; i++)
   {
      if ((modes[i].x==x) && (modes[i].y==y))
      {
         return modes[i].videomode;
      }
   }
   
   return ((x<<8) | (y & 0x000000FFUL));
}

void TDisplayQNX4::SetCrtMode(ushort mode)
{
   struct winsize win;

   win.ws_col=char((mode>>8) & 0x000000FFUL);
   win.ws_row=char(mode & 0x000000FFUL);
   win.ws_xpixel=0;
   win.ws_ypixel=0;

   if (mode!=0xFFFF)
   {
      IgnoreConsoleResizing=1;
      if (ioctl(fileno(stdin), TIOCSWINSZ, &win)!=-1)
      {
         term_relearn_size();
      }
   }
}

int TDisplayQNX4::SetCrtModeRes(unsigned w, unsigned h, int fW, int fH)
{
   struct winsize win;

   win.ws_col=w;
   win.ws_row=h;
   win.ws_xpixel=0;
   win.ws_ypixel=0;

   IgnoreConsoleResizing=1;
   if (ioctl(fileno(stdin), TIOCSWINSZ, &win)!=-1)
   {
      term_relearn_size();
      return 1;
   }

   return 0;
}

void TDisplayQNX4::SetCrtModeExt(char* mode)
{
   IgnoreConsoleResizing=1;
   system(mode);
   term_relearn_size();
}

int TDisplayQNX4::CheckForWindowSize(void)
{
   int ret=0;

   if (ConsoleResizing)
   {
      if (IgnoreConsoleResizing)
      {
         ret=0;
         ConsoleResizing=0;
         IgnoreConsoleResizing=0;
      }
      else
      {
         ret=1;
         ConsoleResizing=0;
      }
   }

   return ret;
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
