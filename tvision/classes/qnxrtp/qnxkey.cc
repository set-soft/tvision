/* QNX RtP keyboard handler routines source.
   Copyright (c) 2002-2003 by Mike Gorchak
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
#define Uses_string
#include <tv.h>

// I delay the check to generate as much dependencies as possible
#if defined(TVOS_UNIX) && defined(TVOSf_QNXRtP)

/* include main photon header for pterm key modifier support.           */
/* we must include photon headers before all (GUI vs TUI conflict ;)    */
#include <Ph.h>

#include <termios.h>

#include <term.h>
#include <ncurses.h>
#include <sys/ioctl.h>
#include <sys/dcmd_chr.h>

#include <dlfcn.h>

#include <tv/qnxrtp/key.h>

struct dlphoton
{
   void* phhandle;
   struct _Ph_ctrl* phctrl;
   unsigned short ig;
   PhCursorInfo_t qcurbuf;

   /* photon functions */
   struct _Ph_ctrl* (*PhAttach)(char const* name, PhChannelParms_t const* parms);
   int              (*PhDetach)(struct _Ph_ctrl* Ph);
   int              (*PhInputGroup)(PhEvent_t* event);
   int              (*PhQueryCursor)(unsigned short ig, PhCursorInfo_t* buf);
};

struct dlphoton TGKeyQNXRtP::phcon;
bool TGKeyQNXRtP::inpterm;

int openph(struct dlphoton* ph);
int closeph(struct dlphoton* ph);
int detectph(struct dlphoton* ph);
unsigned long getkeymodph(struct dlphoton* ph);

int openph(struct dlphoton* ph)
{
   ph->phctrl=NULL;
   ph->ig=0;

   ph->phhandle=dlopen("/usr/lib/libph.so", RTLD_NOW | RTLD_GLOBAL | RTLD_WORLD);
   
   if (ph->phhandle==NULL)
   {
      return -1;
   }
   
   (void*)(ph->PhAttach)=dlsym(ph->phhandle, "PhAttach");
   if (ph->PhAttach==NULL)
   {
      closeph(ph);
      return -3;
   }

   (void*)ph->PhDetach=dlsym(ph->phhandle, "PhDetach");
   if (ph->PhDetach==NULL)
   {
      closeph(ph);
      return -4;
   }

   (void*)ph->PhInputGroup=dlsym(ph->phhandle, "PhInputGroup");
   if (ph->PhInputGroup==NULL)
   {
      closeph(ph);
      return -5;
   }

   (void*)ph->PhQueryCursor=dlsym(ph->phhandle, "PhQueryCursor");
   if (ph->PhQueryCursor==NULL)
   {
      closeph(ph);
      return -6;
   }
   
   return 0;
}

int closeph(struct dlphoton* ph)
{
   if (ph->phhandle!=NULL)
   {
      if (ph->phctrl!=NULL)
      {
         ph->PhDetach(ph->phctrl);
      }
      dlclose(ph->phhandle);
      ph->phhandle=NULL;
   }

   return 0;
}

int detectph(struct dlphoton* ph)
{
   ph->phctrl=ph->PhAttach(NULL, NULL);

   if(!ph->phctrl)
   {
      return -1;
   }

   ph->ig=ph->PhInputGroup(NULL);

   return 0;
}

unsigned long getkeymodph(struct dlphoton* ph)
{
   int stat;

   stat=ph->PhQueryCursor(ph->ig, &ph->qcurbuf);
   
   if (stat==-1)
   {
      return 0x00000000UL;
   }
   
   return ph->qcurbuf.key_mods;
}

#define kbCtrl (kbLeftCtrl | kbRightCtrl)
#define kbAlt  (kbRightAlt | kbLeftAlt)

struct termios TGKeyQNXRtP::saved_attributes;
ushort TGKeyQNXRtP::sFlags;
ushort TGKeyQNXRtP::undecoded;

