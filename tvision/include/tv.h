/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.

 *
 *
 */

#define _TV_VERSION 0x0103

#define Uses_EventCodes
#define Uses_ViewCommands
#define Uses_ipstream
#define Uses_opstream
#define __INC_STDDLG_H

#if defined( Uses_TGKey )
#define __INC_GKEY_H
#endif

#if defined( Uses_TFileViewer )
#define Uses_TCollection
#define Uses_TScroller
#define Uses_TWindow
#define __INC_FILEVIEW_H
#endif

#if defined( Uses_TRangeValidator )
#define Uses_TFilterValidator
#endif

#if defined( Uses_TFilterValidator )
#define Uses_TValidator
#endif

#if defined( Uses_TValidator )
#define Uses_TObject
#if !defined( NO_STREAM )
#define Uses_TStreamable
#endif
#define __INC_VALID_H
#endif

#if defined( Uses_TApplication )
#define Uses_TProgram
#define __INC_APP_H
#endif

#if defined( Uses_TProgram )
#define Uses_TEvent
#define Uses_TGroup
#define __INC_APP_H
#define __INC_TIME_H
#endif

#if defined( Uses_TDeskTop )
#define Uses_TGroup
#define __INC_APP_H
#endif

#if defined( Uses_TBackground )
#define Uses_TView
#define __INC_APP_H
#endif

#if defined( Uses_TReplaceDialogRec )
#define __INC_FINDREPL_H
#endif

#if defined( Uses_TFindDialogRec )
#define __INC_FINDREPL_H
#endif

#if defined( Uses_TIndicator )
#define Uses_TView
#define __INC_INDICATO_H
#endif

#if defined( Uses_TTerminal )
#define Uses_TTextDevice
#define __INC_TEXTVIEW_H
#endif

#if defined( Uses_TTextDevice )
#define Uses_TScroller
#define __INC_TEXTVIEW_H
#endif

#if defined( Uses_TStatusLine )
#define Uses_TView
#define __INC_MENUS_H
#endif

#if defined( Uses_TStatusDef )
#define __INC_MENUS_H
#endif

#if defined( Uses_TStatusItem )
#define __INC_MENUS_H
#endif

#if defined( Uses_TMenuBox )
#define Uses_TMenuView
#define __INC_MENUS_H
#endif

#if defined( Uses_TMenuBar )
#define Uses_TMenuView
#define __INC_MENUS_H
#endif

#if defined( Uses_TMenuView )
#define Uses_TView
#define __INC_MENUS_H
#endif

#if defined( Uses_TMenu )
#define Uses_TMenuItem
#define __INC_MENUS_H
#endif

#if defined( Uses_TSubMenu )
#define Uses_TMenuItem
#define __INC_MENUS_H
#endif

#if defined( Uses_TMenuItem )
#define __INC_MENUS_H
#endif

#if defined( Uses_TColorDialog )
#define Uses_TDialog
#define __INC_COLORSEL_H
#endif

#if defined( Uses_TColorItemList )
#define Uses_TListViewer
#define __INC_COLORSEL_H
#endif

#if defined( Uses_TColorGroupList )
#define Uses_TListViewer
#define __INC_COLORSEL_H
#endif

#if defined( Uses_TColorDisplay )
#define Uses_TView
#define __INC_COLORSEL_H
#endif

#if defined( Uses_TMonoSelector )
#define __INC_COLORSEL_H
#endif

#if defined( Uses_TMonoSelector )
#define Uses_TCluster
#define __INC_COLORSEL_H
#endif

#if defined( Uses_TColorSelector )
#define Uses_TView
#define __INC_COLORSEL_H
#endif

#if defined( Uses_TColorGroup )
#define __INC_COLORSEL_H
#endif

#if defined( Uses_TColorItem )
#define __INC_COLORSEL_H
#endif

#if defined( Uses_TChDirDialog )
#define Uses_TDialog
#define __INC_STDDLG_H
#endif

#if defined( Uses_TDirListBox )
#define Uses_TListBox
#define __INC_STDDLG_H
#endif

#if defined( Uses_TDirCollection )
#define Uses_TCollection
#define Uses_TDirEntry
#define __INC_STDDLG_H
#endif

#if defined( Uses_TDirEntry )
#define __INC_STDDLG_H
#endif

#if defined( Uses_TFileDialog )
#define Uses_TDialog
#define __INC_STDDLG_H
#endif

#if defined( Uses_TFileInfoPane )
#define Uses_TView
#define Uses_TSearchRec
#define __INC_STDDLG_H
#endif

