/* TDisplay/TScreen header.
   Copyright (c) 2002 by Salvador E. Tropea (SET)
   Covered by the GPL license.

   This driver defines a class compatible with original Turbo Vision but
implemented in a very different way. Most members are function pointer
that can be changed by the hardware/OS dependent driver.
   This idea was propposed by JASC.
   I also included some members that Robert added to original TV.
   A lot of new members were added, this is just compatible with Borland's
classes, but doesn't have much in common.
*/

#if defined( Uses_TScreen ) && !defined( __TScreen )
#define __TScreen

class TVCodePage;

// SET: That's a wrapper to enclose the DOS fonts stuff.
// Under Linux can't be used and just returns error.
class TFont
{
public:
 TFont() {};
 virtual ~TFont() {}; // The class is destroyed
 virtual int SelectFont(int height, int width=8, int bank=0, int noForce=0, int modeRecalculate=1);
 virtual void RestoreDefaultFont(void) {};
 // This flag indicates that the next call must use the BIOS fonts or the
 // original fonts. Lamentably the last is very complicated.
 static  int UseDefaultFontsNextTime;
};

// Components are 0-255
typedef struct
{
 uchar R,G,B,Alpha;
} TScreenColor;

/**[txh]********************************************************************

  Description:
  SET: Here I'm writing some implementation details that could help to
understand it.@*
  This class is the low level stuff and is used by TScreen. The user isn't
supposed to call it.@*
  Example: TDisplay::getRows() is needed by TScreen to know the display
rows, but a class (like TDesktop) should use TScreen::getRows(), it will
return a cached data that can additionally have some translation applied to
the value returned by TDisplay::getRows().@*
  It should be posible to split the physical screen in 4 different screens
handled by a special TScreen class. In this case TDisplay::getRows will
return a different value than TScreen, lets say the double. Then calling
TScreen::clearScreen will erase the current "screen" and not the whole
physical screen.@*
  JASC have the idea of making some of the members pointers that can be
changed by the hardware layers. I implemented it but with many
differences.@*
  
***************************************************************************/

class TDisplay
{
public:
 friend class TView;

 // Members defined by original TV library v2.0
 enum videoModes
 {
  smBW80    = 0x0002,
  smCO80    = 0x0003,
  smMono    = 0x0007,
  smFont8x8 = 0x0100
 };

 static void   (*clearScreen)(uchar w, uchar h);

 // SET: The following two are for compatibility, see *CursorShape
 static void     setCursorType(ushort val);
 static ushort   getCursorType();

 static ushort (*getRows)();
 static ushort (*getCols)();

 static void   (*setCrtMode)(ushort mode);
 static ushort (*getCrtMode)();

 // Not found in original TV 2.0
 // Robert added it to know the cursor position and restore it.
 static void   (*getCursorPos)(unsigned &x, unsigned &y); // old GetCursor
 static void   (*setCursorPos)(unsigned  x, unsigned  y); // ols SetCursor
 // SET: I moved it to the class
 // Returns 0 if the MSB of the background selects more colors.
 static int    (*getBlinkState)();

 // SET: These are the real functions for *CursorType
 static void   (*setCursorShape)(unsigned start, unsigned end);
 static void   (*getCursorShape)(unsigned &start, unsigned &end);
 // SET: This version sets the mode through an external program or other complex
 // mode setting.
 static void   (*setCrtModeExt)(char *mode);
 // SET: Fonts handling
 static int      SelectFont(int height, int noForce=0, int modeRecalculate=1, int width=8);
 static TFont   *GetFontHandler(void) { return font; }
 static void     SetFontHandler(TFont *f);
 static void     RestoreDefaultFont(void);
 // SET: Checks if screen size was changed by an external "force"
 static int    (*checkForWindowSize)(void);
 // SET: For windowed cases
 static
 const  char  *(*getWindowTitle)(void);
 static int    (*setWindowTitle)(const char *name);
 // This should be called before initialization.
 // Isn't mandatory but helps some drivers.
 static void     setArgv(int aArgc, char **aArgv, char **aEnvir);
 // Driver options
 static Boolean  setShowCursorEver(Boolean value);
 static Boolean  getShowCursorEver() { return opts1 & ShowCursorEver ? True : False; };
 
