/**[txh]********************************************************************

  Copyright (c) 2002 by Salvador E. Tropea
  Covered by the GPL license.

  Description: 
  Keyboard `driver' classes for DOS.
  
***************************************************************************/

#if defined(TVCompf_djgpp) && !defined(DOSKEY_HEADER_INCLUDED)
#define DOSKEY_HEADER_INCLUDED
struct TEvent;

// A class to encapsulate the globals, all is static!
class TGKeyDOS : public TGKey
{
public:
 TGKeyDOS() {};

 // Function replacements
 static unsigned GetShiftStateBIOS();
 static void     ClearBIOS();
 static unsigned GetShiftStateDirect();
 static void     ClearDirect();
 static ushort   GKey();
 static void     FillTEvent(TEvent &e);
 static void     SetKbdMapping(int version);
 static int      GetKbdMapping(int version);

 // Setup the pointers to point our members
 static void     Init();

protected:
 // Specific for this driver
 static void     GetRawBIOS(void);
 static void     GetRawDirect(void);
 static void   (*GetRaw)(void);
 static int      KbHitBIOS();
 static int      KbHitDirect();
 static int      InternationalToKey(uchar ascii);
 static int      Abstract;
 static char     ascii;
 static ushort   sFlags;
 static KeyType  rawCode;
 static int      useBIOS;
 static int      translateKeyPad;
 static const uchar kbWithASCII0[256];
 static const uchar kbWithASCIIE0[256];
 static char        useFixKbdBuffer;
};
#endif // DOSKEY_HEADER_INCLUDED

