// This is an example of a progress bar.
//
// Modified THERMO.ZIP in the C++ lib to work with TURBO VISION
// and fixed a small bug.
//
// Author: Jay Perez.
#define Uses_unistd
#define Uses_stdio
#define Uses_stdlib
#define Uses_ctype

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
#define Uses_string
 
#include <tv.h>
__link( RView )
__link( RDialog )
__link( RButton )

#include "tprogbar.h"

const int sampleIterations = 21 ;

const int cmAboutCmd   = 100;  // User selected menu item 'About'
const int cmStatusCmd  = 101;  // User selected menu item 'Progress Bar'


//========================================================================

class myProgress : public TProgressBar
{
	 public :
	 myProgress(const TRect& r, unsigned long iters ) :
		TProgressBar( r , iters ){};
	 virtual void mainProcess( void );
};

myProgress *bar ;

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
					 "\003Creating a Progress Bar\n\003\n"));
		  pd->insert(new TButton(TRect(3,9,32,11),"~O~k",cmOK,bfDefault));
 
        if (validView(pd) != 0)
        {
				deskTop->execView(pd);
 
            CLY_destroy(pd);
        }
	 }
}

//========================================================================

void TMyApplication::statusDlg()
{
	 TView *tv;

	 TDialog *pd = new TDialog( TRect( 10, 3, 69, 11 ), "Please Wait" ) ;
	 if (pd) {

	 pd->options |= ofFirstClick;
	 pd->options |= ofCentered;

	 TRect r = TRect(4,4,25,6) ;
	 tv = new TButton(r, "~S~tart", cmOK, bfNormal | bfBroadcast  ) ;
	 tv->options &= ~ofSelectable;
	 pd->insert( tv );

	 r = TRect(34,4,55,6) ;
	 tv = new TButton( r, "~C~ancel", cmCancel, bfNormal);
	 pd->insert( tv );

	 bar = new myProgress( TRect( 4, 2, 55, 3 ), 0 ) ;

	 pd->insert( ( TView * ) bar ) ;

	 if( TProgram::application->validView( pd ) != 0 ) {    // If it's valid...
		 TProgram::deskTop->execView( pd ) ;
		 TObject::CLY_destroy( pd ) ;
	 }
	 }
	 return ;
}

//========================================================================

int main(void)
{
	 TMyApplication myApplication;

	 myApplication.run();

	 return 0;
}

//========================================================================

void myProgress::mainProcess( void )
{

	 // Place your processing logic here...

	 unsigned long cnt = 0 ;

	 setMaxIter( sampleIterations ) ;   // set maximum number of iterations...
													// do it here or on the constructor ...
													// or both.

	 for( int x = 0; x < sampleIterations; x++ )
	 {
		  setCurIter( ++cnt );         // set the current iteration count & update
		  usleep(500000);
	 }

	 usleep(500000);
	 message(owner,evCommand,cmOK,this);// close dialog box
}

//========================================================================
