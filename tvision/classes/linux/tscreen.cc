/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* Copyright (C) 1998-2001 Salvador Eduardo Tropea */
#define Uses_TScreen
#define Uses_TEvent
#define Uses_TDrawBuffer
#define Uses_TGKey
#define Uses_string
#define Uses_ctype
#include <tv.h>

#include <fcntl.h>
#include <iostream.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdarg.h>
#ifdef __GLIBC__
// Works for glibc 2.0 and glibc 2.1 for Alpha.
#include <sys/perm.h>
#endif

// I don't know why it's needed, but it seems to be only known to me way
// to get rhgdb built so it doesn't mess up terminal settings.
// Strange but rhide and setedit doesn't have this problem.
// Andris Pavenis <pavenis@lanet.lv>
#define SAVE_TERMIOS

int dual_display = 0;
// SET: Starts as suspended to avoid TScreen::suspend() calls and other
// similar stuff before initializing
char TScreen::suspended = 1;
char TScreen::initialized = 0;

#ifdef TVOSf_FreeBSD
#include <ncurses.h>
#else
#include <curses.h>
#endif
#include <term.h>
#include <sys/ioctl.h>

//#define DEBUG
#ifdef DEBUG
#ifdef TVOS_UNIX
extern char *program_invocation_short_name;
#define LOG(s) cerr << program_invocation_short_name << ": " << s << "\r\n"
#else
#define LOG(s) cerr << __FILE__": " << s << "\r\n"
#endif
#else
#define LOG(s)
#endif

static int use_pc_chars = 1;
int timeout_wakeup,timer_value;
#define LINUX_TERMINAL 0
#define GENER_TERMINAL 1
#define VCSA_TERMINAL  2
static int TerminalType;

int vcsWfd=-1;          /* virtual console system descriptor */
int vcsRfd=-1;          /* SET: Same for reading */
int tty_fd=-1;          /* tty descriptor */

/* can I access the MDA ports ? */
int port_access=0;
FILE *tty_file=0;
unsigned short *mono_mem = NULL; /* mmapped mono video mem */
int mono_mem_desc=-1;
extern int cur_x,cur_y;

enum { PAL_MONO, PAL_LOW, PAL_HIGH, PAL_LOW2 };
static int palette;
static int force_redraw = 0;

#if 0 // Not needed right now, but I keep it, SET
/**[txh]********************************************************************

  Description:
  Sends the string to the terminal using printf like arguments. Is just a
replacement for fprintf(tty_file,...) but ensures tty_file was initialized
and does a flush to be sure the characters are flushed to the device.
(SET)

***************************************************************************/

void TScreen::fSendToTerminal(const char *value, ...)
{
 if (!tty_file || !value)
    return;
    
 va_list ap;
     
 va_start(ap,value);
 vfprintf(tty_file,value,ap);
 va_end(ap);
 fflush(tty_file);
}
#endif

/**[txh]********************************************************************

  Description:
  Sends the string to the terminal. Is just a replacement for
fputs(tty_file,...) but ensures tty_file was initialized and does a flush to
be sure the characters are flushed to the device. (SET)

***************************************************************************/

void TScreen::SendToTerminal(const char *value)
{
 if (!tty_file || !value)
    return;
    
 fputs(value,tty_file);
 fflush(tty_file);
}

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
 for (i=128; i<256; i++)
     PC2curses[i]=i;
 // Special characters we can't print
 PC2curses[127]='?';
 PC2curses[128+27]='?';
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
 PC2curses[0xB1]=ACS_CKBOARD; // 50%
 PC2curses[0xB2]=ACS_CKBOARD; // 80%
 PC2curses[0xFE]=ACS_BULLET;
 PC2curses[0x11]=ACS_LARROW;
 PC2curses[0x10]=ACS_RARROW;
 PC2curses[0x19]=ACS_DARROW;
 PC2curses[0x18]=ACS_UARROW;
 // ACS_BOARD is the best choice here but isn't available in Xterms because
 // the DEC graphic chars have only one "gray" character (ACS_CKBOARD)
 PC2curses[0xB0]=ACS_CKBOARD;//ACS_BOARD;
 // The block isn't available in DEC graphics.
 //PC2curses[0xDB]=' ';//ACS_BLOCK;
 PC2curses[0xDB]=ACS_BLOCK;
 PC2curses[0xDC]=' ';
 PC2curses[0xDD]=' ';
 PC2curses[0xDF]=' ';
 // I added the following line even when is supposed to be the default
 // because in some way I managed to break it in Eterm while testing so I
 // think other users could do the same. Explicitly requesting it for G1
 // is the best.
 TScreen::SendToTerminal("\x1B)0"); // Choose DEC characters for G1 set (ISO2022)
}

