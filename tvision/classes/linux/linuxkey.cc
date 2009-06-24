/*****************************************************************************

  Linux keyboard routines.
  Copyright (c) 1998-2002 by Salvador E. Tropea (SET)
  Some portions come from code:
  Copyright (c) 1998 by Robert Hoehne.
  Covered by the GPL license.

  This driver is a relatively complex one because:
  1) Deals with console mode.
  2) Translates keyboard codes.
  3) Parses escape sequences.
  4) Patches keyboard tables and restores it on console switchs.

  Todo:
  * How do I setup things before they are hooked? SetKbdMapping needs to
  be reworked.
  * Should I move VT detection to a separated module?
  * Asegurarse que atexit se llame el que restaura el teclado.
  * El módulo de TScreenLinux tendría que llamar a InitOnce(), si esto le da
  error no seguir. Y si el tien problemas debería llamar al Suspend() para
  que restaure.
   
*****************************************************************************/
#include <tv/configtv.h>

#define Uses_stdio
#define Uses_string
#define Uses_unistd
#define Uses_TEvent
#define Uses_TGKey
#define Uses_FullSingleKeySymbols
#define Uses_TScreen
#define Uses_TVCodePage
#define Uses_signal
#include <tv.h>

// I delay the check to generate as much dependencies as possible
#ifdef TVOSf_Linux

#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
// For the VT and key patch stuff
#include <sys/kd.h>
#include <sys/vt.h>

#include <tv/linux/key.h>
#include <tv/linux/screen.h>
#include <tv/linux/log.h>

int            TGKeyLinux::hIn=-1;
FILE          *TGKeyLinux::fIn=NULL;
int            TGKeyLinux::oldInFlags;
int            TGKeyLinux::newInFlags;
struct termios TGKeyLinux::inTermiosOrig;
struct termios TGKeyLinux::inTermiosNew;
const char    *TGKeyLinux::error=NULL;
int            TGKeyLinux::bufferKeys[MaxLenEscapeSequence];
int            TGKeyLinux::keysInBuffer=0;
int            TGKeyLinux::nextKey=-1;
int            TGKeyLinux::lastKeyCode;
int            TGKeyLinux::lastModifiers;
int            TGKeyLinux::translatedModifiers;
char           TGKeyLinux::dontPatchKeyboard=0;
int            TGKeyLinux::ourVT=-1;
struct vt_mode TGKeyLinux::oldVTMode;
struct vt_mode TGKeyLinux::newVTMode;
char           TGKeyLinux::vtHooked=0;
char           TGKeyLinux::canPatchKeyboard=0;
char           TGKeyLinux::keyPatched=0;
char           TGKeyLinux::ascii;
struct kbentry TGKeyLinux::entry;

/* Linux IOCTL values found experimentally */
const int kblNormal=0,kblShift=1,kblAltR=2,kblCtrl=4,kblAltL=8;

/**[txh]********************************************************************

  Description:
  Does initialization tasks performed only once.
  
  Return:
  0 if success, !=0 if an error ocurred. In the last case the error member
points to a descriptive error.
  
***************************************************************************/

