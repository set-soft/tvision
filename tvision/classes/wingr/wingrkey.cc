/**[txh]********************************************************************

  Copyright (c) 2002 by Jose Angel Sanchez Caso (JASC)

  Covered by the GPL license.

  It said:

  Console code based on code by Vadim Beloborodov
  Squeleton based on code by Salvador E. Tropea (SET) (c) (1998)
  windows GUI code by Jose Angel Sanchez Caso (JASC).

  Description:
  Windows Keyboard routines.

***************************************************************************/
#include <tv/configtv.h>

#define Uses_ctype
#define Uses_string
#define Uses_TEvent
#define Uses_TGKey
#define Uses_TKeys
#define Uses_TKeys_Extended
#define Uses_TDisplay
#define Uses_TScreen
#define Uses_TVCodePage
#include <tv.h>

#if defined(TVOS_Win32) && !defined(TV_Disable_WinGr_Driver)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <tv/wingr/screen.h>
#include <tv/wingr/key.h>
#include <tv/wingr/mouse.h>

/*      else
      {
      sprintf( buff
	     , "   CHAR keycode: %d charcode: %d scancode: %d mask %d"
	     , storedEvent.keyDown.keyCode
	     , storedEvent.keyDown.charScan.charCode
	     , storedEvent.keyDown.charScan.scanCode
	     , storedEvent.keyDown.shiftState);

      TScreenWindows::writeLine( 1
			       , 3
                               , strlen(buff)
                               , buff
                               , 0xF1 ); }
			  static int rt= 0;
                          char buff[512];

  */

/*
 * Translate keyboard events to Salvador E. Tropea's key codes
 * by Vadim Beloborodov
 * Originally in trans.cc
 */

ushort       TGKeyWinGr::keyMask= 0;
CodePage   * TGKeyWinGr::remapKey= NULL;  // Multilingual keyboard support


/* -------------------------------------------------------------------------- */
   unsigned TGKeyWinGr::GetShiftState()
/* -------------------------------------------------------------------------- */
{ return( keyMask ); }


typedef struct
  { int x11
  ; short scan
  ; short ascii
  ; } ConvKeyRec ;



/*
 * JASC, not implemented key codes ( may be in future )
 *
 *  VK_LBUTTON (  1)
 *  VK_MBUTTON (  4)
 *  VK_RBUTTON (  2)
 *  VK_F13     (124)
 *  VK_F14     (125)
 *  VK_F15     (126)
 *  VK_F16     (127)
 *  VK_F17     (128)
 *  VK_F18     (129)
 *  VK_F19     (130)
 *  VK_F20     (131)
 *  VK_F21     (132)
 *  VK_F22     (133)
 *  VK_F23     (134)
 *  VK_F24     (135)
 *
 * VK_CANCEL   (  3)
 * VK_CLEAR    ( 12)
 * VK_PAUSE    ( 19)
 * VK_SELECT   ( 41)
 * VK_EXECUTE  ( 43)
 * VK_SNAPSHOT ( 44)
 * VK_HELP     ( 47)
 *
 * kbWinLeft = 0x0063
 * kbWinRight= 0x0064
 * kbWinSel  = 0x0065
 *
 */

