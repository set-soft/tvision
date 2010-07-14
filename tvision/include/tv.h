/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.
Modified by Salvador E. Tropea

 *
 *
 */

// SET: We still need to add a lot of TV 2.0 functionality
#define _TV_VERSION 0x0103

// SET: Internal release number. Used to check the version of the release
// in programs that are very sensitive, like my editor.
#define TV_MAJOR_VERSION  2
#define TV_MIDDLE_VERSION 2
#define TV_MINOR_VERSION  1
#define TV_VERSION "2.2.1"

#define Uses_EventCodes
#define Uses_ViewCommands
#define Uses_ipstream
#define Uses_opstream
#define INCLUDE_STDDLG_H

// SET: Start of the TEditor classes re-incorporated in 1.0.4
#ifdef Uses_TEditWindow
#define Uses_TWindow
#define Uses_TFileEditor
#endif

#ifdef Uses_TFileEditor
#define Uses_TEditor
#define Uses_limits
#endif

#ifdef Uses_TMemo
#define Uses_TEditor
#endif

#ifdef Uses_TEditor
#define Uses_TView
#define Uses_TPalette
#define Uses_TCommandSet
#define INCL_EDITORS_H
#endif

#ifdef Uses_TVConfigFile
#define INCL_CONFIGFILE_H
#define Uses_stdio
#endif

#ifdef Uses_TVOSClipboard
#define INCL_OSCLIPBOARD_H
#endif

#ifdef Uses_TCalculator
#define Uses_TCalcDisplay
#define Uses_TDialog
#endif

#ifdef Uses_TCalcDisplay
#define Uses_TView
#define INCL_CALC_H
#define INCLUDE_STDLIB_H
#endif

#ifdef Uses_TEditorApp
#define Uses_TApplication
#define INCL_TVEDIT_H
#endif
// SET: End of the TEditor classes

// SET: Start of the THelp* classes
#ifdef Uses_THelpViewer
 #define Uses_THelpWindow
#endif

#ifdef Uses_THelpWindow
 #define Uses_THelpFile
 #define Uses_TWindow
 #define Uses_TScroller
 #define Uses_TScrollBar
 #define Uses_TEvent
 #define INCLUDE_HELP_H
#endif

#ifdef Uses_THelpFile
 #define Uses_TStreamable
 #define Uses_ipstream
 #define Uses_opstream
 #define Uses_fpstream
 #define Uses_TObject
 #define Uses_TPoint
 #define Uses_TRect
 #define INCLUDE_HELPBASE_H
#endif
// SET: End of the THelp* classes

#if defined( Uses_TVFontCollection )
 #define Uses_TStringCollection
 #define Uses_TSortedListBox
 #define Uses_stdio
 #define INCLUDE_FONTCOLL_H
#endif

#if defined( Uses_TGKey )
#define INCLUDE_GKEY_H
#define Uses_TVCodePage
#endif

#if defined( Uses_TFileViewer )
#define Uses_TCollection
#define Uses_TScroller
#define Uses_TWindow
// ssize_t
#define Uses_sys_types
#define INCLUDE_FILEVIEW_H
#endif

#if defined( Uses_TPXPictureValidator )
#define Uses_TValidator
#endif

#if defined( Uses_TStringLookupValidator )
#define Uses_TStringCollection
#define Uses_TLookupValidator
#endif

#if defined( Uses_TLookupValidator )
#define Uses_TValidator
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
#define INCLUDE_VALID_H
#endif

#if defined( Uses_TApplication )
#define Uses_TProgram
#define INCLUDE_APP_H
#endif

#if defined( Uses_TProgram )
#define Uses_TEvent
#define Uses_TGroup
#define Uses_allegro // For END_OF_MAIN
#define INCLUDE_APP_H
#define INCLUDE_TIME_H
#endif

#if defined( Uses_TDeskTop )
#define Uses_TGroup
#define INCLUDE_APP_H
#endif

#if defined( Uses_TBackground )
#define Uses_TView
#define INCLUDE_APP_H
#endif

