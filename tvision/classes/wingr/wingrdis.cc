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

#ifdef TVOS_Win32

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
HFONT      TDisplayWinGr::hFont;          /* Font properties */
HDC        TDisplayWinGr::hdc;            /* A device context used for drawing */
COLORREF   TDisplayWinGr::colorMap[ 16 ];
TEXTMETRIC TDisplayWinGr::tm;

HCURSOR    TDisplayWinGr::normCursor;
HCURSOR    TDisplayWinGr::sizeCursor;
HCURSOR    TDisplayWinGr::handCursor;

RECT       TDisplayWinGr::mSize;             /* Windows resizing calculations */

LOGFONTA * TDisplayWinGr::fonts= NULL; // To hold available fonts

dialogModeRec TDisplayWinGr::mode= { -1, 0 };     // To hold modes


/* -------------------------------------------------------------------------- */
   void TDisplayWinGr::lowSetCursor( int x
				     , int y
				     , bool dir )
/* -------------------------------------------------------------------------- */
{ ushort * dst;
  char fg, bg;
  char letra;
  RECT rect;

  if ( sizeChanged )             // Don't draw on resizing
  { return; }

  if ( !TScreen::screenBuffer )
  { return; }

  if ( zPos  )                   // Cursor not visible now
  { dir= false; }
  
  if ((unsigned) x >= (unsigned)getCols() ) // JASC, this tests both bounds
  { return; }
  
  if ((unsigned) y >= (unsigned)getRows() ) // JASC, this tests both bounds 
  { return; }

  dst = TScreen::screenBuffer    // Calc new cursor position
      + x
      + y*TScreen::screenWidth;   

  if ( dir )
  { fg= attrColor(*dst) >>  4;   // Normal  
    bg= attrColor(*dst) & 0xF; }
  else
  { bg= attrColor(*dst) >>  4;   // Swap colors (reverse)
    fg= attrColor(*dst) & 0xF; }


  letra= attrChar( *dst );
       
  SetBkColor  ( hdc, colorMap[bg] );  // Background color
  SetTextColor( hdc, colorMap[fg] );  // Foreground color

  rect.top=    y * tm.tmHeight + cShapeFr;
  rect.bottom= y * tm.tmHeight + cShapeTo;
  rect.left=   x * tm.tmMaxCharWidth + 0;
  rect.right=  x * tm.tmMaxCharWidth + 8;


  DrawText( hdc                           // Output text
	  , &letra, 1
	  , &rect

	  , DT_NOPREFIX
	  | DT_SINGLELINE
	  | DT_BOTTOM ); }
  

/* -------------------------------------------------------------------------- */
   void TDisplayWinGr::winRecalc( int w
		                , int h  )
/* -------------------------------------------------------------------------- */
{ RECT  wSize;

  GetWindowRect( hwnd        /* acquire window size */
	       , &wSize );   

  if ( w )
  { TScreen::screenWidth = ( w + tm.tmMaxCharWidth / 2 )/ tm.tmMaxCharWidth; } /* quantize width  */
  if ( h )
  { TScreen::screenHeight= ( h + tm.tmHeight       / 2 )/ tm.tmHeight;       } /* quantize height */

  wSize.right=  getCols() * tm.tmMaxCharWidth; /* quantize width  */
  wSize.bottom= getRows() * tm.tmHeight;       /* quantize height */
 
  MoveWindow( hwnd
            , wSize.left
	    , wSize.top
            , wSize.right + mSize.right + mSize.left
	    , wSize.bottom+ mSize.top   + mSize.bottom
	    , true );            // repaintin soon
  sizeChanged++; }



/* ------------------------------------------------------------------------- */
   int CALLBACK EnumFonts( const LOGFONT    * lplf     // address of logical-font data structure
                         , const TEXTMETRIC * lpntm    // address of physical-font data structure
                         , long unsigned int  /*FontType*/ // type of font
                         , LPARAM          /*lpData*/ )    // address of application-defined data
