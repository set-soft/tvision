/**[txh]********************************************************************

  Copyright (c) 2002 by Jose Angel Sánchez Caso (JASC)
  
    -> altomaltes@yahoo.es
    -> altomaltes@gmail.com
    
  Squeletons by Salvador E. Tropea (SET)

  Description:
  Windows Screen routines.


  Defaults loaded from resource file tvrc or .tvrc

    HalfColor     ->  Palette brightness
    FullColor
    
    ScreenWidth   -> Screen font and size
    ScreenHeight
    FontWidth
    FontHeight
    Font10x20
    
    optSearch
    optSearch
    optSearch


***************************************************************************/


//#define DEBUG

#include <tv/configtv.h>

#define Uses_string
#define Uses_stdlib
#define Uses_unistd
#define Uses_AllocLocal
#define Uses_TScreen
#define Uses_TEvent
#define Uses_TGKey
#define Uses_TVCodePage
#define Uses_alloca
#define Uses_TVOSClipboard
#include <tv.h>

#include <tv/win32/win32clip.h>


#if defined(TVOS_Win32) && !defined(TV_Disable_WinGr_Driver)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <tv/wingr/screen.h>
#include <tv/wingr/mouse.h>
#include <tv/wingr/key.h>

/* Not always defined */
#ifndef IDC_ARROW
 #define IDC_ARROW MAKEINTRESOURCE(32512)
#endif
#ifndef IDC_HAND
 #define IDC_HAND MAKEINTRESOURCE(32649)
#endif
#ifndef IDC_SIZENWSE
 #define IDC_SIZENWSE MAKEINTRESOURCE(32642)
#endif


int TScreenWinGr::amountOfCells= 0;    /* Allocated screen cells  */

DWORD TScreenWinGr::style= WS_CAPTION       /* Window style  */
                         | WS_SYSMENU
                         | WS_MAXIMIZEBOX
                         | WS_MINIMIZEBOX;

/* Window new styles  */

DWORD TScreenWinGr::exStyle= WS_EX_ACCEPTFILES  // Drag 'n' drop files
//                           | WS_EX_CLIENTEDGE  // Raised edge
                           ;

long TScreenWinGr::cursorDelay= 300000;
long TScreenWinGr::HalfColor= 0xB0;
long TScreenWinGr::FullColor= 0xFF;


/*
 *
 */

int TScreenWinGr::SetFont( int changeP, TScreenFont256 *fontP
                         , int changeS, TScreenFont256 *fontS
                         , int fontCP, int appCP )
{ if (!changeP && !changeS) 
  { return 1;
  }
  
  if ( !fontP )
  { fontP= defaultFont; 
  }
                         
  if ( !fontS )
  { fontS= defaultFont;  
  }
  if ( changeP )
  { selectFont( primary, fontP );
  }

  if ( changeS )
  { selectFont( secondary, fontS );
  }

  if ( appCP==-1 )
  { TVCodePage::SetScreenCodePage( fontCP );
  }
  else
  { TVCodePage::SetCodePage( appCP
                           , fontCP
                           ,-1 );
  }                       
  
  return( 1 );
}                         

/*
 *
 */

void TScreenWinGr::RestoreFonts()
{ SetFont( 1, NULL
         , 1, NULL
         , TVCodePage::ISOLatin1Linux
         , TVCodePage::ISOLatin1Linux); 
}


/*
 *
 */

int TScreenWinGr::GetFontGeometry(unsigned &w, unsigned &h)
{ w= primary.w;
  h= primary.h;
  return 1;
}

/*
 *
 */

int TScreenWinGr::GetFontGeometryRange( unsigned &wmin, unsigned &hmin
                                      , unsigned &wmax, unsigned &hmax )
{ wmin= foWmin;
  hmin= foHmin;
  wmax= foWmax;
  hmax= foHmax;
  return 1;
}


/*
 *  Time callback to blink the cursor
 */ 
