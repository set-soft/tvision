/*---------------------------------------------------------*/
/*                                                         */
/*   Turbo Vision 1.0                                      */
/*   TVGUID07 Demo Source File                             */
/*   Copyright (c) 1991 by Borland International           */
/*                                                         */
/*---------------------------------------------------------*/
/*
  This is a modified version of tvguid07.cc that can display UTF-8 encoded
files.

  Taked from the Sergio Sigala <ssigala@globalnet.it> Turbo Vision port to
UNIX.
  LSM: TurboVision for UNIX
  ftp://sunsite.unc.edu /pub/Linux/devel/lang/c++/tvision-0.6.tar.gz
  Copying policy: BSD
  Adapted by Salvador Eduardo Tropea (SET) <set-soft@usa.net>.

  Now the TInterior::draw is impoved and the resize is correctly handled.
*/

// same as tvguid06 except for improved draw method

#define Uses_stdlib             // for exit(), rand()
#define Uses_iostream
#define Uses_fstream
#define Uses_stdio              // for puts() etc
#define Uses_string             // for strlen etc
#define Uses_ctype
#define Uses_getline

#define Uses_TEventQueue
#define Uses_TEvent
#define Uses_TProgram
#define Uses_TApplication
#define Uses_TKeys
#define Uses_TRect
#define Uses_TMenuBar
#define Uses_TSubMenu
#define Uses_TMenuItem
#define Uses_TStatusLine
#define Uses_TStatusItem
#define Uses_TStatusDef
#define Uses_TDeskTop
#define Uses_TView
#define Uses_TWindow
#define Uses_TScroller
#include <tv.h>

UsingNamespaceStd

const int cmMyFileOpen = 200; // assign new command values
const int cmMyNewWin   = 201;

const char *fileToRead = "test.utf8";
const int maxLineLength = maxViewWidth+1;
const int maxLines      = 100;
uint16 *lines[maxLines];
int lineCount = 0;

static inline
uint16 charUTF8_2_U16(const char *&b)
{
 int c=*((uchar *)(b++));
 int n,t;

 if (c & 0x80)
   {
    n=1;
    while (c & (0x80>>n))
      n++;

    c&=(1<<(8-n))-1;

    while (--n>0)
      {
       t=*((unsigned char *)(b++));
       if ((!(t&0x80)) || (t&0x40))
          return 0;
       c=(c<<6) | (t&0x3F);
      }
   }

 return c;
}

static
uint16 *strUTF8_2_U16(const char *b, ssize_t len)
{
 uint16 tmp[len*2];
 const char *end=b+len;
 int retLen=0;
 while (b<end)
    tmp[retLen++]=charUTF8_2_U16(b);
 tmp[retLen]=0;
 retLen++;
 uint16 *ret=new uint16[retLen];
 memcpy(ret,tmp,retLen*2);
 return ret;
}

void readFile( const char *fileName )
{
 FILE *f=fopen( fileName, "rt" );
 if (!f)
   {
    cout << "Invalid file name..." << endl;
    exit(1);
   }
 else
   {
    char *line=NULL;
    size_t sz=0;
    ssize_t ret;
    while (lineCount<maxLines && !feof(f))
      {
       ret=CLY_getline(&line,&sz,f);
       if (ret!=-1)
         {
          if (line[ret-1]=='\n')
             line[ret-1]=0;
          lines[lineCount++]=strUTF8_2_U16(line,ret);
         }
      }
   }
}

void deleteFile()
{
    for( int i = 0; i < lineCount; i++ )
        delete lines[i];
}

class TMyApp : public TApplication
{

public:
    TMyApp();
    static TStatusLine *initStatusLine( TRect r );
    static TMenuBar *initMenuBar( TRect r );
    virtual void handleEvent( TEvent& event);
    void myNewWindow();
};


static short winNumber = 0;             // initialize window number

class TDemoWindow : public TWindow      // define a new window class
{

public:

   TDemoWindow( const TRect& r, const char *aTitle, short aNumber );
    void makeInterior();

};

class TInterior : public TScroller
{

public:

    TInterior( const TRect& bounds, TScrollBar *aHScrollBar,
           TScrollBar *aVScrollBar ); // constructor
    virtual void draw();                // override TView::draw
};

