/*************************************************************************/
/*                                                                       */
/* TSINPUTL.H                                                            */
/*                                                                       */
/* Copyright (c) 1992, Vincent J. Dentice                                */
/* All rights reserved                                                   */
/*                                                                       */
/*                                                                       */
/*   Date    Prg  Ver  Description                                       */
/* --------  ---  ---  ------------------------------------------------- */
/* 09/30/92  VJD  0.1  Initial module definition.                        */
/* 11/16/92  VJD  0.2  Added streamability to TStaticInputLine class.    */
/* 11/17/92  VJD  0.3  Replaced matchFirstChar() function with member    */
/*                     function getNextMatch().                          */
/* 11/17/92  RG   0.3  Added functionality to jump to the beginning and  */
/*                     end of the list by using the HOME and END keys.   */
/*                     This code was supplied by Robert Gloeckner        */
/*                     (100034,3033).                                    */
/*                                                                       */
/*************************************************************************/

#include <ctype.h>
#define Uses_string

#define Uses_TEvent
#define Uses_TKeys
#define Uses_TStaticInputLine
#define Uses_TStreamableClass
#include "tsinputl.h"

Boolean matchFirstChar(void *, void *);


TStaticInputLine::TStaticInputLine(const TRect& bounds, int aMaxLen, TCollection *aList) :
		  TInputLine(bounds, aMaxLen)
{
   list = aList;
}


void *TStaticInputLine::getNextMatch(char testChar)
{
   return (char *)list->firstThat(matchFirstChar, &testChar);
}


void TStaticInputLine::handleEvent(TEvent& event)
{
   char testChar[2];
   char *tempData;
   ccIndex index;

   if (event.what == evKeyDown) {
      if (isprint(event.keyDown.charScan.charCode)) {
	 testChar[0] = event.keyDown.charScan.charCode;
	 testChar[1] = '\0';
	 tempData = (char *)getNextMatch(testChar[0]);
	 if (tempData != 0) {
	    strcpy(data, tempData);
	    selectAll(True);
	    drawView();
	 }
	 clearEvent(event);
      }
      else
	 switch (event.keyDown.keyCode) {
	    case kbUp :
	       index = list->indexOf(data) - 1;
	       if (index < 0)
		  index = list->getCount() - 1;
	       strcpy(data, (char *)list->at(index));
	       selectAll(True);
	       drawView();
	       clearEvent(event);
	       break;

	    case kbDown :
	       index = list->indexOf(data) + 1;
	       if (index >= list->getCount())
		  index = 0;
	       strcpy(data, (char *)list->at(index));
	       selectAll(True);
	       drawView();
	       clearEvent(event);
	       break;

	    case kbHome:
	       index = 0;
	       strcpy(data, (char *)list->at(index));
	       selectAll(True);
	       drawView();
	       clearEvent(event);
	       break;

	    case kbEnd:
	       index = list->getCount() - 1;
	       strcpy(data, (char *)list->at(index));
	       selectAll(True);
	       drawView();
	       clearEvent(event);
	       break;

	    case kbLeft  :
	    case kbRight :
	    case kbBack  :
	    case kbIns   :
	    case kbDel   : clearEvent(event);
	 }
   }
   TInputLine::handleEvent(event);
}


void TStaticInputLine::newList(TCollection *aList)
{
   if (list)
      CLY_destroy(list);
   list = aList;
   drawView();
}


void *TStaticInputLine::read( ipstream& is )
{
   TInputLine::read(is);
   is >> list;
   return this;
}


void TStaticInputLine::write( opstream& os )
{
   TInputLine::write(os);
   os << list;
}

TStreamable *TStaticInputLine::build()
{
   return new TStaticInputLine(streamableInit);
}


TStaticInputLine::TStaticInputLine(StreamableInit) : TInputLine(streamableInit) { }



Boolean matchFirstChar(void *string1, void *string2)
{
   char *temp1, *temp2;

   temp1 = (char *)string1;
   temp2 = (char *)string2;

   if (toupper(temp1[0]) == toupper(temp2[0]))
      return True;
   else
      return False;
}