int TGKeyLinux::InitOnce()
{
 LOG("TGKeyLinux::InitOnce");
 hIn=fileno(stdin);
 
 if (!isatty(hIn))
   {
    error=_("that's an interactive application, don't redirect stdin");
    return 1;
   }
 //  We can't use stdin for all the operations, instead we must open it again
 // using another file descriptor.
 //  Here is why: In order to get some keys, I saw it for ESC pressed alone,
 // we must set the O_NONBLOCK attribute.
 //  We can do it, but the stdout file handle seems to be just a copy of the
 // stdin file handle (dupped?). When you use duplicated file handles they
 // share the "File Status Flags". It means that setting O_NONBLOCK will
 // set O_NONBLOCK for output too. So what? well when we do that the one that
 // doesn't block is Linux kernel, so if we send too much information to
 // stdout, no matters if we use fflush, sometimes Linux could lose data.
 //  This effect seems to happend at exit, may be because owr process dies
 // before Linux have a chance to process the remaining data and it closes
 // the file handle. The fact is that using O_NONBLOCK sometimes we fail to
 // restore the cursor position.
 //  Now a question remains: do I have to restore the mode?
 char *ttyName=ttyname(hIn);
 if (!ttyName)
   {
    error=_("failed to get the name of the current terminal used for input");
    return 3;
   }
 fIn=fopen(ttyName,"r+b");
 if (!fIn)
   {
    error=_("failed to open the input terminal");
    return 4;
   }
 hIn=fileno(fIn);

 if (tcgetattr(hIn,&inTermiosOrig))
   {
    error=_("can't get input terminal attributes");
    return 2;
   }

 memcpy(&inTermiosNew,&inTermiosOrig,sizeof(inTermiosNew));
 // Ignore breaks
 inTermiosNew.c_iflag|= (IGNBRK | BRKINT);
 // Disable Xon/off
 inTermiosNew.c_iflag&= ~(IXOFF | IXON);
 // Character oriented, no echo, no signals
 inTermiosNew.c_lflag&= ~(ICANON | ECHO | ISIG);
 if (tcsetattr(hIn,TCSAFLUSH,&inTermiosNew))
   {
    error=_("can't set input terminal attributes");
    return 3;
   }
 // Don't block, needed to get some keys, even when the input is in character
 // oriented mode. I saw it for ESC alone.
 oldInFlags=fcntl(hIn,F_GETFL,0);
 newInFlags=oldInFlags | O_NONBLOCK;
 fcntl(hIn,F_SETFL,newInFlags);

 // Find if we are running in a VT and if that's the case the number
 // This name should be the same but ...
 ttyName=ttyname(STDOUT_FILENO);
 if (ttyName)
   {
    if (sscanf(ttyName,"/dev/tty%2d",&ourVT)!=1)
       // SET: Some Slackware systems (I think that is what Andris uses)
       // define /dev/ttyNN as symlinks to /dev/vc/NN:
       if (sscanf(ttyName,"/dev/vc/%2d",&ourVT)!=1)
          ourVT=-1;
   }
 // Get the mode of the current VT
 if (ourVT!=-1)
   {
    if (ioctl(hIn,VT_GETMODE,&oldVTMode))
       // If we fail disable it
       ourVT=-1;
    else
      {// Tell the kernel to inform us about console changes
       newVTMode=oldVTMode;
       newVTMode.mode  =VT_PROCESS;
       newVTMode.relsig=SIGUSR1;
       newVTMode.acqsig=SIGUSR2;
      }
   }
 // Check if we can patch the keyboard
 canPatchKeyboard=0;
 long optPatchKeys=1; // Default is patch the keyboard
 TScreen::optSearch("PatchKeys",optPatchKeys);
 if (optPatchKeys && ourVT!=-1 && ioctl(hIn,KDGKBENT,&entry)==0)
   {
    canPatchKeyboard=1;
    keyMapInit();
   }
 // Now do the hook/patching
 doHookAndPatch();
 // We don't need to call Resume
 suspended=0;
 return 0;
}

/**[txh]********************************************************************

  Description:
  Restore the original console state.
  
***************************************************************************/

void TGKeyLinux::Suspend()
{
 doUnHookAndUnPatch();
 // Now we use a new opened file handle for input.
 // As these flags are the flags for the file handle (not the terminal itself)
 // we don't need to restore the originals.
 // Why don't do it anyways? because the TScreen::resume is called before and
 // it needs to get the cursor position using escape sequences without waiting
 // for an EOL.
 //fcntl(hIn,F_SETFL,oldInFlags);
 tcsetattr(hIn,TCSAFLUSH,&inTermiosOrig);
 LOG("TGKeyLinux::Suspend");
}

/**[txh]********************************************************************

  Description:
  Memorize current console state and setup the one needed for us.
  
***************************************************************************/

void TGKeyLinux::Resume()
{// Read current state
 tcgetattr(hIn,&inTermiosOrig);
 oldInFlags=fcntl(hIn,F_GETFL,0);
 // Set our state
 tcsetattr(hIn,TCSAFLUSH,&inTermiosNew);
 // The following shouldn't be needed, I'll let it here unless I discover
 // some side effect.
 fcntl(hIn,F_SETFL,newInFlags);
 // The user could do some action to alter the keyboard mapping tables
 if (canPatchKeyboard)
    keyMapInit();
 // Patch keyboard and hook the signals
 doHookAndPatch();
 LOG("TGKeyLinux::Resume");
}

int TGKeyLinux::KbHit()
{
 if (keysInBuffer || nextKey!=-1)
    return 1;     // We have a key waiting for processing
 nextKey=fgetc(fIn);
 return nextKey!=-1;
}

