/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#ifdef __linux__

#define Uses_TScreen
#define Uses_TEvent
#define Uses_TDrawBuffer
#define Uses_TGKey
#include <tv.h>

#include <ctype.h>
#include <fcntl.h>
#include <iostream.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#ifdef __GLIBC__
#include <sys/perm.h>
#endif

int dual_display = 0;

#ifdef __FreeBSD__
#include <ncurses.h>
#else
#include <curses.h>
#endif
#include <term.h>
#include <sys/ioctl.h>

#ifdef DEBUG
#ifdef __linux__
extern char *program_invocation_short_name;
#define LOG(s) cerr << program_invocation_short_name << ": " << s << "\n"
#else
#define LOG(s) cerr << __FILE__": " << s << "\n"
#endif
#else
#define LOG(s)
#endif

static int use_pc_chars = 1;
int timeout_wakeup,timer_value;
int vcs_fd=-1;          /* virtual console system descriptor */
int tty_fd=-1; /* tty descriptor */
/* can I access the MDA ports ? */
int port_access=0;
FILE *tty_file;
unsigned short *mono_mem = NULL; /* mmapped mono video mem */
int mono_mem_desc=-1;
#ifndef __SAVECURSOR_OK
extern int cur_x,cur_y;
#endif

enum { PAL_MONO, PAL_LOW, PAL_HIGH };
static int palette;
static int force_redraw = 0;

#define DELAY_SIGALRM 100

/* lookup table to translate characters from pc set to standard ascii */

static unsigned char pctoascii[] =
{
  " OOooooooooo!!!*><|!!O_|^V><--^V !\"#$%&'()*+,-./0123456789:;<=>?"
  "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~d"
  "cueaaaaceeeiiiaaeafooouuyOUcLYPfaiounN--?--//!<>:%%|{+++++I+'++."
  "`++}-+++`.+++=+++++++++++'.|-||-abipeouyooooooEn=+><()-=o..Vn2X "
};

static unsigned PC2curses[256];

/**[txh]********************************************************************

  Description:
  This function fixes the PC437->ASCII table. This solution is only partial
because the UNIX terminals usually use ISO-1 and never PC437. This will be
finally fixed when I add the codepage stuff in TVision (currently only
available in my editor). Anyways, it makes the windows look much better in
Xterminals, additionally that's what is supposed to do (ask curses). The
only non-clear stuff is that I'm supposing that the frames are in one of the
character sets (G0 or G1) and not made with some trick. That's true in Linux
and any ISO2022 terminal (Xterminals and DEC almost sure). (SET)

***************************************************************************/

