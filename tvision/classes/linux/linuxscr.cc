/*****************************************************************************

  Linux screen routines.
  Copyright (c) 1996-1998 by Robert Hoehne.
  Copyright (c) 1999-2002 by Salvador E. Tropea (SET)
  Covered by the GPL license.

*****************************************************************************/
/*
TODO: Cuando entra o setea el modo asegurarse que el cursor es lo que yo pienso.

TODO:
TurboVision_screenOptions
Un mecanismo para deshabilitar el uso de vcs como opción
   * Environment variable support.
   * Date: Wed, 29 Jan 1997 16:51:40 +0100 (MET)
  char env[PATH_MAX] = "", *p;
  if ((p = getenv("TVOPT")) != NULL)
  {
     LOG("environment variable TVOPT=" << p);
     for (char *d = env; *p != '\0'; p++)
       *d++ = uctolower(*p);
  }
See ToDo in the mouse initialization.

When using no lat1 chars we had it:
    else if (strchr(terminal, '-')!=NULL && // Have a modifier
             !(strstr(terminal,"-c-")!=NULL || !strcmp(terminal+strlen(terminal)-2,"-c"))) // It isn't color modifier
      { // Not some color variation, so most probably a different charset
       use_pc_chars = 0;
       TerminalType=GENER_TERMINAL;
      }
This is not supported any more

Moverlo a un flag en la inicialización:

  Description:
  Finds if this terminal have variable code page of that's fixed.
  
  Return: 
  True if the terminal have a variable code page.
  
Boolean TScreen::codePageVariable()
{
 return (TerminalType==GENER_TERMINAL ||
         TerminalType==XTERM_TERMINAL) ? False : True;
}

Important limitations:

When using the escape sequences output (no VCS):
* We must set the charset to G1 and select G1 as transparent. We don't know
the original state so we just select G0 and use Graph map for G0. These are
the deafults after a console reset, but doesn't have to be the right
settings.
 A similar thing happends with the option to display control characters.
* I don't know how to restore the console content. MC uses a separated
helper called cons.saver to do the trick.
  In this new version of the driver I implemented something that saves
most of the information in the screen. I used an interesting thing in
Linux kernel, that's service 2 of TIOCLINUX, it can be used to select the
whole screen and then you can do a paste (like with the mouse), but this is
available only for real consoles and I don't know why when I restore such
a screen the mouse gets temporally invisible.

When using VCS:
* The code to bypass mc and get the real console works, but the cons.saver
task of mc sometimes writes over the application.

When GPM isn't available:
* The Linux kernel says:
 * Code for xterm like mouse click reporting by Peter Orbaek 20-Jul-94
 * <poe@daimi.aau.dk>
But this doesn't work at all, if you enable mouse reporting (\E[?9h or
\E[?1000h) you get nothing. You'll get something when somebody calls
set_selection code using mode 16. This is far from usable.

*/
#include <tv/configtv.h>

#ifdef TVOSf_Linux

#define Uses_TScreen
#define Uses_TEvent
#define Uses_TDrawBuffer
#define Uses_TGKey
#define Uses_string
#define Uses_ctype
#define Uses_iostream
#include <tv.h>

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <termios.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <tv/linux/screen.h>
#include <tv/linux/key.h>
#include <tv/linux/mouse.h>
#include <tv/linux/log.h>

#ifdef h386LowLevel
 #include <asm/io.h>
#endif

// What a hell is that?!
#define force_redraw 0

struct termios TScreenLinux::outTermiosOrig;
struct termios TScreenLinux::outTermiosNew;
char          *TScreenLinux::error=NULL;
char           TScreenLinux::secondaryAvailable=0;
char           TScreenLinux::tioclinuxOK=0;
int            TScreenLinux::mdaMemH=-1;
ushort        *TScreenLinux::mdaMem=NULL;
int            TScreenLinux::palette;
ushort        *TScreenLinux::userBuffer=NULL;
unsigned       TScreenLinux::userBufferSize;
int            TScreenLinux::oldCol=-1,
               TScreenLinux::oldBack=-1,
               TScreenLinux::oldFore=-1;

// Structure for TIOCLINUX service 2 set selection, very kernel
// dependent.
typedef struct
{
 char service    __attribute__((packed));
 ushort xs       __attribute__((packed));
 ushort ys       __attribute__((packed));
 ushort xe       __attribute__((packed));
 ushort ye       __attribute__((packed));
 ushort sel_mode __attribute__((packed));
} setSel;

