// Copyright (C) 1992 James H. Price, All rights reserved
//
//  TESTDYN.CPP
//
//    Test program for dynamic text
//

#define Uses_unistd
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
#ifdef TVCompf_djgpp
#include <dos.h>
#endif
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


///////////////////////////////////////////////////////////////////////////
///////////////////////// Test routine ////////////////////////////////////

extern void postMsg(const char *);
extern void postInfo(int, const char *);

void Shell::Test()
{
 //postMsg("Hola");
 postInfo(1,"  Chau");
 CLY_YieldProcessor(1000000);
 postInfo(2,"  Pepe");
}


///////////////////////////////////////////////////////////////////////////
////////////////////////////// main() function ////////////////////////////

int main()
{
	Shell shell;
	shell.run();                        // About box when program starts
	return 0;
}

