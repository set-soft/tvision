/* X11 keyboard handler routines header.
   Copyright (c) 2001-2003 by Salvador E. Tropea (SET)
   Covered by the GPL license. */
#if (defined(TVOS_UNIX) || defined(TVCompf_Cygwin)) && defined(HAVE_X11) && !defined(X11KEY_HEADER_INCLUDED)
#define X11KEY_HEADER_INCLUDED
struct TEvent;

const unsigned MaxKbLen=7;
typedef struct
{
 unsigned symbol;
 unsigned key;
} keyEquiv;

// A class to encapsulate the globals, all is static!
class TGKeyX11 : public TGKey
{
public:
 TGKeyX11() {};

 // Function replacements
 //static void     Suspend();
 //static void     Resume();
 static int      KbHit();
 //static void     Clear();
 static ushort   GKey();
 static unsigned GetShiftState();
 static void     FillTEvent(TEvent &e);
 //static void     SetKbdMapping(int version);
 //static int      SetCodePage(int id);
 static void     FillCharCode(TEvent &);

 // Setup the pointers to point our members
 static void     Init();

protected:
 // Specific for this driver
 static int      getKeyEvent(int block);
 // Search the unicode in the current input CP
 static unsigned Unicode2CP(uint16 unicode);

 static int      lenKb,kbWaiting;
 static char     bufferKb[MaxKbLen+1];
 static KeySym   Key;
 static unsigned kbFlags;
 static uchar    KeyCodeByKeySym[256];
 static keyEquiv XEquiv[];
 static uchar    KeyCodeByASCII[96];

 // Values used to fill the event
 static unsigned Symbol;
 static unsigned Unicode;
 static unsigned Flags;
 static uchar    Scan;

 // This is used to translate the WM close to a cmQuit event
 static uchar    sendQuit;

 friend class TScreenX11;
};
#endif // X11KEY_HEADER_INCLUDED

