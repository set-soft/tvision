/* QNX 4 screen routines source.
   Copyright (c) 1998-2003 by Mike Gorchak
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
#if defined(TVOS_UNIX) && defined(TVOSf_QNX4)

#include <termios.h>

#include <tv/qnx4/screen.h>
#include <tv/qnx4/mouse.h>
#include <tv/qnx4/key.h>

extern "C"
{
   #include <sys/dev.h>
   #include <sys/term.h>
   #include <sys/qnxterm.h>
}

TScreen* TV_QNX4DriverCheck()
{
   TScreenQNX4* drv=new TScreenQNX4();

   if (!TScreen::initialized)
   {
      delete drv;

      return 0;
   }

   return drv;
}

unsigned char TScreenQNX4::DefaultRadioButton = 0x07;
cc_t TScreenQNX4::oldKeys[5];
ushort TScreenQNX4::oldScreenSizeX=0;
ushort TScreenQNX4::oldScreenSizeY=0;
ushort TScreenQNX4::ForceModeChange=0;
ushort* TScreenQNX4::UserScreenData=NULL;

TScreenQNX4::TScreenQNX4()
{
   char* terminaltype;
   int len;

   terminaltype=getenv("TERM");
   if (terminaltype==NULL)
   {
      return;
   }
   if (strlen(terminaltype)<3)
   {
      return;
   }
   if (memcmp(terminaltype, "qnx", 3)!=0)
   {
      return;
   }

   SpecialKeysDisable(fileno(stdin));

   TDisplayQNX4::Init();

   if (!InitTermLib())
   {
      return;
   }
   
   if (dCB) dCB();

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

   /* save the user screen contents */
   TScreen::screenWidth =getCols();
   TScreen::screenHeight=getRows();

   UserScreenData=new ushort[TScreen::screenWidth*TScreen::screenHeight];
   for (int i=0; i<TScreen::screenHeight; i++)
   {
      term_save_image(i, 0, (char*)(UserScreenData+i*TScreen::screenWidth), TScreen::screenWidth);
   }

   flags0 = CanSetVideoSize | CursorShapes;

   SetVideoMode(screenMode);

   TScreen::screenWidth=GetRows();
   TScreen::screenWidth=GetCols();

   screenBuffer=new ushort[TScreen::screenWidth * TScreen::screenHeight];
   len=TScreen::screenWidth * TScreen::screenHeight;
   for (int i=0; i<len; i++)
   {
      screenBuffer[i] = 0x0720;
   }

   TScreen::Resume=Resume;
   TScreen::Suspend=Suspend;
   TScreen::setCrtData=SetCrtData;
   TScreen::setVideoMode=SetVideoMode;
   TScreen::setVideoModeRes_p=SetVideoModeRes;
   TScreen::setVideoModeExt=SetVideoModeExt;
   TScreen::getCharacters=getCharacters;
   TScreen::getCharacter=getCharacter;
   TScreen::setCharacter=setCharacter;
   TScreen::setCharacters=setCharacters;
   TScreen::System_p=System;
   TScreen::fixCrtMode=FixCrtMode;

   TGKeyQNX4::Init();   
   THWMouseQNX4::Init(ConsoleMode);

   term_flush();   
   initialized=1;
}

TScreenQNX4::~TScreenQNX4()
{
   if ((initialized) && (!suspended))
   {
      term_color(TERM_WHITE | TERM_BLACK_BG);
      term_clear(TERM_CLS_SCR);
      term_flush();
   }

   if (initialized)
   {
      setCursorType(startupCursor);
   }

   if (screenBuffer)
   {
      DeleteArray(screenBuffer);
      screenBuffer=NULL;
   }
   
   SpecialKeysRestore(fileno(stdin));

   setVideoMode(startupMode);

   if (initialized)
   {
      /* restore the user screen contents */
      TScreen::screenWidth =getCols();
      TScreen::screenHeight=getRows();

      if (UserScreenData!=NULL)
      {
         for (int i=0; i<TScreen::screenHeight; i++)
         {
            term_cur(i, 0);
            term_restore_image(i, 0, (char*)(UserScreenData+i*TScreen::screenWidth), TScreen::screenWidth);
         }
         DeleteArray(UserScreenData);
         UserScreenData=NULL;
      }
   }
}

