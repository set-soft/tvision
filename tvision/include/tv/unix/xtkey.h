/* XTerm keyboard handler routines header.
   Copyright by Salvador E. Tropea (SET) (2002)
   Covered by the GPL license. */
#if defined(TVOS_UNIX) && !defined(TVOSf_QNXRtP) && !defined(XTERMKEY_HEADER_INCLUDED)
#define XTERMKEY_HEADER_INCLUDED
struct TEvent;
struct node;

const int MaxLenEscapeSequence=16;

// A class to encapsulate the globals, all is static!
class TGKeyXTerm : public TGKey
{
public:
 TGKeyXTerm() {};

 // Function replacements
 static void     Suspend();
 static void     Resume();
 static int      KbHit();
 static void     Clear();
 static ushort   GKey();
 static unsigned GetShiftState();
 static void     FillTEvent(TEvent &e);

 // Setup the pointers to point our members
 static void     Init();

 friend class TScreenXTerm;
 friend class TDisplayXTerm;

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
 // Escape sequences tree creation helpers
 static node *NewNode();
 static node *NewNode(node *p);
 static node *SearchInList(node *p, uchar k);
 static node *AddKey2List(node *p, uchar k, node **pa);
 static void AddKey(const uchar *key, uchar code, uchar modifiers);
 // This fills the tree with known combinations
 static void PopulateTree();
 // File handle for the input
 static int hIn;
 // Stream for the hIn
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
 // Keys tree
 static node *Keys;
 // Mouse is reported as a escape sequence and hence handled here
 static int MouseButtons;
};

#endif // XTERMKEY_HEADER_INCLUDED

