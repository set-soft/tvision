/*****************************************************************************

  Linux screen routines.
  Copyright (c) 2002-2003 by Salvador E. Tropea (SET)
  Covered by the GPL license.

  Configuration variables:
  ScreenWidth
  ScreenHeight
  ScreenPalette
  FontWidth
  FontHeight
  AppCP
  ScrCP
  InpCP
  UseShellScreen

*****************************************************************************/
#include <tv/configtv.h>

#define Uses_TScreen
#define Uses_TEvent
#define Uses_TDrawBuffer
#define Uses_TGKey
#define Uses_TVCodePage
#define Uses_string
#define Uses_ctype
#include <tv.h>

// I delay the check to generate as much dependencies as possible
#if defined(TVOS_UNIX) && !defined(TVOSf_QNXRtP)

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <termios.h>

#include <tv/unix/xtscreen.h>
#include <tv/unix/xtkey.h>
#include <tv/unix/mouse.h>
#include <tv/unix/xtmouse.h>
#include <tv/linux/log.h>

struct termios TScreenXTerm::outTermiosOrig;
struct termios TScreenXTerm::outTermiosNew;
const char    *TScreenXTerm::error=NULL;
int            TScreenXTerm::palette;
int            TScreenXTerm::oldCol=-1,
               TScreenXTerm::oldBack=-1,
               TScreenXTerm::oldFore=-1;
char           TScreenXTerm::useShellScreen=0;

#define force_redraw 0

TScreen *TV_XTermDriverCheck()
{
 TScreenXTerm *drv=new TScreenXTerm();
 if (!TScreen::initialized)
   {
    delete drv;
    return 0;
   }
 return drv;
}

void TScreenXTerm::sigWindowSizeChanged(int sig)
{
 // Set a flag because we don't know if we can do it right now
 TScreenXTerm::windowSizeChanged=1;
 signal(sig,sigWindowSizeChanged);
}

void TScreenXTerm::Init()
{
 TScreen::Resume=Resume;
 TScreen::Suspend=Suspend;
 TScreen::setCrtData=defaultSetCrtData;
 TScreen::setVideoMode=SetVideoMode;
 TScreen::setVideoModeExt=SetVideoModeExt;
 TScreen::System=System;
 TScreen::setCharacters=SetCharacters;
 TDisplay::checkForWindowSize=CheckForWindowSize;
}


int TScreenXTerm::InitOnce()
{
 LOG("TScreenXTerm::InitOnce");
 hOut=fileno(stdout);
 // SET: Original code tried to open a tty at any cost, eg: if stdout was
 // redirected it tried to open stdin's tty for writing. I don't see the
 // point of such an effort and in fact crashes the library, so here I just
 // explain to the user how to run the application in the right way.
 if (!isatty(hOut))
   {
    error=_("that's an interactive application, don't redirect stdout. "
            "If you want to collect information redirect stderr like this: "
            "program 2> file");
    return 1;
   }

 if (tcgetattr(hOut,&outTermiosOrig))
   {
    error=_("can't get output terminal information");
    return 2;
   }

 // Save cursor position, attributes and charset
 fputs("\E7",stdout);
 memcpy(&outTermiosNew,&outTermiosOrig,sizeof(outTermiosNew));
 outTermiosNew.c_oflag|=OPOST;
 if (tcsetattr(hOut,TCSAFLUSH,&outTermiosNew))
   {
    error=_("can't configure terminal mode");
    return 3;
   }

 // Impossible to know the actual palette, just assume a default
 memcpy(OriginalPalette,PC_BIOSPalette,sizeof(OriginalPalette));
 memcpy(ActualPalette,PC_BIOSPalette,sizeof(ActualPalette));

 // Setup the driver properties.
 // We can change the palette (but can't restore it perfectly).
 //  I assume that's XTerm 4.x or Eterm 0.9.x, this doesn't work
 // for older versions.
 flags0=CanSetPalette | CanSetVideoSize;
 return 0;
}


