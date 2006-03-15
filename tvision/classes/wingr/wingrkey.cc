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
       sprintf( buff
         , " SHIFT %u"                 
           " CHARCODE %u"                 
           " KEYCODE %u"                 
           " SCANCODE %u         "                 
         , keyMask
         , storedEvent.keyDown.charScan.charCode
         , storedEvent.keyDown.keyCode  
         , storedEvent.keyDown.charScan.scanCode  );
                          
       TScreenWinGr::writeLine( 1
                              , 7
                              , strlen(buff)
                              , buff
                              , 0xF1 ); 
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
 *
 * , { '{'         , kbOpenCurly  , '{', 0 }    0x005f 
 * , { '|'         , kbOr         , '|', 0 }    0x005d 
 * , { '}'         , kbCloseCurly , '}', 0 }    0x0060 
 * , { '~'         , kbTilde      , '~', 0 }    0x0061 
 * , { ''         , kbMacro      , '', 0 }   
 * , { 'Ç'         , kbBackSpace  , 'Ç', 0 }  
 *, { '!'         , kbAdmid      , '!', 0 }    0x0050 
 *, { '"'         , kbDobleQuote , '"', 0 }    0x0051 
 *, { '#'         , kbNumeral    , '#', 0 }    0x0052 
 * , { '$'         , kbDolar      , '$', 0 }   0x0053 
 * , { '%'         , kbPercent    , '%', 0 }   0x0054 
 * , { '&'         , kbAmper      , '&', 0 }   0x0055 
 * , { '\''        , kbQuote      , '\'', 0}   0x002e 
 * , { '('         , kbOpenPar    , '(', 0 }   0x0056 
 * , { ')'         , kbClosePar   , ')', 0 }   0x0057 
 *  , { ':'         , kbDoubleDot  , ':', 0 }  0x0058 
 * , { ';'         , kbColon      , ';', 0 }   0x002d 
 * , { '<'         , kbLessThan   , '<', 0 }   0x0059 
 * , { '='         , kbEqual      , '=', 0 }   0x0038 
 * , { '>'         , kbGreaterThan, '>', 0 }   0x005a 
 * , { '?'         , kbQuestion   , '?', 0 }   0x005b 
 * , { '@'         , kbA_Roba     , '@', 0 }   0x005c 

 * , { '['         , kbOpenBrace  , '[', 0 }   0x001B 
 * , { ']'         , kbCloseBrace , ']', 0 }   0x001D 
 * , { '^'         , kbCaret      , '^', 0 }   0x004f 
 * , { '_'         , kbUnderLine  , '_', 0 }   0x005e */
  
