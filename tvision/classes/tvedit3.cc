/* Modified by Robert Hoehne and Salvador Eduardo Tropea for the gcc port */
/* Modified by Vadim Beloborodov to be used on WIN32 console */
/* Modified to compile with gcc v3.x by Salvador E. Tropea, with the help of
   Andris Pavenis. */
/* Modified by Salvador E. Tropea to use messageBox with formating. */
/*----------------------------------------------------------*/
/*                                                          */
/*   Turbo Vision 1.0                                       */
/*   Copyright (c) 1991 by Borland International            */
/*                                                          */
/*   Turbo Vision TVEDIT source file                        */
/*----------------------------------------------------------*/

// SET: Internationalized, changed Alt- by Alt+

#define Uses_TApplication
#define Uses_TMenuBar
#define Uses_TRect
#define Uses_TSubMenu
#define Uses_TKeys
#define Uses_TMenuItem
#define Uses_TStatusLine
#define Uses_TStatusItem
#define Uses_TStatusDef
#define Uses_TPoint
#define Uses_TEditor
#define Uses_MsgBox
#define Uses_TFileDialog
#define Uses_TDeskTop
#define Uses_TEditorApp
#define Uses_TVEdConstant
#define Uses_stdarg

#include <tv.h>

TMenuBar *TEditorApp::initMenuBar( TRect r )
{

      TSubMenu& sub1 = *new TSubMenu( __("~F~ile"), kbAltF ) +
        *new TMenuItem( __("~O~pen"), cmOpen, kbF3, hcNoContext, "F3" ) +
        *new TMenuItem( __("~N~ew"), cmNew, kbNoKey ) +
        *new TMenuItem( __("~S~ave"), cmSave, kbF2, hcNoContext, "F2" ) +
        *new TMenuItem( __("S~a~ve as..."), cmSaveAs, kbNoKey ) +
             newLine() +
        *new TMenuItem( __("~C~hange dir..."), cmChangeDrct, kbNoKey ) +
        *new TMenuItem( __("S~h~ell"), cmCallShell, kbNoKey ) +
        *new TMenuItem( __("E~x~it"), cmQuit, kbAltX, hcNoContext, "Alt+X" );

      TSubMenu& sub2 = *new TSubMenu( __("~E~dit"), kbAltE ) +
        *new TMenuItem( __("~U~ndo"), cmUndo, kbNoKey ) +
             newLine() +
        *new TMenuItem( __("Cu~t~"), cmCut, kbShiftDel, hcNoContext, "Shift+Del" ) +
        *new TMenuItem( __("~C~opy"), cmCopy, kbCtrlIns, hcNoContext, "Ctrl+Ins" ) +
        *new TMenuItem( __("~P~aste"), cmPaste, kbShiftIns, hcNoContext, "Shift+Ins" ) +
        *new TMenuItem( __("~S~how clipboard"), cmShowClip, kbNoKey ) +
             newLine() +
        *new TMenuItem( __("~C~lear"), cmClear, kbCtrlDel, hcNoContext, "Ctrl+Del" );

      TSubMenu& sub3 = *new TSubMenu( __("~S~earch"), kbAltS ) +
        *new TMenuItem( __("~F~ind..."), cmFind, kbNoKey ) +
        *new TMenuItem( __("~R~eplace..."), cmReplace, kbNoKey ) +
        *new TMenuItem( __("~S~earch again"), cmSearchAgain, kbNoKey );

      TSubMenu& sub4 = *new TSubMenu( __("~W~indows"), kbAltW ) +
        *new TMenuItem( __("~S~ize/move"),cmResize, kbCtrlF5, hcNoContext, "Ctrl+F5" ) +
        *new TMenuItem( __("~Z~oom"), cmZoom, kbF5, hcNoContext, "F5" ) +
        *new TMenuItem( __("~T~ile"), cmTile, kbNoKey ) +
        *new TMenuItem( __("C~a~scade"), cmCascade, kbNoKey ) +
        *new TMenuItem( __("~N~ext"), cmNext, kbF6, hcNoContext, "F6" ) +
        *new TMenuItem( __("~P~revious"), cmPrev, kbShiftF6, hcNoContext, "Shift+F6" ) +
        *new TMenuItem( __("~C~lose"), cmClose, kbAltF3, hcNoContext, "Alt+F3" );

      /*TSubMenu& sub5 = *new TSubMenu( __("~O~ptions"), kbAltO )
        + *new TMenuItem( __("~M~acros"), cmMacros, kbNoKey );*/

    r.b.y = r.a.y+1;
    return new TMenuBar( r, sub1 + sub2 + sub3 + sub4
                            //+ sub5
                        );
}

