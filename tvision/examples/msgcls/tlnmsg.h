#if !defined __TLNMSG_H__
#define __TLNMSG_H__

#define Uses_TWindow
#define Uses_TNSCollection
#define Uses_TScrollBar
#define Uses_TListViewer
#define Uses_TPalette
#define Uses_TEvent
#define Uses_TProgram
#define Uses_TDeskTop
#define Uses_TEventQueue
#define Uses_TStaticText

#include <tv.h>

//#include "const.h"
const int cmFindMsgBox =0x1000;
const int cmInsMsgBox  =0x1001;
const int cmFindInfoBox=0x1002;
const int cmInsInfoBox =0x1003;


extern void postMsg(const char *);
extern void postInfo(int, const char *);

//
// TMsgListViewer: Very similar to listbox, except it has a TNSCollection
// (nonstreamable) and works with two scroll bars which are attached
// to the frame of it's owner window.
//

class TMsgListViewer : public TListViewer {

  private:
    // list of items to display
    TNSCollection * items;

  public:
    TMsgListViewer( const TRect& bounds, ushort aNumCols,
                    TScrollBar * aHScrollBar, TScrollBar * aVScrollBar);

    void getText( char * dest, ccIndex item, short maxLen );

    virtual TPalette& getPalette() const;

    void insert( const char * msg) ;

    TNSCollection * list() { return items; } ;

} ;

//
// TlnMsgWindow: Cyan window to hold the TMsgListViewer.
//

class TlnMsgWindow : public TWindow
{
  // private members
  TMsgListViewer * msgViewer;

  public :
    TlnMsgWindow(const TRect& bounds);
    ~TlnMsgWindow();
    virtual void handleEvent(TEvent& event);
    virtual TPalette& getPalette() const;
} ;

//
// TlnInfoWindow: displays information in a gray window like a dialog
// box with the exception that it isn't modal.  The application posts
// information to the window and is responsible for closing the window
// when done.
//

class TStaticPrompt : public TStaticText
{
  public:
    TStaticPrompt(TRect&, const char *);   // constructor
    virtual TPalette& getPalette() const; // Text blue on cyan
} ;

class TlnInfoWindow : public TWindow
{

  //
  // Create an array of pointers to static text items.. make
  // this larger or smaller as required.  Current setting assumes
  // maximum box length of 25 lines.  The last line is reserved for
  // special prompts to the user, e.g. "Press Escape to Continue" or
  // "Ctrl-Break to Quit".  A control break handler is not supplied.
  //

  private:
    int count;
    TStaticText * items[25];

  public:
    TlnInfoWindow(const TRect&, const char*, short);
    ~TlnInfoWindow();

    // Once virtual, always virtual!!
    virtual void handleEvent(TEvent&);
    virtual TPalette& getPalette() const;
};

typedef struct {
  int line;
  const char * text;
} InfoData;


#endif
