/* Linux keyboard handler routines header.
   Copyright by Salvador E. Tropea (SET) (2001-2002)
   Covered by the GPL license. */
#if defined(TVOSf_Linux) && !defined(LINUXKEY_HEADER_INCLUDED)
#define LINUXKEY_HEADER_INCLUDED
struct TEvent;

const int MaxLenEscapeSequence=16;

// A class to encapsulate the globals, all is static!
class TGKeyLinux : public TGKey
{
public:
 TGKeyLinux() {};

 // Function replacements
 static void     Suspend();
 static void     Resume();
 static int      KbHit();
 static void     Clear();
 static ushort   GKey();
 static unsigned GetShiftState();
 static void     FillTEvent(TEvent &e);
 static void     SetKbdMapping(int version);
 static int      GetKbdMapping(int version);

 enum { Default, KOI8 };

 // Setup the pointers to point our members
 static void     Init(int map);

 friend class TScreenLinux;
 friend class TDisplayLinux;

protected:
 // Specific for this driver
 static int InitOnce();
 // Low level code to get a key and process it
 static int GetRaw();
 // A low level helper
 static int GetKeyParsed();
 // Get a key from the buffer
 static int GetKeyFromBuffer();
 // Escape sequences parser
 static int ProcessEscape();
 // Low level function to find the key modifiers
 static unsigned GetLinuxShiftState();
 // Trap the console switching signals
 static void hookVTSignals();
 // Stop traping the console switching signals
 static void unHookVTSignals();
 // Change the keyboard map
 static void patchKeyMap();
 // Restore keymap
 static void unPatchKeyMap();
 // Initializes the patching tables
 static void keyMapInit();
 // Signal handler called when we lose the VT
 static void releaseVTHandler(int);
 // Signal handler called when we get back the VT
 static void acquireVTHandler(int);
 // Small helpers to do both operations and check
 static void doHookAndPatch();
 static void doUnHookAndUnPatch();
 // File handle for the input
 static int hIn;
 // Stream for the input
 static FILE *fIn;
 // Old mode of the input file handle
 static int oldInFlags;
 // New mode of the input file handle
 static int newInFlags;
 // Termios data of the console before starting
 static struct termios inTermiosOrig;
 // Our termios settings
 static struct termios inTermiosNew;
 // Description of the last error
 static const char *error;
 // Buffer to store a key escape sequence while we analize it.
 // If a sequence failed the keys must be returned from here.
 static int bufferKeys[MaxLenEscapeSequence];
 // How many keys we have in the buffer
 static int keysInBuffer;
 // The next key to return. It avoids using things like ungetc
 static int nextKey;
 // Last key code we decoded
 static int lastKeyCode;
 // Last set of modifiers
 static int lastModifiers;
 // Modifiers in the TV format
 static int translatedModifiers;
 // ASCII of the key
 static char ascii;
 // Keyboard translation tables
 static unsigned char kbToName[128];
 static unsigned char kbExtraFlags[128];
 // Flag to disable keyboard patching. Use SetKbdMapping(linuxDisableKeyPatch).
 static char dontPatchKeyboard;
 // Our Virtual Terminal number
 static int ourVT;
 // The mode of our VT before we started
 static struct vt_mode oldVTMode;
 // The mode of our VT used by TV
 static struct vt_mode newVTMode;
 // Indicates if we hooked thed signals and patched the keyboard
 static char vtHooked;
 // Indicates if the ioctl to patch the keyboard is usable
 static char canPatchKeyboard;
 // Indicates if we patched the keyboard
 static char keyPatched;
 // Generic keyboard entry structure used in various places
 static struct kbentry entry;
};

#endif // LINUXKEY_HEADER_INCLUDED

