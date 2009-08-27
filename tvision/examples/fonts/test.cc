/**[txh]********************************************************************

  Copyright (c) 2002 by Salvador E. Tropea
  This code is Public Domain and you can use it for any purpose. Note that
when you link to another libraries, obviously needed, the code can (and
currently will) be affected by the libraries license.
  Currently my TV port is GPL so this code becomes GPL, but if you link
with another Turbo Vision port or the library changes license things are
different.

  Description:
  This examples shows how to use bitmaped fonts.
  Not all terminals supports it.
  
***************************************************************************/

#define Uses_stdio
#define Uses_string
#define Uses_limits
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

const int cmTestFont=100;
const int cmTestRestore=101;
const int cmTestFontSecondary=102;

class TMyApp : public TApplication
{
public:
 TMyApp();
 static TStatusLine *initStatusLine(TRect r);
 static TMenuBar *initMenuBar(TRect r);
 virtual void handleEvent(TEvent& event);
 void testFont();
 void testRestore();
 void testFontSecondary();
 void shell();

 uchar *fontData;
 int quit;
 int changed1,changed2;
};


TMyApp::TMyApp() :
  TProgInit(&TMyApp::initStatusLine,&TMyApp::initMenuBar,&TMyApp::initDeskTop)
{
 quit=0;
 changed1=changed2=0;
 if (!TScreen::canSetBFont())
   {
    messageBox("This terminal doesn't support changing the fonts",mfError | mfOKButton);
    quit=1;
   }
 else
   {
    unsigned w,h,wm,hm;
    int noSize=0;
    TScreen::getFontGeometry(w,h);
    if (w!=8 || h!=16)
      {
       if (TScreen::canSetFontSize())
         {
          TScreen::getFontGeometryRange(w,h,wm,hm);
          if (w>8 || wm<8 || h>16 || hm<16)
             noSize=1;
         }
       else
         noSize=1;
      }
    if (noSize)
      {
       messageBox("This terminal doesn't support the test fonts used for this demo",mfError | mfOKButton);
       quit=1;
      }
    else
      {
       FILE *f=fopen("font.016","rb");
       if (f)
         {
          fontData=new uchar[4096];
          fread(fontData,4096,1,f);
          fclose(f);
         }
       else
         {
          messageBox("Error reading the test font!",mfError | mfOKButton);
          quit=1;
         }
      }
   }
}

void TMyApp::handleEvent(TEvent &event)
{
 TApplication::handleEvent(event);
 if (event.what==evCommand)
   {
    switch (event.message.command)
      {
       case cmTestFont:
            testFont();
            break;

       case cmTestFontSecondary:
            testFontSecondary();
            break;

       case cmTestRestore:
            testRestore();
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
 return new TMenuBar(r,
     *new TSubMenu("~D~emo",kbAltD) +
       *new TMenuItem("Test ~f~ont",cmTestFont,kbNoKey,hcNoContext,"")+
       *new TMenuItem("Test ~f~ont as secondary",cmTestFontSecondary,kbNoKey,hcNoContext,"")+
       *new TMenuItem("~R~estore font",cmTestRestore,kbNoKey,hcNoContext,"")+
        newLine() +
       *new TMenuItem("S~h~ell",cmCallShell,kbNoKey,hcNoContext)+
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

void TMyApp::shell()
{
 suspend();
 TScreen::System(CLY_GetShellName());
 resume();
 redraw();
}

void TMyApp::testFont()
{
 if (changed1)
   {
    messageBox("\03Already using it",mfInformation | mfOKButton);
    return;
   }
 TScreenFont256 font;
 font.w=8;
 font.h=16;
 font.data=fontData;
 if (TScreen::setPrimaryFont(&font,TVCodePage::ISOLatin1Linux))
   {
    messageBox("\03Font successfuly changed",mfInformation | mfOKButton);
    changed1=1;
   }
 #ifdef TVOS_DOS
 /* Test for DOS driver mode switch when the font have the same size */
 messageBox("\03Now I'll change the video mode, fonts should survive",mfInformation | mfOKButton);
 setScreenMode(TDisplay::smCO90x30);
 #endif
}

void TMyApp::testRestore()
{
 if (!changed1 && !changed2)
   {
    messageBox("\03No need to restore the font",mfInformation | mfOKButton);
    return;
   }
 TScreen::restoreFonts();
 messageBox("\03Font restored",mfInformation | mfOKButton);
 changed1=changed2=0;
}

void TMyApp::testFontSecondary()
{
 if (!TScreen::canSetSBFont())
   {
    messageBox("\03This terminal doesn't support a secondary font",mfError | mfOKButton);
    return;
   }
 if (changed2)
   {
    messageBox("\03Already using it",mfInformation | mfOKButton);
    return;
   }
 if (changed1)
   {
    TScreen::restoreFonts();
    changed1=0;
   }

 unsigned w,h;
 TScreen::getFontGeometry(w,h);
 if (w!=8 || h!=16)
    messageBox("\03The default font doesn't match the size of the test font, the next operation will fail",mfInformation | mfOKButton);

 TScreenFont256 font;
 font.w=8;
 font.h=16;
 font.data=fontData;
 if (TScreen::setSecondaryFont(&font))
   {
    messageBox("\03Restored primary font and defined secondary font. Look at the dialog title.",mfInformation | mfOKButton);
    changed2=1;
   }
 else
    messageBox("\03Error setting the secondary font.",mfError | mfOKButton);
}

int main(int argc, char *argv[], char **envir)
{
 TDisplay::setArgv(argc,argv,envir);
 TMyApp myApp;
 const char *title=TScreen::getWindowTitle();
 TScreen::setWindowTitle("Turbo Vision font test program");
 //myApp.setScreenMode(TDisplay::smCO90x34);
 //myApp.setScreenMode(90,30,9,16);
 //myApp.setScreenMode(132,60,9,16); Test for a VESA mode
 myApp.run();
 if (title)
   {
    TScreen::setWindowTitle(title);
    delete[] title;
   }
 return 0;
}