inline int range(int test, int min, int max)
{
  return test < min ? min : test > max ? max : test;
}

inline void safeput(char *&p, char *cap)
{
  if (cap != NULL) while (*cap != '\0') *p++ = *cap++;
}

void TV_WindowSizeChanged(int sig)
{
 // Set a flag because we don't know if we can do it right now
 TScreen::windowSizeChanged=1;
 signal(sig,TV_WindowSizeChanged);
}

// SET: That's the job of curses endwin(), additionally it does a much more
// complete work so I don't see the point of duplicating work, in my system
// I didn't see any change after removing it, but I left the code just in
// case
#ifdef SAVE_TERMIOS
static struct termios old_term,new_term;
#endif

// SET: A couple of functions to disable/restore the XON/XOFF control keys
// and others like ^C, ^\ and ^Z
static
cc_t oldKeys[5];

void SpecialKeysDisable(int file)
{
 struct termios term;
 tcgetattr(file,&term);
 oldKeys[0]=term.c_cc[VSUSP];
 oldKeys[1]=term.c_cc[VSTART];
 oldKeys[2]=term.c_cc[VSTOP];
 oldKeys[3]=term.c_cc[VQUIT];
 oldKeys[4]=term.c_cc[VINTR];
 term.c_cc[VSUSP] =0;
 term.c_cc[VSTART]=0;
 term.c_cc[VSTOP] =0;
 term.c_cc[VQUIT] =0;
 term.c_cc[VINTR] =0;
 tcsetattr(file,TCSANOW,&term);
}

void SpecialKeysRestore(int file)
{
 struct termios term;
 tcgetattr(file,&term);
 term.c_cc[VSUSP] =oldKeys[0];
 term.c_cc[VSTART]=oldKeys[1];
 term.c_cc[VSTOP] =oldKeys[2];
 term.c_cc[VQUIT] =oldKeys[3];
 term.c_cc[VINTR] =oldKeys[4];
 tcsetattr(file,TCSANOW,&term);
}


