/*************************************************************************/
/*                                                                       */
/* TCMBOWIN.CPP                                                          */
/*                                                                       */
/* Copyright (c) 1992, Vincent J. Dentice                                */
/* All rights reserved                                                   */
/*                                                                       */
/* The TComboBox class is an extension to Borland International's Turbo  */
/* Vision Applications Framework for DOS.  It provides a class that      */
/* acts like a Combo Box in other graphical environments like Microsoft  */
/* Windows and IBM OS/2.                                                 */
/*                                                                       */
/* It is designed to be with a TDialog class and a TCollection Class.    */
/*                                                                       */
/*                                                                       */
/*   Date    Prg  Ver  Description                                       */
/* --------  ---  ---  ------------------------------------------------- */
/* 09/30/92  VJD  0.1  Initial module definition.                        */
/* 11/16/92  VJD  0.2  Added streamability to the TComboBox classes.     */
/*                                                                       */
/*************************************************************************/


#define Uses_TComboWindow
#define Uses_TScrollBar
#define Uses_TStreamableClass
#define Uses_TPalette
#include "tcombobx.h"

#define cpComboWindow "\x13\x13\x15\x04\x05\x1A\x1C"


TComboWindow::TComboWindow(const TRect& bounds, TCollection *aList) :
			TWindowInit(&TComboWindow::initFrame),
			TWindow(bounds, 0, wnNoNumber)
{
   TRect r;
   TScrollBar  *sb;

   setState(sfShadow, False);
   flags = 0;

   r = getExtent();
   r.a.x = r.b.x - 1;
   sb = new TScrollBar(r);
   insert(sb);

   r = getExtent();
   r.b.x--;
   viewer = new TComboViewer(r, aList, sb);
   insert(viewer);
}


TPalette& TComboWindow::getPalette() const
{
   static TPalette palette (cpComboWindow, sizeof(cpComboWindow)-1);
   return palette;
}


void TComboWindow::getSelection(char *dest)
{
   viewer->getText(dest, viewer->focused, 255);
}


void TComboWindow::handleEvent(TEvent& event)
{
   if ((event.what == evMouseDown) && !containsMouse(event)) {
      endModal(cmCancel);
      clearEvent(event);
   }
   TWindow::handleEvent(event);
}


void TComboWindow::setSelection(const char *data)
{
   viewer->focusItem(viewer->list->indexOf((void *)data));
}


void * TComboWindow::read( ipstream& is )
{
   TWindow::read(is);
   is >> viewer;
   return this;
}

void TComboWindow::write( opstream& os )
{
   TWindow::write(os);
   os << viewer;
}


TStreamable *TComboWindow::build()
{
   return new TComboWindow(streamableInit);
}


TComboWindow::TComboWindow(StreamableInit) : TWindowInit(0),
					     TWindow(streamableInit)
{}