ConvKeyRec xlateTable[]=
  { { VK_UP       , kbUp         , 0   }  /* Move up, up arrow       */
  , { VK_ESCAPE   , kbEsc        , 0   }
  , { VK_TAB      , kbTab        , 0   }
  , { VK_END      , kbEnd        , 0   }  /* EOL                     */
  , { VK_LEFT     , kbLeft       , 0   }  /* Move left, left arrow   */
  , { VK_DOWN     , kbDown       , 0   }  /* Move down, down arrow   */
  , { VK_HOME     , kbHome       , 0   }
  , { VK_PRIOR    , kbPgUp       , 0   }
  , { VK_NEXT     , kbPgDn       , 0   }
  , { VK_RETURN   , kbEnter      , 0   }  /* Return, enter           */
  , { VK_PAUSE    , kbPause      , 0   }  /* Pause, hold             */
  , { VK_RIGHT    , kbRight      , 0   }  /* Move right, right arrow */
  , { VK_DELETE   , kbDelete     , 0   }  /* Delete, rubout          */
  , { VK_INSERT   , kbInsert     , 0   }  /* Insert, insert here     */
  , { VK_PRINT    , kbPrnScr     , 0   }
  , { VK_BACK     , kbBackSpace  , 0   }  /* back space, back char   */

  , { VK_F1       , kbF1         , 0   }
  , { VK_F2       , kbF2         , 0   }
  , { VK_F3       , kbF3         , 0   }
  , { VK_F4       , kbF4         , 0   }
  , { VK_F5       , kbF5         , 0   }
  , { VK_F6       , kbF6         , 0   }
  , { VK_F7       , kbF7         , 0   }
  , { VK_F8       , kbF8         , 0   }
  , { VK_F9       , kbF9         , 0   }
  , { VK_F10      , kbF10        , 0   }
  , { VK_F11      , kbF11        , 0   }
  , { VK_F12      , kbF12        , 0   }

  , { '0'         , kb0          , '0' }
  , { '1'         , kb1          , '1' }
  , { '2'         , kb2          , '2' }
  , { '3'         , kb3          , '3' }
  , { '4'         , kb4          , '4' }
  , { '5'         , kb5          , '5' }
  , { '6'         , kb6          , '6' }
  , { '7'         , kb7          , '7' }
  , { '8'         , kb8          , '8' }
  , { '9'         , kb9          , '9' }
  , { VK_NUMPAD0  , kb0          , '0' }
  , { VK_NUMPAD1  , kb1          , '1' }
  , { VK_NUMPAD2  , kb2          , '2' }
  , { VK_NUMPAD3  , kb3          , '3' }
  , { VK_NUMPAD4  , kb4          , '4' }
  , { VK_NUMPAD5  , kb5          , '5' }
  , { VK_NUMPAD6  , kb6          , '6' }
  , { VK_NUMPAD7  , kb7          , '7' }
  , { VK_NUMPAD8  , kb8          , '8' }
  , { VK_NUMPAD9  , kb9          , '9' }

  , { 'A'         , kbA          , 'A' }
  , { 'B'         , kbB          , 'B' }     
  , { 'C'         , kbC          , 'C' }     
  , { 'D'         , kbD          , 'D' }     
  , { 'E'         , kbE          , 'E' }     
  , { 'F'         , kbF          , 'F' }     
  , { 'G'         , kbG          , 'G' }     
  , { 'H'         , kbH          , 'H' }     
  , { 'I'         , kbI          , 'I' }     
  , { 'J'         , kbJ          , 'J' }     
  , { 'K'         , kbK          , 'K' }
  , { 'L'         , kbL          , 'L' }     
  , { 'M'         , kbM          , 'M' }     
  , { 'N'         , kbM          , 'N' }     
  , { 'O'         , kbO          , 'O' }     
  , { 'P'         , kbP          , 'P' }     
  , { 'Q'         , kbQ          , 'Q' }     
  , { 'R'         , kbR          , 'R' }     
  , { 'S'         , kbS          , 'S' }     
  , { 'T'         , kbT          , 'T' }     
  , { 'U'         , kbU          , 'U' }     
  , { 'V'         , kbV          , 'V' }     
  , { 'W'         , kbW          , 'W' }     
  , { 'X'         , kbX          , 'X' }     
  , { 'Y'         , kbY          , 'Y' }     
  , { 'Z'         , kbZ          , 'Z' }    

  , { VK_SPACE    , kbSpace      , ' ' }  /* 20     */
  , { '!'         , kbAdmid      , '!' }  /* 0x0050 */
  , { '"'         , kbDobleQuote , '"' }  /* 0x0051 */
  , { '#'         , kbNumeral    , '#' }  /* 0x0052 */
  , { '$'         , kbDolar      , '$' }  /* 0x0053 */
  , { '%'         , kbPercent    , '%' }  /* 0x0054 */
  , { '&'         , kbAmper      , '&' }  /* 0x0055 */
  , { '\''        , kbQuote      , '\''}  /* 0x002e */
  , { '('         , kbOpenPar    , '(' }  /* 0x0056 */
  , { ')'         , kbClosePar   , ')' }  /* 0x0057 */
  , { VK_MULTIPLY , kbAsterisk   , '*' }  /* 6A */
  , { VK_ADD      , kbPlus       , '+' }  /* 6B */
  , { ','         , kbComma      , ',' }  /* 0x0030 */
  , { '-'         , kbMinus      , '-' }  /* 6D */
  , { '.'         , kbStop       , '.' }  
  , { VK_DIVIDE   , kbSlash      , '/' }  /* 6F     */


  , { ':'         , kbDoubleDot  , ':' }  /* 0x0058 */
  , { ';'         , kbColon      , ';' }  /* 0x002d */
  , { '<'         , kbLessThan   , '<' }  /* 0x0059 */
  , { '='         , kbEqual      , '=' }  /* 0x0038 */
  , { '>'         , kbGreaterThan, '>' }  /* 0x005a */
  , { '?'         , kbQuestion   , '?' }  /* 0x005b */
  , { '@'         , kbA_Roba     , '@' }  /* 0x005c */

  , { '['         , kbOpenBrace  , '[' }   /* 0x001B */
  , { VK_SEPARATOR, kbBackSlash  , '\\'}   /* 6C   */
  , { ']'         , kbCloseBrace , ']' }   /* 0x001D */
  , { '^'         , kbCaret      , '^' }   /* 0x004f */
  , { '_'         , kbUnderLine  , '_' }   /* 0x005e */
  , { VK_DECIMAL  , kbGrave      , '`' }   /* 6E     */

  , { '{'         , kbOpenCurly  , '{' }   /* 0x005f */
  , { '|'         , kbOr         , '|' }   /* 0x005d */
  , { '}'         , kbCloseCurly , '}' }   /* 0x0060 */
  , { '~'         , kbTilde      , '~' }   /* 0x0061 */
  , { ''         , kbMacro      , '' }   
  , { 'Ç'         , kbBackSpace  , 'Ç' }  
   

  , {   0         , 0            ,  0  }}; /* Terminator */

