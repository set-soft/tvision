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
 * Modified to compile with gcc v3.x by Salvador E. Tropea, with the help of
 * Andris Pavenis.
 * Added code page example by Salvador E. Tropea (SET)
 */
 
// SET: moved the standard headers before tv.h
#include <stdio.h>
#define Uses_string

#define Uses_TVCodePage
#define Uses_TView
#define Uses_TRect
#define Uses_TStatusLine
#define Uses_TStatusDef
#define Uses_TStatusItem
#define Uses_TKeys
#define Uses_MsgBox
#define Uses_fpstream
#define Uses_TEvent
#define Uses_TDeskTop
#define Uses_TApplication
#define Uses_TWindow
#define Uses_TDeskTop
#define Uses_TScreen
#define Uses_IOS_BIN
#define Uses_THelpWindow
// Needed to remap the "system" menu character
#define Uses_TVCodePage
#define Uses_TStreamableClass

#include <tv.h>

#include "tvdemo.h"
#include "gadgets.h"
#include "fileview.h"
#include "puzzle.h"
#include "demohelp.h"

UsingNamespaceStd

/* SS: changed */

//#ifdef __DPMI32__
#define HELP_FILENAME "demohelp.h32"
//#else
//#define HELP_FILENAME "DEMOHELP.H16"
//#endif

__link(RCalculator);
__link(RCalcDisplay);
//
// main: create an application object.  Constructor takes care of all
//   initialization.  Calling run() from TProgram makes it tick and
//   the destructor will destroy the world.
//
//   File names can be specified on the command line for automatic
//   opening.
//

int main(int argc, char **argv, char **envir)
{
    // The following is optional, but helps some low level drivers.
    // Don't do it if you experiment any side effect in the arguments or you
    // use these variables in a not so common way.
    // Note that the Linux console driver will alter argv to be able to change
    // what ps reports, you'll get the setWindowTitle argument.
    TDisplay::setArgv(argc,argv,envir);
    
    // Uncommenting the next line the cursor won't be hided.
    // This is a test to get an application a little bit friendly for Braille
    // Terminals.
    //TDisplay::setShowCursorEver(True);
    
    // We have non-ASCII symbols that depends on the code page. We must
    // recode them to the screen code page, here is how we hook the code page
    // change chain.
    TVDemo::oldCPCallBack=TVCodePage::SetCallBack(TVDemo::cpCallBack);
    
    TVDemo *demoProgram = new TVDemo(argc,argv);

    // This is new in our port of TV:
    const char *title=TScreen::getWindowTitle();
    TScreen::setWindowTitle("Turbo Vision Demo Program");
    demoProgram->run();

    TObject::CLY_destroy( demoProgram );
    if( title )
        {
        TScreen::setWindowTitle(title);
        delete[] title;
        }

    return 0;
}


//
// Constructor for the application.  Command line parameters are interpreted
//   as file names and opened.  Wildcards are accepted and put up a dialog
//   box with the appropriate search path.
//

TVDemo::TVDemo( int argc, char **argv ) :
    TProgInit( &TVDemo::initStatusLine,
               &TVDemo::initMenuBar,
               &TVDemo::initDeskTop )
{
    TView *w;
    char fileSpec[128];
    int len;

    TRect r = getExtent();                      // Create the clock view.
    r.a.x = r.b.x - 9;      r.b.y = r.a.y + 1;
    clock = new TClockView( r );
    insert(clock);

    r = getExtent();                            // Create the heap view.
    r.a.x = r.b.x - 13;     r.a.y = r.b.y - 1;
    heap = new THeapView( r );
    insert(heap);

    while (--argc > 0)                              // Display files specified
        {                                           //  on command line.
        strcpy( fileSpec, *++argv );
        len = strlen( fileSpec );

        /* SS: changed */

        if( fileSpec[len-1] == '/' )
            strcat( fileSpec, "*" );
        if( strchr( fileSpec, '*' ) || strchr( fileSpec, '?' ) )
            openFile( fileSpec );
        else
            {
            w = validView( new TFileWindow( fileSpec ) );
            if( w != 0 )
                deskTop->insert(w);
            }
        }

}


//
// DemoApp::getEvent()
//  Event loop to check for context help request
//

