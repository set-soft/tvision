/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* Copyright (C) 1998-2003 Salvador Eduardo Tropea */
/*
TODO:
TurboVision_screenOptions
See ToDo in the mouse initialization.

TODO:
Alt+X not working on Woody. Solution: start Eterm with --meta-mod 1.

TODO:
Mouse reporting not disabled at exit!!!

*/
#include <tv/configtv.h>

#define Uses_TScreen
#define Uses_TEvent
#define Uses_TDrawBuffer
#define Uses_TGKey
#define Uses_string
#define Uses_ctype
#define Uses_ioctl
#define Uses_TVCodePage
#define Uses_signal
#define Uses_stdio
#define Uses_stdlib
#include <tv.h>

// I delay the check to generate as much dependencies as possible
#if defined(HAVE_NCURSES) && defined(TVOS_UNIX) && !defined(TVOSf_QNXRtP)

#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>
#include <termios.h>
#include <sys/mman.h>

// I don't know why it's needed, but it seems to be only known to me way
// to get rhgdb built so it doesn't mess up terminal settings.
// Strange but rhide and setedit doesn't have this problem.
// Andris Pavenis <pavenis@lanet.lv>
#define SAVE_TERMIOS
#include <tv/unix/screen.h>
#include <tv/unix/key.h>
#include <tv/unix/mouse.h>
#include <tv/linux/mouse.h>

#ifdef TVOSf_FreeBSD
 #include <ncurses.h>
#else
 #include <curses.h>
#endif
#include <term.h>

//#define DEBUG
#ifdef DEBUG
 #ifdef TVOSf_Linux
  extern char *program_invocation_short_name;
  #define LOG(s) do {cerr << program_invocation_short_name << ": " << s << endl; fflush(stderr);} while(0)
 #else
  #define LOG(s) do {cerr << s << endl; fflush(stderr);} while(0)
 #endif
#else
 #define LOG(s) do {;} while(0)
#endif

int      TScreenUNIX::use_pc_chars=1;
int      TScreenUNIX::TerminalType;
FILE    *TScreenUNIX::tty_file=0;
int      TScreenUNIX::palette;
int      TScreenUNIX::force_redraw=0;
cc_t     TScreenUNIX::oldKeys[5];
int      TScreenUNIX::old_col=-1;
int      TScreenUNIX::old_fore=-1;
int      TScreenUNIX::old_back=-1;
ushort  *TScreenUNIX::user_buffer=0;
int      TScreenUNIX::user_buffer_size;
unsigned TScreenUNIX::user_cursor_x;
unsigned TScreenUNIX::user_cursor_y;
char    *TScreenUNIX::terminal=0;
char     TScreenUNIX::cursesInitialized=0;
char     TScreenUNIX::termAttrsSaved=0;
#ifdef SAVE_TERMIOS
struct termios TScreenUNIX::old_term;
struct termios TScreenUNIX::new_term;
#endif

TScreen *TV_UNIXDriverCheck()
{
 TScreenUNIX *drv=new TScreenUNIX();
 if (!TScreen::initialized)
   {
    delete drv;
    return 0;
   }
 return drv;
}

/**[txh]********************************************************************

Moverlo a un flag en la inicialización

  Description:
  Finds if this terminal have variable code page of that's fixed.
  
  Return: 
  True if the terminal have a variable code page.
  
Boolean TScreen::codePageVariable()
{
 return (TerminalType==GENER_TERMINAL ||
         TerminalType==XTERM_TERMINAL) ? False : True;
}
***************************************************************************/
 

/**[txh]********************************************************************

  Description:
  Sends the string to the terminal. Is just a replacement for
fputs(tty_file,...) but ensures tty_file was initialized and does a flush to
be sure the characters are flushed to the device. (SET)

***************************************************************************/

void TScreenUNIX::SendToTerminal(const char *value)
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

void TScreenUNIX::InitPCCharsMapping()
{
 int i;
 // By default take the above translation
 for (i=0; i<256; i++)
    {
     PC2curses[i]=pctoascii[i];
    }
 if (use_pc_chars==1)
    return;
 for (i=128; i<256; i++)
     PC2curses[i]=i;
 // Special characters we can't print
 PC2curses[127]='?';
 PC2curses[128+27]='?';
 // Terminals where the alternate charset fails (FreeBSD)
 if (use_pc_chars==2)
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
 TScreenUNIX::SendToTerminal("\x1B)0"); // Choose DEC characters for G1 set (ISO2022)
}

