/* Modified by Robert Hoehne and Salvador Eduardo Tropea for the gcc port */
/* Modified by Vadim Beloborodov to be used on WIN32 console */
/* Modified to compile with gcc v3.x by Salvador E. Tropea, with the help of
   Andris Pavenis. */
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
#define Uses_StrStream
#define Uses_iomanip

#include <tv.h>
#include <stdarg.h>

TMenuBar *TEditorApp::initMenuBar( TRect r )
{

      TSubMenu& sub1 = *new TSubMenu( _("~F~ile"), kbAltF ) +
        *new TMenuItem( _("~O~pen"), cmOpen, kbF3 ) +
        *new TMenuItem( _("~N~ew"), cmNew, kbNoKey ) +
        *new TMenuItem( _("~S~ave"), cmSave, kbF2, hcNoContext, "F2" ) +
        *new TMenuItem( _("S~a~ve as..."), cmSaveAs, kbNoKey ) +
             newLine() +
        *new TMenuItem( _("~C~hange dir..."), cmChangeDrct, kbNoKey ) +
        *new TMenuItem( _("S~h~ell"), cmCallShell, kbNoKey ) +
        *new TMenuItem( _("E~x~it"), cmQuit, kbAltX, hcNoContext, "Alt+X" );

      TSubMenu& sub2 = *new TSubMenu( _("~E~dit"), kbAltE ) +
        *new TMenuItem( _("~U~ndo"), cmUndo, kbNoKey ) +
             newLine() +
        *new TMenuItem( _("Cu~t~"), cmCut, kbShiftDel, hcNoContext, "Shift+Del" ) +
        *new TMenuItem( _("~C~opy"), cmCopy, kbCtrlIns, hcNoContext, "Ctrl+Ins" ) +
        *new TMenuItem( _("~P~aste"), cmPaste, kbShiftIns, hcNoContext, "Shift+Ins" ) +
        *new TMenuItem( _("~S~how clipboard"), cmShowClip, kbNoKey ) +
             newLine() +
        *new TMenuItem( _("~C~lear"), cmClear, kbCtrlDel, hcNoContext, "Ctrl+Del" );

      TSubMenu& sub3 = *new TSubMenu( _("~S~earch"), kbAltS ) +
        *new TMenuItem( _("~F~ind..."), cmFind, kbNoKey ) +
        *new TMenuItem( _("~R~eplace..."), cmReplace, kbNoKey ) +
        *new TMenuItem( _("~S~earch again"), cmSearchAgain, kbNoKey );

      TSubMenu& sub4 = *new TSubMenu( _("~W~indows"), kbAltW ) +
        *new TMenuItem( _("~S~ize/move"),cmResize, kbCtrlF5, hcNoContext, "Ctrl+F5" ) +
        *new TMenuItem( _("~Z~oom"), cmZoom, kbF5, hcNoContext, "F5" ) +
        *new TMenuItem( _("~T~ile"), cmTile, kbNoKey ) +
        *new TMenuItem( _("C~a~scade"), cmCascade, kbNoKey ) +
        *new TMenuItem( _("~N~ext"), cmNext, kbF6, hcNoContext, "F6" ) +
        *new TMenuItem( _("~P~revious"), cmPrev, kbShiftF6, hcNoContext, "Shift+F6" ) +
        *new TMenuItem( _("~C~lose"), cmClose, kbAltF3, hcNoContext, "Alt+F3" );

      /*TSubMenu& sub5 = *new TSubMenu( _("~O~ptions"), kbAltO )
        + *new TMenuItem( _("~M~acros"), cmMacros, kbNoKey );*/

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
            *new TStatusItem( _("~F10~ Menu"), kbF10, cmMenu ) +
            *new TStatusItem( _("~F2~ Save"), kbF2, cmSave ) +
            *new TStatusItem( _("~F3~ Open"), kbF3, cmOpen ) +
            *new TStatusItem( _("~Alt+F3~ Close"), kbAltF3, cmClose ) +
            *new TStatusItem( _("~F5~ Zoom"), kbF5, cmZoom ) +
            *new TStatusItem( _("~F6~ Next"), kbF6, cmNext ) +
            *new TStatusItem( 0, kbCtrlF5, cmResize )
            );

}

void TEditorApp::outOfMemory()
{
    messageBox(_("Not enough memory for this operation."), mfError | mfOKButton );
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
    CreateStrStream(os,buf,PATH_MAX+80);
    switch( dialog )
        {
        case edOutOfMemory:
            return messageBox( _("Not enough memory for this operation."),
                               mfError | mfOKButton );
        case edReadError:
            {
            os << _("Error reading file ") << va_arg( arg, _charPtr )
               << "." << CLY_std(ends);
            va_end(arg);
            return messageBox( GetStrStream(os,buf), mfError | mfOKButton );
            }
        case edWriteError:
            {
            os << _("Error writing file ") << va_arg( arg,_charPtr )
               << "." << CLY_std(ends);
            va_end( arg );
            return messageBox( GetStrStream(os,buf), mfError | mfOKButton );
            }
        case edCreateError:
            {
            os << _("Error creating file ") << va_arg( arg, _charPtr )
               << "." << CLY_std(ends);
            va_end( arg );
            return messageBox( GetStrStream(os,buf), mfError | mfOKButton );
            }
        case edSaveModify:
            {
            os << va_arg( arg, _charPtr )
               << _(" has been modified. Save?") << CLY_std(ends);
            va_end( arg );
            return messageBox( GetStrStream(os,buf), mfInformation | mfYesNoCancel );
            }
        case edSaveUntitled:
            return messageBox( _("Save untitled file?"),
                               mfInformation | mfYesNoCancel );
        case edSaveAs:
            {
            return execDialog( new TFileDialog( "*",
                                                _("Save file as"),
                                                _("~N~ame"),
                                                fdOKButton,
                                                101 ), va_arg( arg, _charPtr ) );
            }

        case edFind:
            {
            return execDialog( createFindDialog(), va_arg( arg, _charPtr ) );
            }

        case edSearchFailed:
            return messageBox( _("Search string not found."),
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
            return messageBoxRect( r, _("Replace this occurence?"),
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


