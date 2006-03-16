/*----------------------------------------------------------*/
/*                                                          */
/*   Turbo Vision TVDEMO source file                        */
/*                                                          */
/*----------------------------------------------------------*/
/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *
 */
/*
 * Modified by Sergio Sigala <ssigala@globalnet.it>
 * Added code page example by Salvador E. Tropea (SET)
 */

#define Uses_TRect
#define Uses_TMenuBar
#define Uses_TSubMenu
#define Uses_TMenuItem
#define Uses_TKeys
#define Uses_fpstream
#define Uses_TView
#define Uses_TPalette
#define Uses_MsgBox
#define Uses_TFileDialog
#define Uses_TApplication
#define Uses_TDeskTop
#define Uses_TStaticText
#define Uses_TDialog
#define Uses_TEventQueue
#define Uses_TEditor  // JASC 2006 cmOpen
// Needed to remap the "system" menu character
#define Uses_TVCodePage

#include <tv.h>

#include "tvdemo.h"
#include "tvcmds.h"
#include "gadgets.h"
#include "mousedlg.h"
#include "demohelp.h"
#include "fileview.h"
#include "calendar.h"

uchar TVDemo::systemMenuIcon[]="~\360~";
uchar TVDemo::osystemMenuIcon[]="~\360~";
TVCodePageCallBack TVDemo::oldCPCallBack=NULL;

//
// Mouse Control Dialog Box function
//

void TVDemo::mouse()
{
    TMouseDialog *mouseCage = (TMouseDialog *) validView( new TMouseDialog() );

    if (mouseCage != 0)
        {
        mouseCage->helpCtx = hcOMMouseDBox;
        mouseCage->setData(&(TEventQueue::mouseReverse));
        if (deskTop->execView(mouseCage) != cmCancel)
            mouseCage->getData(&(TEventQueue::mouseReverse));
        }
    CLY_destroy(mouseCage);
   
}


//
// File Viewer function
//

void TVDemo::openFile( char *fileSpec )
{
    TFileDialog *d= (TFileDialog *)validView(
    new TFileDialog(fileSpec, "Open a File", "~N~ame", fdOpenButton, 100 ));

    if( d != 0 && deskTop->execView( d ) != cmCancel )
        {
        char fileName[PATH_MAX];
        d->getFileName( fileName );
        d->helpCtx = hcFOFileOpenDBox;
        TView *w= validView( new TFileWindow( fileName ) );
        if( w != 0 )
            deskTop->insert(w);
    }
    CLY_destroy(d);
}


//
// "Out of Memory" function ( called by validView() )
//

void TVDemo::outOfMemory()
{
    messageBox( "Not enough memory available to complete operation.",
      mfError | mfOKButton );
}

//
// getPalette() function ( returns application's palette )
//
#define cpAppColor \
       "\x71\x70\x78\x74\x20\x28\x24\x17\x1F\x1A\x31\x31\x1E\x71\x1F" \
    "\x37\x3F\x3A\x13\x13\x3E\x21\x3F\x70\x7F\x7A\x13\x13\x70\x7F\x7E" \
    "\x70\x7F\x7A\x13\x13\x70\x70\x7F\x7E\x20\x2B\x2F\x78\x2E\x70\x30" \
    "\x3F\x3E\x1F\x2F\x1A\x20\x72\x31\x31\x30\x2F\x3E\x31\x13\x38\x00" \
    "\x17\x1F\x1A\x71\x71\x1E\x17\x1F\x1E\x20\x2B\x2F\x78\x2E\x10\x30" \
    "\x3F\x3E\x70\x2F\x7A\x20\x12\x31\x31\x30\x2F\x3E\x31\x13\x38\x00" \
    "\x37\x3F\x3A\x13\x13\x3E\x30\x3F\x3E\x20\x2B\x2F\x78\x2E\x30\x70" \
    "\x7F\x7E\x1F\x2F\x1A\x20\x32\x31\x71\x70\x2F\x7E\x71\x13\x78\x00" \
    "\x37\x3F\x3A\x13\x13\x30\x3E\x1E"    // help colors

