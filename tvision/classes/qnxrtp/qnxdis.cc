/* QNX RtP screen routines source.
   Copyright (c) 2002-2003 by Mike Gorchak
   Covered by the BSD license. */

#include <tv/configtv.h>

#define Uses_stdio
#define Uses_stdlib
#define Uses_unistd
#define Uses_TDisplay
#define Uses_TScreen
#define Uses_string

#include <tv.h>

// I delay the check to generate as much dependencies as possible
#if defined(TVOS_UNIX) && defined(TVOSf_QNXRtP)

#include <termios.h>
#include <tv/qnxrtp/screen.h>

#include <term.h>
#include <sys/ioctl.h>

int TDisplayQNXRtP::cur_x=0;
int TDisplayQNXRtP::cur_y=0;

int TDisplayQNXRtP::tty_fd=-1;

int TDisplayQNXRtP::cursorStart=85;
int TDisplayQNXRtP::cursorEnd=100;

ushort TDisplayQNXRtP::mode=0;

volatile sig_atomic_t TDisplayQNXRtP::windowSizeChanged=0;

TDisplayQNXRtP::~TDisplayQNXRtP()
{
}

void TDisplayQNXRtP::Init()
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

void TDisplayQNXRtP::SetCursorPos(unsigned x, unsigned y)
{
   char out[1024], *p = out;

   safeput(p, tparm(cursor_address, y, x));
   p-=4;
   write(tty_fd, out, p - out);

   cur_x = x;
   cur_y = y;
}

void TDisplayQNXRtP::GetCursorPos(unsigned &x, unsigned &y)
{
   y = cur_y;
   x = cur_x;
}

void TDisplayQNXRtP::SetCursorShape(unsigned start, unsigned end)
{
   char out[1024], *p=out;

   if (start>=end)
   {
      safeput(p, tparm(cursor_invisible));
      write(tty_fd, out, p-out);
   }
   else
   {
      if (end-start>15)
      {
         // It is hack for now. cursor_visible is empty string. Why ???
         safeput(p, tparm(cursor_normal));
         safeput(p, tparm(cursor_visible));
         write(tty_fd, out, p-out);
      }
      else
      {
         safeput(p, tparm(cursor_normal));
         write(tty_fd, out, p-out);
      }
   }
   cursorStart=start;
   cursorEnd=end;
}

void TDisplayQNXRtP::GetCursorShape(unsigned &start, unsigned &end)
{
   start = cursorStart;
   end = cursorEnd;
}

ushort TDisplayQNXRtP::GetRows()
{
   winsize win;

   win.ws_row=0xFFFF;
   ioctl(tty_fd, TIOCGWINSZ, &win);
   
   return win.ws_row!=0xFFFF ? win.ws_row : 25;
}

ushort TDisplayQNXRtP::GetCols()
{
   winsize win;

   win.ws_col=0xFFFF;
   ioctl(tty_fd, TIOCGWINSZ, &win);

   return win.ws_col!=0xFFFF ? win.ws_col : 80;
}

void TDisplayQNXRtP::SetCrtMode(ushort mode)
{
   TDisplayQNXRtP::mode=mode;
}

void TDisplayQNXRtP::SetCrtModeExt(char* mode)
{
   system(mode);
}

int TDisplayQNXRtP::CheckForWindowSize(void)
{
   int ret;
   
   ret=windowSizeChanged!=0;
   windowSizeChanged=0;

   return ret;
}

const char* TDisplayQNXRtP::GetWindowTitle(void)
{
   return NULL;
}

int TDisplayQNXRtP::SetWindowTitle(const char* name)
{
   return 0;
}

inline void TDisplayQNXRtP::safeput(char* &p, char* cap)
{
   if (cap)
   {
      while (*cap)
      {
         *p++=*cap++;
      }
   }
}

#else

   // Here to generate the dependencies in RHIDE
   #include <tv/qnxrtp/screen.h>

#endif // TVOS_UNIX && TVOSf_QNXRtP