void TGKeyLinux::Clear()
{
 // Discard our buffer
 keysInBuffer=0;
 // Discard a key waiting
 nextKey=-1;
 // Flush the input
 fflush(fIn);
}

/*****************************************************************************

  Here starts the keyboard parser and translator.
  It uses a static tree/hash to parse the escape sequences, may be this should
be configurable.

*****************************************************************************/

// -  9 = Tab tiene conflicto con kbI+Control lo cual es natural, por otro
// -lado Ctrl+Tab no lo reporta en forma natural
// -  a = Enter tiene conflicto con ^J, como ^J no lo reporta naturalmente sino
// -forzado por el keymap lo mejor es definirlo directamente.
unsigned char TGKeyLinux::kbToName[128] =
{
 0,kbA,kbB,kbC,kbD,kbE,kbF,kbG,                                           // 00-07
 kbH,kbTab,kbEnter,kbK,kbL,kbM,kbN,kbO,                                   // 08-0F
 kbP,kbQ,kbR,kbS,kbT,kbU,kbV,kbW,                                         // 10-17
 kbX,kbY,kbZ,kbEsc,0,kbCloseBrace,kb6,kbMinus,                            // 18-1F
 kbSpace,kbAdmid,kbDobleQuote,kbNumeral,kbDolar,kbPercent,kbAmper,kbQuote,// 20-27
 kbOpenPar,kbClosePar,kbAsterisk,kbPlus,kbComma,kbMinus,kbStop,kbSlash,   // 28-2F
 kb0,kb1,kb2,kb3,kb4,kb5,kb6,kb7,                                         // 30-37
 kb8,kb9,kbDoubleDot,kbColon,kbLessThan,kbEqual,kbGreaterThan,kbQuestion, // 38-3F
 kbA_Roba,kbA,kbB,kbC,kbD,kbE,kbF,kbG,                                    // 40-47
 kbH,kbI,kbJ,kbK,kbL,kbM,kbN,kbO,                                         // 48-4F
 kbP,kbQ,kbR,kbS,kbT,kbU,kbV,kbW,                                         // 50-57
 kbX,kbY,kbZ,kbOpenBrace,kbBackSlash,kbCloseBrace,kbCaret,kbUnderLine,    // 58-5F
 kbGrave,kbA,kbB,kbC,kbD,kbE,kbF,kbG,                                     // 60-67
 kbH,kbI,kbJ,kbK,kbL,kbM,kbN,kbO,                                         // 68-6F
 kbP,kbQ,kbR,kbS,kbT,kbU,kbV,kbW,                                         // 70-77
 kbX,kbY,kbZ,kbOpenCurly,kbOr,kbCloseCurly,kbTilde,kbBackSpace            // 78-7F
};

unsigned char TGKeyLinux::kbExtraFlags[128] =
{
 0,kblCtrl,kblCtrl,kblCtrl,kblCtrl,kblCtrl,kblCtrl,kblCtrl,               // 00-07
 kblCtrl,0,0,kblCtrl,kblCtrl,kblCtrl,kblCtrl,kblCtrl,                     // 08-0F
 kblCtrl,kblCtrl,kblCtrl,kblCtrl,kblCtrl,kblCtrl,kblCtrl,kblCtrl,         // 10-17
 kblCtrl,kblCtrl,kblCtrl,0,0,kblCtrl,kblCtrl,kblCtrl,                     // 18-1F
 0,kblShift,kblShift,kblShift,kblShift,kblShift,kblShift,0,               // 20-27
 kblShift,kblShift,kblShift,kblShift,0,0,0,0,                             // 28-2F
 0,0,0,0,0,0,0,0,                                                         // 30-37
 0,0,kblShift,0,kblShift,0,kblShift,kblShift,                             // 38-3F
 kblShift,kblShift,kblShift,kblShift,kblShift,kblShift,kblShift,kblShift, // 40-47
 kblShift,kblShift,kblShift,kblShift,kblShift,kblShift,kblShift,kblShift, // 48-4F
 kblShift,kblShift,kblShift,kblShift,kblShift,kblShift,kblShift,kblShift, // 50-57
 kblShift,kblShift,kblShift,0,0,0,kblShift,kblShift,                      // 58-5F
 0,0,0,0,0,0,0,0,                                                         // 60-67
 0,0,0,0,0,0,0,0,                                                         // 68-6F
 0,0,0,0,0,0,0,0,                                                         // 70-77
 0,0,0,kblShift,kblShift,kblShift,kblShift,0,                             // 78-7F
};

