/*************************************************************************/
/*                                                                       */
/* Test program file for TComboBox and TStaticInputLine classes.         */
/*                                                                       */
/* Copyright (c) 1992, Vincent J. Dentice                                */
/* All rights reserved                                                   */
/*                                                                       */
/*************************************************************************/

#define Uses_ifpstream
#define Uses_ofpstream
#define Uses_TApplication
#define Uses_TButton
#define Uses_TDeskTop
#define Uses_TDialog
#define Uses_TEvent
#define Uses_TInputLine
#define Uses_TKeys
#define Uses_TMenuBar
#define Uses_TMenuItem
#define Uses_TRect
#define Uses_TStatusLine
#define Uses_TStatusItem
#define Uses_TStatusDef
#define Uses_TStringCollection
#define Uses_TSubMenu
#include <tv.h>

#define Uses_TComboBox
#define Uses_TStaticInputLine
#include "tcombobx.h"
#include "tsinputl.h"

const int cmNewDialog = 200; // assign new command values


class TMyApp : public TApplication {
   public:
      TMyApp();
      static TStatusLine *initStatusLine( TRect r );
      static TMenuBar *initMenuBar( TRect r );
      virtual void handleEvent( TEvent& event);
      void newDialog();
};

//static short winNumber = 0;          // initialize window number


TMyApp::TMyApp() : TProgInit(&TMyApp::initStatusLine,
			     &TMyApp::initMenuBar,
			     &TMyApp::initDeskTop)
{
}


TStatusLine *TMyApp::initStatusLine(TRect r)
{
   r.a.y = r.b.y - 1;     // move top to 1 line above bottom
   return new TStatusLine( r,
      *new TStatusDef( 0, 0xFFFF ) +
	 *new TStatusItem( 0, kbF10, cmMenu ) +
	 *new TStatusItem( "~Alt-X~ Exit", kbAltX, cmQuit ) +
	 *new TStatusItem( "~Alt-F3~ Close", kbAltF3, cmClose )
      );
}

TMenuBar *TMyApp::initMenuBar( TRect r )
{
   r.b.y = r.a.y + 1;    // set bottom line 1 line below top line
   return new TMenuBar( r,
      *new TSubMenu( "~F~ile", kbAltF )+
	 *new TMenuItem( "~D~ialog", cmNewDialog, kbF3, hcNoContext, "F3" )+
	  newLine()+
	 *new TMenuItem( "E~x~it", cmQuit, cmQuit, hcNoContext, "Alt-X" )
      );
}

void TMyApp::handleEvent(TEvent& event)
{
   TApplication::handleEvent(event); // act like base!
   if(event.what == evCommand) {
      switch(event.message.command) {
	 case cmNewDialog:      // but respond to additional commands
	    newDialog();        // define action for cmMyNewWin
	    break;
	 default:
	    return;
      }
      clearEvent( event );       // clear event after handling
   }
}

void TMyApp::newDialog()
{
   TDialog    *pd;
   TInputLine *tv;
   TStringCollection *list;
   struct {
      char line1[128];
      TCollection *list1;
      char line2[128];
      TCollection *list2;
   } data;

   list = new TStringCollection(5,2);

   list->insert(newStr("Test 1"));
   list->insert(newStr("Test 2"));

   TRect r(2,1,27,10);

   pd = new TDialog(r, "Test Dialog");
   if (pd) {
      tv = new TInputLine(TRect(2,1,20,2), 128);
      pd->insert(tv);
      pd->insert(new TComboBox(TRect(20,1,21,2), tv, list));

      tv = new TStaticInputLine(TRect(2,3,20,4), 128, list);
      pd->insert(tv);
      pd->insert(new TComboBox(TRect(20,3,21,4), tv, list));

      pd->insert(new TButton(TRect( 1,6,11,8), "~O~K", cmOK, bfDefault));
      pd->insert(new TButton(TRect(12,6,22,8), "Cancel", cmCancel, bfNormal));
   }

   pd->selectNext(False);
   deskTop->execView(pd);

   pd->getData(&data);

   CLY_destroy(pd);
   CLY_destroy(list);
}

int main()
{
   TMyApp myApp;
   myApp.run();
   return 0;
}
