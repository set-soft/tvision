/* WinNT screen routines header.
   Copyright (c) 2002 by Salvador E. Tropea (SET)
   Covered by the GPL license. */
// This headers needs windows header
#if defined(TVOS_Win32) && !defined(WINNTKEY_HEADER_INCLUDED)
#define WINNTKEY_HEADER_INCLUDED
struct TEvent;

// A class to encapsulate the globals, all is static!
class TGKeyWinNT : public TGKey
{
public:
 TGKeyWinNT() {};

 // Function replacements
 //static void     Suspend();
 //static void     Resume();
 static int      KbHit();
 static void     Clear();
 //static ushort   GKey();
 static unsigned GetShiftState();
 static void     FillTEvent(TEvent &e);
 //static void     SetKbdMapping(int version);

 // Setup the pointers to point our members
 static void     Init();

protected:
 // For this driver
 static INPUT_RECORD inpRec;
 static unsigned     shiftState;
 static ushort       vk2kk[];

 friend class THWMouseWinNT;
 friend class TScreenWinNT;
};

#endif // WINNTKEY_HEADER_INCLUDED