#define cpAppBlackWhite \
       "\x70\x70\x78\x7F\x07\x07\x0F\x07\x0F\x07\x70\x70\x07\x70\x0F" \
    "\x07\x0F\x07\x70\x70\x07\x70\x0F\x70\x7F\x7F\x70\x07\x70\x07\x0F" \
    "\x70\x7F\x7F\x70\x07\x70\x70\x7F\x7F\x07\x0F\x0F\x78\x0F\x78\x07" \
    "\x0F\x0F\x0F\x70\x0F\x07\x70\x70\x70\x07\x70\x0F\x07\x07\x08\x00" \
    "\x07\x0F\x0F\x07\x70\x07\x07\x0F\x0F\x70\x78\x7F\x08\x7F\x08\x70" \
    "\x7F\x7F\x7F\x0F\x70\x70\x07\x70\x70\x70\x07\x7F\x70\x07\x78\x00" \
    "\x70\x7F\x7F\x70\x07\x70\x70\x7F\x7F\x07\x0F\x0F\x78\x0F\x78\x07" \
    "\x0F\x0F\x0F\x70\x0F\x07\x70\x70\x70\x07\x70\x0F\x07\x07\x08\x00" \
    "\x07\x0F\x07\x70\x70\x07\x0F\x70"    // help colors

#define cpAppMonochrome \
       "\x70\x07\x07\x0F\x70\x70\x70\x07\x0F\x07\x70\x70\x07\x70\x00" \
    "\x07\x0F\x07\x70\x70\x07\x70\x00\x70\x70\x70\x07\x07\x70\x07\x00" \
    "\x70\x70\x70\x07\x07\x70\x70\x70\x0F\x07\x07\x0F\x70\x0F\x70\x07" \
    "\x0F\x0F\x07\x70\x07\x07\x70\x07\x07\x07\x70\x0F\x07\x07\x70\x00" \
    "\x70\x70\x70\x07\x07\x70\x70\x70\x0F\x07\x07\x0F\x70\x0F\x70\x07" \
    "\x0F\x0F\x07\x70\x07\x07\x70\x07\x07\x07\x70\x0F\x07\x07\x01\x00" \
    "\x70\x70\x70\x07\x07\x70\x70\x70\x0F\x07\x07\x0F\x70\x0F\x70\x07" \
    "\x0F\x0F\x07\x70\x07\x07\x70\x07\x07\x07\x70\x0F\x07\x07\x01\x00" \
    "\x07\x0F\x07\x70\x70\x07\x0F\x70"    // help colors

TPalette& TVDemo::getPalette() const
{
    static TPalette newcolor ( cpAppColor , sizeof( cpAppColor )-1 );
    static TPalette newblackwhite( cpAppBlackWhite , sizeof( cpAppBlackWhite )-1 );
    static TPalette newmonochrome( cpAppMonochrome , sizeof( cpAppMonochrome )-1 );
    static TPalette *palettes[] =
        {
        &newcolor,
        &newblackwhite,
        &newmonochrome
        };
    return *(palettes[appPalette]);

}


//
// isTileable() function ( checks a view on desktop is tileable or not )
//

static Boolean isTileable(TView *p, void * )
{
   if( (p->options & ofTileable) != 0)
       return True;
   else
       return False;
}

//
// idle() function ( updates heap and clock views for this program. )
//

void TVDemo::idle()
{
    TProgram::idle();
    clock->update();
    heap->update();
    if (deskTop->firstThat(isTileable, 0) != 0 )
        {
        enableCommand(cmTile);
        enableCommand(cmCascade);
        }
    else 
        {
        disableCommand(cmTile);
        disableCommand(cmCascade);
        }
}

//
// closeView() function
//

static void closeView(TView *p, void *p1)
{
    message(p, evCommand, cmClose, p1);
}

//
// loadDesktop() function 
//