TInterior::TInterior( const TRect& bounds, TScrollBar *aHScrollBar,
              TScrollBar *aVScrollBar ) :
       TScroller( bounds, aHScrollBar, aVScrollBar )
{
    growMode = gfGrowHiX | gfGrowHiY;   //make size follow that of the window
    options = options | ofFramed;
    state |= sfCursorVis;
    setLimit( maxLineLength, maxLines );
}

void TInterior::draw()
{
    // Attention! be careful with attributes the upper 8 bits returned
    // by getColor can mess things.
    unsigned color = getColor(0x0301) & 0xFF;
    for( int i = 0; i < size.y; i++ )
        {// Here we just use an Unicode16 buffer
        TDrawBufferU16 b;
        b.moveChar( 0, ' ', color, size.x );
        // fill line buffer with spaces
        int j = delta.y + i;       // delta is scroller offset
        if( lines[j] )
            {
            // This is something like strncpy but works for Unicode16
            uint16 d[maxLineLength];
            uint16 *o=lines[j];
            int p=0;
            while (p<size.x && o[p])
              {
               d[p]=o[p];
               p++;
              }
            d[p]=0;
            b.moveStr( 0, d, color );
            }
        writeLine( 0, i, size.x, 1, b);
        }
}

void TDemoWindow::makeInterior()
{
    TScrollBar *vScrollBar =
        standardScrollBar( sbVertical | sbHandleKeyboard );
    TScrollBar *hScrollBar =
        standardScrollBar( sbHorizontal |  sbHandleKeyboard );
    TRect r = getClipRect();    // get exposed view bounds
    r.grow( -1, -1 );           // shrink to fit inside window frame
    insert( new TInterior( r, hScrollBar, vScrollBar ));
}

TMyApp::TMyApp() :
    TProgInit( &TMyApp::initStatusLine,
               &TMyApp::initMenuBar,
               &TMyApp::initDeskTop
             )
{
}

TStatusLine *TMyApp::initStatusLine(TRect r)
{
    r.a.y = r.b.y - 1;     // move top to 1 line above bottom
    return new TStatusLine( r,
        *new TStatusDef( 0, 0xFFFF ) +
        // set range of help contexts
            *new TStatusItem( 0, kbF10, cmMenu ) +
            // define an item
            *new TStatusItem( "~Alt-X~ Exit", kbAltX, cmQuit ) +
            // and another one
            *new TStatusItem( "~Alt-F3~ Close", kbAltF3, cmClose )
            // and another one
        );
}

TMenuBar *TMyApp::initMenuBar( TRect r )
{
    r.b.y = r.a.y + 1;    // set bottom line 1 line below top line
    return new TMenuBar( r,
        *new TSubMenu( "~F~ile", kbAltF )+
            *new TMenuItem( "~O~pen", cmMyFileOpen, kbF3, hcNoContext, "F3" )+
            *new TMenuItem( "~N~ew",  cmMyNewWin,   kbF4, hcNoContext, "F4" )+
            newLine()+
            *new TMenuItem( "E~x~it", cmQuit, cmQuit, hcNoContext, "Alt-X" )+
        *new TSubMenu( "~W~indow", kbAltW )+
            *new TMenuItem( "~N~ext", cmNext,     kbF6, hcNoContext, "F6" )+
            *new TMenuItem( "~Z~oom", cmZoom,     kbF5, hcNoContext, "F5" )
        );
}

void TMyApp::handleEvent(TEvent& event)
{
    TApplication::handleEvent(event);   // act like base!
    if( event.what == evCommand )
        {
        switch( event.message.command )
            {
            case cmMyNewWin:            // but respond to additional commands
                myNewWindow();          // define action for cmMyNewWin
                                        //   command
                break;
            default:
                return;
            }
        clearEvent( event );            // clear event after handling
        }
}

void TMyApp::myNewWindow()
{
    TRect r( 0, 0, 26, 7 );             // set initial size and position

    /* SS: micro change here */

    r.move( rand() % 53, rand() % 16 ); // randomly move around screen
    TDemoWindow *window = new TDemoWindow ( r, "Demo Window", ++winNumber);
    deskTop->insert(window);    // put window into desktop and draw it
}


TDemoWindow::TDemoWindow( const TRect& bounds, const char *aTitle,
              short aNumber) :
         TWindow( bounds, aTitle, aNumber),
         TWindowInit( &TDemoWindow::initFrame )
{
    makeInterior(); // creates scrollable interior and inserts into window
}


int main()
{
    readFile( fileToRead );
    TMyApp myApp;
    myApp.run();
    deleteFile();
    return 0;
}