/************************** Escape sequences tree **************************/
struct node
{
 char value;
 unsigned char code;
 unsigned char modifiers;
 node *next;
};

static node F6[]={{1,0,0},{'~',kbF6,0,0}};
static node F7[]={{1,0,0},{'~',kbF7,0,0}};
static node F8[]={{1,0,0},{'~',kbF8,0,0}};
static node F9[]={{1,0,0},{'~',kbF9,0,0}};
static node F10[]={{1,0,0},{'~',kbF10,0,0}};
static node F11[]={{1,0,0},{'~',kbF11,0,0}};
static node F12[]={{1,0,0},{'~',kbF12,0,0}};
static node Delete[]={{1,0,0},{'~',kbDelete,0,0}};
static node End[]={{1,0,0},{'~',kbEnd,0,0}};
static node PgUp[]={{1,0,0},{'~',kbPgUp,0,0}};
static node PgDn[]={{1,0,0},{'~',kbPgDn,0,0}};

static node Brace1[]=
{
 {4,0,0},
 {'~',kbHome,0,0},
 {'7',0,0,F6},
 {'8',0,0,F7},
 {'9',0,0,F8}
};

static node Brace2[]=
{
 {5,0,0},
 {'~',kbInsert,0,0},
 {'0',0,0,F9},
 {'1',0,0,F10},
 {'3',0,0,F11},
 {'4',0,0,F12}
};

static node BraceKeys[]=
{
 {5,0,0},
 {'A',kbF1,0,0},
 {'B',kbF2,0,0},
 {'C',kbF3,0,0},
 {'D',kbF4,0,0},
 {'E',kbF5,0,0},
};

static node Sequences[]=
{
 {13,0,0},
 {'[',0,0,BraceKeys},
 {'A',kbUp,0,0},
 {'B',kbDown,0,0},
 {'C',kbRight,0,0},
 {'D',kbLeft,0,0},
 {'G',kb5,0,0},
 {'P',kbPause,0,0},
 {'1',0,0,Brace1},
 {'2',0,0,Brace2},
 {'3',0,0,Delete},
 {'4',0,0,End},
 {'5',0,0,PgUp},
 {'6',0,0,PgDn}
};

static node Keys[]=
{
 {1,0,0},
 {'[',0,0,Sequences}
};
/*********************** End of escape sequences tree ***********************/

/**[txh]********************************************************************

  Description:
  Parse a escape sequence.

  Returns: 1 if the sequence found, 0 if not and the keys are stored in the
buffer.
  
***************************************************************************/

int TGKeyLinux::ProcessEscape()
{
 int nextVal;

 nextVal=fgetc(fIn);
 if (nextVal==EOF)     // Just ESC
    return 0;

 node *p=Keys;
 int cant,i;
 keysInBuffer=0;
 while (nextVal!=EOF)
   {
    NextNode:
    bufferKeys[keysInBuffer++]=nextVal;
    cant=p->value;
    for (i=1; i<=cant; i++)
       {
        if (p[i].value==nextVal)
          {
           if (p[i].next)
             {
              p=p[i].next;
              nextVal=fgetc(fIn);
              goto NextNode;
             }
           lastKeyCode=p[i].code;
           lastModifiers=p[i].modifiers;
           keysInBuffer=0;
           return 1;
          }
       }
    return 0;
   }
 return 0;
}

int TGKeyLinux::GetKeyFromBuffer()
{
 int ret=bufferKeys[--keysInBuffer];
 if (keysInBuffer)
    memcpy(bufferKeys,bufferKeys+1,keysInBuffer);
 return ret;
}

/**[txh]********************************************************************

  Description:
  Gets a key from the buffer, waiting value or stdin and if needed calls
the escape sequence parser.
  
***************************************************************************/

int TGKeyLinux::GetKeyParsed()
{
 lastModifiers=0;
 translatedModifiers=-1;
 // If we have keys in the buffer take from there.
 // They already failed the escape sequence test.
 if (keysInBuffer)
    return GetKeyFromBuffer();

 // Use the value we have on hold or get a new one
 int nextVal=nextKey;
 nextKey=-1;
 if (nextVal==-1)
    nextVal=fgetc(fIn);
 if (nextVal==-1)
    return -1;

 // Is that an escape sequence?
 if (nextVal=='\e')
   {
    if (ProcessEscape())
       return -2;
    if (!keysInBuffer)
       return '\e';
    lastKeyCode=GetKeyFromBuffer();
    lastModifiers=kblAltL;
    return -3;
   }

 return nextVal;
}

