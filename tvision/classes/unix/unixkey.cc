/*****************************************************************************

 Todo:
 Volar esto: int use_real_keyboard_bios=0;

 This code is GPL, see copying file for more details.
 Keyboard handler for Linux Copyright by Salvador E. Tropea (SET) (1998,1999)

 That's the counterpart of the gkey.cc for DOS that I created some months ago
it uses curses input and tries to detect the shift/control status from the
keycode reported by curses. Additionally the routines detects ALT using the
ESC-keycode sequence (META+key) with a patch in the low level tv_GetRaw.

RH: I moved that patch to the TGKey::gkey() member

 To make it better in the Linux console I'm mixing the modifiers flags from
the ioctl call with the ones I figured out from the code.

 It fixes a lot of problems that appears when running from telnet, but not
all. Lamentably looks like telnet standard protocols can't simply report all
the keyboard combinations. We lose some important things like Ctrl+Function
key, Shift+(Inset,End,Home,PgUp,PgDn,Delete,Arrows,etc.) and more.

*****************************************************************************/
#include <tv/configtv.h>

#define Uses_stdio
#define Uses_unistd
#define Uses_ctype
#define Uses_stdlib
#define Uses_TEvent
#define Uses_TGKey
#define Uses_FullSingleKeySymbols
#define Uses_string
#include <tv.h>

// I delay the check to generate as much dependencies as possible
#if defined(HAVE_NCURSES) && defined(TVOS_UNIX) && !defined(TVOSf_QNXRtP)

#include <tv/unix/key.h>

// New curses (ncurses) headers
#ifdef TVOSf_FreeBSD
 #include <ncurses.h>
#else
 #include <curses.h>
#endif
#include <term.h>

#ifdef HAVE_KEYSYMS
 // X11R6 keysyms list
 #include <X11/keysym.h>
#endif

//#define GKEY
#define TOSTDERR
#ifndef GKEY
 #define dbprintf(a...)
#else
 #ifdef TOSTDERR
  #define dbprintf(a...) fprintf(stderr,a)
 #else
  #define dbprintf(a...) printf(a)
 #endif
#endif

/* Linux IOCTL values found experimentally */
const int kblNormal=0,kblShift=1,kblAltR=2,kblCtrl=4,kblAltL=8;
const int MouseB1Down=0x20,MouseB2Down=0x21,MouseB3Down=0x22,MouseUp=0x23;

#define IN_FD fileno(stdin)
#define META_MASK 0x8000
#define NCursesGetCh   getch
#define NCursesUnGetCh ungetch
// Who knows what's that?
#undef clear

int use_real_keyboard_bios=0;
// xterm is a crappy terminal and does all in a way too different to the
// standard.
static int XtermMode=0;
static int MouseButtons=0;

int     TGKeyUNIX::Abstract;
char    TGKeyUNIX::ascii;
ushort  TGKeyUNIX::sFlags;
KeyType TGKeyUNIX::rawCode;

// -  9 = Tab tiene conflicto con kbI+Control lo cual es natural, por otro
// -lado Ctrl+Tab no lo reporta en forma natural
// -  a = Enter tiene conflicto con ^J, como ^J no lo reporta naturalmente sino
// -forzado por el keymap lo mejor es definirlo directamente.
static
unsigned char kbToName1[128] =
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

// 15e = kb5 (del keypad)
static
unsigned char kbToName2[128] =
{
 0,0,kbDown,kbUp,kbLeft,kbRight,kbHome,kbBackSpace, // 00-07
 0,kbF1,kbF2,kbF3,kbF4,kbF5,kbF6,kbF7,              // 08-0F
 kbF8,kbF9,kbF10,kbF1,kbF2,kbF3,kbF4,kbF5,          // 10-17
 kbF6,kbF7,kbF8,kbF9,kbF10,0,0,0,                   // 18-1F
 0,0,0,0,0,0,0,0,                                   // 20-27
 0,0,0,0,0,0,0,0,                                   // 28-2F
 0,0,0,0,0,0,0,0,                                   // 30-37
 0,0,0,0,0,kbLeft,kbDown,kbRight,                   // 38-3F
 kbUp,kbEterm,kbHome,kbEnd,kbUp,kbDown,kbRight,kbLeft, // 40-47
 0,0,kbDelete,kbInsert,0,0,0,0,                     // 48-4F
 0,0,kbPgDn,kbPgUp,0,0,0,0,                         // 50-57
 0,0,0,0,0,0,kb5,0,                                 // 58-5F
 0,0,0,0,0,0,0,0,                                   // 60-67
 kbEnd,0,0,0,0,0,0,0,                               // 68-6F
 0,0,0,0,0,0,0,0,                                   // 70-77
 0,0,0,0,0,0,0,kbBackSpace                          // 78-7F
};

