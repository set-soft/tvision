/**[txh]********************************************************************

  Copyright 2001-2009 by Salvador E. Tropea
  This file is covered by the GPL license.
  
  Module: Screen
  Include: TScreen
  Comments:
  This is the base class for medium level screen output.
  Most members are pointers to functions defined by each platform dependent
driver. Default members are provided.
  This is a complete new file created from concepts that José Angel Sanchez
Caso provide me and coded from scratch. The names of some members are the
same used in original Turbo Vision for compatibility purposes.

***************************************************************************/

#define Uses_stdio
#define Uses_stdlib
#define Uses_string
#define Uses_unistd
#define Uses_AllocLocal
#define Uses_ctype
#define Uses_TScreen
#define Uses_TVConfigFile
#define Uses_TGKey
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
char     TScreen::useSecondaryFont=0;
uint32   TScreen::flags0=0;
TScreen *TScreen::driver=NULL;
const char
        *TScreen::currentDriverShortName=NULL;
TVScreenFontRequestCallBack
         TScreen::frCB=NULL;
long     TScreen::forcedAppCP=-1,
         TScreen::forcedScrCP=-1,
         TScreen::forcedInpCP=-1;
int      TScreen::maxAppHelperHandlers=8;
const char
        *TScreen::windowClass="XTVApp";

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
void   (*TScreen::setCharacter)(unsigned offset, uint32 value)
                                                =TScreen::defaultSetCharacter;
void   (*TScreen::setCharacters)(unsigned offset, ushort *values, unsigned count)
                                                =TScreen::defaultSetCharacters;
int    (*TScreen::System_p)(const char *command, pid_t *pidChild, int in,
                            int out, int err)
                                                =TScreen::defaultSystem;
int    (*TScreen::getFontGeometry)(unsigned &w, unsigned &h)
                                                =TScreen::defaultGetFontGeometry;
int    (*TScreen::getFontGeometryRange)(unsigned &wmin, unsigned &hmin,
                                       unsigned &umax, unsigned &hmax)
                                                =TScreen::defaultGetFontGeometryRange;
int    (*TScreen::setFont_p)(int changeP, TScreenFont256 *fontP,
                             int changeS, TScreenFont256 *fontS,
                             int fontCP, int appCP)
                                                =TScreen::defaultSetFont;
void   (*TScreen::restoreFonts)()               =TScreen::defaultRestoreFonts;
int    (*TScreen::setVideoModeRes_p)(unsigned w, unsigned h, int fW, int fH)
                                                =TScreen::defaultSetVideoModeRes;
TScreen::appHelperHandler (*TScreen::openHelperApp)(AppHelper kind)
                                                =TScreen::defaultOpenHelperApp;
Boolean (*TScreen::closeHelperApp)(appHelperHandler id)
                                                =TScreen::defaultCloseHelperApp;
Boolean (*TScreen::sendFileToHelper)(appHelperHandler id, const char *file, void *extra)
                                                =TScreen::defaultSendFileToHelper;
const char *(*TScreen::getHelperAppError)()     =TScreen::defaultGetHelperAppError;


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

int  TScreen::defaultSetVideoModeRes(unsigned w, unsigned h, int fW, int fH)
{// Set the screen mode
 int ret=setCrtModeRes(w,h,fW,fH);
 if (ret)
    // Cache the data about it and initialize related stuff
    setCrtData();
 return ret;
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
 if (drawingMode==unicode16)
    offset*=2;
 return screenBuffer[offset];
}

void TScreen::defaultGetCharacters(unsigned offset, ushort *buf, unsigned count)
{
 if (drawingMode==unicode16)
    memcpy(buf,screenBuffer+offset*2,count*sizeof(ushort)*2);
 else
    memcpy(buf,screenBuffer+offset,count*sizeof(ushort));
}

void TScreen::defaultSetCharacter(unsigned offset, uint32 value)
{
 uint16 Value=value;
 setCharacters(offset,&Value,1);
}

