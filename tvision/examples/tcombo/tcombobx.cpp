/*************************************************************************/
/*                                                                       */
/* TCOMBOBX.CPP                                                          */
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
/* 11/17/92  VJD  0.3  Added functions dataSize, getData and setData.    */
/* 11/18/92  RG   0.3  Added code to select the correct item when the    */
/*                     TComboWindow comes up.  These code modifications  */
/*                     where supplied by Robert Gloeckner (100034,3033). */
/*                                                                       */
/*************************************************************************/

#define Uses_string

#define Uses_TComboBox
#define Uses_TComboWindow
#define Uses_TKeys
#define Uses_TStreamableClass
#define Uses_TPalette
#include "tcombobx.h"

#define cpComboBox "\x16"

char* TComboBox::icon = "\x19";


TComboBox::TComboBox(const TRect& bounds, TInputLine *aLink, TCollection *aList) :
		TView(bounds)
{
   options |= ofPostProcess;
   eventMask |= evBroadcast;
   link = aLink;
   list = aList;
}


void TComboBox::shutDown()
{
   link = 0;
   TView::shutDown();
}


unsigned TComboBox::dataSize()
{
   return sizeof(void *);
}


void TComboBox::draw()
{
   TDrawBuffer b;

   b.moveStr(0, icon, getColor(0x01));
   writeLine(0, 0, size.x, size.y, b);
}


void TComboBox::getData(void *rec)
{
//   TCollection **p = (TCollection **)rec;
//   *p = list;
   *(TCollection **)rec = list;
}


TPalette& TComboBox::getPalette() const
{
   static TPalette palette(cpComboBox, sizeof(cpComboBox)-1);
   return palette;
}


void TComboBox::handleEvent(TEvent& event)
{
   TComboWindow *ComboWindow;
   TRect  r, p;
   ushort c;

   TView::handleEvent(event);
   if ((event.what == evMouseDown) ||
       (event.what == evKeyDown && ctrlToArrow(event.keyDown.keyCode) == kbDown
				&& (link->state & sfFocused) != 0))
   {
      if (strlen((const char *)link->getData()))               // If length of link->data > 0,
	 list->insert(newStr((const char *)link->getData()));  // add new data to list
      link->select();                       // Make InputLine the active view
      r = link->getBounds();                // Get bounds of the InputLine
      r.b.x += 1;                           // Extend x bound by 1
      r.a.y += 1;                           // Move bound down by 1
      r.b.y += 7;                           // Extend y bound by 7
      p = owner->getExtent();               // Get extent of the Dialog Box
      r.intersect(p);                       // Get intersection of Dialog and r
      r.b.y -= 1;
      ComboWindow = new TComboWindow(r, list); // Create a new TComboWindow
      if (ComboWindow != 0) {
	 if (strlen((const char *)link->getData()))
	    ComboWindow->setSelection((const char *)link->getData());

	 c = owner->execView(ComboWindow);           // Execute TComboWindow as modal view

	 if (c == cmOK) {                            // If TComboWindow return cmOK
	    char rslt[256];

	    ComboWindow->getSelection(rslt);         // Set the link data to the selection
	    link->setDataFromStr(rslt);
	    link->selectAll(True);                   // Select all in the linked view
	    link->drawView();                        // Redraw the linked view
	 }
	 CLY_destroy(ComboWindow);
      }
      clearEvent(event);
   }
   else if (event.what == evBroadcast)
      if ((event.message.command == cmReleasedFocus && event.message.infoPtr == link)
       || (event.message.command == cmRecordHistory))
	 if (strlen((const char *)link->getData()))               // If length of link->data > 0,
	    list->insert(newStr((const char *)link->getData()));  // add new data to list
}


void TComboBox::newList(TCollection *aList)
{
   if (list)
      CLY_destroy(list);

   list = aList;
}


void TComboBox::setData(void *rec)
{
    TCollection *p = (TCollection *)rec;

    newList(p);
}


void TComboBox::write(opstream& os)
{
   TView::write(os);
   os << link << list;
}


void *TComboBox::read(ipstream& is)
{
   TView::read(is);
   is >> link >> list;

   return this;
}


TStreamable *TComboBox::build()
{
   return new TComboBox(streamableInit);
}


TComboBox::TComboBox(StreamableInit) : TView(streamableInit) { }