/**[txh]********************************************************************

  Description:
  Gets the next key, their modifiers and ASCII. Is a postprocessor for
GetKeyParsed.
  
***************************************************************************/

int TGKeyLinux::GetRaw()
{
 int result=GetKeyParsed();

 if (result==-1)
    return 0;   // No key
 if (result==-2)
   {
    lastModifiers|=GetLinuxShiftState();
    ascii=0;
    return 1;   // Key already processed
   }
 if (result==-3) // Forced modifier
   {
    result=lastKeyCode;
    ascii=result>127 ? result : 0;
   }
 else
    ascii=result;

 // Translate the key
 if (result>=128)
    lastKeyCode=kbUnkNown;
 else
   {
    lastModifiers|=kbExtraFlags[result];
    lastKeyCode=kbToName[result];
   }
 lastModifiers|=GetLinuxShiftState();
 return 1;
}

/**[txh]********************************************************************

  Description:
  Gets a key from the input and converts it into the TV format.
  
***************************************************************************/

ushort TGKeyLinux::GKey()
{
 if (GetRaw())
   { // Here I add the modifiers to the key code
    if (lastModifiers & kblShift)
       lastKeyCode|=kbShiftCode;
    if (lastModifiers & kblCtrl)
       lastKeyCode|=kbCtrlCode;
    switch (AltSet)
      {
       case 0: // Normal thing, left is left, right is right
            if (lastModifiers & kblAltL)
               lastKeyCode|=kbAltLCode;
            else
               if (lastModifiers & kblAltR)
                  lastKeyCode|=kbAltRCode;
            break;
       case 1: // Reverse thing
            if (lastModifiers & kblAltL)
               lastKeyCode|=kbAltRCode;
            else
               if (lastModifiers & kblAltR)
                  lastKeyCode|=kbAltLCode;
            break;
       default: // Compatibility
            if (lastModifiers & (kblAltL | kblAltR))
               lastKeyCode|=kbAltLCode;
      }
    return lastKeyCode;
   }
 return kbUnkNown;
}
/*****************************************************************************
  End of parser and translator.
*****************************************************************************/

/**[txh]********************************************************************

  Description:
  Finds the value of the modifiers, if the ioctl fails the values we know
from the last key are used.

  Return: The modifiers in Linux kernel format.
  
***************************************************************************/

unsigned TGKeyLinux::GetLinuxShiftState()
{
 int arg=6; /* TIOCLINUX function #6 */
 unsigned shift=0;
 
 if (ioctl(hIn,TIOCLINUX,&arg)!=-1)
    shift=arg;
 else
    shift=lastModifiers;
 return shift;
}

/**[txh]********************************************************************

  Description:
  Finds the value of the modifiers in TV format.
  
***************************************************************************/

unsigned TGKeyLinux::GetShiftState()
{
 lastModifiers=GetLinuxShiftState();
 if (!lastModifiers) return 0;
 if (translatedModifiers==-1)
   {
    translatedModifiers=0;
    if (lastModifiers & kblShift)
       translatedModifiers|=kbLeftShiftDown | kbRightShiftDown;
    if (lastModifiers & kblCtrl)
       translatedModifiers|=kbLeftCtrlDown | kbRightCtrlDown | kbCtrlDown;
    if (lastModifiers & kblAltR)
       translatedModifiers|=kbRightAltDown | kbAltDown;
    if (lastModifiers & kblAltL)
       translatedModifiers|=kbLeftAltDown | kbAltDown;
   }
 return translatedModifiers;
}


/**[txh]********************************************************************

  Description:
  Fills the TV event structure for a key.
  
***************************************************************************/

void TGKeyLinux::FillTEvent(TEvent &e)
{
 GKey();
 e.keyDown.charScan.charCode=lastModifiers & kblAltL ? 0 : ascii;
 e.keyDown.charScan.scanCode=ascii;
 e.keyDown.charCode=ascii; // Needed for GetAltChar
 e.keyDown.raw_scanCode=ascii;
 e.keyDown.keyCode=lastKeyCode;
 e.keyDown.shiftState=lastModifiers;
 e.what=evKeyDown;
}


