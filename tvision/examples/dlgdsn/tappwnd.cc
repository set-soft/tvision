/***************************************************************************

    tappwnd.cc - Application window
    ---------------------------------------------------------------------
    May, 2000
    Copyright (C) 2000 by Warlei Alves
    walves@usa.net
    
    Modified by Salvador E. Tropea to compile without warnings.
    
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
#include "dsgobjs.h"

#define cpAppWnd\
     "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f" \
     "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f" \
     "\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f" \
     "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f" \

TAppWindow::TAppWindow(): 
   TWindowInit(&TAppWindow::initFrame),
   TWindow( TRect(0, 0, 60, 20), "", 0 ),
   menuBar( 0 ),
   statusLine( 0 )
{
   options |= ofCentered;
   TRect r = getExtent();
   r.grow(0, -1);
   deskTop = new TDeskTop( r );
   deskTop->growMode = gfGrowHiX | gfGrowHiY;
   insert(deskTop);
}

TAppWindow::~TAppWindow()
{
   removeMenuBar();
   removeStatusLine();
}

TFrame * TAppWindow::initFrame(TRect r)
{
   return 0;
}

TPalette& TAppWindow::getPalette() const
{
   static TPalette p( cpAppWnd, sizeof(cpAppWnd) - 1 );
   return p;
}

void TAppWindow::changeBounds(const TRect& bounds)
{
   TWindow::changeBounds(bounds);
   TRect r = getExtent();
   r.a.y = r.b.y - 1;
   statusLine->changeBounds(r);
   deskTop->redraw();
}

void TAppWindow::removeStatusLine()
{
   if (statusLine != 0)
   {
      remove(statusLine);
      delete statusLine;
      statusLine = 0;
   }
}

void TAppWindow::removeMenuBar()
{
   if (menuBar != 0)
   {
      remove(menuBar);
      delete menuBar;
      menuBar = 0;
   }
}

void TAppWindow::setMenuBar(TEditCollection * aItems)
{
   removeMenuBar();
   if (aItems == 0) return;
}

void TAppWindow::setStatusLine(TEditCollection * aItems)
{
   removeStatusLine();
   if (aItems == 0) return;
}

void TAppWindow::setMenuBar(TMenuBar * aMenuBar)
{
   removeMenuBar();
   if (aMenuBar == 0) return;
   menuBar = aMenuBar;
   menuBar->growMode = gfGrowHiX;
   insert(menuBar);
}

void TAppWindow::setStatusLine(TStatusLine * aStatusLine)
{
   removeStatusLine();
   if (aStatusLine == 0) return;
   statusLine = aStatusLine;
   statusLine->growMode = gfGrowRel;
   insert(statusLine);
}

int lmessageBox(const char * msg, TGroup * aOwner)
{
   TDialog * d = new TDialog( TRect(0, 0, 40, 9), __("Confirm") );
   d->insert( new TStaticText( TRect(3, 2, d->size.x - 2, d->size.y - 3), msg) );
   d->options |= ofCentered;
   TRect r = TRect(0, 0, 10, 2);
   r.move((d->size.x / 2) - 11, d->size.y - 3);
   d->insert( new TButton( r, __("~Y~es"), cmYes, 0) );
   r.a.x += 11; r.b.x+= 11;
   d->insert( new TButton( r, __("~C~ancel"), cmCancel, 0) );
   d->selectNext(false);
   int rst = aOwner->execView(d);
   TObject::CLY_destroy( d );
   return rst;
}

void TAppWindow::handleEvent(TEvent& event)
{
   TWindow * w;
   
   TWindow::handleEvent(event);
   
   if (event.what == evCommand)
   {
      switch (event.message.command)
      {
         case cmQuit:
           if ( lmessageBox( __("Are you sure?"), deskTop ) == cmYes )
              endModal(cmQuit);
         break;
         case cmViewAppWindow:
            w = new TWindow( TRect( 0, 0, 30, 10), "Teste", 0 );
            w->options |= ofCentered;
            deskTop->insert(w);
            break;
      }
      clearEvent(event);
   }
}

