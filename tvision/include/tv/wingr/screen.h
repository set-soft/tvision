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


typedef struct
{ short font;
  short mode;
} dialogModeRec;



struct TDisplayWinGr :  public virtual TDisplay // virtual to avoid problems with multiple inheritance
{ static bool processEvent(  );

  static void testCreate( HWND hw
			, LPARAM lParam );

  static int testEvents( UINT   message
                       , WPARAM wParam
		       , LPARAM lParam );

 static unsigned xPos;        /* Cursor pos        */
 static unsigned yPos;        /* Cursor pos        */
 static unsigned zPos;        /* Cursor size       */
 static HDC        hdc;  /* A device context used for drawing */
 static LOGFONTA * fonts; // To hold available fonts

 static void   Init();        // This sets the pointers of TDisplay to point to this class

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
 

 static void winRecalc( int w
		      , int h  );

 static void SetCrtMode( const char * );

// Support variables

 static int        sizeChanged; /* Resizing counter  */
 static int        forceRedraw; /* Used to force setCharacters to unconditionally draw */

 static TEvent storedEvent; /* Correctly processes message queue */
 static HFONT      hFont;

public:
 static HWND       hwnd;

 static COLORREF   colorMap[ 16 ];
 static TEXTMETRIC tm;

 static HCURSOR normCursor;
 static HCURSOR sizeCursor;
 static HCURSOR handCursor;

 static RECT mSize;             /* Windows resizing calculations */

 static char cShapeFr;
 static char cShapeTo;

 static dialogModeRec mode;



};







// With this order the destructor will be called first for TScreenWin32,
// TScreen, TDisplayWin32 and finally TDisplay.

struct TScreenWinGr: public virtual TDisplayWinGr
		   , public         TScreen
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

 static int    System(const char *command
                            , pid_t *pidChild
                            , int in
                            , int out
                            , int err);

 // Support functions
 
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
 

};

   LRESULT CALLBACK WindowProcedure( HWND hwnd           /* This function is called by the Windowsfunction DispatchMessage( ) */
				   , UINT message
                                   , WPARAM wParam
				   , LPARAM lParam );


#endif // WINDOWSSCR_HEADER_INCLUDED

