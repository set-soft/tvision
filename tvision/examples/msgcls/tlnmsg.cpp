/**********************************************************************
 * File Name:   TLNMSG.CPP                                            *
 * Date:        01/29/92                                              *
 *                                                                    *
 * Description: Implements a scrollable message box to post messages  *
 * to the user.  Call "postMsg(const char *)" with the text you want  *
 * inserted into the message window.  If the message window doesn't   *
 * exist, it is created.  The message is then posted to the end       *
 * of the list.                                                       *
 * Implements an information box to give the user current status      *
 * (similar to Borlands "Compiling" and "Linking" information boxes.  *
 * Call "postInfo(int, const char *)" with the line number and the    *
 * information string.  If the information box doesn't exist, it is   *
 * created.  Pass a negative line number to close the box.  The last  *
 * line is reserved for a user prompt, eg, "Press Escape to Continue" *
 * and is displayed as Cyan on a Blue background.  NOTE: This view    *
 * is NOT MODAL therefore it doesn't call getEvent!  The code that is *
 * posting messages needs to handle events such as Ctrl-Break or the  *
 * user pressing escape.                                              *
 *                                                                    *
 * Revisions:                                                         *
 *                                                                    *
 **********************************************************************/

#define Uses_string

#include "tlnmsg.h"
//#include "const.h"     // for command constants

// ********************************************************************

TMsgListViewer::TMsgListViewer( const TRect& bounds, ushort aNumCols,
                  TScrollBar * aHScrollBar, TScrollBar * aVScrollBar ) :
  TListViewer ( bounds, aNumCols, aHScrollBar, aVScrollBar )
{
  // expand and shrink to the right and bottom

  dragMode = dmDragGrow;
  growMode = gfGrowHiY | gfGrowHiX;

  items = new TNSCollection(6, 1);
  setRange(0);
}

void TMsgListViewer::getText( char * dest, ccIndex item, short maxLen )
{
  * dest = EOS;

  if (items != 0) {
    strncpy(dest, (const char *) (items -> at(item)), maxLen);
    dest[maxLen] = 0;
  }
}

void TMsgListViewer:: insert(const char * msg)
{
  items -> insert( newStr( (const char *) msg) );
  setRange(items -> getCount());
  drawView();
}

// map into the 1 - 5 into 9 - 13 of the owner (TMsgWindow)

#define cpMsgList "\x09\x0A\x0B\x0C\x0D"

TPalette& TMsgListViewer::getPalette() const
{
  static TPalette palette( cpMsgList, sizeof(cpMsgList) - 1);
  return palette;
}

// ********************************************************************

TlnMsgWindow::TlnMsgWindow(const TRect& bounds) :
  TWindowInit( &TlnMsgWindow::initFrame),
  TWindow(bounds, "Message", wnNoNumber)
{
  TScrollBar * vBar;
  TScrollBar * hBar;

  // Initialize the window
  dragMode = dmLimitAll;

  palette = wpCyanWindow;


  vBar = standardScrollBar( sbVertical | sbHandleKeyboard );
  hBar = standardScrollBar( sbHorizontal | sbHandleKeyboard );

  TRect r = getExtent();
  r.a.x = 1; r.a.y = 1; r.b.x -= 1; r.b.y -= 1;
  msgViewer = new TMsgListViewer(r, 1, hBar, vBar);
  insert(msgViewer);
}

TlnMsgWindow::~TlnMsgWindow()
{
}

void TlnMsgWindow::handleEvent(TEvent& event)
{
  TWindow::handleEvent(event);

  if (event.what == evBroadcast)
    switch(event.message.command) {
      case cmFindMsgBox :               // return a pointer to self
        clearEvent(event);
        break;
      case cmInsMsgBox :               // insert a message into box
        msgViewer -> insert((const char *) event.message.infoPtr);
        clearEvent(event);
        drawView();
        break;
    }
}

// This will map colors 9 - 13 into the ListViewer Palette

#define cpMsgWindow "\x10\x11\x12\x13\x14\x15\x16\x17\x39\x3A\x3B\x3C\x3D"

TPalette& TlnMsgWindow::getPalette() const
{
  static TPalette palette( cpMsgWindow, sizeof(cpMsgWindow) - 1);
  return palette;
}

