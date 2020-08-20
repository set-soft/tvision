/**[txh]********************************************************************

  Copyright (c) 2002 by Salvador E. Tropea
  This code is Public Domain and you can use it for any purpose. Note that
when you link to another libraries, obviously needed, the code can (and
currently will) be affected by the libraries license.
  Currently my TV port is GPL so this code becomes GPL, but if you link
with another Turbo Vision port or the library changes license things are
different.

  Description:
  This examples shows how to change the screen or window size.
  Not all terminals supports it.
  
***************************************************************************/

#define Uses_stdio
#define Uses_string
#define Uses_TProgram
#define Uses_TApplication
#define Uses_TRect
#define Uses_TMenuBar
#define Uses_TSubMenu
#define Uses_TMenuItem
#define Uses_TStatusLine
#define Uses_TStatusItem
#define Uses_TStatusDef
#define Uses_MsgBox
#define Uses_TKeys
#define Uses_TScreen
#define Uses_TVCodePage

#include <tv.h>

enum  {
  cmTest80x25= 100
, cmTest80x28
, cmTest80x50
, cmTest90x30
, cmTest94x34
, cmTest132x25
, cmTest132x50
, cmTest80x25b
, cmTest80x28b
, cmTest80x50b
, cmTest90x30b
, cmTest94x34b
, cmTest132x25b
, cmTest132x50b
, cmTestCO80     // JASC, color modes
, cmTestBW80     // JASC, color modes
, cmTestMono     // JASC, color modes
};

class TMyApp : public TApplication
{
public:
 TMyApp();
 static TStatusLine *initStatusLine(TRect r);
 static TMenuBar *initMenuBar(TRect r);
 virtual void handleEvent(TEvent& event);
 void testMode(unsigned mode,unsigned w, unsigned h);
 void testMode(unsigned w, unsigned h);
 void shell();

 int quit;
};


TMyApp::TMyApp() :
  TProgInit(&TMyApp::initStatusLine,&TMyApp::initMenuBar,&TMyApp::initDeskTop)
{
 quit=0;
 if (!TScreen::canSetVideoSize())
   {
    messageBox("This terminal doesn't support changing the screen size",mfError | mfOKButton);
    quit=1;
   }
 else
    messageBox("This terminal supports changing the screen/window size. It doesn't mean it will work",mfInformation | mfOKButton);
}

void TMyApp::handleEvent(TEvent &event)
{
 TApplication::handleEvent(event);
 if (event.what==evCommand)
   {
    switch (event.message.command)
      { case cmTestCO80:
          setScreenMode( TDisplay::smCO80 );
        break;
        
        case cmTestBW80:
          setScreenMode( TDisplay::smBW80 );
        break;
        
        case cmTestMono:
          setScreenMode( TDisplay::smMono );
        break;


       case cmTest80x25:
            testMode(TScreen::smCO80x25,80,25);
            break;

       case cmTest80x28:
            testMode(TScreen::smCO80x28,80,28);
            break;

       case cmTest80x50:
            testMode(TScreen::smCO80x50,80,50);
            break;

       case cmTest90x30:
            testMode(TScreen::smCO90x30,90,30);
            break;

       case cmTest94x34:
            testMode(TScreen::smCO94x34,94,34);
            break;

       case cmTest132x25:
            testMode(TScreen::smCO132x25,132,25);
            break;

       case cmTest132x50:
            testMode(TScreen::smCO132x50,132,50);
            break;

       case cmTest80x25b:
            testMode(80,25);
            break;

       case cmTest80x28b:
            testMode(80,28);
            break;

       case cmTest80x50b:
            testMode(80,50);
            break;

       case cmTest90x30b:
            testMode(90,30);
            break;

       case cmTest94x34b:
            testMode(94,34);
            break;

       case cmTest132x25b:
            testMode(132,25);
            break;

       case cmTest132x50b:
            testMode(132,50);
            break;

       case cmCallShell:
            shell();
            break;
      }
    clearEvent(event);
   }
 if (quit)
   {
    TEvent event;
    event.what=evCommand;
    event.message.command=cmQuit;
    TApplication::handleEvent(event);
   }
}

