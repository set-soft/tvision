/* QNX RtP keyboard handler routines header.
   Copyright (c) 2002-2003 by Mike Gorchak
   Covered by the BSD license. */

#if defined(TVOSf_QNXRtP) && !defined(QNXRTPKEY_HEADER_INCLUDED)
#define QNXRTPKEY_HEADER_INCLUDED

#define TVPH_KEY_SHIFT 0x00000001UL
#define TVPH_KEY_CTRL  0x00000002UL
#define TVPH_KEY_ALT   0x00000004UL

struct _QNXRtPArtKeys
{
   ushort rawkey;
   ushort transkey;
};

typedef struct _QNXRtPArtKeys QNXRtPArtKeys;

class TGKeyQNXRtP: public TGKey
{
   public:
      TGKeyQNXRtP() {};

      static void     Suspend();
      static void     Resume();
      static int      KbHit();
      static void     Clear();
      static ushort   GKey();
      static unsigned GetShiftState();
      static void     FillTEvent(TEvent &e);
      static void     SetKbdMapping(int version);
      static void     Init();

   protected:
      static ushort GetRaw();
      static ushort MakeArtKeys(QNXRtPArtKeys* array, ushort rawkey);

   protected:
      static struct termios saved_attributes;
      static ushort sFlags;
      static ushort undecoded;
      static ushort hightranstable[0x0100];
      static ushort lowtranstable[0x0100];
      static QNXRtPArtKeys shifttranstable[];
      static QNXRtPArtKeys alttranstable[];
      static QNXRtPArtKeys ctrltranstable[];
      static struct dlphoton phcon;
      static bool inpterm;
};

#endif // QNXRTPKEY_HEADER_INCLUDED