/*, { '0'               , kb0          , '0', 0x45 }
, { '1'         , kb1          , '1', 0x16 }
, { '2'         , kb2          , '2', 0x1E }
, { '3'         , kb3          , '3', 0x26 }
, { '4'         , kb4          , '4', 0x25 }
, { '5'         , kb5          , '5', 0x2E }
, { '6'         , kb6          , '6', 0x36 }
, { '7'         , kb7          , '7', 0x3D }
, { '8'         , kb8          , '8', 0x3E }
, { '9'         , kb9          , '9', 0x46 }
, { VK_NUMPAD0  , kb0          , '0', 0x70 }
, { VK_NUMPAD1  , kb1          , '1', 0x69 }
, { VK_NUMPAD2  , kb2          , '2', 0x72 }
, { VK_NUMPAD3  , kb3          , '3', 0x7A }
, { VK_NUMPAD4  , kb4          , '4', 0x6B }
, { VK_NUMPAD5  , kb5          , '5', 0x73 }
, { VK_NUMPAD6  , kb6          , '6', 0x74 }
, { VK_NUMPAD7  , kb7          , '7', 0x6C }
, { VK_NUMPAD8  , kb8          , '8', 0x75 }
, { VK_NUMPAD9  , kb9          , '9', 0x7D }

, { 'A'        , kbA          , 'A', 0x1C }
, { 'B'        , kbB          , 'B', 0x32 }     
, { 'C'        , kbC          , 'C', 0x21 }     
, { 'D'        , kbD          , 'D', 0x23 }     
, { 'E'        , kbE          , 'E', 0x24 }     
, { 'F'        , kbF          , 'F', 0x2B }     
, { 'G'        , kbG          , 'G', 0x34 }     
, { 'H'        , kbH          , 'H', 0x33 }     
, { 'I'        , kbI          , 'I', 0x43 }     
, { 'J'        , kbJ          , 'J', 0x3B }     
, { 'K'        , kbK          , 'K', 0x42 }
, { 'L'        , kbL          , 'L', 0x4B }     
, { 'M'        , kbM          , 'M', 0x3A }     
, { 'N'        , kbM          , 'N', 0x31 }     
, { 'O'        , kbO          , 'O', 0x44 }     
, { 'P'        , kbP          , 'P', 0x4D }     
, { 'Q'        , kbQ          , 'Q', 0x15 }     
, { 'R'        , kbR          , 'R', 0x2D }     
, { 'S'        , kbS          , 'S', 0x1B }     
, { 'T'        , kbT          , 'T', 0x2C }     
, { 'U'        , kbU          , 'U', 0x3C }     
, { 'V'        , kbV          , 'V', 0x2A }     
, { 'W'        , kbW          , 'W', 0x1D }     
, { 'X'        , kbX          , 'X', 0x22 }     
, { 'Y'        , kbY          , 'Y', 0x35 }     
, { 'Z'        , kbZ          , 'Z', 0x1A }    
{ VK_SPACE    , kbSpace      , ' ', 0x29 }  
//, { VK_MULTIPLY , kbAsterisk   , '*', 0 } 
//, { VK_ADD      , kbPlus       , '+', 0 } 
//, { VK_SEPARATOR, kbBackSlash  , ',', 0 } 
//, { VK_SUBTRACT , kbMinus      , '-', 0 } 
//, { VK_DECIMAL  , kbGrave      , '.', 0 }  
//, { VK_DIVIDE   , kbSlash      , '/', 0 } 
//, { VK_SEPARATOR, kbBackSlash  , '\\', 0} 
//, { VK_DECIMAL  , kbGrave      , '`', 0 } 

 */


ConvKeyRec xlateTableKeys[]=
{{ ' '        , kbSpace       }
,{ VK_UP       , kbUp         }  /* Move up, up arrow       */
,{ VK_ESCAPE   , kbEsc        }
,{ VK_TAB      , kbTab        }
,{ VK_HOME     , kbHome       }
,{ VK_END      , kbEnd        }  /* EOL                     */
,{ VK_LEFT     , kbLeft       }  /* Move left, left arrow   */
,{ VK_RIGHT    , kbRight      }  /* Move right, right arrow */
,{ VK_UP       , kbUp         }  /* Move down, down arrow   */
,{ VK_DOWN     , kbDown       }  /* Move down, down arrow   */
,{ VK_PRIOR    , kbPgUp       }
,{ VK_NEXT     , kbPgDn       }
,{ VK_RETURN   , kbEnter      }  /* Return, enter           */
,{ VK_PAUSE    , kbPause      }  /* Pause, hold             */
,{ VK_DELETE   , kbDelete     }  /* Delete, rubout          */
,{ VK_INSERT   , kbInsert     }  /* Insert, insert here     */
,{ VK_PRINT    , kbPrnScr     }
,{ VK_BACK     , kbBackSpace  }  /* back space, back char   */

,{ VK_F1       , kbF1         }
,{ VK_F2       , kbF2         }
,{ VK_F3       , kbF3         }
,{ VK_F4       , kbF4         }
,{ VK_F5       , kbF5         }
,{ VK_F6       , kbF6         }
,{ VK_F7       , kbF7         }
,{ VK_F8       , kbF8         }
,{ VK_F9       , kbF9         }
,{ VK_F10      , kbF10        }
,{ VK_F11      , kbF11        }
,{ VK_F12      , kbF12        }
,{   0         , 0            }}; /* Terminator */

