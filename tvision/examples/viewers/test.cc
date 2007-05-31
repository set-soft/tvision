/**[txh]********************************************************************

  Copyright (c) 2007 by Salvador E. Tropea
  This code is Public Domain and you can use it for any purpose. Note that
when you link to another libraries, obviously needed, the code can (and
currently will) be affected by the libraries license.
  Currently my TV port is GPL so this code becomes GPL, but if you link
with another Turbo Vision port or the library changes license things are
different.

  Description:
  This examples shows how to use the applications helpers.
  
***************************************************************************/

#define Uses_string
#define Uses_TKeys
#define Uses_TEventQueue
#define Uses_TEvent
#define Uses_TProgram
#define Uses_TApplication
#define Uses_TMenuBar
#define Uses_TSubMenu
#define Uses_TMenuItem
#define Uses_TStatusLine
#define Uses_TStatusItem
#define Uses_TStatusDef
#define Uses_MsgBox
#define Uses_TScreen
#define Uses_TFileDialog
#define Uses_TDeskTop
#include <tv.h>

UsingNamespaceStd

const int cmMyFileOpen=200;

class TMyApp : public TApplication
{
public:
 TMyApp();
 ~TMyApp();
 static TMenuBar *initMenuBar(TRect r);
 virtual void handleEvent(TEvent& event);
         void openFile(char *fileSpec);
 static  void viewImage(const char *fileName);
 static  void viewPDF(const char *fileName);

protected:
 static TScreen::appHelperHandler hPDF;
 static TScreen::appHelperHandler hImg;
};

TScreen::appHelperHandler TMyApp::hPDF=-1;
TScreen::appHelperHandler TMyApp::hImg=-1;

TMyApp::TMyApp() :
    TProgInit(&TMyApp::initStatusLine,
              &TMyApp::initMenuBar,
              &TMyApp::initDeskTop)
{
}

TMyApp::~TMyApp()
{
 if (hImg!=-1)
    TScreen::closeHelperApp(hImg);
 if (hPDF!=-1)
    TScreen::closeHelperApp(hPDF);
}

TMenuBar *TMyApp::initMenuBar(TRect r)
{
 r.b.y=r.a.y+1;
 return new TMenuBar(r,
     *new TSubMenu("~F~ile",kbAltF)+
         *new TMenuItem("~V~iew",cmMyFileOpen,kbF3,hcNoContext,"F3")+
         newLine()+
         *new TMenuItem("E~x~it",cmQuit,kbAltX,hcNoContext,"Alt-X"));
}

void TMyApp::openFile(char *fileSpec)
{
 TFileDialog *d=(TFileDialog *)validView(
    new TFileDialog(fileSpec,"View a File","~N~ame",fdOpenButton,100));

 if (d && deskTop->execView(d)!=cmCancel)
   {
    char fileName[PATH_MAX];
    d->getFileName(fileName);
    int l=strlen(fileName);
    if (strcmp(fileName+l-4,".pdf")==0)
      {
       viewPDF(fileName);
      }
    else if (strcmp(fileName+l-4,".jpg")==0 ||
             strcmp(fileName+l-4,".png")==0 ||
             strcmp(fileName+l-4,".gif")==0 ||
             strcmp(fileName+l-4,".pcx")==0 ||
             strcmp(fileName+l-4,".tif")==0 ||
             strcmp(fileName+l-4,".bmp")==0)
      {
       viewImage(fileName);
      }
    else
       messageBox("Unsupported file extension",mfError | mfOKButton);
   }
 CLY_destroy(d);
}

void TMyApp::viewImage(const char *fileName)
{
 if (hImg==-1)
   {
    hImg=TScreen::openHelperApp(TScreen::ImageViewer);
    if (hImg==-1)
      {
       messageBox(TScreen::getHelperAppError(),mfError | mfOKButton);
       return;
      }
   }
 if (!TScreen::sendFileToHelper(hImg,fileName,NULL))
   {
    messageBox(TScreen::getHelperAppError(),mfError | mfOKButton);
    return;
   }
}

void TMyApp::viewPDF(const char *fileName)
{
 if (hPDF==-1)
   {
    hPDF=TScreen::openHelperApp(TScreen::PDFViewer);
    if (hPDF==-1)
      {
       messageBox(TScreen::getHelperAppError(),mfError | mfOKButton);
       return;
      }
   }
 if (!TScreen::sendFileToHelper(hPDF,fileName,NULL))
   {
    messageBox(TScreen::getHelperAppError(),mfError | mfOKButton);
    return;
   }
}

void TMyApp::handleEvent(TEvent& event)
{
 TApplication::handleEvent(event);
 if (event.what==evCommand)
   {
    switch (event.message.command)
      {
       case cmMyFileOpen:
            openFile("*");
            break;
       default:
            return;
      }
    clearEvent(event);
   }
}

int main()
{
 TMyApp myApp;
 myApp.run();
 return 0;
}
