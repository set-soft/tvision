/* Win32 screen routines header.
   Copyright (c) 2002 by Salvador E. Tropea (SET)
   Covered by the GPL license. */
// This headers needs windows header
#if defined(TVOS_Win32) && !defined(WIN32KEY_HEADER_INCLUDED)
#define WIN32KEY_HEADER_INCLUDED
struct TEvent;

const unsigned eventKeyboardQSize=16;

// A class to encapsulate the globals, all is static!
class TGKeyWin32 : public TGKey
{
public:
 TGKeyWin32() {};

 // Function replacements
 //static void     Suspend();
 //static void     Resume();
 static int      KbHit();
 //static void     Clear();
 //static ushort   GKey();
 static unsigned GetShiftState();
 static void     FillTEvent(TEvent &e);
 //static void     SetKbdMapping(int version);

 // Setup the pointers to point our members
 static void     Init();
 static void     DeInit();

protected:
 // For this driver
 // Extract the shift state from the event (also used by the mouse class)
 static void   ProcessControlKeyState(INPUT_RECORD *ir);
 // Translate Win32 shift state values to TV equivalents
 static ushort transShiftState(DWORD state);
 // Translate Win32 key events to TV equivalents
 static int    transKeyEvent(KeyDownEvent &dst, KEY_EVENT_RECORD& src);
 // Add a key to the queue
 static void   putConsoleKeyboardEvent(KeyDownEvent &key);
 // Remove a key event from the queue
 static int    getConsoleKeyboardEvent(KeyDownEvent &key);
 // Process a Win32 key event and put it in the queue
 static void   HandleKeyEvent();

 // Last value recieved for the shift modifiers
 static ushort LastControlKeyState;
 // Table used to Translate keyboard events
 static const char KeyTo[256];
 // Table for ASCII printable values
 static const char testChars[];
 // Queue
 static KeyDownEvent *evKeyboardIn;
 static KeyDownEvent *evKeyboardOut;
 static KeyDownEvent evKeyboardQueue[eventKeyboardQSize];
 static unsigned evKeyboardLength;
 static CRITICAL_SECTION lockKeyboard;

 friend class THWMouseWin32;
 friend class TScreenWin32;
};

#endif // WIN32KEY_HEADER_INCLUDED