// This routine was heavily modified, and I think it needs more work (SET)
void startcurses()
{
  int xterm=0;

  char *terminal = getenv("TERM");
  #ifdef SAVE_TERMIOS
  /* Save the terminal attributes so we can restore them later. */
  /* for the user screen */
  tcgetattr (STDOUT_FILENO, &old_term);
  #endif

  // SET: Original code tried to open a tty at any cost, eg: if stdout was
  // redirected it tried to open stdin's tty for writing. I don't see the
  // point of such an effort and in fact crashes the library, so here I just
  // explain to the user how to run the application in the right way.
  if (!isatty(fileno(stdout)))
    {
     fprintf(stderr,_("\r\nError! that's an interactive application, don't redirect stdout\r\n"
                      "If you want to collect information redirect stderr like this:\r\n\r\n"
                      "program 2> file\r\n\r\n"));
     exit(-1);
    }
  // SET: open it as a different file because we don't want to mix both
  char *tty_name=ttyname(fileno(stdout));
  if (!tty_name)
    {
     fprintf(stderr,"Failed to get the name of the current terminal used for output!\r\n"
                    "Please e-mail to salvador@inti.gov.ar giving details about your setup\r\n"
                    "and the output of the tty command\r\n");
     exit(-1);
    }
  tty_file=fopen(tty_name,"w+b");
  if (!tty_file)
    {
     fprintf(stderr,"Failed to open the %s terminal!\r\n"
                    "Please e-mail to salvador@inti.gov.ar giving details about your setup\r\n"
                    "and the output of the tty command\r\n",tty_name);
     tty_file=fopen(tty_name,"wb");
     if (tty_file)
        fprintf(stderr,"Also tell me it works only for writing\r\n");
     exit(-1);
    }
  tty_fd=fileno(tty_file);

  // old buggy code: if (!newterm(terminal,stdin,tty_file))
  // SET: according to man newterm that's the right order! It was a really
  // hard bug, and produced all kind of wierd behavior (arrow keys failing,
  // endwin not restoring screen, etc.
  if (!newterm(terminal,tty_file,stdin))
    {
     fprintf(stderr,"Not connected to a terminal (newterm for %s)\n",terminal);
     exit(-1);
    }

  // SET: If we are under xterm* initialize some special stuff:
  if (strncmp(terminal,"xterm-eterm-tv",14)==0)
    { // Special keyboard treatment
     TGKey::SetKbdMapping(KBD_ETERM_STYLE);
     xterm=1;
    }
  else
  if (strncmp(terminal,"xterm",5)==0)
    { // Special keyboard treatment
     TGKey::SetKbdMapping(KBD_XTERM_STYLE);
     xterm=1;
    }

  /* Configure curses */
  stdscr->_flags |= _ISPAD;
  // Make curses interpret the escape sequences
  keypad(stdscr, TRUE);
  // SET: remove the buffering and pass the values directly to us. The man
  // pages recomend using it for interactive applications. Looks like it
  // doesn't affect if the delay is 0 but I call it anyways
  cbreak();
  noecho();
  /* set getch() in non-blocking mode */
  timeout(0);

  /* Select the palette and characters handling according to the terminal */
  if (canWriteVCS)
    {
     palette = PAL_HIGH;
     TScreen::screenMode = TScreen::smCO80;
     use_pc_chars = 1;
     TerminalType=VCSA_TERMINAL;
    }
  else
   {
    // Unknown terminal, use monochrome
    if (!terminal)
      {
       palette = PAL_MONO;
       TScreen::screenMode = TScreen::smMono;
       use_pc_chars = 0;
       TerminalType=GENER_TERMINAL;
      }
    else if (!strcmp(terminal,"console") ||
             !strcmp(terminal,"linux"))
      { // Special case where we know the values and that 17 colors are available
       palette = PAL_HIGH;
       TScreen::screenMode = TScreen::smCO80;
       use_pc_chars = 1;
       TerminalType=LINUX_TERMINAL;
       LOG("Using high palette");
      }
    else if (xterm && has_colors())
      { // SET: Here we know the escape sequences and as a plus the bold
        // attribute can be used for another 8 foreground colors
       //palette = PAL_LOW2; Alternative code
       palette = PAL_HIGH;
       TScreen::screenMode = TScreen::smCO80;
       use_pc_chars = 0;
       TerminalType=GENER_TERMINAL;
      }
    else if (has_colors())
      { // Generic color terminal, that's more a guess than a real thing
       palette = max_colors > 8 ? PAL_HIGH : PAL_LOW;
       TScreen::screenMode = TScreen::smCO80;
       use_pc_chars = 0;
       LOG("Using color palette (" << max_colors << "-8 colors)");
       TerminalType=GENER_TERMINAL;
      }
    else
      { // No colors available
       palette = PAL_MONO;
       TScreen::screenMode = TScreen::smMono;
       use_pc_chars = 0;
       TerminalType=GENER_TERMINAL;
      }
   }

  switch (TerminalType)
    {
     case LINUX_TERMINAL:
          // SET: Use G1 charset, set G1 to the loaded video fonts, print control chars.
          // It means the kernel will send what we write directly to screen, just
          // like in the VCSA case.
          TScreen::SendToTerminal("\e)K\xE");
          break;
     case GENER_TERMINAL:
          // Select IBM PC chars?
          TScreen::SendToTerminal(enter_pc_charset_mode);
          break;
    }
  #ifdef SAVE_TERMIOS
  /* Save the terminal attributes so we can restore them later. */
  /* for our screen */
  tcgetattr (tty_fd, &new_term);
  #endif

  // SET: Generate a map to convert special chars into curses values
  InitPCCharsMapping(use_pc_chars);
  // SET: Hook the signal generated when the size of the window changes
  signal(SIGWINCH,TV_WindowSizeChanged);
  #ifdef TVOSf_Linux
  // SET: There is no reason to have 1s of delay for ESC in an interactive
  // program. Now the code supports Alt+Key in xterms so constructing an
  // escape sequence by hand isn't need at all. I also tested in Linux
  // console and I was able to simulate Alt+F pressing ESC-F so 100ms is
  // enough.
  ESCDELAY=100;
  #endif
}

