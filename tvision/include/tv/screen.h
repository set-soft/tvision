/* TDisplay/TScreen header.
   Copyright (c) 2002-2003 by Salvador E. Tropea (SET)
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

// Components are 0-255
struct TScreenColor
{
 uchar R,G,B,Alpha;
};

struct TScreenFont256
{
 unsigned w,h;
 uchar *data;
};

struct TScreenResolution
{
 unsigned x,y;
};

// Type for the callback called when the driver needs a new font.
typedef TScreenFont256 *(*TVScreenFontRequestCallBack)(int which, unsigned w,
                         unsigned height);
// Type for the callback called when the driver is detected.
// This is called when we know which driver will be used but it isn't
// initilized yet.
typedef void (*TVScreenDriverDetectCallBack)();

const int TDisplayDOSModesNum=18;

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

class CLY_EXPORT TDisplay
{
public:
 friend class TView;

 // Members defined by original TV library v2.0
 enum videoModes
 {
  smBW80    = 0x0002,
  smCO80    = 0x0003,
  smCO80x25 = 0x0003,
  smMono    = 0x0007,
  smFont8x8 = 0x0100,
  // Modes defined by Robert
  // Extended
  smCO80x28 = 0x0103,
  smCO80x35 = 0x0203,
  smCO80x40 = 0x0303,
  smCO80x43 = 0x0403,
  smCO80x50 = 0x0503,
  // Tweaked
  smCO80x30 = 0x0703,
  smCO80x34 = 0x0803,
  smCO90x30 = 0x0903,
  smCO90x34 = 0x0A03,
  smCO94x30 = 0x0B03,
  smCO94x34 = 0x0C03,
  smCO82x25 = 0x0D03, // Created by SET to get 8x16 char cells
  // Common VESA text modes
  smCO80x60 = 0x0108,
  smCO132x25= 0x0109,
  smCO132x43= 0x010A,
  smCO132x50= 0x010B,
  smCO132x60= 0x010C
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
 // SET: A new and more rasonable way to choose a video mode
 static int    (*setCrtModeRes_p)(unsigned w, unsigned h, int fW, int fH);
 static int     setCrtModeRes(unsigned w, unsigned h, int fW=-1, int fH=-1)
                { return setCrtModeRes_p(w,h,fW,fH); };
 // SET: These are the real functions for *CursorType
 static void   (*setCursorShape)(unsigned start, unsigned end);
 static void   (*getCursorShape)(unsigned &start, unsigned &end);
 // SET: This version sets the mode through an external program or other complex
 // mode setting.
 static void   (*setCrtModeExt)(char *mode);
 // SET: Fonts handling. I changed it for a new approach, see TScreen
 // SET: Checks if screen size was changed by an external "force"
 static int    (*checkForWindowSize)(void);
 // SET: For windowed cases
 static
 const  char  *(*getWindowTitle)(void);
 static int    (*setWindowTitle)(const char *name);
 // SET: Roman Valyushenko propposed some mechanism no indicate the
 // application is busy
 static Boolean (*showBusyState)(Boolean state);
 // SET: The bell/beep is something really tied to the terminal
 static void   (*beep)();
 static void     bell() { beep(); };
 // This should be called before initialization.
 // Isn't mandatory but helps some drivers.
 static void     setArgv(int aArgc, char **aArgv, char **aEnvir);
 // Driver options
 static Boolean  setShowCursorEver(Boolean value);
 static Boolean  getShowCursorEver() { return opts1 & ShowCursorEver ? True : False; };
 static Boolean  setDontMoveHiddenCursor(Boolean value);
 static Boolean  getDontMoveHiddenCursor() { return opts1 & DontMoveHiddenCursor ? True : False; };
 // Helper to look for the closest resolution from a list
 static Boolean  searchClosestRes(TScreenResolution *res, unsigned x, unsigned y,
                                  unsigned cant, unsigned &pos);
 // Returns the default palette
 static const TScreenColor *getDefaultPalette() { return PC_BIOSPalette; }
 // Uses to be called when driver is detected but not yet initialized
 static TVScreenDriverDetectCallBack setDetectCallBack(TVScreenDriverDetectCallBack aCB);
 // Searchs information about a known DOS video mode
 static Boolean searchDOSModeInfo(ushort mode, unsigned &w, unsigned &h, int &fW, int &fH);
 // Used to know the format needed for buffers passed to setCharacters
 static int     getDrawingMode() { return drawingMode; }
 
 // Tables for the DOS video modes, used to look for similar modes by other drivers
 static TScreenResolution dosModesRes[];
 static TScreenResolution dosModesCell[];
 static int dosModes[];

 // Drawing modes:
 // codepage: cells are 8 bits, the first is the character in the current code
 //           page encoding and the next is the colors attribute.
 // unicode16: cells are 16 bits, the first is the 16 bits unicode value and
 //            the next is the colors attribute.
 enum { codepage=0, unicode16=1 };

 // This flag indicates the user wants to avoid characters that produce moire artifacts
 static char avoidMoire;

 // We must remove it
 static int dual_display;

protected:
 // SET: Those are more complex so now they aren't inline
 TDisplay();
 TDisplay(const TDisplay&);
 // We will use casts to base classes, destructors must be pointers
 virtual ~TDisplay();

 // A copy of the command line arguments, old applications doesn't
 // provide it.
 static int    argc;
 static char **argv;
 // Note: MinGW defines a macro called environ in stdlib.h so no C++
 // member can be named environ. It should be just _environ and the
 // internal name __environ. Is a serious namespace pollution.
 static char **environment;
 // Special options
 enum Options1
 {
  ShowCursorEver=1,  // The main goal of this option is to verify the help
                     // for Braille Terminals
  DontMoveHiddenCursor=2 // This option disables the help for Braille
                         // Terminals in favor of speed.
 };
 static uint32 opts1;

 // Helper to access TVCodePage and set the default values
 static void SetDefaultCodePages(int idScr, int idApp, int idInp);
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
 // This is the palette parsed from the tvrc file or the application
 static TScreenColor UserStartPalette[16];
 // Code page initializator
 static TVCodePage *codePage;
 // Initialization call back
 static TVScreenDriverDetectCallBack dCB;
 // Are we showing signs of busy state?
 static Boolean busyState;
 // Format used for the drawing calls (codepage, unicode16, etc.)
 static int     drawingMode;
 
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
 static int         defaultSetCrtModeRes(unsigned w, unsigned h, int fW=-1, int fH=-1);
 static Boolean     defaultShowBusyState(Boolean state);
 static void        defaultBeep();

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
class CLY_EXPORT TScreen : virtual public TDisplay
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
 static void   (*setCharacter)(unsigned offset, uint32 value);
 static void   (*setCharacters)(unsigned offset, ushort *values, unsigned count);
 // SET: Used to set the video mode using an external program
 static void   (*setVideoModeExt)(char *mode);
 // SET: Set the video mode using sizes.
 static int    (*setVideoModeRes_p)(unsigned w, unsigned h, int fW, int fH);
 static int      setVideoModeRes(unsigned w, unsigned h, int fW=-1, int fH=-1)
                 { return setVideoModeRes_p(w,h,fW,fH); };
 // SET: executes the indicated command
 static int    (*System_p)(const char *command, pid_t *pidChild, int in,
                           int out, int err);
 static int      System(const char *command, pid_t *pidChild=0, int in=-1,
                         int out=-1, int err=-1)
                 { return System_p(command,pidChild,in,out,err); };
 // Palette handling, they call the TDisplay members
 static void     getPaletteColors(int from, int number, TScreenColor *colors);
 static void     setPaletteColors(int from, int number, TScreenColor *colors);
 static void     resetPalette(); // Sets the original palette
 // Font handling
 // These funtions can fail and only drivers with CanSetFontSize should
 // implement theme. The rest uses defaults that ever fails.
 static int    (*getFontGeometry)(unsigned &w, unsigned &h);
 static int    (*getFontGeometryRange)(unsigned &wmin, unsigned &hmin,
                                       unsigned &umax, unsigned &hmax);
 // The following are implemented only if CanSetBFont and/or CanSetSBFont
 // are set.
 static int      setPrimaryFont(TScreenFont256 *font, int fontCP=-1, int appCP=-1)
                   { return setFont(1,font,0,NULL,fontCP,appCP); };
 static int      setSecondaryFont(TScreenFont256 *font)
                   { return setFont(0,NULL,1,font); };
 // That's the real function to set the fonts
 static int    (*setFont_p)(int changeP, TScreenFont256 *fontP,
                            int changeS, TScreenFont256 *fontS,
                            int fontCP, int appCP);
 static int    setFont(int changeP, TScreenFont256 *fontP,
                       int changeS, TScreenFont256 *fontS,
                       int fontCP=-1, int appCP=-1)
               { return setFont_p(changeP,fontP,changeS,fontS,fontCP,appCP); };
 static void   (*restoreFonts)();
 static TVScreenFontRequestCallBack
                 setFontRequestCallBack(TVScreenFontRequestCallBack cb);
 // Helpers:
 static int      disableSecondaryFont() { return setSecondaryFont(NULL); };
 static int      disablePrimaryFont()   { return setPrimaryFont(NULL); };
 static Boolean  isSecondaryFontEnabled() { return useSecondaryFont ? True : False; };
 // It looks for a configuration variable that belongs to the current driver
 static Boolean  optSearch(const char *variable, long &val);
 static char    *optSearch(const char *variable);
 static const char
                *getDriverShortName() { return currentDriverShortName; }
 static void     beep() { TDisplay::beep(); };
 static void     bell() { TDisplay::beep(); };
 // SET: Application Helpers
 enum AppHelper { FreeHandler, ImageViewer, PDFViewer };
 typedef ccIndex appHelperHandler;
 static appHelperHandler (*openHelperApp)(AppHelper kind);
 static Boolean (*closeHelperApp)(appHelperHandler id);
 static Boolean (*sendFileToHelper)(appHelperHandler id, const char *file, void *extra);
 static const char *(*getHelperAppError)();
 static int maxAppHelperHandlers;
 // SET: Windows class, for drivers that creates windows and can somehow tag them.
 // The X Windows driver can do it. Applications could overwrite it.
 static const char *windowClass;

 // SET: flags capabilities flags
 enum Capabilities1
 {
  CodePageVar=1,
  CanSetPalette=2,    // We can change colors
  CanReadPalette=4,   // We have reliable information about the original colors.
                      // If this value isn't present the user can set the palette,
                      // but the original colors want be restored at exit. We will
                      // try to let them as default for the used display.
  PalNeedsRedraw=8,   // Indicates we must redraw after changing the palette.
  CursorShapes=16,    // When the cursor shape is usable.
  UseScreenSaver=32,  // Does screen saver have any sense for this driver?
  CanSetBFont=64,     // We can set the primary bitmap font
  CanSetSBFont=128,   // We can set the secondary bitmap font
  CanSetFontSize=256, // We can set the width and height of the font
  CanSetVideoSize=512,// We can set the video size (window size or video mode)
  NoUserScreen=0x400  // No user screen exists i.e. we are in a window we draw
 };

 // Used internally to avoid nested calls to suspend/resume
 static char   suspended;
 // SET: Indicates if the screen was initialized by the TScreen constructor
 static char  initialized;

 // SET: It says if we should offer the user to select the code page
 // or the terminal have a fixed encoding and needs things as is.
 static Boolean codePageVariable() { return flags0 & CodePageVar     ? True : False; };
 static Boolean canSetPalette()    { return flags0 & CanSetPalette   ? True : False; };
 static Boolean canReadPalette()   { return flags0 & CanReadPalette  ? True : False; };
 static Boolean palNeedsRedraw()   { return flags0 & PalNeedsRedraw  ? True : False; };
 static Boolean cursorShapes()     { return flags0 & CursorShapes    ? True : False; };
 static Boolean useScreenSaver()   { return flags0 & UseScreenSaver  ? True : False; };
 static Boolean canSetBFont()      { return flags0 & CanSetBFont     ? True : False; };
 static Boolean canSetSBFont()     { return flags0 & CanSetSBFont    ? True : False; };
 static Boolean canSetFontSize()   { return flags0 & CanSetFontSize  ? True : False; };
 static Boolean canSetVideoSize()  { return flags0 & CanSetVideoSize ? True : False; };
 static Boolean noUserScreen()     { return flags0 & NoUserScreen    ? True : False; };

protected:
 // SET: Capabilities flags
 static uint32 flags0;
 static const char *currentDriverShortName;
 // Indicates if we must use the secondary font for the higher foreground
 // colors.
 static char      useSecondaryFont;
 // Font Request Call Back
 static TVScreenFontRequestCallBack frCB;
 // Values from the user configuration
 static long forcedAppCP, forcedScrCP, forcedInpCP;

 // Used to set the palette requested by the user/application
 static Boolean parseUserPalette();

 // SET: Default behaviors:
 static void   defaultSetVideoMode(ushort mode);
 static void   defaultSetVideoModeExt(char *mode);
 static int    defaultSetVideoModeRes(unsigned w, unsigned h, int fW=-1, int fH=-1);
 static void   defaultClearScreen();
 static void   defaultSetCrtData();
 static ushort defaultFixCrtMode(ushort mode);
 static ushort defaultGetCharacter(unsigned offset);
 static void   defaultGetCharacters(unsigned offset, ushort *buf, unsigned count);
 static void   defaultSetCharacter(unsigned offset, uint32 value);
 static void   defaultSetCharacters(unsigned offset, ushort *values, unsigned count);
 static int    defaultSystem(const char *command, pid_t *pidChild, int in, int out,
                             int err);
 static int    defaultGetFontGeometry(unsigned &w, unsigned &h);
 static int    defaultGetFontGeometryRange(unsigned &wmin, unsigned &hmin,
                                           unsigned &umax, unsigned &hmax);
 static int    defaultSetFont(int changeP, TScreenFont256 *fontP,
                              int changeS, TScreenFont256 *fontS,
                              int fontCP=-1, int appCP=-1);
 static void   defaultRestoreFonts();
 static appHelperHandler defaultOpenHelperApp(AppHelper kind);
 static Boolean defaultCloseHelperApp(appHelperHandler id);
 static Boolean defaultSendFileToHelper(appHelperHandler id, const char *file, void *extra);
 static const char *defaultGetHelperAppError();

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

