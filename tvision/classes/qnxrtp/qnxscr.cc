/* QNX RtP screen routines source.
   Copyright (c) 2002 by Mike Gorchak
   Covered by the BSD license. */

#include <tv/configtv.h>

#define Uses_TScreen
#define Uses_TEvent
#define Uses_TDrawBuffer
#define Uses_TGKey
#define Uses_TVCodePage
#define Uses_string
#define Uses_ctype
#define Uses_stdio
#define Uses_unistd
#define Uses_fcntl
#define Uses_stdlib
#define Uses_sys_stat

#include <tv.h>

// I delay the check to generate as much dependencies as possible
#if defined(TVOS_UNIX) && defined(TVOSf_QNXRtP)

#include <termios.h>
#include <tv/qnxrtp/screen.h>
#include <tv/qnxrtp/key.h>

#include <term.h>
#include <sys/ioctl.h>
#include <ncurses.h>

TScreen* TV_QNXRtPDriverCheck()
{
   TScreenQNXRtP* drv=new TScreenQNXRtP();

   if (!TScreen::initialized)
   {
      delete drv;

      return 0;
   }

   return drv;
}

int TScreenQNXRtP::old_col=-1;
int TScreenQNXRtP::old_fore=-1;
int TScreenQNXRtP::old_back=-1;
int TScreenQNXRtP::force_redraw=0;
cc_t TScreenQNXRtP::oldKeys[5];
struct termios TScreenQNXRtP::old_term;
struct termios TScreenQNXRtP::new_term;

TScreenQNXRtP::TScreenQNXRtP()
{
   int i;
   int len;

   SpecialKeysDisable(fileno(stdin));

   if (!isatty(fileno(stdout)))
   {
      fprintf(stderr,"\r\n");
      fprintf(stderr,_("Error! %s"),_("that's an interactive application, don't redirect stdout. "
              "If you want to collect information redirect stderr like this: "
              "program 2> file"));
      fprintf(stderr,"\r\n");
      return;
   }

   tty_fd=fileno(stdout);

   tcgetattr(fileno(stdout), &old_term);

   // Don't need special rights anymore
   seteuid(getuid());
   setegid(getgid());

   initialized=1;
   if (dCB) dCB();

   TDisplayQNXRtP::Init();

   TScreen::Resume=Resume;
   TScreen::Suspend=Suspend;
   TScreen::setCrtData=setCrtData;
   TScreen::setVideoMode=setVideoMode;
   TScreen::setVideoModeExt=setVideoModeExt;
   TScreen::getCharacters=getCharacters;
   TScreen::getCharacter=getCharacter;
   TScreen::setCharacter=setCharacter;
   TScreen::setCharacters=setCharacters;
   TScreen::System=System;

   TGKeyQNXRtP::Init();

//   THWMouseQNXRtP::Init();

   // Added by SET:
   // Look for user settings
   optSearch("AppCP",forcedAppCP);
   optSearch("ScrCP",forcedScrCP);
   optSearch("InpCP",forcedInpCP);
   // User settings have more priority than detected settings
   codePage=new TVCodePage(forcedAppCP!=-1 ? forcedAppCP : TVCodePage::PC437,
                           forcedScrCP!=-1 ? forcedScrCP : TVCodePage::PC437,
                           forcedInpCP!=-1 ? forcedInpCP : TVCodePage::PC437);
   SetDefaultCodePages(TVCodePage::PC437,TVCodePage::PC437,TVCodePage::PC437);

   suspended=0;

   startupCursor=getCursorType();
   screenMode=startupMode=getCrtMode();

   screenWidth=GetCols();
   screenHeight=GetRows();

   screenBuffer=new ushort[screenWidth * screenHeight];

   len=screenWidth*screenHeight;

   for (i=0; i<len; i++)
   {
      screenBuffer[i] = 0x0720;
   }

   StartNCurses();
   setVideoMode(screenMode);
}

TScreenQNXRtP::~TScreenQNXRtP()
{
   setCursorType(startupCursor);
   stdscr->_flags &= ~_ISPAD;

   TScreenQNXRtP::SendToTerminal(tparm(set_attributes, 0, 0, 0, 0, 0, 0, 0, 0, 0));

   if (!suspended)
   {
      clear();
      RestoreGTables();
      refresh();
      resetterm();
      echo();
      endwin();
   }

   tcsetattr(fileno(stdout), TCSANOW, &old_term);

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
}

void TScreenQNXRtP::Resume()
{
   setVideoMode(screenMode);
   tcsetattr(tty_fd, TCSANOW, &new_term);
   SetGTables();
   doupdate();
}