TScreenXTerm::TScreenXTerm()
{
 // Identify the terminal, if that's XTerm or Eterm go on.
 char *terminal=getenv("TERM");
 if (!terminal || !(!strncmp(terminal,"xterm",5) || !strncasecmp(terminal,"Eterm",5)))
    return;

 if (!strncasecmp(terminal,"Eterm",5))
    terminalType=Eterm;

 // Initialize terminal
 if (InitOnce())
   {
    fprintf(stderr,_("Error! %s"),error);
    fprintf(stderr,"\r\n");
    return;
   }
 // Initialize keyboard
 if (TGKeyXTerm::InitOnce())
   {
    tcsetattr(hOut,TCSAFLUSH,&outTermiosOrig);
    fprintf(stderr,_("Error! %s"),TGKeyXTerm::error);
    fprintf(stderr,"\r\n");
    return;
   }
 // Ok, if we are here this driver is usable.
 initialized=1;
 if (dCB) dCB();

 // Don't need special rights anymore
 seteuid(getuid());
 setegid(getgid());

 // Hook the signal generated when the size of the window changes
 signal(SIGWINCH,sigWindowSizeChanged);

 TDisplayXTerm::Init();
 Init();

 TGKeyXTerm::Init();

 long aux;
 if (optSearch("UseShellScreen",aux))
    useShellScreen=aux;

 // Code page initialization
 // Look for user settings
 optSearch("AppCP",forcedAppCP);
 optSearch("ScrCP",forcedScrCP);
 optSearch("InpCP",forcedInpCP);
 // User settings have more priority than detected settings
 codePage=new TVCodePage(forcedAppCP!=-1 ? forcedAppCP : TVCodePage::ISOLatin1Linux,
                         forcedScrCP!=-1 ? forcedScrCP : TVCodePage::ISOLatin1Linux,
                         forcedInpCP!=-1 ? forcedInpCP : TVCodePage::ISOLatin1Linux);
 SetDefaultCodePages(TVCodePage::ISOLatin1Linux,TVCodePage::ISOLatin1Linux,
                     TVCodePage::ISOLatin1Linux);

 if (terminalType==Eterm)
   {// Only 8 colors + brightness (8*2+8*2)
    palette=PAL_LOW;
    THWMouseXTermFull::Init(THWMouseXTermFull::modeEterm);
    // Eterm 0.9.x supports palette setting
    TDisplay::setDisPaletteColors=SetDisPaletteColorsEt;
    ResetPaletteColors=ResetPaletteColorsEt;
    setCrtModeRes=SetCrtModeEt;
    if (parseUserPalette())
       setPaletteColors(0,16,UserStartPalette);
   }
 else
   {// 16+16 colors
    palette=PAL_HIGH;
    THWMouseXTermFull::Init(THWMouseXTermFull::modeXTerm);
    // XTerm from X 4.x supports palette setting
    TDisplay::setDisPaletteColors=SetDisPaletteColorsXT;
    ResetPaletteColors=ResetPaletteColorsXT;
    // XTerm colors are ugly, change them.
    if (parseUserPalette())
       setPaletteColors(0,16,UserStartPalette);
    else
       SetDisPaletteColorsXT(0,16,ActualPalette);
    setCrtModeRes=SetCrtModeXT;
   }
 // This is what GNU/Debian Woody uses by default
 fontW=6; fontH=13;
 TScreenXTerm::screenMode=TScreenXTerm::smCO80;
 LOG((palette==PAL_HIGH ? "Using high palette" : "Using low palette"));

 // G0=ASCII (+LAT1?) G1=Dec. SI
 // How do I restore the original state?
 fputs("\E(B\E)0\xF",stdout);

 startupCursor=getCursorType();
 startupMode  =getCrtMode();
 startScreenWidth =getCols();
 startScreenHeight=getRows();

 // Look for defaults
 unsigned maxX=startScreenWidth, maxY=startScreenHeight;
 unsigned fW=fontW, fH=fontH;
 if (optSearch("ScreenWidth",aux))
    maxX=aux;
 if (optSearch("ScreenHeight",aux))
    maxY=aux;
 if (optSearch("FontWidth",aux))
    fW=aux;
 if (optSearch("FontHeight",aux))
    fH=aux;
 if (maxX!=startScreenWidth || maxY!=startScreenHeight)
    setCrtModeRes(maxX,maxY,fW,fH);

 cursorLines=getCursorType();
 screenMode =getCrtMode();
 screenWidth =getCols();
 screenHeight=getRows();
 LOG("Screen size: " << (int)screenWidth << "," << (int)screenHeight);
 screenBuffer=new ushort[screenWidth * screenHeight];
 SaveScreen();

 // This is our state, save it
 tcgetattr(hOut,&outTermiosNew);
 suspended=0;
 setCursorType(0);
}

