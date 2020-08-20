/* WinNT screen routines header.
   Copyright (c) 2002-2005 by Salvador E. Tropea (SET)
   Covered by the GPL license. */
// This headers needs windows header
#if defined(TVOS_Win32) && !defined(WINNTSCR_HEADER_INCLUDED)
#define WINNTSCR_HEADER_INCLUDED

// Some windows.h headers defines a macro with this name
#undef YieldProcessor

#define USE_NEW_BUFFER
#define hCurrentOut hOut

// virtual to avoid problems with multiple inheritance
class TDisplayWinNT : virtual public TDisplay
{
protected:
 TDisplayWinNT() {};
 // We will use casts to base classes, destructors must be pointers
 virtual ~TDisplayWinNT();
 // This sets the pointers of TDisplay to point to this class
 static void   Init();

 // Behaviors to replace TDisplay
 static void   SetCursorPos(unsigned x, unsigned y);
 static void   GetCursorPos(unsigned &x, unsigned &y);
 static void   GetCursorShape(unsigned &start, unsigned &end);
 static void   SetCursorShape(unsigned start, unsigned end);
 static ushort GetRows();
 static ushort GetCols();
 // Default static void   SetCrtMode(ushort mode);
 // Default static void   SetCrtModeExt(char *mode);
 // Not available static ushort getCrtMode();
 // Not implemented static int    CheckForWindowSize(void);
 static const char
              *GetWindowTitle(void);
 static int    SetWindowTitle(const char *name);
 static void   ClearScreen(uchar w, uchar h);
 static void   Beep();

 // Low level routines, doesn't use cached values
 static void   GetCursorPosLow(unsigned &x, unsigned &y);
 static void   GetCursorShapeLow(unsigned &start, unsigned &end);
 static void   SetCursorShapeLow(unsigned start, unsigned end);
 static void   SetCursorPosLow(unsigned x, unsigned y);

 // Variables for this driver
 // Input/output handles
 static HANDLE hOut,hIn;
 #ifdef USE_NEW_BUFFER
 static HANDLE hStdOut;
 #endif
 // Cursor position
 static unsigned currentCursorX,currentCursorY;
 // Cursor shape
 static unsigned curStart, curEnd;
};

// With this order the destructor will be called first for TScreenWin32,
// TScreen, TDisplayWinNT and finally TDisplay.
class TScreenWinNT : public TDisplayWinNT, public TScreen
{
public:
 TScreenWinNT();
 // We will use casts to base classes, destructors must be pointers
 virtual ~TScreenWinNT();

 static void YieldProcessor(int micros);

 friend class TGKeyWinNT;
 friend class THWMouseWinNT;
 friend class TDisplayWinNT;

protected:
 static void   Resume();
 static void   Suspend();
 // Default: ushort fixCrtMode( ushort mode )
 // Default: static void   setCrtData();
 // Default: static void   clearScreen();
 // Default: static void   setVideoMode(ushort mode);
 // Default: static void   setVideoModeExt(char *mode);
 static void   GetCharacters(unsigned offset,ushort *buf,unsigned count);
 static ushort GetCharacter(unsigned dst);
 static void   SetCharacter(unsigned offset, unsigned int value);
 static void   SetCharacters(unsigned dst, ushort *src, unsigned len);
 static int    System(const char *command, pid_t *pidChild, int in, int out,
                      int err);
 static int    SetCrtModeRes(unsigned w, unsigned h, int fW=-1, int fH=-1);

 // Support functions
 #ifndef USE_NEW_BUFFER
 static void SaveScreen();
 static void SaveScreenReleaseMemory();
 static void RestoreScreen();
 #endif
 //static void ScreenUpdate();
 static BOOL WINAPI ConsoleEventHandler(DWORD dwCtrlType);
 static void ensureOutBufCapacity(unsigned count);
 // Initialization done at start
 static int  InitOnce();

 // Support variables
 // Old console state
 #ifndef USE_NEW_BUFFER
 static ushort*  saveScreenBuf;
 static unsigned saveScreenSize;
 static unsigned saveScreenCursorStart, saveScreenCursorEnd;
 static unsigned saveScreenCursorX, saveScreenCursorY;
 #endif
 static DWORD    saveScreenConsoleMode;
 static unsigned saveScreenWidth, saveScreenHeight;
 // Buffer used to arrange the data as needed by Win32 API
 static CHAR* outBuf;
 static WORD* outBufAttr;
 static CHAR_INFO* outBufCI;
 static unsigned outBufCapacity;
};

#endif // WINNTSCR_HEADER_INCLUDED

