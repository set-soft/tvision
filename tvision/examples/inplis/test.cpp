/*

  Created by L. Borobia 92-11-30

  File: TEST.CPP

  Testprogram to show how you can use the TInputBox.

*/

#include <stdio.h>

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
#define Uses_TDeskTop

#include "inplist.h"

const int cmInputBox  = 100;

class TMyApp : public TApplication
 {
  public:
   TMyApp();
   static TStatusLine *initStatusLine( TRect r );
   static TMenuBar *initMenuBar( TRect r );
   virtual void handleEvent( TEvent& event);
   void inputBox();
 };


TMyApp::TMyApp() :
    TProgInit( &TMyApp::initStatusLine,
	       &TMyApp::initMenuBar,
	       &TMyApp::initDeskTop
	     )
 {
 }

void TMyApp::handleEvent(TEvent& event)
 {
  TApplication::handleEvent(event);
  if( event.what == evCommand )
      {
	switch( event.message.command )
	    {
	     case cmInputBox:
	       inputBox();
	      break;

	     default:
	      return;
	    }
	clearEvent( event );
      }
 }

TMenuBar *TMyApp::initMenuBar( TRect r )
 {
  r.b.y = r.a.y + 1;
  return new TMenuBar( r,
     *new TSubMenu( "~D~emo", kbAltD ) +
       *new TMenuItem( "~T~est", cmInputBox, kbNoKey, hcNoContext, "" )+
       *new TMenuItem( "E~x~it", cmQuit, kbNoKey, hcNoContext, "Alt-X" )
     );
 }

TStatusLine *TMyApp::initStatusLine( TRect r )
 {
    r.a.y = r.b.y - 1;
    return new TStatusLine( r,
	*new TStatusDef( 0, 0xFFFF ) +
	    *new TStatusItem( 0, kbF10, cmMenu ) +
	    *new TStatusItem( "~Alt-X~ Exit", kbAltX, cmQuit )
	);
 }

void TMyApp::inputBox()
 {
  short i = 0;
  const int MAXINPLN = 30;
  ListBoxItem *v;
  char label[MAXLABELLEN];
  TLineCollection *list;
  //TRect r = getExtent();
  list = new TLineCollection(MAXINPLN,0);
  for (i = 0; i < MAXINPLN; i++)
   {
    v = new ListBoxItem;
    sprintf(label,"Label No. %d",i);
    strcpy(v->label,label);
    strcpy(v->value,"value");
    list->insert(v);
   }
  TInputDialog *pd = new TInputDialog(TRect(0,0,50,19),"Input Box", list);
  if (validView(pd))
   {
    deskTop->execView( pd );
    CLY_destroy( pd );
   }
 }

int main()
 {
  TMyApp myApp;
  myApp.run();
  return 0;
 }
