#define Uses_TApplication
#define Uses_TDeskTop
#define Uses_TMenuItem
#define Uses_TMenu
#define Uses_TMenuBar
#define Uses_TStatusItem
#define Uses_TStatusDef
#define Uses_TStatusLine
#define Uses_TKeys
#define Uses_TRect
#define Uses_TEvent
#define Uses_TButton
#define Uses_TStaticText
#define Uses_TDialog
#define Uses_MsgBox
#include <tv.h>

const unsigned short cmHelpDemoCmd = 100;
const unsigned short cmTestA = 101;
const unsigned short cmTestB = 102;
const unsigned short cmTestC = 103;


const unsigned short hcFirst        = 100;      // First context
const unsigned short hcButtonOne    = 100;
const unsigned short hcButtonTwo    = 101;
const unsigned short hcButtonHelp   = 102;
const unsigned short hcHelpDemo     = 103;
const unsigned short hcExit         = 104;
const unsigned short hcDemoMenu     = 105;
const unsigned short hcLast         = 104;      // Last context

char *helpString[] = {
    "Button OK!",
    "Button Cancel!",
    "Button Help!",
    "Help Demo Window",
    "Exit program",
    "Demo menu"
};

//
// ---------------------> Classes
//
class THintStatusLine : public TStatusLine
{

public:

    THintStatusLine(TRect r, TStatusDef& def) : TStatusLine( r, def ) { }
    virtual const char *hint(ushort aHelpCtx)
        {
        if( aHelpCtx >= hcFirst && aHelpCtx <= hcLast )
            return( helpString[aHelpCtx-100] );
        else
            return( "" );           // Don't return 0 here.
        }
};

class TMyApp : public TApplication
{

public:

    TMyApp() : TProgInit(&TMyApp::initStatusLine,
                         &TMyApp::initMenuBar,
                         &TMyApp::initDeskTop ) { }
    static TMenuBar *initMenuBar( TRect r );
    static TStatusLine *initStatusLine( TRect r );
    virtual void handleEvent( TEvent& event);
};

class THelpDemo : public TDialog
{
public:
    THelpDemo();
    virtual void handleEvent( TEvent& event );
};


//
// Member functions of TMyApp
//
TMenuBar *TMyApp::initMenuBar( TRect bounds )
{
    bounds.b.y = bounds.a.y + 1;

    TMenuBar *mainMenu = new TMenuBar (bounds, new TMenu(
        *new TMenuItem("~D~emos", kbAltD, new TMenu(
            *new TMenuItem("~H~elp Demo", cmHelpDemoCmd, kbAltH, hcHelpDemo) +
             newLine() +
            *new TMenuItem("E~x~it", cmQuit, kbAltX, hcExit, "Alt-X")
            ), hcDemoMenu)
        ));

    return( mainMenu );
}

TStatusLine *TMyApp::initStatusLine( TRect bounds )
{
    bounds.a.y = bounds.b.y - 1;

    THintStatusLine *statusLine = new THintStatusLine (bounds,
        *new TStatusDef(0, hcFirst-1) +
            *new TStatusItem("~Alt-X~ Exit", kbAltX, cmQuit) +
            *new TStatusItem("~Alt-H~ Help Demo", kbAltH, cmHelpDemoCmd) +
        *new TStatusDef(hcFirst, hcButtonHelp) +
            *new TStatusItem("~Alt-F3~ Close", kbAltF3, cmClose) +
            *new TStatusItem("~Alt-K~ OK", kbAltK, cmTestA) +
            *new TStatusItem("~Alt-C~ Cancel", kbAltC, cmTestB) +
            *new TStatusItem("~Alt-H~ Help", kbAltH, cmTestC) +
        *new TStatusDef(hcButtonHelp + 1, 0xFFFF) +
            *new TStatusItem("~Alt-X~ Exit", kbAltX, cmQuit) +
            *new TStatusItem("~Alt-H~ Help Demo", kbAltH, cmHelpDemoCmd)
        );

    return (statusLine);
}

void TMyApp::handleEvent(TEvent& event)
{
    TApplication::handleEvent(event);

    switch(event.what)
    {
        case evCommand:             // handle COMMAND events
        {
            switch(event.message.command)
            {
                case cmHelpDemoCmd:
                {
                    TView *helpBox = new THelpDemo;
                    if( validView(helpBox) )
                        deskTop->insert( helpBox );
                    break;
                }
                default:
                    return;
            }
            clearEvent(event);      // Clear the event we handled
        }
    }
}


THelpDemo::THelpDemo() :
    TWindowInit(&THelpDemo::initFrame),
    TDialog(TRect(0, 0, 30, 12), "Help Demo Box")
{
    TView *control = new TStaticText(TRect(2, 5, 17, 7), "Example of help contexts");
    insert(control);

    TView *defControl = new TButton(TRect(18,2,28,4), "O~K~", cmTestA, bfDefault);
    defControl->helpCtx = hcButtonOne;
    insert(defControl);

    control = new TButton(TRect(18,5,28,7), "~C~ancel", cmTestB, bfNormal);
    control->helpCtx = hcButtonTwo;
    insert(control);

    control = new TButton(TRect(18,8,28,10), "~H~elp", cmTestC, bfNormal);
    control->helpCtx = hcButtonHelp;
    insert(control);

    setCurrent(defControl, normalSelect);
    options |= ofCentered;
}


void THelpDemo::handleEvent(TEvent& event)
{
    TDialog::handleEvent(event);

    if( event.what == evCommand )
    {
        switch( event.message.command )
        {
        case cmTestA:
            messageBox("OK...", mfOKButton);
            break;
        case cmTestB:
            messageBox("Cancel...", mfOKButton);
            break;
        case cmTestC:
            messageBox("Help...", mfOKButton);
            break;
        default:
            return;
        }
        clearEvent( event );
    }
}


//
//---------------   MAIN   ------------------
//
int main()
{
    TMyApp myApp;

    myApp.run();

    return( 0 );
}