void TScreenXTerm::Suspend()
{// Invalidate the knowledge about the current color of the terminal
 oldCol=oldBack=oldFore=-1;
 // Restore old font
 if (fontChanged && oldFontName)
    fprintf(stdout,"\E]50;%s\x7",oldFontName);
 // Restore original window size
 signal(SIGWINCH,SIG_DFL);
 if (startScreenWidth!=screenWidth || startScreenHeight!=screenHeight)
    fprintf(stdout,"\E[8;%d;%dt",startScreenHeight+(terminalType==Eterm ? 1 : 0),
            startScreenWidth);
 // Restore screen contents
 RestoreScreen();
 // Restore the palette, must be before restoring the charset
 if (paletteModified)
    ResetPaletteColors();
 // Restore the cursor shape
 setCursorType(startupCursor);
 // That's the default, we don't know the original values
 fputs("\E(B\E)0\xF",stdout);
 // Restore cursor position, attributes and charset
 fputs("\E8",stdout);
 // Ensure all is processed before leaving
 fflush(stdout);
 // Restore console mode, I think drain is better here, but the previous flush
 // is needed anyways.
 tcsetattr(hOut,TCSADRAIN,&outTermiosOrig);
 LOG("TScreenXTerm Suspend");
}

TScreenXTerm::~TScreenXTerm()
{
 // At this point we are not suspended
 LOG("TScreenXTerm Destructor");
}

void TScreenXTerm::DeallocateResources()
{
 LOG("TScreenXTerm DeallocateResources");
 if (oldFontName)
   {
    DeleteArray(oldFontName);
    oldFontName=NULL;
   }
}

void TScreenXTerm::Resume()
{
 // Save console mode
 tcgetattr(hOut,&outTermiosOrig);
 // Set our console mode
 tcsetattr(hOut,TCSAFLUSH,&outTermiosNew);
 // That's the default
 fputs("\E(B\E)0\xF",stdout);
 selCharset=0;
 // Save cursor position, attributes and charset
 fputs("\E7",stdout);
 SaveScreen();
 // Setup our palette
 if (paletteModified)
    SetDisPaletteColorsXT(0,16,ActualPalette);
 // Check the window size
 startScreenWidth =getCols();
 startScreenHeight=getRows();
 if (startScreenWidth!=screenWidth || startScreenHeight!=screenHeight)
    fprintf(stdout,"\E[8;%d;%dt",screenHeight+(terminalType==Eterm ? 1 : 0),
            screenWidth);
 // Restore our font
 if (fontChanged)
   {
    DeleteArray(oldFontName);
    oldFontName=GetCurrentFontName();
    fprintf(stdout,"\E]50;%dx%d\x7",fontW,fontH);
   }
 signal(SIGWINCH,sigWindowSizeChanged);
 // When we set the video mode the cursor is hidded
 ushort oldCursorLines=cursorLines;
 // Check for video size change and save some state
 //setVideoMode(screenMode);
 // Set our cursor shape
 setCursorType(oldCursorLines);
 LOG("TScreenXTerm Resume");
}

/*****************************************************************************
  Save/Restore screen
*****************************************************************************/

void TScreenXTerm::SaveScreen()
{
 if (!useShellScreen)
    // Just switch to the alternative buffer
    printf("\E[?47h");
}

void TScreenXTerm::RestoreScreen()
{
 if (useShellScreen)
    // Clear the screen
    printf("\E[2J");
 else
    // Just go back to the normal buffer
    printf("\E[?47l");
}

void TScreenXTerm::SaveScreenReleaseMemory(void)
{
}