void TScreenUNIX::sigWindowSizeChanged(int sig)
{
 // Set a flag because we don't know if we can do it right now
 TScreenUNIX::windowSizeChanged=1;
 signal(sig,sigWindowSizeChanged);
}

void TScreenUNIX::SpecialKeysDisable(int file)
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

void TScreenUNIX::SpecialKeysRestore(int file)
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
void TScreenUNIX::startcurses(int &terminalCodePage)
{
  int xterm=0;

  // SET: If we are under xterm* initialize some special stuff:
  if (strncmp(terminal,"xterm-eterm-tv",14)==0)
    { // Special keyboard treatment
     TGKey::SetKbdMapping(TGKey::unixEterm);
     xterm=1;
    }
  else
  if (strncmp(terminal,"xterm",5)==0 || strncmp(terminal,"Eterm",5)==0)
    { // Special keyboard treatment
     TGKey::SetKbdMapping(TGKey::unixXterm);
     xterm=1;
    }

  /* Configure curses */
  stdscr->_flags |= _ISPAD;
  // Make curses interpret the escape sequences
  keypad(stdscr,TRUE);
  // SET: remove the buffering and pass the values directly to us. The man
  // pages recomend using it for interactive applications. Looks like it
  // doesn't affect if the delay is 0 but I call it anyways
  cbreak();
  noecho();
  /* set getch() in non-blocking mode */
  timeout(0);

  /* Select the palette and characters handling according to the terminal */
  // Unknown terminal, use monochrome
  if (!terminal)
    {
     palette = PAL_MONO;
     TScreenUNIX::screenMode = TScreenUNIX::smMono;
     use_pc_chars = 0;
     TerminalType=GENER_TERMINAL;
    }
  else if (!strcmp(terminal,"console") ||
           !strncmp(terminal,"linux",5))
    { // Special case where we know the values or can make a good guess
     // Assume linux or linux-c
     palette = PAL_HIGH;
     TScreenUNIX::screenMode = TScreenUNIX::smCO80;
     use_pc_chars = 1;
     TerminalType=LINUX_TERMINAL;
     LOG("Using high palette");
     if (strstr(terminal,"-m-")!=NULL || !strcmp(terminal+strlen(terminal)-2,"-m")) // linux-m
       { // Mono mode, explicitly supress colors even if they are available
        palette = PAL_MONO;
        TScreen::screenMode = TScreen::smMono;
       }
     else if (strchr(terminal, '-')!=NULL && // Have a modifier
              !(strstr(terminal,"-c-")!=NULL || !strcmp(terminal+strlen(terminal)-2,"-c"))) // It isn't color modifier
       { // Not some color variation, so most probably a different charset
        use_pc_chars = 0;
        TerminalType=GENER_TERMINAL;
       }
     LOG(palette == PAL_HIGH ? "Using high palette" : "Using mono palette");
    }
  else if (xterm && has_colors())
    { // SET: Here we know the escape sequences and as a plus the bold
      // attribute can be used for another 8 foreground colors
     //palette = PAL_LOW2; Alternative code
     palette = PAL_HIGH;
     TScreenUNIX::screenMode = TScreenUNIX::smCO80;
     use_pc_chars = 0;
     TerminalType=XTERM_TERMINAL;
    }
  else if (has_colors())
    { // Generic color terminal, that's more a guess than a real thing
     palette = (max_colors>8) || !strcmp(terminal,"screen") ? PAL_HIGH : PAL_LOW;
     TScreenUNIX::screenMode = TScreenUNIX::smCO80;
     use_pc_chars = 0;
     LOG("Using color palette (" << max_colors << "-8 colors)");
     TerminalType=GENER_TERMINAL;
    }
  else
    { // No colors available
     palette = PAL_MONO;
     TScreenUNIX::screenMode = TScreenUNIX::smMono;
     use_pc_chars = 0;
     TerminalType=GENER_TERMINAL;
    }
  // SET: Not sure about it: FreeBSD terminals doesn't have frames in the "alternate
  // characters map" or at leat needs more setup for using them.
  if (strncmp(terminal,"cons25",6)==0)
    {
     use_pc_chars=2;
     if (terminal[6]=='r' || terminal[6]=='u')
        terminalCodePage=TVCodePage::KOI8r;
    }

  switch (TerminalType)
    {
     case LINUX_TERMINAL:
          // SET: Use G1 charset, set G1 to the loaded video fonts, print control chars.
          // It means the kernel will send what we write directly to screen, just
          // like in the VCSA case.
          SendToTerminal("\e)K\xE");
          break;
     case XTERM_TERMINAL:
          // SET: I still wondering why I keep using ncurses. The f*ck|ng ncurses
          // puts the keypad in an "application"(?) mode where numbers are never
          // reported.
          SendToTerminal("\e>");
     case GENER_TERMINAL:
          // Select IBM PC chars?
          SendToTerminal(enter_pc_charset_mode);
          #ifdef TVOSf_FreeBSD
          // Disable autowrapping
          SendToTerminal("\e[?7l");
          #endif
          break;
    }
  #ifdef SAVE_TERMIOS
  /* Save the terminal attributes so we can restore them later. */
  /* for our screen */
  tcgetattr (tty_fd, &new_term);
  #endif

  // SET: Generate a map to convert special chars into curses values
  InitPCCharsMapping();
  // SET: Hook the signal generated when the size of the window changes
  signal(SIGWINCH,sigWindowSizeChanged);
}

