/* QNX RtP keyboard handler routines header.
   Copyright (c) 2002 by Mike Gorchak
   Covered by the BSD license. */

#if defined(TVOSf_QNXRtP) && !defined(QNXRTPKEY_HEADER_INCLUDED)
#define QNXRTPKEY_HEADER_INCLUDED

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

protected:
      static struct termios saved_attributes;
      static ushort sFlags;
      static ushort undecoded;
      static ushort hightranstable[0x0100];
      static ushort lowtranstable[0x0100];
      static QNXRtPArtKeys shifttranstable[];
      static QNXRtPArtKeys alttranstable[];
      static QNXRtPArtKeys ctrltranstable[];
};

#endif // QNXRTPKEY_HEADER_INCLUDED
