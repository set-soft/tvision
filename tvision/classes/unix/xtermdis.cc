/*****************************************************************************

  XTerm display routines.
  Copyright (c) 2002 by Salvador E. Tropea (SET)
  Covered by the GPL license.

*****************************************************************************/
#include <tv/configtv.h>

#if defined(TVOS_UNIX) && !defined(TVOSf_QNXRtP)
#define Uses_stdio
#define Uses_stdlib
#define Uses_unistd
#define Uses_TDisplay
#define Uses_TScreen
#define Uses_string
#include <tv.h>

#include <termios.h>
#include <term.h>
#include <sys/ioctl.h>

#include <tv/unix/xtscreen.h>
#include <tv/linux/log.h>

int                   TDisplayXTerm::curX=0;
int                   TDisplayXTerm::curY=0;
// Current cursor shape
int                   TDisplayXTerm::cursorStart=86;  //  86 %
int                   TDisplayXTerm::cursorEnd  =99;  //  99 %
// 1 when the size of the window where the program is running changed
volatile sig_atomic_t TDisplayXTerm::windowSizeChanged=0;
int                   TDisplayXTerm::hOut=-1;   // Handle for the console output

// All the code is in TScreenLinux, but this is the right moment, by this
// time TScreenLinux is suspended.
TDisplayXTerm::~TDisplayXTerm()
{
 LOG("TDisplayXTerm Destructor");
 TScreenXTerm::DeallocateResources();
}

void TDisplayXTerm::Init()
{
 setCursorPos=SetCursorPos;
 getCursorPos=GetCursorPos;
 getCursorShape=GetCursorShape;
 setCursorShape=SetCursorShape;

 // Check if TIOCGWINSZ works
 winsize win;
 win.ws_col=0xFFFF;
 ioctl(hOut,TIOCGWINSZ,&win);
 if (win.ws_col!=0xFFFF)
   {// Ok!
    getRows=GetRowsIOCTL;
    getCols=GetColsIOCTL;
   }
 else
   {// Nope, use Window Manipulation escape sequences
    getRows=GetRowsSeq;
    getCols=GetColsSeq;
   }
 setCrtMode=SetCrtMode;
 setCrtModeExt=SetCrtModeExt;
 checkForWindowSize=CheckForWindowSize;
 getWindowTitle=GetWindowTitle;
 setWindowTitle=SetWindowTitle;
}

void TDisplayXTerm::SetCursorPos(unsigned x, unsigned y)
{
 fprintf(stdout,"\E[%d;%dH",y+1,x+1);
 curX=x; curY=y;
}

void TDisplayXTerm::GetCursorPos(unsigned &x, unsigned &y)
{
 fflush(stdin);
 fputs("\E[6n",stdout);
 // Is it safe?
 int nR,nC;
 if (fscanf(stdin,"\E[%d;%dR",&nR,&nC)==2)
   {
    curX=nC-1;
    curY=nR-1;
   }
 x=curX; y=curY;
}

void TDisplayXTerm::SetCursorShape(unsigned start, unsigned end)
{
 LOG("Setting cursor shape to " << start << "," << end);
 if (start>=end)
   { // Hide cursor DECRST DECTCEM
    fputs("\E[?25l",stdout);
   }
 else
   { // Show cursor DECSET DECTCEM
    fputs("\E[?25h",stdout);
   }
 cursorStart=start;
 cursorEnd=end;
}

void TDisplayXTerm::GetCursorShape(unsigned &start, unsigned &end)
{
 // Currently we don't know the real state.
 start=cursorStart;
 end  =cursorEnd;
}

ushort TDisplayXTerm::GetRowsIOCTL()
{
 winsize win;
 ioctl(hOut,TIOCGWINSZ,&win);

 return win.ws_row;
}

ushort TDisplayXTerm::GetRowsSeq()
{
 fputs("\E[18t",stdout);
 int nR,nC;
 if (fscanf(stdin,"\E[8;%d;%dt",&nR,&nC)==2)
    return nR;
 return 25;
}

ushort TDisplayXTerm::GetColsIOCTL()
{
 winsize win;

 ioctl(hOut,TIOCGWINSZ,&win);
 return win.ws_col;
}

ushort TDisplayXTerm::GetColsSeq()
{
 fputs("\E[18t",stdout);
 int nR,nC;
 if (fscanf(stdin,"\E[8;%d;%dt",&nR,&nC)==2)
    return nC;
 return 80;
}

void TDisplayXTerm::SetCrtMode(ushort )
{ // Just set the cursor to a known state
 cursorStart=86;
 cursorEnd  =99;
 fputs("\E[?25h",stdout);
}

void TDisplayXTerm::SetCrtModeExt(char *mode)
{
 setCrtMode(0); // Just set the cursor to a known state
 system(mode);
}

int TDisplayXTerm::CheckForWindowSize(void)
{
 int ret=windowSizeChanged!=0;
 windowSizeChanged=0;
 return ret;
}

const char *TDisplayXTerm::GetWindowTitle(void)
{
 char buffer[256]; // Put a max.
 fputs("\E[21t",stdout);
 fgets(buffer,255,stdin);
 // OSC l Name ST (\E]lName\0)
 if (buffer[0]!=27 || buffer[1]!=']' || buffer[2]!='l')
    return 0;
 return newStr(buffer+3);
}

int TDisplayXTerm::SetWindowTitle(const char *name)
{
 fprintf(stdout,"\E]2;%s\x7",name);
 return 1;
}
#endif // TVOS_UNIX && !TVOSf_QNXRtP