void TScreenXTerm::CheckSizeBuffer(int oldWidth, int oldHeight)
{
 // allocating a zeroed screenBuffer, because this function
 // is called in most cases (in RHIDE) after a SIGWINCH
 if (screenWidth!=oldWidth || screenHeight!=oldHeight || !screenBuffer)
   {
    // Realloc screen buffer only if actually needed (it doesn't exist
    // or screen size is changed)
    if (screenBuffer)
       DeleteArray(screenBuffer);
    screenBuffer=new ushort[screenWidth*screenHeight];
   }
 memset(screenBuffer,0,screenWidth*screenHeight*sizeof(ushort));
}

void TScreenXTerm::SetVideoMode(ushort mode)
{
 int oldWidth=screenWidth;
 int oldHeight=screenHeight;

 setCrtMode(mode);
 defaultSetCrtData();
 CheckSizeBuffer(oldWidth,oldHeight);
}

void TScreenXTerm::SetVideoModeExt(char *mode)
{
 int oldWidth=screenWidth;
 int oldHeight=screenHeight;

 setCrtModeExt(mode);
 defaultSetCrtData();
 CheckSizeBuffer(oldWidth,oldHeight);
}

void TScreenXTerm::SetCharacters(unsigned dst, ushort *src, unsigned len)
{
 ushort *old=screenBuffer+dst;
 ushort *old_right=old+len-1;
 ushort *src_right=src+len-1;

 /* remove unchanged characters from left to right */
 if (!force_redraw)
   {
    while (len>0 && *old==*src)
      {
       dst++;
       len--;
       old++;
       src++;
      }
 
    /* remove unchanged characters from right to left */
    while (len>0 && *old_right==*src_right)
      {
       len--;
       old_right--;
       src_right--;
      }
   }

 /* write only middle changed characters */
 if (len>0)
    writeBlock(dst,len,old,src);
}


/*
 The following tables creates a layout quite similar to what linux calls
ISO lat 1 fonts. Lamentably looks like Xterm doesn't implement all the
DEC graphics.
 Additionally I don't know what happends if X is configured for another
encoding, does Xterm use it for values between 160 and 254? or it ever
uses ISO-88590-1?

Which is OK? is just that Xterm have poor fonts? Is Linux extending DEC charset?
Linux have full block (DEC 48), but Xterm no.
Linux have all arrows, but Xterm no.
*/

const uchar TScreenXTerm::Code[256]=
{
 '?','=','?','?',96,98,99,100,101,97/*104?*/,97,' '/**/,' '/**/,115/**/,111/**/,' '/**/,
 ' '/**/,104/*'n'*/,105,121,122,124,'<','>','^','v','>','<','*','*','?',123,
 32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
 48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
 64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,
 80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,
 96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,
 112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,'Y'/**/,
 113,120/**/,113/**/,109,120/**/,120,108,116,113/**/,106,113,118,107,117,119,110,
 114,120,113,109,120,120,108,116,113,106,113,118,107,117,119,110, // Most aprox.
 160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
 176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
 192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
 208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,
 224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
 240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255
};
 
const uchar TScreenXTerm::Modifier[256]=
{
 0,0,0,0,1,1,1,1,1,1,1,0,0,1,1,0, //   0- 15
 0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,1, //  16- 31
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //  32- 47
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //  48- 63
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //  64- 79
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //  80- 95
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //  96-111
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 112-127
 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // 128-143
 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // 144-159
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 160-175
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 176-191
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 192-207
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 208-223
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 224-239
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0  // 240-255
};

/*
 * Writes a block of text.
 */

void TScreenXTerm::writeBlock(int dst, int len, ushort *old, ushort *src)
{
 int col=-1;
 uchar val,mod;

 fprintf(stdout,"\E[%d;%dH",dst/TScreenXTerm::screenWidth+1,dst%TScreenXTerm::screenWidth+1);

 while (len-->0)
   {
    int code  =CLY_Low16(*src);
    int newcol=CLY_High16(*src);
  
    *old++=*src++;
    if (col!=newcol) /* change color ? */
      {
       col=newcol;
       if (palette==PAL_MONO)
         {
          if (col==0x0f)
             fputs("\E[0;1m",stdout); // Bold
          else
            if (col==0x70)
               fputs("\E[0;7m",stdout); // Reverse
            else
               fputs("\E[0m",stdout); // Normal
         }
       else
          mapColor(col);
      }
    // Convert the value to something in the ISO-Latin-1 range.
    // Use the DEC graphics for some values in the control range.
    if (code==12 || code==14)
      {
       fputs("\E[7m \E[27m",stdout); // Reversed space
      }
    else
      {
       val=Code[code];
       mod=Modifier[code];
       if (mod!=selCharset)
         {
          selCharset=mod;
          fputc(15-selCharset,stdout);
         }
       fputc(val,stdout);
      }
   }
 if (palette==PAL_MONO)
    fputs("\E[0m",stdout); // Normal

 fprintf(stdout,"\E[%d;%dH",curY+1,curX+1);
}