static
void InitPCCharsMapping(int use_pc_chars)
{
 int i;
 // By default take the above translation
 for (i=0; i<256; i++)
    {
     PC2curses[i]=pctoascii[i];
    }
 if (use_pc_chars)
    return;
 // Patch the curses available values from terminfo
 PC2curses[0xDA]=ACS_ULCORNER; // Ú
 PC2curses[0xC9]=ACS_ULCORNER; // É We don't have doubles in curses
 PC2curses[0xC0]=ACS_LLCORNER; // À
 PC2curses[0xC8]=ACS_LLCORNER; // È
 PC2curses[0xBF]=ACS_URCORNER; // ¿
 PC2curses[0xBB]=ACS_URCORNER; // »
 PC2curses[0xD9]=ACS_LRCORNER; // Ù
 PC2curses[0xBC]=ACS_LRCORNER; // ¼

 PC2curses[0xC3]=ACS_LTEE;     // Ã
 PC2curses[0xCC]=ACS_LTEE;     // Ì
 PC2curses[0xC6]=ACS_LTEE;     // Æ
 PC2curses[0xC7]=ACS_LTEE;     // Ç

 PC2curses[0xB4]=ACS_RTEE;     // ´
 PC2curses[0xB9]=ACS_RTEE;     // ¹
 PC2curses[0xB5]=ACS_RTEE;     // µ
 PC2curses[0xB6]=ACS_RTEE;     // ¶

 PC2curses[0xC1]=ACS_BTEE;     // Á
 PC2curses[0xCA]=ACS_BTEE;     // Ê
 PC2curses[0xCF]=ACS_BTEE;     // Ï
 PC2curses[0xD0]=ACS_BTEE;     // Ð

 PC2curses[0xC2]=ACS_TTEE;     // Â
 PC2curses[0xCB]=ACS_TTEE;     // Ë
 PC2curses[0xD1]=ACS_TTEE;     // Ñ
 PC2curses[0xD2]=ACS_TTEE;     // Ò

 PC2curses[0xC4]=ACS_HLINE;    // Ä
 PC2curses[0xCD]=ACS_HLINE;    // Í
 PC2curses[0xB3]=ACS_VLINE;    // ³
 PC2curses[0xBA]=ACS_VLINE;    // º
 PC2curses[0xC5]=ACS_PLUS;
 PC2curses[0x04]=ACS_DIAMOND;
 PC2curses[0xB1]=ACS_CKBOARD;
 PC2curses[0xFE]=ACS_BULLET;
 PC2curses[0x11]=ACS_LARROW;
 PC2curses[0x10]=ACS_RARROW;
 PC2curses[0x19]=ACS_DARROW;
 PC2curses[0x18]=ACS_UARROW;
 // ACS_BOARD is the best choice here but isn't available in Xterms because
 // the DEC graphic chars have only one "gray" character (ACS_CKBOARD)
 PC2curses[0xB0]=ACS_CKBOARD;//ACS_BOARD;
 // The block isn't available in DEC graphics.
 PC2curses[0xDB]=' ';//ACS_BLOCK;
 PC2curses[0xDC]=' ';
 PC2curses[0xDD]=' ';
 PC2curses[0xDF]=' ';
 // I added the following line even when is supposed to be the default
 // because in some way I managed to break it in Eterm while testing so I
 // think other users could do the same. Explicitly requesting it for G1
 // is the best.
 write(tty_fd,"\x1B)0",3); // Choose DEC characters for G1 set (ISO2022)
}

inline int range(int test, int min, int max)
{
  return test < min ? min : test > max ? max : test;
}

inline void safeput(char *&p, char *cap)
{
  if (cap != NULL) while (*cap != '\0') *p++ = *cap++;
}

static struct termios old_term,new_term;


void startcurses()
{
  char *terminal = getenv("TERM");
  /* Save the terminal attributes so we can restore them later. */
  /* for the user screen */
  tcgetattr (STDOUT_FILENO, &old_term);

  char *tty = ttyname(fileno(stdout));
  char *ttyi = ttyname(fileno(stdin));
  if (ttyi || tty)
  {
    if (tty)
      tty_file = fopen(tty,"w+b");
    else
      tty_file = fopen(ttyi,"w+b");
  }
  else
  {
    fprintf(stderr,"Not connected to a terminal\n");
    exit(-1);
  }
  if (!tty_file)
  {
    fprintf(stderr,"Could not open terminal '%s'\n", tty ? tty : ttyi);
    exit(-1);
  }
  tty_fd = fileno(tty_file);
  if (!newterm(terminal,stdin,tty_file))
  {
    fprintf(stderr,"Not connected to a terminal (newterm for %s)\n",terminal);
    exit(-1);
  }
  if (strncmp(terminal,"xterm",5)==0)
     TGKey::SetKbdMapping(KBD_XTERM_STYLE);
  stdscr->_flags |= _ISPAD;
#if 1
  keypad(stdscr, TRUE);
#endif
#if 0
  cbreak();
  noecho();
#endif
  timeout(0);   /* set getch() in non-blocking mode */
#if 1
  if (vcs_fd < 0)
  {
    char buf[256], *p = buf;
    safeput(p,enter_pc_charset_mode);
    write(tty_fd, buf, p - buf);
  }
#endif
  if (vcs_fd >= 0)
  {
    palette = PAL_HIGH;
    TScreen::screenMode = TScreen::smCO80;
    use_pc_chars = 1;
  }
  else
  {
    if (!terminal)
    {
      palette = PAL_MONO;
      TScreen::screenMode = TScreen::smMono;
      use_pc_chars = 0;
    }
    else if (!strcmp(terminal,"console") ||
             !strcmp(terminal,"linux"))

    {
      palette = PAL_HIGH;
      TScreen::screenMode = TScreen::smCO80;
      use_pc_chars = 1;
    }
    else if (has_colors())
    {
      palette = PAL_HIGH;
      TScreen::screenMode = TScreen::smCO80;
      use_pc_chars = 0;
    }
    else
    {
      palette = PAL_MONO;
      TScreen::screenMode = TScreen::smMono;
      use_pc_chars = 0;
    }
  }
  /* Save the terminal attributes so we can restore them later. */
  /* for our screen */
  tcgetattr (tty_fd, &new_term);
  InitPCCharsMapping(use_pc_chars);
}