// ********************************************************************

void postMsg(const char * msg)
{
   TlnMsgWindow * wPtr;
   TRect r;

   wPtr = (TlnMsgWindow *)
     message(TProgram::deskTop, evBroadcast, cmFindMsgBox, 0);

   if (wPtr == 0) {
      // Create the message window
      r = TProgram::deskTop -> getExtent();
      r.a.y = r.b.y - 6;
      TProgram::deskTop -> insert(wPtr = new TlnMsgWindow(r));
   }

   message(wPtr, evBroadcast, cmInsMsgBox, (void *) msg);
}

// ********************************************************************

TStaticPrompt::TStaticPrompt(TRect& bounds, const char * text) :
  TStaticText(bounds, text)
{
}

// map the palette into the 9th entry of TlnInfoWindow
#define cpStaticPrompt "\x09"

TPalette& TStaticPrompt::getPalette() const
{
  static TPalette palette( cpStaticPrompt, sizeof(cpStaticPrompt) - 1);
  return palette;
}


// ********************************************************************

TlnInfoWindow::TlnInfoWindow( const TRect& bounds,
                              const char * aTitle,
                              short aNumber) :
  TWindowInit(&TlnInfoWindow::initFrame),
  TWindow (bounds, aTitle, aNumber)
{
  unsigned i;

  // don't allow move, grow, close or zoom
  flags = 0;

  // center it in the group
  options |= ofCentered;

  // Buffer the screen writes to prevent flicker
  // Buffer will be automatically deleted if low on memory
  options |= ofBuffered;

  count = 0;
  for (i = 0; i < sizeof(items) / sizeof(TStaticText *); ++i)
    items[i] = NULL;

  // turn off the mouse event queue
  TEventQueue::suspend();

}

TlnInfoWindow::~TlnInfoWindow()
{
  // Turn the mouse event queue back on
  TEventQueue::resume();
}

void TlnInfoWindow::handleEvent(TEvent& event)
{
  // all events get cleared by this function to allow
  // "modal" operation without stealing the processor.

  int i, lastLine;

  if (event.what == evBroadcast)

    switch (event.message.command) {

      case cmFindInfoBox :
        clearEvent(event);
        break;

      case cmInsInfoBox :
        InfoData * p = (InfoData *) event.message.infoPtr;
        TRect r = getExtent();
        lastLine = r.b.y - 2;
        i = p -> line;
        if (i > r.b.y - r.a.y - 1)
          break;

        // lock all screen writes till buffer updated

        lock();
        if (items[i] != NULL) {
          remove(items[i]);
          CLY_destroy(items[i]);
        }


        r.a.y = i; r.b.y = i+1;

        if  (i == lastLine) {
          r.a.x += 1; r.b.x -= 1;
          items[i] = new TStaticPrompt( r, p -> text );
        } else {
          r.a.x += 2; r.b.x -= 1;
          items[i] = new TStaticText ( r, p -> text);
        }

        insert(items[i]);

        // unlock screen writes and redraw the buffer
        unlock();

        clearEvent(event);
        break;
    } // endswitch

}

// add 0x14 to the end of the normal gray window palette.  This
// maps into "Scrollbar Reserved", Background = 1 (Blue), Foreground
// = 3 (Cyan)
//                       1   2   3   4   5   6   7   8   9
#define cpInfoWindow "\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F\x14"

TPalette& TlnInfoWindow::getPalette() const
{
  static TPalette palette( cpInfoWindow, sizeof(cpInfoWindow) - 1);
  return palette;
}


// ********************************************************************

void postInfo(int line, const char * text)
{
   TlnInfoWindow * wPtr;
   InfoData data;

   wPtr = (TlnInfoWindow *)
     message(TProgram::deskTop, evBroadcast, cmFindInfoBox, 0);

   if ((line < 0) && (wPtr != 0)){
     TProgram::deskTop -> CLY_destroy(wPtr);
     return;
   }

   if (wPtr == 0) {
      // Create the information window
      TProgram::deskTop ->
        insert(wPtr = new TlnInfoWindow( TRect(0, 0, 40, 12),
                                         "Information", wnNoNumber));
   }

   data.line = line;
   data.text = text;
   message(wPtr, evBroadcast, cmInsInfoBox, (void *) &data);
}

