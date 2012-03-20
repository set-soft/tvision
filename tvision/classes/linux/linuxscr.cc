/*****************************************************************************

  Linux screen routines.
  Copyright (c) 1996-1998 by Robert Hoehne.
  Copyright (c) 1999-2004 by Salvador E. Tropea (SET)
  Covered by the GPL license.

  Important note:
  This driver uses the G0 charset slot selecting the user defined map ACM.
  If you load a font and their SFM map then you must load the propper ACM if
the font doesn't map 1 to 1 with the code page. If you don't understand it
please read the Console Tools documentation (html docs). Where each concept
is explained.
  UseVCS=1
  UseMDA=1
  PatchKeys=1
  UseSecondaryFont=0
  AppCP
  ScrCP
  InpCP
  LoadSecondFont
  ScreenPalette
  BrokenCursorShape

*****************************************************************************/
/*
ToDo: Find a way to enable SEPARATED_HANDLE without losing performance. How?
      I know is possible, but not how, currently I must force the stream to
      be unbuffered and the speed is 5 times inferior. Perhaps using an internal
      buffer.

ToDo: What if the user really have 512 characters? in this case the console have
      only 8 colors.

ToDo: What if the fonts geometry changes on the fly? Or in a console switch?

ToDo: Add support for the ISO codepages as used by Linux, not just the standard
+ my frames.

TODO:
TurboVision_screenOptions
See ToDo in the mouse initialization.

When using no lat1 chars we had it:
    else if (strchr(terminal, '-')!=NULL && // Have a modifier
             !(strstr(terminal,"-c-")!=NULL || !strcmp(terminal+strlen(terminal)-2,"-c"))) // It isn't color modifier
      { // Not some color variation, so most probably a different charset
       use_pc_chars = 0;
       TerminalType=GENER_TERMINAL;
      }
This is not supported any more

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

When trying to use a secondary font:
* The Linux kernel switchs to 8 colors mode. See comments in InitOnce().
For this reason we don't enable it by default.

When using unknown code pages:
* Looks like is common practice to load kernel maps that aren't really
encoded in Unicode, they are easier to create but they destroy any hope
to figure out what are representing. KOI8-R is an example, the SFM and
ACMs are in fact representing KOI8-R values and not Unicode. I got
information about systems mixing both: KOI8-R values + euro and others in
Unicode. I use a workaround for it, but this means most Linux systems
are just broken.
*/
#include <tv/configtv.h>

#define Uses_TScreen
#define Uses_TEvent
#define Uses_TDrawBuffer
#define Uses_TGKey
#define Uses_TVCodePage
#define Uses_string
#define Uses_ctype
#define Uses_signal
#include <tv.h>

// I delay the check to generate as much dependencies as possible
#ifdef TVOSf_Linux

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <termios.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/kd.h>

#include <tv/linux/screen.h>
#include <tv/linux/key.h>
#include <tv/linux/mouse.h>
#include <tv/linux/log.h>

#ifdef h386LowLevel
 #if HAVE_OUTB_IN_SYS
  #include <sys/io.h>
 #else
  #include <asm/io.h>
 #endif
#endif

// What a hell is that?!
#define force_redraw 0

#define ENTER_UTF8  "\e%G"
#define EXIT_UTF8   "\e%@"

struct termios TScreenLinux::outTermiosOrig;
struct termios TScreenLinux::outTermiosNew;
const char    *TScreenLinux::error=NULL;
char           TScreenLinux::secondaryAvailable=0;
int            TScreenLinux::mdaMemH=-1;
ushort        *TScreenLinux::mdaMem=NULL;
int            TScreenLinux::palette;
ushort        *TScreenLinux::userBuffer=NULL;
unsigned       TScreenLinux::userBufferSize;
int            TScreenLinux::oldCol=-1,
               TScreenLinux::oldBack=-1,
               TScreenLinux::oldFore=-1;
CodePage       TScreenLinux::unknownACM,
               TScreenLinux::unknownSFM;
uchar          TScreenLinux::canSetFonts=0,
               TScreenLinux::primaryFontSet=0,
               TScreenLinux::secondaryFontSet=0;
struct console_font_op
               TScreenLinux::linuxFont,
               TScreenLinux::ourFont;
int            TScreenLinux::origCPScr,
               TScreenLinux::origCPApp,
               TScreenLinux::origCPInp,
               TScreenLinux::origUTF8Mode,
               TScreenLinux::font512Detected=0,
               TScreenLinux::reduceTo8Colors=0;

// Information about known font maps
struct stCodePageCk
{
 int codepage;
 uint32 checksum;
};

// ISO 8859-2 fonts seems to have a lot of bugs:
// * The iso02.f16.psf file is useless, it have a map but doesn't have
// frames and wastes at least 17 glyphs.
// * The lat2-16.psf file is ok, but lack an unicode map.
// * The lat2u-16.psf file is usable, but doesn't help to display directly.
// It also have a bug, code 0x91 is N with accute but the unicode is missing
// in the map.

// These are the SFM (Screen Font Map) maps
// This map explain which unicodes can be rendered by each character in the
// font.
struct stCodePageCk TScreenLinux::knownFonts[]=
{
 { TVCodePage::ISOLatin1Linux,  0x6E30159A },
 { TVCodePage::ISOLatin1uLinux, 0x2038159A },
 { TVCodePage::KOI8r,           0x207E10FA },
 { TVCodePage::ISOLatin2uLinux, 0xE50A3143 },
 { TVCodePage::ISOLatin2Sun,    0x92522C97 },
 { 0,                           0          }
};