/* ------------------------------------------------------------------------- */
   int TGKeyWinGr::setKey( int vcode )
/* ------------------------------------------------------------------------- */
{ ConvKeyRec * ptr;

  for( ptr= xlateTable   /* Scan the special key table */
     ; ptr->x11
     ; ptr++ )
  { if (ptr->x11 == vcode )
    { storedEvent.what= evKeyDown;
      storedEvent.keyDown.shiftState= keyMask;
      storedEvent.keyDown.keyCode= ptr->scan | keyMask;
      storedEvent.keyDown.charScan.scanCode= ptr->scan;
      storedEvent.keyDown.charScan.charCode= ptr->ascii;
      return( ptr->ascii ); }}

  return(0); }

#include <stdio.h>

/* ------------------------------------------------------------------------- */
   int TGKeyWinGr::testEvents( UINT   message
                               , WPARAM wParam
			       , LPARAM lParam )
/* ------------------------------------------------------------------------- */
{ switch( message )
  { case WM_KEYUP:
      switch( wParam )
      { case VK_SHIFT  : keyMask &= ~kbShiftCode;   break;
	case VK_CONTROL: keyMask &= ~kbCtrlCode;    break;
	case VK_CAPITAL: keyMask &= ~kbCapsState;   break;
	case VK_SCROLL : keyMask &= ~kbScrollState; break; }
    break;                   

    case WM_KEYDOWN:
      switch( wParam )
      { case VK_SHIFT  : keyMask |= kbShiftCode;   break;
	case VK_CONTROL: keyMask |= kbCtrlCode;    break;
        case VK_CAPITAL: keyMask |= kbCapsState;   break;
	case VK_SCROLL : keyMask |= kbScrollState; break; }
      setKey( wParam );
      if ( storedEvent.keyDown.charScan.charCode )
      { storedEvent.what= evNothing; }
    break;

    case WM_CHAR:      
      storedEvent.what= evKeyDown;
      storedEvent.keyDown.shiftState= keyMask;
      storedEvent.keyDown.charScan.charCode= ( wParam<32 && wParam!=9 ) ? 0 : wParam;
      storedEvent.keyDown.keyCode= /* keyMask | wParam */ 0;
      storedEvent.keyDown.raw_scanCode= ( lParam >> 16 ) & 0xFF;
      storedEvent.keyDown.charScan.scanCode= 0;

      if ( remapKey )
      { if ( wParam>127 )
        { storedEvent.keyDown.charScan.charCode= remapKey->Font[ wParam - 128 ]; }}
        
    return( 2 );

    case WM_SYSKEYDOWN:
      setKey( wParam );
      storedEvent.keyDown.keyCode   |= kbAltLCode;
      storedEvent.keyDown.shiftState|= kbAltLCode;
    case WM_SYSCHAR:  /* (fixme) Block windows system keys, fix: not all of then must be blocked */
    return( 2 ); }

  storedEvent.keyDown.raw_scanCode= ( lParam >> 16 ) & 0xFF;
  return(0); }