#if defined( Uses_TReplaceDialogRec )
#define INCLUDE_FINDREPL_H
#define Uses_string
#endif

#if defined( Uses_TFindDialogRec )
#define INCLUDE_FINDREPL_H
#define Uses_string
#endif

#if defined( Uses_TIndicator )
#define Uses_TView
#define INCLUDE_INDICATO_H
#endif

#if defined( Uses_TTerminal )
#define Uses_TTextDevice
#define INCLUDE_TEXTVIEW_H
#endif

#if defined( Uses_TTextDevice )
#define Uses_TScroller
#define Uses_iostream
#define Uses_stdio // EOF
#define INCLUDE_TEXTVIEW_H
#endif

#if defined( Uses_TStatusLine )
#define Uses_TView
#define INCLUDE_MENUS_H
#endif

#if defined( Uses_TStatusDef )
#define INCLUDE_MENUS_H
#endif

#if defined( Uses_TStatusItem )
#define INCLUDE_MENUS_H
#endif

#if defined( Uses_TMenuBox )
#define Uses_TMenuView
#define INCLUDE_MENUS_H
#endif

#if defined( Uses_TMenuBar )
#define Uses_TMenuView
#define INCLUDE_MENUS_H
#endif

#if defined( Uses_TMenuView )
#define Uses_TView
#define INCLUDE_MENUS_H
#endif

#if defined( Uses_TMenu )
#define Uses_TMenuItem
#define INCLUDE_MENUS_H
#endif

#if defined( Uses_TSubMenu )
#define Uses_TMenuItem
#define INCLUDE_MENUS_H
#endif

#if defined( Uses_TMenuItem )
#define INCLUDE_MENUS_H
#endif

// SET: Just include the file to get the cm* definitions, it includes a
// Broadcast for colors changed.
#ifdef Uses_TColorCommands
#define INCLUDE_COLORSEL_H
#endif

#if defined( Uses_TColorDialog )
#define Uses_TDialog
#define INCLUDE_COLORSEL_H
#endif

#if defined( Uses_TColorItemList )
#define Uses_TListViewer
#define INCLUDE_COLORSEL_H
#endif

#if defined( Uses_TColorGroupList )
#define Uses_TListViewer
#define INCLUDE_COLORSEL_H
#endif

#if defined( Uses_TColorDisplay )
#define Uses_TView
#define INCLUDE_COLORSEL_H
#endif

#if defined( Uses_TMonoSelector )
#define INCLUDE_COLORSEL_H
#endif

#if defined( Uses_TMonoSelector )
#define Uses_TCluster
#define INCLUDE_COLORSEL_H
#endif

#if defined( Uses_TColorSelector )
#define Uses_TView
#define INCLUDE_COLORSEL_H
#endif

#if defined( Uses_TColorGroup )
#define INCLUDE_COLORSEL_H
#endif

#if defined( Uses_TColorItem )
#define INCLUDE_COLORSEL_H
#endif

#if defined( Uses_TChDirDialog )
#define Uses_TDialog
#define INCLUDE_STDDLG_H
#endif

#if defined( Uses_TDirListBox )
#define Uses_TListBox
#define Uses_limits
#define INCLUDE_STDDLG_H
#endif

#if defined( Uses_TDirCollection )
#define Uses_TCollection
#define Uses_TDirEntry
#define INCLUDE_STDDLG_H
#endif

#if defined( Uses_TDirEntry )
#define INCLUDE_STDDLG_H
#endif

#if defined( Uses_TFileDialog )
#define Uses_TDialog
#define INCLUDE_STDDLG_H
#define Uses_limits
#endif

#if defined( Uses_TFileInfoPane )
#define Uses_TView
#define Uses_TSearchRec
#define INCLUDE_STDDLG_H
#define INCLUDE_TIME_H
#endif

#if defined( Uses_TFileList )
#define Uses_TSortedListBox
#define Uses_TFileCollection
#define Uses_TSearchRec
#define INCLUDE_STDDLG_H
#endif

#if defined( Uses_TSortedListBox )
#define Uses_TListBox
#define INCLUDE_STDDLG_H
#endif