/*****************************************************************************

  Here starts the code to modify Linux kernel tables in order to get better
keyboard information.
  Basically it makes Linux kernel map the key+modifiers to the key alone.
That's because these keys acts in special ways when a modifier is applied and
the application can't receive the key.
  An example: Shift+PgUp is usually mapped to "scroll up one screen", so we
map it to just PgUp. In this way we get the key, then we get the modifiers
from the ioctl.
  That's the code based in Robert's code it was readapted but the idea is
the same.
  The code is complicated because we must restore the map when the user
changes to another console. So most of the code deals with the signals
related to virtual console switching.

*****************************************************************************/

typedef struct
{
 uchar  change_table;
 uchar  change_index;
 uchar  old_table;
 uchar  old_index;
 ushort old_val;
 ushort new_val;
} change_entry;

#define SCAN_F1   0x3b
#define SCAN_F2   0x3c
#define SCAN_F3   0x3d
#define SCAN_F4   0x3e
#define SCAN_F5   0x3f
#define SCAN_F6   0x40
#define SCAN_F7   0x41
#define SCAN_F8   0x42
#define SCAN_F9   0x43
#define SCAN_F10  0x44
#define SCAN_Q    0x10
#define SCAN_S    0x1f
#define SCAN_J    0x24
#define SCAN_M    0x32
#define SCAN_Z    0x2C
#define SCAN_PGUP  104
#define SCAN_PGDN  109
#define SCAN_BKSP   14
#define SCAN_SPAC   57
#define SCAN_TAB    15
// As ^[ is usually Esc we must be sure it doesn't happend
#define SCAN_LFBR   26
// The Scroll lock key is evil for an interactive application
// As in my keyboard that's above Home and some people wrongly
// press it instead of Home I'm translating to Home.
#define SCAN_SCRL   70
#define SCAN_HOME  102