VOID CALLBACK cursorProc( HWND hwnd          // handle of window for timer messages
                        , UINT uMsg          // WM_TIMER message
                        , UINT idEvent       // timer identifier
                        , DWORD /*dwTime*/ ) // current system time
{ if ( uMsg )
  { TScreenWinGr::zPos ^= 0x80;         /* Toggle phase (turn 180 degrees) */

    if ( TScreenWinGr::xPos >= 0 )
    { TScreenWinGr::lowSetCursor( TScreenWinGr::xPos /* Show or hide cursor */
                                , TScreenWinGr::yPos
                                , true ); 
    }                                 
  }
  
  SetTimer( hwnd                 /* Reload timer        */ 
          , idEvent
          , TScreenWinGr::cursorDelay / 1000   /* from usecs to msecs */
          , cursorProc );
}


/*
 *  JASC, 2002
 *
 *   Make a windows screen
 */
TScreenWinGr::TScreenWinGr()
{ WNDCLASSEX  wincl;            /* Datastructure for the windowclass */
  STARTUPINFO startinfo;
  RECT windowArea;

  TvWinInstance= GetModuleHandle (NULL);

#ifndef DEBUG                   /* Use the console to debug messages */
  FreeConsole();
#endif

  GetStartupInfoA (&startinfo);   /* Get the command line passed to the process. */

/*
 *  JASC, ene 2006, now bitmap fonts 
 */  


/* Look for defaults */

  optSearch("HalfColor", HalfColor );  /* Palette brightness */
  optSearch("FullColor", FullColor );

/*
 *  Screen font and size
 */

  long maxX=  80; optSearch("ScreenWidth" , maxX  );
  long maxY=  25; optSearch("ScreenHeight", maxY  );
  long fontW=  8; optSearch("FontWidth"   , fontW );
  long fontH= 16; optSearch("FontHeight"  , fontH );

  long aux; if ( optSearch( "Font10x20", aux ) && aux )
  { fontW= 10, fontH= 20;
  }

/*
 *  Get and store the device size
 *
 */

  DEVMODE ret;
  ret.dmSize= sizeof( ret );
  EnumDisplaySettings( NULL
                     , ENUM_CURRENT_SETTINGS
                     , &ret );

  dmPelsWidth = ret.dmPelsWidth ;
  dmPelsHeight= ret.dmPelsHeight;
  screenWidth = maxX;
  screenHeight= maxY;

  TestAllFonts( fontW      /* Try the best fit */
              , fontH );
  CheckWindowSize( windowArea );
  
/* 
 * Code page, User settings have more priority than detected settings 
 */
  optSearch("AppCP", forcedAppCP );
  optSearch("ScrCP", forcedScrCP );
  optSearch("InpCP", forcedInpCP );

  codePage=
    new TVCodePage( forcedAppCP != -1 ? forcedAppCP : TVCodePage::ISOLatin1Linux
                  , forcedScrCP != -1 ? forcedScrCP : TVCodePage::ISOLatin1Linux
                  , forcedInpCP != -1 ? forcedInpCP : TVCodePage::ISOLatin1Linux );
                 
  SetDefaultCodePages( TVCodePage::ISOLatin1Linux
                     , TVCodePage::ISOLatin1Linux
                     , TVCodePage::ISOLatin1Linux );

  normCursor= LoadCursor( NULL, IDC_ARROW   );
  handCursor= LoadCursor( NULL, IDC_HAND    );
  
  if( handCursor==NULL )                        /* Not available in 95 and NT */
  { handCursor= LoadCursor(NULL, IDC_ARROW  );
  }
  sizeCursor= LoadCursor(NULL, IDC_SIZENWSE );

  wincl.hIcon  = LoadIcon( NULL              /* Use default icon and mousepointer */
                         , IDI_APPLICATION);
                         
  wincl.hIconSm= LoadIcon( NULL
                         , IDI_APPLICATION);

  wincl.style        = 0;                 /* no CS_DBLCLKS (Catch dbl-clicks) */
  wincl.cbSize       = sizeof(WNDCLASSEX);
  wincl.hCursor      = normCursor;
  wincl.hInstance    = TvWinInstance;
  wincl.cbClsExtra   = 0;                /* No extra bytes after window class  */
  wincl.cbWndExtra   = 0;                /* structure or the window instance   */
  wincl.lpfnWndProc  = WindowProcedure;  /* This function is called by windows */
  wincl.lpszMenuName = NULL;             /* No menu                            */
  wincl.lpszClassName= className;
  wincl.hbrBackground= NULL;

/* 
 * Register the window class, if fail quit the program 
 */
 
  if( !RegisterClassEx( &wincl ) )
  { return; 
  }


/* 
 * The class is registered, lets create a program 
 */
  hwnd= CreateWindowEx
  ( exStyle               /* Possible styles                        */
  , className             /* Classname ("MyLittleWindow")           */
  , className             /* Title Text                             */
  , style                 /* defaultwindow                          */
  , CW_USEDEFAULT         /* Window dimensions                      */
  , CW_USEDEFAULT
  , windowArea.right
  , windowArea.bottom
  , HWND_DESKTOP          /* The window is a childwindow to desktop */
  , NULL                  /* No menu                                */
  , TvWinInstance         /* Program Instance handler               */
  , NULL );               /* No Window Creation data                */
  
  if ( ! hwnd )           /* Fails to create (font creation assigns) */
  { return; 
  }

/*
 * Store the window geometry and client size
 */

  hdc= GetDC( hwnd );

  /*
   *  Now we have an hwnd, and can create fonts
   */

  initialized++ ; 

  ShowWindow( hwnd                /* Make the window visible on the screen  */
            ,  startinfo.dwFlags 
            & STARTF_USESHOWWINDOW 
            ? startinfo.wShowWindow
            : SW_SHOWDEFAULT );

  for ( int col= 0
      ; col<16
      ; col++ )
  { colorMap[ col ]= RGB( (col&4) ? (col&8) ? FullColor:HalfColor:0
                        , (col&2) ? (col&8) ? FullColor:HalfColor:0
                        , (col&1) ? (col&8) ? FullColor:HalfColor:0 );
  }

  flags0= CodePageVar
        | CursorShapes
        | CanSetBFont   
        | CanSetSBFont
        | NoUserScreen
        | CanSetVideoSize
        | CanSetFontSize
        | CanSetPalette
        | CanReadPalette;
 
     
  cursorLines=
    startupCursor=
     getCursorType();


  screenMode= startupMode= smCO80;


/*
 * Start cursor blinking handler  
 */

  cursorProc( hwnd     // handle of window for timer messages
            , 0        // Don´t draw cursor
            , 0        // timer identifier
            , 0 );     // current system time
            
}            



