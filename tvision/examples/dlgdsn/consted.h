/***************************************************************************

    consted.h - Prototypes for constant listers and its editors
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
 
#define Uses_TDialog
#define Uses_TStringCollection
#define Uses_TEvent
#define Uses_TButton
#define Uses_TListBox
#define Uses_TRadioButtons
#define Uses_TParamText

#include <tv.h>

class ipstream;
class opstream;


class TConstCollection: public TCollection
{
public:
   TConstCollection(const char * aListName);
   ~TConstCollection();
   void add(const char * Ident, int Value, bool IsReadOnly = false);
   void change(int index, const char * Ident, int Value);
   int getIndex(char * Ident);
   int getIndex(int Value);
   char * getId(int Value);
   int getValue(char * Ident);
   virtual void freeItem(void * item);
   void sort(int mode);
   char * listName;
   int sortmode;
   virtual void * readItem(ipstream&) {return 0;};
   virtual void writeItem(void *, opstream&) { };
};


class TIntConstListBox: public TListBox
{
   TConstCollection * items;
public:
   TIntConstListBox(const TRect& bounds,
       ushort aNumCols, TScrollBar * aScrollBar):
       TListBox(bounds, aNumCols, aScrollBar) { };
   virtual void getText(char *dest, ccIndex item, short maxChars);
   virtual void setData(void * data);
   virtual void getData(void * data);
};

class TConstEdit: public TDialog
{
   TConstCollection * Items;
   TButton * EditBtn;
   TButton * AddBtn;
   TButton * ClearBtn;
   TButton * DelBtn;
   TIntConstListBox * List;
   TRadioButtons * SortRB;
   TConstCollection * cList;
   TParamText * LVal;
   int Select(TConstCollection * AItems, int32 Current);
public:
   TConstEdit();
   void Edit(TConstCollection * AItems);
// int Select(TConstCollection * AItems, int Current);
   int Select(TConstCollection * AItems, char * Current);
   virtual void handleEvent(TEvent& event);
};

TConstCollection * CommandsList();
TConstCollection * HelpCtxsList();