/* ------------------------------------------------------------------------- */
{ int arraySize= 1;                                 // Terminator space

  if ( lplf->lfCharSet != OEM_CHARSET )  // Only pc 437 charset
  { return( 1 ); }
  
  if ( lplf->lfItalic )                  // No italics (bad box drawings)
  { return( 2 ); }
  
  if ( lplf->lfUnderline )               // No underlines (evident)
  { return( 3 ); }

  if ( lpntm->tmPitchAndFamily           // Only fixed width
     & TMPF_FIXED_PITCH )
  { return( 4 ); }


  if ( TDisplayWinGr::fonts )            // Previously allocated
  { for ( LOGFONT * ptr=                 // Estimate array size
          TDisplayWinGr::fonts   
        ; ptr->lfWeight
        ; ptr ++ )
    { arraySize++; }                     // Enlarge array size

    TDisplayWinGr::fonts=              // Get more room
      (LOGFONT *) realloc( TDisplayWinGr::fonts
                         , arraySize 
                         * sizeof( LOGFONT )); }
  else
  { TDisplayWinGr::fonts=              // Get more room
      (LOGFONT *) calloc( arraySize
                        , sizeof( LOGFONT )); }

  TDisplayWinGr::fonts[ arraySize-1 ]= *lplf;      // store new font
  TDisplayWinGr::fonts[ arraySize   ].lfWeight= 0; // terminate array


  return( 6 ); }


/* ------------------------------------------------------------------------- */
   int CALLBACK EnumFamilies( const LOGFONTA    * lplf     // address of logical-font data structure
                            , const TEXTMETRICA * /*lpntm*/    // address of physical-font data structure
                            , long unsigned int   /*FontType*/ // type of font
                            , LPARAM              lpData ) // address of application-defined data
/* ------------------------------------------------------------------------- */
{ EnumFontFamilies( TDisplayWinGr::hdc
                  , lplf->lfFaceName
                  , EnumFonts
                  , lpData );
  return(1); }



/* ------------------------------------------------------------------------- */
   void TDisplayWinGr::testCreate( HWND hw
                                   , LPARAM lParam )
/* ------------------------------------------------------------------------- */
{ CREATESTRUCT * lpcs;
  RECT  wSize;

  lpcs= (CREATESTRUCT *)lParam;
  hdc= GetDC( hw );

  EnumFontFamilies( hdc            /* Get a list of possible fonts */          
                  , NULL
                  , EnumFamilies
                  , 0 );
  hwnd= hw;
  SetCrtMode( "Terminal 10 x 18" );   /* Set a nice font set */

  GetWindowRect ( hwnd, &mSize );     /* acquire window size */
  GetClientRect ( hwnd, &wSize );     /* acquire client size */

  mSize.left= wSize.left - mSize.left; mSize.right -= wSize.right;
  mSize.top = wSize.top  - mSize.top ; mSize.bottom-= wSize.bottom;

  winRecalc( wSize.right
           , wSize.bottom ); }         /* Recalc windows parameters */



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
      DeleteObject( hFont );  /* Free fonn resource                 */
      PostQuitMessage(0);
      storedEvent.what= evCommand;
      storedEvent.message.command= cmQuit;
    return( 2 );


    case WM_SIZE:
      switch( wParam )
      { case SIZE_MINIMIZED:
	return(0);

	case SIZE_MAXIMIZED: 
	  TScreen::screenWidth = ( LOWORD(lParam) + tm.tmMaxCharWidth / 2 )/ tm.tmMaxCharWidth; /* quantize width  */
	  TScreen::screenHeight= ( HIWORD(lParam) + tm.tmHeight       - 1 )/ tm.tmHeight;       /* quantize height */
        break;

	case SIZE_RESTORED:
	  if ( lParam )
	  { TScreen::screenWidth = ( LOWORD(lParam) + tm.tmMaxCharWidth / 2 )/ tm.tmMaxCharWidth; /* quantize width  */
	    TScreen::screenHeight= ( HIWORD(lParam) + tm.tmHeight       / 2 )/ tm.tmHeight; }     /* quantize height */
	break; }

      sizeChanged++;
    return(4);

    case WM_PAINT: /* The window needs to be painted (redrawn). */
      BeginPaint( hwnd, &ps );

      if ( !sizeChanged )      /* if size changed, later repainting. crash repaninting now */
      { if ( TScreen::screenBuffer )
        { int x= ps.rcPaint.left  / tm.tmMaxCharWidth;
          int y= ps.rcPaint.top   / tm.tmHeight;
          int w= ps.rcPaint.right / tm.tmMaxCharWidth;
          int h= ps.rcPaint.bottom/ tm.tmHeight;

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
	    h --; y ++; }}}                      /* A line minus left */
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
    { return( 0 ); }                            /* Mouse event found    */


    if ( TGKeyWinGr::testEvents( message      /* Try for key envens */
                               , wParam
		               , lParam ))
    { return( 0 ); }                            /* Key event found    */


    if ( TDisplayWinGr::testEvents( message   /* Try another interesting envents */
                                  , wParam
                                  , lParam ))
    { return( 0 ); }}                           /* Interesting event found    */
  else
  { if ( message == WM_CREATE )
    { TDisplayWinGr::testCreate( hwnd
                               , lParam );
      return(0); }}

  return( DefWindowProc( hwnd           // Other events must be forwardwed
                       , message
                       , wParam
		       , lParam )); }

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
		, false ); }

  xPos= x; yPos= y;                // Update cursor position

  lowSetCursor( x
	      , y
	      , true ); }