TScreen *TV_LinuxDriverCheck()
{
 TScreenLinux *drv=new TScreenLinux();
 if (!TScreen::initialized)
   {
    delete drv;
    return 0;
   }
 return drv;
}

void TScreenLinux::sigWindowSizeChanged(int sig)
{
 // Set a flag because we don't know if we can do it right now
 TScreenLinux::windowSizeChanged=1;
 signal(sig,sigWindowSizeChanged);
}

void TScreenLinux::Init(int mode)
{
 TScreen::Resume=Resume;
 TScreen::Suspend=Suspend;
 TScreen::setCrtData=defaultSetCrtData;
 TScreen::setVideoMode=SetVideoMode;
 TScreen::setVideoModeExt=SetVideoModeExt;
 TScreen::getCharacter=GetCharacter;
 TScreen::setCharacter=SetCharacter;
 TScreen::System=System;

 switch (mode)
   {
    case lnxInitVCSrw:
         TScreen::setCharacters=SetCharactersVCS;
         TScreen::getCharacters=GetCharactersVCS;
         LOG("Using VCS r/w code");
         break;
    case lnxInitVCSwo:
         TScreen::setCharacters=SetCharactersVCS;
         TScreen::getCharacters=defaultGetCharacters;
         LOG("Using VCS write only code");
         break;
    case lnxInitSimple:
         TScreen::setCharacters=SetCharactersTerm;
         TScreen::getCharacters=defaultGetCharacters;
         LOG("Using escape sequences");
         break;
    case lnxInitMDA:
         TScreen::setCharacters=SetCharactersMDA;
         TScreen::getCharacters=GetCharactersMDA;
         LOG("Using MDA code");
         break;
   }
}