void TVDemo::loadDesktop(fpstream &s)
{
    TView  *p;

    if (deskTop->valid(cmClose))
        { 
        deskTop->forEach(::closeView, 0);  // Clear the desktop
        do {
           s >> p;
           deskTop->insertBefore(validView(p), deskTop->last);
           }
           while (p != 0);
        }
}

//
// Menubar initialization.
//

TMenuBar *TVDemo::initMenuBar(TRect r)
{
    TSubMenu& sub1 =
      *new TSubMenu( (char *)systemMenuIcon, 0, hcSystem ) +
        *new TMenuItem( "~A~bout...", cmAboutCmd, kbNoKey, hcSAbout ) +
         newLine() +
        *new TMenuItem( "~P~uzzle", cmPuzzleCmd, kbNoKey, hcSPuzzle ) +
        *new TMenuItem( "Ca~l~endar", cmCalendarCmd, kbNoKey, hcSCalendar ) +
        *new TMenuItem( "Ascii ~T~able", cmAsciiCmd, kbNoKey, hcSAsciiTable ) +
        *new TMenuItem( "~C~alculator", cmCalcCmd, kbNoKey, hcCalculator );

    TSubMenu& sub2 =
      *new TSubMenu( "~F~ile", 0, hcFile ) +
      //*new TSubMenu( "~ๆ~มสฬ", 0, hcFile ) + // KOI8 test
        *new TMenuItem( "~O~pen...", cmOpenCmd, kbF3, hcFOpen, "F3" ) +
        *new TMenuItem( "~C~hange Dir...", cmChDirCmd, kbNoKey, hcFChangeDir ) +
         newLine() +
        *new TMenuItem( "S~h~ell", cmCallShell, kbNoKey, hcFDosShell ) +
        *new TMenuItem( "E~x~it", cmQuit, kbAltX, hcFExit, "Alt-X" );

    TSubMenu& sub3 =
      *new TSubMenu( "~W~indows", 0, hcWindows ) +
        *new TMenuItem( "~R~esize/move", cmResize, kbCtrlF5, hcWSizeMove, "Ctrl-F5" ) +
        *new TMenuItem( "~Z~oom", cmZoom, kbF5, hcWZoom, "F5" ) +
        *new TMenuItem( "~N~ext", cmNext, kbF6, hcWNext, "F6" ) +
        *new TMenuItem( "~C~lose", cmClose, kbAltF3, hcWClose, "Alt-F3" ) +
        *new TMenuItem( "~T~ile", cmTile, kbNoKey, hcWTile ) +
        *new TMenuItem( "C~a~scade", cmCascade, kbNoKey, hcWCascade );

    TSubMenu& sub4 =
      *new TSubMenu( "~O~ptions", 0, hcOptions ) +
        *new TMenuItem( "~M~ouse...", cmMouseCmd, kbNoKey, hcOMouse ) +
        *new TMenuItem( "~C~olors...", cmColorCmd, kbNoKey, hcOColors ) +
        *new TMenuItem( "~S~ave desktop", cmSaveCmd, kbNoKey, hcOSaveDesktop ) +
        *new TMenuItem( "~R~etrieve desktop", cmRestoreCmd, kbNoKey, hcORestoreDesktop ) +
        *new TMenuItem( "~T~est inputbox", cmTestInputBox, kbNoKey, hcNoContext ) +
        *new TMenuItem( "~T~est picture validator", cmTestPicture, kbNoKey, hcNoContext );

    r.b.y =  r.a.y + 1;
    return (new TMenuBar( r, sub1 + sub2 + sub3 + sub4 ) );
}

// Called each time the code page changes. In this example we only have
// potential code page changes at start-up.
void TVDemo::cpCallBack(ushort *map)
{
 TVCodePage::RemapString(systemMenuIcon,osystemMenuIcon,map);
 TCalendarView::upArrowChar=TVCodePage::RemapChar(TCalendarView::oupArrowChar,map);
 TCalendarView::downArrowChar=TVCodePage::RemapChar(TCalendarView::odownArrowChar,map);
 // If the chain was already used call it
 if (oldCPCallBack)
    oldCPCallBack(map);
}

