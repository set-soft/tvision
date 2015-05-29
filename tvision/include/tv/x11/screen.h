/* X11 screen routines header.
   Copyright (c) 2001-2007 by Salvador E. Tropea (SET)
   Covered by the GPL license. */
// X headers are needed to include it
#if (defined(TVOS_UNIX) || defined(TVCompf_Cygwin)) && defined(HAVE_X11) && !defined(X11SCR_HEADER_INCLUDED)
#define X11SCR_HEADER_INCLUDED

class TVX11Clipboard;
class TVX11UpdateThread;
class TNSCollection;

// virtual to avoid problems with multiple inheritance
class TDisplayX11 : virtual public TDisplay
{
protected:
 TDisplayX11() {};
 // We will use casts to base classes, destructors must be pointers
 virtual ~TDisplayX11();
 // This sets the pointers of TDisplay to point to this class
 static void   Init();

 // Behaviors to replace TDisplay
 static void   SetCursorPos(unsigned x, unsigned y);
 static void   GetCursorPos(unsigned &x, unsigned &y);
 static void   GetCursorShape(unsigned &start, unsigned &end);
 static void   SetCursorShape(unsigned start, unsigned end);
 static ushort GetRows();
 static ushort GetCols();
 // Not available static void   SetCrtMode(ushort mode);
 // Not available static void   SetCrtModeExt(char *mode);
 // Not available static ushort GetCrtModeExt()
 static int    CheckForWindowSize(void);
 // Not available static const char *getWindowTitle(void);
 // Not available static int setWindowTitle(const char *name);

 // Variables for this driver
 // Cursor position
 static int       cursorX, cursorY;
 // Cursor position in pixels
 static int       cursorPX, cursorPY;
 // Size of the font
 static unsigned  fontW,fontWb,fontH,fontSz;
 // Cursor shape in absolute values (0-fontH)
 static char      cShapeFrom,cShapeTo;
 // Flag to indicate that the size of the window changed
 static char      windowSizeChanged;
 // Window size in characters
 static int       maxX, maxY;
 // Window size before last resize
 static int       newX, newY;
};

// With this order the destructor will be called first for TScreenX11,
// TScreen, TDisplayX11 and finally TDisplay.
class TScreenX11 : public TDisplayX11, public TScreen
{
public:
 TScreenX11();
 // We will use casts to base classes, destructors must be pointers
 virtual ~TScreenX11();

 friend class TGKeyX11;
 friend class THWMouseX11;
 friend class TDisplayX11;
 friend class TVX11Clipboard;
 friend class TVX11UpdateThread;

protected:
 // Default: void   Resume();
 // Default: void   Suspend();
 // Default: ushort fixCrtMode( ushort mode )
 // Default: static void   setCrtData();
 static void   clearScreen();
 // Default: static void   setVideoMode(ushort mode);
 // Default: static void   setVideoModeExt(char *mode);
 // Default: static void   getCharacters(unsigned offset,ushort *buf,unsigned count);
 // Default: static ushort getCharacter(unsigned dst);
 static void   setCharacter(unsigned offset, uint32 value);
 static void   setCharacterU16(unsigned offset, uint32 value);
 static void   setCharacters(unsigned dst, ushort *src, unsigned len);
 static void   setCharactersU16(unsigned offset, ushort *values, unsigned count);
 static void   setCharactersX11U16(unsigned offset, ushort *values, unsigned w);
 static int    System(const char *command, pid_t *pidChild, int in, int out, int err);
 static int    setWindowTitle(const char *aName);
 static const char *getWindowTitle(void);
 static int    SetDisPaletteColors(int from, int number, TScreenColor *colors);
 static int    GetFontGeometry(unsigned &w, unsigned &h);
 static int    GetFontGeometryRange(unsigned &wmin, unsigned &hmin,
                                    unsigned &umax, unsigned &hmax);
 static int    SetFont(int changeP, TScreenFont256 *fontP,
                       int changeS, TScreenFont256 *fontS,
                       int fontCP=-1, int appCP=-1);
 static void   RestoreFonts();
 static int    SetCrtModeRes(unsigned w, unsigned h, int fW=-1, int fH=-1);
 static Boolean ShowBusyState(Boolean state);
 static void    Beep();
 static appHelperHandler OpenHelperApp(AppHelper kind);
 static Boolean CloseHelperApp(appHelperHandler id);
 static Boolean SendFileToHelper(appHelperHandler id, const char *file, void *extra);
 static const char *GetHelperAppError();
 
protected:
 // Blinking cursor emulation
 static void   UnDrawCursor();
 static void   DrawCursor();
 static void   DisableCursor();
 static void   EnableCursor();