int TScreenLinux::InitOnce()
{
 LOG("TScreenLinux::InitOnce");
 hOut=fileno(stdout);
 // SET: Original code tried to open a tty at any cost, eg: if stdout was
 // redirected it tried to open stdin's tty for writing. I don't see the
 // point of such an effort and in fact crashes the library, so here I just
 // explain to the user how to run the application in the right way.
 if (!isatty(hOut))
   {
    error=_("that's an interactive application, don't redirect stdout\r\n"
            "If you want to collect information redirect stderr like this:\r\n\r\n"
            "program 2> file\r\n");
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

 // Test the TIOCLINUX
 int arg=6;
 tioclinuxOK=(ioctl(hOut,TIOCLINUX,&arg)!=-1);
 if (tioclinuxOK)
    LOG("Linux Console IOCTL working");
 return 0;
}


void TScreenLinux::DetectVCS()
{
 /*
  * This approach was suggested by:
  * Martynas Kunigelis <algikun@santaka.sc-uni.ktu.lt>
  * Date: Mon, 20 Jan 1997 15:55:14 +0000 (EET)
  * Robert added code to bypass parents.
  * SET added code to use write only devices.
  */
 FILE *statfile;
 char path[PATH_MAX];
 bool found_vcsa=false;
 int  pid=getpid();

 sprintf(path,"/proc/%d/stat",pid);
 /* If it fails lets see what terminal own our parent */
 while (!found_vcsa &&
        (pid!=-1) &&
        ((statfile=fopen(path,"r"))!=NULL))
   {
    int dev;
    int ppid;
 
    /* TTYs have 4 as major number */
    /* virtual consoles have minor numbers <= 63 */
    fscanf(statfile, "%*d %*s %*c %d %*d %*d %d", &ppid, &dev);
 
    LOG("ppid: " << ppid << " device: " << dev << "(" << (dev & 0xff) << ")");
    /* Be a little bit smart, we don't want to bypass X */
    /* X terminals are attached to 0 (unknown) terminal */
    if (dev==0) break;
    
    if ((dev & 0xff00)==0x0400 && (dev & 0xff)<=63)
      {
       LOG("virtual console detected");
       sprintf(path, "/dev/vcsa%d",dev & 0xff);
       found_vcsa = true;
  
       // SET: Open it with two files, one for write and the other for read.
       // Doing it the administrator can give write access to some users,
       // but not read, that's very secure.
       if ((vcsWfd=open(path,O_WRONLY))<0)
          LOG("unable to open " << path << ", running in stdout mode");
       else
          if ((vcsRfd=open(path,O_RDONLY))<0)
             LOG("only write access to " << path);
      }
    fclose(statfile);
    if (!found_vcsa && (pid!=ppid))
      {
       pid=ppid;
       sprintf(path,"/proc/%d/stat",pid);
      }
    else
       pid=-1;
   }
}
//--------------- End of VCS detection code

void TScreenLinux::DetectSecondaryDisplay()
{
 //--------------- MDA initialization code
 #ifdef h386LowLevel
 secondaryAvailable=!ioperm(0x3b4,7,1);
 if (secondaryAvailable)
   {
    unsigned char is_mda=(inb(0x3ba) & 0x70) >> 4;
    is_mda=(is_mda==0) || (is_mda==1) || (is_mda==5);
    if (is_mda)
      {
       mdaMemH=open("/dev/mem",O_RDWR);
       if (mdaMemH!=-1)
         {
          mdaMem=(unsigned short *)mmap(NULL,80*25*2,PROT_READ|PROT_WRITE,
                   MAP_SHARED,mdaMemH,0xB0000);
         }
       else
         secondaryAvailable=0;
      }
    else
      secondaryAvailable=0;
   }
 #endif
}
//--------------- End of MDA initialization code

TScreenLinux::TScreenLinux()
{
 // Identify the terminal, if that's linux or console go on.
 char *terminal=getenv("TERM");
 if (!terminal || !(!strcmp(terminal,"console") || !strncmp(terminal,"linux",5)))
    return;

 // Initialize terminal
 if (InitOnce())
   {
    fprintf(stderr,_("Error! %s\r\n"),error);
    return;
   }
 // Initialize keyboard
 if (TGKeyLinux::InitOnce())
   {
    tcsetattr(hOut,TCSAFLUSH,&outTermiosOrig);
    fprintf(stderr,_("Error! %s\r\n"),TGKeyLinux::error);
    return;
   }
 // Ok, if we are here this driver is usable.
 initialized=1;

 // Determine which subdriver to use (VCS R/W, VCS W or plain terminal)
 DetectVCS();
 DetectSecondaryDisplay();

 // Don't need special rights anymore
 seteuid(getuid());
 setegid(getgid());

 // Hook the signal generated when the size of the window changes
 signal(SIGWINCH,sigWindowSizeChanged);

 // Initialize TDisplay using the detected capabilities
 int mode=lnxInitSimple;
 if (vcsWfd!=-1)
    mode=vcsRfd!=-1 ? lnxInitVCSrw : lnxInitVCSwo;
 TDisplayLinux::Init(mode);
 Init(mode);

 TGKeyLinux::Init();
 #ifdef HAVE_GPM
 THWMouseGPM::Init();
 #endif

 if (mode==lnxInitVCSrw || mode==lnxInitVCSwo)
   {// VCS access is assumed to be color
    palette=PAL_HIGH;
    TScreenLinux::screenMode=TScreenLinux::smCO80;
   }
 else
   {// Start assuming linux or linux-c terminal (color)
    palette=PAL_HIGH;
    TScreenLinux::screenMode=TScreenLinux::smCO80;
    // Now check if the user requested monochrome
    if (strstr(terminal,"-m-")!=NULL || !strcmp(terminal+strlen(terminal)-2,"-m")) // linux-m
      { // Mono mode, explicitly supress colors even if they are available
       palette = PAL_MONO;
       TScreen::screenMode=TScreen::smMono;
      }
    LOG((palette == PAL_HIGH ? "Using high palette" : "Using mono palette"));
   }

 cursorLines=startupCursor=getCursorType();
 screenMode=startupMode=getCrtMode();
 screenWidth =getCols();
 screenHeight=getRows();
 LOG("Screen size: " << (int)screenWidth << "," << (int)screenHeight);
 screenBuffer=new ushort[screenWidth * screenHeight];
 SaveScreen();

 // Use G1 charset, set G1 to the loaded video fonts, print control chars.
 // It means the kernel will send what we write directly to screen, just
 // like in the VCSA case.
 // How do I know the previous settings to restore them? I taked a look
 // at the Linux kernel and couldn't find a way to get disp_ctrl.
 if (!canWriteVCS())
    fputs("\e)K\xE",stdout);
 // This is our state, save it
 tcgetattr(hOut,&outTermiosNew);
 suspended=0;
 setCursorType(0);
}

void TScreenLinux::Suspend()
{// Invalidate the knowledge about the current color of the terminal
 oldCol=oldBack=oldFore=-1;
 // Restore screen contents
 RestoreScreen();
 // Restore the cursor shape
 setCursorType(startupCursor);
 // Restore cursor position, attributes and charset
 // Here I just guess the previous state was G0 + no display control :-(
 if (!canWriteVCS())
    fputs("\e)0\xF",stdout);
 fputs("\E8",stdout);
 // Restore console mode
 tcsetattr(hOut,TCSAFLUSH,&outTermiosOrig);
 LOG("TScreenLinux Suspend");
}

TScreenLinux::~TScreenLinux()
{
 // At this point we are not suspended
 LOG("TScreenLinux Destructor");
}

void TScreenLinux::DeallocateResources()
{
 LOG("TScreenLinux DeallocateResources");
 // Now release allocated resources
 if (screenBuffer)
   {
    DeleteArray(screenBuffer);
    screenBuffer=NULL;
   }
 if (vcsWfd>=0)
    close(vcsWfd);
 if (vcsRfd>=0)
    close(vcsRfd);
 if (mdaMem)
   {
    munmap((char *)mdaMem,80*25*2);
    mdaMem=NULL;
   }
 if (mdaMemH!=-1)
   {
    close(mdaMemH);
    mdaMemH=-1;
   }
}

void TScreenLinux::Resume()
{
 // Save console mode
 tcgetattr(hOut,&outTermiosOrig);
 // Set our console mode
 tcsetattr(hOut,TCSAFLUSH,&outTermiosNew);
 // Save cursor position, attributes and charset
 fputs("\E7",stdout);
 // When we set the video mode the cursor is hidded
 ushort oldCursorLines=cursorLines;
 // Check for video size change and save some state
 setVideoMode(screenMode);
 SaveScreen();
 // Set our cursor shape
 setCursorType(oldCursorLines);
 // Set the charset
 if (!canWriteVCS())
    fputs("\e)K\xE",stdout);
 LOG("TScreenLinux Resume");
}

/*****************************************************************************
  Save/Restore screen
*****************************************************************************/

void TScreenLinux::SaveScreen()
{
 if (canReadVCS())
   {
    userBufferSize=getCols()*getRows();
    userBuffer=(ushort *)realloc(userBuffer,userBufferSize*sizeof(ushort));
    getCharacters(0,userBuffer,userBufferSize);
   }
 else
   {// First create an empty screen
    // Fill the screenBuffer with spaces
    unsigned i;
    for (i=0; i<userBufferSize; i++)
        screenBuffer[i]=0x0720;

    // Then we dump the screen to the screenBuffer as reference
    unsigned width=getCols(),height=getRows();
    userBufferSize=width*height;
    if (tioclinuxOK)
      {
       LOG("Using TIOCLINUX to read the screen content");
       setSel sel;
       sel.service=2;       // Set selection
       sel.xs=1; sel.ys=1;
       sel.xe=width;
       sel.ye=height;       // The whole screen
       sel.sel_mode=0;      // Character by character
       fflush(stdin);       // Be sure we don't mix stuff
       if (ioctl(hOut,TIOCLINUX,&sel)!=-1)
         {// Ok, now we get the screen content, but no the colors
          char val[2];
          int character,w=width;
          val[1]=0x07; // Default color

          // Now paste the selection (the whole screen)
          sel.service=3;
          ioctl(hOut,TIOCLINUX,&sel);

          // The kernel will send the text "compressed", any trailing spaces
          // are stripped and a \n is reported.
          ushort *s=screenBuffer;
          ushort *end=screenBuffer+userBufferSize;
          do
            {
             character=fgetc(stdin);
             if (character!=-1)
               {
                if (character==10)
                  {
                   s+=w;
                   w=width;
                  }
                else
                  {
                   val[0]=character ? character : ' ';
                   *(s++)=*((ushort *)val);
                   w--;
                   if (!w) w=width; // Auto LF ;-)
                  }
               }
            }
          while (character!=-1 && s<end);
         }
      }

    // Finally we copy it to the user buffer
    userBuffer=(ushort *)realloc(userBuffer,userBufferSize*sizeof(ushort));
    memcpy(userBuffer,screenBuffer,userBufferSize*sizeof(ushort));
   }
}

void TScreenLinux::RestoreScreen()
{
 if (canWriteVCS())
    setCharacters(0,userBuffer,userBufferSize);
 else
   {// If we can't restore the screen just clear it
    // Set color to gray over black
    fputs("\E[22;37;40m",stdout);
    // Clear the screen to it
    //fputs("\E[2J",stdout);
    setCharacters(0,userBuffer,userBufferSize);
   }
}

void TScreenLinux::SaveScreenReleaseMemory(void)
{
 free(userBuffer);
}


void TScreenLinux::CheckSizeBuffer(int oldWidth, int oldHeight)
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

void TScreenLinux::SetVideoMode(ushort mode)
{
 int oldWidth=screenWidth;
 int oldHeight=screenHeight;

 setCrtMode(mode);
 CheckSizeBuffer(oldWidth,oldHeight);
}

void TScreenLinux::SetVideoModeExt(char *mode)
{
 int oldWidth=screenWidth;
 int oldHeight=screenHeight;

 setCrtModeExt(mode);
 CheckSizeBuffer(oldWidth,oldHeight);
}

ushort TScreenLinux::GetCharacter(unsigned dst)
{
 ushort src;
 getCharacters(dst,&src,1);
 return src;
}

void TScreenLinux::SetCharacter(unsigned offset,ushort value)
{
 setCharacters(offset,&value,1);
}

void TScreenLinux::GetCharactersMDA(unsigned offset, ushort *buf, unsigned count)
{
 memcpy(buf,mdaMem+offset,count*sizeof(ushort));
}

void TScreenLinux::GetCharactersVCS(unsigned offset, ushort *buf, unsigned count)
{
 lseek(vcsRfd,offset*sizeof(ushort)+4,SEEK_SET);
 read(vcsRfd,buf,count*sizeof(ushort));
}

/*
 * Draws a line of text on the screen.
 */
void TScreenLinux::SetCharactersMDA(unsigned dst, ushort *src, unsigned len)
{
 memcpy(mdaMem+dst, src, len*sizeof(ushort));
}

void TScreenLinux::SetCharactersVCS(unsigned dst, ushort *src, unsigned len)
{
 unsigned length=len*sizeof(ushort);

 lseek(vcsWfd,dst*sizeof(ushort)+4,SEEK_SET);
 write(vcsWfd,src,length);
 if (!canReadVCS())
    // SET: cache it to avoid reads that needs special priviledges
    memcpy(screenBuffer+dst,src,length);
}

void TScreenLinux::SetCharactersTerm(unsigned dst, ushort *src, unsigned len)
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
 * Writes a block of text.
 */

void TScreenLinux::writeBlock(int dst, int len, ushort *old, ushort *src)
{
 int col=-1;

 fprintf(stdout,"\E[%d;%dH",dst/TScreenLinux::screenWidth+1,dst%TScreenLinux::screenWidth+1);

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
       fputs(ENTER_UTF8 "\xEF\x80",stdout);
       /* Set the last 6 bits */
       fputc(code | 0x80,stdout);
       /* Escape UTF-8 */
       fputs(EXIT_UTF8,stdout);
      }
    else if (code==128+27)
      {/* A specially evil code: Meta+ESC, it can't be printed */
       /* Just send Unicode 0xF09B to screen */
       fputs(ENTER_UTF8 "\xEF\x82\x9B" EXIT_UTF8,stdout);
      }
    else
       /* The rest pass directly unchanged */
       fputc(code,stdout);
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

void TScreenLinux::mapColor(int col)
{
 static char map[] = {0, 4, 2, 6, 1, 5, 3, 7};
 int back,fore;

 if (col==oldCol)
    return;
 oldCol=col;
 back=(col >> 4) & 7;
 fore=col & 15;

 #define SB set_a_background ? set_a_background : set_background
 #define SF set_a_foreground ? set_a_foreground : set_foreground

 if (fore!=oldFore && back!=oldBack)
    fprintf(stdout,"\E[%d;%d;%dm",fore>7 ? 1 : 22,30+map[fore & 7],40+map[back]);
 else
   {
    if (fore!=oldFore)
       fprintf(stdout,"\E[%d;%dm",fore>7 ? 1 : 22,30+map[fore & 7]);
    else
       fprintf(stdout,"\E[%dm",40+map[back]);
   }

 oldFore = fore;
 oldBack = back;

 #undef SB
 #undef SF
}

// SET: Call to an external program, optionally forking
int TScreenLinux::System(const char *command, pid_t *pidChild)
{
 if (!pidChild)
    return system(command);

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

#endif // TVOSf_Linux
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