#if defined( Uses_TFileCollection )
#define Uses_TSortedCollection
#define Uses_TSearchRec
#define INCLUDE_STDDLG_H
#endif

#if defined( Uses_TFileInputLine )
#define Uses_TInputLine
#define INCLUDE_STDDLG_H
#endif

#if defined( Uses_TSearchRec )
#define INCLUDE_STDDLG_H
#define INCLUDE_TIME_H
#define Uses_limits
#endif

#if defined( Uses_THistory )
#define Uses_TView
#define INCLUDE_DIALOGS_H
#endif

#if defined( Uses_THistoryWindow )
#define Uses_TWindow
#define INCLUDE_DIALOGS_H
#endif

#if defined( Uses_THistoryViewer )
#define Uses_TListViewer
#define INCLUDE_DIALOGS_H
#endif

#if defined( Uses_T1Label )
#define Uses_TLabel
#endif

#if defined( Uses_TLabel )
#define Uses_TStaticText
#define INCLUDE_DIALOGS_H
#endif

#if defined( Uses_TParamText )
#define Uses_TStaticText
#define INCLUDE_DIALOGS_H
#endif

#ifdef Uses_T1StaticText
#define Uses_string
#define Uses_TStaticText
#endif

#if defined( Uses_TStaticText )
#define Uses_TView
#define INCLUDE_DIALOGS_H
#endif

#if defined( Uses_TListBox )
#define Uses_TListViewer
#define INCLUDE_DIALOGS_H
#endif

#if defined( Uses_TCheckBoxes )
#define Uses_TCluster
#define INCLUDE_DIALOGS_H
#endif

#if defined( Uses_TRadioButtons )
#define Uses_TCluster
#define INCLUDE_DIALOGS_H
#endif

#if defined( Uses_TCluster )
#define Uses_TView
#define INCLUDE_DIALOGS_H
#endif

#if defined( Uses_TSItem )
#define INCLUDE_DIALOGS_H
#endif

#if defined( Uses_TButton )
#define Uses_TView
#define INCLUDE_DIALOGS_H
#endif

#if defined( Uses_TInput1Line )
#define Uses_TInputLine
#endif

#if defined( Uses_TInputLine )
#define Uses_TView
#define INCLUDE_DIALOGS_H
#endif

#if defined( Uses_TDialog )
#define Uses_TWindow
#define INCLUDE_DIALOGS_H
#endif

#if defined( Uses_TWindow )
#define Uses_TGroup
#define INCLUDE_VIEWS_H
#endif

#if defined( Uses_TGroup )
#define Uses_TView
#define Uses_TRect
#define INCLUDE_VIEWS_H
#endif

#if defined( Uses_TListViewer )
#define Uses_TView
#define INCLUDE_VIEWS_H
#endif

#if defined( Uses_TScroller )
#define Uses_TView
#define Uses_TPoint
#define INCLUDE_VIEWS_H
#endif

#if defined( Uses_TScrollBar )
#define Uses_TView
#define INCLUDE_VIEWS_H
#endif

#if defined( Uses_TFrame )
#define Uses_TView
#define INCLUDE_VIEWS_H
#endif

#if defined( Uses_TView )
#define Uses_TObject
#if !defined( NO_STREAM )
#define Uses_TStreamable
#endif
#define Uses_TDrawBuffer
#define Uses_TPoint
#define INCLUDE_VIEWS_H
#endif

#if defined( Uses_TPalette )
#define INCLUDE_VIEWS_H
#endif

#if defined( Uses_TCommandSet )
#define INCLUDE_VIEWS_H
#endif

#if defined( Uses_ViewCommands )
#define INCLUDE_VIEWS_H
#endif

#if defined( Uses_TStrListMaker )
#define Uses_TObject
#if !defined( NO_STREAM )
#define Uses_TStreamable
#endif
#define Uses_TStrIndexRec
#define Uses_TStringList // Forced by TStringList::name!
#define INCLUDE_RESOURCE_H
#endif

