/*************************************************************************/
/*                                                                       */
/* TCMBOVWR.CPP                                                          */
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
/*                     Rewrote header files to behave like original      */
/*                     Turbo Vision header files.                        */
/* 11/17/92  VJD  0.3  Added the missing dataSize function to            */
/*                     TComboViewer.                                     */
/*                                                                       */
/*************************************************************************/

#define Uses_string

#define Uses_TComboViewer
#define Uses_TKeys
#define Uses_TStreamableClass
#define Uses_TPalette
#include "tcombobx.h"

#define cpComboViewer "\x06\x06\x07\x06\x06"

struct TComboViewerRec
{
    TCollection *items;
    ushort selection;
};


TComboViewer::TComboViewer(const TRect& bounds, TCollection *aList, TScrollBar *ts) :
	      TListViewer(bounds, 1, 0, ts)
{
   list = 0;
   newList(aList);
}


unsigned TComboViewer::dataSize()
{
   return sizeof(TComboViewerRec);
}


TPalette& TComboViewer::getPalette() const
{
   static TPalette palette(cpComboViewer, sizeof(cpComboViewer)-1);
   return palette;
}


void TComboViewer::getData( void * rec )
{
    TComboViewerRec *p = (TComboViewerRec *)rec;

    p->items = list;
    p->selection = focused;
}


void TComboViewer::getText(char *dest, ccIndex item, short maxLen)
{
   if (list != 0 ) {
      strncpy( dest, (const char *)(list->at(item)), maxLen );
      dest[maxLen] = '\0';
   }
   else
      *dest = EOS;
}


void TComboViewer::handleEvent(TEvent& event)
{
   if ((event.what == evMouseDown && event.mouse.doubleClick) ||
       (event.what == evKeyDown && event.keyDown.keyCode == kbEnter))
   {
      endModal(cmOK);
      clearEvent(event);
   }
   else	if ((event.what ==  evKeyDown && event.keyDown.keyCode == kbEsc) ||
	    (event.what ==  evCommand && event.message.command ==  cmCancel))
   {
      endModal(cmCancel);
      clearEvent(event);
   }
   else
      TListViewer::handleEvent(event);
}


void TComboViewer::newList( TCollection *aList )
{
   if (list)
      CLY_destroy(list);
   list = aList;
   if(aList != 0)
      setRange(aList->getCount());
   else
      setRange(0);
   if(range > 0)
      focusItem(0);
   drawView();
}


void TComboViewer::setData( void *rec )
{
    TComboViewerRec *p = (TComboViewerRec *)rec;
    newList(p->items);
    focusItem(p->selection);
    drawView();
}


void TComboViewer::shutDown()
{
   list = 0;
   TListViewer::shutDown();
}


void * TComboViewer::read( ipstream& is )
{
   TListViewer::read(is);
   is >> list;
   return this;
}

void TComboViewer::write( opstream& os )
{
   TListViewer::write(os);
   os << list;
}


TStreamable *TComboViewer::build()
{
   return new TComboViewer(streamableInit);
}


TComboViewer::TComboViewer(StreamableInit) : TListViewer(streamableInit) { }