int TScreenQNX4::InitTermLib()
{
   _dev_info_entry DeviceInfo;

   if (term_load()<0)
   {
      return 0;
   }

   term_video_on();
   term_resize_on();
   term_box_off();

   dev_info(fileno(stdin), &DeviceInfo);
   if (strcmp(DeviceInfo.driver_type, "console")==0)
   {
      ConsoleMode=QNX_CONSOLE_RAW;
      DefaultRadioButton=0x07;
   }
   else
   {
      if (getenv("WINDOWID")==NULL)
      {
         ConsoleMode=QNX_CONSOLE_PTERM;
         DefaultRadioButton=0xFE;
      }
      else
      {
         ConsoleMode=QNX_CONSOLE_XQSH;
         DefaultRadioButton=0xFE;
      }
   }

   dev_mode(fileno(stdin), _DEV_OPOST, _DEV_OPOST);
}

void TScreenQNX4::Resume()
{
   SetCrtData();
   
   if ((oldScreenSizeX!=TScreen::screenWidth) || (oldScreenSizeY!=TScreen::screenHeight))
   {
      TDisplayQNX4::ConsoleResizing=1;
      ForceModeChange=1;
   }
   
   for (int i=0; i<oldScreenSizeY; i++)
   {
      term_cur(i, 0);
      term_restore_image(i, 0, (char*)(screenBuffer+i*oldScreenSizeX), oldScreenSizeX);
   }
}

void TScreenQNX4::Suspend()
{
   setCursorType(startupCursor);

   oldScreenSizeX=TScreen::screenWidth;
   oldScreenSizeY=TScreen::screenHeight;

   term_color(TERM_WHITE | TERM_BLACK_BG);
   term_clear(TERM_CLS_SCR);
   term_flush();
}

void TScreenQNX4::SetCrtData()
{
   TScreen::screenMode  = getCrtMode();
   TScreen::screenWidth = getCols();
   TScreen::screenHeight= getRows();
   TScreen::hiResScreen = Boolean(screenHeight > 25);
   TScreen::cursorLines = getCursorType();
   setCursorType(0);
}

void TScreenQNX4::SetVideoMode(ushort mode)
{
   int oldWidth=TScreen::screenWidth;
   int oldHeight=TScreen::screenHeight;
   ushort _rows=25;
   ushort _cols=80;

   SetCrtMode(FixCrtMode(mode));
   SetCrtData();

   if ((oldWidth!=TScreen::screenWidth) || (oldHeight!=TScreen::screenHeight) || (ForceModeChange))
   {
      if (screenBuffer)
      {
         DeleteArray(screenBuffer);
         screenBuffer=NULL;
      }
      screenBuffer = new ushort[screenWidth*screenHeight];

      memset(screenBuffer, 0, screenWidth*screenHeight*sizeof(ushort));

      ForceModeChange=0;
   }
   else
   {
      // restore screen contents if mode not setted up.
      if (screenBuffer!=NULL)
      {
         for (int i=0; i<oldScreenSizeY; i++)
         {
            term_cur(i, 0);
            term_restore_image(i, 0, (char*)(screenBuffer+i*TScreen::screenWidth), TScreen::screenWidth);
         }
      }
   }
}

int TScreenQNX4::SetVideoModeRes(unsigned w, unsigned h, int fW, int fH)
{
   int oldWidth=TScreen::screenWidth;
   int oldHeight=TScreen::screenHeight;

   setCrtModeRes(w, h, fW, fH);
   SetCrtData();

   if ((oldWidth!=TScreen::screenWidth) || (oldHeight!=TScreen::screenHeight) || (ForceModeChange))
   {
      if (screenBuffer)
      {
         DeleteArray(screenBuffer);
         screenBuffer=NULL;
      }
      screenBuffer = new ushort[screenWidth*screenHeight];
      memset(screenBuffer, 0, screenWidth*screenHeight*sizeof(ushort));
      ForceModeChange=0;
   }
   else
   {
      // restore screen contents if mode not setted up.
      for (int i=0; i<oldScreenSizeY; i++)
      {
         term_cur(i, 0);
         term_restore_image(i, 0, (char*)screenBuffer+i*TScreen::screenWidth*2, TScreen::screenWidth);
      }
   }
}

