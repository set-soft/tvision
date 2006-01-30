/**[txh]********************************************************************

  Copyright (c) 2002 José Angel Sánchez Caso (JASC)

  skeletons by Salvador E. Tropea (SET)

  Description:
  Windows GUI Display routines.

***************************************************************************/
#include <tv/configtv.h>

#define Uses_stdio
#define Uses_stdlib
#define Uses_TDisplay
#define Uses_TScreen
#define Uses_TEvent
#define Uses_TGKey
#define Uses_TMouse
#define Uses_TProgram
#define Uses_TVCodePage
#define Uses_TDeskTop
#define Uses_TRect
#define Uses_TDialog
#define Uses_TSItem          // JASC, ago 2002
#define Uses_TCheckBoxes     // JASC, ago 2002
#define Uses_TRadioButtons   // JASC, ago 2002
#define Uses_TButton         // JASC, ago 2002
#define Uses_TLabel          // JASC, ago 2002
#include <tv.h>

#if defined(TVOS_Win32) && !defined(TV_Disable_WinGr_Driver)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <tv/wingr/screen.h>
#include <tv/wingr/key.h>
#include <tv/wingr/mouse.h>

const int maxTitleSize= 256;

char       TDisplayWinGr::cShapeFr;
char       TDisplayWinGr::cShapeTo;

unsigned   TDisplayWinGr::xPos= UINT_MAX; /* X axis of the cursor */
unsigned   TDisplayWinGr::yPos= UINT_MAX; /* Y axis of the cursor */
unsigned   TDisplayWinGr::zPos;           /* Z axis of the cursor (means size )*/
int        TDisplayWinGr::sizeChanged= 0; /* Resizinf counter                  */
int        TDisplayWinGr::forceRedraw= 0;

TEvent     TDisplayWinGr::storedEvent;    /* Correctly processes message queue */
HWND       TDisplayWinGr::hwnd= NULL;     /* Main window handler */
HDC        TDisplayWinGr::hdc;            /* A device context used for drawing */
COLORREF   TDisplayWinGr::colorMap[ 16 ];

HCURSOR    TDisplayWinGr::normCursor;
HCURSOR    TDisplayWinGr::sizeCursor;
HCURSOR    TDisplayWinGr::handCursor;

bitmapFontRec TDisplayWinGr::primary  = {0,0,0,0,0};
bitmapFontRec TDisplayWinGr::secondary= {0,0,0,0,0};


/* -------------------------------------------------------------------------- */
   void TDisplayWinGr::lowSetCursor( int x
				                   , int y
				                   , bool dir )
/* -------------------------------------------------------------------------- */
{ ushort * dst;
  char fg, bg;
  char letra;
  int to, fr;

/*  if ( sizeChanged )             // Don't draw on resizing
  { return; 
  }
*/
  if ( !TScreen::screenBuffer )
  { return; 
  }

  if ( zPos  )                   // Cursor not visible now
  { dir= false; 
  }
  
  if ((unsigned) x >= (unsigned)getCols() ) // JASC, this tests both bounds
  { return; 
  }
  
  if ((unsigned) y >= (unsigned)getRows() ) // JASC, this tests both bounds 
  { return; 
  }

  dst = TScreen::screenBuffer    // Calc new cursor position
      + x
      + y*TScreen::screenWidth;   

  if ( dir )
  { bg= attrColor(*dst) >>  4;   // Swap colors (reverse)
    fg= attrColor(*dst) & 0xF; 
  }
  else
  { fg= attrColor(*dst) >>  4;   // Normal  
    bg= attrColor(*dst) & 0xF; 
  }

  letra= attrChar( *dst );
       
  SetBkColor  ( hdc, colorMap[(int)bg] );  // Background color
  SetTextColor( hdc, colorMap[(int)fg] );  // Foreground color
  
  fr= dir ? cShapeFr:0;
  to= dir ? cShapeTo:primary.h;

  BitBlt( hdc
        , x * primary.w
        , y * primary.h + fr
        , primary.w
        , to - fr - 1
        , primary.bitmapMemo
        , 0, letra * primary.h
        , SRCCOPY );

  }
  

