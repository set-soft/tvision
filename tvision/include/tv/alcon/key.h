/*****************************************************************************

  Copyright (c) 2003-2004 by Grzegorz Adam Hankiewicz
  Copyright (c) 2002-2009 by Salvador E. Tropea (SET) <set@ieee.org>
  
  AlCon driver for Turbo Vision. It uses the conio.h emulation library on top
  of Allegro.

  Contributed by Grzegorz Adam Hankiewicz <gradha@hankiewicz.datanet.co.uk>

  This is part of Turbo Vision ditribution and covered by the same license.

*****************************************************************************/

#ifndef ALCONKEY_HEADER_INCLUDED
#define ALCONKEY_HEADER_INCLUDED

#ifdef HAVE_ALLEGRO

// A class to encapsulate the globals, all is static!
class TGKeyAlcon : public TGKey
{
public:
 // Usually nothing goes here and the initialization code goes in Init()
 TGKeyAlcon() {};

 // Function replacements
 // Put the keyboard in the original state
 static void     Suspend();
 // Memorize current state and setup our state
 static void     Resume();
 // Is a key waiting to be retreived?
 static int      KbHit();
 // Remove all the keys waiting to be retrieved.
 static void     Clear();
 // Get the next key. The format is the one you can see in tkeyext.h and
 // gkey.h
 static ushort   GKey();
 // What's the current state of modifier keys. The IBM BIOS values are used.
 // Not all targets implement it.
 static unsigned GetShiftState();
 // Fill a TEvent structure with the next available key (or evNothing)
 static void     FillTEvent(TEvent &e);
 // Set a particular mode requested by the user. The values should be allocated
 // in the keyMode enumeration found in TGKey to avoid collisions between drivers.
 static void     SetKbdMapping(int version);

 // Setup the pointers to point our members
 static void     Init();

protected:
 // Put support routines and variables here
 static unsigned Symbol;
 static uchar    Scan;
 static unsigned Flags;
};
#endif // HAVE_ALLEGRO
#endif // ALCONKEY_HEADER_INCLUDED