ConvKeyRec xlateTableChars[]=
{{ ' '        , kbSpace       }
,{ '0'        , kb0           }     
,{ '1'        , kb1           }     
,{ '2'        , kb2           }     
,{ '3'        , kb3           }     
,{ '4'        , kb4           }     
,{ '5'        , kb5           }     
,{ '6'        , kb6           }     
,{ '7'        , kb7           }     
,{ '8'        , kb8           }     
,{ '9'        , kb9           }     
,{ 'A'        , kbA           }
,{ 'B'        , kbB           }     
,{ 'C'        , kbC           }     
,{ 'D'        , kbD           }     
,{ 'E'        , kbE           }     
,{ 'F'        , kbF           }     
,{ 'G'        , kbG           }     
,{ 'H'        , kbH           }     
,{ 'I'        , kbI           }     
,{ 'J'        , kbJ           }     
,{ 'K'        , kbK           }
,{ 'L'        , kbL           }     
,{ 'M'        , kbM           }     
,{ 'N'        , kbM           }     
,{ 'O'        , kbO           }     
,{ 'P'        , kbP           }     
,{ 'Q'        , kbQ           }     
,{ 'R'        , kbR           }     
,{ 'S'        , kbS           }     
,{ 'T'        , kbT           }     
,{ 'U'        , kbU           }     
,{ 'V'        , kbV           }     
,{ 'W'        , kbW           }     
,{ 'X'        , kbX           }     
,{ 'Y'        , kbY           }     
,{ 'Z'        , kbZ           }    
,{   0         , 0            }}; /* Terminator */


/*
 *  Driver entry, tells caller last know shift state
 *
 * 
 *  those are not totally implemeted on TVISION
 *
 *
 *  GetKeyState( VK_CAPITAL      ) & TOGGLED_KEY ? kbCapsLockToggle   : 0 
 *  GetKeyState( VK_INSERT       ) & TOGGLED_KEY ? kbInsertToggle     : 0 
 * 
 *  GetKeyState( VK_SCROLL        ) & PRESSED_KEY ? kbScrollLockDown  : 0
 *  GetKeyState( VK_CAPITAL  ) & PRESSED_KEY ? kbCapsLockDown     : 0 
 *  GetKeyState( VK_NUMLOCK  ) & PRESSED_KEY ? kbNumLockDown      : 0 
 *  GetKeyState( VK_SNAPSHOT ) & PRESSED_KEY ? kbSysReqPress      : 0 
 * 
 *  GetKeyState( VK_SCROLL       ) & TOGGLED_KEY ? kbScrollLockToggle : 0
 * 
 *  GetKeyState( VK_NUMLOCK      ) & TOGGLED_KEY ? kbNumLockToggle    : 0  
 */     


#define TOGGLED_KEY 0x01 /* The key is toggled */ 
#define PRESSED_KEY 0x80 /* The key is pressed */

unsigned TGKeyWinGr::GetShiftState()
{ return( keyMask );
}  


int TGKeyWinGr::setKey( const ConvKeyRec * xlate
                      , int vcode )
{ const ConvKeyRec * ptr;

  for( ptr= xlate   /* Scan the special key table */
     ; ptr->win
     ; ptr++ )
  { if (ptr->win == vcode )
    { storedEvent.what= evKeyDown;
      storedEvent.keyDown.keyCode= ptr->key | keyMask;
      storedEvent.keyDown.charScan.charCode= 0;
      return( vcode );
    }
  }
  return(0); 
}

#include <stdio.h>

/*
 *
 */ 
