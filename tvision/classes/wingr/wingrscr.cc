/**[txh]********************************************************************

  Copyright (c) 2002 by Jose Angel Sánchez Caso (JASC)
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
#include <tv.h>

#ifdef TVOS_Win32

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


static char * className= "TVISION for windows"; /* Make the classname into a global variable */


/* ------------------------------------------------------------------------- */
   VOID CALLBACK cursorProc( HWND hwnd      // handle of window for timer messages
                           , UINT uMsg      // WM_TIMER message
                           , UINT idEvent   // timer identifier
                           , DWORD /*dwTime*/ ) // current system time
/* ------------------------------------------------------------------------- */
{ if ( uMsg )
  { TScreenWinGr::zPos ^= 0x80;         /* Toggle phase (turn 180 degrees) */

    if ( TScreenWinGr::xPos >= 0 )
    TScreenWinGr::lowSetCursor( TScreenWinGr::xPos /* Show or hide cursor */
                                , TScreenWinGr::yPos
				, true ); } 
  SetTimer( hwnd                              /* Reload timer        */ 
	  , idEvent
	  , cursorDelay / 1000                /* from usecs to msecs */
	  , cursorProc ); }


/* ------------------------------------------------------------------------- */
   TScreenWinGr::TScreenWinGr()
/* ------------------------------------------------------------------------- */
{ FreeConsole();
  WNDCLASSEX  wincl;            /* Datastructure for the windowclass */
  STARTUPINFO startinfo;  
  HINSTANCE   TvWinThisInstance= GetModuleHandle (NULL); 
  
  GetStartupInfoA (&startinfo);   /* Get the command line passed to the process. */

  normCursor= LoadCursor(NULL, IDC_ARROW    );
  handCursor= LoadCursor(NULL, IDC_HAND     );
  /* Not available in 95 and NT */
  if( handCursor==NULL )
  { handCursor= LoadCursor(NULL, IDC_ARROW  ); }
  sizeCursor= LoadCursor(NULL, IDC_SIZENWSE );


  wincl.hIcon        = LoadIcon( NULL              /* Use default icon and mousepointer */
			       , IDI_APPLICATION);
  wincl.hIconSm      = LoadIcon( NULL
                               , IDI_APPLICATION);

  wincl.style        = /* CS_DBLCLKS */ 0;                 /* don´t Catch double-clicks */
  wincl.cbSize       = sizeof(WNDCLASSEX);
  wincl.hCursor      = normCursor;
  wincl.hInstance    = TvWinThisInstance;
  wincl.cbClsExtra   = 0;                          /* No extra bytes after the window class */
  wincl.cbWndExtra   = 0;                          /* structure or the window instance */
  wincl.lpfnWndProc  = WindowProcedure;            /* This function is called by windows */
  wincl.lpszMenuName = NULL;                       /* No menu */
  wincl.lpszClassName= className;
  wincl.hbrBackground= NULL;

  if( !RegisterClassEx(&wincl) )   /* Register the window class, if fail quit the program */
  { return; }

/* The class is registered, lets create a program */

  CreateWindow
      ( className               /* Classname ("MyLittleWindow")           */
      , className               /* Title Text                             */
      , WS_CAPTION                /* defaultwindow                          */
      | WS_SYSMENU
      | WS_MAXIMIZEBOX
      | WS_MINIMIZEBOX
      , CW_USEDEFAULT             /* Windows decides the position           */
      , CW_USEDEFAULT             /* where the window end up on the screen  */
      , 800, 448                  /* win size                               */
      , HWND_DESKTOP              /* The window is a childwindow to desktop */
      , NULL                      /* No menu                                */
      , TvWinThisInstance         /* Program Instance handler               */
      , NULL );                   /* No Window Creation data                */

  if ( ! hwnd )
  { return; }


  initialized++ ; 


  ShowWindow( hwnd                /* Make the window visible on the screen  */
	    ,  startinfo.dwFlags 
	     & STARTF_USESHOWWINDOW 
	    ? startinfo.wShowWindow
	    : SW_SHOWDEFAULT );

  for ( int col= 0
      ; col<16
      ; col++ )
  { colorMap[ col ]= RGB( (col&4) ? (col&8) ? 0xFF:0x80:0
                        , (col&2) ? (col&8) ? 0xFF:0x80:0
                        , (col&1) ? (col&8) ? 0xFF:0x80:0 ); }


  flags0= CodePageVar
	| CursorShapes;

  screenMode=
   startupMode=
    getCrtMode();

  cursorLines=
   startupCursor=
    getCursorType();

  screenWidth=  GetCols();
  screenHeight= GetRows();
  screenBuffer= ( ushort *) calloc( sizeof( ushort )
                                  , screenHeight*screenWidth ); // This zeroes memory


/*
 * Start cursor blinking handler  
 */

  cursorProc( hwnd     // handle of window for timer messages
	    , 0        // Don´t draw cursor
            , 0        // timer identifier
            , 0 ); }   // current system time




/* ------------------------------------------------------------------------- */
   void TScreenWinGr::Init()
/* ------------------------------------------------------------------------- */
{ TScreen::System_p=        System;
  TScreen::Resume=          Resume;
  TScreen::Suspend=         Suspend;
  TScreen::clearScreen=     clearScreen;
  TScreen::setCharacter=    setCharacter;
  TScreen::setCharacters=   setCharacters; 
  TScreen::setVideoMode=    setVideoMode;
  TScreen::setVideoModeExt= setVideoModeExt; }

/* ------------------------------------------------------------------------- */
   void TScreenWinGr::Done()
/* ------------------------------------------------------------------------- */
{ }

/* ------------------------------------------------------------------------- */
   void TScreenWinGr::Resume()
/* ------------------------------------------------------------------------- */
{ SaveScreen(); }

