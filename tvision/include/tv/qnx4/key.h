/* QNX 4 keyboard handler routines header.
   */

#if defined(TVOSf_QNX4) && !defined(QNX4KEY_HEADER_INCLUDED)
#define QNX4KEY_HEADER_INCLUDED

class TGKeyQNX4: public TGKey
{
   public:
      TGKeyQNX4() {};

      static void     Suspend();
      static void     Resume();
      static int      KbHit();
      static void     Clear();
      static ushort   GKey();
      static unsigned GetShiftState();
      static void     FillTEvent(TEvent &e);
      static void     SetKbdMapping(int version);
      static void     Init();
};

#endif // QNX4KEY_HEADER_INCLUDED
