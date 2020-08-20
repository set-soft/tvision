/* UNIX screen handler routines header.
   Copyright by Salvador E. Tropea (SET) (2001-2003)
   Covered by the GPL license. */

#if defined(HAVE_NCURSES) && defined(TVOS_UNIX) && !defined(TVOSf_QNXRtP) && \
   !defined(UNIXSCREEN_HEADER_INCLUDED)
#define UNIXSCREEN_HEADER_INCLUDED

// virtual to avoid problems with multiple inheritance
class TDisplayUNIX : virtual public TDisplay
{
protected:
 TDisplayUNIX() {};
 // We will use casts to base classes, destructors must be pointers
 virtual ~TDisplayUNIX();
 // This sets the pointers of TDisplay to point to this class
 static void   Init();

 // Behaviors to replace TDisplay
 static void   SetCursorPos(unsigned x, unsigned y);
 static void   GetCursorPos(unsigned &x, unsigned &y);
 static void   GetCursorShape(unsigned &start, unsigned &end);
 static void   SetCursorShape(unsigned start, unsigned end);
 // Not available static void clearScreen(uchar, uchar);
 static ushort GetRows();
 static ushort GetCols();
 // Not available static void setCrtMode(ushort mode);
 // Not available static ushort getCrtMode();
 static void   SetCrtModeExt(char *mode);
 static int    CheckForWindowSize(void);
 // Not available static const char *getWindowTitle(void);
 // Not available static int setWindowTitle(const char *name);

 // Functions and members specific for this driver
 inline static void safeput(char *&p, const char *cap);
 inline static int  canWriteVCS();
 inline static int  canReadVCS();
 inline static int  canOnlyWriteVCS();

 static int vcsWfd; // virtual console system descriptor
 static int vcsRfd; // SET: Same for reading
 static int tty_fd; // tty descriptor
 // Current cursor position
 static int cur_x, cur_y;
 // SET: Current cursor shape
 static int cursorStart,cursorEnd;
 // SET: 1 when the size of the window where the program is running changed
 static volatile sig_atomic_t windowSizeChanged;
};

inline
void TDisplayUNIX::safeput(char *&p, const char *cap)
{
 if (cap)
    while (*cap) *p++=*cap++;
}

// With this order the destructor will be called first for TScreenUnix,
// TScreen, TDisplayUNIX and finally TDisplay.
class TScreenUNIX : public TDisplayUNIX, public TScreen
{
public:
 TScreenUNIX();
 // We will use casts to base classes, destructors must be pointers
 virtual ~TScreenUNIX();

 friend class THWMouseXTerm;

 // Fix me: should be protected but right now is used by the editor for the
 // pal. that should be moved here.
 static void SendToTerminal(const char *value);
protected:
 static void InitPCCharsMapping();
 static void sigWindowSizeChanged(int sig);
 static void SpecialKeysDisable(int file);
 static void SpecialKeysRestore(int file);
 static void startcurses(int &terminalCodePage);
 static void mapColor(char *&p, int col);
 static void writeBlock(int dst, int len, ushort *old, ushort *src);
 static void RestoreScreen();

 static void   Resume();
 static void   Suspend();
 // Default: ushort fixCrtMode( ushort mode )
 static void   setCrtData();
 // Default: void clearScreen()
 static void   setVideoMode(ushort mode);
 static void   setVideoModeExt(char *mode);
 static void   getCharacters(unsigned offset,ushort *buf,unsigned count);
 static ushort getCharacter(unsigned dst);
 static void   setCharacter(unsigned offset,uint32 value);
 static void   setCharacters(unsigned dst,ushort *src,unsigned len);
 static int    System(const char *command, pid_t *pidChild, int in, int out,
                      int err);

 enum terminalType
 {
  LINUX_TERMINAL=0,
  GENER_TERMINAL=1,
  VCSA_TERMINAL=2,
  XTERM_TERMINAL=3
 };
 enum
 {
  PAL_MONO, PAL_LOW, PAL_HIGH, PAL_LOW2
 };

 static int      use_pc_chars;
 static int      TerminalType;
 static FILE    *tty_file;
 static int      palette;
 static int      force_redraw;
 // SET: data to restore the XON/XOFF control keys and others like ^C, ^\ and ^Z
 // Used by SpecialKeys*
 static cc_t     oldKeys[5];
 // Used by mapColor:
 static int      old_col,old_fore,old_back;
 // Save/Restore screen
 static ushort  *user_buffer;
 static int      user_buffer_size;
 static unsigned user_cursor_x,user_cursor_y;
 static char    *terminal;
 static char     cursesInitialized;
 static char     termAttrsSaved;
 // SET: That's the job of curses endwin(), additionally it does a much more
 // complete work so I don't see the point of duplicating work, in my system
 // I didn't see any change after removing it, but I left the code just in
 // case
 #ifdef SAVE_TERMIOS
 static struct termios old_term,new_term;
 #endif
};

#endif