/*
 * Converts colors from the large TV palette (16 foreground colors and
 * 16 background colors) to ncurses small palette (8 foreground colors
 * and 8 background colors).
 */

void TScreenUNIX::mapColor(char *&p, int col)
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
             safeput(p,tparm(SB,map[back]));
          if (fore!=old_fore)
             safeput(p,tparm(SF,map[fore]));
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

void TScreenUNIX::writeBlock(int dst, int len, ushort *old, ushort *src)
{
  char out[4096], *p = out;
  int col=-1;
  int altSet=0,needAltSet;

  safeput(p,tparm(cursor_address,dst/TScreenUNIX::screenWidth,
          dst%TScreenUNIX::screenWidth));
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
        case XTERM_TERMINAL:
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

TScreenUNIX::TScreenUNIX()
{
  // ************* First check all the things that can make as fail to work
  // Release ^Q and ^S from Xon/Xoff duties
  SpecialKeysDisable(fileno(stdin));
  // SET: Original code tried to open a tty at any cost, eg: if stdout was
  // redirected it tried to open stdin's tty for writing. I don't see the
  // point of such an effort and in fact crashes the library, so here I just
  // explain to the user how to run the application in the right way.
  if (!isatty(fileno(stdout)))
    {
     fprintf(stderr,"\r\n");
     fprintf(stderr,_("Error! %s"),_("that's an interactive application, don't redirect stdout. "
             "If you want to collect information redirect stderr like this: "
             "program 2> file"));
     fprintf(stderr,"\r\n");
     return;
    }
  // SET: open it as a different file because we don't want to mix both
  char *tty_name=ttyname(fileno(stdout));
  if (!tty_name)
    {
     fprintf(stderr,"Failed to get the name of the current terminal used for output!\r\n"
                    "Please e-mail to salvador@inti.gov.ar giving details about your setup\r\n"
                    "and the output of the tty command\r\n");
     return;
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
     return;
    }
  tty_fd=fileno(tty_file);
  #ifdef SAVE_TERMIOS
  /* Save the terminal attributes so we can restore them later. */
  /* for the user screen */
  tcgetattr(STDOUT_FILENO,&old_term);
  termAttrsSaved=1;
  #endif

  terminal=getenv("TERM");
  // old buggy code: if (!newterm(terminal,stdin,tty_file))
  // SET: according to man newterm that's the right order! It was a really
  // hard bug, and produced all kind of wierd behavior (arrow keys failing,
  // endwin not restoring screen, etc.
  if (!newterm(terminal,tty_file,stdin))
    {
     fprintf(stderr,"Not connected to a terminal (newterm for %s)\n",terminal);
     return;
    }
  cursesInitialized=1;
  // ************* If all went ok we can use this driver

  /* Don't need special rights anymore */
  seteuid(getuid());
  setegid(getgid());
  
  initialized=1;
  if (dCB) dCB();

  TDisplayUNIX::Init();

  TScreen::Resume=Resume;
  TScreen::Suspend=Suspend;
  TScreen::setCrtData=setCrtData;
  TScreen::setVideoMode=setVideoMode;
  TScreen::setVideoModeExt=setVideoModeExt;
  TScreen::getCharacters=getCharacters;
  TScreen::getCharacter=getCharacter;
  TScreen::setCharacters=setCharacters;
  TScreen::System_p=System;

  TGKeyUNIX::Init();
  if (terminal && (strncmp(terminal,"xterm",5)==0 || strncmp(terminal,"Eterm",5)==0))
     THWMouseXTerm::Init();
  #ifdef HAVE_GPM
  else
     THWMouseGPM::Init();
  #endif

  startupCursor=getCursorType();
  screenMode=startupMode=getCrtMode();

  /* acquire screen size */
  screenWidth =GetCols();
  screenHeight=GetRows();
  #ifdef TVOSf_FreeBSD
  // Kludge: until we find a right way to fix the "last-line" display
  // problem, this is a solution.
  //screenHeight--;
  #endif
  LOG("screen size is " << (int)screenWidth << "x" << (int)screenHeight);

  screenBuffer=new ushort[screenWidth * screenHeight];

  // Fill the screenBuffer with spaces
  int i,len = screenWidth*screenHeight;
  for (i=0;i<len;i++)
      screenBuffer[i] = 0x0720;

  int terminalCodePage=TVCodePage::ISOLatin1Linux;
  startcurses(terminalCodePage);

  // Look for user settings
  optSearch("AppCP",forcedAppCP);
  optSearch("ScrCP",forcedScrCP);
  optSearch("InpCP",forcedInpCP);
  // User settings have more priority than detected settings
  // That's the most common case and I don't know about any reliable way to
  // find a better default.
  codePage=new TVCodePage(forcedAppCP!=-1 ? forcedAppCP : TVCodePage::ISOLatin1Linux,
                          forcedScrCP!=-1 ? forcedScrCP : terminalCodePage,
                          forcedInpCP!=-1 ? forcedInpCP : TVCodePage::ISOLatin1Linux);
  SetDefaultCodePages(TVCodePage::ISOLatin1Linux,TVCodePage::ISOLatin1Linux,
                      TVCodePage::ISOLatin1Linux);

  setVideoMode(screenMode);
  suspended = 0;

  flags0=CodePageVar | UseScreenSaver;
}

void TScreenUNIX::Resume()
{ // Avoid wrong actions
  setVideoMode(screenMode);

  #ifdef SAVE_TERMIOS
  /* Restore the terminal attributes. */
  /* for our screen */
  tcsetattr(tty_fd, TCSANOW, &new_term);
  #endif

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
          SendToTerminal("\e)K\xE");
          break;
     case XTERM_TERMINAL:
     case GENER_TERMINAL:
          // Select IBM PC chars
          SendToTerminal("\e(U");
          #ifdef TVOSf_FreeBSD
          // Disable autowrapping
          SendToTerminal("\e[?7l");
          #endif
          break;
    }
}