// These are the ACM (Application Charset Map) maps
// This map is used to determine what an 8 bits codes from the application
// means. Think about it: how the kernel knows if a file is encoded in
// latin 1, 2, koi8, etc? Simple: doesn't know, you must set this table.
// Important: ACM maps are usually strict, it means they define things as
// the standards says. This is quite annoying because you loose direct
// access to extra stuff defined in the controls range. Quite bad for VCS
// access so here I map the checksum to the equivalent Linux font.
struct stCodePageCk TScreenLinux::knownScreenMaps[]=
{
 { -1,                          0xD53CF052 }, // Trivial map, what this mean?
 //{ TVCodePage::ISOLatin1Linux,  0xD53CF052 }, // Trivial map, what this mean?
 // It means the codes should pass unchanged, no remap on the fly.
 // Hence 0xA0 is translated into unicode U+F0A0 which is then passed to the
 // screen using the 0xA0 `glyph' of the font.
 { TVCodePage::ISOLatin1Linux,  0xD62C0061 },
 { TVCodePage::ISOLatin2Linux,  0x5594183F },
 { TVCodePage::KOI8r,           0x4F463C4D },
 { TVCodePage::ISOLatin2eLinux, 0x72341867 }, // Euro hacked
 { 0,                           0          }
};

// Information about known languages
struct stCodePageLang
{
 const char *langs;
 int codepage;
};

struct stCodePageLang TScreenLinux::langCodePages[]=
{
 { "ca,da,nl,et,fr,de,fi,is,it,no,pt,es,sv", TVCodePage::ISOLatin1Linux },
 { "hr,cs,hu,pl,ro,sk,sl",                   TVCodePage::ISOLatin2Linux },
 // ru_RU.KOI8-R is common, but not reliable according to Alexey Godin
 { "ru",                                     TVCodePage::KOI8r          },
 { NULL,                                     0                          }
 //{ "el",                                     TVCodePage::ISOGreek       },
 ///{ "tr",                                     TVCodePage::ISO9           },
 //hebrew          iw_IL.ISO-8859-8
 //lithuanian      lt_LT.ISO-8859-13
};

// Structure for TIOCLINUX service 2 set selection, very kernel
// dependent.
typedef struct
{
 char service;//    __attribute__((packed));
 ushort xs       __attribute__((packed));
 ushort ys       __attribute__((packed));
 ushort xe       __attribute__((packed));
 ushort ye       __attribute__((packed));
 ushort sel_mode __attribute__((packed));
} setSel;

static uint32 adler32(uint32 adler, const char *buf, unsigned len);

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
 TScreen::System_p=System;
 TDisplay::checkForWindowSize=CheckForWindowSize;
 TScreen::getFontGeometry=GetFontGeometry;
 TScreen::setFont_p=SetFont;
 TScreen::restoreFonts=RestoreFonts;

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


const int firstTrySize=512;

//#define DEBUG_CODEPAGE
#ifdef DEBUG_CODEPAGE
// This is the code to debug the code page detection
// The information generated by it is very useful and hence I keep the code
// here.
static int compareUni(const void *v1, const void *v2)
{
 struct unipair *p1=(struct unipair *)v1;
 struct unipair *p2=(struct unipair *)v2;
 if (p1->fontpos==p2->fontpos)
    return (p1->unicode>p2->unicode)-(p1->unicode<p2->unicode);
 return (p1->fontpos>p2->fontpos)-(p1->fontpos<p2->fontpos);
}