/*
 * Converts colors from the large TV palette (16 foreground colors and
 * 16 background colors) to ncurses small palette (8 foreground colors
 * and 8 background colors).
 */

void TScreenXTerm::mapColor(int col)
{
 static char map[] = {0, 4, 2, 6, 1, 5, 3, 7, 0, 4, 2, 6, 1, 5, 3, 7};
 int back,fore;

 if (col==oldCol)
    return;
 oldCol=col;
 back=(col >> 4) & 15;
 fore=col & 15;

 if (palette==PAL_LOW)
   {// Just 8 colors, but use bold and blink getting 16+16
    if (fore!=oldFore && back!=oldBack)
       fprintf(stdout,"\E[%d;%d;%d;%dm",
               fore>7 ? 1 : 22,30+map[fore],
               back>7 ? 5 : 25,40+map[back]);
    else
     {
      if (fore!=oldFore)
         fprintf(stdout,"\E[%d;%dm",fore>7 ? 1 : 22,30+map[fore]);
      else
         fprintf(stdout,"\E[%d;%dm",back>7 ? 5 : 25,40+map[back]);
     }
   }
 else
   { // 16 fore and 16 background colors, yuuuhuuu! ;-)
    if (fore!=oldFore && back!=oldBack)
       fprintf(stdout,"\E[%d;%dm",map[fore]+(fore>7 ?  90 : 30),
                                  map[back]+(back>7 ? 100 : 40));
    else
      {
       if (fore!=oldFore)
          fprintf(stdout,"\E[%dm",map[fore]+(fore>7 ?  90 : 30));
       else
          fprintf(stdout,"\E[%dm",map[back]+(back>7 ? 100 : 40));
      }
   }

 oldFore = fore;
 oldBack = back;
}