 // We must remove it
 static int dual_display;

protected:
 // SET: Those are more complex so now they aren't inline
 TDisplay();
 TDisplay(const TDisplay&);
 // We will use casts to base classes, destructors must be pointers
 virtual ~TDisplay();

 static TFont *font;
 // A copy of the command line arguments, old applications doesn't
 // provide it.
 static int    argc;
 static char **argv;
 static char **environ;
 // Special options
 enum Options1
 {
  ShowCursorEver=1  // The main goal of thi option is to help Braille Terminals
 };
 static uint32 opts1;

 // Hardware Palette handling routines and variables.
 // This is low level and should be called from TScreen.
 static void (*getDisPaletteColors)(int from, int number, TScreenColor *colors);
 static int  (*setDisPaletteColors)(int from, int number, TScreenColor *colors);
 // The shell/user palette
 static TScreenColor OriginalPalette[16];
 // The palette we are using
 static TScreenColor ActualPalette[16];
 // The PC BIOS palette
 static TScreenColor PC_BIOSPalette[16];
 // To know if the palette changed (set by high level routines)
 static char paletteModified;
 // Code page initializator
 static TVCodePage *codePage;
 
 // Default behaviors
 static void        defaultClearScreen(uchar, uchar);
 static ushort      defaultGetRows();
 static ushort      defaultGetCols();
 static void        defaultSetCrtMode(ushort mode);
 static ushort      defaultGetCrtMode();
 static void        defaultGetCursorPos(unsigned &x, unsigned &y);
 static void        defaultSetCursorPos(unsigned  x, unsigned  y);
 static void        defaultSetCursorShape(unsigned start, unsigned end);
 static void        defaultGetCursorShape(unsigned &start, unsigned &end);
 static void        defaultSetCrtModeExt(char *mode);
 static int         defaultCheckForWindowSize(void);
 static const char *defaultGetWindowTitle(void);
 static int         defaultSetWindowTitle(const char *name);
 static int         defaultGetBlinkState();
 static void        defaultGetDisPaletteColors(int from, int number, TScreenColor *colors);
 static int         defaultSetDisPaletteColors(int from, int number, TScreenColor *colors);

private:
 // From original TV 2.0.
 // As they are private I just ignore them.
 //static void videoInt();
 //static void updateIntlChars();

 // From where they came from? TV 1.03?
 //static ushort * equipment;
 //static uchar * crtInfo;
 //static uchar * crtRows;
 //static uchar Page;
};

// virtual to avoid problems with multiple inheritance
class TScreen : virtual public TDisplay
{
public:
 // From original TV 2.0
 TScreen();
 // We will use casts to base classes, destructors must be pointers
 virtual ~TScreen();

 static void   (*setVideoMode)(ushort mode);
 static void   (*clearScreen)();

 static ushort   startupMode;
 static ushort   startupCursor;
 static ushort   screenMode;
 static uchar    screenWidth;
 static uchar    screenHeight;
 // True if it have more than 25 lines (obsolete)
 static Boolean  hiResScreen;
 // True if we must be careful about writing to video screen (obsolete)
 static Boolean  checkSnow;
 // Current cursor shape
 static ushort   cursorLines;
 // Pointer to the screen arranged as Attribut/Character array.
 // This is optional and in some particular platform could be a NULL pointer.
 // SET: Changed to avoid problems, in fact some casts to int failed in
 // Alpha/Linux. We found it with Dim Zegebart.
 static uint16 *screenBuffer;

 static void   (*setCrtData)();
 static ushort (*fixCrtMode)(ushort mode);

 // SET: Currently they are wrappers that ensure we don't nest calls and
 // then calls the real members.
 static void     suspend();
 static void     resume();

 // SET: Real suspend and resume
 static void   (*Suspend)();
 static void   (*Resume)();