ushort TGKeyQNXRtP::hightranstable[0x0100]=
{//  0/8         1/9           2/A           3/B        4/C         5/D         6/E      7/F
   0x0000,     0x0000,        kbDown,       kbUp,      kbLeft,    kbRight,    kbHome,      kbCtH,          // 0x0100
   0x0000,     kbF1,          kbF2,         kbF3,      kbF4,      kbF5,       kbF6,        kbF7,           // 0x0108
   kbF8,       kbF9,          kbF10,        kbF11,     kbF12,     kbShF1,     kbShF2,      kbShF3,         // 0x0110
   kbShF4,     kbShF5,        kbShF6,       kbShF7,    kbShF8,    kbShF9,     kbShF10,     kbShF11,        // 0x0118
   kbShF12,    kbCtF1,        kbCtF2,       kbCtF3,    kbCtF4,    kbCtF5,     kbCtF6,      kbCtF7,         // 0x0120
   kbCtF8,     kbCtF9,        kbCtF10,      kbCtF11,   kbCtF12,   kbAlF1,     kbAlF2,      kbAlF3,         // 0x0128
   kbAlF4,     kbAlF5,        kbAlF6,       kbAlF7,    kbAlF8,    kbAlF9,     kbAlF10,     kbAlF11,        // 0x0130
   kbAlF12,    0x0000,        0x0000,       0x0000,    0x0000,    0x0000,     0x0000,      0x0000,         // 0x0138
   0x0000,     0x0000,        0x0000,       0x0000,    0x0000,    0x0000,     0x0000,      0x0000,         // 0x0140
   kbCtDelete, kbCtInsert,    kbDelete,     kbInsert,  0x0000,    kbAlA,      0x0000,      0x0000,         // 0x0148
   kbCtUp,     kbCtDown,      kbPgDn,       kbPgUp,    kbAlB,     kbCtTab,    kbAlD,       0x0000,         // 0x0150
   0x0000,     0x0000,        0x0000,       0x0000,    0x0000,    0x0000,     0x0000,      0x0000,         // 0x0158
   kbShInsert, kbShTab,       kbShDelete,   kbMinus,   kbAlC,     kb5,        kbCt5,       0x0000,         // 0x0160 // FIXME: kbMinus - gray minus, kb5 - numeric 5, kbCt5 - numeric Ctrl-5
   kbEnd,      kbCtEnd,       kbAlF,        kbAlH,     kbAlM,     kbAlE,      kbAlI,       0x0000,         // 0x0168
   kbAlO,      kbAlK,         0x0000,       0x0000,    kbAlL,     kbAlG,      kbAlR,       kbAlJ,          // 0x0170
   kbAlP,      kbAlQ,         kbAlN,        kbCtMinus, kbCtPlus,  kbAlS,      kbAlT,       0x0000,         // 0x0178 // FIXME: kbCtMinus and kbCtPlus are gray.
   kbAlV,      kbPlus,        0x0000,       0x0000,    kbAlW,     kbAlX,      kbAlY,       kbCtHome,       // 0x0180 // FIXME: kbPlus is gray.
   0x0000,     kbCtLeft,      0x0000,       0x0000,    kbCtPgDn,  kbAlZ,      kbCtPgUp,    0x0000,         // 0x0188
   kbShUp,     kbShDown,      kbCtRight,    kbShLeft,  kbShRight, kbShCtLeft, kbShCtRight, kbAlSpace,      // 0x0190
   kbAlU,      kbCtBackSpace, kbCtEnter,    kbAlEnter, kbShEnter, kbShEnd,    kbShHome,    kbShPgUp,       // 0x0198
   kbShPgDn,   kbShBackSpace, kbCt0,        kbCt1,     kbCt2,     kbCt3,      kbCt4,       kbCt5,          // 0x01A0
   kbCt6,      kbCt7,         kbCt8,        kbCt9,     kbCtSpace, kbShCtTab,  kbShCtHome,  kbShCtEnd,      // 0x01A8
   kbShCtPgUp, kbShCtPgDn,    kbShCtInsert, kbShCt0,   kbShCt1,   kbShCt2,    kbShCt3,     kbShCt4,        // 0x01B0
   kbShCt5,    kbShCt6,       kbShCt7,      kbShCt8,   kbShCt9,   0x0000,     0x0000,      0x0000,         // 0x01B8
   kbShCtA,    kbShCtB,       kbShCtC,      kbShCtD,   kbShCtE,   kbShCtF,    kbShCtG,     kbShCtH,        // 0x01C0
   kbShCtI,    kbShCtJ,       kbShCtK,      kbShCtL,   kbShCtM,   kbShCtN,    kbShCtO,     kbShCtP,        // 0x01C8
   kbShCtQ,    kbShCtR,       kbShCtS,      kbShCtT,   kbShCtU,   kbShCtV,    kbShCtW,     kbShCtX,        // 0x01D0
   kbShCtY,    kbShCtZ,       0x0000,       0x0000,    0x0000,    0x0000,     0x0000,      0x0000,         // 0x01D8
   kbAl0,      kbAl1,         kbAl2,        kbAl3,     kbAl4,     kbAl5,      kbAl6,       kbAl7,          // 0x01E0
   kbAl8,      kbAl9,         0x0000,       0x0000,    0x0000,    0x0000,     0x0000,      kbCtEsc,        // 0x01E8
   0x0000,     0x0000,        0x0000,       0x0000,    0x0000,    0x0000,     0x0000,      0x0000,         // 0x01F0
   0x0000,     0x0000,        0x0000,       0x0000,    0x0000,    0x0000,     0x0000,      0x0000          // 0x01F8
};

