/*****************************************************************************

  XTerm display routines.
  Copyright (c) 2002,2003 by Salvador E. Tropea (SET)
  Covered by the GPL license.

  ToDo:
  * GetWindowTitle can lock until a key is pressed, shouldn't do it.

*****************************************************************************/
#include <tv/configtv.h>

#define Uses_stdio
#define Uses_stdlib
#define Uses_unistd
#define Uses_ioctl
#define Uses_string
#define Uses_TDisplay
#define Uses_TScreen
#define Uses_TGKey
#define Uses_signal
#include <tv.h>

// I delay the check to generate as much dependencies as possible
#if defined(TVOS_UNIX) && !defined(TVOSf_QNXRtP)

#include <termios.h>

#include <tv/unix/xtscreen.h>
#include <tv/unix/xtkey.h>
#include <tv/linux/log.h>

// GNU libc pulls it but other libc implementations just have a forward
// declaration for struct timeval.
#include <sys/time.h>
// That's a workaround for a bug in the glibc 2.3.2 and 2.3.3:
// The TEMP_FAILURE_RETRY macro uses errno, but errno.h isn't included!
#include <errno.h>
// TEMP_FAILURE_RETRY isn't mandatory.
#ifndef TEMP_FAILURE_RETRY
 #define TEMP_FAILURE_RETRY(a) (a)
#endif

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
unsigned              TDisplayXTerm::fontW,
                      TDisplayXTerm::fontH;
char                  TDisplayXTerm::fontChanged=0;
uchar                 TDisplayXTerm::startScreenWidth,
                      TDisplayXTerm::startScreenHeight;
char                 *TDisplayXTerm::oldFontName=NULL;
// This is the list of fixed fonts included in XFree86 4.1.0
TScreenResolution     TDisplayXTerm::XFonts[]=
{
 {  5, 7 },
 {  5, 8 },
 {  6,10 },
 {  6,12 },
 {  6,13 },
 {  7,13 },
 {  7,14 },
 {  8,13 },
 {  8,16 },
 {  9,15 },
 {  9,18 },
 { 10,20 },
 { 10,24 }
};

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
 if ((win.ws_col!=0xFFFF)&&(win.ws_col!=0))
   {// Ok!
    getRows=GetRowsIOCTL;
    getCols=GetColsIOCTL;
   }
 else
   {// Nope, use Window Manipulation escape sequences
    getRows=GetRowsSeq;
    getCols=GetColsSeq;
   }
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
 LOG("GetColsIOCTL() " << win.ws_col);
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

 // Nasty:
 // Lamentably Xterm doesn't return it immediately
 fd_set set;
 struct timeval timeout;
 int ret;

 FD_ZERO(&set);
 FD_SET(TGKeyXTerm::hIn,&set);
 timeout.tv_sec=0;
 timeout.tv_usec=300000;
 ret=TEMP_FAILURE_RETRY(select(FD_SETSIZE,&set,NULL,NULL,&timeout));
 // Note: As this feature was reported as potentially exploitable now most
 // XTerms just ignores this request. So if after 300 ms we didn't get an
 // answer we assume the XTerm won't reply and report empty title.
 if (!ret)
    return newStr("");
 fgets(buffer,255,TGKeyXTerm::fIn);
 buffer[255]=0;
 // OSC l Name ST (\E]lName\E\\)
 if (buffer[0]!=27 || buffer[1]!=']' || buffer[2]!='l')
    return NULL;
 // Convert it into something the application can use for other things, not
 // only for restoring. Read: get rid of the EOS.
 char *end=strstr(buffer,"\E\\");
 if (end) *end=0;
 return newStr(buffer+3);
}

int TDisplayXTerm::SetWindowTitle(const char *name)
{
 if (terminalType==Eterm)
    fprintf(stdout,"\E]2;%s\x7",name); // BEL is the string terminator
 else
    fprintf(stdout,"\E]2;%s\E\\",name); // \E\\ is the string terminator
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

// This is supported only by XTerm
char *TDisplayXTerm::GetCurrentFontName()
{
 if (terminalType==Eterm) return NULL;
 char fontNameAux[84];
 fputs("\E]50;?\x7",stdout);
 if (fscanf(TGKeyXTerm::fIn,"\E]50;%80[^\x7]\x7",fontNameAux)==1)
    return newStr(fontNameAux);
 return NULL;
}

int TDisplayXTerm::SetCrtModeXT(unsigned w, unsigned h, int fW, int fH)
{
 const unsigned XFontsCant=sizeof(XFonts)/sizeof(TScreenResolution);
 int fontOK=1, setFont=0;
 unsigned whichFont;
 if (fW!=-1 || fH!=-1)
   {// This is all a guess, I took the GNU/Debian Woody configuration as base.
    // That's XFree86 4.1.0
    if (fW==-1) fW=fontW;
    if (fH==-1) fH=fontH;
    if ((unsigned)fW!=fontW || (unsigned)fH!=fontH)
      {
       setFont=1;
       if (!searchClosestRes(XFonts,fW,fH,XFontsCant,whichFont))
          fontOK=0;
       fontW=XFonts[whichFont].x;
       fontH=XFonts[whichFont].y;
      }
   }

 if (TScreen::screenBuffer)
    DeleteArray(TScreen::screenBuffer);
 TScreen::screenBuffer=new ushort[w*h];
 fprintf(stdout,"\E[8;%d;%dt",h,w);

 if (setFont)
   {
    if (!fontChanged)
      {// Memorize current font
       oldFontName=GetCurrentFontName();
       fontChanged=oldFontName!=NULL;
      }
    fprintf(stdout,"\E]50;%dx%d\x7",fontW,fontH);
   }
 // Ensure that's done before returning. I get wrong width from the IOCTL
 // if I don't flush and ask quickly.
 fflush(stdout);
 return fontOK ? 1 : 2;
}

int TDisplayXTerm::SetCrtModeEt(unsigned w, unsigned h, int fW, int fH)
{
 return SetCrtModeXT(w,h+1,fW,fH);
}
#else

#include <tv/unix/xtscreen.h>
#include <tv/unix/xtkey.h>
#include <tv/linux/log.h>

#endif // TVOS_UNIX && !TVOSf_QNXRtP