 // Events processing
 // Events loop
 static void   ProcessGenericEvents();
 // Internal line update
 static void   writeLineCP(int x, int y, int w, void *str, unsigned color);
 static void   writeLineU16(int x, int y, int w, void *str, unsigned color);
 static void   writeLineX11U16(int x, int y, int w, void *str, unsigned color);
 static void   (*writeLine)(int x, int y, int w, void *str, unsigned color);
 // Internal rectangle update
 static void   redrawBufCP(int x, int y, unsigned w, unsigned off);
 static void   redrawBufU16(int x, int y, unsigned w, unsigned off);
 static void   (*redrawBuf)(int x, int y, unsigned w, unsigned off);

 // Font helpers
 static void   CreateXImageFont(int which, uchar *font, unsigned w, unsigned h);
 static void   DestroyXImageFont(int which);
 // It creates the XImage used for the cursor.
 static void   AdjustCursorImage();
 static TScreenFont256 *ChooseClosestFont(unsigned fW, unsigned fH);
 
 static void   FullRedraw();
 static void   DoResize(unsigned w, unsigned h);
 inline
 static void   drawChar(GC gc, unsigned x, unsigned y, uchar aChar, uchar aAttr);
 inline
 static void   drawCharU16(GC gc, unsigned x, unsigned y, uint16 aChar);
 // Creates the mouse cursors
 static Boolean createCursors();
 // Unicode font helpers
 inline // Find which glyph represents the unicode value
 static uint16  unicode2index(uint16 unicode);
 inline // Ensure the glyph is available as XImage
 static void    checkUnicodeGlyph(uint16 code);
 // Helper to set the bacground and foreground in one step
 static void    XSetBgFg(uint16 attr);
 static void    XSetBgFgC(uint16 attr);
 
 // Variables for this driver
 // Foreground and background colors
 static int       fg,bg;
 // Cursor state
 static char      cursorEnabled,cursorInScreen;
 // Packed colors
 static uchar     curAttr;
 // Buffer for cursor shape
 static char     *cursorData;
 // Timers for the cursor blinking
 static struct timeval refCursorTime, curCursorTime;
 // X display we are connected
 static Display  *disp;
 // Screen we are on
 static ulong     screen;
 static Visual   *visual;
 // The parent of all the windows
 static Window    rootWin;
 // The main window, that's the visible thing where we draw
 static Window    mainWin;
 // Color map we have
 static Colormap  cMap;
 // Normal graphics context
 static GC        gc;
 // Cursor graphics context
 static GC        cursorGC;
 // Internationalization stuff
 static XIC       xic;
 static XIM       xim;
 // Needed to catch the windows close
 static Atom      theProtocols;
 // Our colors mapped to X ones
 static ulong     colorMap[16];
 // The images that makes our font
 static XImage   *ximgFont[256];
 static XImage   *ximgSecFont[256];
 // The image for the cursor
 static XImage   *cursorImage;
 // Hints about our size preferences
 static XSizeHints *sizeHints;
 // Window class and resources name
 static XClassHint *classHint;
 // Default fonts
 static TScreenFont256  font8x16;
 static TScreenFont256  font10x20;
 static TScreenFont256 *defaultFont;
 static uchar shapeFont10x20[];
 static uchar shapeFont8x16[];
 static uchar primaryFontChanged;
 // Should we hide the cursor when we lose the focus?
 static char  hideCursorWhenNoFocus;
 // Avoid resizing to a cells multiple if the WM doesn't follow our hints
 static char  dontResizeToCells;
 // Busy cursor stuff
 static Cursor busyCursor, leftPtr;
 static unsigned char busyCursorMap[];
 static unsigned char busyCursorMask[];
 // For testing purposes should be removed
 void LoadFontAsUnicode();
 char *SearchX11Font(const char *foundry, const char *family, int w, int h);
 char *SearchX11Font(const char *foundry, const char *family);
 char *SearchX11Font(const char *pattern);
 // Application Helpers stuff
 static const char *appHelperNameError[];
 static int appHelperError;
 static TNSCollection *appHelperHandlers;
};

