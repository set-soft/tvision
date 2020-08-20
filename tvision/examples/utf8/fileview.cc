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
#define Uses_TVConfigFile
#define Uses_TDialog
#define Uses_TCheckBoxes
#define Uses_TLabel
#define Uses_TSItem
#define Uses_TRadioButtons
#define Uses_TInputLine
#define Uses_TValidator
#define Uses_TButton
#define Uses_MsgBox
#include <tv.h>

UsingNamespaceStd

const int cmMyFileOpen = 200; // assign new command values
const int cmMyNewWin   = 201;
const int cmNewDialog  = 202;

const char *fileToRead = "test.utf8";
const int maxLineLength = maxViewWidth+1;
const int maxLines      = 100;
uint16 *lines[maxLines];
int lineCount = 0;

struct DialogData
{
    ushort checkBoxData;
    ushort radioButtonData;
    uint16 inputLineData[128];
    char inputLineRangeData[34];
};

DialogData *demoDialogData;

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

static
uint16 *strUTF8_2_U16(uint16 *dest, const char *b)
{
 int retLen=0;
 while (*b)
    dest[retLen++]=charUTF8_2_U16(b);
 dest[retLen]=0;
 retLen++;
 return dest;
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

class TDemoWindow;

class TMyApp : public TApplication
{

public:
    TMyApp();
    ~TMyApp();
    static TStatusLine *initStatusLine( TRect r );
    static TMenuBar *initMenuBar( TRect r );
    virtual void handleEvent( TEvent& event);
    void myNewWindow();
    void newDialog();

    TDemoWindow *window;
};


static short winNumber = 0;             // initialize window number
class TInterior;


class TDemoWindow : public TWindow      // define a new window class
{

public:

   TDemoWindow( const TRect& r, const char *aTitle, short aNumber );
    void makeInterior();
    TInterior *interior;

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
    interior = new TInterior( r, hScrollBar, vScrollBar );
    insert( interior );
}

TMyApp::TMyApp() :
    TProgInit( &TMyApp::initStatusLine,
               &TMyApp::initMenuBar,
               &TMyApp::initDeskTop
             )
{
    demoDialogData = new DialogData;
    demoDialogData->checkBoxData = 1;
    demoDialogData->radioButtonData = 2;
    //strcpy( demoDialogData->inputLineData, "Phone Mum!" );
    strUTF8_2_U16(demoDialogData->inputLineData, "Phone Mum!");
    sprintf( demoDialogData->inputLineRangeData, "%d", 16 );
}

TMyApp::~TMyApp()
{
   delete demoDialogData;
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
            *new TMenuItem( "E~x~it", cmQuit, kbAltX, hcNoContext, "Alt-X" )+
        *new TSubMenu( "~W~indow", kbAltW )+
            *new TMenuItem( "~N~ext", cmNext,     kbF6, hcNoContext, "F6" )+
            *new TMenuItem( "~Z~oom", cmZoom,     kbF5, hcNoContext, "F5" )+
            *new TMenuItem( "~D~ialog", cmNewDialog, kbF2, hcNoContext, "F2" )
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
            case cmNewDialog:
                newDialog();
                break;
            default:
                return;
            }
        clearEvent( event );            // clear event after handling
        }
    else if( event.what == evKeyDown )
        {
         printf("Key down: U+%04X\n",event.keyDown.charCode);
        }
}

void TMyApp::myNewWindow()
{
    TRect r( 0, 0, 26, 7 );             // set initial size and position

    /* SS: micro change here */

    r.move( rand() % 53, rand() % 16 ); // randomly move around screen
    window = new TDemoWindow ( r, "Demo Window", ++winNumber);
    deskTop->insert(window);    // put window into desktop and draw it
}


TDemoWindow::TDemoWindow( const TRect& bounds, const char *aTitle,
              short aNumber) :
         TWindow( bounds, aTitle, aNumber),
         TWindowInit( &TDemoWindow::initFrame )
{
    makeInterior(); // creates scrollable interior and inserts into window
}


// changed from tvguid12: add buttons
void TMyApp::newDialog()
{
    TDialog *pd = new TDialog( TRect( 20, 4, 60, 20), "Demo Dialog" );
    if( pd )
        {
        TView *b = new TCheckBoxes( TRect( 3, 3, 18, 6),
            new TSItem( "~H~varti",
            new TSItem( "~T~ilset",
            new TSItem( "~J~arlsberg", 0 )
            )));
        pd->insert( b );

        pd->insert( new TLabel( TRect( 2, 2, 10, 3), "Cheeses", b ));

        b = new TRadioButtons( TRect( 22, 3, 34, 6),
            new TSItem( "~S~olid",
            new TSItem( "~R~unny",
            new TSItem( "~M~elted", 0 )
            )));
        pd->insert( b );

        pd->insert( new TLabel( TRect( 21, 2, 33, 3), "Consistency", b ));

        // add input line
        b = new TInputLineU16( TRect( 3, 8, 37, 9 ), 128 );
        pd->insert( b );
        pd->insert( new TLabel( TRect( 2, 7, 24, 8 ),
                "Delivery Instructions", b ));

        // add input line with range validation
        TInputLine *inp=new TInputLine( TRect( 3,11, 37,12 ), 32 );
        pd->insert( inp );
        pd->insert( new TLabel( TRect( 2,10, 26,11 ),
                "A value from -20 to 590", inp ));
        TValidator *vld=new TRangeValidator(-20,590);
        inp->SetValidator( vld );

        pd->insert( new TButton( TRect( 15, 13, 25, 15 ), "~O~K", cmOK,
                    bfDefault ));
        pd->insert( new TButton( TRect( 28, 13, 38, 15 ), "~C~ancel", cmCancel,
                    bfNormal ));

        // we save the dialog data:
        pd->setData( demoDialogData );

        ushort control = deskTop->execView( pd );

        // and read it back when the dialog box is successfully closed
        if( control != cmCancel )
            {
            char *end;
            pd->getData( demoDialogData );
            // this is a message box with arguments like printf
            if (lineCount)
              {
               delete[] lines[0];
               lines[0]=new uint16[128];
               memcpy(lines[0],demoDialogData->inputLineData,256);
               window->interior->draw();
              }
            messageBox( mfInformation|mfOKButton, "Deliver: %s value %ld",
                        (char *)demoDialogData->inputLineData,
                        strtol(demoDialogData->inputLineRangeData,&end,0) );
            }
        }
    CLY_destroy( pd );
}

int main()
{
    /* The following code could be used to replace the configuration file and
       hardcore the values. Note that this could be a good idea for Unicode16
       variable but is most probably quite bad for the rest */
    #if 0
    TProgInit::config=new TVMainConfigFile();
    TVMainConfigFile::Add("X11","Unicode16",1);
    TVMainConfigFile::Add("X11","UnicodeFont","boxround.sft");
    TVMainConfigFile::Add("X11","UseX11Fonts",1);
    TVMainConfigFile::Add("X11","FontWidth",12);
    TVMainConfigFile::Add("X11","FontHeight",13);
    TVMainConfigFile::Add("X11","ScreenHeight",40);
    #endif

    readFile( fileToRead );
    TMyApp myApp;
    myApp.run();
    deleteFile();
    return 0;
}