ushort TGKeyQNXRtP::lowtranstable[0x0100]=
{//  0/8         1/9        2/A           3/B          4/C            5/D            6/E            7/F
   0x0000,     kbCtA,      kbCtB,        kbCtC,       kbCtD,         kbCtE,         kbCtF,         kbCtG,       // 0x0000
   0x0000,     kbTab,      kbCtJ,        kbCtK,       kbCtL,         kbEnter,       kbCtN,         kbCtO,       // 0x0008
   kbCtP,      kbCtQ,      kbCtR,        kbCtS,       kbCtT,         kbCtU,         kbCtV,         kbCtW,       // 0x0010
   kbCtX,      kbCtY,      kbCtZ,        kbEsc,       kbCtBackSpace, kbCtOpenBrace, 0x0000,        0x0000,      // 0x0018
   kbSpace,    kbAdmid,    kbDobleQuote, kbNumeral,   kbDolar,       kbPercent,     kbAmper,       kbQuote,     // 0x0020
   kbOpenPar,  kbClosePar, kbAsterisk,   kbPlus,      kbComma,       kbMinus,       kbStop,        kbSlash,     // 0x0028
   kb0,        kb1,        kb2,          kb3,         kb4,           kb5,           kb6,           kb7,         // 0x0030
   kb8,        kb9,        kbDoubleDot,  kbColon,     kbLessThan,    kbEqual,       kbGreaterThan, kbQuestion,  // 0x0038
   kbA_Roba,   kbShA,      kbShB,        kbShC,       kbShD,         kbShE,         kbShF,         kbShG,       // 0x0040
   kbShH,      kbShI,      kbShJ,        kbShK,       kbShL,         kbShM,         kbShN,         kbShO,       // 0x0048
   kbShP,      kbShQ,      kbShR,        kbShS,       kbShT,         kbShU,         kbShV,         kbShW,       // 0x0050
   kbShX,      kbShY,      kbShZ,        kbOpenBrace, kbBackSlash,   kbCloseBrace,  kbCaret,       kbUnderLine, // 0x0058
   kbQuote,    kbA,        kbB,          kbC,         kbD,           kbE,           kbF,           kbG,         // 0x0060
   kbH,        kbI,        kbJ,          kbK,         kbL,           kbM,           kbN,           kbO,         // 0x0068
   kbP,        kbQ,        kbR,          kbS,         kbT,           kbU,           kbV,           kbW,         // 0x0070
   kbX,        kbY,        kbZ,          kbOpenCurly, kbOr,          kbCloseCurly,  kbTilde,       kbBackSpace, // 0x0078
   /* add russian or other keys here */
   0x0000,     0x0000,     0x0000,       0x0000,      0x0000,        0x0000,        0x0000,        0x0000,      // 0x0080
   0x0000,     0x0000,     0x0000,       0x0000,      0x0000,        0x0000,        0x0000,        0x0000,      // 0x0088
   0x0000,     0x0000,     0x0000,       0x0000,      0x0000,        0x0000,        0x0000,        0x0000,      // 0x0090
   0x0000,     0x0000,     0x0000,       0x0000,      0x0000,        0x0000,        0x0000,        0x0000,      // 0x0098
   0x0000,     0x0000,     0x0000,       0x0000,      0x0000,        0x0000,        0x0000,        0x0000,      // 0x00A0
   0x0000,     0x0000,     0x0000,       0x0000,      0x0000,        0x0000,        0x0000,        0x0000,      // 0x00A8
   0x0000,     0x0000,     0x0000,       0x0000,      0x0000,        0x0000,        0x0000,        0x0000,      // 0x00B0
   0x0000,     0x0000,     0x0000,       0x0000,      0x0000,        0x0000,        0x0000,        0x0000,      // 0x00B8
   0x0000,     0x0000,     0x0000,       0x0000,      0x0000,        0x0000,        0x0000,        0x0000,      // 0x00C0
   0x0000,     0x0000,     0x0000,       0x0000,      0x0000,        0x0000,        0x0000,        0x0000,      // 0x00C8
   0x0000,     0x0000,     0x0000,       0x0000,      0x0000,        0x0000,        0x0000,        0x0000,      // 0x00D0
   0x0000,     0x0000,     0x0000,       0x0000,      0x0000,        0x0000,        0x0000,        0x0000,      // 0x00D8
   0x0000,     0x0000,     0x0000,       0x0000,      0x0000,        0x0000,        0x0000,        0x0000,      // 0x00E0
   0x0000,     0x0000,     0x0000,       0x0000,      0x0000,        0x0000,        0x0000,        0x0000,      // 0x00E8
   0x0000,     0x0000,     0x0000,       0x0000,      0x0000,        0x0000,        0x0000,        0x0000,      // 0x00F0
   0x0000,     0x0000,     0x0000,       0x0000,      0x0000,        0x0000,        0x0000,        0x0000       // 0x00F8
};

