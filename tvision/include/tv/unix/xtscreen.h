/* XTerm display handler routines header.
   Copyright by Salvador E. Tropea (SET) (2002-2003)
   Covered by the GPL license. */
#if defined(TVOS_UNIX) && !defined(TVOSf_QNXRtP) && !defined(XTERMSCREEN_HEADER_INCLUDED)
#define XTERMSCREEN_HEADER_INCLUDED

// virtual to avoid problems with multiple inheritance
class TDisplayXTerm : virtual public TDisplay
{
protected:
 TDisplayXTerm() {};
 // We will use casts to base classes, destructors must be pointers
 virtual ~TDisplayXTerm();
 // This sets the pointers of TDisplay to point to this class
 static void   Init();

 // Behaviors to replace TDisplay
 static void   SetCursorPos(unsigned x, unsigned y);
 static void   GetCursorPos(unsigned &x, unsigned &y);
 static void   SetCursorShape(unsigned start, unsigned end);
 static void   GetCursorShape(unsigned &start, unsigned &end);
 // Not available static void clearScreen(uchar, uchar);
 static ushort GetRowsIOCTL();
 static ushort GetRowsSeq();
 static ushort GetColsIOCTL();
 static ushort GetColsSeq();
 // Not available static void   SetCrtMode(ushort mode);
 // Not available static ushort getCrtMode();
 // Not available static void   SetCrtModeExt(char *mode);
 static int    SetCrtModeXT(unsigned w, unsigned h, int , int );
 static int    SetCrtModeEt(unsigned w, unsigned h, int , int );
 static int    CheckForWindowSize(void);
 static const char *GetWindowTitle(void);
 static int    SetWindowTitle(const char *name);
 static int    SetDisPaletteColorsXT(int from, int number, TScreenColor *colors);
 static int    SetDisPaletteColorsEt(int from, int number, TScreenColor *colors);

 // Functions and members specific for this driver
 // Current cursor position
 static int curX, curY;
 // Current cursor shape
 static int cursorStart,cursorEnd;
 // 1 when the size of the window where the program is running changed
 static volatile sig_atomic_t windowSizeChanged;
 static int hOut;   // Handle for the console output
 // Font size, this is just a guess
 static unsigned fontW,fontH;
 // To distiguish between similar terminals
 enum { XTerm, Eterm };
 static int terminalType;
 // How to restore the palette
 static void   ResetPaletteColorsXT();
 static void   ResetPaletteColorsEt();
 static void (*ResetPaletteColors)();
 // Color translation table
 static char cMap[16];
 // Which charset is currently selected (G0/G1)
 static int selCharset;
 // Available font sizes
 static TScreenResolution XFonts[];
 static char  fontChanged;
 static char *oldFontName;
 static char *GetCurrentFontName();
 // Window size at start-up
 static uchar startScreenWidth,startScreenHeight;
};

// With this order the destructor will be called first for TScreen,
// TScreenXTerm, TScreen, TScreenXTerm::suspend, TDisplayXTerm and
// finally TDisplay (twice).
class TScreenXTerm : public TDisplayXTerm, public TScreen
{
public:
 TScreenXTerm();
 // We will use casts to base classes, destructors must be pointers
 virtual ~TScreenXTerm();

 friend class TDisplayXTerm;

protected:

 static void   Suspend();
 static void   Resume();
 // Default: ushort fixCrtMode( ushort mode )
 // Default: void   setCrtData();
 // Default: void   clearScreen()
 static void   SetVideoMode(ushort mode);
 static void   SetVideoModeExt(char *mode);
 // Default: static void   GetCharacters(unsigned offset,ushort *buf,unsigned count);
 // Default: static ushort GetCharacter(unsigned dst);
 // Default: static void   SetCharacter(unsigned offset,ushort value);
 static void   SetCharacters(unsigned dst,ushort *src,unsigned len);
 static int    System(const char *command, pid_t *pidChild, int in, int out,
                      int err);

 // Initialization code executed just once
 static int InitOnce();
 // Initialize member pointers
 static void Init();
 // Window size change signal handler
 static void sigWindowSizeChanged(int sig);
 // Helper function to save the current content of the screen
 static void SaveScreen();
 static void RestoreScreen();
 static void SaveScreenReleaseMemory(void);
 // Helper used to resize screenBuffer
 static void CheckSizeBuffer(int oldWidth, int oldHeight);
 // Helper to write the text and escape sequences
 static void writeBlock(int dst, int len, ushort *old, ushort *src);
 // Helper to set the color escape sequence
 static void mapColor(int col);
 // Closes and releases all, called from TDisplayXTerm
 static void DeallocateResources();
 // Terminal state before starting
 static struct termios outTermiosOrig;
 // Our terminal state
 static struct termios outTermiosNew;
 // Last error
 static const char *error;
 // Kind of palette
 static int palette;
 enum
 {
  PAL_MONO, PAL_LOW, PAL_HIGH, PAL_LOW2
 };
 // Current colors, for escape sequences
 static int oldCol, oldBack, oldFore;
 // Tables to convert to ISO 2022 scheme
 static const uchar Code[256];
 static const uchar Modifier[256];
 // Indicates if we must use the shell screen instead of the secondary buffer
 static char  useShellScreen;
};

#endif // XTERMSCREEN_HEADER_INCLUDED