int TScreenLinux::AnalyzeCodePage()
{
 ushort scrUniMap[E_TABSZ];
 int i,j;

 // GIO_UNIMAP: get unicode-to-font mapping from kernel
 int success;
 struct unimapdesc map;
 map.entry_ct=firstTrySize;
 map.entries=new struct unipair[firstTrySize];
 success= ioctl(hOut,GIO_UNIMAP,&map)!=-1;
 if (!success && map.entry_ct>firstTrySize)
   {
    LOG("The starting size of " << firstTrySize << " wasn't enough, we need " << map.entry_ct);
    delete[] map.entries;
    map.entries=new struct unipair[map.entry_ct];
    success= ioctl(hOut,GIO_UNIMAP,&map)!=-1;
   }
 if (success)
   {
    fprintf(stderr,"The map have %d entries\n",map.entry_ct);
    fprintf(stderr,"Raw information:\n");
    for (i=0; i<map.entry_ct; i++)
        fprintf(stderr,"0x%02X => 0x%04X\n",map.entries[i].fontpos,map.entries[i].unicode);
    fprintf(stderr,"\n\nSorted information:\n");
    // Sort the entries
    qsort(map.entries,map.entry_ct,sizeof(struct unipair),compareUni);
    // Print all marking the entries that provides more than a code
    for (i=0,j=0; i<map.entry_ct; i++)
       {
        if (j!=map.entries[i].fontpos)
          {
           while (j<map.entries[i].fontpos)
              fprintf(stderr,"0x%02X => NO UNICODE\n",j++);
          }
        fprintf(stderr,"0x%02X => 0x%04X",map.entries[i].fontpos,map.entries[i].unicode);
        if (i && map.entries[i-1].fontpos==map.entries[i].fontpos)
           fputs(" *",stderr);
        else
           j++;
        fputc('\n',stderr);
       }
    // Create a special list containing only one unicode by symbol
    ushort UnicodeMap[256];
    memset(UnicodeMap,0xFF,256*sizeof(ushort));
    for (i=0; i<map.entry_ct; i++)
       {
        int pos=map.entries[i].fontpos;
        if (i && map.entries[i-1].fontpos==pos) continue;
        if (pos>255)
          {
           font512Detected++;
           continue;
          }
        UnicodeMap[pos]=map.entries[i].unicode;
       }
    // Print this map
    fputs("-------------\nSimplified map for checksum:\n",stderr);
    for (i=0; i<256; i++)
        if (UnicodeMap[i]==0xFFFF)
           fprintf(stderr,"0x%02X NO UNICODE\n",i);
        else
           fprintf(stderr,"0x%02X U+%04x\n",i,UnicodeMap[i]);
    // Compute a good check sum
    uint32 cks=adler32(0,(char *)UnicodeMap,256*sizeof(ushort));
    fprintf(stderr,"Adler-32 checksum: 0x%08X\n",cks);
    // Map it to internal codes
    fputs("-------------\nInternal codes equivalent:\n",stderr);
    for (i=0; i<256; i++)
       {
        int v=UnicodeMap[i];
        if (v==0xFFFF)
           fputs("  0,",stderr);
        else
           fprintf(stderr,"%3d,",TVCodePage::InternalCodeForUnicode(v));
        if (!((i+1) & 0xF))
           fputc('\n',stderr);
       }
    fputs("-------------\nWhat's missing:\n",stderr);
    for (i=0; i<256; i++)
       {
        int v=UnicodeMap[i];
        if (v==0xFFFF)
           continue;
        if (TVCodePage::InternalCodeForUnicode(v)==-1)
           fprintf(stderr,"%03d U+%04x\n",i,v);
       }
    fputs("-------------\n",stderr);
    for (i=0; knownFonts[i].codepage && knownFonts[i].checksum!=cks; i++);
    if (knownFonts[i].codepage)
      {
       installedSFM=knownFonts[i].codepage;
       fprintf(stderr,"Known code page detected %d\n",installedSFM);
      }
    else
      {
       fputs("Unknown code page, creating a custom one\n",stderr);
       CreateSFMFromTable(UnicodeMap);
      }
   }
 else
   {
    LOG("GIO_UNIMAP failed");
    // Something arbitrary
    installedSFM=TVCodePage::ISOLatin1Linux;
   }

 fputs("-------------\nGIO_UNISCRNMAP\n",stderr);
 if (ioctl(hOut,GIO_UNISCRNMAP,scrUniMap)==-1)
   {
    LOG("GIO_UNISCRNMAP failed");
    delete[] map.entries;
    return 0;
   }
 int isTrivial=1;
 for (i=0; i<E_TABSZ; i++)
    {
     fprintf(stderr,"%3d => U+%04x ",i,scrUniMap[i]);
     if (i+0xF000!=scrUniMap[i] && i!=scrUniMap[i])
       {
        isTrivial=0;
        fputc('*',stderr);
       }
     fputc('\n',stderr);
    }
 fprintf(stderr,"This map %s trivial\n",isTrivial ? "IS" : "**ISN'T**");
 fputs("-------------\nInternal codes equivalent:\n",stderr);
 for (i=0; i<256; i++)
    {
     int v=scrUniMap[i];
     if (v==0xFFFF)
        fputs("  0,",stderr);
     else
        fprintf(stderr,"%3d,",TVCodePage::InternalCodeForUnicode(v));
     if (!((i+1) & 0xF))
        fputc('\n',stderr);
    }
 // Compute a good check sum
 uint32 cksS=adler32(0,(char *)scrUniMap,E_TABSZ*sizeof(ushort));
 fprintf(stderr,"Adler-32 checksum for screen map: 0x%08X\n",cksS);
 for (i=0; knownScreenMaps[i].codepage && knownScreenMaps[i].checksum!=cksS; i++);
 if (knownScreenMaps[i].codepage)
   {
    installedACM=knownScreenMaps[i].codepage;
    if (installedACM==-1)
      {
       fprintf(stderr,"Using the same used for SFM\n");
       installedACM=installedSFM;
      }
    else
       fprintf(stderr,"Known code page detected %d\n",installedACM);
   }
 else
   {
    fputs("Unknown code page, creating a new one\n",stderr);
    // This a workaround for broken KOI8-R systems.
    if (installedSFM==TVCodePage::KOI8r && scrUniMap[0xC0]==0xC0)
      {// That's bogus, try to fix it
       ushort *map=TVCodePage::GetTranslate(TVCodePage::KOI8r);
       fputs("-------------\nACM is bogus, workaround:\n",stderr);
       for (i=0; i<256; i++)
           if (scrUniMap[i]<0x100)
             {
              scrUniMap[i]=TVCodePage::UnicodeForInternalCode(map[i]);
              fprintf(stderr,"%3d => U+%04x\n",i,scrUniMap[i]);
             }
           else
              fprintf(stderr,"%3d => U+%04x *\n",i,scrUniMap[i]);
      }
    CreateACMFromTable(scrUniMap);
   }

 delete[] map.entries;
 return 1;
}
#else
int TScreenLinux::AnalyzeCodePage()
{
 // Get the font unicode map (SFM)
 // GIO_UNIMAP: get unicode-to-font mapping from kernel
 int success;
 struct unimapdesc map;
 map.entry_ct=firstTrySize;
 map.entries=new struct unipair[firstTrySize];
 success= ioctl(hOut,GIO_UNIMAP,&map)!=-1;
 if (!success && map.entry_ct>firstTrySize)
   {
    LOG("The starting size of " << firstTrySize << " wasn't enough, we need " << map.entry_ct);
    delete[] map.entries;
    map.entries=new struct unipair[map.entry_ct];
    success= ioctl(hOut,GIO_UNIMAP,&map)!=-1;
   }
 if (!success)
   {
    delete[] map.entries;
    return 0;
   }
 // Make a simplified version
 int i;
 ushort UnicodeMap[256];
 memset(UnicodeMap,0xFF,256*sizeof(ushort));
 for (i=0; i<map.entry_ct; i++)
    {
     int pos=map.entries[i].fontpos;
     if (pos>255)
       {
        font512Detected++;
        continue;
       }
     if (map.entries[i].unicode<UnicodeMap[pos])
        UnicodeMap[pos]=map.entries[i].unicode;
    }
 // Compute a good check sum of it
 uint32 cks=adler32(0,(char *)UnicodeMap,256*sizeof(ushort));
 // Find if we know about this one
 for (i=0; knownFonts[i].codepage && knownFonts[i].checksum!=cks; i++);
 if (knownFonts[i].codepage)
   {
    installedSFM=knownFonts[i].codepage;
    LOG("Known SFM detected: " << installedSFM);
   }
 else
   {
    LOG("Unknown SFM: " << cks);
    // We don't know about it, but we have enough information to try using it
    CreateSFMFromTable(UnicodeMap);
   }
 // Now look for the ACM
 // GIO_UNISCRNMAP: get full Unicode screen mapping
 ushort scrUniMap[E_TABSZ];
 if (ioctl(hOut,GIO_UNISCRNMAP,scrUniMap)!=-1)
   {
    // Compute a good check sum
    cks=adler32(0,(char *)scrUniMap,E_TABSZ*sizeof(ushort));
    // Find if we know about this one
    for (i=0; knownScreenMaps[i].codepage && knownScreenMaps[i].checksum!=cks; i++);
    if (knownScreenMaps[i].codepage)
      {
       installedACM=knownScreenMaps[i].codepage;
       if (installedACM==-1)
          installedACM=installedSFM;
       LOG("Known ACM detected: " << installedACM);
      }
    else
      {
       LOG("Unknown ACM: " << cks);
       // This a workaround for broken KOI8-R systems.
       if (installedSFM==TVCodePage::KOI8r && scrUniMap[0xC0]==0xC0)
         {// That's bogus, try to fix it
          ushort *map=TVCodePage::GetTranslate(TVCodePage::KOI8r);
          for (i=0; i<256; i++)
              if (scrUniMap[i]<0x100)
                 scrUniMap[i]=TVCodePage::UnicodeForInternalCode(map[i]);
         }
       CreateACMFromTable(scrUniMap);
      }
   }
 delete[] map.entries;
 return 1;
}
#endif

