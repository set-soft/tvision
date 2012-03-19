/**[txh]********************************************************************

  Copyright (c) 2003 by Salvador E. Tropea
  This code is Public Domain and you can use it for any purpose. Note that
when you link to another libraries, obviously needed, the code can (and
currently will) be affected by the libraries license.
  Currently my TV port is GPL so this code becomes GPL, but if you link
with another Turbo Vision port or the library changes license things are
different.

  Description:
  This example shows how to use internationalization (i18n) for TV messages.
  
***************************************************************************/

#define Uses_stdlib
#define Uses_TApplication
#define Uses_TKeys
#define Uses_TRect
#define Uses_TMenuBar
#define Uses_TSubMenu
#define Uses_TMenuItem
#define Uses_TStatusLine
#define Uses_TStatusItem
#define Uses_TStatusDef
#define Uses_TDeskTop
#define Uses_TScreen
#include <tv.h>

class TMyApp : public TApplication
{
public:
 TMyApp();
 static TMenuBar *initMenuBar(TRect r);
};

TMyApp::TMyApp() :
    TProgInit(&TMyApp::initStatusLine,
              &TMyApp::initMenuBar,
              &TMyApp::initDeskTop
             )
{
}

TMenuBar *TMyApp::initMenuBar(TRect r)
{
 r.b.y=r.a.y+1;
 return new TMenuBar( r,
     *new TSubMenu("~F~ile",kbAltF)+
         *new TMenuItem("~O~pen",200,kbF3,hcNoContext,"F3")+
         *new TMenuItem("~Y~es", 200,kbF4,hcNoContext,"F4")+
         newLine()+
         *new TMenuItem("E~x~it",cmQuit,kbAltX,hcNoContext,"Alt-X")+
     // This definition doesn't exist in TV, only the plural
     *new TSubMenu(__("~W~indow"), kbAltW )+
         *new TMenuItem("~N~ext",cmNext,kbF6,hcNoContext,"F6")+
         *new TMenuItem("~Z~oom",cmZoom,kbF5,hcNoContext,"F5")
     );
}

int main()
{
 // Here I force the use of spanish
 putenv("LANG=es_AR");
 // Here we tell gettext to use the "domain" test located at the "intl" dir.
 // In real life TVIntl::textDomain("test"); should be enough.
 //TVIntl::autoInit("setedit");
 TMyApp myApp;
 myApp.run();
 return 0;
}