const unsigned kbCtrl =kbCtrlCode>>4;
const unsigned kbAlt  =kbAltLCode>>4;
const unsigned char kbShift=kbShiftCode>>4;

static
unsigned char kbExtraFlags1[128] =
{
 0,kbCtrl,kbCtrl,kbCtrl,kbCtrl,kbCtrl,kbCtrl,kbCtrl,              // 00-07
 kbCtrl,0,0,kbCtrl,kbCtrl,kbCtrl,kbCtrl,kbCtrl,                   // 08-0F
 kbCtrl,kbCtrl,kbCtrl,kbCtrl,kbCtrl,kbCtrl,kbCtrl,kbCtrl,         // 10-17
 kbCtrl,kbCtrl,kbCtrl,0,0,kbCtrl,kbCtrl,kbCtrl,                   // 18-1F
 0,kbShift,kbShift,kbShift,kbShift,kbShift,kbShift,0,             // 20-27
 kbShift,kbShift,kbShift,kbShift,0,0,0,0,                         // 28-2F
 0,0,0,0,0,0,0,0,                                                 // 30-37
 0,0,kbShift,0,kbShift,0,kbShift,kbShift,                         // 38-3F
 kbShift,kbShift,kbShift,kbShift,kbShift,kbShift,kbShift,kbShift, // 40-47
 kbShift,kbShift,kbShift,kbShift,kbShift,kbShift,kbShift,kbShift, // 48-4F
 kbShift,kbShift,kbShift,kbShift,kbShift,kbShift,kbShift,kbShift, // 50-57
 kbShift,kbShift,kbShift,0,0,0,kbShift,kbShift,                   // 58-5F
 0,0,0,0,0,0,0,0,                                                 // 60-67
 0,0,0,0,0,0,0,0,                                                 // 68-6F
 0,0,0,0,0,0,0,0,                                                 // 70-77
 0,0,0,kbShift,kbShift,kbShift,kbShift,0,                         // 78-7F
};

static
unsigned char kbExtraFlags2[128] =
{
 0,0,0,0,0,0,0,0,                               // 00-07
 0,0,0,0,0,0,0,0,                               // 08-0F
 0,0,0,kbShift,kbShift,kbShift,kbShift,kbShift, // 10-17
 kbShift,kbShift,kbShift,kbShift,kbShift,0,0,0, // 18-1F
 0,0,0,0,0,0,0,0,                               // 20-27
 0,0,0,0,0,0,0,0,                               // 28-2F
 0,0,0,0,0,0,0,0,                               // 30-37
 0,0,0,0,0,0,0,0,                               // 38-3F
 0,0,0,0,0,0,0,0,                               // 40-47
 0,0,0,0,0,0,0,0,                               // 48-4F
 0,0,0,0,0,0,0,0,                               // 50-57
 0,0,0,0,0,0,0,0,                               // 58-5F
 0,0,0,0,0,0,0,0,                               // 60-67
 0,0,0,0,0,0,0,0,                               // 68-6F
 0,0,0,0,0,0,0,0,                               // 70-77
 0,0,0,0,0,0,0,kbAlt                            // 78-7F
};

#ifdef HAVE_KEYSYMS
// This table should be filled at compile time, or I can trust the values
// are standard?!
static
unsigned char kbX11Keys[256];

typedef struct
{
 unsigned symbol;
 unsigned key;
} keyEquiv;