#if defined( Uses_TFileList )
#define Uses_TSortedListBox
#define Uses_TFileCollection
#define Uses_TSearchRec
#define __INC_STDDLG_H
#endif

#if defined( Uses_TSortedListBox )
#define Uses_TListBox
#define __INC_STDDLG_H
#endif

#if defined( Uses_TFileCollection )
#define Uses_TSortedCollection
#define Uses_TSearchRec
#define __INC_STDDLG_H
#endif

#if defined( Uses_TFileInputLine )
#define Uses_TInputLine
#define __INC_STDDLG_H
#endif

#if defined( Uses_TSearchRec )
#define __INC_STDDLG_H
#endif

#if defined( Uses_THistory )
#define Uses_TView
#define __INC_DIALOGS_H
#endif

#if defined( Uses_THistoryWindow )
#define Uses_TWindow
#define __INC_DIALOGS_H
#endif

#if defined( Uses_THistoryViewer )
#define Uses_TListViewer
#define __INC_DIALOGS_H
#endif

#if defined( Uses_TLabel )
#define Uses_TStaticText
#define __INC_DIALOGS_H
#endif

#if defined( Uses_TParamText )
#define Uses_TStaticText
#define __INC_DIALOGS_H
#endif

#if defined( Uses_TStaticText )
#define Uses_TView
#define __INC_DIALOGS_H
#endif

#if defined( Uses_TListBox )
#define Uses_TListViewer
#define __INC_DIALOGS_H
#endif

#if defined( Uses_TCheckBoxes )
#define Uses_TCluster
#define __INC_DIALOGS_H
#endif

#if defined( Uses_TRadioButtons )
#define Uses_TCluster
#define __INC_DIALOGS_H
#endif

#if defined( Uses_TCluster )
#define Uses_TView
#define __INC_DIALOGS_H
#endif

#if defined( Uses_TSItem )
#define __INC_DIALOGS_H
#endif

#if defined( Uses_TButton )
#define Uses_TView
#define __INC_DIALOGS_H
#endif

#if defined( Uses_TInputLine )
#define Uses_TView
#define __INC_DIALOGS_H
#endif

#if defined( Uses_TDialog )
#define Uses_TWindow
#define __INC_DIALOGS_H
#endif

#if defined( Uses_TWindow )
#define Uses_TGroup
#define __INC_VIEWS_H
#endif

#if defined( Uses_TGroup )
#define Uses_TView
#define Uses_TRect
#define __INC_VIEWS_H
#endif

#if defined( Uses_TListViewer )
#define Uses_TView
#define __INC_VIEWS_H
#endif

#if defined( Uses_TScroller )
#define Uses_TView
#define Uses_TPoint
#define __INC_VIEWS_H
#endif

#if defined( Uses_TScrollBar )
#define Uses_TView
#define __INC_VIEWS_H
#endif

#if defined( Uses_TFrame )
#define Uses_TView
#define __INC_VIEWS_H
#endif

#if defined( Uses_TView )
#define Uses_TObject
#if !defined( NO_STREAM )
#define Uses_TStreamable
#endif
#define Uses_TDrawBuffer
#define Uses_TPoint
#define __INC_VIEWS_H
#endif

#if defined( Uses_TPalette )
#define __INC_VIEWS_H
#endif

#if defined( Uses_TCommandSet )
#define __INC_VIEWS_H
#endif

#if defined( Uses_ViewCommands )
#define __INC_VIEWS_H
#endif

#if defined( Uses_TStrListMaker )
#define Uses_TObject
#if !defined( NO_STREAM )
#define Uses_TStreamable
#endif
#define Uses_TStrIndexRec
#define __INC_RESOURCE_H
#endif

#if defined( Uses_TStringList )
#define Uses_TObject
#if !defined( NO_STREAM )
#define Uses_TStreamable
#endif
#define __INC_RESOURCE_H
#endif

#if defined( Uses_TStrIndexRec )
#define __INC_RESOURCE_H
#endif

#if defined( Uses_TResourceFile )
#define Uses_TObject
#define __INC_RESOURCE_H
#endif

#if defined( Uses_TResourceItem )
#define __INC_RESOURCE_H
#endif

#if defined( Uses_TResourceCollection )
#define Uses_TStringCollection
#define __INC_RESOURCE_H
#endif

#if defined( Uses_TStringCollection )
#define Uses_TSortedCollection
#define __INC_RESOURCE_H
#endif

#if defined( Uses_MsgBox )
#define __INC_MSGBOX_H
#endif

#if defined( Uses_TScreen )
#define __INC_SYSTEM_H
#endif

#if defined( Uses_TEventQueue )
#define Uses_TEvent
#define __INC_SYSTEM_H
#endif

