/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* Copyright (C) 1999-2000 Salvador Eduardo Tropea */
#include <tv/configtv.h>

#ifdef TVOS_UNIX
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

#include <tv/linuxscr.h>

// SET: Enclosed all the I/O stuff in "__i386__ defined" because I don't
// think it have much sense in non-Intel PCs. In fact looks like it gives
// some problems when compiling for Alpha (__alpha__).
//   Also make it only for Linux until I know how to do it for FreeBSD.

#if defined(TVCPU_x86) && defined(TVOSf_Linux)
 // Needed for ioperm, used only by i386.
 // I also noted that glibc 2.1.3 for Alpha, SPARC and PPC doesn't have
 // this header
 #include <sys/perm.h>
 #define h386LowLevel
#endif

#ifdef h386LowLevel
#include <asm/io.h>

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
#endif

int TDisplayUNIX::cur_x=0;
int TDisplayUNIX::cur_y=0;
// SET: Current cursor shape
int TDisplayUNIX::cursorStart=85;  //  85 %
int TDisplayUNIX::cursorEnd  =100; // 100 %

// SET: 1 when the size of the window where the program is running changed
volatile sig_atomic_t TDisplayUNIX::windowSizeChanged=0;

int TDisplayUNIX::vcsWfd=-1; // virtual console system descriptor
int TDisplayUNIX::vcsRfd=-1; // SET: Same for reading
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
 #ifdef h386LowLevel
  if (dual_display /*|| screenMode == 7*/)
  {
    unsigned short loc = y*80+x;
    O(0x0e,loc >> 8);
    O(0x0f,loc & 0xff);
    return;
  }
 #endif

  if (canWriteVCS())
  {
    unsigned char where[2] = {x, y};

    lseek(vcsWfd, 2, SEEK_SET);
    write(vcsWfd, where, sizeof(where));
    // SET: cache the value to avoid the need to read it
    cur_x=x; cur_y=y;
  }
  else			/* standard out */
  {
    char out[1024], *p = out;

    safeput(p, tparm(cursor_address,y, x));
    write(tty_fd, out, p - out);
    cur_x = x;
    cur_y = y;
  }
}

void TDisplayUNIX::GetCursorPos(unsigned &x, unsigned &y)
{
  if (dual_display)
     return;
  if (canWriteVCS()) /* use vcs */
    {
     // SET: We need special priviledges to read /dev/vcsaN, but not for
     // writing so avoid the need of reads.
     if (canReadVCS())
       {
        unsigned char where[2];
    
        lseek(vcsRfd, 2, SEEK_SET);
        read(vcsRfd, where, sizeof(where));
        x = where[0];
        y = where[1];
       }
     else
       {
        x=cur_x; y=cur_y;
       }
    }
  else
  {
    char s[40];
    
    // write/read are better here, because other functions might be buffered
    write(tty_fd,"\e[6n",4); // Request cursor position from terminal
    read(tty_fd,s,sizeof(s)); // Should never overflow...
  
    y = atoi(s+2)-1;
    x = atoi(strchr(s,';')+1)-1;
  }
}

void TDisplayUNIX::SetCursorShape(unsigned start, unsigned end)
{
 #ifdef h386LowLevel
  if (dual_display /*|| screenMode == 7*/)
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
   return;
  }
 #endif
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
}

void TDisplayUNIX::GetCursorShape(unsigned &start, unsigned &end)
{
 #ifdef h386LowLevel
 if (dual_display /*|| screenMode == 7*/)
   {
    unsigned short ct;
    ct = (I(0x0a) << 8) | I(0x0b);
    if (!ct)
       start=end=0;
    else
       { start=80; end=100; }
    return;
   }
 #endif
 // Currently we don't know the real state.
 start=cursorStart;
 end  =cursorEnd;
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
#endif // TVOS_UNIX

