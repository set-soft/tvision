/* QNX 4 screen routines source.
   Copyright (c) 1998-2003 by Mike Gorchak
   Covered by the BSD license. */

#include <tv/configtv.h>

#define Uses_stdio
#define Uses_unistd
#define Uses_ctype
#define Uses_stdlib
#define Uses_TEvent
#define Uses_TGKey
#define Uses_TKeys
#define Uses_TKeys_Extended
#define Uses_TDisplay
#define Uses_TScreen
#define Uses_string
#include <tv.h>

// I delay the check to generate as much dependencies as possible
#if defined(TVOS_UNIX) && defined(TVOSf_QNX4)

#include <termios.h>

#include <tv/qnx4/key.h>
#include <tv/qnx4/screen.h>

extern "C"
{
   #include <sys/term.h>
   #include <sys/qnxterm.h>
   #include <sys/qioctl.h>
   #include <conio.h>
}

struct termios TGKeyQNX4::saved_attributes;

ushort TGKeyQNX4::keytranslatetable[QNX4_KEYTABLE_SIZE]=
{
   0x0000,     kbCtA,      kbCtB,        kbCtC,         kbCtD,       kbCtE,         kbCtF,         kbCtG,       /* 0x000-0x007 */ // Low translation table.
   kbCtH,      kbTab,      kbCtJ,        kbCtK,         kbCtL,       kbEnter,       kbCtN,         kbCtO,       /* 0x008-0x00F */
   kbCtP,      kbCtQ,      kbCtR,        kbCtS,         kbCtT,       kbCtU,         kbCtV,         kbCtW,       /* 0x010-0x017 */
   kbCtX,      kbCtY,      kbCtZ,        kbEsc,         0x0000,      kbCtOpenBrace, kbCt6,         kbCtMinus,   /* 0x018-0x01F */
   kbSpace,    kbAdmid,    kbDobleQuote, kbNumeral,     kbDolar,     kbPercent,     kbAmper,       kbQuote,     /* 0x020-0x027 */
   kbOpenPar,  kbClosePar, kbAsterisk,   kbPlus,        kbComma,     kbMinus,       kbStop,        kbSlash,     /* 0x028-0x02F */
   kb0,        kb1,        kb2,          kb3,           kb4,         kb5,           kb6,           kb7,         /* 0x030-0x037 */
   kb8,        kb9,        kbDoubleDot,  kbColon,       kbLessThan,  kbEqual,       kbGreaterThan, kbQuestion,  /* 0x038-0x03F */
   kbA_Roba,   kbShA,      kbShB,        kbShC,         kbShD,       kbShE,         kbShF,         kbShG,       /* 0x040-0x047 */
   kbShH,      kbShI,      kbShJ,        kbShK,         kbShL,       kbShM,         kbShN,         kbShO,       /* 0x048-0x04F */
   kbShP,      kbShQ,      kbShR,        kbShS,         kbShT,       kbShU,         kbShV,         kbShW,       /* 0x050-0x057 */
   kbShX,      kbShY,      kbShZ,        kbOpenBrace,   kbBackSlash, kbCloseBrace,  kbCaret,       kbUnderLine, /* 0x058-0x05F */
   kbQuote,    kbA,        kbB,          kbC,           kbD,         kbE,           kbF,           kbG,         /* 0x060-0x067 */
   kbH,        kbI,        kbJ,          kbK,           kbL,         kbM,           kbN,           kbO,         /* 0x068-0x06F */
   kbP,        kbQ,        kbR,          kbS,           kbT,         kbU,           kbV,           kbW,         /* 0x070-0x077 */
   kbX,        kbY,        kbZ,          kbOpenCurly,   kbOr,        kbCloseCurly,  kbTilde,       kbBackSpace, /* 0x077-0x07F */
   0x0000,     0x0000,     0x0000,       0x0000,        0x0000,      0x0000,        0x0000,        0x0000,      /* 0x080-0x087 */ // International input characters starting here.
   0x0000,     0x0000,     0x0000,       0x0000,        0x0000,      0x0000,        0x0000,        0x0000,      /* 0x088-0x08F */
   0x0000,     0x0000,     0x0000,       0x0000,        0x0000,      0x0000,        0x0000,        0x0000,      /* 0x090-0x097 */
   0x0000,     0x0000,     0x0000,       0x0000,        0x0000,      0x0000,        0x0000,        0x0000,      /* 0x098-0x09F */
   0x0000,     0x0000,     0x0000,       0x0000,        0x0000,      0x0000,        0x0000,        0x0000,      /* 0x0A0-0x0A7 */
   0x0000,     0x0000,     0x0000,       0x0000,        0x0000,      0x0000,        0x0000,        0x0000,      /* 0x0A8-0x0AF */
   0x0000,     0x0000,     0x0000,       0x0000,        0x0000,      0x0000,        0x0000,        0x0000,      /* 0x0B0-0x0B7 */
   0x0000,     0x0000,     0x0000,       0x0000,        0x0000,      0x0000,        0x0000,        0x0000,      /* 0x0B8-0x0BF */
   0x0000,     0x0000,     0x0000,       0x0000,        0x0000,      0x0000,        0x0000,        0x0000,      /* 0x0C0-0x0C7 */
   0x0000,     0x0000,     0x0000,       0x0000,        0x0000,      0x0000,        0x0000,        0x0000,      /* 0x0C8-0x0CF */
   0x0000,     0x0000,     0x0000,       0x0000,        0x0000,      0x0000,        0x0000,        0x0000,      /* 0x0D0-0x0D7 */
   0x0000,     0x0000,     0x0000,       0x0000,        0x0000,      0x0000,        0x0000,        0x0000,      /* 0x0D8-0x0DF */
   0x0000,     0x0000,     0x0000,       0x0000,        0x0000,      0x0000,        0x0000,        0x0000,      /* 0x0E0-0x0E7 */
   0x0000,     0x0000,     0x0000,       0x0000,        0x0000,      0x0000,        0x0000,        0x0000,      /* 0x0E8-0x0EF */
   0x0000,     0x0000,     0x0000,       0x0000,        0x0000,      0x0000,        0x0000,        0x0000,      /* 0x0F0-0x0F7 */
   0x0000,     0x0000,     0x0000,       0x0000,        0x0000,      0x0000,        0x0000,        0x0000,      /* 0x0F8-0x0FF */
   0x0000,     0x0000,     kbDown,       kbUp,          kbLeft,      kbRight,       kbHome,        0x0000,      /* 0x100-0x107 */ // High translation table.
   0x0000,     kbF1,       kbF2,         kbF3,          kbF4,        kbF5,          kbF6,          kbF7,        /* 0x108-0x10F */
   kbF8,       kbF9,       kbF10,        kbF11,         kbF12,       kbShF1,        kbShF2,        kbShF3,      /* 0x110-0x117 */
   kbShF4,     kbShF5,     kbShF6,       kbShF7,        kbShF8,      kbShF9,        kbShF10,       kbShF11,     /* 0x118-0x11F */
   kbShF12,    kbCtF1,     kbCtF2,       kbCtF3,        kbCtF4,      kbCtF5,        kbCtF6,        kbCtF7,      /* 0x120-0x127 */
   kbCtF8,     kbCtF9,     kbCtF10,      kbCtF11,       kbCtF12,     kbAlF1,        kbAlF2,        kbAlF3,      /* 0x128-0x12F */
   kbAlF4,     kbAlF5,     kbAlF6,       kbAlF7,        kbAlF8,      kbAlF9,        kbAlF10,       kbAlF11,     /* 0x130-0x137 */
   kbAlF12,    0x0000,     0x0000,       0x0000,        0x0000,      0x0000,        0x0000,        0x0000,      /* 0x138-0x13F */
   0x0000,     0x0000,     0x0000,       0x0000,        0x0000,      0x0000,        0x0000,        0x0000,      /* 0x140-0x147 */
   kbCtDelete, kbCtInsert, kbDelete,     kbInsert,      kbAlInsert,  kbAlA,         kbAlDelete,    kbAlEnd,     /* 0x148-0x14F */
   0x0000,     0x0000,     kbPgDn,       kbPgUp,        kbAlB,       0x0000,        kbAlD,         kbCtEnter,   /* 0x150-0x157 */
   0x0000,     0x0000,     0x0000,       0x0000,        0x0000,      0x0000,        0x0000,        0x0000,      /* 0x158-0x15F */
   0x0000,     kbShTab,    0x0000,       kbMinus,       kbAlC,       0x0000,        0x0000,        0x0000,      /* 0x160-0x167 */ // FIXME: kbMinus is a gray minus.
   kbEnd,      kbCtEnd,    kbAlF,        kbAlH,         kbAlM,       kbAlE,         kbAlI,         0x0000,      /* 0x168-0x16F */
   kbAlO,      kbAlK,      0x0000,       kbCtBackSpace, kbAlL,       kbAlG,         kbAlR,         kbAlJ,       /* 0x170-0x177 */
   kbAlP,      kbAlQ,      kbAlN,        0x0000,        0x0000,      kbAlS,         kbAlT,         0x0000,      /* 0x178-0x17F */
   kbAlV,      kbPlus,     0x0000,       0x0000,        kbAlW,       kbAlX,         kbAlY,         kbCtHome,    /* 0x180-0x187 */ // FIXME: kbPlus - plus is gray.
   kbShEnter,  kbCtLeft,   0x0000,       0x0000,        kbCtPgDn,    kbAlZ,         kbCtPgUp,      0x0000,      /* 0x187-0x18F */ // FIXME: Code 0x18F - is Ctrl-PrintScreen and Ctrl-Asterisk.
   0x0000,     kbAlEnter,  kbCtRight,    kbShBackSpace, 0x0000,      kbAlMinus,     kbAlBackSpace, 0x0000,      /* 0x190-0x197 */ // FIXME: kbAlMinus is a Alt + gray Minus. Code 0x197 - is Alt-PrintScreen.
   kbAlU,      0x0000,     0x0000,       0x0000,        0x0000,      0x0000,        0x0000,        0x0000,      /* 0x198-0x19F */
   0x0000,     0x0000,     0x0000,       0x0000,        0x0000,      0x0000,        0x0000,        0x0000,      /* 0x1A0-0x1A7 */
   0x0000,     0x0000,     0x0000,       0x0000,        0x0000,      0x0000,        0x0000,        0x0000,      /* 0x1A8-0x1AF */
   0x0000,     0x0000,     0x0000,       0x0000,        0x0000,      0x0000,        0x0000,        0x0000,      /* 0x1B0-0x1B7 */
   0x0000,     0x0000,     0x0000,       0x0000,        0x0000,      0x0000,        0x0000,        0x0000,      /* 0x1B8-0x1BF */
   0x0000,     0x0000,     0x0000,       0x0000,        0x0000,      0x0000,        0x0000,        0x0000,      /* 0x1C0-0x1C7 */
   0x0000,     0x0000,     0x0000,       0x0000,        0x0000,      0x0000,        0x0000,        0x0000,      /* 0x1C8-0x1CF */
   0x0000,     0x0000,     0x0000,       0x0000,        0x0000,      0x0000,        0x0000,        0x0000,      /* 0x1D0-0x1D7 */
   0x0000,     0x0000,     0x0000,       0x0000,        0x0000,      0x0000,        0x0000,        0x0000,      /* 0x1D8-0x1DF */
   0x0000,     0x0000,     0x0000,       0x0000,        0x0000,      0x0000,        0x0000,        0x0000,      /* 0x1E0-0x1E7 */
   0x0000,     0x0000,     0x0000,       0x0000,        0x0000,      0x0000,        0x0000,        0x0000,      /* 0x1E8-0x1EF */
   0x0000,     0x0000,     0x0000,       0x0000,        0x0000,      0x0000,        0x0000,        0x0000,      /* 0x1F0-0x1F7 */
   0x0000,     0x0000,     0x0000,       0x0000,        0x0000,      0x0000,        0x0000,        0x0000       /* 0x1F8-0x1FF */
};

