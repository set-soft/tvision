// ***********************************************************************
//
//  LISTBOX1.CPP Test Program
//  TListViewer Example - Scrolling Lists
//  revised November 27,1993
//  C.Porter 70262,1047
// ***********************************************************************
#include <stdlib.h>

#define Uses_TApplication
#define Uses_TBackground
#define Uses_TButton
#define Uses_TInputLine
#define Uses_TKeys
#define Uses_TDeskTop
#define Uses_TDialog
#define Uses_TListBox
#define Uses_TMenu
#define Uses_TMenuBar
#define Uses_TMenuItem
#define Uses_TRect
#define Uses_TScrollBar
#define Uses_TStaticText
#define Uses_TStatusDef
#define Uses_TStatusItem
#define Uses_TStatusLine

#include <tv.h>
#include "lst_view.h"

char *cityList[] = { "Boston MA.   ",
				"Washington DC  ",
				"Orlando FL.    ",
				"New York City N.Y",
				"Chicago IL ",
				"Dallas TX.",
				"Birmingham AL.",
				"Memphis TN ",
				"Denver CO",
				"San Francisco CA.",
				"Los Angeles CA.",
				"Fort Worth TX",
				"Seattle WA", NULL };
char *numList[] = {"1","2","3","4","5","6","7","8","9","10","11","12","13",NULL};

//  global data
const int cmAbout   = 100;  // User selected menu item 'About'
const int cmList    = 101;  // User selected menu item 'List'

//*******************************************************
class TApp : public TApplication {
  public:

    TApp();

    // virtual functions to be locally redefined
    static TMenuBar *initMenuBar(TRect r);
    void handleEvent(TEvent &event);
    void idle();

    // declare new functions
    void AboutDialog();
    void ListDialog();
};
//*******************************************************

TApp::TApp() : TProgInit(TApplication::initStatusLine,
					TApp::initMenuBar,
					TApplication::initDeskTop) {

  TRect r = getExtent();
  r.a.x = r.b.x - 13;
  r.a.y = r.b.y - 1;

}
//*******************************************************

TMenuBar *TApp::initMenuBar(TRect r) {

  r.b.y = r.a.y + 1;
  return(new TMenuBar(r, new TMenu(
    *new TMenuItem("~A~bout", cmAbout, kbAltA, hcNoContext, 0,
	 new TMenuItem("~L~ist", cmList, kbAltL, hcNoContext, 0)))));
}
//*******************************************************

void TApp::handleEvent(TEvent &event) {

  TApplication::handleEvent(event);
  if (event.what == evCommand) {
    switch(event.message.command) {
	 case cmAbout:       // display the about box
	   AboutDialog();
	   clearEvent(event);
	   break;
	 case cmList:        // display our list box
	   ListDialog();
	   clearEvent(event);
	   break;
    }
  }
}
//*******************************************************
/* AboutDialog - create modal About dialog box */

void TApp::AboutDialog() {

  TDialog *pd = new TDialog(TRect(0, 0, 35, 10), "About");
  if (pd) {
	pd->options |= ofCentered;
	pd->insert(new TStaticText(TRect(1, 2, 34, 6),
			"\003TListViewBox Example 1\n"
			"\003A Simple TListViewer\n"
			"\003C.Porter CIS 70262,1047\n"));
	pd->insert(new TButton(TRect(13, 7, 22,9), "~O~k", cmOK, bfDefault));
	   deskTop->execView(pd);
  }
  destroy(pd);
}
//*******************************************************

void TApp::ListDialog() {

  TDialog *pd = new TListViewDialog(TRect(0, 0, 30, 15), "Scroll List",cityList,numList,13);

  if(validView(pd)) deskTop->execView(pd);

  destroy(pd);
}
//*******************************************************

void TApp::idle() {

  TProgram::idle();

}

//************************************************************************
int main(void) {
  TApp myApp;
  myApp.run();
  return 0;
}
