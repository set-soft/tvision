/**[txh]********************************************************************

  Description:
  That's a small example on how to use the TTerminal class. I wrote it
because Andris found a bug in the Robert's port (was in the prevLine,
Robert translated it from assembler in the wrong way).

  Copyright (c) 1999 by Salvador E. Tropea

  This file is provided as an example nothing else. As all files needs a
disclaimer y choose to use the GPL license for this file. So use it under
the terms of the GPL license.

***************************************************************************/

#define Uses_TApplication
#define Uses_TEvent
#define Uses_TDialog
#define Uses_TScrollBar
#define Uses_TTerminal
#define Uses_TButton
#define Uses_TDeskTop
#include <tv.h>

#include <tv/configtv.h>
// The SSC code doesn't support as much as needed for it.
#ifndef HAVE_SSC

class TMyApp : public TApplication
{
public:
 TMyApp();
 virtual void handleEvent(TEvent &event);
 void Test();
};

TMyApp::TMyApp() :
  TProgInit( &TMyApp::initStatusLine,
             &TMyApp::initMenuBar,
             &TMyApp::initDeskTop
           )
{
}

const int cmAbout=0x1000;

void TMyApp::handleEvent( TEvent &event )
{
 TApplication::handleEvent(event);

 if (event.what==evCommand)
   {
    switch (event.message.command)
      {
       case cmAbout:
            Test();
            break;
       default:
            break;
      }
   }
}

class TestDialog : public TDialog
{
public:
 TestDialog();
};

TestDialog::TestDialog() :
  TWindowInit(&TestDialog::initFrame),
  TDialog(TRect(0,0,60,18),"Dumb terminal")
{
 options |= ofCentered;

 TScrollBar *hsb=new TScrollBar(TRect(58,1,59,13));
 TScrollBar *vsb=new TScrollBar(TRect(1,13,58,14));
 insert(hsb);
 insert(vsb);
 TTerminal *tt=new TTerminal(TRect(1,1,57,12),hsb,vsb,4096);
 tt->do_sputn("Hello!\nThat's just a test in the buffer.\nThat's all falks.",58);
 //tt->do_sputn("Hello!\r\nThat's just a test in the buffer.\r\nThat's all falks.",60);
 insert(tt);
 insert(new TButton(TRect(25,15,35,17),"O~K~",cmOK,bfNormal));

 selectNext(False);
}

void TMyApp::Test()
{
 TestDialog *d=new TestDialog;
 deskTop->execView(d);
 CLY_destroy(d);
}

int main()
{
 TEvent init;
 init.what=evCommand;
 init.message.command=cmAbout;

 TMyApp myApp;
 myApp.putEvent(init);
 myApp.run();
 return 0;
}

#else

int main()
{
 fprintf (stderr, "Sorry: The SSC code doesn't support as much as needed for it.\n");
}

#endif