#if defined( Uses_TStringList )
#define Uses_TObject
#if !defined( NO_STREAM )
#define Uses_TStreamable
#endif
#define INCLUDE_RESOURCE_H
#endif

#if defined( Uses_TStrIndexRec )
#define INCLUDE_RESOURCE_H
#endif

#if defined( Uses_TResourceFile )
#define Uses_TObject
#define INCLUDE_RESOURCE_H
#endif

#if defined( Uses_TResourceItem )
#define INCLUDE_RESOURCE_H
#endif

#if defined( Uses_TResourceCollection )
#define Uses_TStringCollection
#define INCLUDE_RESOURCE_H
#endif

#if defined( Uses_TStringCollection )
#define Uses_TSortedCollection
#define INCLUDE_RESOURCE_H
#endif

#if defined( Uses_MsgBox )
#define INCLUDE_MSGBOX_H
#endif

#if defined( Uses_TScreen )
#define INCLUDE_SYSTEM_H
#endif

#if defined( Uses_TEventQueue )
#define Uses_TEvent
#define INCLUDE_SYSTEM_H
#endif

#if defined( Uses_TEvent )
#define Uses_TPoint
#define INCLUDE_SYSTEM_H
#endif

#if defined( Uses_EventCodes )
#define INCLUDE_SYSTEM_H
#endif

#if defined( Uses_TVCodePage )
#define INCLUDE_CODEPAGE_H
#define Uses_TVPartitionTree556
#endif

#if defined( Uses_TSortedCollection )
#define Uses_TNSSortedCollection
#define Uses_TCollection
#define INCLUDE_OBJECTS_H
#endif

#if defined( Uses_TCollection )
#define Uses_TNSCollection
#if !defined( NO_STREAM )
#define Uses_TStreamable
#endif
#define INCLUDE_OBJECTS_H
#endif

#if defined( Uses_TRect )
#define Uses_TPoint
#define INCLUDE_OBJECTS_H
#endif

#if defined( Uses_TPoint )
#define INCLUDE_OBJECTS_H
#endif

#if defined( Uses_TDrawBuffer )
#define INCLUDE_DRAWBUF_H
#endif

#if defined( Uses_fpstream )
#define Uses_fpbase
#define Uses_iopstream
#define INCLUDE_TOBJSTRM_H
#endif

#if defined( Uses_ofpstream )
#define Uses_fpbase
#define Uses_iostream_simple
#define INCLUDE_TOBJSTRM_H
#endif

#if defined( Uses_ifpstream )
#define Uses_fpbase
#define Uses_iostream_simple
#define INCLUDE_TOBJSTRM_H
#endif

#if defined( Uses_fpbase )
#define Uses_pstream
#define Uses_fstream_simple
#define INCLUDE_TOBJSTRM_H
#define Uses_FILEBUF_OPENPROT
#endif

#if defined( Uses_iopstream )
#define Uses_iostream_simple
#define INCLUDE_TOBJSTRM_H
#endif

#if defined( Uses_opstream )
#define Uses_pstream
#define INCLUDE_TOBJSTRM_H
#endif

#if defined( Uses_ipstream )
#define Uses_pstream
#define INCLUDE_TOBJSTRM_H
#endif

#if defined( Uses_pstream )
#define Uses_iostream_simple
#define INCLUDE_TOBJSTRM_H
#endif

#if defined( Uses_TPReadObjects )
#define Uses_TNSCollection
#define INCLUDE_TOBJSTRM_H
#endif

#if defined( Uses_TPWrittenObjects )
#define Uses_TNSSortedCollection
#define INCLUDE_TOBJSTRM_H
#endif

#if defined( Uses_TStreamableTypes )
#define Uses_TNSSortedCollection
#define INCLUDE_TOBJSTRM_H
#endif

#if defined( Uses_TStreamableClass )
#define INCLUDE_TOBJSTRM_H
#endif

#if defined( Uses_TStreamable )
#define INCLUDE_TOBJSTRM_H
#endif

#if defined( Uses_TNSSortedCollection )
#define Uses_TNSCollection
#define INCLUDE_TVOBJS_H
#endif