void TVDemo::getEvent(TEvent &event)
{
    TWindow *w;
    THelpFile *hFile;
    fpstream *helpStrm;
    static Boolean helpInUse = False;

    TApplication::getEvent(event);
    switch (event.what)
        {
        case evCommand:
            if ((event.message.command == cmHelp) && ( helpInUse == False)) 
                {
                helpInUse = True;
                helpStrm = new fpstream(HELP_FILENAME, CLY_IOSIn|CLY_IOSBin);
                hFile = new THelpFile(*helpStrm);
                if (!helpStrm)
                    {
                    messageBox("Could not open help file", mfError | mfOKButton);
                    delete hFile;
                    }
                else
                    {
                    w = new THelpWindow(hFile, getHelpCtx());
                    if (validView(w) != 0)
                        {
                        execView(w);
                        CLY_destroy(w);
                        }
                    clearEvent(event);
                    }
                helpInUse = False;
                }
            break;
        case evMouseDown:
            if (event.mouse.buttons != 1)
                event.what = evNothing;
            break;
        }

}  

//
// Create statusline.
//

TStatusLine *TVDemo::initStatusLine( TRect r )
{
    r.a.y = r.b.y - 1;

    return (new TStatusLine( r,
      *new TStatusDef( 0, 50 ) +
        *new TStatusItem( "~F1~ Help", kbF1, cmHelp ) +
        *new TStatusItem( "~Alt-X~ Exit", kbAltX, cmQuit ) +
        *new TStatusItem( 0, kbAltF3, cmClose ) +
        *new TStatusItem( 0, kbF10, cmMenu ) +
        *new TStatusItem( 0, kbF5, cmZoom ) +
        *new TStatusItem( 0, kbCtrlF5, cmResize ) +
      *new TStatusDef( 50, 0xffff ) +
        *new TStatusItem( "Howdy", kbF1, cmHelp )
        )
    );
}


//
// Puzzle function
//

void TVDemo::puzzle()
{
    TPuzzleWindow *puzz = (TPuzzleWindow *) validView(new TPuzzleWindow);

    if(puzz != 0)
        {
        puzz->helpCtx = hcPuzzle;
        deskTop->insert(puzz);
	}
}


//
// retrieveDesktop() function ( restores the previously stored Desktop )
//

// SET: Mike modified it to lowercase, is less annoying on POSIX systems
static const char *TVDemoFile="tvdemo.dst";

void TVDemo::retrieveDesktop()
{
	/* SS: changed */

//    struct ffblk ffblk;

//    if (findfirst("TVDEMO.DST", &ffblk, 0))
	FILE *fp;
	if ((fp = fopen(TVDemoFile, "r")) == NULL)
        messageBox("Could not find desktop file", mfOKButton | mfError);
        else
        {
	fclose(fp);
        fpstream *f = new fpstream(TVDemoFile, CLY_IOSIn|CLY_IOSBin);
        if( !f )
            messageBox("Could not open desktop file", mfOKButton | mfError);
        else
           {
           TVDemo::loadDesktop(*f);
           if( !f )
               messageBox("Error reading desktop file", mfOKButton | mfError);
           }
        delete f;
        }
}

//
// saveDesktop() function ( saves the DeskTop by calling storeDesktop function )
//

void TVDemo::saveDesktop()
{
    fpstream *f = new fpstream(TVDemoFile, CLY_IOSOut | CLY_IOSBin);

    if( f )
        {
        TVDemo::storeDesktop(*f);
        if( !f )
            {
            messageBox("Could not create tvdemo.dst.", mfOKButton | mfError);
            delete f;
            ::remove(TVDemoFile);
            return;
            }
        }
    delete f;
}

//
// writeView() function ( writes a view object to a resource file )
//

static void writeView(TView *p, void *strm)
{
   fpstream *s = (fpstream *) strm;
   if (p != TProgram::deskTop->last)
      *s << p;
}

//
// storeDesktop() function ( stores the Desktop in a resource file )
//

void TVDemo::storeDesktop(fpstream& s)
{
  deskTop->forEach(::writeView, &s);
  s << 0;
}

//
// Tile function
//

void TVDemo::tile()
{
    deskTop->tile( deskTop->getExtent() );
}
