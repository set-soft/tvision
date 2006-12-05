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
 
#if    defined(TVOS_Win32) && !defined(TV_Disable_WinGr_Driver) \
   && !defined(WINGRSCR_HEADER_INCLUDED)
#define WINGRSCR_HEADER_INCLUDED


#ifdef TV_BIG_ENDIAN          /* Intel machines */
  #define attrChar(a)   ((a) >> 0x08)
  #define attrColor(a)  ((a) &  0xFF)
#else
  #define attrChar(a)   ((a) &  0xFF)
  #define attrColor(a)  ((a) >> 0x08)
#endif

#ifdef DEBUG
  #define PRINTDEBUG( w ) \
  { std::cout \
  << #w << " --> " << w  \
  << " ( " << __FILE__ <<  " : " << __LINE__ << " )\n";\
  std::cout.flush(); }
#else
  #define PRINTDEBUG( w )
#endif

typedef struct
{ HBITMAP bitmapRaster;
  HGDIOBJ bitmapObject;
  HDC     bitmapMemo;
  unsigned w, h;
} bitmapFontRec;

typedef struct
{ TDisplay::videoModes mode;
  ushort wScreen
       , hScreen
       , wFont
       , hFont;
} VideoModeData;


struct TDisplayWinGr : public virtual TDisplay // virtual to avoid problems with multiple inheritance
{ static  unsigned xPos;        /* Cursor pos                */
  static  unsigned yPos;        /* Cursor pos                */
  static  unsigned zPos;        /* Cursor size               */
  static       HDC hdc;         /* Context used for drawing  */
  static      RECT wGeo;        /* Window position and size  */
  static    char * className;   /* Make the classname into a global variable */
  static HINSTANCE TvWinInstance;

/*
 * To store display limits
 */

  static  unsigned  dmPelsWidth ;
  static  unsigned  dmPelsHeight;

/*
 * To store font limits
 */

  static int foWmin;
  static int foHmin;
  static int foWmax;
  static int foHmax;

  static void   Init();   /* Sets pointers of TDisplay to point to this class */

  static void lowSetCursor( int x
                          , int y
                          , bool dir );

  static int testEvents( UINT   message
                       , WPARAM wParam
                       , LPARAM lParam );


protected:

  static bool processEvent(  );

  static void testCreate( HWND hw
                        , LPARAM lParam );

 
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
 static int         SetCrtModeRes( unsigned w, unsigned h );
 static const char *GetWindowTitle(void);
 static int         SetWindowTitle(const char *name);
 static int         CheckForWindowSize( );
 static void        Beep();


// Support functions
 
  static void TestAllFonts( unsigned fW
                          , unsigned fH );

  static  int SetFontCrt( bitmapFontRec & fontResource
                       , ushort w, ushort h );
                       
  static  int SetFontMode( bitmapFontRec  & fr, ushort newMode );

  static  int selectFont( bitmapFontRec  & fontResource
                       , TScreenFont256 * fontData );
 
  static  int CheckWindowSize(  RECT & windowArea  );
  static void winRecalc();
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

 static          uchar shapeFont8x16[];
 static          uchar shapeFont10x20[];
  
 static TScreenFont256 font8x16;  
 static TScreenFont256 font10x20;
 static TScreenFont256 * defaultFont;

/* 
 *  28/1/2006, new bitmap fonts  
 */ 
 
  static VideoModeData videoModes[];

/* 
 *  10/3/2006, draw 'n' drop support
 */ 
  static char   dragDropName[ PATH_MAX ];
  static int    dragDropIdx;
  static void * dragDropHnd;
  

};




// With this order the destructor will be called first for TScreenWin32,
// TScreen, TDisplayWin32 and finally TDisplay.

struct TScreenWinGr: public virtual TDisplayWinGr
                   , public         TScreen
{ static   int amountOfCells;    /* Allocated screen cells */
  static DWORD style;            /* Window style           */
  static DWORD exStyle;          /* Window new styles      */
  static long HalfColor;
  static long FullColor;
  static long cursorDelay;


  TScreenWinGr();                /* We will use casts to base classes, destructors must be pointers */

  static void  Init();

  virtual ~TScreenWinGr();

protected:


 // Default: ushort fixCrtMode( ushort mode )
 // Default: static void   setCrtData();
 // Default: static void   setVideoModeExt(char *mode);
 // Default: static void   getCharacters(unsigned offset,ushort *buf,unsigned count);
 // Default: static ushort getCharacter(unsigned dst);
 
  static int   SetDisPaletteColors( int f, int n, TScreenColor *  );
  static int   SetVideoModeRes( unsigned w, unsigned h, int fW, int fH );
  static void  setVideoMode(ushort mode);
  static void  Resume();
  static void  Suspend();
  static void  setCharacter( unsigned offset
                           , uint32 value );

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
 
 // Support functions

  static void SaveScreen();
  static void RestoreScreen();
  static void resizeMemoryBuffer();

public:

  static void  writeLine( unsigned x
                        , unsigned y
                        , int w
                        , char * str
                        , uchar color );


// Support variables

protected: 

};

/*
 * This function is called by the Windowsfunction DispatchMessage( )
 */

LRESULT CALLBACK WindowProcedure( HWND hwnd
                                , UINT message
                                , WPARAM wParam
                                , LPARAM lParam );


#endif // WINDOWSSCR_HEADER_INCLUDED