TScreenUNIX::~TScreenUNIX()
{
  if (cursesInitialized)
    {
     // FIXME: When I know, how to get the cursor state
     setCursorType(startupCursor); // make the cursor visible
     #ifdef TVOSf_FreeBSD
     // Enable autowrapping
     SendToTerminal("\e[?7h");
     #endif
     // SET: Enhanced the cleanup
     // 1) Undo this nasty trick or curses will fail to do the rest:
     stdscr->_flags &= ~_ISPAD;
     // 2) Now reset the attributes, there is no "default color", is an atribute
     // I know the name of only some attributes: 0=normal (what we need),
     // 1=bold, 4=underline, 5=blink, 7=inverse and I think they are 9:
     TScreenUNIX::SendToTerminal(tparm(set_attributes,0,0,0,0,0,0,0,0,0));
     if (!TScreenUNIX::suspended)
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
     cursesInitialized=0;
    }
  if (tty_file)
    {
     fclose(tty_file);
     tty_file=0;
    }
  
  #ifdef SAVE_TERMIOS
  if (termAttrsSaved)
    {
     termAttrsSaved=0;
     tcsetattr(STDOUT_FILENO, TCSANOW, &old_term);
    }
  #endif

  if (!suspended)
    {
     RestoreScreen();
     suspended=1;
    }

  if (screenBuffer)
    {
     DeleteArray(screenBuffer);
     screenBuffer=0;
    }
 SpecialKeysRestore(fileno(stdin));

 LOG("terminated");
}

void TScreenUNIX::Suspend()
{
  old_col = old_back = old_fore = -1;
  // FIXME: When I know, how to get the cursor state
  setCursorType(startupCursor);
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
    
    #ifdef TVOSf_FreeBSD
    // Enable autowrapping
    SendToTerminal("\e[?7h");
    #endif
    RestoreScreen();
  }
}