/* 
 *    JASC, 2002
 *
 *  Link the driver by loading the static method pointers
 */ 
void TScreenWinGr::Init()
{ TScreen::System_p            = System;
  TScreen::Resume              = Resume;
  TScreen::Suspend             = Suspend;
  TScreen::setFont_p           = SetFont;
  TScreen::setCharacter        = setCharacter;
  TScreen::setCharacters       = setCharacters;
  TScreen::setVideoMode        = setVideoMode;
  TScreen::setVideoModeRes_p   = SetVideoModeRes;
  TScreen::getFontGeometry     = GetFontGeometry;
  TScreen::restoreFonts        = RestoreFonts;
  TScreen::setDisPaletteColors = SetDisPaletteColors;
  TScreen::getFontGeometryRange= GetFontGeometryRange;

  TVWin32Clipboard::Init();           /* Use the windows clipboard */
  setVideoModeRes( 0xFFFF
                 , 0xFFFF
                 , -1, -1 );
}

/*
 *
 */
void TScreenWinGr::Resume()
{ SaveScreen(); 
}

/*
 *
 */
void TScreenWinGr::Suspend()
{ RestoreScreen(); 
}

/*
 *  Free screen resources
 */
TScreenWinGr::~TScreenWinGr()
{ UnregisterClass( className         /* Free class resources */
                 , TvWinInstance );
  DestroyWindow  ( hwnd   );         /* Free the window      */

  if ( screenBuffer )
  { delete screenBuffer;
    screenBuffer= NULL; 
  }
}

/*

 *  JASC jul/2002, write a unique colored line
 */
