/* QNX RtP keyboard handler routines source.
   Copyright (c) 2002 by Mike Gorchak
   Covered by the BSD license. */

#include <tv/configtv.h>

#if defined(TVOS_UNIX) && defined(TVOSf_QNXRtP)

#define Uses_stdio
#define Uses_unistd
#define Uses_ctype
#define Uses_stdlib
#define Uses_TEvent
#define Uses_TGKey
#define Uses_TKeys
#define Uses_TKeys_Extended
#define Uses_string
#include <tv.h>
#include <termios.h>
#include <tv/qnxrtp/key.h>

#include <term.h>
#include <ncurses.h>
#include <sys/ioctl.h>
#include <sys/dcmd_chr.h>

#define kbCtrl (kbLeftCtrl | kbRightCtrl)

struct termios TGKeyQNXRtP::saved_attributes;
ushort TGKeyQNXRtP::sFlags;
ushort TGKeyQNXRtP::undecoded;

ushort TGKeyQNXRtP::hightranstable[0x0100]=
       {//  0/8         1/9        2/A         3/B        4/C         5/D         6/E      7/F
          0x0000,     0x0000,     kbDown,     kbUp,      kbLeft,    kbRight,    kbHome,      kbCtH,          // 0x0100
          0x0000,     kbF1,       kbF2,       kbF3,      kbF4,      kbF5,       kbF6,        kbF7,           // 0x0108
          kbF8,       kbF9,       kbF10,      kbF11,     kbF12,     kbShF1,     kbShF2,      kbShF3,         // 0x0110
          kbShF4,     kbShF5,     kbShF6,     kbShF7,    kbShF8,    kbShF9,     kbShF10,     kbShF11,        // 0x0118
          kbShF12,    kbCtF1,     kbCtF2,     kbCtF3,    kbCtF4,    kbCtF5,     kbCtF6,      kbCtF7,         // 0x0120
          kbCtF8,     kbCtF9,     kbCtF10,    kbCtF11,   kbCtF12,   kbAlF1,     kbAlF2,      kbAlF3,         // 0x0128
          kbAlF4,     kbAlF5,     kbAlF6,     kbAlF7,    kbAlF8,    kbAlF9,     kbAlF10,     kbAlF11,        // 0x0130
          kbAlF12,    0x0000,     0x0000,     0x0000,    0x0000,    0x0000,     0x0000,      0x0000,         // 0x0138
          0x0000,     0x0000,     0x0000,     0x0000,    0x0000,    0x0000,     0x0000,      0x0000,         // 0x0140
          kbCtDelete, kbCtInsert, kbDelete,   kbInsert,  0x0000,    kbAlA,      0x0000,      0x0000,         // 0x0148
          kbCtUp,     kbCtDown,   kbPgDn,     kbPgUp,    kbAlB,     kbCtTab,    kbAlD,       0x0000,         // 0x0150
          0x0000,     0x0000,     0x0000,     0x0000,    0x0000,    0x0000,     0x0000,      0x0000,         // 0x0158
          kbShInsert, kbShTab,    kbShDelete, kbMinus,   kbAlC,     kb5,        kbCt5,       0x0000,         // 0x0160 // FIXME: kbMinus - gray minus, kb5 - numeric 5, kbCt5 - numeric Ctrl-5
          kbEnd,      kbCtEnd,    kbAlF,      kbAlH,     kbAlM,     kbAlE,      kbAlI,       0x0000,         // 0x0168
          kbAlO,      kbAlK,      0x0000,     0x0000,    kbAlL,     kbAlG,      kbAlR,       kbAlJ,          // 0x0170
          kbAlP,      kbAlQ,      kbAlN,      kbCtMinus, kbCtPlus,  kbAlS,      kbAlT,       0x0000,         // 0x0178 // FIXME: kbCtMinus and kbCtPlus are gray.
          kbAlV,      kbPlus,     0x0000,     0x0000,    kbAlW,     kbAlX,      kbAlY,       kbCtHome,       // 0x0180 // FIXME: kbPlus is gray.
          0x0000,     kbCtLeft,   0x0000,     0x0000,    kbCtPgDn,  kbAlZ,      kbCtPgUp,    0x0000,         // 0x0188
          kbShUp,     kbShDown,   kbCtRight,  kbShLeft,  kbShRight, kbShCtLeft, kbShCtRight, kbAlSpace       // 0x0190
       };

