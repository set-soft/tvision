// This is an example of a progress bar.
//
// Modified THERMO.ZIP in the C++ lib to work with TURBO VISION
// and fixed a small bug.
//
// Author: Jay Perez. Modified by Barnaby Falls

#define Uses_TBackground
#define Uses_TListBox
#define Uses_TMenu
#define Uses_TMenuBar
#define Uses_TMenuItem
#define Uses_TScrollBar
#define Uses_TStaticText
#define Uses_TStatusDef
#define Uses_TStatusItem
#define Uses_TStatusLine
#define Uses_TStringCollection
#define Uses_MsgBox
#define Uses_TEventQueue
#define Uses_TApplication
#define Uses_TRect
#define Uses_TDeskTop
#define Uses_TView
#define Uses_TWindow
#define Uses_TDialog
#define Uses_TButton
#define Uses_StaticText
#define Uses_TSItem
#define Uses_TLabel
#define Uses_TInputLine
#define Uses_TEvent
#define Uses_TKeys
#define Uses_TDrawBuffer
#define Uses_TStreamableClass
#define Uses_TStreamable
 
#include <tv.h>
__link( RView )
__link( RDialog )
__link( RButton )
#include <mem.h>                       // memset

#include <dos.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <strstrea.h>
#include <iomanip.h>
#include <conio.h>

#include "tprogbar.h"

const cmAboutCmd   = 100;  // User selected menu item 'About'
const cmStatusCmd  = 101;  // User selected menu item 'Progress Bar'

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
					 "\003Using a Progress Bar\n\003\n"));
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
   TDialog *pd = new TDialog(TRect(0,0,60,15),"Example Progress Bar");
   pd->flags &= ~wfClose;
   pd->options |= ofCentered;
   TProgressBar *pbar = new TProgressBar(TRect(2,2,pd->size.x-2,3),300);
   pd->insert(pbar);
   pd->insert(new TButton(TRect(10,pd->size.y-3,pd->size.x-10,pd->size.y-1),"~C~ancel",cmCancel,bfDefault));
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

//========================================================================

int main(void)
{
	 TMyApplication myApplication;

	 myApplication.run();

	 return 0;
}

