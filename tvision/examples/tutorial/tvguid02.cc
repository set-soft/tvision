/*---------------------------------------------------------*/
/*                                                         */
/*   Turbo Vision 1.0                                      */
/*   TVGUID02 Demo Source File                             */
/*   Copyright (c) 1991 by Borland International           */
/*                                                         */
/*---------------------------------------------------------*/

#define Uses_TApplication
#define Uses_TKeys
#define Uses_TRect
#define Uses_TStatusLine
#define Uses_TStatusItem
#define Uses_TStatusDef
#define Uses_TDeskTop
#include <tv.h>

// tv.h ensures that the correct *.h files are included for each
// of the above classes and their bases.

/*
  Taked from the Sergio Sigala <ssigala@globalnet.it> Turbo Vision port to
UNIX.
  LSM: TurboVision for UNIX
  ftp://sunsite.unc.edu /pub/Linux/devel/lang/c++/tvision-0.6.tar.gz
  Copying policy: BSD
  Adapted by Salvador Eduardo Tropea (SET) <set-soft@usa.net>.

  Here you can see how a new item is added to the status bar of the
application. See how the overloaded + operator simplifies the work. The
0,0xFFFF is the range where the this status items are showed. It is related
to the context sensitive help and you'll see it latter.
*/


class TMyApp : public TApplication
{

public:
    TMyApp();
    static TStatusLine *initStatusLine( TRect r );
    // new for tvguid02
};

TMyApp::TMyApp() :
    TProgInit( &TMyApp::initStatusLine,
               &TMyApp::initMenuBar,
               &TMyApp::initDeskTop
             )
{
}

// new for tvguid02:
TStatusLine *TMyApp::initStatusLine(TRect r)
{
    r.a.y = r.b.y - 1;     // move top to 1 line above bottom
    return new TStatusLine( r,
        *new TStatusDef( 0, 0xFFFF ) +
        // set range of help contexts
            *new TStatusItem( "~Alt-X~ Exit", kbAltX, cmQuit ) +
            // define an item
            *new TStatusItem( "~Alt-F3~ Close", kbAltF3, cmClose )
            // and another one
        );
}

int main()
{
    TMyApp myApp;
    myApp.run();
    return 0;
}