 // New functionality
 // Needed by some mouse handlers
 static ushort (*getCharacter)(unsigned offset);
 // Only used internally by Win32 ports
 static void   (*getCharacters)(unsigned offset, ushort *buf, unsigned count);
 // Used by TView and some mouse handlers, also internally
 static void   (*setCharacter)(unsigned offset, ushort value);
 static void   (*setCharacters)(unsigned offset, ushort *values, unsigned count);
 // SET: Used to set the video mode using an external program
 static void   (*setVideoModeExt)(char *mode);
 // SET: executes the indicated command
 static int    (*System)(const char *command, pid_t *pidChild=0);
 // Palette handling, they call the TDisplay members
 static void   getPaletteColors(int from, int number, TScreenColor *colors);
 static void   setPaletteColors(int from, int number, TScreenColor *colors);
 static void   resetPalette(); // Sets the original palette

 // SET: flags capabilities flags
 enum Capabilities1
 {
  CodePageVar=1,
  CanSetPalette=2,  // We can change colors
  CanReadPalette=4, // We have reliable information about the original colors.
                    // If this value isn't present the user can set the palette,
                    // but the original colors want be restored at exit. We will
                    // try to let them as default for the used display.
  PalNeedsRedraw=8  // Indicates we must redraw after changing the palette.
 };

 // Used internally to avoid nested calls to suspend/resume
 static char   suspended;
 // SET: Indicates if the screen was initialized by the TScreen constructor
 static char  initialized;

 // SET: It says if we should offer the user to select the code page
 // or the terminal have a fixed encoding and needs things as is.
 static Boolean codePageVariable() { return flags0 & CodePageVar ? True : False; };
 static Boolean canSetPalette()    { return flags0 & CanSetPalette ? True : False; };
 static Boolean canReadPalette()   { return flags0 & CanReadPalette ? True : False; };
 static Boolean palNeedsRedraw()   { return flags0 & PalNeedsRedraw ? True : False; };

protected:
 // SET: Capabilities flags
 static uint32 flags0;

 // SET: Default behaviors:
 static void   defaultSetVideoMode(ushort mode);
 static void   defaultSetVideoModeExt(char *mode);
 static void   defaultClearScreen();
 static void   defaultSetCrtData();
 static ushort defaultFixCrtMode(ushort mode);
 static ushort defaultGetCharacter(unsigned offset);
 static void   defaultGetCharacters(unsigned offset, ushort *buf, unsigned count);
 static void   defaultSetCharacter(unsigned offset, ushort value);
 static void   defaultSetCharacters(unsigned offset, ushort *values, unsigned count);
 static int    defaultSystem(const char *command, pid_t *pidChild);

 // The following members are used to implement a tricky initialization
 // process.
 // Externally it looks like a the old mechanism, you create a TScreen object
 // and you destroy it. Internally it detects the correct driver and creates
 // an object of this class, when you destroy the TScreen objects it destroys
 // the right class.
private:
 // SET: It indicates the class is initialized or at least trying to do it
 // It avoids problems with inheritance and calls to base classes.
 static char     initCalled;
 static TScreen *driver;
};

#if 0
// SET: Added the following functionallity:
// Linux specific stuff, enclose it in TVOSf_Linux section:
//   If you want to setup a specific value define this variable in your code
// and assign a value. If the variable isn't in your code the linker will use
// a definition found in the library.
extern unsigned TurboVision_screenOptions;

//   This value makes the screen driver try to provide a mode that supports
// a switch to the user screen.
//   Current example: if the TScreen class have access to the /dev/vcsaN
// devices for writing we can't provide user screen (no way to read the
// screen and tcsetattr or endwin() fails). Even with that it's better than
// using escape sequences, but RHIDE will work better if can switch. So I'm
// adding it so Robert can ask for it (or not).
const unsigned TurboVision_screenUserScreenNeeded=1;

// SET: Moved/Redefined
// Internally used by Linux driver, don't use
#endif
#endif  // Uses_TScreen