ushort TGKeyQNXRtP::lowtranstable[0x0100]=
       {//  0/8         1/9        2/A           3/B        4/C            5/D            6/E      7/F
          0x0000,     kbCtA,      kbCtB,        kbCtC,     kbCtD,         kbCtE,         kbCtF,    kbCtG,          // 0x0000
          0x0000,     0x0000,     kbCtJ,        kbCtK,     kbCtL,         kbEnter,       kbCtN,    kbCtO,          // 0x0008
          kbCtP,      kbCtQ,      kbCtR,        kbCtS,     kbCtT,         kbCtU,         kbCtV,    kbCtW,          // 0x0010
          kbCtX,      kbCtY,      kbCtZ,        kbEsc,     kbCtBackSpace, kbCtOpenBrace, 0x0000,   0x0000,         // 0x0018
          kbSpace,    kbAdmid,    kbDobleQuote, kbNumeral, kbDolar,       kbPercent,     kbAmper,  kbOpenPar,      // 0x0020
          kbOpenPar,  kbClosePar, kbAsterisk,   kbPlus,    kbComma,       kbMinus
       };


void TGKeyQNXRtP::Resume()
{
   tcgetattr(fileno(stdin), &saved_attributes);
}

void TGKeyQNXRtP::Suspend()
{
   tcsetattr(fileno(stdin), TCSANOW, &saved_attributes);
}

int TGKeyQNXRtP::KbHit()
{
   int c;

   c=getch();

   if (c != ERR)
   {
      ungetch(c);
   }

   return c != ERR;
}

void TGKeyQNXRtP::Clear()
{
   tcflush(fileno(stdin), TCIFLUSH);
}

ushort TGKeyQNXRtP::GKey()
{
   ushort rawkey;

   rawkey=GetRaw();
   undecoded=rawkey;

   do {
      if (rawkey<0x0100)
      {
         rawkey=lowtranstable[rawkey];
         break;
      }

      if ((rawkey>0x00FF) && (rawkey<0x01FF))
      {
         rawkey=hightranstable[rawkey-0x0100];
         break;
      }
   } while(1);

   
   return rawkey;
}

unsigned TGKeyQNXRtP::GetShiftState()
{
   int shift = 0;
   int QNXShift = 0;
   int DieStatus;

   devctl(fileno(stdin), DCMD_CHR_LINESTATUS, &QNXShift, sizeof(QNXShift), &DieStatus);
   
   if (QNXShift & _LINESTATUS_CON_ALT)
   {
      shift|=(kbRightAlt | kbLeftAlt);
   }

   if (QNXShift & _LINESTATUS_CON_CTRL)
   {
      shift|=(kbRightCtrl | kbLeftCtrl);
   }

   if (QNXShift & _LINESTATUS_CON_SHIFT)
   {
      shift|=(kbRightShift | kbLeftShift);
   }

   return shift;
}

void TGKeyQNXRtP::FillTEvent(TEvent &e)
{
   ushort fullcode;

   fullcode=GKey();

   if ((undecoded>0x001F)&&(undecoded<0x007F))
   {
      e.keyDown.charScan.charCode=(uchar)undecoded;
   }
   else
   {
      e.keyDown.charScan.charCode=0;
   }
   
   e.keyDown.keyCode=fullcode;
   e.keyDown.shiftState=sFlags;
   e.what=evKeyDown;
}

void TGKeyQNXRtP::Init()
{
   TGKey::Suspend      =TGKeyQNXRtP::Suspend;
   TGKey::Resume       =TGKeyQNXRtP::Resume;
   TGKey::kbhit        =TGKeyQNXRtP::KbHit;
   TGKey::clear        =TGKeyQNXRtP::Clear;
   TGKey::gkey         =TGKeyQNXRtP::GKey;
   TGKey::getShiftState=TGKeyQNXRtP::GetShiftState;
   TGKey::fillTEvent   =TGKeyQNXRtP::FillTEvent;
}

ushort TGKeyQNXRtP::GetRaw()
{
   int code;

   if ((code = getch()) == ERR)
   {
      sFlags=0;
      return 0;
   }

   sFlags=GetShiftState();

   // artificial keys.

   do
   {
      if (sFlags==0)
      {
         break;                 // no artificial keys available.
      }

      break;

   } while(1);

   return code;
}

#else
// Here to generate the dependencies in RHIDE
#include <tv/qnxrtp/key.h>
#endif // TVOS_UNIX && TVOSf_QNXRtP