void TScreenQNX4::SetVideoModeExt(char* mode)
{
   int oldWidth=TScreen::screenWidth;
   int oldHeight=TScreen::screenHeight;

   setCrtModeExt(mode);
   SetCrtData();

   if ((oldWidth!=TScreen::screenWidth) || (oldHeight!=TScreen::screenHeight) || (ForceModeChange))
   {
      if (screenBuffer)
      {
         DeleteArray(screenBuffer);
         screenBuffer=NULL;
      }
      screenBuffer = new ushort[screenWidth*screenHeight];
      memset(screenBuffer, 0, screenWidth*screenHeight*sizeof(ushort));
      ForceModeChange=0;
   }
   else
   {
      // restore screen contents if mode not setted up.
      for (int i=0; i<oldScreenSizeY; i++)
      {
         term_cur(i, 0);
         term_restore_image(i, 0, (char*)screenBuffer+i*TScreen::screenWidth*2, TScreen::screenWidth);
      }
   }
}

void TScreenQNX4::getCharacters(unsigned offset, ushort* buf, unsigned count)
{
   memcpy(buf,screenBuffer+offset,count*sizeof(ushort));
}

ushort TScreenQNX4::getCharacter(unsigned dst)
{
   ushort src;

   getCharacters(dst, &src, 1);

   return src;
}

void TScreenQNX4::setCharacters(unsigned dst, ushort* src, unsigned len)
{
   ushort *old = screenBuffer + dst;
   ushort *old_right = old + len - 1;
   ushort *src_right = src + len - 1;

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

   if (len > 0)
   {
      int x = dst % TScreen::screenWidth;
      int y = dst / TScreen::screenWidth;
      unsigned char* oldchar = (unsigned char*) old;
      unsigned char* srcchar = (unsigned char*) src;

      if (ConsoleMode!=QNX_CONSOLE_RAW)
      {
         if (ConsoleMode==QNX_CONSOLE_PTERM)
         {
            unsigned char* oldchar = (unsigned char*) old;
            unsigned char* srcchar = (unsigned char*) src;

            for (int EachSymbol=0; EachSymbol<len; EachSymbol++)
            {
               *(oldchar+EachSymbol*2)=*(srcchar+EachSymbol*2);           // symbol
               *(oldchar+EachSymbol*2+1)=*(srcchar+EachSymbol*2+1);       // attribute
               if (*(oldchar+EachSymbol*2)<0x20)
               {
                  switch (*(oldchar+EachSymbol*2))
                  {
                     case 0x00:
                     case 0x01:
                     case 0x02:
                     case 0x03:
                     case 0x04:
                     case 0x05:
                     case 0x06:
                     case 0x08:
                     case 0x09:
                     case 0x0A:
                     case 0x0B:
                     case 0x0C:
                     case 0x0D:
                                *(oldchar+EachSymbol*2)=0x20;
                                break;
                     case 0x1B: *(oldchar+EachSymbol*2)='-';
                                break;
                     case 0x07: *(oldchar+EachSymbol*2)=TScreenQNX4::DefaultRadioButton;
                                break;
                  }
               }
            }
         }
         else
         {
            unsigned char* oldchar = (unsigned char*) old;
            unsigned char* srcchar = (unsigned char*) src;

            for (int EachSymbol=0; EachSymbol<len; EachSymbol++)
            {
               *(oldchar+EachSymbol*2)=*(srcchar+EachSymbol*2);           // symbol
               *(oldchar+EachSymbol*2+1)=*(srcchar+EachSymbol*2+1);       // attribute
               if (*(oldchar+EachSymbol*2)<0x20)
               {
                  switch(*(oldchar+EachSymbol*2))
                  {
                     case 0x00:
                     case 0x01:
                     case 0x02:
                     case 0x03:
                     case 0x04:
                     case 0x05:
                     case 0x06: *(oldchar+EachSymbol*2)=0x20;
                                break;
                     case 0x07: *(oldchar+EachSymbol*2)=TScreenQNX4::DefaultRadioButton;
                                break;
                     case 0x08:
                     case 0x09:
                     case 0x0A:
                     case 0x0B:
                     case 0x0C:
                     case 0x0D:
                     case 0x0E:
                     case 0x0F: *(oldchar+EachSymbol*2)=0x20;
                                break;
                     case 0x10: *(oldchar+EachSymbol*2)='>';
                                break;
                     case 0x11: *(oldchar+EachSymbol*2)='<';
                                break;
                     case 0x12: *(oldchar+EachSymbol*2)='|';
                                break;
                     case 0x13:
                     case 0x14:
                     case 0x15:
                     case 0x16:
                     case 0x17:
                     case 0x18: *(oldchar+EachSymbol*2)='^';
                                break;
                     case 0x19: *(oldchar+EachSymbol*2)='v';
                                break;
                     case 0x1A: *(oldchar+EachSymbol*2)='<';
                                break;
                     case 0x1B: *(oldchar+EachSymbol*2)='>';
                                break;
                     case 0x1C: *(oldchar+EachSymbol*2)='_';
                                break;
                     case 0x1D: *(oldchar+EachSymbol*2)='-';
                                break;
                     case 0x1E: *(oldchar+EachSymbol*2)='^';
                                break;
                     case 0x1F: *(oldchar+EachSymbol*2)='v';
                                break;
                  }
               }
            }
         }
      }
      else
      {
         memcpy(oldchar, srcchar, len*2);
      }

      if (ConsoleMode!=QNX_CONSOLE_RAW)		// pterm and qnxterm bug elimination
      {
         if (y>TScreen::screenWidth/2)
	 {
	    term_cur(0, 0);
	 }
         else
	 {
            term_cur(TScreen::screenHeight-1, 0);
         }
         term_flush();
	  
         term_cur(y, x);
         term_flush();
      }
      
      term_restore_image(y, x, (char*)old, len);
      term_flush();

      if (ConsoleMode!=QNX_CONSOLE_RAW)
      {
         TDisplayQNX4::SetCursorPos(CursorLastX, CursorLastY);
      }
   }
}