void TScreenLinux::CreateSFMFromTable(ushort *table)
{
 TVCodePage::CreateCPFromUnicode(&unknownSFM,TVCodePage::LinuxSFM,
                                 "Linux SFM",table);
 TVCodePage::AddCodePage(&unknownSFM);
 installedSFM=TVCodePage::LinuxSFM;
}

void TScreenLinux::CreateACMFromTable(ushort *table)
{
 TVCodePage::CreateCPFromUnicode(&unknownACM,TVCodePage::LinuxACM,
                                 "Linux ACM",table);
 TVCodePage::AddCodePage(&unknownACM);
 installedACM=TVCodePage::LinuxACM;
}

int TScreenLinux::GuessCodePageFromLANG()
{
 char *lang=getenv("LANG");
 if (!lang || strlen(lang)<2)
    return 0;
 int i,ret=0;
 char b[3];
 b[0]=tolower(lang[0]);
 b[1]=tolower(lang[1]);
 b[2]=0;

 for (i=0; langCodePages[i].langs && !strstr(langCodePages[i].langs,b); i++);
 if (langCodePages[i].langs)
   {
    installedSFM=langCodePages[i].codepage;
    LOG("Code page for a known language guess: " << installedSFM);
    ret=1;
   }
 else
   {
    LOG("Unknown language: " << b << " " << lang);
   }
 return ret;
}

/**[txh]********************************************************************

  Description:
  Determines if the console is in UTF-8 or single char mode.
  
  Return: 1 UTF-8, 0 single char, -1 error.
  
***************************************************************************/

int TScreenLinux::isInUTF8()
{
 unsigned y, x;

 // The following test is from an idea in console-tools by
 // Ricardas Cepas <rch@pub.osf.lt>. GPL.
 setCursorPos(0,0);
 // "\357\200\240" = U+F020 = `space' in Linux's straight-to-font zone
 fputs("\357\200\240",fOut);
 // Make sure it is printed
 fflush(fOut);
 getCursorPos(x,y);
 // Get a single byte in UTF-8 and 3 bytes othewise
 switch (x)
   {
    case 1: // UTF-8
         x=1;
         LOG("UTF-8 mode detected");
         break;
    case 3: // single-byte mode
         x=0;
         LOG("Single char mode detected");
         break;
    default: // error
         LOG("Error, can't determine UTF-8 state, column: " << x);
         return -1;
   }
  
 return x;
}