void stopcurses()
{
  endwin();
  fclose(tty_file);
}

/*
 * Converts colors from the large TV palette (16 foreground colors and
 * 16 background colors) to ncurses small palette (8 foreground colors
 * and 8 background colors).
 */

static int old_col = -1;
static int old_fore=-1,old_back=-1;

static void mapColor(char *&p, int col)
{
  static char map[] = {0, 4, 2, 6, 1, 5, 3, 7};
  int back,fore;
  if (col == old_col)
    return;
  old_col = col;
  back = (col >> 4) & 7;
  fore = col & 15;
  if (palette == PAL_LOW)
  {
    fore &= 7;

    if (fore == back) fore = (fore + 1) & 7;    /* kludge */
//    if (fore != old_fore)
    safeput(p, tparm(set_foreground,map[fore]));
//    if (back != old_back)
    safeput(p, tparm(set_background,map[back]));
  }
  else
  {
    if (fore != old_fore && back != old_back)
      sprintf(p,"\033[%d;%d;%dm",fore>7?1:21,30+map[fore%8],40+map[back]);
    else
    {
      if (fore != old_fore)
        sprintf(p,"\033[%d;%dm",fore>7?1:21,30+map[fore%8]);
      else
        sprintf(p,"\033[%dm",40+map[back]);
    }
    p += strlen(p);
  }
  old_fore = fore;
  old_back = back;
}


/*
 * Writes a block of text.
 */

static void writeBlock(int dst, int len, ushort *old, ushort *src)
{
  char out[4096], *p = out;
  int col = -1;
  int altSet=0,needAltSet;

#if 0
#define ___C() write(2,out,p-out)
#else
#define ___C()
#endif

#define __C()\
  do { write(tty_fd, out, p - out); ___C(); } while(0)

#define _C() \
  if (p > out+4000) \
  { \
    __C();\
    p = out; \
  }

#ifdef __SAVECURSOR_OK
  safeput(p, save_cursor );
#endif
  safeput(p, tparm(cursor_address, dst / TScreen::screenWidth,
          dst % TScreen::screenWidth));
  while (len-- > 0)
  {
    int code = *src & 0xff;
    int newcol = *src >> 8;
  
    *old++ = *src++;
    if (col == -1 || col != newcol) /* change color ? */
    {
      col = newcol;
      if (palette == PAL_HIGH || palette == PAL_LOW)
        mapColor(p, col);
      else if (palette == PAL_MONO)
      {
        safeput(p, exit_attribute_mode);
        if (col == 0x0f) safeput(p, enter_bold_mode);
        else if (col == 0x70)
                safeput(p, enter_reverse_mode);
      }
    }

    if (!use_pc_chars || code < ' ')
      {
       code=PC2curses[code];
       //code=pctoascii[code];
       needAltSet=code & WA_ALTCHARSET;
       if (needAltSet && !altSet)
         {
          altSet=1;
          safeput(p,enter_alt_charset_mode);
         }
       else
         if (!needAltSet && altSet)
           {
            altSet=0;
            safeput(p,exit_alt_charset_mode);
           }
      }
    *p++ = code;
    _C();
  }
  if (altSet)
     safeput(p,exit_alt_charset_mode);
#ifdef __SAVECURSOR_OK
  safeput(p, restore_cursor);
#else
  safeput(p, tparm(cursor_address, cur_y, cur_x));
#endif
  if (palette == PAL_MONO) safeput(p, exit_attribute_mode);
  __C();
}

