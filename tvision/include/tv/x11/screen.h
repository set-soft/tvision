/* X11 screen routines header.
   Copyright (c) 2001 by Salvador E. Tropea (SET)
   Covered by the GPL license. */
// X headers are needed to include it
#ifndef X11SCR_HEADER_INCLUDED
#define X11SCR_HEADER_INCLUDED

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
 // Not available, but needs to do some job
 static void   SetCrtMode(ushort mode);
 // Not available, but needs to do some job
 static void   SetCrtModeExt(char *mode);
 // Not available, static ushort GetCrtModeExt()
 static int    CheckForWindowSize(void);
 // Not available static const char *getWindowTitle(void);
 // Not available static int setWindowTitle(const char *name);

 // Variables for this driver
 // Cursor position
 static int       cursorX, cursorY;
 // Cursor position in pixels
 static int       cursorPX, cursorPY;
 // Size of the font
 static unsigned  fontW,fontH;
 // Cursor shape in absolute values (0-fontH)
 static char      cShapeFrom,cShapeTo;
 // Flag to indicate that the size of the window changed
 static char      windowSizeChanged;
 // Window size in characters
 static int       maxX, maxY;
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
 static void   setCharacter(unsigned offset, ushort value);
 static void   setCharacters(unsigned dst, ushort *src, unsigned len);
 static int    System(const char *command, pid_t *pidChild);
 static int    setWindowTitle(const char *aName);
 static const char *getWindowTitle(void);
 static void   SetDisPaletteColors(int from, int number, TScreenColor *colors);
 
protected:
 // Blinking cursor emulation
 static void   sigAlm(int sig);
 static void   UnDrawCursor();
 static void   DrawCursor();
 static void   DisableCursor();
 static void   EnableCursor();

 // Events processing
 // Events loop
 static void   ProcessGenericEvents();
 // Internal line update
 static void   writeLine(int x, int y, int w, unsigned char *str, unsigned color);
 // Internal rectangle update
 static void   redrawBuf(int x, int y, unsigned w, unsigned off);

 // Variables for this driver
 // Foreground and background colors
 static int       fg,bg;
 // Cursor state
 static char      cursorEnabled,cursorInScreen;
 // Packed colors
 static uchar     curAttr;
 // Indicates if we must make a change in the cursor (for the blink)
 static volatile char
                  cursorChange;
 // Buffer for cursor shape
 static char     *cursorData;
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
 // The image for the cursor
 static XImage   *cursorImage;
};

#define aMouseEvent (ButtonPressMask | ButtonReleaseMask | ButtonMotionMask)
#define aKeyEvent   (KeyPressMask | KeyReleaseMask)

#define charPos 0
#define attrPos 1

#endif // X11SCR_HEADER_INCLUDED