keyEquiv XEquiv[] =
{
 /* Cursor control & motion */
 {XK_Home,kbHome}, {XK_Left,kbLeft}, {XK_Up,kbUp}, {XK_Right,kbRight},
 {XK_Down,kbDown}, {XK_Page_Up,kbPgUp}, {XK_Page_Down, kbPgDn},
 {XK_End,kbEnd}, {XK_Begin,kbHome},
 /* Misc Functions */
 {XK_Insert, kbInsert}, {XK_Delete, kbDelete},
 /* TTY Functions */
 {XK_Return, kbEnter}, {XK_BackSpace, kbBackSpace}, {XK_Tab, kbTab},
 {XK_Escape, kbEsc},
 /* Auxilliary Functions */
 {XK_F1, kbF1}, {XK_F2, kbF2}, {XK_F3, kbF3}, {XK_F4, kbF4}, {XK_F5, kbF5},
 {XK_F6, kbF6}, {XK_F7, kbF7}, {XK_F8, kbF8}, {XK_F9, kbF9}, {XK_F10, kbF10},
 {XK_F11, kbF11}, {XK_F12, kbF12},
 /* Keypad Functions */
 {XK_KP_0,kb0}, {XK_KP_1,kb1}, {XK_KP_2,kb2}, {XK_KP_3,kb3}, {XK_KP_4,kb4},
 {XK_KP_5,kb5}, {XK_KP_6,kb6}, {XK_KP_7,kb7}, {XK_KP_8,kb8}, {XK_KP_9,kb9},
 {XK_KP_Enter, kbEnter}, {XK_KP_Home,kbHome}, {XK_KP_Left,kbLeft},
 {XK_KP_Up, kbUp}, {XK_KP_Right,kbRight}, {XK_KP_Down, kbDown},
 {XK_KP_Page_Up, kbPgUp}, {XK_KP_Page_Down,kbPgDn}, {XK_KP_End,kbEnd},
 {XK_KP_Begin, kbHome}, {XK_KP_Insert, kbInsert}, {XK_KP_Delete, kbDelete},
 {XK_KP_Equal, kbEqual}, {XK_KP_Multiply, kbAsterisk}, {XK_KP_Add, kbPlus},
 {XK_KP_Subtract, kbMinus}, {XK_KP_Decimal, kbStop}, {XK_KP_Divide, kbSlash},
 /* End */
 {0,0}
};
#endif // HAVE_KEYSYMS

void TGKeyUNIX::SetKbdMapping(int version)
{
 Mode=version;
 switch (version)
   {
    case unixXterm: // It can be combined with others
         #ifdef HAVE_DEFINE_KEY
         // SET: Here is a temporal workaround for Eterm when the user uses
         // the xterm terminfo file (normal in Debian).
         define_key("\e[7~",KEY_F(58)); // Home
         define_key("\e[8~",KEY_F(59)); // End
         define_key("\eOx",KEY_F(56)); // Keypad Up
         define_key("\eOv",KEY_F(55)); // Keypad Right
         define_key("\eOw",KEY_F(54)); // Keypad Down
         define_key("\eOt",KEY_F(53)); // Keypad Left
         #endif
         #ifdef KEY_MOUSE
         // This exists in 1.9.9 but not in 1.8.6
         // 0631 == KEY_MOUSE
         kbToName2[KEY_MOUSE & 0x7F]=kbMouse;
         kbExtraFlags2[KEY_MOUSE & 0x7F]=0;
         #endif
         XtermMode=1;
         break;
    case unixNoXterm:
         #ifdef KEY_MOUSE
         kbToName2[KEY_MOUSE & 0x7F]=kbF7;
         kbExtraFlags2[KEY_MOUSE & 0x7F]=kbShift;
         #endif
         XtermMode=0;
         break;
    case unixEterm:
         #if defined(HAVE_KEYSYMS) && defined(HAVE_DEFINE_KEY)
         // SET: I submited a patch to Eterm maintainers for it:
         define_key("\x1B[k",KEY_F(57)); // End
         memset(kbX11Keys,0,sizeof(kbX11Keys));
         {
          int i;
          for (i=0; XEquiv[i].symbol; i++)
              kbX11Keys[XEquiv[i].symbol & 0xFF]=XEquiv[i].key;
         }
         #endif
         #ifdef KEY_MOUSE
         // 0631 == KEY_MOUSE
         kbToName2[KEY_MOUSE & 0x7F]=kbMouse;
         kbExtraFlags2[KEY_MOUSE & 0x7F]=0;
         #endif
         XtermMode=2;
         break;
   }
}