void TScreenLinux::AvoidUTF8Mode()
{
 // Determine UTF-8 mode
 origUTF8Mode=isInUTF8();
 if (origUTF8Mode==-1)
    LOG("Error determining UTF-8 mode");
 // If necesary exit from it
 if (origUTF8Mode==1)
    fputs(EXIT_UTF8,fOut);
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
    error=_("that's an interactive application, don't redirect stdout. "
            "If you want to collect information redirect stderr like this: "
            "program 2> file");
    return 1;
   }
 #ifdef SEPARATED_HANDLE
 /* Testing a reopen */
 char *ttyName=ttyname(hOut);
 if (!ttyName)
   {
    error=_("failed to get the name of the current terminal used for output");
    return 3;
   }
 fOut=fopen(ttyName,"w+b");
 if (!fOut)
   {
    error=_("failed to open the output terminal");
    return 4;
   }
 int fgs=fcntl(hOut,F_GETFL,0);
 fcntl(fileno(fOut),F_SETFL,fgs);

 if (tcgetattr(hOut,&outTermiosOrig))
   {
    error=_("can't get output terminal information");
    return 2;
   }
 hOut=fileno(fOut);
 setvbuf(fOut,NULL,_IONBF,0);
 #else
 fOut=stdout;

 if (tcgetattr(hOut,&outTermiosOrig))
   {
    error=_("can't get output terminal information");
    return 2;
   }
 #endif

 // Save cursor position, attributes and charset
 fputs("\E7",fOut);
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

 if (tioclinuxOK)
   {// We know the default colors, we can just hope they are the currently used
    GetDisPaletteColors(0,16,OriginalPalette);
    memcpy(ActualPalette,OriginalPalette,sizeof(ActualPalette));
   }
 else
   {// We don't know what palette is used so we can just setup our own palette
    // Get the PC BIOS palette
    defaultGetDisPaletteColors(0,16,ActualPalette);
    // Now set it
    SetDisPaletteColors(0,16,ActualPalette);
   }

 // Look for user settings
 optSearch("AppCP",forcedAppCP);
 optSearch("ScrCP",forcedScrCP);
 optSearch("InpCP",forcedInpCP);
 // Try to figure out which code page is loaded
 // Note: This is always needed for the defaults
 if (!tioclinuxOK || !AnalyzeCodePage())
   {
    GuessCodePageFromLANG();
   }

 #ifdef DEBUG_CODEPAGE
 fprintf(stderr,"Using: AppCP: 0x%08lX ScrCP: 0x%08lX InpCP: 0x%08lX\n",
         forcedAppCP!=-1 ? forcedAppCP : installedACM,
         forcedScrCP!=-1 ? forcedScrCP : installedSFM,
         forcedInpCP!=-1 ? forcedInpCP : installedACM);
 fprintf(stderr,"Default: AppCP: 0x%08X ScrCP: 0x%08X InpCP: 0x%08X\n",
         installedACM,installedSFM,installedACM);
 #endif

 // User settings have more priority than detected settings
 codePage=new TVCodePage(forcedAppCP!=-1 ? forcedAppCP : installedACM,
                         forcedScrCP!=-1 ? forcedScrCP : installedSFM,
                         forcedInpCP!=-1 ? forcedInpCP : installedACM);
 SetDefaultCodePages(installedACM,installedSFM,installedACM);
 if (tioclinuxOK && GetLinuxFontGeometry())
   {
    canSetFonts=1;
    linuxFont.data=ourFont.data=NULL;
    linuxFont.flags=ourFont.flags=0;
   }

 // Setup the driver properties.
 // Our code page isn't fixed.
 // We can change the palette (but can't restore it perfectly)
 // We can also change the cursor shape.
 // Even when Linux have a blanker we run in full screen so screen savers
 // could be used with some sense.
 // We support fonts, but we can change their geometry at will.
 flags0=CanSetPalette | CodePageVar | UseScreenSaver;
 // Some fbcon drivers fail to implement the cursor shape stuff.
 // Using this variable we can inform it to the application.
 long brokenCursorShapes=0;
 optSearch("BrokenCursorShape",brokenCursorShapes);
 if (!brokenCursorShapes)
    flags0|=CursorShapes;
 if (canSetFonts)
   {
    flags0|=CanSetBFont;
    // When we use the secondary font Linux reduces the colors number to 8.
    // This is good if the 512 chars are defining a big charset, but really
    // bad if they are defining 2 different charsets. As we use it to define
    // 2 charsets the impact is negative. So we enable it only if the user
    // specifically asks for it.
    long useSecondaryFont=0;
    TScreen::optSearch("UseSecondaryFont",useSecondaryFont);
    if (useSecondaryFont)
       flags0|=CanSetSBFont;
   }
 if (font512Detected && !useSecondaryFont)
   {
    reduceTo8Colors=1;
    LOG("Applying color reduction to 8 colors");
   }

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
    fprintf(stderr,_("Error! %s"),error);
    fprintf(stderr,"\r\n");
    return;
   }
 // Initialize keyboard
 if (TGKeyLinux::InitOnce())
   {
    tcsetattr(hOut,TCSAFLUSH,&outTermiosOrig);
    fprintf(stderr,_("Error! %s"),TGKeyLinux::error);
    fprintf(stderr,"\r\n");
    return;
   }
 // Ok, if we are here this driver is usable.
 initialized=1;
 if (dCB) dCB();

 // Determine which subdriver to use (VCS R/W, VCS W or plain terminal)
 {
  long useVCS=1;
  TScreen::optSearch("UseVCS",useVCS);
  if (useVCS)
     DetectVCS();
 }
 {
  long useMDA=1;
  TScreen::optSearch("UseMDA",useMDA);
  if (useMDA)
     DetectSecondaryDisplay();
 }

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

 TGKeyLinux::Init(installedSFM==TVCodePage::KOI8r ? TGKeyLinux::KOI8 :
                  TGKeyLinux::Default);
 #ifdef HAVE_GPM
 THWMouseGPM::Init();
 #endif
 // Now we can do it
 getCursorPos(oldCurX,oldCurY);

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

 // User requested palette
 if (palette==PAL_HIGH && parseUserPalette())
    setPaletteColors(0,16,UserStartPalette);

 cursorLines=startupCursor=getCursorType();
 screenMode=startupMode=getCrtMode();
 screenWidth =getCols();
 screenHeight=getRows();
 LOG("Screen size: " << (int)screenWidth << "," << (int)screenHeight);
 screenBuffer=new ushort[screenWidth * screenHeight];
 SaveScreen();

 AvoidUTF8Mode();

 // Use G1 charset, set G1 to the loaded video fonts, print control chars.
 // It means the kernel will send what we write directly to screen, just
 // like in the VCSA case.
 // How do I know the previous settings to restore them? I taked a look
 // at the Linux kernel and couldn't find a way to get disp_ctrl.
 if (!canWriteVCS())
    fputs("\e)K\xE",fOut);
 // This is our state, save it
 tcgetattr(hOut,&outTermiosNew);
 suspended=0;
 setCursorType(0);

 // Give a chance to the fonts callback
 if (canSetFonts && frCB && GetLinuxFontGeometry())
   {
    TScreenFont256 *font=frCB(0,linuxFont.width,linuxFont.height);
    SetFont(1,font,0,NULL);
    if (font)
       DeleteArray(font->data);
    delete font;

    #if 1
    long sec=0;
    TScreenFont256 *font2=NULL;
    if (optSearch("LoadSecondaryFont",sec) && sec)
      {
       font2=frCB(1,linuxFont.width,linuxFont.height);
       if (font2)
         {
          SetFont(0,NULL,1,font2);
          DeleteArray(font2->data);
          delete font2;
         }
      }
    #endif
   }
}

