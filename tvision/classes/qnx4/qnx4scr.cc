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

#include <tv/qnx4/screen.h>
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

TScreenQNX4::TScreenQNX4()
{
   char* terminaltype;

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

   TDisplayQNX4::Init();

   if (!InitTermLib())
   {
      return;
   }

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
   
   initialized=1;
}

TScreenQNX4::~TScreenQNX4()
{
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
   term_flush();
   
   dev_info(fileno(stdin), &DeviceInfo);
   if (strcmp(DeviceInfo.driver_type, "console")==0)
   {
      ConsoleMode=True;
      DefaultRadioButton=0x07;
   }
   else
   {
      ConsoleMode=False;
      DefaultRadioButton=0xFE;
   }

   dev_mode(fileno(stdin), _DEV_OPOST, _DEV_OPOST);
}

void TScreenQNX4::Resume()
{
}

void TScreenQNX4::Suspend()
{
}

void TScreenQNX4::setCrtData()
{
}

void TScreenQNX4::setVideoMode(ushort mode)
{
}

void TScreenQNX4::setVideoModeExt(char* mode)
{
}

void TScreenQNX4::getCharacters(unsigned offset, ushort* buf, unsigned count)
{
}

ushort TScreenQNX4::getCharacter(unsigned dst)
{
}

void TScreenQNX4::setCharacters(unsigned dst, ushort* src, unsigned len)
{
}

void TScreenQNX4::setCharacter(unsigned offset, ushort value)
{
   setCharacters(offset, &value, 1);
}

int TScreenQNX4::System(const char *command, pid_t* pidChild, int in, int out, int err)
{
}

#else
// Here to generate the dependencies in RHIDE
#include <tv/qnx4/screen.h>
#include <tv/qnx4/key.h>
#endif // TVOS_UNIX && TVOSf_QNX4