void TScreenWinGr::writeLine( unsigned x
                            , unsigned y
                            , int  w
                            , char * org
                            , uchar color )
{ unsigned letter;
  uchar * str= (uchar *) org;

  if ( w<=0 )   // Nothing to do
  { return; 
  }

  SetBkColor  ( hdc, colorMap[ color&15 ]);     // Color de fondo
  SetTextColor( hdc, colorMap[ color>>4 ]);     // Color de fondo
    
  while( w -- )
  { letter= *str++;  /* Next character */
    BitBlt( hdc
          , x ++ * primary.w   /* Move right */
          , y    * primary.h
          , primary.w
          , primary.h 

          , useSecondaryFont && ( color  & 0x8 )
          ? secondary.bitmapMemo
          : primary.bitmapMemo
          
          , 0, letter * primary.h
          , SRCCOPY );
  }        
                             
  if ( y==yPos )
  { if ( xPos >= x)
    { if ( xPos<= x+w )
      { if ( xPos >= 0 )
        { lowSetCursor( xPos
                      , yPos
                      , true );
        }
      }
    }
  }
}
           
              
/*
 *  JASC jul/2002
 *
 *  Write a unique colored line to memory and screen
 */
void TScreenWinGr::setCharacters( unsigned  offset
                                , ushort  * src
                                , unsigned  len )
{ uchar letra;
  uchar color;
  int add  ;
  int last ;

  ushort x= offset % screenWidth;  // Regenerate cartesian coordinates
  ushort y= offset / screenWidth;

  ushort *old = screenBuffer + offset;
  ushort *old_right = old + len - 1;
  ushort *src_right = src + len - 1;

  char * dst;

  if ( !len )
  { return; 
  }

/* 
 * remove unchanged characters from left to right 
 */
  if ( !forceRedraw )
  { while (len > 0 && *old == *src)   
    { x++; offset++; old++; src++; len--; 
    }

/* 
 * remove unchanged characters from right to left 
 */
    while (len > 0 && *old_right == *src_right) 
    { old_right--; src_right--; len--;     
    }
  }

  AllocLocalStr(tmp,len); /* write only middle changed characters */
  dst= tmp; add= 0; last= -1;

  memcpy( old                /* Copy to screen buffer */     
        , src
        , len*2 );

  while(len--)              /* Iterate                             */
  { letra= attrChar(*src);  /* JASC, are macros (endian dependent) */
    color= attrColor(*src);
    
    if ( color != last )
    { if (last>=0)
      { writeLine( x
                 , y
                 , add
                 , tmp
                 , last );  /* Print last same color block */
        dst= tmp; x+= add; add= 0; }
        last= color; }
    *dst= letra;
    dst++; src++; add++; }

  if (!add)
  { return; }
  
  writeLine( x
           , y
           , add
           , tmp
           , last );        /* Print last block */
}


 
/*
 *
 */
void TScreenWinGr::setCharacter(unsigned offset, uint32 value)
{ setCharacters( offset
               , (ushort *)&value
               , 1 );
}

/*
 *
 */
void TScreenWinGr::SaveScreen()
{ 
}

/*
 *
 */
void TScreenWinGr::RestoreScreen()
{ 
}

/*
 *
 */
int TScreenWinGr::SetDisPaletteColors( int from
                                     , int number
                                     , TScreenColor *colors )
{ int to;

/*
 *  Some boundaries cheching
 */
  from= from <  0 ? 0 : from;
  to  = from+number;
  to  = to   > 16 ? 16: to;
  number= 0;                 /* Now is a counter */

  while( from < to )
  { colorMap[ from++ ]= RGB( colors[ from ].R
                           , colors[ from ].G
                           , colors[ from ].B );
    number ++;
  }

 return( number );
}



/*
 *    JASC, 2006
 *
 *    Change the amount of memory for cells. Avoid heap fragmentation, only
 *  realloqing if more memory requested.
 */
void TScreenWinGr::resizeMemoryBuffer()
{ int newAmountOfCells= getRows()*getCols();

  if ( newAmountOfCells > amountOfCells )
  { if ( screenBuffer )
    { free( screenBuffer );
    }
    amountOfCells= newAmountOfCells;
    screenBuffer= ( ushort *) calloc( sizeof(ushort) * 10
                                    , amountOfCells );
  }
  else
  { memset( screenBuffer, 0, sizeof(ushort)*amountOfCells );
  }
}