static
change_entry changes[]=
{
 { kblAltL,  SCAN_F1,   kblNormal, SCAN_F1,   0, 0},
 { kblAltR,  SCAN_F1,   kblNormal, SCAN_F1,   0, 0},
 { kblCtrl,  SCAN_F1,   kblNormal, SCAN_F1,   0, 0},
 { kblShift, SCAN_F1,   kblNormal, SCAN_F1,   0, 0},
 { kblAltL,  SCAN_F2,   kblNormal, SCAN_F2,   0, 0},
 { kblAltR,  SCAN_F2,   kblNormal, SCAN_F2,   0, 0},
 { kblCtrl,  SCAN_F2,   kblNormal, SCAN_F2,   0, 0},
 { kblShift, SCAN_F2,   kblNormal, SCAN_F2,   0, 0},
 { kblAltL,  SCAN_F3,   kblNormal, SCAN_F3,   0, 0},
 { kblAltR,  SCAN_F3,   kblNormal, SCAN_F3,   0, 0},
 { kblCtrl,  SCAN_F3,   kblNormal, SCAN_F3,   0, 0},
 { kblShift, SCAN_F3,   kblNormal, SCAN_F3,   0, 0},
 { kblAltL,  SCAN_F4,   kblNormal, SCAN_F4,   0, 0},
 { kblAltR,  SCAN_F4,   kblNormal, SCAN_F4,   0, 0},
 { kblCtrl,  SCAN_F4,   kblNormal, SCAN_F4,   0, 0},
 { kblShift, SCAN_F4,   kblNormal, SCAN_F4,   0, 0},
 { kblAltL,  SCAN_F5,   kblNormal, SCAN_F5,   0, 0},
 { kblAltR,  SCAN_F5,   kblNormal, SCAN_F5,   0, 0},
 { kblCtrl,  SCAN_F5,   kblNormal, SCAN_F5,   0, 0},
 { kblShift, SCAN_F5,   kblNormal, SCAN_F5,   0, 0},
 { kblAltL,  SCAN_F6,   kblNormal, SCAN_F6,   0, 0},
 { kblAltR,  SCAN_F6,   kblNormal, SCAN_F6,   0, 0},
 { kblCtrl,  SCAN_F6,   kblNormal, SCAN_F6,   0, 0},
 { kblShift, SCAN_F6,   kblNormal, SCAN_F6,   0, 0},
 { kblAltL,  SCAN_F7,   kblNormal, SCAN_F7,   0, 0},
 { kblAltR,  SCAN_F7,   kblNormal, SCAN_F7,   0, 0},
 { kblCtrl,  SCAN_F7,   kblNormal, SCAN_F7,   0, 0},
 { kblShift, SCAN_F7,   kblNormal, SCAN_F7,   0, 0},
 { kblAltL,  SCAN_F8,   kblNormal, SCAN_F8,   0, 0},
 { kblAltR,  SCAN_F8,   kblNormal, SCAN_F8,   0, 0},
 { kblCtrl,  SCAN_F8,   kblNormal, SCAN_F8,   0, 0},
 { kblShift, SCAN_F8,   kblNormal, SCAN_F8,   0, 0},
 { kblAltL,  SCAN_F9,   kblNormal, SCAN_F9,   0, 0},
 { kblAltR,  SCAN_F9,   kblNormal, SCAN_F9,   0, 0},
 { kblCtrl,  SCAN_F9,   kblNormal, SCAN_F9,   0, 0},
 { kblShift, SCAN_F9,   kblNormal, SCAN_F9,   0, 0},
 { kblAltL,  SCAN_F10,  kblNormal, SCAN_F10,  0, 0},
 { kblAltR,  SCAN_F10,  kblNormal, SCAN_F10,  0, 0},
 { kblCtrl,  SCAN_F10,  kblNormal, SCAN_F10,  0, 0},
 { kblShift, SCAN_F10,  kblNormal, SCAN_F10,  0, 0},
 { kblCtrl,  SCAN_Q,    kblNormal, SCAN_Q,    0, 0},
 { kblCtrl,  SCAN_S,    kblNormal, SCAN_S,    0, 0},
 { kblCtrl,  SCAN_J,    kblNormal, SCAN_J,    0, 0},
 { kblCtrl,  SCAN_M,    kblNormal, SCAN_M,    0, 0},
 { kblCtrl,  SCAN_Z,    kblNormal, SCAN_Z,    0, 0},
 { kblShift, SCAN_PGUP, kblNormal, SCAN_PGUP, 0, 0},
 { kblShift, SCAN_PGDN, kblNormal, SCAN_PGDN, 0, 0},
 { kblCtrl,  SCAN_BKSP, kblNormal, SCAN_BKSP, 0, 0},
 { kblCtrl,  SCAN_SPAC, kblNormal, SCAN_SPAC, 0, 0},
 { kblCtrl,  SCAN_LFBR, kblNormal, SCAN_LFBR, 0, 0},
 { kblNormal,SCAN_SCRL, kblNormal, SCAN_HOME, 0, 0},
 { kblAltL,  SCAN_SCRL, kblNormal, SCAN_HOME, 0, 0},
 { kblAltR,  SCAN_SCRL, kblNormal, SCAN_HOME, 0, 0},
 { kblCtrl,  SCAN_TAB,  kblNormal, SCAN_TAB,  0, 0},
 { kblCtrl | kblShift,  SCAN_TAB, kblNormal, SCAN_TAB, 0, 0}
};

#define changeSize (sizeof(changes)/sizeof(change_entry))

void TGKeyLinux::unPatchKeyMap()
{
 if (!canPatchKeyboard || !keyPatched)
    return;

 unsigned i;
 for (i=0;i<changeSize;i++)
    {
     change_entry *e=&changes[i];
     entry.kb_table=e->change_table;
     entry.kb_index=e->change_index;
     entry.kb_value=e->old_val;
     ioctl(hIn,KDSKBENT,&entry);
    }
 keyPatched=0;
}

void TGKeyLinux::patchKeyMap()
{
 if (!canPatchKeyboard || keyPatched)
    return;

 unsigned i;
 for (i=0;i<changeSize;i++)
    {
     change_entry *e=&changes[i];
     entry.kb_table=e->change_table;
     entry.kb_index=e->change_index;
     entry.kb_value=e->new_val;
     ioctl(hIn,KDSKBENT,&entry);
    }
 keyPatched=1;
}

// Get the information needed to patch/unpatch
void TGKeyLinux::keyMapInit()
{
 unsigned i;
 for (i=0; i<changeSize; i++)
    {
     change_entry *e=&changes[i];
     entry.kb_table=e->change_table;
     entry.kb_index=e->change_index;
     ioctl(hIn,KDGKBENT,&entry);
     e->old_val=entry.kb_value;
     entry.kb_table=e->old_table;
     entry.kb_index=e->old_index;
     ioctl(hIn,KDGKBENT,&entry);
     e->new_val=entry.kb_value;
    }
}

