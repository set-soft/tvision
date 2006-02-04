/**[txh]********************************************************************

  Copyright (c) 2002 by Jose Angel Sánchez Caso (JASC)
  
    -> altomaltes@yahoo.es
    -> altomaltes@gmail.com
    
  Squeletons by Salvador E. Tropea (SET)

  Description:
  Windows Screen routines.

***************************************************************************/
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
#include <tv.h>


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

const int cursorDelay= 300000;

const int halfCOLOR= 0xC0;
const int fullCOLOR= 0xFF;


static char * className= "TVISION for windows"; /* Make the classname into a global variable */

int TScreenWinGr::amountOfCells= 0;    /* Allocated screen cells  */


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
{ const unsigned foWmin=4
               , foHmin=6
               , foWmax=10
               , foHmax=20;                      
  wmin= foWmin;
  hmin= foHmin;
  wmax= foWmax;
  hmax= foHmax;
  return 1;
}


/*
 *  Time callback to blink the cursor
 */ 
VOID CALLBACK cursorProc( HWND hwnd      // handle of window for timer messages
                        , UINT uMsg      // WM_TIMER message
                        , UINT idEvent    // timer identifier
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
          , cursorDelay / 1000   /* from usecs to msecs */
          , cursorProc );
}


/*
 *  Make a windows screen
 */

 void xxx()
 {      int i;
  i ++;
 }

 

TScreenWinGr::TScreenWinGr()
{ FreeConsole();
  WNDCLASSEX  wincl;            /* Datastructure for the windowclass */
  STARTUPINFO startinfo;  
  HINSTANCE   TvWinThisInstance= GetModuleHandle (NULL); 
  long aux;                       /* User data retrieval */
  
  GetStartupInfoA (&startinfo);   /* Get the command line passed to the process. */
  
/*
 *  JASC, ene 2006, now bitmap fonts 
 */  
 
  int maxX=80; 
  int maxY=25;
  int fontW=10; 
  int fontH=20;

 /* Look for defaults */

/* 
 * Code page, User settings have more priority than detected settings 
 */
  optSearch("AppCP", forcedAppCP );
  optSearch("ScrCP", forcedScrCP );
  optSearch("InpCP", forcedInpCP );

  codePage=
    new TVCodePage( forcedAppCP!=-1 ? forcedAppCP : TVCodePage::ISOLatin1Linux
                  , forcedScrCP!=-1 ? forcedScrCP : TVCodePage::ISOLatin1Linux
                  , forcedInpCP!=-1 ? forcedInpCP : TVCodePage::ISOLatin1Linux );
                 
  SetDefaultCodePages( TVCodePage::ISOLatin1Linux
                     , TVCodePage::ISOLatin1Linux
                     , TVCodePage::ISOLatin1Linux );

  if (optSearch("ScreenWidth" , aux)) { maxX=  aux; }
  if (optSearch("ScreenHeight", aux)) { maxY=  aux; }
  if (optSearch("FontWidth"   , aux)) { fontW= aux; }
  if (optSearch("FontHeight"  , aux)) { fontH= aux; }
 
  if ( optSearch( "Font10x20", aux ) && aux )
  { fontW=10, fontH=20; 
    defaultFont= &font10x20;
  }
 
  normCursor= LoadCursor( NULL, IDC_ARROW    );
  handCursor= LoadCursor( NULL, IDC_HAND     );
  
  if( handCursor==NULL )                        /* Not available in 95 and NT */
  { handCursor= LoadCursor(NULL, IDC_ARROW  ); 
  }
  sizeCursor= LoadCursor(NULL, IDC_SIZENWSE );

  wincl.hIcon  = LoadIcon( NULL              /* Use default icon and mousepointer */
                         , IDI_APPLICATION);
                         
  wincl.hIconSm= LoadIcon( NULL
                         , IDI_APPLICATION);

  wincl.style        = /* CS_DBLCLKS */ 0;  /* don´t Catch double-clicks */
  wincl.cbSize       = sizeof(WNDCLASSEX);
  wincl.hCursor      = normCursor;
  wincl.hInstance    = TvWinThisInstance;
  wincl.cbClsExtra   = 0;                /* No extra bytes after window class */
  wincl.cbWndExtra   = 0;                /* structure or the window instance  */
  wincl.lpfnWndProc  = WindowProcedure;  /* This function is called by windows*/
  wincl.lpszMenuName = NULL;             /* No menu                           */
  wincl.lpszClassName= className;
  wincl.hbrBackground= NULL;

/* 
 * Register the window class, if fail quit the program 
 */
 
  if( !RegisterClassEx(&wincl) ) 
  { return; 
  }

/* 
 * The class is registered, lets create a program 
 */
              xxx();
              
  hwnd= CreateWindowEx
  ( 0                        /* Possible styles */
  , className               /* Classname ("MyLittleWindow")           */
  , className               /* Title Text                             */
  , WS_CAPTION                /* defaultwindow                          */
  | WS_SYSMENU
  | WS_MAXIMIZEBOX
  | WS_MINIMIZEBOX
  , CW_USEDEFAULT             /* Windows decides the position           */
  , CW_USEDEFAULT             /* where the window end up on the screen  */
  , maxX * fontW
  , maxY * fontH
  , HWND_DESKTOP              /* The window is a childwindow to desktop */
  , NULL                      /* No menu                                */
  , TvWinThisInstance         /* Program Instance handler               */
  , NULL );                   /* No Window Creation data                */
  

  if ( ! hwnd )               /* Fails to create (font creation assigns) */
  { return; 
  }
  
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
  { colorMap[ col ]= RGB( (col&4) ? (col&8) ? fullCOLOR:halfCOLOR:0
                        , (col&2) ? (col&8) ? fullCOLOR:halfCOLOR:0
                        , (col&1) ? (col&8) ? fullCOLOR:halfCOLOR:0 ); 
  }

  flags0= CodePageVar
        | CursorShapes
        | CanSetBFont   
        | CanSetSBFont
        | NoUserScreen
        | CanSetVideoSize
        | CanSetFontSize
   /*     | CanSetPalette 
        | CanReadPalette */;
 
     
  cursorLines=
    startupCursor=
     getCursorType();

/*
 * Start cursor blinking handler  
 */

  cursorProc( hwnd     // handle of window for timer messages
            , 0        // Don´t draw cursor
            , 0        // timer identifier
            , 0 );     // current system time
            
}            