int TGKeyUNIX::GetKbdMapping(int version)
{
 switch (version)
   {
    case unixXterm:
         return XtermMode==1;
    case unixNoXterm:
         return !XtermMode;
         break;
    case unixEterm:
         return XtermMode==2;
   }
 return 0;
}

// The intelligence is here
ushort TGKeyUNIX::GKey(void)
{
 Abstract=0;

 GetRaw();
 // Xterm hacks:
 if (XtermMode)
   {
    #ifdef KEY_MOUSE
    if (rawCode.full==KEY_MOUSE)
      {
       dbprintf("TGKey::gkey: Mouse event detected\r\n");
       Abstract=kbMouse;
       return rawCode.full;
      }
    #endif
    #ifdef HAVE_KEYSYMS
    if (rawCode.full==KEY_F(57))
      {
       dbprintf("TGKey::gkey: Special Eterm keysym detected\r\n");
       Abstract=kbEterm;
       return rawCode.full;;
      }
    #endif
   if (rawCode.full & 0x80)
     {
      sFlags|=kblAltL;
      rawCode.full &= ~0x80;
      dbprintf("Adding left alt because the code contains 0x80 and xterm detected\r\n");
     }
   }
 if (rawCode.full & META_MASK)
   {
    sFlags|=kblAltL;
    rawCode.full &= ~META_MASK;
    dbprintf("Adding left alt because the code contains META key\r\n");
   }
 
 //---- The following code takes advantage of the flags reported by the ioctl
 //---- call. As this mechanism isn't available if we aren't loged in the
 //---- console I add more information later.
 
 // Compose the shift flags:
 if (sFlags & kblShift)
    Abstract|=kbShiftCode;
 if (sFlags & kblCtrl)
    Abstract|=kbCtrlCode;
 switch (AltSet)
   {
    case 0: // Normal thing, left is left, right is right
         if (sFlags & kblAltL)
            Abstract|=kbAltLCode;
         else
            if (sFlags & kblAltR)
               Abstract|=kbAltRCode;
         break;
    case 1: // Reverse thing
         if (sFlags & kblAltL)
            Abstract|=kbAltRCode;
         else
            if (sFlags & kblAltR)
               Abstract|=kbAltLCode;
         break;
    default: // Compatibility
         if (sFlags & (kblAltL | kblAltR))
            Abstract|=kbAltLCode;
   }

 //--- Translate the rest
 unsigned char *name,*flags;
 // This values could be 0 or 1 because curses reports upto 512 keys
 unsigned isASCIIif;
 if (rawCode.b.scan)
   {
    name=kbToName2;
    flags=kbExtraFlags2;
    isASCIIif=128;
   }
 else
   {
    name=kbToName1;
    flags=kbExtraFlags1;
    isASCIIif=32;
   }
 unsigned key=rawCode.full;
 if (key & 0x80)
   { // Keys over 127 are interpreted as ASCII
    ascii=key;
    rawCode.b.scan=ascii;
    //Abstract|=kbUnknown;
   }
 else
   { // The rest are passed by the tables
    key&=0x7F;
    ascii=key>=isASCIIif ? key : 0;
    if (name[key]==kbTab) ascii='\t';
    rawCode.b.scan=ascii;
    Abstract|=name[key] | (flags[key]<<4);
    sFlags|=(flags[key]<<4);
   }
 return rawCode.full;
}

// All the info. from BIOS in one call
void TGKeyUNIX::GetRaw(void)
{
 int code;

 /* see if there is data available */
 if ((code=NCursesGetCh())!=ERR)
   {
    if (code==27)
      {
       if (kbhit())
         {
          int code2=NCursesGetCh();
          if (code2!=27)
             code=code2 | META_MASK;
         }
      }
   }
 else
   {
    rawCode.full=0;
    sFlags=0;
    return;
   }
 rawCode.full=code;
 //printf("0x%04X\r\n",code);
 sFlags=GetShiftState();
}

int TGKeyUNIX::KbHit(void)
{
 int c=NCursesGetCh();
 if (c!=ERR) NCursesUnGetCh(c);
 return c!=ERR;
}

void TGKeyUNIX::Clear(void)
{
 tcflush(IN_FD,TCIFLUSH);
}