void stopcurses()
{
 // SET: Enhanced the cleanup
 // 1) Undo this nasty trick or curses will fail to do the rest:
 stdscr->_flags &= ~_ISPAD;
 // 2) Now reset the attributes, there is no "default color", is an atribute
 // I know the name of only some attributes: 0=normal (what we need),
 // 1=bold, 4=underline, 5=blink, 7=inverse and I think they are 9:
 TScreen::SendToTerminal(tparm(set_attributes,0,0,0,0,0,0,0,0,0));
 if (!TScreen::suspended)
   {
    // 3) Clear the screen
    clear();
    refresh();
    // 4) Set usable settings (like new line mode)
    resetterm();
    // 5) Enable the echo or the user won't see anything
    echo();
    // Now we can finally end
    endwin();
   }
 fclose(tty_file);
 
 #ifdef SAVE_TERMIOS
 tcsetattr (STDOUT_FILENO, TCSANOW, &old_term);
 #endif
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
#define SB set_a_background ? set_a_background : set_background
#define SF set_a_foreground ? set_a_foreground : set_foreground
  switch (palette)
    {
     // SET: That's a guess because we don't know how the colors are mapped
     // It works for xterm*
     case PAL_LOW:
          fore&=7;
          if (fore==back)
             fore=(fore+1) & 7;    /* kludge */
          if (back!=old_back)
             safeput(p,tparm(SB,back));
          if (fore!=old_fore)
             safeput(p,tparm(SF,fore));
          break;
     /* SET: That's an alternative code for PAL_HIGH
     case PAL_LOW2:
          if (fore!=old_fore || back!=old_back)
            {
             if (fore>7)
                sprintf(p,"\x1B[%d;%d;1m",map[(fore & 7)]+30,map[back]+40);
             else // Reset attributes and then do the rest
                sprintf(p,"\x1B[0;%d;%dm",map[fore]+30,map[back]+40);
             p+=strlen(p);
            }
          break;*/
     // SET: The value to reset the bold is 22 and not 21
     case PAL_HIGH:
          if (fore!=old_fore && back!=old_back)
             sprintf(p,"\e[%d;%d;%dm",fore>7 ? 1 : 22,
                     30+map[fore & 7],40+map[back]);
          else
           {
            if (fore!=old_fore)
               sprintf(p,"\e[%d;%dm",fore>7 ? 1 : 22,30+map[fore & 7]);
            else
               sprintf(p,"\e[%dm",40+map[back]);
           }
          p+=strlen(p);
          break;
    }
  old_fore = fore;
  old_back = back;
#undef SB
#undef SF
}


/*
 * Writes a block of text.
 */