#if defined( Uses_TEvent )
#define Uses_TPoint
#define __INC_SYSTEM_H
#endif

#if defined( Uses_EventCodes )
#define __INC_SYSTEM_H
#endif

#if defined( Uses_TSortedCollection )
#define Uses_TNSSortedCollection
#define Uses_TCollection
#define __INC_OBJECTS_H
#endif

#if defined( Uses_TCollection )
#define Uses_TNSCollection
#if !defined( NO_STREAM )
#define Uses_TStreamable
#endif
#define __INC_OBJECTS_H
#endif

#if defined( Uses_TRect )
#define Uses_TPoint
#define __INC_OBJECTS_H
#endif

#if defined( Uses_TPoint )
#define __INC_OBJECTS_H
#endif

#if defined( Uses_TDrawBuffer )
#define __INC_DRAWBUF_H
#endif

#if defined( Uses_fpstream )
#define Uses_fpbase
#define Uses_iopstream
#define __INC_TOBJSTRM_H
#endif

#if defined( Uses_ofpstream )
#define Uses_fpbase
#define __INC_TOBJSTRM_H
#endif

#if defined( Uses_ifpstream )
#define Uses_fpbase
#define __INC_TOBJSTRM_H
#endif

#if defined( Uses_fpbase )
#define Uses_pstream
#define __INC_TOBJSTRM_H
#endif

#if defined( Uses_iopstream )
#define __INC_TOBJSTRM_H
#endif

#if defined( Uses_opstream )
#define Uses_pstream
#define __INC_TOBJSTRM_H
#endif

#if defined( Uses_ipstream )
#define Uses_pstream
#define __INC_TOBJSTRM_H
#endif

#if defined( Uses_pstream )
#define __INC_TOBJSTRM_H
#endif

#if defined( Uses_TPReadObjects )
#define Uses_TNSCollection
#define __INC_TOBJSTRM_H
#endif

#if defined( Uses_TPWrittenObjects )
#define Uses_TNSSortedCollection
#define __INC_TOBJSTRM_H
#endif

#if defined( Uses_TStreamableTypes )
#define Uses_TNSSortedCollection
#define __INC_TOBJSTRM_H
#endif

#if defined( Uses_TStreamableClass )
#define __INC_TOBJSTRM_H
#endif

#if defined( Uses_TStreamable )
#define __INC_TOBJSTRM_H
#endif

#if defined( Uses_TNSSortedCollection )
#define Uses_TNSCollection
#define __INC_TVOBJS_H
#endif

#if defined( Uses_TNSCollection )
#define Uses_TObject
#define __INC_TVOBJS_H
#endif

#if defined( Uses_TObject )
#define __INC_TVOBJS_H
#endif

#if defined( Uses_TKeys )
#define __INC_TKEYS_H
#endif

#include <tvconfig.h>
#include <ttypes.h>

#if defined( __INC_TIME_H )
#include <time.h>
#endif

#if defined( __INC_TKEYS_H )
#include <tkeys.h>
#endif

#if defined( __INC_GKEY_H )
#include <gkey.h>
#endif

#include <tvutil.h>

#if defined( __INC_TVOBJS_H )
#include <tvobjs.h>
#endif

#if defined( __INC_TOBJSTRM_H )
#include <tobjstrm.h>
#endif

#if defined( __INC_DRAWBUF_H )
#include <drawbuf.h>
#endif

#if defined( __INC_OBJECTS_H )
#include <objects.h>
#endif

#if defined( __INC_VALID_H)
#include <validate.h>
#endif

#if defined( __INC_SYSTEM_H )
#include <system.h>
#endif

#if defined( __INC_MSGBOX_H )
#include <msgbox.h>
#endif

#if defined( __INC_RESOURCE_H )
#include <resource.h>
#endif

#if defined( __INC_VIEWS_H )
#include <views.h>
#endif

#if defined( __INC_DIALOGS_H )
#include <dialogs.h>
#endif

#if defined( __INC_STDDLG_H )
#include <stddlg.h>
#endif

#if defined( __INC_COLORSEL_H )
#include <colorsel.h>
#endif

#if defined( __INC_MENUS_H )
#include <menus.h>
#endif

#if defined( __INC_TEXTVIEW_H )
#include <textview.h>
#endif

#if defined( __INC_INDICATO_H )
#include <indicato.h>
#endif

#if defined( __INC_FINDREPL_H )
#include <findrepl.h>
#endif

#if defined( __INC_APP_H )
#include <app.h>
#endif

#if defined( __INC_FILEVIEW_H )
#include <fileview.h>
#endif
