/*
 * Windows screen routines header.
 *
 *  Copyright (c) 2002 by Jose Angel Sanchez Caso (JASC)
 *
 *  Squeleton based in code by Salvador E. Tropea (SET)
 *  Covered by the GPL license. 
 */
 
#if defined(TVOS_Win32) && !defined(TV_Disable_WinGr_Driver) \
    && !defined(WINGRKEY_HEADER_INCLUDED)
#define WINGRKEY_HEADER_INCLUDED

typedef struct
{ int   win;
  short key;
} ConvKeyRec;


struct TEvent;

struct TGKeyWinGr: virtual public TDisplayWinGr // Access to display attributes
		         , virtual public TGKey         // A class to encapsulate the globals, all is static!
                 , virtual public TVCodePage    // Trick to access codepage mappings
{ TGKeyWinGr(): TVCodePage(0,0,0) {};          // Only need access to protected members of TVCodePage

  static void init();
  static int testEvents( UINT   message
                       , WPARAM wParam
                       , LPARAM lParam );


// Function replacements
// static void     Suspend();
// static void     Resume();
// static void     Clear();
// static ushort   GKey();
// static void     SetKbdMapping(int version);
 

 static int      KbHit        (); 
 static unsigned GetShiftState();
 static void     Init         ();                // Setup the pointers to point our members
 static void     FillTEvent   ( TEvent &e );
 

protected:  // For this driver
 
 friend class THWMouseWinGr;
 friend class TScreenWinGr;

private:


// Support variables

 static ushort     keyMask;
 static CodePage * remapKey;  // Multilingual keyboard support

// Support methods

 static int setKey( const ConvKeyRec * xlate 
                  , int vcode );


};

#endif // WINDOWSKEY_HEADER_INCLUDED