/* ------------------------------------------------------------------------- */
   void TScreenWinGr::Suspend()
/* ------------------------------------------------------------------------- */
{ RestoreScreen(); }

/* ------------------------------------------------------------------------- */
TScreenWinGr::~TScreenWinGr()
/* ------------------------------------------------------------------------- */
{ if (screenBuffer)
  { delete screenBuffer;
    screenBuffer= NULL; }}

/* ------------------------------------------------------------------------- */
   void TScreenWinGr::clearScreen()
/* ------------------------------------------------------------------------- */
{ }

/* -------------------------------------------------------------------------- */
   void TScreenWinGr::writeLine( unsigned x
                               , unsigned y
                               , int  w
		               , char * str
                               , uchar color )
/* -------------------------------------------------------------------------- */

{ if (w<=0)   // Nothing to do
  { return; }

  SetBkColor  ( hdc, colorMap[color>>4]);     // Color de fondo
  SetTextColor( hdc, colorMap[color&15]);     // Color de fondo


 TextOut( hdc                               // Output text
        , x * tm.tmMaxCharWidth
        , y * tm.tmHeight 
	, str
	, w );


  if ( y==yPos )
  { if ( xPos >= x)
    { if ( xPos<= x+w )
      { lowSetCursor( xPos
                    , yPos
                    , true ); }}}}
           
              
/* ------------------------------------------------------ [ JASC jul/2002] -- */
   void TScreenWinGr::setCharacters( unsigned  offset
		                   , ushort  * src
	                           , unsigned  len )
/* -------------------------------------------------------------------------- */
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

  if (!len)
  { return; }

  if (!forceRedraw)
  { while (len > 0 && *old == *src)             /* remove unchanged characters from left to right */
    { x++; offset++;  old++; src++; len--; }


    while (len > 0 && *old_right == *src_right) /* remove unchanged characters from right to left */
    { old_right--; src_right--; len--;     }}


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
	   , last ); }       /* Print last block */


 

/* ------------------------------------------------------------------------- */
   void TScreenWinGr::setCharacter(unsigned offset, ushort value)
/* ------------------------------------------------------------------------- */
{ setCharacters( offset
	       , &value
	       , 1 ); }

/* ------------------------------------------------------------------------- */
   void TScreenWinGr::SaveScreen()
/* ------------------------------------------------------------------------- */
{ }

/* ------------------------------------------------------------------------- */
   void TScreenWinGr::RestoreScreen()
/* ------------------------------------------------------------------------- */
{ }

/* ------------------------------------------------------------------------- */
   void TScreenWinGr::SaveScreenReleaseMemory(void)
/* ------------------------------------------------------------------------- */
{/* if (buffer)
  { delete buffer; }

 buffer = NULL; */ }


/* ------------------------------------------------------------------------- */
   void TScreenWinGr::setVideoMode( ushort mode )
/* ------------------------------------------------------------------------- */
{ int sz= getRows()*getCols();

  if (screenBuffer)
  { free( screenBuffer);
    screenBuffer= NULL; }
                        
  screenBuffer= ( ushort *) calloc( sizeof(ushort)
                                  , sz ); // This zeroes memory

  setCrtMode( fixCrtMode( mode ) );
  setCrtData(); }


/* ------------------------------------------------------------------------- */
   int TScreenWinGr::System( const char * command
                           , pid_t      * pidChild
                           , int in
                           , int out
                           , int err)
/* ------------------------------------------------------------------------- */

#ifndef TVCompf_Cygwin
{ if (pidChild)     // fork mechanism not implemented, indicate the child finished
  { *pidChild=0; }
  // If the caller asks for redirection replace the requested handles
  if (in!=-1)
     dup2(in,STDIN_FILENO);
  if (out!=-1)
     dup2(out,STDOUT_FILENO);
  if (err!=-1)
     dup2(err,STDERR_FILENO);
  return system(command); }
#else

/*
 *  fork mechanism is implemented in Cygwin, so linux code should work -- OH!
 *   SET: Call to an external program, optionally forking
 */

{ if (!pidChild)
  {
   // If the caller asks for redirection replace the requested handles
   if (in!=-1)
      dup2(in,STDIN_FILENO);
   if (out!=-1)
      dup2(out,STDOUT_FILENO);
   if (err!=-1)
      dup2(err,STDERR_FILENO);
   return system(command); }

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
    { _exit(127); }

    char *argv[4];

    // If the caller asks for redirection replace the requested handles
    if (in!=-1)
       dup2(in,STDIN_FILENO);
    if (out!=-1)
       dup2(out,STDOUT_FILENO);
    if (err!=-1)
       dup2(err,STDERR_FILENO);

    argv[0]=getenv("SHELL");
    if (!argv[0])
    { argv[0]="/usr/bin/sh"; }

    argv[1]="-c";
    argv[2]=(char *)command;
    argv[3]=0;
    execvp( argv[0]
          , argv );
    _exit(127); }    // We get here only if exec failed

  if (cpid==-1)      // Fork failed do it manually
  { *pidChild=0;
    return system(command); }

 *pidChild=cpid;
 return(0); }
#endif

/* ------------------------------------------------------------------------- */
   TScreen * TV_WinGrDriverCheck()
/* ------------------------------------------------------------------------- */
{ TScreenWinGr *drv=new TScreenWinGr();

  if ( !TScreen::initialized )
  { delete drv;
    drv= NULL; }

    TGKeyWinGr::init();
  TScreenWinGr::Init();
 TDisplayWinGr::Init();
 THWMouseWinGr::init();

 return(drv); }

#else

#include <tv/wingr/screen.h>
#include <tv/wingr/mouse.h>
#include <tv/wingr/key.h>

#endif // TVOS_Win32