void TScreen::defaultSetCharacters(unsigned offset, ushort *values, unsigned count)
{
 if (drawingMode==unicode16)
    memcpy(screenBuffer+offset*2,values,count*2*2);
 else
    memcpy(screenBuffer+offset,values,count*2);
}

int TScreen::defaultSystem(const char *command, pid_t *pidChild, int in, int out,
                           int err)
{
 // fork mechanism not available
 if (pidChild)
    *pidChild=0;
 // If the caller asks for redirection replace the requested handles
 if (in!=-1)
    dup2(in,STDIN_FILENO);
 if (out!=-1)
    dup2(out,STDOUT_FILENO);
 if (err!=-1)
    dup2(err,STDERR_FILENO);
 return system(command);
}

int  TScreen::defaultGetFontGeometry(unsigned &, unsigned &) { return 0; }
int  TScreen::defaultGetFontGeometryRange(unsigned &, unsigned &,
                                          unsigned &, unsigned &) { return 0; }
int  TScreen::defaultSetFont(int , TScreenFont256 *, int , TScreenFont256 *,
                             int, int) { return 0; }
void TScreen::defaultRestoreFonts() {}

TScreen::appHelperHandler TScreen::defaultOpenHelperApp(TScreen::AppHelper)
{ return -1; }
Boolean TScreen::defaultCloseHelperApp(appHelperHandler) { return False; }
Boolean TScreen::defaultSendFileToHelper(appHelperHandler, const char *, void *)
{ return False; }
const char *TScreen::defaultGetHelperAppError()
{
 return __("This feature isn't implemented by the current driver.");
}

/*****************************************************************************
  Real members
*****************************************************************************/

struct stDriver
{
 // Test function for this driver
 drvChecker  driver;
 // The drivers with more priority are tried first
 int         priority;
 // Configuration section name for this driver
 const char *name;
};

#ifdef TV_Disable_WinGr_Driver
  #define TV_WinGr_Driver_Entry
#else
  #define TV_WinGr_Driver_Entry { TV_WinGrDriverCheck,  80, "WinGr" },
#endif

static
stDriver Drivers[]=
{
 #ifdef TVOS_DOS
 { TV_DOSDriverCheck, 100, "DOS" },
 #endif

 #ifdef TVOS_UNIX
  #ifdef HAVE_X11
   { TV_XDriverCheck, 100, "X11" },
  #endif // HAVE_X11
  #ifdef TVOSf_Linux
   { TV_LinuxDriverCheck, 90, "Linux" },
  #endif
  #ifdef TVOSf_QNXRtP
   { TV_QNXRtPDriverCheck, 90, "QNX" },
  #else
   #ifdef TVOSf_QNX4
    { TV_QNX4DriverCheck, 90, "QNX4" },
   #endif // TVOSf_QNX4
   { TV_XTermDriverCheck, 60, "XTerm" },
   #ifdef HAVE_NCURSES
   { TV_UNIXDriverCheck, 10, "UNIX" },
   #endif
  #endif // TVOSf_QNXRtP
 #endif

 #ifdef TVOS_Win32
  #ifdef HAVE_X11
   // This is experimental, but believe it or not Cygwin have X11 support
   { TV_XDriverCheck,     100, "X11" },
  #endif // HAVE_X11
  #ifdef TVOSf_NT
   { TV_WinNTDriverCheck,  90, "WinNT" },
   TV_WinGr_Driver_Entry
   { TV_Win32DriverCheck,  50, "Win32" },
  #else
   { TV_Win32DriverCheck,  90, "Win32" },
   TV_WinGr_Driver_Entry
   { TV_WinNTDriverCheck,  50, "WinNT" },
  #endif
 #endif

 #ifdef HAVE_ALLEGRO
   { TV_AlconDriverCheck,  30, "AlCon" },
 #endif
};

const int nDrivers=sizeof(Drivers)/sizeof(stDriver);