ushort TScreen::startupMode = 0xFFFF;
ushort TScreen::startupCursor = 0;
ushort TScreen::screenMode = 0xFFFF;
uchar TScreen::screenWidth = 0;
uchar TScreen::screenHeight = 0;
Boolean TScreen::hiResScreen = False;
Boolean TScreen::checkSnow = True;
ushort *TScreen::screenBuffer = 0;
ushort TScreen::cursorLines = 0;

int TScreen_suspended = 1;

void SaveScreen();
void RestoreScreen();
void ScreenUpdate();
extern ushort user_mode;

// SET: Enclosed all the I/O stuff in "__i386__ defined" because I don't
// think it have much sense in non-Intel PCs. In fact looks like it gives
// some problems when compiling for Alpha (__alpha__).

#ifdef __i386__
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

void TScreen::GetCursor(int &x,int &y)
{
  if (dual_display)
  {
  }
  else
  {
    TDisplay::GetCursor(x,y);
  }
}

void TScreen::SetCursor(int x,int y)
{
 #ifdef __i386__
  if (dual_display || screenMode == 7)
  {
    unsigned short loc = y*80+x;
    O(0x0e,loc >> 8);
    O(0x0f,loc & 0xff);
  }
  else
 #endif
  {
    TDisplay::SetCursor(x,y);
  }
}

// static int was_blink = 0;
void setBlinkState();
void setIntenseState();
int getBlinkState();


TScreen::TScreen()
{
  screenMode = startupMode = getCrtMode();
  /*
   * ESC ] Ps ND string NP
   *    ND can be any non-digit Character (it's discarded)
   *    NP can be any non-printing Character (it's discarded)
   *    string can be any ASCII printable string (max 511 characters)
   *    Ps = 0 -> use string as a new icon name and title
   *    Ps = 1 -> use string is a new icon name only
   *    Ps = 2 -> use string is a new title only
   *    Ps = 46 -> use string as a new log file name
   */
//  if (getenv("DISPLAY") != NULL) cout << "\033]2;TurboVision\007";
  
  /*
   * Environment variable support.
   * Date: Wed, 29 Jan 1997 16:51:40 +0100 (MET)
   */
  char env[PATH_MAX] = "", *p;
  if ((p = getenv("TVOPT")) != NULL)
  {
     LOG("environment variable TVOPT=" << p);
     for (char *d = env; *p != '\0'; p++)
       *d++ = uctolower(*p);
  }
  
  /* acquire screen size */
  
  winsize win;
  ioctl(STDIN_FILENO, TIOCGWINSZ, &win);
  if (win.ws_col > 0 && win.ws_row > 0)
  {
    screenWidth = win.ws_col;
    screenHeight = win.ws_row;
  }
  else
  {
    LOG("unable to detect screen size");
    screenWidth = 80;
    screenHeight = 25;
  }
  #ifdef __FreeBSD__
  /*
          * Kludge: until we find a right way to fix the "last-line" display
   * problem, this is a solution.
          */
  screenHeight--;
  #endif
  LOG("screen size is " << (int) screenWidth << "x" <<
      (int) screenHeight);
  screenBuffer = new ushort[screenWidth * screenHeight];

  /* vcs stuff */
  
  vcs_fd = -1;
  if (strstr(env, "novcs") != NULL) LOG("vcs support disabled");
  else
  {
    /*
     * This approach was suggested by:
     * Martynas Kunigelis <algikun@santaka.sc-uni.ktu.lt>
     * Date: Mon, 20 Jan 1997 15:55:14 +0000 (EET)
     */
    FILE *statfile;
    char path[PATH_MAX];
    sprintf(path, "/proc/%d/stat", getpid());
    if ((statfile = fopen(path, "r")) != NULL)
    {
      int dev;
  
      /* TTYs have 4 as major number */
      /* virtual consoles have minor numbers <= 63 */
  
      fscanf(statfile, "%*d %*s %*c %*d %*d %*d %d", &dev);
      if ((dev & 0xff00) == 0x0400 && (dev & 0xff) <= 63)
      {
        LOG("virtual console detected");
        sprintf(path, "/dev/vcsa%d", dev & 0xff);
        if ((vcs_fd = open(path, O_RDWR)) < 0)
        {
          LOG("unable to open " << path <<
                  ", running in stdout mode");
        }
      }
      fclose(statfile);
    }
  }

  #ifdef __i386__
  port_access = !ioperm(0x3b4, 7, 1);
  if (port_access)
  {
    unsigned char is_mda = (inb(0x3ba) & 0x70) >> 4;
    is_mda = (is_mda == 0) || (is_mda == 1) || (is_mda == 5);
    if (is_mda)
    {
      mono_mem_desc = open("/dev/mem",O_RDWR);
      if (mono_mem_desc != -1)
      {
        mono_mem = (unsigned short *)mmap(NULL,80*25*2,PROT_READ|PROT_WRITE,
                   MAP_SHARED,mono_mem_desc,0xB0000);
      }
    }
  }
  #endif

  
  /* Don't need special rights anymore */
  seteuid(getuid());
  setegid(getgid());

  /* internal stuff */
  
//  in = out = &queue[0];
//  timeout_auto = -1;
//  timeout_esc = -1;
  timeout_wakeup = timer_value = 0;
  
  if (vcs_fd < 0)
  // Fill the screenBuffer with spaces
  {
    int i,len = screenWidth*screenHeight;
    for (i=0;i<len;i++)
        screenBuffer[i] = 0x0720;
  }
#if 0
  resume();
#else
  startcurses();
  SaveScreen();
  setVideoMode(screenMode);
  TScreen_suspended = 0;
#endif
}