void TScreenLinux::Suspend()
{// Invalidate the knowledge about the current color of the terminal
 oldCol=oldBack=oldFore=-1;
 // Restore screen contents
 RestoreScreen();
 // Restore the font
 SuspendFont();
 // Restore the cursor shape
 setCursorType(startupCursor);
 // Restore cursor position, attributes and charset
 // Here I just guess the previous state was G0 + no display control :-(
 if (!canWriteVCS())
    fputs("\e)0\xF",fOut);
 // Reset the palette, lamentably we don't know the original state :-(
 if (tioclinuxOK)
    // Go back to default colors we memorized
    SetDisPaletteColors(0,16,OriginalPalette);
 else
    // Just reset to default palette (should be equivalent)
    fputs("\E]R",fOut);
 // Restore UTF-8 mode
 if (origUTF8Mode==1)
    fputs(ENTER_UTF8,fOut);
 // Is that a Linux bug? Sometime \E8 works, others not.
 fputs("\E8",fOut);
 // Restore cursor position
 SetCursorPos(oldCurX,oldCurY);
 // Ensure the last command is executed
 fflush(fOut);
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
 fputs("\E7",fOut);
 getCursorPos(oldCurX,oldCurY);
 AvoidUTF8Mode();
 // Restore our fonts
 ResumeFont();
 if (tioclinuxOK)
    // We know the default colors, we can just hope they are the currently used
    GetDisPaletteColors(0,16,OriginalPalette);
 SetDisPaletteColors(0,16,ActualPalette);
 // When we set the video mode the cursor is hidded
 ushort oldCursorLines=cursorLines;
 // Check for video size change and save some state
 setVideoMode(screenMode);
 SaveScreen();
 // Set our cursor shape
 setCursorType(oldCursorLines);
 // Set the charset
 if (!canWriteVCS())
    fputs("\e)K\xE",fOut);
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
    unsigned i,size=screenWidth*screenHeight*2;
    char *s=(char *)screenBuffer;
    for (i=0; i<size; i+=2)
       {
        s[i]=' '; s[i+1]=7;
       }

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
       fflush(TGKeyLinux::fIn);       // Be sure we don't mix stuff
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
             character=fgetc(TGKeyLinux::fIn);
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
                   //*(s++)=*((ushort *)val);
                   *(s++)=val[0]|(val[1]<<8);
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
    fputs("\E[22;37;40m",fOut);
    // Clear the screen to it
    //fputs("\E[2J",fOut);
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
 defaultSetCrtData();
 CheckSizeBuffer(oldWidth,oldHeight);
}

void TScreenLinux::SetVideoModeExt(char *mode)
{
 int oldWidth=screenWidth;
 int oldHeight=screenHeight;

 setCrtModeExt(mode);
 defaultSetCrtData();
 CheckSizeBuffer(oldWidth,oldHeight);
}

ushort TScreenLinux::GetCharacter(unsigned dst)
{
 ushort src;
 getCharacters(dst,&src,1);
 return src;
}

void TScreenLinux::GetCharactersMDA(unsigned offset, ushort *buf, unsigned count)
{
 memcpy(buf,mdaMem+offset,count*sizeof(ushort));
}