/* ------------------------------------------------------------------------- */
   void TDisplayWinGr::GetCursorPos( unsigned & x
                                   , unsigned & y )
/* ------------------------------------------------------------------------- */
{ x= xPos;
  y= yPos; }



/* ------------------------------------------------------------------------- */
   void TDisplayWinGr::GetCursorShape( unsigned & start  // From SET code
                                     , unsigned & end )
/* ------------------------------------------------------------------------- */
{ start= cShapeFr; start*= 100; start/= tm.tmHeight;    // Force integer aritmethic
  end  = cShapeTo; end  *= 100; end  /= tm.tmHeight; }


/* ------------------------------------------------------------------------- */
   void TDisplayWinGr::SetCursorShape( unsigned start    // From SET code
                                     , unsigned end )
/* ------------------------------------------------------------------------- */
{ lowSetCursor( xPos       // First of all, remove old cursor
              , yPos
              , false );

  cShapeFr= ( start*tm.tmHeight ) / 100; // Force integer aritmethic
  cShapeTo= (   end*tm.tmHeight ) / 100;

  if ( start>=end && getShowCursorEver() )
  { return; }

  if ((unsigned)cShapeFr > (unsigned)tm.tmHeight )  // Pretty range test  ( 1 test 2 comprobations )
  { cShapeFr= tm.tmHeight; }

  if ((unsigned)cShapeTo > (unsigned)tm.tmHeight )  // Pretty range test  ( 1 test 2 comprobations )
  { cShapeTo= tm.tmHeight; }

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
                , 0x64 ); }

/* ------------------------------------------------------------------------- */
   void TDisplayWinGr::SetFontMode( int fontIdx )
/* ------------------------------------------------------------------------- */
{ static int lastFont= -1;    // Change tracing
  LOGFONT * ptr;              // Font operations

  if ( fontIdx < 0 )          // Reloads mode
  { fontIdx= mode.font; }

  if ( fontIdx == lastFont )  // All done
  { return; }
  else
  { lastFont= fontIdx; }

  ptr= TDisplayWinGr::fonts + fontIdx; // Point to font definition 

  hFont= CreateFont( ptr->lfHeight         /* Font height        */
                   , ptr->lfWidth          /* Font width         */
                   , ptr->lfEscapement     /* Escapement         */
                   , ptr->lfOrientation    /* Orientation        */
                   , ptr->lfWeight         /* Weigth             */
	           , ptr->lfItalic         /* Italic             */
	           , ptr->lfUnderline      /* Underline          */
	           , ptr->lfStrikeOut      /* Bold               */
	           , ptr->lfCharSet        /* Charset            */
	           , ptr->lfOutPrecision   /* output precision   */
	           , ptr->lfClipPrecision  /* clipping precision */
	           , ptr->lfQuality        /* output quality     */
	           , ptr->lfPitchAndFamily /* pitch and family   */
	           , ptr->lfFaceName );    /* typeface name      */

  SelectObject  (  hdc, hFont  );
  GetTextMetrics(  hdc, &tm    ); 

  cShapeTo= tm.tmHeight;
  cShapeFr= tm.tmHeight-2; 
  mode.font= fontIdx; }                        // Store font mode

/* ------------------------------------------------------------------------- */
   void TDisplayWinGr::SetCrtMode( const char * modeName )
