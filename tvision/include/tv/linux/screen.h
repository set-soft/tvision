/* Linux keyboard handler routines header.
   Copyright by Salvador E. Tropea (SET) (2001-2003)
   Covered by the GPL license. */
#if defined(TVOSf_Linux) && !defined(LINUXSCREEN_HEADER_INCLUDED)
#define LINUXSCREEN_HEADER_INCLUDED

// Modes you can pass to Init
const int lnxInitVCSrw=0, lnxInitVCSwo=1, lnxInitSimple=2, lnxInitMDA=3;

struct stCodePageCk;
struct stCodePageLang;
struct CodePage;

// virtual to avoid problems with multiple inheritance
class TDisplayLinux : virtual public TDisplay
{
protected:
 TDisplayLinux() {};
 // We will use casts to base classes, destructors must be pointers
 virtual ~TDisplayLinux();
 // This sets the pointers of TDisplay to point to this class
 static void   Init(int mode);

 // Behaviors to replace TDisplay
 // Monochrome (MDA, Hercules, etc.) members are marked with MDA
 // /dev/vcs* members are marked with VCS
 static void   SetCursorPos(unsigned x, unsigned y);
 static void   SetCursorPosMDA(unsigned x, unsigned y);
 static void   SetCursorPosVCS(unsigned x, unsigned y);
 
 static void   GetCursorPos(unsigned &x, unsigned &y);
 static void   GetCursorPosVCS(unsigned &x, unsigned &y);
 static void   GetCursorPosGeneric(unsigned &x, unsigned &y);
 
 static void   SetCursorShape(unsigned start, unsigned end);
 static void   SetCursorShapeMDA(unsigned start, unsigned end);
 
 static void   GetCursorShape(unsigned &start, unsigned &end);
 static void   GetCursorShapeMDA(unsigned &start, unsigned &end);

 // Not available static void clearScreen(uchar, uchar);
 
 static ushort GetRows();
 //static ushort GetRowsMDA(); is ushort TDisplay::defaultGetRows()
 
 static ushort GetCols();
 //static ushort GetColsMDA(); is ushort TDisplay::defaultGetCols()
 
 // Not available static void   SetCrtMode(ushort mode);
 // Not available static ushort getCrtMode();
 // Not available static void   SetCrtModeExt(char *mode);
 static int    CheckForWindowSize(void);
 static const char *GetWindowTitle(void);
 static int    SetWindowTitle(const char *name);
 
 static int    SetDisPaletteColors(int from, int number, TScreenColor *colors);
 static void   GetDisPaletteColors(int from, int number, TScreenColor *colors);

 // Functions and members specific for this driver
 // Helpers to make the code easy to read
 inline static int  canWriteVCS();
 inline static int  canReadVCS();
 static void   setUpEnviron();
 // Current cursor position
 static int curX, curY;
 // Position to restore
 static unsigned oldCurX, oldCurY;
 // Current cursor shape
 static int cursorStart,cursorEnd;
 // 1 when the size of the window where the program is running changed
 static volatile sig_atomic_t windowSizeChanged;
 static int vcsWfd; // virtual console system descriptor
 static int vcsRfd; // Same for reading
 static int hOut;   // Handle for the console output
 static FILE *fOut; // Stream for the console output
 // Original environment of the application, here we put the title
 static char *origEnvir;
 // New environment, a copy of the original
 static char *newEnvir;
 // How much space we have
 static int maxLenTit;
 // Can we use TIOCLINUX? Initialized but TScreen, but also used here
 static char tioclinuxOK;
 // Color translation table
 static char cMap[16];
 // The code page we detected (or just guess) is installed
 static int  installedSFM;
 static int  installedACM;
};

inline
int  TDisplayLinux::canWriteVCS()
{
 return vcsWfd>=0;
}

inline
int  TDisplayLinux::canReadVCS()
{
 return vcsRfd>=0;
}

// With this order the destructor will be called first for TScreen,
// TScreenLinux, TScreen, TScreenLinux::suspend, TDisplayLinux and
// finally TDisplay (twice).
class TScreenLinux : public TDisplayLinux, public TScreen
{
public:
 TScreenLinux();
 // We will use casts to base classes, destructors must be pointers
 virtual ~TScreenLinux();

 friend class TDisplayLinux;
 friend class TGKeyLinux; // Suspend/ResumeFont on VT switch

protected:

 static void   Suspend();
 static void   Resume();
 // Default: ushort fixCrtMode( ushort mode )
 // Default: void   setCrtData();
 // Default: void   clearScreen()
 static void   SetVideoMode(ushort mode);
 static void   SetVideoModeExt(char *mode);
 static void   GetCharactersMDA(unsigned offset,ushort *buf,unsigned count);
 static void   GetCharactersVCS(unsigned offset,ushort *buf,unsigned count);
 static ushort GetCharacter(unsigned dst);
 static void   SetCharacter(unsigned offset,uint32 value);
 static void   SetCharactersTerm(unsigned dst,ushort *src,unsigned len);
 static void   SetCharactersVCS(unsigned dst,ushort *src,unsigned len);
 static void   SetCharactersMDA(unsigned dst,ushort *src,unsigned len);
 static int    System(const char *command, pid_t *pidChild, int in, int out,
                      int err);
 static int    GetFontGeometry(unsigned &w, unsigned &h);
 static int    SetFont(int changeP, TScreenFont256 *fontP,
                       int changeS, TScreenFont256 *fontS,
                       int fontCP=-1, int appCP=-1);
 static void   RestoreFonts();

 // Initialization code executed just once
 static int InitOnce();
 // Initialize member pointers
 static void Init(int mode);
 // Detects if we can access /dev/vcs* device. Initializes vcsWfd and vcsRfd.
 static void DetectVCS();
 // Detects a MDA board. Indicated in secondaryAvailable.
 static void DetectSecondaryDisplay();
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
 // Closes and releases all, called from TDisplayLinux
 static void DeallocateResources();
 // Analyzes the console code page
 static int  AnalyzeCodePage();
 static void CreateSFMFromTable(ushort *table);
 static void CreateACMFromTable(ushort *table);
 // A guess about the code page using the language
 static int  GuessCodePageFromLANG();
 // Tries to find the geometry of loaded fonts
 static int  GetLinuxFontGeometry();
 static void FreeFontsMemory();
 static int  AllocateFontsMemory();
 static int  GetLinuxFont();
 static void ExpandFont(uchar *dest, TScreenFont256 *font);
 static void SuspendFont();
 static void ResumeFont();
 static void AvoidUTF8Mode();
 static int  isInUTF8();
 // Terminal state before starting
 static struct termios outTermiosOrig;
 // Our terminal state
 static struct termios outTermiosNew;
 // Last error
 static const char *error;
 // Is MDA accesible?
 static char secondaryAvailable;
 // File handle for mmaped memory of MDA board
 static int mdaMemH;
 // Mmaped MDA memory
 static ushort *mdaMem;
 // Kind of palette
 static int palette;
 enum
 {
  PAL_MONO, PAL_LOW, PAL_HIGH, PAL_LOW2
 };
 // Buffer containing the screen before we started
 static ushort *userBuffer;
 // Size of the buffer
 static unsigned userBufferSize;
 // Current colors, for escape sequences
 static int oldCol, oldBack, oldFore;
 // List of known code pages
 static struct stCodePageCk   knownFonts[];
 static struct stCodePageCk   knownScreenMaps[];
 // List of known languages and their code pages
 static struct stCodePageLang langCodePages[];
 // Used when the loaded maps are unknown
 static CodePage unknownACM, unknownSFM;
 // Fonts stuff
 static uchar  canSetFonts,primaryFontSet,secondaryFontSet;
 static struct console_font_op linuxFont;
 static struct console_font_op ourFont;
 static int    origCPScr, origCPApp, origCPInp;
 // Original UTF-8 state
 static int    origUTF8Mode;
 // We detected the user is using a 512 symbols font
 static int    font512Detected;
 // Related to font512Detected, it means we have to reduce colors to 8
 static int    reduceTo8Colors;
};

// SET: Enclosed all the I/O stuff in "__i386__ defined" because I don't
// think it have much sense in non-Intel PCs. In fact looks like it gives
// some problems when compiling for Alpha (__alpha__).
//   Also make it only for Linux until I know how to do it for FreeBSD.

#if defined(TVCPU_x86)
 // Needed for ioperm, used only by i386.
 // I also noted that glibc 2.1.3 for Alpha, SPARC and PPC doesn't have
 // this header
 #include <sys/perm.h>
 #define h386LowLevel
#endif

#endif // LINUXSCREEN_HEADER_INCLUDED