/* -------------------------------------------------------------------------- */
   void TDisplayWinGr::winRecalc( )
/* -------------------------------------------------------------------------- */
{ RECT  wSize, cSize;
  int w, h;  

  GetWindowRect ( hwnd, &wSize );     /* acquire window size */
  GetClientRect ( hwnd, &cSize );     /* acquire client size */
  
  cSize.right-=  cSize.left;  /* calc increments */
  cSize.bottom-= cSize.top;  

  if ( ! ( cSize.right + cSize.bottom ))   /* Window not created */
  { return; 
  }

  wSize.right-=  wSize.left;  /* calc increments */
  wSize.bottom-= wSize.top;  
  
  w= getCols() * primary.w; /* quantize width    */
  h= getRows() * primary.h;       /* quantize height   */
  
  if (( cSize.right  == w )         /* No changes, all done */
   && ( cSize.bottom == h ))
  { return;
  } 
    
  MoveWindow( hwnd
            , wSize.left
            , wSize.top
            , wSize.right  - cSize.right  + w 
	        , wSize.bottom - cSize.bottom + h 
	        , true );            // repaintin soon
}





/* ------------------------------------------------------------------------- */
   int TDisplayWinGr::testEvents( UINT   message
                                  , WPARAM wParam
                                  , LPARAM lParam )
/* ------------------------------------------------------------------------- */
{ PAINTSTRUCT ps;        /* Also used during window drawing   */

  switch( message )
  { case WM_QUIT:
      storedEvent.what= evCommand;
      storedEvent.message.command= cmQuit;
    return( 1 );

    case WM_DESTROY:          /* send a WM_QUIT to the messagequeue */
//      DeleteObject( hFont );  /* Free fonn resource                 */
      PostQuitMessage(0);
      storedEvent.what= evCommand;
      storedEvent.message.command= cmQuit;
    return( 2 );


    case WM_SIZE:
         if (  primary.w )
{      switch( wParam )
      { case SIZE_MINIMIZED:
        return(0);

	    case SIZE_MAXIMIZED: 
	      TScreen::screenWidth = ( LOWORD(lParam) + primary.w / 2 )/ primary.w; /* quantize width  */
	      TScreen::screenHeight= ( HIWORD(lParam) + primary.h - 1 )/ primary.h;       /* quantize height */
        break;

	    case SIZE_RESTORED:
	    if ( lParam )
	    { TScreen::screenWidth = ( LOWORD(lParam) + primary.w / 2 )/ primary.w; /* quantize width  */
	      TScreen::screenHeight= ( HIWORD(lParam) + primary.h / 2 )/ primary.h; }     /* quantize height */
	    break; 
      } }

      sizeChanged++;
      return(4);

    case WM_PAINT: /* The window needs to be painted (redrawn). */
      BeginPaint( hwnd, &ps );

      if ( !sizeChanged )      /* if size changed, later repainting. crash repaninting now */
      { if ( TScreen::screenBuffer )
        { int x= ps.rcPaint.left  / primary.w;
          int y= ps.rcPaint.top   / primary.h;
          int w= ps.rcPaint.right / primary.w;
          int h= ps.rcPaint.bottom/ primary.h;

	      w-= x; h -= y;

	      unsigned ofs= getCols()*y+x;
	      ushort  *src= TScreen::screenBuffer+ofs;

	      forceRedraw=1;
	      
          while (h>=0)
	      { TScreen::setCharacters( ofs
		                          , src
				                  , w+1 );
            src+= getCols();         /* Next line         */
            ofs+= getCols();
            h --; y ++;              /* A line minus left */
          }
        }
      }                      
	  forceRedraw=0;

      EndPaint( hwnd, &ps );
    return( 5 ); }
    
    return( 0 ); }    /* for messages that we don't deal with */


