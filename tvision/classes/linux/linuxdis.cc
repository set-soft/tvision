/*****************************************************************************

  Linux display routines.
  Copyright (c) 1996-1998 by Robert Hoehne.
  Copyright (c) 1999-2002 by Salvador E. Tropea (SET)
  Covered by the GPL license.

  Known problems:
* Linux palette handling is quite poor, you can set the 16 colors used by your
VT, you can set the 16 colors used in all the consoles (at the same time and
making them the defaults) and you can get the 16 default colors. But you can't
know the 16 colors used by the current VT. Currently I assume the console is
using the same colors as the rest of the consoles. So I read the default
colors and restore them.

*****************************************************************************/
#include <tv/configtv.h>

#define Uses_stdio
#define Uses_stdlib
#define Uses_unistd
#define Uses_signal
#define Uses_TDisplay
#define Uses_TScreen
#define Uses_string
#define Uses_TGKey
#define Uses_TVCodePage
#include <tv.h>

// I delay the check to generate as much dependencies as possible
#ifdef TVOSf_Linux

#include <termios.h>
//#include <term.h> Not needed anymore
#include <sys/ioctl.h>
// GIO_CMAP ioctl
#include <linux/kd.h>

#include <tv/linux/screen.h>
#include <tv/linux/key.h>
#include <tv/linux/log.h>

int                   TDisplayLinux::curX=0;
int                   TDisplayLinux::curY=0;
// Current cursor shape
int                   TDisplayLinux::cursorStart=86;  //  86 %
int                   TDisplayLinux::cursorEnd  =99;  //  99 %
// 1 when the size of the window where the program is running changed
volatile sig_atomic_t TDisplayLinux::windowSizeChanged=0;
int                   TDisplayLinux::vcsWfd=-1; // virtual console system descriptor
int                   TDisplayLinux::vcsRfd=-1; // Same for reading
int                   TDisplayLinux::hOut=-1;   // Handle for the console output
FILE                 *TDisplayLinux::fOut=NULL;
char                 *TDisplayLinux::origEnvir=NULL;
char                 *TDisplayLinux::newEnvir=NULL;
int                   TDisplayLinux::maxLenTit=0;
char                  TDisplayLinux::tioclinuxOK=0;
char                  TDisplayLinux::cMap[16]={0,4,2,6,1,5,3,7,8,12,10,14,9,13,11,15};
unsigned              TDisplayLinux::oldCurX,
                      TDisplayLinux::oldCurY;
                      // These are the Linux kernel defaults: CP437 font and lat1 map
int                   TDisplayLinux::installedSFM=TVCodePage::PC437;
int                   TDisplayLinux::installedACM=TVCodePage::ISOLatin1Linux;

// All the code is in TScreenLinux, but this is the right moment, by this
// time TScreenLinux is suspended.
TDisplayLinux::~TDisplayLinux()
{
 LOG("TDisplayLinux Destructor");
 TScreenLinux::DeallocateResources();
}

void TDisplayLinux::Init(int mode)
{
 switch (mode)
   {
    case lnxInitVCSrw:
         setCursorPos=SetCursorPosVCS;
         getCursorPos=GetCursorPosVCS;
         break;
    case lnxInitVCSwo:
         setCursorPos=SetCursorPosVCS;
         getCursorPos=GetCursorPosGeneric;
         // Set the cursor to a known position to avoid reading the postion.
         SetCursorPos(0,0);
         break;
    case lnxInitSimple:
         setCursorPos=SetCursorPos;
         getCursorPos=GetCursorPos;
         break;
    case lnxInitMDA:
         setCursorPos=SetCursorPosMDA;
         getCursorPos=GetCursorPosGeneric;
         break;
   }
 if (mode==lnxInitMDA)
   {
    getCursorShape=GetCursorShapeMDA;
    setCursorShape=SetCursorShapeMDA;
    getRows=defaultGetRows;
    getCols=defaultGetCols;
    getDisPaletteColors=defaultGetDisPaletteColors;
    setDisPaletteColors=defaultSetDisPaletteColors;
   }
 else
   {
    getCursorShape=GetCursorShape;
    setCursorShape=SetCursorShape;
    getRows=GetRows;
    getCols=GetCols;
    getDisPaletteColors=tioclinuxOK ? GetDisPaletteColors : defaultGetDisPaletteColors;
    setDisPaletteColors=SetDisPaletteColors;
   }
 checkForWindowSize=CheckForWindowSize;
 getWindowTitle=GetWindowTitle;
 setWindowTitle=SetWindowTitle;

 setUpEnviron();
}

void TDisplayLinux::SetCursorPos(unsigned x, unsigned y)
{
 fprintf(fOut,"\E[%d;%dH",y+1,x+1);
 curX=x; curY=y;
}

// Generic approach, just returns the last value we set
// Used for MDA and VCS when we can't read
void TDisplayLinux::GetCursorPosGeneric(unsigned &x, unsigned &y)
{
 x=curX; y=curY;
}

void TDisplayLinux::GetCursorPos(unsigned &x, unsigned &y)
{
 char s[40];

 fputs("\E[6n",fOut);
 *s=0;
 fgets(s,sizeof(s)-1,TGKeyLinux::fIn); // Response is \E[y;xR

 if (sscanf(s,"\E[%d;%dR",&y,&x)==2)
   {
    x--; y--;
   }
 else
   {
    x=curX; y=curY;
   }
}

void TDisplayLinux::SetCursorShape(unsigned start, unsigned end)
{
 LOG("Setting cursor shape to " << start << "," << end);
 if (start>=end)
   {
    if (!getShowCursorEver())
       fputs("\E[?1c",fOut);
    cursorStart=start;
    cursorEnd=end;
   }
 else
   {// Approximate with the size (1-8)
    int size=(int)((end-start)/12.5)+1;
    if (size>8) size=8;
    fprintf(fOut,"\E[?%dc",size);
    cursorStart=(int)((8-size)*12.5);
    cursorEnd=99;
   }
}