void TScreenLinux::GetCharactersVCS(unsigned offset, ushort *buf, unsigned count)
{
 lseek(vcsRfd,offset*sizeof(ushort)+4,SEEK_SET);
 read(vcsRfd,buf,count*sizeof(ushort));
 #if TV_BIG_ENDIAN
   {
    unsigned i;
    uchar *s=(uchar *)buf,aux;
    count*=2;
    for (i=0; i<count; i+=2)
       {
        aux=s[i];
        s[i]=s[i+1];
        s[i+1]=aux;
       }
   }
 #endif
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

 if (reduceTo8Colors)
   {
    unsigned i;
    uchar *s=(uchar *)src, back, fore, newfore;
    for (i=1; i<length; i+=2)
       {
        back=(s[i] >> 4) & 7;
        fore=s[i] & 15;
        newfore=fore & 7;
        if (back!=fore && back==newfore)
           newfore=(newfore+1) & 7;
        s[i]=newfore | (back<<4);
        // The following is faster but you need a palette designed for 8
        // colors. I personally think people using UTF-8 for consoles
        // must be ready to lose performance.
        //s[i]&=0xF7;
       }
   }

 lseek(vcsWfd,dst*sizeof(ushort)+4,SEEK_SET);
 #if TV_BIG_ENDIAN
   {
    unsigned i;
    uchar *s=(uchar *)src;
    uchar b[length*2];
    for (i=0; i<length; i+=2)
       {
        b[i]=s[i+1];
        b[i+1]=s[i];
       }
    write(vcsWfd,b,length);
   }
 #else
    write(vcsWfd,src,length);
 #endif
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

 fprintf(fOut,"\E[%d;%dH",dst/TScreenLinux::screenWidth+1,dst%TScreenLinux::screenWidth+1);

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
             fputs("\E[0;1m",fOut); // Bold
          else
            if (col==0x70)
               fputs("\E[0;7m",fOut); // Reverse
            else
               fputs("\E[0m",fOut); // Normal
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
    if (code<32 && ((CTRL_ALWAYS>>code) & 1))
      {/* This character can't be printed, we must use unicode */
       /* Enter UTF-8 and start constructing 0xF000 code */
       fputs(ENTER_UTF8 "\xEF\x80",fOut);
       /* Set the last 6 bits */
       fputc(code | 0x80,fOut);
       /* Escape UTF-8 */
       fputs(EXIT_UTF8,fOut);
      }
    else if (code==128+27)
      {/* A specially evil code: Meta+ESC, it can't be printed */
       /* Just send Unicode 0xF09B to screen */
       fputs(ENTER_UTF8 "\xEF\x82\x9B" EXIT_UTF8,fOut);
      }
    else
       /* The rest pass directly unchanged */
       fputc(code,fOut);
   }
 if (palette==PAL_MONO)
    fputs("\E[0m",fOut); // Normal

 fprintf(fOut,"\E[%d;%dH",curY+1,curX+1);
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

 if (reduceTo8Colors)
   {
    int newfore;
    newfore=fore & 7;
    if (back!=fore && back==newfore)
       newfore=(newfore+1) & 7;
    fore=newfore;
   }

 #define SB set_a_background ? set_a_background : set_background
 #define SF set_a_foreground ? set_a_foreground : set_foreground

 if (fore!=oldFore && back!=oldBack)
    fprintf(fOut,"\E[%d;%d;%dm",fore>7 ? 1 : 22,30+map[fore & 7],40+map[back]);
 else
   {
    if (fore!=oldFore)
       fprintf(fOut,"\E[%d;%dm",fore>7 ? 1 : 22,30+map[fore & 7]);
    else
       fprintf(fOut,"\E[%dm",40+map[back]);
   }

 oldFore = fore;
 oldBack = back;

 #undef SB
 #undef SF
}