/* -------------------------------------------------------------------------- */
   LRESULT CALLBACK WindowProcedure( HWND hwnd    /* This function is called by the Windowsfunction DispatchMessage( ) */
                                   , UINT message
                                   , WPARAM wParam
                                   , LPARAM lParam )
/* -------------------------------------------------------------------------- */
{ if ( THWMouseWinGr::hwnd ) 

  { if ( THWMouseWinGr::testEvents( message   /* Try for mouse envens */
                                  , wParam
                                  , lParam ))
    { return( 0 );                            /* Mouse event found    */
    }


    if ( TGKeyWinGr::testEvents( message      /* Try for key envens */
                               , wParam
	                           , lParam ))
    { return( 0 );                            /* Key event found    */
    }


    if ( TDisplayWinGr::testEvents( message   /* Try another interesting envents */
                                  , wParam
                                  , lParam ))
    { return( 0 );                           /* Interesting event found    */
    }
  }

  return( DefWindowProc( hwnd           // Other events must be forwardwed
                       , message
                       , wParam
                       , lParam )); 
}

/* ------------------------------------------------------------------------- */
   bool TDisplayWinGr::processEvent(  )
/* ------------------------------------------------------------------------- */
{ MSG message;                  /* Here messages to the application is saved */

  if ( PeekMessage( &message
                  , NULL          /* Default window              */
                  , 0, 0          /* All messages                */
		  , PM_REMOVE ))  /* Remove mensagges from queue */
  { switch( message.message )     /* Catch some keys             */
    { case WM_KEYUP:
      case WM_KEYDOWN:
	TGKeyWinGr::testEvents( message.message    /* Try for key envens */
		              , message.wParam 
			      , message.lParam ); } 

    TranslateMessage( &message );   /* Arrange key events              */
    DispatchMessage ( &message );   /* Send message to WindowProcedure */
    return( true ); }

  return( false ); }


/* ------------------------------------------------------- JASC, jul 2002 -- */
   void TDisplayWinGr::SetCursorPos( unsigned x
                                   , unsigned y )
/* ------------------------------------------------------------------------- */
{ if (( x != (unsigned)xPos )
   || ( y != (unsigned)yPos ))
  { lowSetCursor( xPos         // Remove old cursor
                , yPos
                , false ); 
  }

  xPos= x; yPos= y;                // Update cursor position
  lowSetCursor( x
	      , y
	      , true ); }

/* ------------------------------------------------------------------------- */
   void TDisplayWinGr::GetCursorPos( unsigned & x
                                   , unsigned & y )
/* ------------------------------------------------------------------------- */
{ x= xPos;
  y= yPos; 
}



/* ------------------------------------------------------------------------- */
   void TDisplayWinGr::GetCursorShape( unsigned & start  // From SET code
                                     , unsigned & end )
/* ------------------------------------------------------------------------- */
{ start= cShapeFr; start*= 100; start/= primary.h;    // Force integer aritmethic
  end  = cShapeTo; end  *= 100; end  /= primary.h; }


/* ------------------------------------------------------------------------- */
   void TDisplayWinGr::SetCursorShape( unsigned start    // From SET code
                                     , unsigned end )
/* ------------------------------------------------------------------------- */
{ lowSetCursor( xPos       // First of all, remove old cursor
              , yPos
              , false );

  cShapeFr= ( start*primary.h ) / 100; // Force integer aritmethic
  cShapeTo= (   end*primary.h ) / 100;

  if ( start>=end && getShowCursorEver() )
  { return; }

  if ((unsigned)cShapeFr > (unsigned)primary.h )  // Pretty range test  ( 1 test 2 comprobations )
  { cShapeFr= primary.h; }

  if ((unsigned)cShapeTo > (unsigned)primary.h )  // Pretty range test  ( 1 test 2 comprobations )
  { cShapeTo= primary.h; }

  if ( start >= end )
  { zPos |=  1; }        /* Disable cursor */
  else
  { zPos &= ~1; }

  lowSetCursor( xPos
              , yPos
              , true ); }