void TGKeyQNX4::Init()
{
   TGKey::kbhit=TGKeyQNX4::KbHit;
   TGKey::clear=TGKeyQNX4::Clear;
   TGKey::gkey=TGKeyQNX4::GKey;
   TGKey::Resume=TGKeyQNX4::Resume;
   TGKey::Suspend=TGKeyQNX4::Suspend;
   TGKey::fillTEvent=TGKeyQNX4::FillTEvent;
   TGKey::getShiftState=TGKeyQNX4::GetShiftState;
}

int TGKeyQNX4::KbHit()
{
   int key;
   int tempkey;

   key=::kbhit();

   if (!key)
   {
      __nodelay(0, 1);
      tempkey=term_key();
      if (tempkey!=-1)
      {
         key=1;
         if (tempkey==K_RESIZE)
         {
            TDisplayQNX4::ConsoleResizing=1;
            term_relearn_size();
         }
         __ungetch(tempkey);
      }
      __nodelay(0, 0);
   }

   return key;
}

void TGKeyQNX4::Resume()
{
   tcgetattr(fileno(stdin), &saved_attributes);
}

void TGKeyQNX4::Suspend()
{
   tcsetattr(fileno(stdin), TCSANOW, &saved_attributes);
}

void TGKeyQNX4::Clear()
{
   tcflush(fileno(stdin), TCIFLUSH);
}