void TScreen::resume()
{
  if (!TScreen_suspended) return;
  if (!dual_display)
  {
    SaveScreen();
  }
  setVideoMode(screenMode);
  /* Restore the terminal attributes. */
  /* for our screen */
  tcsetattr (tty_fd, TCSANOW, &new_term);
  TScreen_suspended = 0;
}

TScreen::~TScreen()
{
#if 0
  suspend();
#else
  // FIXME: When I know, how to get the cursor state
  setCursorType(0x0607); // make the cursor visible
  stopcurses();
  tcsetattr (STDOUT_FILENO, TCSANOW, &old_term);
  if (!TScreen_suspended)
  {
    RestoreScreen();
  }
  TScreen_suspended = 1;
#endif
  delete screenBuffer;
  LOG("terminated");
  if (vcs_fd >= 0) close(vcs_fd);
  if (mono_mem)
  {
    munmap((char *)mono_mem, 80*25*2);
    mono_mem = NULL;
  }
  if (mono_mem_desc != -1)
  {
    close(mono_mem_desc);
    mono_mem_desc = -1;
  }
}

void TScreen::suspend()
{
  if (TScreen_suspended) return;
  old_col = old_back = old_fore = -1;
  // FIXME: When I know, how to get the cursor state
  setCursorType(0x0607); // make the cursor visible
  if (!dual_display)
  {
    /* Restore the terminal attributes. */
    /* for the user screen */
    tcsetattr (STDOUT_FILENO, TCSANOW, &old_term);
    
    RestoreScreen();
  }
  TScreen_suspended = 1;
}

ushort TScreen::fixCrtMode( ushort mode )
{
  return mode;
}

void TScreen::setCrtData()
{
  if (dual_display)
  {
    screenMode = 7;
    screenWidth = 80;
    screenHeight = 25;
    cursorLines = 0x0b0c;
  }
  else
  {
    screenMode = getCrtMode();
    screenWidth = getCols();
    screenHeight = getRows();
    hiResScreen = Boolean(screenHeight > 25);
    if (screenMode == 7)
      cursorLines = 0x0b0c;
    else
      cursorLines = 0x0607;
    setCursorType( 0x2000 );
  }
}

