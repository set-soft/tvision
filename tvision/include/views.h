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

#if !defined( __COMMAND_CODES )
#define __COMMAND_CODES

const ushort

//  Standard command codes

    cmValid         = 0,
    cmQuit          = 1,
    cmError         = 2,
    cmMenu          = 3,
    cmClose         = 4,
    cmZoom          = 5,
    cmResize        = 6,
    cmNext          = 7,
    cmPrev          = 8,
    cmHelp          = 9,

//  TDialog standard commands

    cmOK            = 10,
    cmCancel        = 11,
    cmYes           = 12,
    cmNo            = 13,
    cmDefault       = 14,

//  TView State masks 

    sfVisible       = 0x001,
    sfCursorVis     = 0x002,
    sfCursorIns     = 0x004,
    sfShadow        = 0x008,
    sfActive        = 0x010,
    sfSelected      = 0x020,
    sfFocused       = 0x040,
    sfDragging      = 0x080,
    sfDisabled      = 0x100,
    sfModal         = 0x200,
    sfDefault       = 0x400,
    sfExposed       = 0x800,

// TView Option masks 

    ofSelectable    = 0x001,
    ofTopSelect     = 0x002,
    ofFirstClick    = 0x004,
    ofFramed        = 0x008,
    ofPreProcess    = 0x010,
    ofPostProcess   = 0x020,
    ofBuffered      = 0x040,
    ofTileable      = 0x080,
    ofCenterX       = 0x100,
    ofCenterY       = 0x200,
    ofCentered      = 0x300,

// TView GrowMode masks 

    gfGrowLoX       = 0x01,
    gfGrowLoY       = 0x02,
    gfGrowHiX       = 0x04,
    gfGrowHiY       = 0x08,
    gfGrowAll       = 0x0f,
    gfGrowRel       = 0x10,

// TView DragMode masks 

    dmDragMove      = 0x01,
    dmDragGrow      = 0x02,
    dmLimitLoX      = 0x10,
    dmLimitLoY      = 0x20,
    dmLimitHiX      = 0x40,
    dmLimitHiY      = 0x80,
    dmLimitAll      = dmLimitLoX | dmLimitLoY | dmLimitHiX | dmLimitHiY,

// TView Help context codes

    hcNoContext     = 0,
    hcDragging      = 1,

// TScrollBar part codes 

    sbLeftArrow     = 0,
    sbRightArrow    = 1,
    sbPageLeft      = 2,
    sbPageRight     = 3,
    sbUpArrow       = 4,
    sbDownArrow     = 5,
    sbPageUp        = 6,
    sbPageDown      = 7,
    sbIndicator     = 8,

// TScrollBar options for TWindow.StandardScrollBar 

    sbHorizontal    = 0x000,
    sbVertical      = 0x001,
    sbHandleKeyboard = 0x002,

// TWindow Flags masks

    wfMove          = 0x01,
    wfGrow          = 0x02,
    wfClose         = 0x04,
    wfZoom          = 0x08,

//  TView inhibit flags

    noMenuBar       = 0x0001,
    noDeskTop       = 0x0002,
    noStatusLine    = 0x0004,
    noBackground    = 0x0008,
    noFrame         = 0x0010,
    noViewer        = 0x0020,
    noHistory       = 0x0040,

// TWindow number constants

    wnNoNumber      = 0,

// TWindow palette entries 

    wpBlueWindow    = 0,
    wpCyanWindow    = 1,
    wpGrayWindow    = 2,

//  Application command codes

    cmCut           = 20,
    cmCopy          = 21,
    cmPaste         = 22,
    cmUndo          = 23,
    cmClear         = 24,
    cmTile          = 25,
    cmCascade       = 26,

// Standard messages 

    cmReceivedFocus     = 50,
    cmReleasedFocus     = 51,
    cmCommandSetChanged = 52,

// TScrollBar messages 

    cmScrollBarChanged  = 53,
    cmScrollBarClicked  = 54,

// TWindow select messages 

    cmSelectWindowNum   = 55,

//  TListViewer messages

    cmListItemSelected  = 56,

//  Event masks

    positionalEvents    = evMouse,
    focusedEvents       = evKeyboard | evCommand;

#endif  // __COMMAND_CODES

#if defined( Uses_TCommandSet )

#include <cmdset.h>

#endif

#if defined( Uses_TPalette )

#include <palette.h>

#endif

#if defined( Uses_TView )

#include <view.h>

#endif

#if defined( Uses_TFrame )

#include <frame.h>

#endif

#if defined( Uses_TScrollBar )

#include <scrlbar.h>

#endif

#if defined( Uses_TScroller )

#include <scroller.h>

#endif

#if defined( Uses_TListViewer )

#include <lstviewr.h>

#endif

#if defined( Uses_TGroup )

#include <group.h>

#endif

#if defined( Uses_TWindow )

#include <window.h>

#endif