/*
 *  JASC, 2002
 *
 *    Changes the video mode, both hardware ( resolution ) and software
 *  ( resources ) layers. Only realocates memory on growing, avoiding heap
 *  fragmentation.
 */
void TScreenWinGr::setVideoMode( ushort mode )
{ setCrtMode( mode );
  resizeMemoryBuffer();
  setCrtData();                                 /* Do the hardware layer */
  winRecalc();
}

int TScreenWinGr::SetVideoModeRes( unsigned w
                                 , unsigned h
                                 , int fW
                                 , int fH )
{ if ( w != 0xFFFF )
  { screenWidth=  w;
  }

  if ( h != 0xFFFF )
  { screenHeight= h;
  }

  if ( fW == -1 )
  { fW=  primary.w;
  }

  if ( fH == -1 )
  { fH=  primary.h;
  }

  SetCrtModeRes( fW, fH );
  resizeMemoryBuffer();
  winRecalc();
  return( 1 );
}

/*                                                                         #####
 *    JASC, feb 2006                                                          ##
 *                                                                            ##
 *      Execute an external program in a syncronous way redirecting input,    ##
 *  output and error.                                                         ##
 *                                                                            ##
 *   Originally, From SET code, in a MSDOS or UNIX way.                       ##
 *  Now in a windows way.                                                  #####
 */
int TScreenWinGr::System( const char * command
                        , pid_t      * pidChild
                        , int in
                        , int out
                        , int err )
{ PROCESS_INFORMATION pi;
  STARTUPINFO si;
  char cmdLine[ 4096 ];

  memset(&si, 0, sizeof(si));
  memset(&pi, 0, sizeof(pi));
  si.cb = sizeof(si);

// If the caller asks for redirection replace the requested handles

  if ( pidChild )
  { si.hStdInput = GetStdHandle( STD_INPUT_HANDLE  );  // Default values
    si.hStdOutput= GetStdHandle( STD_OUTPUT_HANDLE );
    si.hStdError = GetStdHandle( STD_ERROR_HANDLE  );

    if ( in != -1 )
    { si.dwFlags|= STARTF_USESTDHANDLES; // Use new handles
      si.hStdInput = (HANDLE)_get_osfhandle( in  );
    }
  
    if ( out != -1 )
    { si.dwFlags|= STARTF_USESTDHANDLES; // Use new handles
      si.hStdOutput= (HANDLE)_get_osfhandle( out );
    }
  
    if ( err != -1 )
    { si.dwFlags|= STARTF_USESTDHANDLES; // Use new handles
      si.hStdError = (HANDLE)_get_osfhandle( err );
    }
  }

  sprintf( cmdLine
         , "/C %s"
         , command );

  if ( !CreateProcess( getenv( "COMSPEC" ) // Do it trough a shell (scrips, etc)
                     , cmdLine             // pointer to command line string
                     , NULL                // pointer to process security attributes
                     , NULL                // pointer to thread security attributes
                     , true                // handle inheritance flag
                     , 0                   // creation flags
                     , NULL                // pointer to new environment block
                     , NULL                // pointer to current directory name
                     , &si
                     , &pi ))
  { return( -1 );                          // Error
  }

  if ( pidChild )                      // Leave in the background
  { return( *pidChild= (int)pi.hProcess );  // Give process identifier
  }
  else
  { WaitForSingleObject                // DO NOT leave in the background
    ( pi.hProcess
    , INFINITE );
    return( NULL );                    // Return task done
  }


}

/*                                                                        #####
 *   JASC 2002                                                               ##
 *                                                                           ##
 *                                                                        #####
 */
TScreen * TV_WinGrDriverCheck()
{ TScreenWinGr *drv=new TScreenWinGr();

  if ( !TScreen::initialized )
  { delete drv;
    drv= NULL; 
  }

 TDisplayWinGr::Init();  /* First */
    TGKeyWinGr::init();
  TScreenWinGr::Init();
 THWMouseWinGr::init();

 return( drv );
}

#else

#include <tv/wingr/screen.h>
#include <tv/wingr/mouse.h>
#include <tv/wingr/key.h>

#endif // TVOS_Win32