static void writeBlock(int dst, int len, ushort *old, ushort *src)
{
  char out[4096], *p = out;
  int col=-1;
  int altSet=0,needAltSet;

  safeput(p,tparm(cursor_address,dst/TScreen::screenWidth,
          dst%TScreen::screenWidth));
  while (len-->0)
    {
     int code  =CLY_Low16(*src);
     int newcol=CLY_High16(*src);
   
     *old++=*src++;
     if (col!=newcol) /* change color ? */
       {
        col = newcol;
        if (palette>PAL_MONO)
           mapColor(p, col);
        else if (palette==PAL_MONO)
          {
           safeput(p, exit_attribute_mode);
           if (col==0x0f)
              safeput(p, enter_bold_mode);
           else
             if (col==0x70)
                safeput(p, enter_reverse_mode);
          }
       }
 
     switch (TerminalType)
       {
        case GENER_TERMINAL:
             code=PC2curses[code];
             needAltSet=code & A_ALTCHARSET;
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
             *p++=code;
             break;
        case LINUX_TERMINAL:
             /* SET: The following uses information I got from Linux kernel.
                The file drivers/char/console.c have all the parsing of the
                escape sequences. I put more information at the end of this
                file.
                Some characters are ever interpreted as control codes, here
                I use a bitmap for them taked directly from the kernel.
                The kernel code also sugest to use UTF-8 to print those
                codes.
                Note that Unicode 0xF000 | character means that character
                must pass unchanged to the screen
             */
             #define CTRL_ALWAYS 0x0800f501  /* Cannot be overridden by disp_ctrl */
             #define ENTER_UTF8  "\e%G"
             #define EXIT_UTF8   "\e%@"
             if (code<32 && ((CTRL_ALWAYS>>code) & 1))
               {/* This character can't be printed, we must use unicode */
                /* Enter UTF-8 and start constructing 0xF000 code */
                safeput(p,ENTER_UTF8 "\xEF\x80");
                /* Set the last 6 bits */
                *p++=code | 0x80;
                /* Escape UTF-8 */
                safeput(p,EXIT_UTF8);
               }
             else if (code==128+27)
               {/* A specially evil code: Meta+ESC, it can't be printed */
                /* Just send Unicode 0xF09B to screen */
                safeput(p,ENTER_UTF8 "\xEF\x82\x9B" EXIT_UTF8);
               }
             else
                /* The rest pass directly unchanged */
                *p++=code;
             break;
       }
     if (p>out+4000)
       {
        write(tty_fd,out,p-out);
        p=out;
       }
    }
  if (altSet)
     safeput(p,exit_alt_charset_mode);
  if (palette==PAL_MONO)
     safeput(p,exit_attribute_mode);
  safeput(p,tparm(cursor_address,cur_y,cur_x));
  write(tty_fd,out,p-out);
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
volatile sig_atomic_t TDisplay::windowSizeChanged=0;

void SaveScreen();
void RestoreScreen();
void ScreenUpdate();
extern ushort user_mode;

// SET: Enclosed all the I/O stuff in "__i386__ defined" because I don't
// think it have much sense in non-Intel PCs. In fact looks like it gives
// some problems when compiling for Alpha (__alpha__).
//   Also make it only for Linux until I know how to do it for FreeBSD.

#if defined(__i386__) && defined(TVOSf_Linux)
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
 #ifdef h386LowLevel
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
  // Release ^Q and ^S from Xon/Xoff duties
  SpecialKeysDisable(fileno(stdin));
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
  #ifdef TVOSf_FreeBSD
  /*
   * Kludge: until we find a right way to fix the "last-line" display
   * problem, this is a solution.
   */
  screenHeight--;
  #endif
  LOG("screen size is " << (int)screenWidth << "x" << (int)screenHeight);
  screenBuffer = new ushort[screenWidth * screenHeight];

  /* vcs stuff */
  vcsWfd=-1;
  vcsRfd=-1;
  if (strstr(env,"novcs"))
     LOG("vcs support disabled by user");
  else
   {
    /*
     * This approach was suggested by:
     * Martynas Kunigelis <algikun@santaka.sc-uni.ktu.lt>
     * Date: Mon, 20 Jan 1997 15:55:14 +0000 (EET)
     */
    FILE *statfile;
    char path[PATH_MAX];
    bool found_vcsa = false;
    int pid = getpid();
    sprintf(path, "/proc/%d/stat", pid);
    /* If it fails lets see what terminal own our parent */
    while (!found_vcsa &&
           (pid != -1) &&
           ((statfile = fopen(path, "r")) != NULL))
     {
      int dev;
      int ppid;
  
      /* TTYs have 4 as major number */
      /* virtual consoles have minor numbers <= 63 */
  
      fscanf(statfile, "%*d %*s %*c %d %*d %*d %d", &ppid, &dev);

      LOG("ppid: " << ppid << " device: " << dev);
      /* Be a little bit smart, we don't want to bypass X */
      /* X terminals are attached to 0 (unknown) terminal */
      if (dev==0) break;
      
      if ((dev & 0xff00) == 0x0400 && (dev & 0xff) <= 63)
      {
        LOG("virtual console detected");
        sprintf(path, "/dev/vcsa%d", dev & 0xff);
        found_vcsa = true;

        if (TurboVision_screenOptions & TurboVision_screenUserScreenNeeded)
          { // SET: Full VCSA or Curses, not half. See screen.h.
           if ((vcsRfd=vcsWfd=open(path,O_RDWR))<0)
              LOG("unable to open " << path << ", running in stdout mode");
          }
        else
          {
           // SET: Open it with two files, one for write and the other for read.
           // Doing it the administrator can give write access to some users,
           // but not read, that's very secure.
           if ((vcsWfd=open(path,O_WRONLY))<0)
              LOG("unable to open " << path << ", running in stdout mode");
           else
             if ((vcsRfd=open(path,O_RDONLY))<0)
                LOG("only write access to " << path);
          }
      }
      fclose(statfile);
      if (!found_vcsa && (pid != ppid))
      {
        pid = ppid;
        sprintf(path, "/proc/%d/stat", pid);
      }
      else
        pid = -1;
     }
   }
  // SET: Set the cursor to a known position to avoid reading the postion.
  if (canOnlyWriteVCS)
     TDisplay::SetCursor(0,0);

  #ifdef h386LowLevel
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
  //in = out = &queue[0];
  //timeout_auto = -1;
  //timeout_esc = -1;
  timeout_wakeup = timer_value = 0;

  // Cases:
  // 1) we can't read VCSs but can write: then the user screen is a fake.
  // 2) we uses curses then the endwin() does the work and we start from
  // blank
  if (!canReadVCS)
    {
     // Fill the screenBuffer with spaces
     int i,len = screenWidth*screenHeight;
     for (i=0;i<len;i++)
         screenBuffer[i] = 0x0720;
    }

  startcurses();
  SaveScreen();
  setVideoMode(screenMode);
  suspended = 0;
  initialized=1;
}