void TGKeyUNIX::FillTEvent(TEvent &e)
{
 GKey();
 if (Abstract==kbMouse)
   { // Mouse events are traslated to keyboard sequences:
    int event=NCursesGetCh();
    int x=NCursesGetCh()-0x21; // They are 0x20+ and the corner is 1,1
    int y=NCursesGetCh()-0x21;
    switch (event)
      {
       case MouseB1Down:
            MouseButtons|=mbLeftButton;
            break;
       case MouseB2Down:
            MouseButtons|=mbMiddleButton;
            break;
       case MouseB3Down:
            MouseButtons|=mbRightButton;
            break;
       case MouseUp: // fuzzy, if both are pressed ...
            if (MouseButtons & mbLeftButton)
               MouseButtons&= ~mbLeftButton;
            else
               MouseButtons&= ~mbRightButton;
            break;
      }
    THWMouse::forceEvent(x,y,MouseButtons);
    e.what=evMouseUp; // Acts like a "key"
    dbprintf("TGKey::fillTEvent: Reporting mouse instead of key (%d,%d:%d)\r\n",x,y,event);
   }
 else
 #ifdef HAVE_KEYSYMS
 if (Abstract==kbEterm)
   { // X keysym reported as key sequence yuupi!
    int key,c;
    unsigned state=0,keysym=0;
    c=0; // To avoid hanging
    do
      {
       key=NCursesGetCh();
       if (key!=';')
         {
          state<<=4;
          state+=key>='A' ? key+10-'A' : key-'0';
         }
       c++;
      }
    while (key!=';' && c<3);
    c=0;
    do
      {
       key=NCursesGetCh();
       if (key!='~')
         {
          keysym<<=4;
          keysym+=key>='A' ? key+10-'A' : key-'0';
         }
       c++;
      }
    while (key!='~' && c<3);
    ushort code=kbX11Keys[keysym & 0xFF];
    dbprintf("TGKey::fillTEvent: Reporting Eterm key (%X,%X code: %X)\r\n",state,keysym,code);
    if (code)
      {
       sFlags=0;
       // How can I do it better?
       if (state & 1)
          sFlags|=kbShiftCode;
       if (state & 4)
          sFlags|=kbCtrlCode;
       if (state & 8)
          sFlags|=kbAltLCode;
       Abstract=code | sFlags;
       e.keyDown.charScan.charCode=code==kbTab ? '\t' : 0;
       e.keyDown.charScan.scanCode=0;
       e.keyDown.raw_scanCode=0;
       e.keyDown.keyCode=Abstract;
       e.keyDown.shiftState=sFlags;
       e.what=evKeyDown;
      }
    else
       e.what=evNothing;
   }
 else
 #endif
   {
    e.keyDown.charScan.charCode=sFlags & kblAltL ? 0 : ascii;
    e.keyDown.charScan.scanCode=rawCode.b.scan;
    e.keyDown.raw_scanCode=rawCode.b.scan;
    e.keyDown.keyCode=Abstract;
    e.keyDown.shiftState=sFlags;
    e.what=evKeyDown;
    dbprintf("TGKey::fillTEvent: Reporting key (%X/%X)\r\n",Abstract,sFlags);
   }
}

// I don't know if other UNIX flavors have an equivalent IOCTL
unsigned TGKeyUNIX::GetShiftState()
{
 return 0;
}

static struct termios saved_attributes;

void TGKeyUNIX::Resume()
{
 tcgetattr(STDIN_FILENO,&saved_attributes);
}

void TGKeyUNIX::Suspend()
{
 tcsetattr(STDIN_FILENO,TCSANOW,&saved_attributes);
}

void TGKeyUNIX::Init()
{
 TGKey::Suspend      =TGKeyUNIX::Suspend;
 TGKey::Resume       =TGKeyUNIX::Resume;
 TGKey::kbhit        =KbHit;
 TGKey::clear        =Clear;
 TGKey::gkey         =GKey;
 TGKey::getShiftState=GetShiftState;
 TGKey::fillTEvent   =FillTEvent;
 TGKey::SetKbdMapping=TGKeyUNIX::SetKbdMapping;
 TGKey::GetKbdMapping=TGKeyUNIX::GetKbdMapping;
}
#else // TVOS_UNIX && !TVOSf_QNXRtP

#include <tv/unix/key.h>

#endif // else TVOS_UNIX && !TVOSf_QNXRtP

