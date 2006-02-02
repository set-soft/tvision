/*
 * Windows GUI screen routines header.
 *
 *  Copyright (c) 2002 by José Angel Sánchez Caso (JASC)  
 *  Copyright (c) 2002 by Salvador E. Tropea (SET)
 * 
 *  From SET code fragment win 32 driver.
 *
 *  Covered by the GPL license. 
 */
 
#if defined(TVOS_Win32) && !defined(TV_Disable_WinGr_Driver) \
    && !defined(WINGRSCR_HEADER_INCLUDED)
#define WINGRSCR_HEADER_INCLUDED


#if    defined( TV_BIG_ENDIAN )         /* Intel machines */
  #define attrChar(a)   ((a) >> 0x08)
  #define attrColor(a)  ((a) &  0xFF)
#else
  #define attrChar(a)   ((a) &  0xFF)
  #define attrColor(a)  ((a) >> 0x08)
#endif

#include <tv/win32/win32clip.h>

typedef struct
{ HBITMAP bitmapRaster;
  HGDIOBJ bitmapObject;
  HDC     bitmapMemo;
  unsigned w, h;
} bitmapFontRec;

struct TDisplayWinGr :  public virtual TDisplay // virtual to avoid problems with multiple inheritance
{ static bool processEvent(  );

  static void testCreate( HWND hw
                        , LPARAM lParam );

  static int testEvents( UINT   message
                       , WPARAM wParam
                       , LPARAM lParam );

 static unsigned xPos;       /* Cursor pos        */
 static unsigned yPos;       /* Cursor pos        */
 static unsigned zPos;       /* Cursor size       */
 static HDC       hdc;       /* A device context used for drawing */

 static void   Init();   /* Sets pointers of TDisplay to point to this class */

 static void lowSetCursor( int x
                         , int y
                         , bool dir );


protected:
 TDisplayWinGr() {};
 virtual ~TDisplayWinGr();  // We will use casts to base classes, destructors must be pointers


 // Behaviors to replace TDisplay

 static void   SetCursorPos  ( unsigned x     , unsigned y   );
 static void   GetCursorPos  ( unsigned &x    , unsigned &y  );
 static void   GetCursorShape( unsigned &start, unsigned &end);
 static void   SetCursorShape( unsigned start , unsigned end );
 static ushort GetRows();
 static ushort GetCols();
 
 static ushort      GetCrtMode();
 static void        SetCrtMode(ushort);
 static void        SetCrtModeExt(char *);
 static const char *GetWindowTitle(void);
 static int         SetWindowTitle(const char *name);
 static int         CheckForWindowSize(void);
 static void        SetFontMode( int fontIdx );
 static void        Beep();


// Support functions
 

 static void winRecalc(  );

 static void SetCrtMode( const char * );

// Support variables

 static int        sizeChanged; /* Resizing counter  */
 static int        forceRedraw; /* Used to force setCharacters to unconditionally draw */

 static TEvent storedEvent; /* Correctly processes message queue */

public:
 static HWND       hwnd;

 static COLORREF   colorMap[ 16 ];

 static HCURSOR normCursor;
 static HCURSOR sizeCursor;
 static HCURSOR handCursor;

/* static RECT mSize;       ( now local ) Windows resizing calculations */

 static char cShapeFr;
 static char cShapeTo;

/* 
 *  28/1/2006, new bitmap fonts  
 */ 
 static bitmapFontRec primary;
 static bitmapFontRec secondary;

};







// With this order the destructor will be called first for TScreenWin32,
// TScreen, TDisplayWin32 and finally TDisplay.

struct TScreenWinGr: public virtual TDisplayWinGr
                   , public         TScreen
                   , public         TVWin32Clipboard
{ TScreenWinGr();                    // We will use casts to base classes, destructors must be pointers

  static void  Init();

  virtual ~TScreenWinGr();

protected:


 // Default: ushort fixCrtMode( ushort mode )
 // Default: static void   setCrtData();
 // Default: static void   setVideoModeExt(char *mode);
 // Default: static void   getCharacters(unsigned offset,ushort *buf,unsigned count);
 // Default: static ushort getCharacter(unsigned dst);
 
 static void   setVideoMode(ushort mode);
 static void   Resume();
 static void   Suspend();
 static void   clearScreen();
 static void   setCharacter( unsigned offset
                           , ushort value );

 static void   setCharacters( unsigned dst
                            , ushort  *src
                            , unsigned len );

  static int   System( const char *command
                     , pid_t *pidChild
                     , int in
                     , int out
                     , int err);

  static int   SetFont( int changeP, TScreenFont256 *fontP
                      , int changeS, TScreenFont256 *fontS
                      , int fontCP, int appCP );
 
  static void  RestoreFonts();
  
  static int GetFontGeometry(unsigned &w, unsigned &h);
  static int GetFontGeometryRange( unsigned &wmin, unsigned &hmin
                                 , unsigned &wmax, unsigned &hmax );
 
  static int SetCrtModeRes( unsigned w
                          , unsigned h
                          , int fW
                          , int fH );
 
 // Support functions

  static int TScreenWinGr::selectFont( bitmapFontRec  & fontResource 
                                     , TScreenFont256 * fontData );
 
  static void  SaveScreen();
  static void  SaveScreenReleaseMemory();
  static void  RestoreScreen();
  static void  Done();

 public:
 static void  writeLine( unsigned x
                       , unsigned y
                       , int w
                       , char * str
                       , uchar color );


// Support variables

protected: 

/* 
 *  28/1/2006, new bitmap fonts  
 */ 
 
 static          uchar shapeFont8x16[];
 static          uchar shapeFont10x20[];
  
 static TScreenFont256 font8x16;  
 static TScreenFont256 font10x20;
 static TScreenFont256 * defaultFont;

};

   LRESULT CALLBACK WindowProcedure( HWND hwnd           /* This function is called by the Windowsfunction DispatchMessage( ) */
                                   , UINT message
                                   , WPARAM wParam
                                   , LPARAM lParam );


#endif // WINDOWSSCR_HEADER_INCLUDED