void TScreenUNIX::setCrtData()
{
 screenMode  =getCrtMode();
 screenWidth =getCols();
 screenHeight=getRows();
 hiResScreen =Boolean(screenHeight > 25);
 cursorLines =getCursorType();
 setCursorType(0);
}

void TScreenUNIX::setVideoMode(ushort mode)
{
  int oldWidth=screenWidth;
  int oldHeight=screenHeight;
  setCrtMode(fixCrtMode(mode));
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

void TScreenUNIX::setVideoModeExt( char *mode )
{
  int oldWidth=screenWidth;
  int oldHeight=screenHeight;
  setCrtModeExt(mode);
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

void TScreenUNIX::getCharacters(unsigned offset, ushort *buf, unsigned count)
{
 memcpy(buf,screenBuffer+offset,count*sizeof(ushort));
}

ushort TScreenUNIX::getCharacter(unsigned dst)
{
 ushort src;
 getCharacters(dst,&src,1);
 return src;
}

/*
 * Draws a line of text on the screen.
 */

void TScreenUNIX::setCharacters(unsigned dst, ushort *src, unsigned len)
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

/*****************************************************************************

 Blink stuff.

*****************************************************************************/

//int blink_use_bios = 1;
//int save_text_palette = 0;

/*void setIntenseState()
{
}

void setBlinkState()
{
}

int getBlinkState()
{ // SET: 1 means blink enabled and hence only 8 colors (was a bug)
  // Report it according to the mode (vcs v.s. tty)
  return canWriteVCS() ? 1 : 0;
}*/

/*****************************************************************************

  Save/Restore screen

*****************************************************************************/

void TScreenUNIX::RestoreScreen()
{
 char b[256],*p=b;
 // Set color to gray over black
 mapColor(p,7); *p=0;
 SendToTerminal(b);
 // Clear the screen to it
 SendToTerminal(clear_screen);
}

// SET: Call to an external program, optionally forking
int TScreenUNIX::System(const char *command, pid_t *pidChild, int in,
                         int out, int err)
{
 if (!pidChild)
   {
    // If the caller asks for redirection replace the requested handles
    if (in!=-1)
       dup2(in,STDIN_FILENO);
    if (out!=-1)
       dup2(out,STDOUT_FILENO);
    if (err!=-1)
       dup2(err,STDERR_FILENO);
    return system(command);
   }

 pid_t cpid=fork();
 if (cpid==0)
   {// Ok, we are the child
    //   I'm not sure about it, but is the best I have right now.
    //   Doing it we can kill this child and all the subprocesses
    // it creates by killing the group. It also have an interesting
    // effect that I must evaluate: By doing it this process lose
    // the controlling terminal and won't be able to read/write
    // to the parents console. I think that's good.
    if (setsid()==-1)
       _exit(127);
    char *argv[4];
   
    // If the caller asks for redirection replace the requested handles
    if (in!=-1)
       dup2(in,STDIN_FILENO);
    if (out!=-1)
       dup2(out,STDOUT_FILENO);
    if (err!=-1)
       dup2(err,STDERR_FILENO);
       
    argv[0]=newStr(getenv("SHELL"));
    if (!argv[0])
       argv[0]=newStr("/bin/sh");
    argv[1]=newStr("-c");
    argv[2]=newStr(command);
    argv[3]=NULL;
    execvp(argv[0],argv);
    delete[] argv[0];
    delete[] argv[1];
    delete[] argv[2];
    // We get here only if exec failed
    _exit(127);
   }
 if (cpid==-1)
   {// Fork failed do it manually
    *pidChild=0;
    return system(command);
   }
 *pidChild=cpid;
 return 0;
}

#else

#include <tv/unix/screen.h>
#include <tv/unix/key.h>
#include <tv/unix/mouse.h>
#include <tv/linux/mouse.h>

#endif // TVOS_UNIX && !TVOSf_QNXRtP
/*****************************************************************************

     This code was in the constructor and commented, it could help in the
   future.
   * ESC ] Ps ND string NP
   *    ND can be any non-digit Character (it's discarded)
   *    NP can be any non-printing Character (it's discarded)
   *    string can be any ASCII printable string (max 511 characters)
   *    Ps = 0 -> use string as a new icon name and title
   *    Ps = 1 -> use string is a new icon name only
   *    Ps = 2 -> use string is a new title only
   *    Ps = 46 -> use string as a new log file name
  if (getenv("DISPLAY") != NULL) cout << "\033]2;TurboVision\007";

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