/* ------------------------------------------------------------------------- */
{ int wd, hd;                    // Desired size
  char fontName[ LF_FACESIZE + 1 ];
  LOGFONT * ptr;                 // Font operations
  int delta= INT_MAX;            // Chossing suitable font
  int pick= 0;
  int index= 0;

  if (!modeName )
  { return; }

  if ( sscanf( modeName         /* Extract font name and sizes */
             , "%s%d%*s%d"
             , fontName
             , &wd
             , &hd ) == 3 )
  { for ( ptr= TDisplayWinGr::fonts         // Estimate array size
        ; ptr->lfWeight
        ; ptr ++ )
    { int w= wd-ptr->lfWidth;  w*= w;
      int h= hd-ptr->lfHeight; h*= h;

      if ( ( w + h ) < delta )   // Better font
      { delta= w+h;              // new approx
        pick= index; }      // New selection

      if ( !delta )              // Perfect match
      { break; }

      index ++; }}

                                     
  SetFontMode( pick ); } // Take new mode 


/* ------------------------------------------------------------------------- */
   void TDisplayWinGr::SetCrtModeExt( char * modeName )
/* ------------------------------------------------------------------------- */
{ TSItem  * item= NULL;
  TView   * b   = NULL;
  LOGFONT * ptr = NULL;                 // Font operations
  int nFonts= 0;

  if ( modeName )
  { SetCrtMode( modeName );
    return; }

  TDialog *pd = new TDialog( TRect( 20, 4, 60, 20)
                           , "Screen mode selector" );

  if( !pd )
  { return; }

  for ( ptr= TDisplayWinGr::fonts         // Estimate array size
      ; ptr->lfWeight
      ; ptr ++ )
  { nFonts ++; }

  while( --ptr >= TDisplayWinGr::fonts )  // Load fonts backwards 
  { char buff[64];
    sprintf( buff
           , "%s %2ld x %2ld"
           , ptr->lfFaceName
           , ptr->lfWidth
           , ptr->lfHeight
          /* , ptr->lfWeight */  );
    item= new TSItem( buff, item ); }


  b= new TRadioButtons( TRect(  2, 3
                             , 25, 3 + nFonts )
                      , item );

  pd->insert( b );
  pd->insert( new TLabel( TRect( 3, 2, 25, 3)
                        , "Available fonts"
                        , b ));

  b= new TRadioButtons( TRect(  27, 3, 38, 6)
                      , new TSItem( "~C~olor"
                      , new TSItem( "~B~/W"
                      , new TSItem( "~M~ono"
                      , 0 ))));

  pd->insert( b );
  pd->insert( new TLabel( TRect( 29, 2, 37, 3)
                        , "Mode"
                        , b ));


  pd->insert( new TButton( TRect( 28,  9, 38, 11 ), "~A~pply" , cmYes   , bfNormal  ));
  pd->insert( new TButton( TRect( 28, 11, 38, 13 ), "~O~K"    , cmOK    , bfDefault ));
  pd->insert( new TButton( TRect( 28, 13, 38, 15 ), "~C~ancel", cmCancel, bfNormal  ));

  pd->setData( &mode );

  ushort control;                 // Default 
  dialogModeRec  savedMode= mode; // Temp mode

  do
  { control= TProgram::deskTop->execView( pd );

    if ( control == cmYes )     // and read it back when the dialog box is successfully closed
    { pd->getData( &mode );
      SetFontMode( mode.font ); // Change font
      winRecalc( 0, 0 ); }}     // Modify window size
  while ( control == cmYes );

  switch ( control )
  { case cmOK: pd->getData( &mode ); break;   // Restore previous
    case cmCancel: mode= savedMode;  break; } // Get new 

  pd->setData( &mode     );  // Store ratio list 
  SetFontMode( mode.font );  // Change hard font
  winRecalc  ( 0, 0      );  // Modify window size

  CLY_destroy( pd );  // No longer needed

  SetCursorShape( 0x58
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
{ switch( mode.mode )
  { case 0: return(smCO80);
    case 1: return(smBW80);
    case 2: return(smMono); }
  return(smCO80); }

/* ------------------------------------------------------------------------- */
   TDisplayWinGr::~TDisplayWinGr() 
/* ------------------------------------------------------------------------- */
{ free( TDisplayWinGr::fonts );
  TDisplayWinGr::fonts= NULL; }

/* ------------------------------------------------------------------------- */
   void TDisplayWinGr::Beep()
/* ------------------------------------------------------------------------- */
{  MessageBeep(0xFFFFFFFF); }


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