#if defined( Uses_TNSCollection )
#define Uses_TObject
#define INCLUDE_TVOBJS_H
#endif

#if defined( Uses_TObject )
#define INCLUDE_TVOBJS_H
#endif

#if defined( Uses_TKeys )
#define INCLUDE_TKEYS_H
#endif

#if defined( Uses_TVPartitionTree556 )
#define INCLUDE_PARTTREE_H
#define Uses_string
#endif

#if defined( INCLUDE_STDLIB_H )
#define Uses_stdlib
#endif

#if defined( INCLUDE_TIME_H )
#define Uses_time
#endif

// That's the file generated by the configure script:
//#include <tv/configtv.h> Included by compatlayer.h
// Compatibility layer
#include <compatlayer.h>
// This file sets some stuff according to the compiler:
#include <tv/tvconfig.h>
// Basic data types used in many places:
#include <tv/ttypes.h>

#if defined( INCLUDE_TKEYS_H )
#include <tv/tkeys.h>
#endif

#if defined( INCLUDE_PARTTREE_H )
#include <tv/parttree.h>
#endif

#if defined( INCLUDE_CODEPAGE_H )
#include <tv/codepage.h>
#endif

#if defined( INCLUDE_GKEY_H )
#include <tv/gkey.h>
#endif

#include <tv/tvutil.h>

#if defined( INCLUDE_TVOBJS_H )
#include <tv/tvobjs.h>
#endif

#if defined( INCLUDE_TOBJSTRM_H )
#include <tv/tobjstrm.h>
#endif

#if defined( INCLUDE_DRAWBUF_H )
#include <tv/drawbuf.h>
#endif

#if defined( INCLUDE_OBJECTS_H )
#include <tv/objects.h>
#endif

#if defined( INCLUDE_VALID_H)
#include <tv/validate.h>
#endif

#if defined( INCLUDE_SYSTEM_H )
#include <tv/system.h>
#endif

#if defined( INCLUDE_MSGBOX_H )
#include <tv/msgbox.h>
#endif

#if defined( INCLUDE_RESOURCE_H )
#include <tv/resource.h>
#endif

#if defined( INCLUDE_VIEWS_H )
#include <tv/views.h>
#endif

#if defined( INCLUDE_DIALOGS_H )
#include <tv/dialogs.h>
#endif

#if defined( INCLUDE_STDDLG_H )
#include <tv/stddlg.h>
#endif

#if defined( INCLUDE_COLORSEL_H )
#include <tv/colorsel.h>
#endif

#if defined( INCLUDE_MENUS_H )
#include <tv/menus.h>
#endif

#if defined( INCLUDE_TEXTVIEW_H )
#include <tv/textview.h>
#endif

#if defined( INCLUDE_INDICATO_H )
#include <tv/indicato.h>
#endif

#if defined( INCLUDE_FINDREPL_H )
#include <tv/findrepl.h>
#endif

#if defined( INCLUDE_APP_H )
#include <tv/app.h>
#endif

#if defined( INCLUDE_FILEVIEW_H )
#include <tv/fileview.h>
#endif

#ifdef INCLUDE_FONTCOLL_H
 #include <tv/fontcoll.h>
#endif

// SET: Start of the THelp* classes incorporated in 2.0
#ifdef INCLUDE_HELPBASE_H
 #include <tv/helpbase.h>
#endif

#ifdef INCLUDE_HELP_H
 #include <tv/help.h>
#endif
// SET: End of the THelp* classes

// SET: Start of the TEditor classes re-incorporated in 1.0.4
#ifdef INCL_EDITORS_H
#include <tv/editors.h>
#endif

#ifdef INCL_TVEDIT_H
#include <tv/tvedit.h>
#endif
// SET: End of the TEditor classes

// SET: Incorporated in 1.0.7
#ifdef INCL_CALC_H
#include <tv/calc.h>
#endif

#ifdef INCL_OSCLIPBOARD_H
#include <tv/osclipboard.h>
#endif

#ifdef INCL_CONFIGFILE_H
#include <tv/configfile.h>
#endif
