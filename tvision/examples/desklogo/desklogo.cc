//========================================================================
//  The following example routines have been provided by the Technical
//  Support staff at Borland International.  They are provided as a
//  courtesy and not as part of a Borland product, and as such, are
//  provided without the assurance of technical support or any specific
//  guarantees.
//========================================================================
//
// Ported and contributed by Joel <jso@europay.com>
//
//========================================================================


#define Uses_string

#define Uses_TApplication
#define Uses_TBackground
#define Uses_TButton
#define Uses_TKeys
#define Uses_TDeskTop
#define Uses_TDialog
#define Uses_TMenu
#define Uses_TMenuBar
#define Uses_TMenuItem
#define Uses_TRect
#define Uses_TStaticText
#define Uses_TStatusDef
#define Uses_TStatusItem
#define Uses_TStatusLine

#include <tv.h>

#define   PATTERN 177

const int cmAbout   = 100;  // User selected menu item 'About'

char *lines[] ={"±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±",
		"±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±",
		"±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±",
		"±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±",
		"±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±",
		"±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±",
    "±±±±±±±±±±±±±±±±±±±±²²²²²²²²²²²²²²²²²²±±±±²²±±±±±±±±±±±±±±²²±±±±±±±±±±±±±±±±±±±±±",
    "±±±±±±±±±±±±±±±±±±±±±±±±±±±±²²±±±±±±±±±±±±±²²±±±±±±±±±±±±²²±±±±±±±±±±±±±±±±±±±±±±",
    "±±±±±±±±±±±±±±±±±±±±±±±±±±±±²²±±±±±±±±±±±±±±²²±±±±±±±±±±²²±±±±±±±±±±±±±±±±±±±±±±±",
    "±±±±±±±±±±±±±±±±±±±±±±±±±±±±²²±±±±±±±±±±±±±±±²²±±±±±±±±²²±±±±±±±±±±±±±±±±±±±±±±±±",
    "±±±±±±±±±±±±±±±±±±±±±±±±±±±±²²±±±±±±±±±±±±±±±±²²±±±±±±²²±±±±±±±±±±±±±±±±±±±±±±±±±",
    "±±±±±±±±±±±±±±±±±±±±±±±±±±±±²²±±±±±±±±±±±±±±±±±²²±±±±²²±±±±±±±±±±±±±±±±±±±±±±±±±±",
    "±±±±±±±±±±±±±±±±±±±±±±±±±±±±²²±±±±±±±±±±±±±±±±±±²²±±²²±±±±±±±±±±±±±±±±±±±±±±±±±±±",
    "±±±±±±±±±±±±±±±±±±±±±±±±±±±±²²±±±±±±±±±±±±±±±±±±±±²²±±±±±±±±±±±±±±±±±±±±±±±±±±±±±",
		"±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±",
		"±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±",
		"±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±",
		"±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±",
		"±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±",
		"±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±",
		"±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±",
		"±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±",
		"±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±" };


//========================================================================
//  class definitions
//------------------------------------------------------------------------
class TApp : public TApplication {
    //  main application class

public:
    TApp();

    static TMenuBar *initMenuBar( TRect r );
    static TDeskTop *initDeskTop( TRect r );
    void handleEvent( TEvent &event );
    void AboutDialog();

};

//------------------------------------------------------------------------
class TNewDeskTop : public TDeskTop
{   // we derive a new desk top which will instantiate the background
    // object with the character we select for the pattern
    // also, the initBackground function is overridden to return
    // our specialized TNewBackground
public:
    TNewDeskTop( const TRect& r );
    static TBackground* initBackground( TRect r );
};

//------------------------------------------------------------------------
class TNewBackground : public TBackground
{
public:
    TNewBackground( const TRect& r, char pattern );
    void draw();
};

//===========================================================================
//           Draw the background
//===========================================================================
void TNewBackground::draw()
{

  TDrawBuffer b;

  for(int i= 0; i < size.y; i++)
	 {
	 for( int j= 0; j < size.x; j++)
		{
		if (i<23 && j<80)
			b.moveChar( j, lines[i][j], getColor(0x01), 1 );
		else
			b.moveChar( j, '±', getColor(0x01), 1 );
		}
	 writeLine( 0, i, size.x, 1, b );
	 }

}


TApp::TApp() : TProgInit( &TApplication::initStatusLine,
		    &TApp::initMenuBar, &TApp::initDeskTop )
{
}



TMenuBar *TApp::initMenuBar( TRect r )
{
    r.b.y = r.a.y + 1;
    return( new TMenuBar( r, new TMenu(
	*new TMenuItem( "~A~bout", cmAbout, kbAltA, hcNoContext, 0 )
	) ) );
}

//------------------------------------------------------------------------
//  redefine desktop initialization to cause usage of newly
//  defined desktop
//------------------------------------------------------------------------
TDeskTop *TApp::initDeskTop( TRect r )
{
    // initDeskTop is passed a TRect which is the size of the
    // application's window - in our case the whole screen
    r.a.y++;
    r.b.y--;
    return ( new TNewDeskTop( r ) );

}



void TApp::handleEvent (TEvent &event)
{
  // override the handleEvent so we can call the about box
  TApplication::handleEvent( event );
  if( event.what == evCommand )
	 {
	 switch( event.message.command )
		{
		case cmAbout:
				 {
				 AboutDialog();
				 clearEvent( event );
				 break;
				 }
		}
	 }
}

//------------------------------------------------------------------------
// create modal About dialog box
//------------------------------------------------------------------------
void TApp::AboutDialog()
{

    TDialog *pd = new TDialog( TRect( 0, 0, 35, 12 ), "About" );
    if (pd)
    {
	pd->options |= ofCentered;
	pd->insert( new TStaticText( TRect( 1, 2, 34, 7 ),
		"\003Turbo Vision Example\n\003\n"
		"\003Modifying the desk top\n\003\n"
		"\003Borland Technical Support"));
	pd->insert( new TButton( TRect( 3,9,32,11 ), "~O~k",
				cmOK, bfDefault ) );
	deskTop->execView( pd );
    }
    CLY_destroy( pd );
}

//========================================================================
//  implementation of TNewDeskTop
//------------------------------------------------------------------------
TNewDeskTop::TNewDeskTop( const TRect& r ) :
		    TDeskInit( &initBackground ),
		    TDeskTop( r )
{
}

//------------------------------------------------------------------------

TBackground *TNewDeskTop::initBackground( TRect r )
{
    // initializing the TBackground object allows us
    // to pass it a character for the desktop pattern
    return new TNewBackground( r, PATTERN );
}


//========================================================================
//  implementation of TNewBackground
//------------------------------------------------------------------------

TNewBackground::TNewBackground( const TRect& r, char pattern ) :
			TBackground( r, pattern )
{
}

//========================================================================
int main(void)
{
    TApp myApp;
    myApp.run();
    return 0;
}