/* 
 * 
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
  TScreen::getFontGeometryRange= GetFontGeometryRange;

  TVWin32Clipboard::Init();  /* compatible clipboard */

  setVideoMode( startupMode= smCO80 );


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
{ if ( screenBuffer )
  { delete screenBuffer;
    screenBuffer= NULL; 
  }
}

/*
 *
 */
/*void TScreenWinGr::clearScreen()
{ 
} */

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
      { lowSetCursor( xPos
                    , yPos
                    , true ); 
      }
    }
  }
}
           
              
/*
 *  JASC jul/2002, write a unique colored line to memory and screen
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
void TScreenWinGr::setCharacter(unsigned offset, ushort value)
{ setCharacters( offset
               , &value
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

/*
 *  JASC, 2002
 *
 *    Changes the video mode, both hardware ( resolution ) and software
 *  ( resources ) layers. Only realocates memory on growing, avoiding heap
 *  fragmentation.
 */
void TScreenWinGr::setVideoMode( ushort mode )
{ int newAmountOfCells;

  setCrtMode( mode );  /* Class name ??? */
  
/*
 * Try if more memory needed, and realloc or clear memory depending
 */
 
  if (( newAmountOfCells= getRows()*getCols()) > amountOfCells )
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
  
  setCrtData();   /* Do the hardware layer */
  winRecalc();
}

int TScreenWinGr::SetVideoModeRes( unsigned w
                                 , unsigned h
                                 , int fW
                                 , int fH )
{ if (( fW == -1 )
   && ( fH == -1 ))
  { TScreenWinGr::screenWidth=  w;
    TScreenWinGr::screenHeight= h;
  }
  else
  { SetCrtModeRes( w, h
                 , fW, fH );
    setVideoMode( 0xFFFF );     /* Alloc memory */
  }
  
  winRecalc();
  return( 1 );
}

/*
 *   ( From SET code ), execute an external program in a syncronous way,
 *  redirecting input, output and error
 */ 
int TScreenWinGr::System( const char * command
                        , pid_t      * pidChild
                        , int in
                        , int out
                        , int err)
{
#ifndef TVCompf_Cygwin
 if ( pidChild )     // fork mechanism not implemented, indicate the child finished
  { *pidChild=0;
  }
#endif

  // If the caller asks for redirection replace the requested handles
  if (!pidChild) // If the caller asks for redirection replace the requested handles
  { if (in!=-1)
    { dup2(in,STDIN_FILENO);
    }
  
    if (out!=-1)
    { dup2(out,STDOUT_FILENO);
    }
  
    if (err!=-1)
    { dup2(err,STDERR_FILENO);
    }
  
    return system(command); 
  }  

#ifdef TVCompf_Cygwin

/*
 *  fork mechanism is implemented in Cygwin, so linux code should work -- OH!
 *   SET: Call to an external program, optionally forking
 */

  pid_t cpid=fork();

/*
 * Ok, we are the child
 *   I'm not sure about it, but is the best I have right now.
 *   Doing it we can kill this child and all the subprocesses
 *   it creates by killing the group. It also have an interesting
 *   effect that I must evaluate: By doing it this process lose
 *   the controlling terminal and won't be able to read/write
 *   to the parents console. I think that's good.
 */

  if ( !cpid )
  { if ( setsid()==-1 )
    { _exit(127); 
    }

    char *argv[4];

    // If the caller asks for redirection replace the requested handles
    if (in!=-1)
    { dup2(in,STDIN_FILENO);
    }
    
    if (out!=-1)
    { dup2(out,STDOUT_FILENO);
    }
    
    if (err!=-1)
    { dup2(err,STDERR_FILENO);
    }

    argv[0]=getenv("SHELL");
    if (!argv[0])
    { argv[0]="/usr/bin/sh"; 
    }

    argv[1]="-c";
    argv[2]=(char *)command;
    argv[3]=0;
    execvp( argv[0]
          , argv );
    _exit(127);     // We get here only if exec failed
  }  

  if (cpid==-1)      // Fork failed do it manually
  { *pidChild=0;
    return system(command); 
  }

 *pidChild=cpid;
#endif
 return(0);
}

/*
 *
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

