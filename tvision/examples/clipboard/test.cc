/**[txh]********************************************************************

  Copyright (c) 2002 by Salvador E. Tropea
  This code is Public Domain and you can use it for any purpose. Note that
when you link to another libraries, obviously needed, the code can (and
currently will) be affected by the libraries license.
  Currently my TV port is GPL so this code becomes GPL, but if you link
with another Turbo Vision port or the library changes license things are
different.

  Description:
  This examples shows how to use the OS independent clipboard class.
  
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
#define Uses_TVOSClipboard
#define Uses_MsgBox
#define Uses_TKeys
#define Uses_TScreen

#include <tv.h>

const int cmTestCopy=100;
const int cmTestPaste=101;
const int cmTestCopy2=102;
const int cmTestPaste2=103;

class TMyApp : public TApplication
{
public:
 TMyApp();
 static TStatusLine *initStatusLine(TRect r);
 static TMenuBar *initMenuBar(TRect r);
 virtual void handleEvent(TEvent& event);
 void testCopy(int clip);
 void testPaste(int clip);
};


TMyApp::TMyApp() :
  TProgInit(&TMyApp::initStatusLine,&TMyApp::initMenuBar,&TMyApp::initDeskTop)
{
}

void TMyApp::handleEvent(TEvent& event)
{
 TApplication::handleEvent(event);
 if (event.what==evCommand)
   {
    switch (event.message.command)
      {
       case cmTestCopy:
            testCopy(0);
            break;

       case cmTestPaste:
            testPaste(0);
            break;

       case cmTestCopy2:
            testCopy(1);
            break;

       case cmTestPaste2:
            testPaste(1);
            break;
      }
    clearEvent(event);
   }
}

TMenuBar *TMyApp::initMenuBar(TRect r)
{
 r.b.y=r.a.y+1;
 return new TMenuBar(r,
     *new TSubMenu("~D~emo",kbAltD) +
       *new TMenuItem("Test ~c~opy",cmTestCopy,kbNoKey,hcNoContext,"" )+
       *new TMenuItem("Test ~p~aste",cmTestPaste,kbNoKey,hcNoContext,"" )+
       *new TMenuItem("Test ~c~opy secondary",cmTestCopy2,kbNoKey,hcNoContext,"" )+
       *new TMenuItem("Test ~p~aste secondary",cmTestPaste2,kbNoKey,hcNoContext,"" )+
       *new TMenuItem("E~x~it",cmQuit,kbNoKey,hcNoContext,"Alt-X")
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

const char *aMessage="Turbo Vision Clipboard demo message: Hi!";

void TMyApp::testCopy(int clip)
{
 if (!TVOSClipboard::isAvailable())
   {
    messageBox("Sorry but none OS specific clipboard is available",mfError | mfOKButton);
    return;
   }
 if (clip>=TVOSClipboard::isAvailable())
   {
    messageBox("Sorry but this OS doesn't have such a clipboard",mfError | mfOKButton);
    return;
   }
 int res=TVOSClipboard::copy(clip,aMessage,strlen(aMessage));
 if (!res)
   {
    messageBox(mfError | mfOKButton,"Error copying to clipboard: %s",
               TVOSClipboard::getError());
    return;
   }
 messageBox(mfInformation | mfOKButton,
            "Clipboard name: %s, a message was copied successfuly",
            TVOSClipboard::getName());
}

void TMyApp::testPaste(int clip)
{
 if (!TVOSClipboard::isAvailable())
   {
    messageBox("Sorry but none OS specific clipboard is available",mfError | mfOKButton);
    return;
   }
 if (clip>=TVOSClipboard::isAvailable())
   {
    messageBox("Sorry but this OS doesn't have such a clipboard",mfError | mfOKButton);
    return;
   }
 unsigned length;
 char *result=TVOSClipboard::paste(clip,length);
 if (!result)
   {
    messageBox(mfError | mfOKButton,"Error pasting from clipboard: %s",
               TVOSClipboard::getError());
    return;
   }
 if (length>80)
   {
    result[80]=0;
    messageBox(mfInformation | mfOKButton,"First 80 characters from clipboard: %s",result);
   }
 else
    messageBox(mfInformation | mfOKButton,"Content of the clipboard: %s",result);
}

int main(int argc, char *argv[], char **envir)
{
 TDisplay::setArgv(argc,argv,envir);
 TMyApp myApp;
 const char *title=TScreen::getWindowTitle();
 TScreen::setWindowTitle("Turbo Vision Demo Program");
 myApp.run();
 if (title)
   {
    TScreen::setWindowTitle(title);
    delete[] title;
   }
 return 0;
}