QNXRtPArtKeys TGKeyQNXRtP::shifttranstable[]=
{
   {kbLeft,      kbShLeft},
   {kbRight,     kbShRight},
   {kbUp,        kbShUp},
   {kbDown,      kbShDown},
   {kbInsert,    kbShInsert},
   {kbDelete,    kbShDelete},
   {kbHome,      kbShHome},
   {kbEnd,       kbShEnd},
   {kbPgUp,      kbShPgUp},
   {kbPgDn,      kbShPgDn},
   {kbEnter,     kbShEnter},
   {kbBackSpace, kbShBackSpace},
   {kb0,         kbShInsert}, // numeric keypad insert
   {kbStop,      kbShDelete}, // numeric keypad delete
   {0xFFFF,      0xFFFF}      // end of shift modifyier artificial keys list
};

QNXRtPArtKeys TGKeyQNXRtP::alttranstable[]=
{
   {kbEnter,  kbAlEnter},
   {kbSpace,  kbAlSpace},
   {0xFFFF,   0xFFFF} // end of alt modifyier artificial keys list
};

QNXRtPArtKeys TGKeyQNXRtP::ctrltranstable[]=
{
   {kbEnter,     kbCtEnter},
   {kbTab,       kbCtI},
   {kbBackSpace, kbCtBackSpace},
   {0xFFFF,      0xFFFF} // end of ctrl modifyier artificial keys list
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

ushort TGKeyQNXRtP::MakeArtKeys(QNXRtPArtKeys* array, ushort rawkey)
{
   int eachkey=0;
   ushort newkey=rawkey;

   do {

      if ((array[eachkey].rawkey==0xFFFF) &&
          (array[eachkey].transkey==0xFFFF))
      {
         break;
      }

      if (rawkey==array[eachkey].rawkey)
      {
         newkey=array[eachkey].transkey;
         break;
      }

      eachkey++;

   } while(1);
   
   return newkey;
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

   // artificial keys.

   if (sFlags==0)
   {
      return rawkey;  // no artificial keys available.
   }

   if (sFlags==kbShift) // processing only shift key (nor Alt, nor Ctrl)
   {
      rawkey=MakeArtKeys(shifttranstable, rawkey);
   }
   if (sFlags==kbAlt) // processing only alt key (nor Shift, nor Ctrl)
   {
      rawkey=MakeArtKeys(alttranstable, rawkey);
   }
   if (sFlags==kbCtrl) // processing only ctrl key (nor Shift, nor Alt)
   {
      rawkey=MakeArtKeys(ctrltranstable, rawkey);
   }

   return rawkey;
}

unsigned TGKeyQNXRtP::GetShiftState()
{
   int shift = 0;
   int QNXShift = 0;
   int QNXShiftEx = 0;
   int DieStatus;

   if (!inpterm)
   {
      devctl(fileno(stdin), DCMD_CHR_LINESTATUS, &QNXShift, sizeof(QNXShift), &DieStatus);
   
      QNXShiftEx=QNXShift & 0xFFFFFF00UL;
      QNXShift&=0x7F;

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
      else
      {
         if (QNXShiftEx & 0x00000800UL) // grey keys shift modifyier
         {
            shift|=(kbRightShift | kbLeftShift);
         }
      }
   }
   else
   {
      QNXShift=getkeymodph(&phcon);

      if (QNXShift & TVPH_KEY_ALT)
      {
         shift|=(kbRightAlt | kbLeftAlt);
      }

      if (QNXShift & TVPH_KEY_CTRL)
      {
         shift|=(kbRightCtrl | kbLeftCtrl);
      }

      if (QNXShift & TVPH_KEY_SHIFT)
      {
         shift|=(kbRightShift | kbLeftShift);
      }
   }

   return shift;
}

void TGKeyQNXRtP::FillTEvent(TEvent &e)
{
   ushort fullcode;

   fullcode=GKey();

   if ((undecoded>0x001F)&&(undecoded<0x0100))
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

   inpterm=false;
   if (openph(&phcon)==0)
   {
      if (detectph(&phcon)==0)
      {
         inpterm=true;
      }
   }

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
   
   return code;
}

#else

   // Here to generate the dependencies in RHIDE
   #include <tv/qnxrtp/key.h>

#endif // TVOS_UNIX && TVOSf_QNXRtP