TMenuBar *TMyApp::initMenuBar(TRect r)
{
 r.b.y=r.a.y+1;
 return new TMenuBar
 ( r
 , *new TSubMenu("~S~ystem",kbAltS)
   + *new TMenuItem("S~h~ell",cmCallShell,kbNoKey,hcNoContext)
   + *new TMenuItem("E~x~it",cmQuit,kbNoKey,hcNoContext,"Alt-X")
 + *new TSubMenu("~M~ode",kbAltM)
   + *new TMenuItem("~8~0x25",cmTest80x25,kbNoKey,hcNoContext)
   + *new TMenuItem("80x~2~8",cmTest80x28,kbNoKey,hcNoContext)
   + *new TMenuItem("80x~5~0",cmTest80x50,kbNoKey,hcNoContext)
   + *new TMenuItem("~9~0x30",cmTest90x30,kbNoKey,hcNoContext)
   + *new TMenuItem("9~4~x34",cmTest94x34,kbNoKey,hcNoContext)
   + *new TMenuItem("~1~32x25",cmTest132x25,kbNoKey,hcNoContext)
   + *new TMenuItem("1~3~2x50",cmTest132x50,kbNoKey,hcNoContext)
 + *new TSubMenu("~R~esolution",kbAltR)
   + *new TMenuItem("~8~0x25",cmTest80x25b,kbNoKey,hcNoContext)
   + *new TMenuItem("80x~2~8",cmTest80x28b,kbNoKey,hcNoContext)
   + *new TMenuItem("80x~5~0",cmTest80x50b,kbNoKey,hcNoContext)
   + *new TMenuItem("~9~0x30",cmTest90x30b,kbNoKey,hcNoContext)
   + *new TMenuItem("9~4~x34",cmTest94x34b,kbNoKey,hcNoContext)
   + *new TMenuItem("~1~32x25",cmTest132x25b,kbNoKey,hcNoContext)
   + *new TMenuItem("1~3~2x50",cmTest132x50b,kbNoKey,hcNoContext)
 + *new TSubMenu("~C~olor",kbAltC)
   + *new TMenuItem("~C~olor"        , cmTestCO80, kbNoKey,hcNoContext)
   + *new TMenuItem("~B~lack & White", cmTestBW80, kbNoKey,hcNoContext)
   + *new TMenuItem("~M~ono"         , cmTestMono, kbNoKey,hcNoContext)
 );
}

TStatusLine *TMyApp::initStatusLine(TRect r)
{
 r.a.y=r.b.y-1;
 return new TStatusLine(r,
     *new TStatusDef(0,0xFFFF)+
     *new TStatusItem(0,kbF10,cmMenu) +
     *new TStatusItem("~Alt-X~ Exit",kbAltX,cmQuit)
   );
}

void TMyApp::shell()
{
 suspend();
 TScreen::System(CLY_GetShellName());
 resume();
 redraw();
}

void TMyApp::testMode(unsigned mode, unsigned w, unsigned h)
{
 unsigned aW=TScreen::getCols();
 unsigned aH=TScreen::getRows();
 if (aW==w && aH==h)
   {
    messageBox("Already using this size",mfInformation | mfOKButton);
    return;
   }
 setScreenMode(mode);
 unsigned nW=TScreen::getCols();
 unsigned nH=TScreen::getRows();
 if (nW==w && nH==h)
   {
    messageBox(mfInformation | mfOKButton,"Successfuly changed to %d x %d size",w,h);
    return;
   }
 if (aW==nW && aH==nH)
   {
    messageBox(mfError | mfOKButton,"Failed to set %d x %d size",w,h);
    return;
   }
 messageBox(mfWarning | mfOKButton,"Partial change, we got %d x %d size",nW,nH);
}

void TMyApp::testMode(unsigned w, unsigned h)
{
 unsigned aW=TScreen::getCols();
 unsigned aH=TScreen::getRows();
 if (aW==w && aH==h)
   {
    messageBox("Already using this size",mfInformation | mfOKButton);
    return;
   }
 setScreenMode(w,h);
 unsigned nW=TScreen::getCols();
 unsigned nH=TScreen::getRows();
 if (nW==w && nH==h)
   {
    messageBox(mfInformation | mfOKButton,"Successfuly changed to %d x %d size",w,h);
    return;
   }
 if (aW==nW && aH==nH)
   {
    messageBox(mfError | mfOKButton,"Failed to set %d x %d size",w,h);
    return;
   }
 messageBox(mfWarning | mfOKButton,"Partial change, we got %d x %d size",nW,nH);
}

int main(int argc, char *argv[], char **envir)
{
 TDisplay::setArgv(argc,argv,envir);
 TMyApp myApp;
 const char *title=TScreen::getWindowTitle();
 TScreen::setWindowTitle("Turbo Vision screen size test program");
 myApp.run();
 if (title)
   {
    TScreen::setWindowTitle(title);
    delete[] title;
   }
 return 0;
}

