/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* Copyright (C) 1999-2002 Salvador Eduardo Tropea */
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
#if defined(HAVE_NCURSES) && defined(TVOS_UNIX) && !defined(TVOSf_QNXRtP)

#include <termios.h>
#include <term.h>

#include <tv/unix/screen.h>

int TDisplayUNIX::cur_x=0;
int TDisplayUNIX::cur_y=0;
// SET: Current cursor shape
int TDisplayUNIX::cursorStart=85;  //  85 %
int TDisplayUNIX::cursorEnd  =100; // 100 %

// SET: 1 when the size of the window where the program is running changed
volatile sig_atomic_t TDisplayUNIX::windowSizeChanged=0;

int TDisplayUNIX::tty_fd=-1; // tty descriptor

TDisplayUNIX::~TDisplayUNIX() {}

void TDisplayUNIX::Init()
{
 setCursorPos=SetCursorPos;
 getCursorPos=GetCursorPos;
 getCursorShape=GetCursorShape;
 setCursorShape=SetCursorShape;
 getRows=GetRows;
 getCols=GetCols;
 setCrtModeExt=SetCrtModeExt;
 checkForWindowSize=CheckForWindowSize;
}

void TDisplayUNIX::SetCursorPos(unsigned x, unsigned y)
{
 char out[1024], *p = out;

 safeput(p, tparm(cursor_address,y, x));
 write(tty_fd, out, p - out);
 cur_x = x;
 cur_y = y;
}

void TDisplayUNIX::GetCursorPos(unsigned &x, unsigned &y)
{
 char s[40];
 
 // write/read are better here, because other functions might be buffered
 write(tty_fd,"\e[6n",4); // Request cursor position from terminal
 read(tty_fd,s,sizeof(s)); // Should never overflow...

 y = atoi(s+2)-1;
 x = atoi(strchr(s,';')+1)-1;
}

void TDisplayUNIX::SetCursorShape(unsigned start, unsigned end)
{
 char out[1024], *p=out;
 if (start>=end) // hide
   {
    safeput(p,tparm(cursor_invisible));
    write(tty_fd,out,p-out);
   }
 else
   {
    safeput(p,tparm(cursor_normal));
    write(tty_fd,out,p-out);
   }
 cursorStart=start;
 cursorEnd=end;
 //fprintf(stderr,"SetCursorShape: %d,%d start>=end? %d\n",start,end,start>=end);
}

void TDisplayUNIX::GetCursorShape(unsigned &start, unsigned &end)
{
 // Currently we don't know the real state.
 start=cursorStart;
 end  =cursorEnd;
 //fprintf(stderr,"GetCursorShape: %d,%d\n",start,end);
}

ushort TDisplayUNIX::GetRows()
{
 if (dual_display) return 25;
 winsize win;
 win.ws_row=0xFFFF;
 ioctl(tty_fd,TIOCGWINSZ,&win);
 return win.ws_row!=0xFFFF ? win.ws_row : 25;
}

ushort TDisplayUNIX::GetCols()
{
 if (dual_display) return 80;
 winsize win;
 win.ws_col=0xFFFF;
 ioctl(tty_fd,TIOCGWINSZ,&win);
 return win.ws_col!=0xFFFF ? win.ws_col : 80;
}

void TDisplayUNIX::SetCrtModeExt(char *mode)
{
 system(mode);
}

int TDisplayUNIX::CheckForWindowSize(void)
{
 int ret=windowSizeChanged!=0;
 windowSizeChanged=0;
 return ret;
}
#else // TVOS_UNIX && !TVOSf_QNXRtP

#include <tv/unix/screen.h>

#endif // else TVOS_UNIX && !TVOSf_QNXRtP