void TScreen::resume()
{ // Avoid wrong actions
  if (!initialized || !suspended)
     return;
  if (!dual_display)
     SaveScreen();
  setVideoMode(screenMode);

  #ifdef SAVE_TERMIOS
  /* Restore the terminal attributes. */
  /* for our screen */
  tcsetattr(tty_fd, TCSANOW, &new_term);
  #endif

  suspended=0;
  // SET: To go back from a temporal ncurses stop we must use just doupdate
  // or refresh. (see suspend).
  doupdate();
  /* SET: For some bizarre reason when we suspend/resume the charset is altered.
     After it we can't display the frames. I can't understand why it happends.
     It looks like for some reason ncurses is using the IBM PC charset but when
     we use endwin()/doupdate() forgets to set it.
     The following escape sequence does the job:
  */
  switch (TerminalType)
    {
     case LINUX_TERMINAL:
          // Use G1 charset, set G1 to the loaded video fonts, print control chars
          TScreen::SendToTerminal("\e)K\xE");
          break;
     case GENER_TERMINAL:
          // Select IBM PC chars
          TScreen::SendToTerminal("\e(U");
          break;
    }
}

TScreen::~TScreen()
{
  // FIXME: When I know, how to get the cursor state
  setCursorType(0x0607); // make the cursor visible
  stopcurses();

  if (!suspended)
     RestoreScreen();
  suspended=1;

  DeleteArray(screenBuffer);
  LOG("terminated");
  if (vcsWfd>=0)
     close(vcsWfd);
  if (vcsRfd>=0)
     close(vcsRfd);
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
 SpecialKeysRestore(fileno(stdin));
}