void TScreen::clearScreen()
{
  TDisplay::clearScreen( screenWidth, screenHeight );
}

void TScreen::setVideoMode( ushort mode )
{
  if (screenBuffer)
     delete screenBuffer;
  setCrtMode( fixCrtMode( mode ) );
  setCrtData();
  // allocating a zeroed screenBuffer, because this function
  // is called in most cases (in RHIDE) after a SIGWINCH
  screenBuffer = new ushort[screenWidth * screenHeight];
  memset(screenBuffer,0,screenWidth*screenHeight*sizeof(ushort));
}

// I'm not sure about it check it Robert
void TScreen::setVideoMode( char *mode )
{
  if (screenBuffer)
     delete screenBuffer;
  setCrtMode( mode );
  setCrtData();
  // allocating a zeroed screenBuffer, because this function
  // is called in most cases (in RHIDE) after a SIGWINCH
  screenBuffer = new ushort[screenWidth * screenHeight];
  memset(screenBuffer,0,screenWidth*screenHeight*sizeof(ushort));
}

void TScreen::setCursorType(ushort ct)
{
 #ifdef __i386__
  if (dual_display || screenMode == 7)
  {
    if (ct == 0x2000) // cursor off
    {
      O(0x0a,0x01);
      O(0x0b,0x00);
    }
    else
    {
      O(0x0a,ct >> 8);
      O(0x0b,ct & 0xff);
    }
  }
  else
 #endif
    TDisplay::setCursorType(ct);
}

ushort TScreen::getCursorType()
{
 #ifdef __i386__
  if (dual_display || screenMode == 7)
  {
    unsigned short ct;
    ct = (I(0x0a) << 8) | I(0x0b);
    if (!ct) ct = 0x2000;
    return ct;
  }
  else
 #endif
    return TDisplay::getCursorType();
}

ushort TScreen::getRows()
{
  return dual_display ? 25 : TDisplay::getRows();
}

ushort TScreen::getCols()
{
  return dual_display ? 80 : TDisplay::getCols();
}

void TScreen::getCharacter(unsigned offset,ushort *buf,unsigned count)
{
  if (dual_display)
  {
    memcpy(buf, mono_mem+offset, count*sizeof(ushort));
    return;
  }
  if (vcs_fd >= 0)      /* use vcs */
  {
    lseek(vcs_fd, offset * sizeof(ushort) + 4, SEEK_SET);
    read(vcs_fd, buf, count*sizeof(ushort));
  }
  else                  /* standard out */
  {
    memcpy(buf,screenBuffer+offset,count*sizeof(ushort));
  }
}

ushort TScreen::getCharacter(unsigned dst)
{
  ushort src;
  getCharacter(dst,&src,1);
  return src;
}

void TScreen::setCharacter(unsigned offset,ushort value)
{
  setCharacter(offset,&value,1);
}

/*
 * Draws a line of text on the screen.
 */

void TScreen::setCharacter(unsigned dst,ushort *src,unsigned len)
{
  if (dual_display)
  {
    memcpy(mono_mem+dst, src, len*sizeof(ushort));
    return;
  }
  if (vcs_fd >= 0)      /* use vcs */
  {
    lseek(vcs_fd, dst * sizeof(ushort) + 4, SEEK_SET);
    write(vcs_fd, src, len * sizeof(ushort));
  }
  else                  /* standard out */
  {
    ushort *old = screenBuffer + dst;
    ushort *old_right = old + len - 1;
    ushort *src_right = src + len - 1;

    /* remove unchanged characters from left to right */

    if (!force_redraw)
    {
      while (len > 0 && *old == *src)
      {
        dst++;
        len--;
        old++;
        src++;
      }

      /* remove unchanged characters from right to left */

      while (len > 0 && *old_right == *src_right)
      {
        len--;
        old_right--;
        src_right--;
      }
    }

    /* write only middle changed characters */

    if (len > 0) writeBlock(dst, len, old, src);
  }
}

#endif // __linux__