// A small class to encapsulate the cliboard, this is too tied to TScreen
class TVX11Clipboard
{
public:
 TVX11Clipboard() {};

protected:
 static int   copy(int id, const char *buffer, unsigned len);
 static char *paste(int id, unsigned &len);
 static void  destroy();

 static void  Init();

 static char    *buffer;
 static unsigned length;
 static Atom     property;
 static int      waiting;
 static const
           char *x11NameError[];

 friend class TScreenX11;
};

const int x11clipNoSelection=1,
          x11clipWrongType=2,
          x11clipNoData=3,
          x11clipX11Error=4,
          x11clipAnother=5,

          x11clipErrors=5;

#define aMouseEvent (ButtonPressMask | ButtonReleaseMask | ButtonMotionMask | \
                     PointerMotionMask | PointerMotionHintMask |\
                     Button1MotionMask | Button2MotionMask | Button3MotionMask |\
                     Button4MotionMask | Button5MotionMask)
#define aKeyEvent   (KeyPressMask | KeyReleaseMask)

#define charPos 0
#define attrPos 1
#define MAKE_16B(ch,attr) ((ch)|((attr)<<8))

#if !HAVE_LINUX_PTHREAD
 #define USE_ALARM_FOR_THREAD 1
#endif

#ifdef USE_ALARM_FOR_THREAD
// This code uses the high resolution Alarm mechanism to update the screen
// Is under testing

 #define SEMAPHORE_ON        TVX11UpdateThread::SemaphoreOn()
 #define SEMAPHORE_OFF       TVX11UpdateThread::SemaphoreOff()
 #define IS_SECOND_THREAD_ON TVX11UpdateThread::CheckSecondThread()
 #define START_UPDATE_THREAD TVX11UpdateThread::StartUpdateThread()
 #define STOP_UPDATE_THREAD  TVX11UpdateThread::StopUpdateThread()
 #define THREAD_AFTER_FORK   TVX11UpdateThread::StopUpdateThread()
 #define NO_EXEC_IN_THREAD   0
 
class TVX11UpdateThread
{
public:
 TVX11UpdateThread() {};
 
 static void SemaphoreOn();
 static void SemaphoreOff();
 static int  CheckSecondThread();
 static void StartUpdateThread();
 static void StopUpdateThread();

protected:
 static void UpdateThread(int signum);
 static void microAlarm(unsigned int usec);
 static int running;
 static int initialized;
};
#elif HAVE_LINUX_PTHREAD
// Linux implementation of POSIX threads
 #define SEMAPHORE_ON        TVX11UpdateThread::SemaphoreOn()
 #define SEMAPHORE_OFF       TVX11UpdateThread::SemaphoreOff()
 #define IS_SECOND_THREAD_ON TVX11UpdateThread::CheckSecondThread()
 #define START_UPDATE_THREAD TVX11UpdateThread::StartUpdateThread()
 #define STOP_UPDATE_THREAD  TVX11UpdateThread::StopUpdateThread()
 #define THREAD_AFTER_FORK   // What can I do?
 #define NO_EXEC_IN_THREAD   1

class TVX11UpdateThread
{
public:
 TVX11UpdateThread() {};
 
 static void SemaphoreOn();
 static void SemaphoreOff();
 static int  CheckSecondThread();
 static void StartUpdateThread();
 static void StopUpdateThread();

protected:
 static void *UpdateThread(void *);
 static int running;
 static int initialized;
 static timeval refWatchDog, nowWatchDog;
 static int watchDogVal;
};
#else
 #define SEMAPHORE_ON
 #define SEMAPHORE_OFF
 #define IS_SECOND_THREAD_ON 0
 #define START_UPDATE_THREAD
 #define STOP_UPDATE_THREAD
 #define NO_EXEC_IN_THREAD   0
 #define THREAD_AFTER_FORK
#endif
// End of Linux implementation of POSIX threads

#endif // X11SCR_HEADER_INCLUDED