static
int cmpDrivers(const void *v1, const void *v2)
{
 int p1=((stDriver *)v1)->priority;
 int p2=((stDriver *)v2)->priority;
 return (p1<p2)-(p1>p2);
}

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

 // Check if the user changed priorities
 int changed=0,i;
 long priority;
 for (i=0; i<nDrivers; i++)
     if (TVMainConfigFile::Search(Drivers[i].name,"Priority",priority))
       {
        Drivers[i].priority=(int)priority;
        changed++;
       }
 // Sort the list if needed
 if (changed)
    qsort(Drivers,nDrivers,sizeof(stDriver),cmpDrivers);
 // Now call the initializations
 for (i=0; i<nDrivers && !driver; i++)
    {
     currentDriverShortName=Drivers[i].name;
     driver=Drivers[i].driver();
    }
 if (!driver)
   {
    fprintf(stderr,"Error: Unsupported hardware\n");
    currentDriverShortName=NULL;
    exit(1);
   }
 long val=0;
 if (optSearch("AvoidMoire",val))
    avoidMoire=val;
 val=0;
 if (optSearch("AltKeysSetting",val))
    TGKey::SetAltSettings(val);
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

void TScreen::getPaletteColors(int from, int number, TScreenColor *colors)
{
 while (number-- && from<16)
    *(colors++)=ActualPalette[from++];
}

void TScreen::setPaletteColors(int from, int number, TScreenColor *colors)
{
 int num=setDisPaletteColors(from,number,colors);
 if (num)
   {
    memcpy(ActualPalette+from,colors,num*sizeof(TScreenColor));
    paletteModified=1;
   }
}

void TScreen::resetPalette()
{
 setDisPaletteColors(0,16,OriginalPalette);
 paletteModified=0;
}

const char *sep=",;";

Boolean TScreen::parseUserPalette()
{
 char *sPal=optSearch("ScreenPalette");
 //printf("parseUserPalette():  %s\n",sPal ? sPal : "None");
 if (!sPal || !*sPal) return False;
 memcpy(UserStartPalette,PC_BIOSPalette,sizeof(UserStartPalette));
 int l=strlen(sPal);
 AllocLocalStr(b,l+1);
 memcpy(b,sPal,l+1);

 char *s=strtok(b,sep),*end;
 int index=0, R, G, B;
 Boolean ret=False;
 while (s)
   {
    for (;*s && isspace(*s); s++);
    R=*s ? strtol(s,&end,0) : UserStartPalette[index].R;

    s=strtok(NULL,sep);
    if (!s) break;
    for (;*s && isspace(*s); s++);
    G=*s ? strtol(s,&end,0) : UserStartPalette[index].G;

    s=strtok(NULL,sep);
    if (!s) break;
    for (;*s && isspace(*s); s++);
    B=*s ? strtol(s,&end,0) : UserStartPalette[index].B;

    UserStartPalette[index].R=R;
    UserStartPalette[index].G=G;
    UserStartPalette[index].B=B;
    //printf("%d: %d,%d,%d\n",index,R,G,B);
    index++;
    ret=True;

    s=strtok(NULL,sep);
   }
 return ret;
}

Boolean TScreen::optSearch(const char *variable, long &val)
{
 if (TVMainConfigFile::Search(currentDriverShortName,variable,val))
    return True;
 // If not found in the driver specific section search in the TV section
 return TVMainConfigFile::Search(variable,val);
}

char *TScreen::optSearch(const char *variable)
{
 char *val=TVMainConfigFile::Search(currentDriverShortName,variable);
 if (val)
    return val;
 // If not found in the driver specific section search in the TV section
 return TVMainConfigFile::Search(variable);
}

TVScreenFontRequestCallBack
 TScreen::setFontRequestCallBack(TVScreenFontRequestCallBack cb)
{
 TVScreenFontRequestCallBack old=frCB;
 frCB=cb;
 return old;
}