void TScreenQNXRtP::Suspend()
{
   char out[0x40], *p = out;

   old_col = old_back = old_fore = -1;

   setCursorType(startupCursor);
   RestoreGTables();
 
   mapColor(p, 0x07);                    // White (fg) on black (bg).
   safeput(p, "\x0C");                   // clear screen (QNX RtP specific).
   *p=0x00;
   printf("%s", out);                    // Execute.
   fflush(stdout);
 
   endwin();
   tcsetattr(tty_fd, TCSANOW, &old_term);
   RestoreScreen();
}

void TScreenQNXRtP::setCrtData()
{
   screenMode  =getCrtMode();
   screenWidth =getCols();
   screenHeight=getRows();
   hiResScreen =Boolean(screenHeight > 25);
   cursorLines =getCursorType();
   setCursorType(0);
}

void TScreenQNXRtP::setVideoMode(ushort mode)
{
   int oldWidth=screenWidth;
   int oldHeight=screenHeight;

   setCrtMode(mode);
   setCrtData();

   if (screenWidth!=oldWidth || screenHeight!=oldHeight || !screenBuffer)
   {
      if (screenBuffer)
      {
         DeleteArray(screenBuffer);
         screenBuffer=NULL;
      }
      screenBuffer = new ushort[screenWidth*screenHeight];
   }
   memset(screenBuffer,0,screenWidth*screenHeight*sizeof(ushort));
}

void TScreenQNXRtP::setVideoModeExt(char* mode)
{
   int oldWidth=screenWidth;
   int oldHeight=screenHeight;

   setCrtModeExt(mode);
   setCrtData();

   if (screenWidth!=oldWidth || screenHeight!=oldHeight || !screenBuffer)
   {
      if (screenBuffer)
      {
         DeleteArray(screenBuffer);
         screenBuffer=NULL;
      }
      screenBuffer = new ushort[screenWidth*screenHeight];
   }
   memset(screenBuffer,0,screenWidth*screenHeight*sizeof(ushort));
}

void TScreenQNXRtP::getCharacters(unsigned offset, ushort* buf, unsigned count)
{
   memcpy(buf,screenBuffer+offset,count*sizeof(ushort));
}

ushort TScreenQNXRtP::getCharacter(unsigned dst)
{
   ushort src;

   getCharacters(dst, &src, 1);

   return src;
}

void TScreenQNXRtP::setCharacters(unsigned dst, ushort* src, unsigned len)
{
   ushort *old = screenBuffer + dst;
   ushort *old_right = old + len - 1;
   ushort *src_right = src + len - 1;

   if (!force_redraw)
   {
      while (len > 0 && *old == *src)
      {
         dst++;
         len--;
         old++;
         src++;
      }
      while (len > 0 && *old_right == *src_right)
      {
         len--;
         old_right--;
         src_right--;
      }
   }

   if (len > 0)
   {
      writeBlock(dst, len, old, src);
   }
}

void TScreenQNXRtP::setCharacter(unsigned offset, ushort value)
{
   setCharacters(offset, &value, 1);
}

int TScreenQNXRtP::System(const char *command, pid_t* pidChild, int in,
                          int out, int err)
{
   char *argv[4];

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
   {
      if (setsid()==-1)
      {
         _exit(127);
      }
   
      // If the caller asks for redirection replace the requested handles
      if (in!=-1)
         dup2(in,STDIN_FILENO);
      if (out!=-1)
         dup2(out,STDOUT_FILENO);
      if (err!=-1)
         dup2(err,STDERR_FILENO);

      argv[0]=getenv("SHELL");
      if (!argv[0])
      {
         argv[0]="/bin/sh";
      }
      argv[1]="-c";
      argv[2]=(char *)command;
      argv[3]=0;
      execvp(argv[0],argv);
     _exit(127);
   }

   if (cpid==-1)
   {
      *pidChild=0;

      return system(command);
   }

   *pidChild=cpid;

   return 0;
}

void TScreenQNXRtP::SpecialKeysDisable(int fd)
{
   struct termios terminal;

   tcgetattr(fd, &terminal);
   oldKeys[0]=terminal.c_cc[VSUSP];
   oldKeys[1]=terminal.c_cc[VSTART];
   oldKeys[2]=terminal.c_cc[VSTOP];
   oldKeys[3]=terminal.c_cc[VQUIT];
   oldKeys[4]=terminal.c_cc[VINTR];
   terminal.c_cc[VSUSP] =0;
   terminal.c_cc[VSTART]=0;
   terminal.c_cc[VSTOP] =0;
   terminal.c_cc[VQUIT] =0;
   terminal.c_cc[VINTR] =0;
   tcsetattr(fd, TCSANOW, &terminal);
}