/* ------------------------------------------------------------------------- */
   ushort TDisplayWinGr::GetRows()
/* ------------------------------------------------------------------------- */
{ return( TScreen::screenHeight ); }

/* ------------------------------------------------------------------------- */
   ushort TDisplayWinGr::GetCols() 
/* ------------------------------------------------------------------------- */
{ return( TScreen::screenWidth ); }


/* ------------------------------------------------------------------------- */
   int TDisplayWinGr::CheckForWindowSize()
/* ------------------------------------------------------------------------- */
{ int ret= sizeChanged;

  sizeChanged= 0;
  return( ret ); }

/* ------------------------------------------------------------------------- */
   void TDisplayWinGr::SetCrtMode( ushort )
/* ------------------------------------------------------------------------- */
{ SetCursorShape( 0x58
                , 0x64 ); 
  winRecalc( );         // Readjust window size
}

/* ------------------------------------------------------------------------- */
   void TDisplayWinGr::SetCrtMode( const char * modeName )
/* ------------------------------------------------------------------------- */
{ int wd, hd;                    // Desired size
  int pick= 0;
  int index= 0;

  if (!modeName )
  { return; 
  }
}


/* ------------------------------------------------------------------------- */
   void TDisplayWinGr::SetCrtModeExt( char * modeName )
/* ------------------------------------------------------------------------- */
{ SetCursorShape( 0x58
                , 0x64 ); }


/* ------------------------------------------------------------------------- */
   const char * TDisplayWinGr::GetWindowTitle(void)
/* ------------------------------------------------------------------------- */
{ static char buff[ maxTitleSize ];

  GetWindowText( hwnd
	       , buff
	       , sizeof( buff) );

  return( buff );}


/* ------------------------------------------------------------------------- */
   int TDisplayWinGr::SetWindowTitle( const char *name )
/* ------------------------------------------------------------------------- */
{ SetWindowText( hwnd
               , name );

  return(0); }


/* ------------------------------------------------------------------------- */
   ushort TDisplayWinGr::GetCrtMode()
/* ------------------------------------------------------------------------- */
{ switch( 0 /* mode.mode */ )
  { case 0: return(smCO80);
    case 1: return(smBW80);
    case 2: return(smMono); 
}
  return(smCO80); }

/* ------------------------------------------------------------------------- */
   TDisplayWinGr::~TDisplayWinGr() 
/* ------------------------------------------------------------------------- */
{ 
}

/* ------------------------------------------------------------------------- */
   void TDisplayWinGr::Beep()
/* ------------------------------------------------------------------------- */
{  MessageBeep(0xFFFFFFFF); 
}


/* ------------------------------------------------------------------------- */
   void TDisplayWinGr::Init()
/* ------------------------------------------------------------------------- */
{ getRows=            GetRows;
  getCols=            GetCols;
  setCrtMode=         SetCrtMode;
  getCrtMode=         GetCrtMode;
  setCursorPos=       SetCursorPos;
  getCursorPos=       GetCursorPos;
  setCrtModeExt=      SetCrtModeExt;
  setWindowTitle=     SetWindowTitle;
  getWindowTitle=     GetWindowTitle;
  getCursorShape=     GetCursorShape;
  setCursorShape=     SetCursorShape;
  checkForWindowSize= CheckForWindowSize;
  beep=               Beep;

  TProgram::doNotReleaseCPU= true; }   // annoys windows loop, implemented here


#else

#include <tv/wingr/screen.h>
#include <tv/wingr/key.h>
#include <tv/wingr/mouse.h>

#endif // TVOS_Win32

