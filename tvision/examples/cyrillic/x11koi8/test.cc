/**[txh]********************************************************************

  Copyright (c) 2002 by Salvador E. Tropea
  This code is Public Domain and you can use it for any purpose. Note that
when you link to another libraries, obviously needed, the code can (and
currently will) be affected by the libraries license.
  Currently my TV port is GPL so this code becomes GPL, but if you link
with another Turbo Vision port or the library changes license things are
different.

  Description:
  This examples shows how to load and use a KOI-8r font when running on the
X11 server.
  Note this example also works for the Linux console and doesn't need KOI8r
fonts loaded in your system.
  
***************************************************************************/

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
#define Uses_TVFontCollection
#define Uses_TScreen
#define Uses_TVConfigFile
#include <tv.h>

class TMyApp : public TApplication
{
public:
 TMyApp();
 static TMenuBar *initMenuBar(TRect r);
 static TVFontCollection *fonts;
 static TScreenFont256 *fontsCallBack(int which, unsigned w, unsigned height);
};

TVFontCollection *TMyApp::fonts=NULL;

TMyApp::TMyApp() :
    TProgInit(&TMyApp::initStatusLine,
              &TMyApp::initMenuBar,
              &TMyApp::initDeskTop
             )
{
}

/**[txh]********************************************************************

  Description:
  This is the magic routine, is called each time a font is needed. We have
to return the requested font or NULL if not available.
  
***************************************************************************/

TScreenFont256 *TMyApp::fontsCallBack(int which, unsigned width, unsigned height)
{
 if (!fonts) return NULL; // Check we successfully loaded the font
 if (which) return NULL; // Just the primary
 if (width!=8 || height!=16) return NULL; // Only 8x16
 uchar *data=fonts->GetFont(width,height);
 if (!data) return NULL;
 TScreenFont256 *f=new TScreenFont256;
 f->w=width;
 f->h=height;
 f->data=data;

 return f;
}

TMenuBar *TMyApp::initMenuBar(TRect r)
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
 // Load a font we know supports cyrillics
 TMyApp::fonts=new TVFontCollection("../../fonts/ocr.sft",TVCodePage::KOI8r);
 // Setup our call back to provide the needed fonts
 TScreen::setFontRequestCallBack(TMyApp::fontsCallBack);
 // Now indicate to TV we want to use KOI8r for any driver.
 // This can be achieved indicating settings in a configuration file or like
 // this:
 TProgInit::config=new TVMainConfigFile();
 TVMainConfigFile::Add(NULL,"AppCP",TVCodePage::KOI8r);
 TVMainConfigFile::Add(NULL,"ScrCP",TVCodePage::KOI8r);
 TVMainConfigFile::Add(NULL,"InpCP",TVCodePage::KOI8r);

 TMyApp myApp;
 myApp.run();
 return 0;
}