int TGKeyWinGr::testEvents( UINT   message
                          , WPARAM wParam
                          , LPARAM lParam )
{ storedEvent.what= evNothing;

  switch( message )
  { case WM_CHAR:      
    case WM_KEYDOWN:
    case WM_SYSCHAR:
    case WM_SYSKEYDOWN:
      keyMask=                  /* Adapt keymask to TVISION */
       (( GetKeyState( VK_LSHIFT  ) & PRESSED_KEY ? kbShiftCode : 0 )
       |( GetKeyState( VK_RSHIFT  ) & PRESSED_KEY ? kbShiftCode : 0 )
       |( GetKeyState( VK_SHIFT   ) & PRESSED_KEY ? kbShiftCode : 0 )
       |( GetKeyState( VK_LCONTROL) & PRESSED_KEY ? kbCtrlCode  : 0 )
       |( GetKeyState( VK_RCONTROL) & PRESSED_KEY ? kbCtrlCode  : 0 )
       |( GetKeyState( VK_CONTROL ) & PRESSED_KEY ? kbCtrlCode  : 0 )
       |( GetKeyState( VK_LMENU   ) & PRESSED_KEY ? kbAltLCode  : 0 )
       |( GetKeyState( VK_RMENU   ) & PRESSED_KEY ? kbAltRCode  : 0 ));

       if ( keyMask & kbAltRCode )
       { keyMask&= ~kbCtrlCode;
       }
       storedEvent.keyDown.shiftState= keyMask;
       storedEvent.keyDown.charScan.scanCode=
       storedEvent.keyDown.raw_scanCode= ( lParam >> 16 ) & 0xFF;
  }
     

  switch( message )
  {  case WM_SYSKEYDOWN:
     case WM_KEYDOWN:

       if ( keyMask )                  /* key + alt or ctrl */
       { if ( setKey( xlateTableChars
                    , wParam ))
         { return( 1 );
         }                  
       }
       
       if ( setKey( xlateTableKeys
                  , wParam ))
       { if ( keyMask & kbAltRCode )  // Emulate alt for system keys
         { keyMask                    &= ~kbAltRCode;
           storedEvent.keyDown.keyCode&= ~kbAltRCode;
           keyMask                    |=  kbAltLCode;
           storedEvent.keyDown.keyCode|=  kbAltLCode;
         }
         else
         { if ( keyMask & kbAltLCode )  // Windows event ?
           { storedEvent.what= evNothing;                // Give event to win
             return( 0 );
           }  
         }
       
         return( 1 );  /* good event */
       }
    break; 

    case WM_SYSCHAR:  /* (fixme) Block windows system keys, fix: not all of then must be blocked */
    return( 1 );
    
    case WM_CHAR:      
       if( wParam >= 32 )                                          
       { storedEvent.what= evKeyDown;
         storedEvent.keyDown.charScan.charCode= wParam;
         storedEvent.keyDown.keyCode= storedEvent.keyDown.shiftState;
       }      
       
/*      if ( remapKey )
      { if ( wParam>127 )
        { storedEvent.keyDown.charScan.charCode= remapKey->Font[ wParam - 128 ]; 
        }
      }
      */
    return( 2 );  /* good event */
   
  }  

  return(0); 
}



/* 
 *
 */

int TGKeyWinGr::KbHit(void)
{ do
  {  
    switch( storedEvent.what )    /* Is there a previous event?        */
    { case evCommand:             /* Windows says "exit"               */
      case evKeyDown:             /* Key event pending                 */
        return( evKeyDown );      /* There is a key event waiting      */

      case evMouseUp:
      case evMouseDown:
      case evMouseMove:
        return( evMouseDown );
    } /* Not strictily necessary, but I do */
  }
  while ( processEvent() );       /* Swallow new events                */

  return(0); }                    /* No events on queue                */


/* 
 *
 */

void TGKeyWinGr::FillTEvent( TEvent &me )
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
      return;                        /* All done, exit function */
    }  
  }  
  while ( processEvent() );          /* Swallow new events      */
}  


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

