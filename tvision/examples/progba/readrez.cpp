/*---------------------------------------------------------*/
/*   Reads a previously streamed TProgressBar Resource     */
/*---------------------------------------------------------*/
#define Uses_TObject
#define Uses_TKeys
#define Uses_TApplication
#define Uses_TEvent
#define Uses_TRect
#define Uses_TDialog
#define Uses_TStaticText
#define Uses_TButton
#define Uses_TMenu
#define Uses_TMenuBar
#define Uses_TSubMenu
#define Uses_TMenuItem
#define Uses_TStatusLine
#define Uses_TStatusItem
#define Uses_TStatusDef
#define Uses_TDeskTop
#define Uses_MsgBox
#define Uses_TStreamableClass
#define Uses_TResourceCollection
#define Uses_TResourceFile
#define Uses_string
#define Uses_fpstream

#include <tv.h>
#include "tprogbar.h"
__link(RWindow);
__link(RGroup);
__link(RDialog);
__link(RFrame);
__link(RResourceCollection);
__link(RButton);
__link(RCluster);
__link(RProgressBar);

#include <stdlib.h>

const char rezFileName[] = "MY.REZ";

const int cmAboutCmd   = 100;  // User selected menu item 'About'
const int cmStatusCmd  = 101;  // User selected menu item 'Progress Bar'

//========================================================================

class TMyApplication : public TApplication
{
public:
   TMyApplication();
   static TMenuBar *initMenuBar(TRect);
   void handleEvent(TEvent &);
private:
   void aboutDlg();
   void statusDlg();
   Boolean isCancel(TDialog *pd);
};

//========================================================================

TMyApplication::TMyApplication() :
TProgInit(&TApplication::initStatusLine,&TMyApplication::initMenuBar,
	 &TApplication::initDeskTop)
{
}

//========================================================================

TMenuBar *TMyApplication::initMenuBar(TRect bounds)
{
	 bounds.b.y = bounds.a.y + 1;
	 return(new TMenuBar(bounds,
				new TMenu(
					 *new TMenuItem("~A~bout",cmAboutCmd,kbAltA,hcNoContext,0,
						  new TMenuItem("~P~rogress Bar",cmStatusCmd,kbAltL,hcNoContext,0)))));
}

//========================================================================

void TMyApplication::handleEvent(TEvent &event)
{
	 TApplication::handleEvent(event);

	 if (event.what == evCommand)
	 {
		  switch (event.message.command)
		  {
		  case cmAboutCmd:
		  {
				aboutDlg();
				clearEvent(event);
				break;
		  }
		  case cmStatusCmd:
		  {
				statusDlg();
				clearEvent(event);
				break;
		  }
		  }
	 }
}

//========================================================================

void TMyApplication::aboutDlg()
{
	 TDialog *pd = new TDialog(TRect(0,0,35,12),"About");
	 if (pd)
	 {
	pd->options |= ofCentered;
	pd->insert(new TStaticText(TRect(1,2,34,7),
		"\003Turbo Vision Example\n\003\n"
					 "\003Streaming a Progress Bar\n\003\n"));
		  pd->insert(new TButton(TRect(3,9,32,11),"~O~k",cmOK,bfDefault));

	if (validView(pd) != 0)
	{
				deskTop->execView(pd);

	    destroy(pd);
	}
	 }
}

//========================================================================

Boolean TMyApplication::isCancel(TDialog *pd)
{
   TEvent event;
   pd->getEvent(event);
   pd->handleEvent(event);
   if(event.what==evCommand && event.message.command==cmCancel)
      return (messageBox("Are you sure you want to Cancel",mfConfirmation|mfYesButton|mfNoButton)==cmYes ? True : False);
   else
      return False;
}

void TMyApplication::statusDlg()
{
   // Read a progress bar from a stream.
   TProgressBar *pbar;
   TDialog *pd;
   fpstream *ofps=0;
   ofps = new fpstream( rezFileName, CLY_IOSIn | CLY_IOSBin );
   if( !ofps->good() )
     messageBox("Stream open error", mfError|mfOKButton );

   TResourceFile *myRez;
   myRez = new TResourceFile( ofps );

   if( !myRez )
     messageBox("Resource file error", mfError|mfOKButton );
   else
     pbar = (TProgressBar*)myRez->get("theProgressBar");

   if( !pbar )// if "getting" resource failed...
     messageBox("Get resource error", mfError|mfOKButton );
   else {

      // Create the dialog box.
      pd = (TDialog*)myRez->get("theDialogBox");
      pd->insert(pbar);
      TProgram::deskTop->insert(pd);     // Modeless !!!!

      int i=0;
      Boolean keepOnGoing=True;

      TRect r(5,5,pd->size.x-5,pd->size.y-5);
      TStaticText *theMessage;

      // The first 3rd.
      theMessage = new TStaticText( r,
	 "This is a MODELESS dialog box. You can drag this box around the desktop." );
      pd->insert(theMessage);
      for(;i<=100;i++) {
	 pbar->update(i);
	 idle();
	 if(isCancel(pd)) {
	    keepOnGoing=False;
	    break;
	    }
	 delay(50);
	 }
      destroy(theMessage);

      if(keepOnGoing) {
	 // The second 3rd
	 theMessage = new TStaticText( r,
	    "Notice that only the attribute is changed to show progress" );
	 pd->insert(theMessage);
	 for(;i<=200;i++) {
	    pbar->update(i);
	    idle();
	    if(isCancel(pd)) {
	       keepOnGoing=False;
	       break;
	       }
	    delay(50);
	    }
	 destroy(theMessage);
	 }

      if(keepOnGoing) {
	 // The last 3rd
	 theMessage = new TStaticText( r,
	    "Syntax: TProgressBar(TRect &r, double total, char bar);" );
	 pd->insert(theMessage);
	 for(;i<=300;i++) {
	    pbar->update(i);
	    idle();
	    if(isCancel(pd)) {
	       keepOnGoing=False;
	       break;
	       }
	    delay(50);
	    }
	 destroy(theMessage); // not necessary since we destroy
			      // the TDialog also
	 }
      destroy(pd);
      }

}

//========================================================================

int main(void)
{
   TMyApplication myApplication;
   myApplication.run();
   return 0;
}






