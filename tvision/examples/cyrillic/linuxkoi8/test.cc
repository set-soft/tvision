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
#define Uses_TVCodePage
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

TMenuBar *TMyApp::initMenuBar( TRect r )
{
 r.b.y=r.a.y+1;
 return new TMenuBar( r,
     //*new TSubMenu("~F~ile",kbAltF)+
     *new TSubMenu("~ๆ~มสฬ",0)+ // KOI8 test
         *new TMenuItem("~O~pen",200,kbF3,hcNoContext,"F3")+
         *new TMenuItem("~N~ew", 200,kbF4,hcNoContext,"F4")+
         newLine()+
         *new TMenuItem("E~x~it",cmQuit,kbAltX,hcNoContext,"Alt-X")+
     *new TSubMenu("~W~indow", kbAltW )+
         *new TMenuItem("~N~ext",cmNext,kbF6,hcNoContext,"F6")+
         *new TMenuItem("~Z~oom",cmZoom,kbF5,hcNoContext,"F5")
     );
}

int main()
{
 TMyApp myApp;
 // Currently autodetection is broken, so I'm forcing it until I find a solution
 //TVCodePage::SetCodePage(TVCodePage::KOI8r);
 myApp.redraw();

 myApp.run();
 return 0;
}