void TScreen::suspend()
{
  if (suspended) return;
  old_col = old_back = old_fore = -1;
  // FIXME: When I know, how to get the cursor state
  setCursorType(0x0607); // make the cursor visible
  if (!dual_display)
  {
    // SET: According to "man initscr" we must do endwin() to temporally stop
    // the ncurses engine, I tried it using tty mode (xterm) and vcs mode and
    // seems to work.
    endwin();
    #ifdef SAVE_TERMIOS
    /* Restore the terminal attributes. */
    /* for the user screen */
    tcsetattr (STDOUT_FILENO, TCSANOW, &old_term);
    #endif
    
    RestoreScreen();
  }
  suspended=1;
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
  int oldWidth=screenWidth;
  int oldHeight=screenHeight;
  setCrtMode( fixCrtMode( mode ) );
  setCrtData();
  // allocating a zeroed screenBuffer, because this function
  // is called in most cases (in RHIDE) after a SIGWINCH
  if (screenWidth!=oldWidth || screenHeight!=oldHeight || !screenBuffer)
    {
     // Realloc screen buffer only if actually needed (it doesn't exist
     // or screen size is changed)
     if (screenBuffer)
        DeleteArray(screenBuffer);
     screenBuffer = new ushort[screenWidth*screenHeight];
    }
  memset(screenBuffer,0,screenWidth*screenHeight*sizeof(ushort));
}

void TScreen::setVideoMode( char *mode )
{
  int oldWidth=screenWidth;
  int oldHeight=screenHeight;
  setCrtMode( mode );
  setCrtData();
  // allocating a zeroed screenBuffer, because this function
  // is called in most cases (in RHIDE) after a SIGWINCH
  if (screenWidth!=oldWidth || screenHeight!=oldHeight || !screenBuffer)
    {
     // Realloc screen buffer only if actually needed (it doesn't exist
     // or screen size is changed)
     if (screenBuffer)
        DeleteArray(screenBuffer);
     screenBuffer = new ushort[screenWidth*screenHeight];
    }
  memset(screenBuffer,0,screenWidth*screenHeight*sizeof(ushort));
}

