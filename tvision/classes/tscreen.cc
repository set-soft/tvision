#define Uses_stdio
#define Uses_stdlib
#define Uses_TScreen
#include <tv.h>
#include <tv/drivers.h>

static void dummy() {}

/*****************************************************************************
  Data members initialization
*****************************************************************************/

ushort   TScreen::startupMode=0;
ushort   TScreen::screenMode=0;
ushort   TScreen::startupCursor=0;
ushort   TScreen::cursorLines=0;
uchar    TScreen::screenWidth=80;
uchar    TScreen::screenHeight=25;
Boolean  TScreen::hiResScreen=False;
Boolean  TScreen::checkSnow=False;
ushort  *TScreen::screenBuffer=NULL;
char     TScreen::suspended=1;
char     TScreen::initialized=0;
char     TScreen::initCalled=0;
uint32   TScreen::flags0=0;
TScreen *TScreen::driver=0;

/*****************************************************************************
  Function pointer members initialization
*****************************************************************************/

void   (*TScreen::setVideoMode)(ushort mode)    =TScreen::defaultSetVideoMode;
void   (*TScreen::setVideoModeExt)(char *mode)  =TScreen::defaultSetVideoModeExt;
void   (*TScreen::clearScreen)()                =TScreen::defaultClearScreen;
void   (*TScreen::setCrtData)()                 =TScreen::defaultSetCrtData;
ushort (*TScreen::fixCrtMode)(ushort mode)      =TScreen::defaultFixCrtMode;
void   (*TScreen::Suspend)()                    =dummy;
void   (*TScreen::Resume)()                     =dummy;
ushort (*TScreen::getCharacter)(unsigned offset)=TScreen::defaultGetCharacter;
void   (*TScreen::getCharacters)(unsigned offset, ushort *buf, unsigned count)
                                                =TScreen::defaultGetCharacters;
void   (*TScreen::setCharacter)(unsigned offset, ushort value)
                                                =TScreen::defaultSetCharacter;
void   (*TScreen::setCharacters)(unsigned offset, ushort *values, unsigned count)
                                                =TScreen::defaultSetCharacters;
int    (*TScreen::System)(const char *command, pid_t *pidChild)
                                                =TScreen::defaultSystem;

/*****************************************************************************
  Default behaviors for the members
*****************************************************************************/

void TScreen::defaultSetVideoMode(ushort mode)
{// Set the screen mode
 setCrtMode(fixCrtMode(mode));
 // Cache the data about it and initialize related stuff
 setCrtData();
}

void TScreen::defaultSetVideoModeExt(char *mode)
{// Set the screen mode
 setCrtModeExt(mode);
 // Cache the data about it and initialize related stuff
 setCrtData();
}

void TScreen::defaultClearScreen()
{
 TDisplay::clearScreen(screenWidth,screenHeight);
}

void TScreen::defaultSetCrtData()
{
 screenMode  =getCrtMode();
 screenWidth =getCols();
 screenHeight=getRows();
 hiResScreen =Boolean(screenHeight>25);
 cursorLines =getCursorType();
 setCursorType(0);
}

ushort TScreen::defaultFixCrtMode(ushort mode)
{
 return mode;
}

ushort TScreen::defaultGetCharacter(unsigned offset)
{
 return screenBuffer[offset];
}

void TScreen::defaultGetCharacters(unsigned offset, ushort *buf, unsigned count)
{
 memcpy(buf,screenBuffer+offset,count*sizeof(ushort));
}

void TScreen::defaultSetCharacter(unsigned offset, ushort value)
{
 screenBuffer[offset]=value;
}

void TScreen::defaultSetCharacters(unsigned offset, ushort *values, unsigned count)
{
 memcpy(screenBuffer+offset,values,count*2);
}

int TScreen::defaultSystem(const char *command, pid_t *pidChild)
{
 // fork mechanism not available
 if (pidChild)
    *pidChild=0;
 return system(command);
}

/*****************************************************************************
  Real members
*****************************************************************************/

drvChecker Drivers[]=
{
 #ifdef TVOS_DOS
 TV_DOSDriverCheck,
 #endif
 #ifdef TVOS_UNIX
 TV_XDriverCheck,
 TV_LinuxDriverCheck,
 #endif
 #ifdef TVOS_Win32
 TV_Win32DriverCheck,
 #endif
 0
};

/**[txh]********************************************************************

  Description:
  This constructor is called when the TApplication object is created. The
TProgramInit constructor creates a dynamic copy instance of a TScreen
object.@*
  Actually it calls the detection routines to determine the best driver
available. If non is found prints and error to the stderr output and aborts
the execution.
  
***************************************************************************/

TScreen::TScreen() :
  TDisplay()
{
 // When the real drivers creates a derived class they will call this
 // constructor so we must avoid getting in an infinite loop.
 // I know that's tricky but it helps to maintain compatibility with the
 // old class structure.
 if (initCalled)
    return;
 initCalled=1;

 int i;
 for (i=0; Drivers[i] && !(driver=Drivers[i]()); i++);
 if (!driver)
   {
    fprintf(stderr,"Error: Unsupported hardware\n");
    exit(1);
   }
}

TScreen::~TScreen()
{
 if (initCalled && driver)
   {
    initCalled=0; // Avoid actions in farther calls
    delete driver;
    driver=0;
   }
 else
   //  When we destroy the "driver" member it will call the specific destructor
   // and it will call this destructor again (is a child class). This time we
   // will have initCalled=0 and this suspend will be executed.
   //  The specific destructor should set suspended=1 if this suspend should be
   // disabled.
   suspend();
}

void TScreen::suspend()
{
 if (suspended) return;
 suspended=1;
 Suspend();
}

void TScreen::resume()
{
 if (!suspended) return;
 suspended=0;
 Resume();
}