void TScreenQNXRtP::SpecialKeysRestore(int fd)
{
   struct termios terminal;

   tcgetattr(fd, &terminal);
   terminal.c_cc[VSUSP] =oldKeys[0];
   terminal.c_cc[VSTART]=oldKeys[1];
   terminal.c_cc[VSTOP] =oldKeys[2];
   terminal.c_cc[VQUIT] =oldKeys[3];
   terminal.c_cc[VINTR] =oldKeys[4];
   tcsetattr(fd, TCSANOW, &terminal);
}

void TScreenQNXRtP::StartNCurses()
{
   initscr();
   stdscr->_flags |= _ISPAD;
   keypad(stdscr, TRUE);
   cbreak();
   noecho();
   nonl();

   if (has_colors())
   {
      start_color();
   }

   refresh();

   timeout(0);

   SetGTables();

   TScreen::screenMode = TScreen::smCO80;

   tcgetattr(tty_fd, &new_term);

   signal(SIGWINCH, sigWindowSizeChanged);
   
   ESCDELAY=1;
}

void TScreenQNXRtP::SetGTables()
{
   printf("\x0F");              // Select GL to G0
   printf("\x1B""\x7E");        // Select GR to G1
   printf("\x1B""\x28""\x42");  // Select G0 to ASCII charset.
   printf("\x1B""\x29""\x3C");  // Select G1 to supplement charset.
   fflush(stdout);
}

void TScreenQNXRtP::RestoreGTables()
{
   printf("\x0F");              // Select GL to G0
   printf("\x1B""\x7E");        // Select GR to G1
   printf("\x1B""\x28""\x42");  // Select G0 to ASCII charset.
   printf("\x1B""\x29""\x30");  // Select G1 to special charset.
   fflush(stdout);
}

void TScreenQNXRtP::sigWindowSizeChanged(int sig)
{
   TScreenQNXRtP::windowSizeChanged=1;
   signal(sig, sigWindowSizeChanged);
}

void TScreenQNXRtP::SendToTerminal(const char* const value)
{
   if (!value)
   {
      return;
   }
    
   printf(value);
   fflush(stdout);
}

void TScreenQNXRtP::RestoreScreen()
{
   char b[256];
   char* p=b;

   mapColor(p, 0x07);
   *p=0;

   SendToTerminal(b);

   SendToTerminal(clear_screen);
}

void TScreenQNXRtP::mapColor(char* &p, int col)
{
   int back;
   int fore;
   static char map[] = {0, 4, 2, 6, 1, 5, 3, 7};

   if (col == old_col)
   {
      return;
   }

   old_col = col;

   back = (col >> 4) & 7;
   fore = col & 15;
   
   if (fore!=old_fore && back!=old_back)
   {
      sprintf(p, "\x1B""[%d;%d;%dm", fore > 7 ? 1 : 22, 30 + map[fore & 7], 40 + map[back]);
      old_fore = fore;
      old_back = back;
   }
   else
   {
      if (fore!=old_fore)
      {
         sprintf(p,"\x1B""[%d;%dm", fore > 7 ? 1 : 22, 30 + map[fore & 7]);
         old_fore = fore;
      }
      else
      {
         sprintf(p,"\x1B""[%dm", 40 + map[back]);
         old_back = back;
      }
   }
   p+=strlen(p);
}

void TScreenQNXRtP::writeBlock(int dst, int len, ushort *old, ushort *src)
{
   char out[4096];
   char *p = out;
   int col = -1;
   int PCTable=-1;

   safeput(p, tparm(cursor_address, dst / TScreen::screenWidth, dst % TScreen::screenWidth));
   p-=4;
   safeput(p, "\x1B""[?7l");

   while (len-- > 0)
   {
      int code = *src & 0xff;
      int newcol = *src >> 8;
  
      *old++ = *src++;
      if (col == -1 || col != newcol)
      {
         col = newcol;
         mapColor(p, col);
      }

      if (code<0x80)
      {
         if (PCTable!=0)
         {
            PCTable=0;
            safeput(p, "\x1B""[11m");
         }
      }
      else
      {
         if (PCTable!=1)
         {
            PCTable=1;
            safeput(p, "\x1B""[12m");
         }
      }

      if (code==0x1B)
      {
         safeput(p, "\x1B""[10m");
         PCTable=-1;
         *p++ = '-';
      }
      else
      {
         *p++ = code;
      }

      if (p > out+4000)
      {
         write(tty_fd, out, p-out);
         p = out;
      }
   }

   safeput(p, "\x1B""[10m");
   PCTable=-1;

   safeput(p, "\x1B""[?7h");
   safeput(p, tparm(cursor_address, cur_y, cur_x));
   p-=4;

   do {
      write(tty_fd, out, p-out);
   } while(0);
}

#else
// Here to generate the dependencies in RHIDE
#include <tv/qnxrtp/screen.h>
#include <tv/qnxrtp/key.h>
#endif // TVOS_UNIX && TVOSf_QNXRtP
