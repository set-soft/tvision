// Copyright (C) 1992 James H. Price, All rights reserved
//
//  TESTDYN.CPP
//
//    Test program for dynamic text
//

#define Uses_MsgBox
#define Uses_TEvent
#define Uses_TApplication
#define Uses_TKeys
#define Uses_TRect
#define Uses_TMenu
#define Uses_TMenuBar
#define Uses_TMenuItem
#define Uses_TStatusLine
#define Uses_TStatusItem
#define Uses_TStatusDef
#define Uses_TDeskTop
#define Uses_TDialog
#define Uses_TInputLine
#define Uses_TButton
#include <tv.h>

#include "dyntext.h"

const int cmAbout = 100;
const int cmTest  = 101;


////////////////////////////////////////////////////////////////////////
//////////////////////// Usual TurboVision Stuff ///////////////////////

class Shell : public TApplication {

public:

	Shell();
	static TMenuBar *initMenuBar( TRect r );
	static TStatusLine *initStatusLine( TRect r );
	void handleEvent( TEvent& event );
	void idle();

private:

	void About();
	void Test();
};

Shell::Shell() : TProgInit (
			&Shell::initStatusLine,
			&Shell::initMenuBar,
			&Shell::initDeskTop
			)
{
}

TMenuBar *Shell::initMenuBar( TRect r )
{
	r.b.y = r.a.y + 1;

	TMenuItem& mI =
		*new TMenuItem( "~T~est", cmTest, kbNoKey, hcNoContext );

	return new TMenuBar( r, new TMenu( mI ) );
}

TStatusLine *Shell::initStatusLine( TRect r )
{
	r.a.y = r.b.y - 1;

	TStatusLine *sL = new TStatusLine( r,
		*new TStatusDef(0, 0xFFFF) +
			*new TStatusItem( 0, kbF10, cmMenu ) +
			*new TStatusItem( "~Alt-X~ Quit", kbAltX, cmQuit ) );

	return sL;
}

void Shell::handleEvent( TEvent &event )
{
	TApplication::handleEvent(event);

	if (event.what == evCommand)
	{
		switch (event.message.command)
		{
			case cmAbout:
				About();
				break;
			case cmTest:
				Test();
				break;
			default:
				break;
		}
	}
}

void Shell::About()
{
	messageBox( "\003Dynamic Text Demo",
		mfInformation | mfOKButton );
}

void Shell::idle()
{
	TProgram::idle();

	// select menu bar if deskTop empty

	if( deskTop->current == 0
	&& !menuBar->getState( sfSelected ) )
	{
		TEvent event;
		event.what = evCommand;
		event.message.command = cmMenu;   // put a cmMenu event in queue
		putEvent( event );
	}
}


//////////////////////////////////////////////////////////////////////////
////////////////////////  Simple Test Dialog Box  ////////////////////////

class TestDialog : public TDialog {
public:
	TestDialog();
	void handleEvent( TEvent& event );
private:
	TInputLine *master;
	DynamicText *slave;
};

TestDialog::TestDialog() : 
		TWindowInit( &TestDialog::initFrame ),
                TDialog( TRect(0,0,40,10), "Dynamic Text" )
{
	options |= ofCentered;

	master = new TInputLine( TRect(10,2,32,3), 21 );
	insert( master );

	// This allocates a 21 byte string (20 + 1 for null).  The 'False'
	// argument indicates that it should be left-justified
	slave = new DynamicText( TRect(11,4,31,5), "Initial text", False );
	insert(slave);

	insert( new TButton( TRect(15,6,25,8), "O~K~", cmOK, bfNormal ) );

	master->select();
}

void TestDialog::handleEvent( TEvent& event )
{
	if( event.what == evKeyDown
	&& event.keyDown.keyCode == kbTab )
	{
		char buf[30];
		master->getData( buf );		// read the input
		slave->setText( buf );    // write it into static text
	}
	TDialog::handleEvent( event );
}

///////////////////////////////////////////////////////////////////////////
///////////////////////// Test routine ////////////////////////////////////

void Shell::Test()
{
	TestDialog *d = new TestDialog;   // create dialog
	deskTop->execView( d );           // execute it
	CLY_destroy( d );                 // destroy it

	// Note: This doesn't illustrate the getData() and setData()
	// functions, which work as usual for TVision objects.  If you
	// use TDialog::getData and setData, you *must* have space in
	// void *rec structure for any DynamicText objects,
}

///////////////////////////////////////////////////////////////////////////
////////////////////////////// main() function ////////////////////////////

int main()
{
	TEvent init;
	init.what = evCommand;
	init.message.command = cmAbout;     // make a cmAbout command event

	Shell shell;
	shell.putEvent(init);               // put it in the queue to pop up
	shell.run();                        // About box when program starts
	return 0;
}