TStatusLine *TEditorApp::initStatusLine( TRect r )
{
    r.a.y = r.b.y-1;
    return new TStatusLine( r,
        *new TStatusDef( 0, 0xFFFF ) +
            // SET: F10 before the others so when the library maintains the
            // cursor visible this option is pointed and the user have a
            // hint about what to do.
            *new TStatusItem( __("~F10~ Menu"), kbF10, cmMenu ) +
            *new TStatusItem( __("~F2~ Save"), kbF2, cmSave ) +
            *new TStatusItem( __("~F3~ Open"), kbF3, cmOpen ) +
            *new TStatusItem( __("~Alt+F3~ Close"), kbAltF3, cmClose ) +
            *new TStatusItem( __("~F5~ Zoom"), kbF5, cmZoom ) +
            *new TStatusItem( __("~F6~ Next"), kbF6, cmNext ) +
            *new TStatusItem( 0, kbCtrlF5, cmResize )
            );

}

void TEditorApp::outOfMemory()
{
    messageBox(__("Not enough memory for this operation."), mfError | mfOKButton );
}

typedef char *_charPtr;
typedef TPoint *PPoint;

/**[txh]********************************************************************

  Description:
  This function provides the default dialog actions.@p
  SET: I modified it to be easy to overwrite some actions. See the real
function.

***************************************************************************/

ushort doEditDialogDefault( int dialog, va_list arg )
{
    char *s;
    switch( dialog )
        {
        case edOutOfMemory:
            return messageBox( __("Not enough memory for this operation."),
                               mfError | mfOKButton );
        case edReadError:
            {
            s=va_arg( arg, _charPtr );
            va_end(arg);
            return messageBox( mfError | mfOKButton,
                               __("Error reading file %s."), s );
            }
        case edWriteError:
            {
            s=va_arg( arg, _charPtr );
            va_end( arg );
            return messageBox( mfError | mfOKButton,
                               __("Error writing file %s."), s );
            }
        case edCreateError:
            {
            s=va_arg( arg, _charPtr );
            va_end( arg );
            return messageBox( mfError | mfOKButton,
                               __("Error creating file %s."), s );
            }
        case edSaveModify:
            {
            s=va_arg( arg, _charPtr );
            va_end( arg );
            return messageBox( mfInformation | mfYesNoCancel,
                               __("%s has been modified. Save?"), s );
            }
        case edSaveUntitled:
            return messageBox( __("Save untitled file?"),
                               mfInformation | mfYesNoCancel );
        case edSaveAs:
            {
            return execDialog( new TFileDialog( "*",
                                                __("Save file as"),
                                                __("~N~ame"),
                                                fdOKButton,
                                                101 ), va_arg( arg, _charPtr ) );
            }

        case edFind:
            {
            return execDialog( createFindDialog(), va_arg( arg, _charPtr ) );
            }

        case edSearchFailed:
            return messageBox( __("Search string not found."),
                               mfError | mfOKButton );
        case edReplace:
            {
            return execDialog( createReplaceDialog(), va_arg( arg, _charPtr ) );
            }

        case edReplacePrompt:
          {
            //  Avoid placing the dialog on the same line as the cursor
            TRect r( 0, 1, 40, 8 );
            r.move( (TProgram::deskTop->size.x-r.b.x)/2, 0 );
            TPoint t = TProgram::deskTop->makeGlobal( r.b );
            t.y++;
            TPoint *pt = va_arg( arg, PPoint );
            if( pt->y <= t.y )
                r.move( 0, TProgram::deskTop->size.y - r.b.y - 2 );
            va_end( arg );
            return messageBoxRect( r, __("Replace this occurence?"),
                                   mfYesNoCancel | mfInformation );
          }
        default:
            return cmCancel;
        }
}

ushort doEditDialog( int dialog, ... )
{ // Just call the default
 va_list arg;

 va_start(arg, dialog);
 ushort ret=doEditDialogDefault(dialog,arg);
 va_end(arg);
 return ret;
}


