/* QNX 4 screen routines source.
   Copyright (c) 1998-2003 by Mike Gorchak
   Covered by the BSD license. */

#if defined(TVOSf_QNX4) && !defined(QNX4KEY_HEADER_INCLUDED)
#define QNX4KEY_HEADER_INCLUDED

#define QNX4_KEYTABLE_SIZE 0x200

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
      static void     Init();

   protected:
      static struct termios saved_attributes;
      static ushort keytranslatetable[QNX4_KEYTABLE_SIZE];
};

#endif // QNX4KEY_HEADER_INCLUDED