void TScreenQNX4::setCharacter(unsigned offset, ushort value)
{
   setCharacters(offset, &value, 1);
}

int TScreenQNX4::System(const char *command, pid_t* pidChild, int in, int out, int err)
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

void TScreenQNX4::SpecialKeysDisable(int fd)
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

void TScreenQNX4::SpecialKeysRestore(int fd)
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


ushort TScreenQNX4::FixCrtMode(ushort mode)
{
   ushort internalmode=0;
   ushort _rows=25;
   ushort _cols=80;

   switch(mode)
   {
      case smBW80:
      case smCO80:
      case smMono:
                      break;
      case smFont8x8:
                      _rows=50;
                      break;
      case smCO80x28:
                      _rows=28;
                      break;
      case smCO80x35:
                      _rows=35;
                      break;
      case smCO80x40:
                      _rows=40;
                      break;
      case smCO80x43:
                      _rows=43;
                      break;
      case smCO80x50:
                      _rows=50;
                      break;
      case smCO80x30:
                      _rows=30;
                      break;
      case smCO80x34:
                      _rows=34;
                      break;
      case smCO90x30:
                      _cols=90;
                      _rows=30;
                      break;
      case smCO90x34:
                      _cols=90;
                      _rows=34;
                      break;
      case smCO94x30:
                      _cols=94;
                      _rows=30;
                      break;
      case smCO94x34:
                      _cols=94;
                      _rows=34;
                      break;
      case smCO82x25:
                      _cols=82;
                      break;
      case smCO80x60:
                      _rows=60;
                      break;
      case smCO132x25:
                      _cols=132;
                      break;
      case smCO132x43:
                      _cols=132;
                      _rows=43;
                      break;
      case smCO132x50:
                      _cols=132;
                      _rows=50;
                      break;
      case smCO132x60:
                      _cols=132;
                      _rows=60;
                      break;
      default:
                      _cols=(mode & 0x0000FF00UL) >> 8;
                      _rows=(mode & 0x000000FFUL);
   }

   internalmode=_rows | (_cols << 8);

   return internalmode;
}

#else

   // Here to generate the dependencies in RHIDE
   #include <tv/qnx4/screen.h>
   #include <tv/qnx4/key.h>

#endif // TVOS_UNIX && TVOSf_QNX4
