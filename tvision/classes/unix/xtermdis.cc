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
#define Uses_TGKey
#include <tv.h>

#include <termios.h>
#include <term.h>
#include <sys/ioctl.h>
#include <signal.h>

#include <tv/unix/xtscreen.h>
#include <tv/unix/xtkey.h>
#include <tv/linux/log.h>

int                   TDisplayXTerm::curX=0;
int                   TDisplayXTerm::curY=0;
// Current cursor shape
int                   TDisplayXTerm::cursorStart=86;  //  86 %
int                   TDisplayXTerm::cursorEnd  =99;  //  99 %
// 1 when the size of the window where the program is running changed
volatile sig_atomic_t TDisplayXTerm::windowSizeChanged=0;
int                   TDisplayXTerm::hOut=-1;   // Handle for the console output
int                   TDisplayXTerm::selCharset=0;
int                   TDisplayXTerm::terminalType=TDisplayXTerm::XTerm;
void                (*TDisplayXTerm::ResetPaletteColors)()=ResetPaletteColorsXT;
char                  TDisplayXTerm::cMap[16]={0,4,2,6,1,5,3,7,8,12,10,14,9,13,11,15};

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
 fflush(TGKeyXTerm::fIn);
 fputs("\E[6n",stdout);
 // Is it safe?
 int nR,nC;
 if (fscanf(TGKeyXTerm::fIn,"\E[%d;%dR",&nR,&nC)==2)
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
    if (!getShowCursorEver())
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
 if (fscanf(TGKeyXTerm::fIn,"\E[8;%d;%dt",&nR,&nC)==2)
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
 if (fscanf(TGKeyXTerm::fIn,"\E[8;%d;%dt",&nR,&nC)==2)
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
 fgets(buffer,255,TGKeyXTerm::fIn);
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

/*
  Important note about palette:
  Only XTerm from X 4.x and Eterm 0.9.x supports the palette manipulation.
  As we don't have an easy way to determine the version of the terminal we
are using this escape sequence must be "neutral".
  The problem is that both terminals have problems with it. For this reason
I added a Shift In (^O=\xF) to the sequence. When the sequence is
interpreted this SI is executed and that's why I restore the value at the
end (if (selCharset) ...). If the sequence isn't interpreted the SI finishes
the sequence and the rest is discarded. In this way we don't mess the
terminal state.
*/
// XTerm code
int TDisplayXTerm::SetDisPaletteColorsXT(int from, int number, TScreenColor *colors)
{
 // Assume all will be ok
 int ret=number;
 fputs("\E]4",stdout);
 while (number-- && from<16)
   {
    fprintf(stdout,";%d;#%2.2X%2.2X%2.2X",cMap[from++],colors->R,colors->G,colors->B);
    colors++;
   }
 fputc(7,stdout);
 return ret;
}

void TDisplayXTerm::ResetPaletteColorsXT()
{// Just setup a default palette
 SetDisPaletteColorsXT(0,16,PC_BIOSPalette);
}

// Eterm code, Linux console style
int TDisplayXTerm::SetDisPaletteColorsEt(int from, int number, TScreenColor *colors)
{
 // Assume all will be ok
 int ret=number;
 while (number-- && from<16)
   {
    fprintf(stdout,"\E]P%1.1X%2.2X%2.2X%2.2X\xF",cMap[from++],colors->R,colors->G,colors->B);
    colors++;
   }
 if (selCharset)
    fputc(14,stdout);
 return ret;
}

void TDisplayXTerm::ResetPaletteColorsEt()
{
 fputs("\E]R",stdout);
}
#endif // TVOS_UNIX && !TVOSf_QNXRtP