/* -------------------------------------------------------------------------- */
   int TGKeyWinGr::KbHit(void)
/* -------------------------------------------------------------------------- */
{ do
  { switch( storedEvent.what )    /* Is there a previous event?        */
    { case evCommand:                /* Windows says "exit"        */
      case evKeyDown:             /* Key event pending                 */
	return( evKeyDown );      /* There is a key event waiting      */

      case evMouseUp:
      case evMouseDown:
      case evMouseMove:
        return( evMouseDown ); }} /* Not strictily necessary, but I do */
  while ( processEvent() );       /* Swallow new events                */

  return(0); }                    /* No events on queue                */


/* ------------------------------------------------------------------------- */
   void TGKeyWinGr::FillTEvent( TEvent &me )
/* ------------------------------------------------------------------------- */
{ me.what= evNothing;                /* The default is no message  */

  do
  { switch( storedEvent.what )       /* Is there a previous event? */
    { case evCommand:                /* Windows says "exit"        */
      case evKeyDown:                /* Key event pending          */
	me= storedEvent;             /* Give stored event          */
	storedEvent.what= evNothing; /* Mark events exhausted      */

      case evMouseUp:
      case evMouseDown:
      case evMouseMove:
      return; }}                     /* All done, exit function */
  while ( processEvent() ); }        /* Swallow new events      */


/* ------------------------------------------------------------------------- */
   void TGKeyWinGr::init( )
/* ------------------------------------------------------------------------- */
{ TGKey::kbhit=         KbHit;
  TGKey::fillTEvent=    FillTEvent;
  TGKey::getShiftState= GetShiftState;

/*
 *   JASC, whe need a lot of work here since codepage returned by
 * GetOEMCP() doesnot match TV codesymbols. only ISO8879_1 tested.
 * if your keyboard provides another mapping, choice one from
 * available in codepage.cc, test and publish to rhide mailing list o to me
 * (jasanchez@polar.es or tachungo@yahoo.es )
 */

  switch ( GetOEMCP() )
  { case 737: remapKey= &stPC737; break; // Greek (formerly 437G)
    case 775: remapKey= &stPC775; break; // Baltic
    case 850: remapKey= &ISO8879_1; break; // MS-DOS Multilingual (Latin I) ( JASC, only this is tested )
    case 852: remapKey= &stPC852; break; // MS-DOS Slavic (Latin II)
    case 855: remapKey= &stPC855; break; // IBM Cyrillic (primarily Russian)
    case 857: remapKey= &stPC857; break; // IBM Turkish
    case 860: remapKey= &stPC860; break; // MS-DOS Portuguese
    case 861: remapKey= &stPC861; break; // MS-DOS Icelandic
    case 863: remapKey= &stPC863; break; // MS-DOS Canadian-French
    case 865: remapKey= &stPC865; break; // MS-DOS Nordic
    case 866: remapKey= &stPC866; break; // MS-DOS Russian (former USSR)
    case 869: remapKey= &stPC869; break; // IBM Modern Greek


/*
 *  Not supported by SET's code pagger
 */
    case  708: // Arabic (ASMO 708)
    case  709: // Arabic (ASMO 449+, BCON V4)
    case  710: // Arabic (Transparent Arabic)
    case  720: // Arabic (Transparent ASMO)
    case  862: // Hebrew
    case  864: // Arabic
    case  874: // Thai
    case  932: // Japan
    case  936: // Chinese (PRC, Singapore)
    case  949: // Korean
    case  950: // Chinese (Taiwan, Hong Kong)
    case 1361: // Korean (Johab)

    case  437: remapKey= NULL; }}  // MS-DOS United States, native format


#else

#include <tv/wingr/screen.h>
#include <tv/wingr/key.h>
#include <tv/wingr/mouse.h>

#endif // TVOS_Win32