// SET: Call to an external program, optionally forking
int TScreenLinux::System(const char *command, pid_t *pidChild, int in,
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

    // Note: execvp takes a char * const argument, but I think it should
    // take a const char *
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

/* The following is a modified version of Adler-32 checksum calculator:
 * adler32.c -- compute the Adler-32 checksum of a data stream
 * Copyright (C) 1995-1998 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
   The above mentioned header says:

  Copyright (C) 1995-2002 Jean-loup Gailly and Mark Adler

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Jean-loup Gailly        Mark Adler
  jloup@gzip.org          madler@alumni.caltech.edu

  This notice isn't removed, I clearly state the original author and that's
 modified, so I think that's ok and compatible with GPL.
 */

const uint32 Base=65521; /* largest prime smaller than 65536 */
const unsigned nMax=5552;
/* nMax is the largest n such that 255n(n+1)/2 + (n+1)(Base-1) <= 2^32-1 */

#define DO1(buf,i)  {s1+=buf[i]; s2+=s1;}
#define DO2(buf,i)  DO1(buf,i); DO1(buf,i+1);
#define DO4(buf,i)  DO2(buf,i); DO2(buf,i+2);
#define DO8(buf,i)  DO4(buf,i); DO4(buf,i+4);
#define DO16(buf)   DO8(buf,0); DO8(buf,8);

static
uint32 adler32(uint32 adler, const char *buf, unsigned len)
{
 uint32 s1=adler & 0xffff;
 uint32 s2=(adler>>16) & 0xffff;
 int k;

 if (buf==NULL)
    return 1;

 while (len>0)
   {
    k=len<nMax ? len : nMax;
    len-=k;
    while (k>=16)
      {
       DO16(buf);
       buf+=16;
       k-=16;
      }
    if (k)
      do
        {
         s1+=*buf++;
         s2+=s1;
        }
      while (--k);
    s1%=Base;
    s2%=Base;
   }
 return (s2 << 16) | s1;
}

/*****************************************************************************
  Fonts routines
*****************************************************************************/

int TScreenLinux::GetLinuxFontGeometry()
{
 memset(&linuxFont,0,sizeof(linuxFont));
 linuxFont.op=KD_FONT_OP_GET;
 linuxFont.height=32;
 linuxFont.width=16;

 int ret=ioctl(hOut,KDFONTOP,&linuxFont);
 /*unsigned count=linuxFont.charcount;
 linuxFont.charcount=0;
 ioctl(hOut,KDFONTOP,&linuxFont);
 linuxFont.charcount=count;*/

 return ret>=0;
}

int TScreenLinux::GetFontGeometry(unsigned &w, unsigned &h)
{
 if (!canSetFonts) return 0;
 w=linuxFont.width;
 h=linuxFont.height;
 return 1;
}

void TScreenLinux::FreeFontsMemory()
{
 if (linuxFont.data)
   {
    free(linuxFont.data);
    linuxFont.data=NULL;
   }
 if (ourFont.data)
   {
    free(ourFont.data);
    ourFont.data=NULL;
   }
}

int TScreenLinux::AllocateFontsMemory()
{
 unsigned bytes=32*((linuxFont.width+7)/8);
 linuxFont.data=(uchar *)malloc(bytes*linuxFont.charcount);
 ourFont.data=(uchar *)malloc(bytes*512);
 return linuxFont.data && ourFont.data;
}

int TScreenLinux::GetLinuxFont()
{
 linuxFont.op=KD_FONT_OP_GET;
 return ioctl(hOut,KDFONTOP,&linuxFont)>=0;
}

void TScreenLinux::ExpandFont(uchar *dest, TScreenFont256 *font)
{// No matters what's the height Linux uses 32
 int i;
 unsigned bytes=32*((linuxFont.width+7)/8);
 unsigned sizeOrig=font->h*((font->w+7)/8);
 uchar *b=font->data;
 memset(dest,0,bytes*256);
 for (i=0; i<256; dest+=bytes, b+=sizeOrig, i++)
     memcpy(dest,b,sizeOrig);
}

int TScreenLinux::SetFont(int changeP, TScreenFont256 *fontP,
                          int changeS, TScreenFont256 *fontS,
                          int fontCP, int appCP)
{
 if (!canSetFonts) return 0;
 if (!changeP && !changeS) return 1;
 // Check for restore fonts
 if (changeP && !fontP && ((!changeS && !secondaryFontSet) || (changeS && !fontS)))
   {// Restore Linux font
    linuxFont.op=KD_FONT_OP_SET;
    int ret=ioctl(hOut,KDFONTOP,&linuxFont)>=0;
    // We no longer have a font loaded
    FreeFontsMemory();
    secondaryFontSet=primaryFontSet=0;
    if (fontCP!=-1)
      {
       if (appCP==-1)
          TVCodePage::SetScreenCodePage(fontCP);
       else
          TVCodePage::SetCodePage(appCP,fontCP,-1);
      }
    return ret;
   }
 // Solve the sizes
 unsigned wP=linuxFont.width,hP=linuxFont.height,
          wS=linuxFont.width,hS=linuxFont.height;
 int newP=changeP && fontP;
 if (newP)
   {
    wP=fontP->w;
    hP=fontP->h;
   }
 int newS=changeS && fontS;
 if (newS)
   {
    wS=fontS->w;
    hS=fontS->h;
   }
 if (wP!=wS || hP!=hS) return 0;
 // Check if the size is in the range
 if (wP!=linuxFont.width || hP!=linuxFont.height)
    return 0;

 // We are about changing the font, we must be sure that Linux font is saved
 if ((newS || newP) && !primaryFontSet && !secondaryFontSet)
   {
    if (!AllocateFontsMemory() || !GetLinuxFont())
       return 0;
    ourFont.width=linuxFont.width;
    ourFont.height=linuxFont.height;
   }

 // Change the requested fonts
 unsigned bytes=32*((linuxFont.width+7)/8);
 uchar newSecondaryFontSet=secondaryFontSet,
       newPrimaryFontSet=primaryFontSet;
 if (changeP)
   {
    if (!secondaryFontSet)
      { // P but not S
       ourFont.charcount=256;
      }
    if (!primaryFontSet)
       TVCodePage::GetCodePages(origCPApp,origCPScr,origCPInp);
    if (fontP)
      {
       ExpandFont(ourFont.data,fontP);
       newPrimaryFontSet=1;
      }
    else
      {
       // Use Linux font as primary
       memcpy(ourFont.data,linuxFont.data,256*bytes);
       newPrimaryFontSet=0;
      }
   }
 if (changeS)
   {
    if (fontS)
      {
       ourFont.charcount=512;
       if (!primaryFontSet)
          memcpy(ourFont.data,linuxFont.data,256*bytes);
       ExpandFont(ourFont.data+256*bytes,fontS);
       newSecondaryFontSet=1;
      }
    else
      {
       if (linuxFont.charcount==512)
         {
          ourFont.charcount=512;
          memcpy(ourFont.data+256*bytes,linuxFont.data+256*bytes,256*bytes);
         }
       else
          ourFont.charcount=256;
       newSecondaryFontSet=0;
      }
   }
 ourFont.op=KD_FONT_OP_SET;
 if (ioctl(hOut,KDFONTOP,&ourFont)<0) return 0;
 primaryFontSet=newPrimaryFontSet;
 secondaryFontSet=newSecondaryFontSet;

 if (changeP && fontCP!=-1)
   {
    if (appCP==-1)
       TVCodePage::SetScreenCodePage(fontCP);
    else
       TVCodePage::SetCodePage(appCP,fontCP,-1);
   }
 return 1;
}

void TScreenLinux::RestoreFonts()
{
 if (!primaryFontSet && !secondaryFontSet)
    return; // Protection
 SetFont(1,NULL,1,NULL,origCPScr,origCPApp);
}

void TScreenLinux::SuspendFont()
{
 if (primaryFontSet || secondaryFontSet)
   {// Set the Linux fonts again
    linuxFont.op=KD_FONT_OP_SET;
    ioctl(hOut,KDFONTOP,&linuxFont);
    // Free their memory, we will memorize them again in the resume
    free(linuxFont.data);
    linuxFont.data=NULL;
   }
}

void TScreenLinux::ResumeFont()
{
 if (primaryFontSet || secondaryFontSet)
   {// Get Linux fonts info
    GetLinuxFontGeometry();
    // Allocate memory for them
    unsigned bytes=32*((linuxFont.width+7)/8);
    linuxFont.data=(uchar *)malloc(bytes*linuxFont.charcount);
    // Get them
    GetLinuxFont();
    // Now restore our fonts
    ourFont.op=KD_FONT_OP_SET;
    ioctl(hOut,KDFONTOP,&ourFont);
   }
}


#else // TVOSf_Linux

#include <tv/linux/screen.h>
#include <tv/linux/key.h>
#include <tv/linux/mouse.h>
#include <tv/linux/log.h>

#endif // else TVOSf_Linux
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
