//========================================================================
//  The following example routines have been provided by the Technical
//  Support staff at Borland International.  They are provided as a
//  courtesy and not as part of a Borland product, and as such, are
//  provided without the assurance of technical support or any specific
//  guarantees.
//========================================================================
//
// Ported and contributed by Joel <jso@europay.com>
// I (SET) did some changes to show some details and hence make it more
// instructive.
//
//========================================================================

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
#define Uses_TPalette

#define Uses_string
#include <tv.h>

#define   PATTERN 177

const int cmAbout   = 100;  // User selected menu item 'About'

// Following logo is just a simple text
const char *logo = "To the success of FSF ";

//========================================================================
//  class definitions
//------------------------------------------------------------------------
class TvApp : public TApplication {
	 //  main application class

public:
	 TvApp();

	 static TMenuBar *initMenuBar( TRect r );
	 static TDeskTop *initDeskTop( TRect r );
	 void handleEvent( TEvent &event );
	 void AboutDialog();

	 virtual TPalette& getPalette() const;

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
// Original Design
//  TDrawBuffer b;
//
//  for(int col= 0; col < size.y; col++)
//	 {
//	 for( int lin= 0; lin < size.x; lin++)
//		{
//		b.moveChar( lin, logo[(col + lin) % strlen(logo)], getColor(0x01), 1 );
//		}
//	 writeLine( 0, col, size.x, 1, b );
//	 }
// End Original Design

// Alternate Design
// This design was originaly implemented in the book:
//  "TURBO VISION POUR C++" of  G. Leblanc (Ed. EYROLLES)
// which I adapted for the choice of *logo (a C string)
// in place of *lines[] (a table of C string)

 #if 0
 TRect r = getClipRect();           // To refresh only what is necessary

 // SET: note that even when valid this aproach is very slow. The original
 // approach is better oriented, but the call to getColor() inside the loop
 // is a big mistake. Anyways, these are 2 examples.
 for (int col=r.a.x; col<r.b.x; col++)
   for (int li=r.a.y; li<r.b.y; li++)
     writeChar(col, li, logo[(col + li) % strlen(logo)], 1, 1);
 #else
  // SET: here is my modified version as a third example.
  // Some important stuff:
  // 1) getColor is called outside the loops, it means once instead of 1760
  // times ;-). (Considering a full draw)
  // 2) The modulus operation is slow (is a div) so is called only once per
  // line.
  // 3) The len of the string is called outside the loop too.
  // 4) The video memory is accessed only once per line, not once per
  // character.
  // And off course only getClipRect() is redrawed.
  TDrawBuffer b;
  int lin,len,col,counter,width;
  ushort color=getColor(0x01);
  TRect r=getClipRect();
  len=strlen(logo);

  width=r.b.x-r.a.x;
  for(lin=r.a.y; lin<r.b.y; lin++)
	 {
	 for(counter=(lin+r.a.x)%len, col=0; col<width; col++)
		{
		b.moveChar(col,logo[counter],color,1);
      if (++counter==len)
         counter=0;
		}
	 writeLine(r.a.x,lin,width,1,b);
	 }
 #endif

// End Alternate Design
 
}

TvApp::TvApp() : TProgInit( &TvApp::initStatusLine,
			 &TvApp::initMenuBar, &TvApp::initDeskTop )
{
 deskTop->redraw(); //SET: This extra redraw is needed to use the new palette
}

TMenuBar *TvApp::initMenuBar( TRect r )
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
TDeskTop *TvApp::initDeskTop( TRect r )
{
	 // initDeskTop is passed a TRect which is the size of the
	 // application's window - in our case the whole screen
	 r.a.y++;
	 r.b.y--;
	 return ( new TNewDeskTop( r ) );

}

void TvApp::handleEvent (TEvent &event)
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
void TvApp::AboutDialog()
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


TPalette& TvApp::getPalette() const
{
 // SET: The following isn't recommendable because that's executed too often
 static TPalette palette(TApplication::getPalette());
 if (appPalette == apColor)
 {                     // modification of application palette
  palette[1] = 0x02;   // Red on Blue for normal article
  palette[2] = 0x14;
  palette[5] = 0x52;   // Green on Blue for preselected article
 }
 return palette;
}

//========================================================================
//  implementation of TNewDeskTop
//------------------------------------------------------------------------
TNewDeskTop::TNewDeskTop( const TRect& r ) :
			 TDeskInit( &initBackground ), TDeskTop( r )
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
	TvApp myApp;
	myApp.run();
	return 0;
}