void TScreen::setCursorType(ushort ct)
{
 #ifdef h386LowLevel
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
 #ifdef h386LowLevel
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
  if (canReadVCS)      /* use vcs */
  {
    lseek(vcsRfd, offset * sizeof(ushort) + 4, SEEK_SET);
    read(vcsRfd, buf, count*sizeof(ushort));
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
  if (canWriteVCS)      /* use vcs */
  {
    unsigned length=len*sizeof(ushort);

    lseek(vcsWfd,dst*sizeof(ushort)+4,SEEK_SET);
    write(vcsWfd,src,length);
    if (!canReadVCS)
       // SET: cache it to avoid reads that needs special priviledges
       memcpy(screenBuffer+dst,src,length);
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

/*****************************************************************************

 Blink stuff.

*****************************************************************************/

int blink_use_bios = 1;
int save_text_palette = 0;

void setIntenseState()
{
}

void setBlinkState()
{
}

int getBlinkState()
{ // SET: 1 means blink enabled and hence only 8 colors (was a bug)
  // Report it according to the mode (vcs v.s. tty)
  return canWriteVCS ? 1 : 0;
}

/*****************************************************************************

  Save/Restore screen

*****************************************************************************/

static ushort *user_buffer=0;
static int user_buffer_size;
static int user_cursor_x,user_cursor_y;


void RestoreScreen()
{
 if (canWriteVCS)
   {
    TScreen::setCharacter(0,user_buffer,user_buffer_size);
    TScreen::SetCursor(user_cursor_x,user_cursor_y);
   }
 else
   {
    char b[256],*p=b;
    switch (TerminalType)
      {
       case LINUX_TERMINAL:
       case GENER_TERMINAL:
            // Set color to gray over black
            mapColor(p,7); *p=0;
            TScreen::SendToTerminal(b);
            // Clear the screen to it
            TScreen::SendToTerminal(clear_screen);
            //TScreen::SendToTerminal("\e[2J"); Linux
            break;
      }
   }
}

void SaveScreen()
{
 if (canWriteVCS)
   {
    user_buffer_size = TScreen::getCols()*TScreen::getRows();
    user_buffer = (ushort *)realloc(user_buffer,user_buffer_size*sizeof(ushort));
    TScreen::getCharacter(0,user_buffer,user_buffer_size);
    TScreen::GetCursor(user_cursor_x,user_cursor_y);
   }
}

void SaveScreenReleaseMemory(void)
{
 free(user_buffer);
}

/*****************************************************************************
SET:
  Here is some information about the Linux console. I got it from the kernel
sources, they says that's VT102:

\e(n
Sets the G0 character set. The terminal have 2 character sets, that's the
commonly used. The n value can be:
0 Graphics (DEC ISO 2022)
B Latin 1  (ISO 8859-1)
U IBM PC
K User

\e)n
It sets what character set is used for G1. That's the alternative charset.

\xF
Select the G0 charset, don't display control characters. After it all printed
values are in G0 charset.

\xE
Select the G1 charset, display control characters. After it all printed
values are in G1 charset. Usually it selects the graphics chars.

\e[10m
Selects G0 or G1 (depending of the last selection done by \xF or \xE), don't
display control characters, don't force the bit 7 to 1.

\e[11m
Selects IBM PC, display control characters, don't force the bit 7 to 1.

\e[12m
Selects IBM PC, display control characters, force the bit 7 to 1.

Description of the available charsets for Linux:
1) Graphics (0):
 This is latin 1 charset with some changes in the range 32-126. This charset
is called VT100. Here are the codes that differ from ASCII:
 43(+): Right arrow                    109(m): simple lower left corner
 44(,): Left arrow                     110(n): simple intersection
 45(-): Up arrow                       111(o): first line filled
 46(.): Down arrow                     112(p): 1/4 of height line filled
 48(0): Filled block                   113(q): 1/2 of height line filled
 95(_): Non break space                114(r): 3/4 of height line filled
 96(`): Diamond                        115(s): last line filled
 97(a): 50% filled.                    116(t): simple vertical with right int.
 98(b): HT                             117(u): simple vertical with left int.
 99(c): FF                             118(v): simple horiz. with upper int.
100(d): CR                             119(w): simple horiz. with lower int.
101(e): LF                             120(x): simple vertical
102(f): Degree                         121(y): <=
103(g): +/-                            122(z): >=
104(h): 25% filled.                    123({): pi
105(i): VT                             124(|): /=
106(j): simple lower right corner      125(}): pounds
107(k): simple upper right corner      126(~): middle point
108(l): simple upper left corner

2) Latin 1 (B):
 That's pure ISO 8859-1 mapping. It means 128-159 have undefined shapes.

3) IBM PC (U):
 That's PC 437 code page. Some characters are undefined if the currently
loaded fonts doesn't have it.

4) User defined (K):
  That's just "send to screen this code". So it depends on the loaded fonts.

 Note that 0-31 and 127 are considered control characters and will be printed
only if this is enabled. Some characters aren't printed even if that's
enabled, that's because if they are printed you can't use the terminal
(imagine disabling ESC ;-).
 Also: 128+27 will never be printed.

 In Eterm only Graphics (VT100) and Latin 1 are supported.
 In xterm a simple program have troubles to print anything above 127.
    
*****************************************************************************/