void TDisplayLinux::GetCursorShape(unsigned &start, unsigned &end)
{
 // Currently we don't know the real state.
 start=cursorStart;
 end  =cursorEnd;
}

ushort TDisplayLinux::GetRows()
{
 winsize win;
 win.ws_row=0xFFFF;
 ioctl(hOut,TIOCGWINSZ,&win);
 return win.ws_row!=0xFFFF ? win.ws_row : 25;
}

ushort TDisplayLinux::GetCols()
{
 winsize win;
 win.ws_col=0xFFFF;
 ioctl(hOut,TIOCGWINSZ,&win);
 return win.ws_col!=0xFFFF ? win.ws_col : 80;
}

int TDisplayLinux::CheckForWindowSize(void)
{
 int ret=windowSizeChanged!=0;
 windowSizeChanged=0;
 return ret;
}

const char *TDisplayLinux::GetWindowTitle(void)
{
 if (origEnvir)
    return newStr(origEnvir);
 if (argv)
    return newStr(argv[0]);
 return 0;
}

void TDisplayLinux::setUpEnviron()
{
 // Note: The old glibc provided the real environment/argv vector, now that's
 // a copy :-(

 if (!argv ||        // The application didn't provide argv or
     newEnvir)       // we already initialized
    return;

 origEnvir=argv[0];
 maxLenTit=strlen(argv[0]);
}

int TDisplayLinux::SetWindowTitle(const char *name)
{
 if (!name || !origEnvir)
    return 0;
 int len=strlen(name);
 if (len>=maxLenTit)
   {
    strncpy(origEnvir,name,maxLenTit-1);
    origEnvir[maxLenTit-1]=0;
   }
 else
    strcpy(origEnvir,name);

 return 1;
}

int TDisplayLinux::SetDisPaletteColors(int from, int number, TScreenColor *colors)
{
 // Assume all will be ok
 int ret=number;
 while (number-- && from<16)
   {
    fprintf(fOut,"\E]P%1.1X%2.2X%2.2X%2.2X",cMap[from++],colors->R,colors->G,colors->B);
    colors++;
   }
 return ret;
}

void TDisplayLinux::GetDisPaletteColors(int from, int number, TScreenColor *colors)
{
 uchar vtColors[16*3];
 // Get default VT colors map
 int ret=ioctl(hOut,GIO_CMAP,vtColors),index;
 if (ret==0)
   {
    while (number-- && from<16)
      {
       index=cMap[from++]*3;
       colors->R=vtColors[index];
       colors->G=vtColors[index+1];
       colors->B=vtColors[index+2];
       colors++;
      }
   }
 else
   // Shouldn't happend
   defaultGetDisPaletteColors(from,number,colors);
}

/*****************************************************************************
 VCS specific code
*****************************************************************************/

void TDisplayLinux::SetCursorPosVCS(unsigned x, unsigned y)
{
 unsigned char where[2]={(unsigned char)x,(unsigned char)y};

 lseek(vcsWfd,2,SEEK_SET);
 write(vcsWfd,where,sizeof(where));
 // Cache the value to avoid the need to read it
 curX=x; curY=y;
}

void TDisplayLinux::GetCursorPosVCS(unsigned &x, unsigned &y)
{
 unsigned char where[2];
 
 lseek(vcsRfd,2,SEEK_SET);
 read(vcsRfd,where,sizeof(where));
 x=where[0];
 y=where[1];
}

/*****************************************************************************
  Code to handle a secondary monochrome display, currently not implemented
 but can be added.
*****************************************************************************/

#ifdef h386LowLevel
#if HAVE_OUTB_IN_SYS
 #include <sys/io.h>
#else
 #include <asm/io.h>
#endif

static inline
unsigned char I(unsigned char i)
{
 outb(i,0x3b4);
 return inb(0x3b5);
}

static inline
void O(unsigned char i,unsigned char b)
{
 outb(i,0x3b4);
 outb(b,0x3b5);
}

void TDisplayLinux::SetCursorPosMDA(unsigned x, unsigned y)
{
 unsigned short loc = y*80+x;
 O(0x0e,loc >> 8);
 O(0x0f,loc & 0xff);
}

void TDisplayLinux::SetCursorShapeMDA(unsigned start, unsigned end)
{
 if (start>=end) // cursor off
   {
    O(0x0a,0x01);
    O(0x0b,0x00);
   }
 else
   {
    start=start*16/100;
    end=end*16/100;
    O(0x0a,start);
    O(0x0b,end);
   }
 cursorStart=start;
 cursorEnd=end;
}

void TDisplayLinux::GetCursorShapeMDA(unsigned &start, unsigned &end)
{
 unsigned short ct;
 ct=(I(0x0a)<<8) | I(0x0b);
 if (!ct)
    start=end=0;
 else
   {
    start=86;
    end=99;
   }
}
#else
void TDisplayLinux::SetCursorPosMDA(unsigned , unsigned ) {}
void TDisplayLinux::SetCursorShapeMDA(unsigned start, unsigned end)
{
 cursorStart=start;
 cursorEnd=end;
}

void TDisplayLinux::GetCursorShapeMDA(unsigned &start, unsigned &end)
{
 start=cursorStart;
 end=cursorEnd;
}
#endif // h386LowLevel

#else // TVOSf_Linux

#include <tv/linux/screen.h>
#include <tv/linux/key.h>
#include <tv/linux/log.h>

#endif // else TVOSf_Linux