void TGKeyLinux::hookVTSignals()
{
 if (vtHooked || dontPatchKeyboard || ourVT==-1)
    return;

 // -------- Set up signal handlers to know about console switches
 struct sigaction sig;
 sigemptyset(&sig.sa_mask);
 sigaddset(&sig.sa_mask,SIGUSR1);
 sigaddset(&sig.sa_mask,SIGUSR2);
 sig.sa_flags=SA_RESTART;
 sigprocmask(SIG_BLOCK,&sig.sa_mask,NULL); // No switches now, we are not
                                           // initialized yet
 sig.sa_handler=releaseVTHandler;
 sigaction(SIGUSR1,&sig,NULL);
 sig.sa_handler=acquireVTHandler;
 sigaction(SIGUSR2,&sig,NULL);
 vtHooked=1;

 // -------- Tell our console to inform us about switches
 if (ioctl(hIn,VT_SETMODE,&newVTMode))
   {
    error=_("ioctl VT_SETMODE failed");
    return;
   }

 sigprocmask(SIG_UNBLOCK,&sig.sa_mask,NULL);
}

void TGKeyLinux::unHookVTSignals()
{
 if (!vtHooked || ourVT==-1)
    return;

 // Make both signals to behave as default
 struct sigaction sig;
 sigemptyset(&sig.sa_mask);
 sigaddset(&sig.sa_mask,SIGUSR1);
 sigaddset(&sig.sa_mask,SIGUSR2);
 sig.sa_flags=SA_RESTART;
 sigprocmask(SIG_BLOCK,&sig.sa_mask,NULL); // No switches now, we are not
                                           // initialized yet
 sig.sa_handler=SIG_DFL;
 sigaction(SIGUSR1,&sig,NULL);
 sig.sa_handler=SIG_DFL;
 sigaction(SIGUSR2,&sig,NULL);

 ioctl(hIn,VT_SETMODE,&oldVTMode);

 sigprocmask(SIG_UNBLOCK,&sig.sa_mask,NULL);
 vtHooked=0;
}

void TGKeyLinux::releaseVTHandler(int)
{
 unPatchKeyMap();
 TMouse::suspend();
 TScreenLinux::SuspendFont();
 ioctl(hIn,VT_RELDISP,1);
}

void TGKeyLinux::acquireVTHandler(int)
{
 ioctl(hIn,VT_RELDISP,VT_ACKACQ);
 ioctl(hIn,VT_WAITACTIVE,ourVT);
 patchKeyMap();
 TMouse::resume();
 TScreenLinux::ResumeFont();
}

void TGKeyLinux::doHookAndPatch()
{
 if (!dontPatchKeyboard && canPatchKeyboard)
   {
    patchKeyMap();
    hookVTSignals();
   }
}

void TGKeyLinux::doUnHookAndUnPatch()
{
 unPatchKeyMap();
 unHookVTSignals();
}
/*****************************************************************************
  End of keyboard patching and VT change hooking
*****************************************************************************/

void TGKeyLinux::SetKbdMapping(int version)
{
 if (version==linuxDisableKeyPatch)
   {
    dontPatchKeyboard=1;
    doUnHookAndUnPatch();
   }
 else if (version==linuxEnableKeyPatch)
   {
    dontPatchKeyboard=0;
    doHookAndPatch();
   }
 Mode=version;
}

int TGKeyLinux::GetKbdMapping(int version)
{
 if (version==linuxDisableKeyPatch)
   {
    return dontPatchKeyboard;
   }
 else if (version==linuxEnableKeyPatch)
   {
    return !dontPatchKeyboard;
   }
 return 0;
}

void TGKeyLinux::Init(int map)
{
 TGKey::Suspend       =TGKeyLinux::Suspend;
 TGKey::Resume        =TGKeyLinux::Resume;
 TGKey::kbhit         =KbHit;
 TGKey::clear         =Clear;
 TGKey::gkey          =GKey;
 TGKey::getShiftState =GetShiftState;
 TGKey::fillTEvent    =FillTEvent;
 TGKey::SetKbdMapping =TGKeyLinux::SetKbdMapping;
 TGKey::GetKbdMapping =TGKeyLinux::GetKbdMapping;
 if (map==KOI8)
   {
    TGKey::SetCodePage(TVCodePage::KOI8r);
    LOG("Using KOI8 keyboard table");
   }
}
#else // TVOSf_Linux

#include <tv/linux/key.h>
#include <tv/linux/log.h>

#endif // else TVOSf_Linux
