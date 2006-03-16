/**[txh]********************************************************************

  Copyright (c) 2002 by Jose Angel Sanchez Caso (JASC)

  Covered by the GPL license.

  Squeletons and modeling by Salvador E. Tropea (SET) (c) (1998)

  THWindows mouse stuff.

*****************************************************************************/
#include <tv/configtv.h>

#define Uses_TDisplay
#define Uses_TScreen
#define Uses_TGKey
#define Uses_TMouse
#define Uses_TEvent
#define Uses_TEventQueue
#define Uses_TVCodePage
#include <tv.h>

#if defined(TVOS_Win32) && !defined(TV_Disable_WinGr_Driver)

// #define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <tv/wingr/screen.h>
#include <tv/wingr/mouse.h>
#include <tv/wingr/key.h>

bool    THWMouseWinGr::pressed= false;  /* JASC, feb 2006, must be known */
HCURSOR THWMouseWinGr::lastCursor= NULL;


/* 
 * 
 */
int THWMouseWinGr::setMouse( LPARAM lParam
                           , int ev )
{ storedEvent.what= ev;
  storedEvent.mouse.doubleClick= False;
  
  storedEvent.mouse.where.x = (short)LOWORD( lParam ) < 0 ? 0
                            : (short)LOWORD( lParam );
  storedEvent.mouse.where.x/= primary.w;

  storedEvent.mouse.where.y = (short)HIWORD( lParam ) < 0 ? 0
                            : (short)HIWORD( lParam );
                            
  storedEvent.mouse.where.y/= primary.h;

  switch( ev )
  { case evMouseUp:
      ReleaseCapture(); pressed= false;
    break;    
  
    case evMouseDown:                     /* Store press coordinates */
      SetCapture( hwnd ); pressed= true;
    break;

    case evMouseMove:
      if ( pressed )                     /* Mouse is down */
      { if ( lastCursor != sizeCursor )  /* Not in the resizing area   */
        { break; 
        }

        storedEvent.what= evNothing;    /* Not a system event */
        storedEvent.mouse.where.x += 1;
        storedEvent.mouse.where.y += 1;

        if ( TScreen::screenWidth  == storedEvent.mouse.where.x
          && TScreen::screenHeight == storedEvent.mouse.where.y )
        { break;                         /* No resizing needed */
        }

        TScreen::screenWidth = storedEvent.mouse.where.x < 0x100 ? storedEvent.mouse.where.x : 0xFF; ;
        TScreen::screenHeight= storedEvent.mouse.where.y < 0x100 ? storedEvent.mouse.where.y : 0xFF;;
        winRecalc(  );    /* Recalc new window size */
      break;
      }

      if ( ! storedEvent.mouse.where.y  )
      { lastCursor= handCursor;
      }
      else
      { if ( storedEvent.mouse.where.y > ( TScreen::screenHeight-2 ))
        { lastCursor= storedEvent.mouse.where.x > ( TScreen::screenWidth-2 )
                    ? sizeCursor
                    : handCursor;
        }
        else
        { lastCursor= normCursor;
        }
      }
    }

  SetCursor( lastCursor );
  return(0);
}


/*
 *
 */
int THWMouseWinGr::testEvents( UINT   message
                             , WPARAM wParam
                             , LPARAM lParam )
{ switch( message )
  { case WM_MOUSEMOVE:   setMouse( lParam, evMouseMove ); break;
    case WM_LBUTTONUP:   setMouse( lParam, evMouseUp   ); break;
    case WM_MBUTTONUP:   setMouse( lParam, evMouseUp   ); break;
    case WM_RBUTTONUP:   setMouse( lParam, evMouseUp   ); break;
    case WM_LBUTTONDOWN: setMouse( lParam, evMouseDown ); break;
    case WM_MBUTTONDOWN: setMouse( lParam, evMouseDown ); break;
    case WM_RBUTTONDOWN: setMouse( lParam, evMouseDown ); break;

    case WM_MOUSEWHEEL :
      storedEvent.what= evMouseMove;   /* Emulate wheel */
      storedEvent.mouse.where.x= (short)LOWORD( lParam ) / primary.w;
      storedEvent.mouse.where.y= (short)HIWORD( lParam ) / primary.h;
      storedEvent.mouse.buttons= ((short) HIWORD( wParam )) > 0 ? mbButton4 : mbButton5;
    return( message );   /* Was a mouse event */
    
    default: return( 0 );
  }

  storedEvent.mouse.buttons= ( wParam & MK_LBUTTON ) ? mbLeftButton   : 0
                           | ( wParam & MK_MBUTTON ) ? mbMiddleButton : 0
                           | ( wParam & MK_RBUTTON ) ? mbRightButton  : 0 ;

  return( message );   /* Was a mouse event */
}

        


/*
 *
 */
void THWMouseWinGr::GetEvent( MouseEventType & me )
{ static int xOld=  0;          /* Persistent across calls    */
  static int yOld=  0;          /* Persistent across calls    */
  static int bOld=  0;          /* Persistent across calls    */

  me.buttons= bOld;                  
  me.where.x= xOld;
  me.where.y= yOld;

  do
  { 
     switch( storedEvent.what )  /* Is there a previous event? */
    { case evCommand:                /* Windows says "exit"        */
      return;

      case evMouseUp:
      case evMouseDown:
      case evMouseMove:
        me= storedEvent.mouse;  /* Give stored event          */
        bOld= me.buttons;       /* The default is no message  */
        xOld= me.where.x;
        yOld= me.where.y;
        storedEvent.what=       /* Mark as exhausted          */
           evNothing;
                 

      case evKeyDown:         /* Key event pending          */
      return; }}              /* All done, exit function    */
  while ( processEvent());    /* Swallow new events         */
}


/*
 *
 */
void THWMouseWinGr::init( )
{ THWMouse::GetEvent= THWMouseWinGr::GetEvent;
  buttonCount= 5;
}

#else

#include <tv/wingr/screen.h>
#include <tv/wingr/mouse.h>
#include <tv/wingr/key.h>

#endif // TVOS_Win32