ushort TGKeyQNX4::GKey()
{
   return term_key();
}

void TGKeyQNX4::FillTEvent(TEvent &e)
{
   ushort fullcode;
   unsigned shiftstate;

   shiftstate=GetShiftState();
   fullcode=GKey();

   if (fullcode<QNX4_KEYTABLE_SIZE)
   {
      e.keyDown.keyCode=keytranslatetable[fullcode];
   }
   else
   {
      e.keyDown.keyCode=0x0000;
   }

   if ((fullcode>0x001F) && (fullcode<0x0100))
   {
      e.keyDown.charScan.charCode=fullcode;
   }
   else
   {
      e.keyDown.charScan.charCode=0;
   }

   e.keyDown.shiftState=shiftstate;

   if ((e.keyDown.keyCode==kbEnter) && (shiftstate & (kbLeftCtrl | kbRightCtrl)))
   {
      e.keyDown.keyCode=kbCtM;
   }

   // qnxterm keyboard hack.
   if (e.keyDown.keyCode==kbCtJ)
   {
      if (shiftstate & (kbLeftCtrl | kbRightCtrl))
      {
         e.keyDown.keyCode=kbCtJ;
      }
      else
      {
         e.keyDown.keyCode=kbEnter;
      }
   }

   if ((e.keyDown.keyCode==kbTab) && (shiftstate & (kbLeftCtrl | kbRightCtrl)))
   {
      e.keyDown.keyCode=kbCtI;
   }
   
   if (shiftstate & (kbLeftShift | kbRightShift))
   {
      switch(e.keyDown.keyCode)
      {
         case kbLeft:
              e.keyDown.keyCode=kbShLeft;
              break;
         case kbRight:
              e.keyDown.keyCode=kbShRight;
              break;
         case kbUp:
              e.keyDown.keyCode=kbShUp;
              break;
         case kbDown:
              e.keyDown.keyCode=kbShDown;
              break;
         case kbHome:
              e.keyDown.keyCode=kbShHome;
              break;
         case kbEnd:
              e.keyDown.keyCode=kbShEnd;
              break;
         case kbPgDn:
              e.keyDown.keyCode=kbShPgDn;
              break;
         case kbPgUp:
              e.keyDown.keyCode=kbShPgUp;
              break;
         case kbInsert:
              e.keyDown.keyCode=kbShInsert;
              break;
         case kbDelete:
              e.keyDown.keyCode=kbShDelete;
              break;
      }
   }

   if ((shiftstate & (kbLeftShift | kbRightShift)) && (e.keyDown.keyCode==kb0))
   {
      e.keyDown.charScan.charCode=0;
      e.keyDown.keyCode=kbShInsert; // on numeric keypad.
   }

   if ((shiftstate & (kbLeftShift | kbRightShift)) && (e.keyDown.keyCode==kbStop))
   {
      e.keyDown.charScan.charCode=0;
      e.keyDown.keyCode=kbShDelete; // on numeric keypad.
   }

   e.what=evKeyDown;
}

unsigned TGKeyQNX4::GetShiftState()
{
  unsigned ModState=0x00000000UL;

  char SendBuffer[0x08]={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  char RecvBuffer[0x04]={0x00, 0x00, 0x00, 0x00};

  qnx_ioctl(fileno(stdin), QCTL_DEV_CTL, SendBuffer, 0x08, RecvBuffer, 0x04);
  
  if (RecvBuffer[0x02]&0x01) ModState|=kbLeftShift | kbRightShift;
  if (RecvBuffer[0x02]&0x02) ModState|=kbLeftCtrl | kbRightCtrl;
  if (RecvBuffer[0x02]&0x04) ModState|=kbLeftAlt | kbRightAlt;

  return ModState;
}

#else

   // Here to generate the dependencies in RHIDE
   #include <tv/qnx4/key.h>

#endif // TVOS_UNIX && TVOSf_QNX4