// SET: Call to an external program, optionally forking
int TScreenXTerm::System(const char *command, pid_t *pidChild, int in,
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

    argv[0]=getenv("SHELL");
    if (!argv[0])
       argv[0]="/bin/sh";
    argv[1]="-c";
    argv[2]=(char *)command;
    argv[3]=0;
    execvp(argv[0],argv);
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

#include <tv/unix/xtscreen.h>
#include <tv/unix/xtkey.h>
#include <tv/unix/mouse.h>
#include <tv/unix/xtmouse.h>
#include <tv/linux/log.h>

#endif // TVOS_UNIX && !TVOSf_QNXRtP
/*****************************************************************************

ESC ( C        Designate G0 Character Set (ISO 2022)
                 C = 0  -> DEC Special Character and Line Drawing Set
                 C = A  -> United Kingdom (UK)
                 C = B  -> United States (USASCII)
ESC ) C        Designate G1 Character Set (ISO 2022)
                 C = 0  -> DEC Special Character and Line Drawing Set
                 C = A  -> United Kingdom (UK)
                 C = B  -> United States (USASCII)
ESC 7          Save Cursor (DECSC)
ESC 8          Restore Cursor (DECRC)
ESC >          Normal Keypad (DECPNM)
SO             Shift Out (Ctrl-N) ->  Switch  to  Alternate  Character  Set:
               invokes the G1 character set.
SI             Shift In  (Ctrl-O)  ->  Switch  to  Standard  Character  Set:
               invokes the G0 character set (the default).
ESC [ Ps ; Ps H Cursor Position [row;column] (default = [1,1]) (CUP)
ESC [ Ps J     Erase in Display (ED)
                 Ps = 0  -> Clear Below (default)
                 Ps = 1  -> Clear Above
                 Ps = 2  -> Clear All
ESC [ Ps n     Device Status Report (DSR)
                 Ps = 5  -> Status Report ESC [ 0 n  (``OK'')
                 Ps = 6  -> Report Cursor Position (CPR) [row;column] as ESC
               [ r ; c R
ESC [ ? Pm h   DEC Private Mode Set (DECSET)
               ******* Lo siguiente restaura la pantalla al salir!!! *******
                 Ps = 4 7  -> Use Alternate Screen Buffer  (unless  disabled
               by the titeInhibit resource)
 Ps = 1 0 0 0  -> Send Mouse X  &  Y  on  button  press  and
               release.  See the section Mouse Tracking.
 Ps = 1 0 0 2  -> Use Cell Motion Mouse Tracking.
 ****** Esto lo soporta la del X 3 y reporta el movimiento del mouse!!!! ***
 Ps = 1 0 0 3  -> Use All Motion Mouse Tracking.
 ****** Probar esto, ¿hará que reporte un modificador para Alt?
 Ps = 1 0 3 5  -> Enable special modifiers for Alt and
       NumLock keys.
 ****** Suena interesante, el de X 3 parece que no lo tiene
 Ps = 1 0 3 6  -> Send ESC when Meta modifies a key
       (enables the metaSendsEscape resource).
 ****** Este es aún mejor que el 47, pero no sirve en Eterm
 Ps = 1 0 4 9  -> Save cursor as in DECSC and use Alter­
       nate Screen Buffer, clearing it first (unless disabled by
       the titeInhibit resource)
 ****** Alguno podrían ayudar
 Ps = 1 0 5 1  -> Set Sun function-key mode.
 Ps = 1 0 5 2  -> Set HP function-key mode.
 Ps = 1 0 6 0  -> Set legacy keyboard emulation (X11R6).
 Ps = 1 0 6 1  -> Set Sun/PC keyboard emulation of VT220
       keyboard.
ESC [ ? Pm l   DEC Private Mode Reset (DECRST)
                 Ps = 4 7  -> Use Normal Screen Buffer
                 Ps = 1 0 0 0  -> Don't Send Mouse X & Y on button press and
               release
ESC [ ? Pm r   Restore DEC Private Mode Values.  The value of Ps  previously
               saved is restored.  Ps values are the same as for DECSET.
ESC [ ? Pm s   Save DEC Private Mode Values.  Ps values are the same as  for
               DECSET.
ESC ] Ps ; Pt BEL
               Set Text Parameters
                 Ps = 0  -> Change Icon Name and Window Title to Pt
                 Ps = 1  -> Change Icon Name to Pt
                 Ps = 2  -> Change Window Title to Pt
                 Ps = 4 6  -> Change Log File to Pt (normally disabled by  a
               compile-time option)
                 Ps = 5 0  -> Set Font to Pt
****** La Xterm que probé soporta 16 fondos y 16 p.planos. Para restaurar los colores
al salir hay que elegir con 39;49, ojo, Eterm 0.8 soporta sólo 8+8
CSI Pm m       Character Attributes (SGR)
    Ps = 0  -> Normal (default)
    Ps = 1  -> Bold
    Ps = 4  -> Underlined
    Ps = 5  -> Blink (appears as Bold)
    Ps = 7  -> Inverse
    Ps = 8  -> Invisible (hidden)
    Ps = 2 2  -> Normal (neither bold nor faint)
    Ps = 2 4  -> Not underlined
    Ps = 2 5  -> Steady (not blinking)
    Ps = 2 7  -> Positive (not inverse)
    Ps = 2 8  -> Visible (not hidden)
    Ps = 3 0  -> Set foreground color to Black
    Ps = 3 1  -> Set foreground color to Red
    Ps = 3 2  -> Set foreground color to Green
    Ps = 3 3  -> Set foreground color to Yellow
    Ps = 3 4  -> Set foreground color to Blue
    Ps = 3 5  -> Set foreground color to Magenta
    Ps = 3 6  -> Set foreground color to Cyan
    Ps = 3 7  -> Set foreground color to White

    Ps = 3 9  -> Set foreground color to default (original)

    Ps = 4 0  -> Set background color to Black
    Ps = 4 1  -> Set background color to Red
    Ps = 4 2  -> Set background color to Green
    Ps = 4 3  -> Set background color to Yellow
    Ps = 4 4  -> Set background color to Blue
    Ps = 4 5  -> Set background color to Magenta
    Ps = 4 6  -> Set background color to Cyan
    Ps = 4 7  -> Set background color to White

    Ps = 4 9  -> Set background color to default (original).
   
         If 16-color support is compiled, the following apply.
         Assume that xterm's resources are set so that the ISO
         color codes are the first 8 of a set of 16.  Then the
         aixterm colors are the bright versions of the ISO colors:
    Ps = 9 0  -> Set foreground color to Black
    Ps = 9 1  -> Set foreground color to Red
    Ps = 9 2  -> Set foreground color to Green
    Ps = 9 3  -> Set foreground color to Yellow
    Ps = 9 4  -> Set foreground color to Blue
    Ps = 9 5  -> Set foreground color to Magenta
    Ps = 9 6  -> Set foreground color to Cyan
    Ps = 9 7  -> Set foreground color to White

    Ps = 1 0 0  -> Set background color to Black
    Ps = 1 0 1  -> Set background color to Red
    Ps = 1 0 2  -> Set background color to Green
    Ps = 1 0 3  -> Set background color to Yellow
    Ps = 1 0 4  -> Set background color to Blue
    Ps = 1 0 5  -> Set background color to Magenta
    Ps = 1 0 6  -> Set background color to Cyan
    Ps = 1 0 7  -> Set background color to White
   
         If xterm is compiled with the 16-color support disabled,
         it supports the following, from rxvt:
    Ps = 1 0 0  -> Set foreground and background color to
         default
   
         If 88- or 256-color support is compiled, the following
         apply.
    Ps = 3 8  ; 5 ; Ps -> Set foreground color to the second Ps
    Ps = 4 8  ; 5 ; Ps -> Set background color to the second Ps

**** ¿Será necesario?
CSI Ps ; Ps " p
          Set conformance level (DECSCL) Valid values for the first
          parameter:
       Ps = 6 1  -> VT100
       Ps = 6 2  -> VT200
       Ps = 6 3  -> VT300
          Valid values for the second parameter:
       Ps = 0  -> 8-bit controls
       Ps = 1  -> 7-bit controls (always set for VT100)
       Ps = 2  -> 8-bit controls

CSI Ps ; Ps ; Ps t
          Window manipulation (from dtterm, as well as extensions).
          Valid values for the first (and any additional parame­
          ters) are:
       **** Estas dos son mortales:
       Ps = 1  -> De-iconify window.
       Ps = 2  -> Iconify window.
       Ps = 3  ; x ; y -> Move window to [x, y].
       Ps = 4  ; height ; width -> Resize the xterm window to height and width in pixels.
       Ps = 5  -> Raise the xterm window to the front of the stacking order.
       Ps = 6  -> Lower the xterm window to the bottom of the stacking order.
       Ps = 7  -> Refresh the xterm window.
       **** Esta permitiría cosas muy buenas!!!
       Ps = 8  ; height ; width -> Resize the text area to [height;width] in characters.
       Ps = 9  ; 0  -> Restore maximized window.
       Ps = 9  ; 1  -> Maximize window (i.e., resize to screen size).
       Ps = 1 1  -> Report xterm window state.  If the xterm
          window is open (non-iconified), it returns CSI 1 t .  If
          the xterm window is iconified, it returns CSI 2 t .
       Ps = 1 3  -> Report xterm window position as CSI 3 ; x; yt
       Ps = 1 4  -> Report xterm window in pixels as CSI  4  ; height ;  width t
       ***** Esta es una alternativa para el tamaño de la ventana
       Ps = 1 8  -> Report the size of the text area in characters as CSI  8  ;  height ;  width t
       Ps = 1 9  -> Report the size of the screen in characters as CSI  9  ;  height ;  width t
       Ps = 2 0  -> Report xterm window's icon label as OSC  L label ST
       ***** Esto implementa el getWindowTitle
       Ps = 2 1  -> Report xterm window's title as OSC  l title ST
       Ps >= 2 4  -> Resize to Ps lines (DECSLPP)

**** Este parece interesante, pero no lo pude hacer andar
CSI Ps ; Pu ´ z
          Enable Locator Reporting (DECELR)
          Valid values for the first parameter:
       Ps = 0  -> Locator disabled (default)
       Ps = 1  -> Locator enabled
       Ps = 2  -> Locator enabled for one report, then dis­
          abled
          The second parameter specifies the coordinate unit for
          locator reports.
          Valid values for the second parameter:
       Pu = 0  or omitted -> default to character cells
       Pu = 1  -> device physical pixels
       Pu = 2  -> character cells
CSI Pm ´ {     Select Locator Events (DECSLE)
          Valid values for the first (and any additional parame­
          ters) are:
       Ps = 0  -> only respond to explicit host requests
          (DECRQLP)
             (default) also cancels any filter rectangle
       Ps = 1  -> report button down transitions
       Ps = 2  -> do not report button down transitions
       Ps = 3  -> report button up transitions
       Ps = 4  -> do not report button up transitions
CSI Ps ´ |     Request Locator Position (DECRQLP)
          Valid values for the parameter are:
       Ps = 0 , 1 or omitted -> transmit a single DECLRP loca­
          tor report

          If Locator Reporting has been enabled by a DECELR, xterm
          will respond with a DECLRP Locator Report.  This report
          is also generated on button up and down events if they
          have been enabled with a DECSLE, or when the locator is
          detected outside of a filter rectangle, if filter rectan­
          gles have been enabled with a DECEFR.

       -> CSI Pe ; Pb ; Pr ; Pc ; Pp &  w

          Parameters are [event;button;row;column;page].
          Valid values for the event:
       Pe = 0  -> locator unavailable - no other parameters
          sent
       Pe = 1  -> request - xterm received a DECRQLP
       Pe = 2  -> left button down
       Pe = 3  -> left button up
       Pe = 4  -> middle button down
       Pe = 5  -> middle button up
       Pe = 6  -> right button down
       Pe = 7  -> right button up
       Pe = 8  -> M4 button down
       Pe = 9  -> M4 button up
       Pe = 1 0  -> locator outside filter rectangle
          ``button'' parameter is a bitmask indicating which but­
          tons are pressed:
       Pb = 0  -> no buttons down
       Pb & 1  -> right button down
       Pb & 2  -> middle button down
       Pb & 4  -> left button down
       Pb & 8  -> M4 button down
          ``row'' and ``column'' parameters are the coordinates of
          the locator position in the xterm window, encoded as
          ASCII decimal.
          The ``page'' parameter is not used by xterm, and will be
          omitted.


**** Esto lo estoy usando para la paleta
OSC Ps ; Pt BEL
          Set Text Parameters.  For colors and font, if Pt is a
          "?", the control sequence elicits a response which con­
          sists of the control sequence which would set the corre­
          sponding value.  The dtterm control sequences allow you
          to determine the icon name and window title.
       Ps = 0  -> Change Icon Name and Window Title to Pt
       Ps = 1  -> Change Icon Name to Pt
       Ps = 2  -> Change Window Title to Pt
       Ps = 3  -> Set X property on top-level window. Pt
          should be in the form "prop=value", or just "prop" to
          delete the property
       Ps = 4  ; c ; name -> Change Color #c to cname.  Any
          number of c name pairs may be given.
       Ps = 1 0  -> Change color names starting with text
          foreground to Pt (a list of one or more color names or
          RGB specifications, separated by semicolon, up to eight,
          as per XParseColor).
       Ps = 1 1  -> Change colors starting with text back­
          ground to Pt
       Ps = 1 2  -> Change colors starting with text cursor to
          Pt
       Ps = 1 3  -> Change colors starting with mouse fore­
          ground to Pt
       Ps = 1 4  -> Change colors starting with mouse back­
          ground to Pt
       Ps = 1 5  -> Change colors starting with Tek foreground
          to Pt
       Ps = 1 6  -> Change colors starting with Tek background
          to Pt
       Ps = 1 7  -> Change colors starting with highlight to
          Pt
       Ps = 4 6  -> Change Log File to Pt (normally disabled
          by a compile-time option)
       Ps = 5 0  -> Set Font to Pt If Pt begins with a "#",
          index in the font menu, relative (if the next character
          is a plus or minus sign) or absolute.  A number is
          expected but not required after the sign (the default is
          the current entry for relative, zero for absolute index­
          ing).
    
*****************************************************************************/
