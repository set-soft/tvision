/* QNX RtP keyboard handler routines header.
   Copyright (c) 2002 by Mike Gorchak
   Covered by the BSD license. */

#if defined(TVOSf_QNXRtP) && !defined(QNXRTPKEY_HEADER_INCLUDED)
#define QNXRTPKEY_HEADER_INCLUDED

// A class to encapsulate the globals, all is static!
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
};

#endif // QNXRTPKEY_HEADER_INCLUDED
