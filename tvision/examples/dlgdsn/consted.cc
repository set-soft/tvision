/***************************************************************************

    consted.cc - Implements the constant listers and its editors
    ---------------------------------------------------------------------
    May, 2000
    Copyright (C) 2000 by Warlei Alves
    walves@usa.net
    
    Heavily modified by Salvador E. Tropea to compile without warnings.
    Some warnings were in fact bugs.
    For gcc 2.95.x and then 3.0.1.
    
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
#define DEFAULT  0
#define BY_ID    1
#define BY_VALUE 2

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define Uses_TDialog
#define Uses_TButton
#define Uses_TRadioButtons
#define Uses_TSItem
#define Uses_TLabel
#define Uses_TListBox
#define Uses_TScrollBar
#define Uses_TCollection
#define Uses_TEvent
#define Uses_TCommands
#define Uses_TFilterValidator
#define Uses_TInputLine
#define Uses_TProgram
#define Uses_TDeskTop
#define Uses_MsgBox
#define Uses_TParamText

#include <tv.h>

#include "consted.h"

const ushort
  cmEdit   = 200,
  cmAdd    = 201,
  cmDel    = 202,
  cmCClear = 203;

extern int execDialog(TDialog *, void *);

static TConstCollection * CommandList;
static TConstCollection * HelpCtxList;

typedef struct TConstData
{
   char * Id;
   int Value;
   int Order;
   bool ReadOnly;
};

TDialog * ConstInputDlg(TValidator * aValid)
{
   TDialog * d = new TDialog( TRect(0, 0, 50, 8), __("Values editor") );
   TInputLine * ip_id = new TInputLine(TRect (10, 2, 48, 3), 50);
   TInputLine * ip_vl = new TInputLine(TRect (10, 3, 17, 4), 5);
   if (aValid) ip_vl->setValidator(aValid);
   d->insert(new TLabel( TRect(1, 2, 10, 3), __("~I~dent:"), ip_id ) );
   d->insert(ip_id);
   d->insert(new TLabel( TRect(1, 3, 10, 4), __("~V~alue:"), ip_id ) );
   d->insert(ip_vl);
   d->insert(new TButton( TRect(28, 5, 38, 7), __("~O~k"), cmOK, bfDefault) );
   d->insert(new TButton( TRect(38, 5, 48, 7), __("~C~ancel"), cmCancel, 0) );
   d->options |= ofCentered;
   d->selectNext(False);
   return d;
}


void TIntConstListBox::getText(char *dest, ccIndex item, short maxChars)
{
  char * c = ( (TConstData *)(items->at(item)) )->Id;
  strncpy(dest, c, maxChars);
  dest[maxChars] = 0;
}


void TIntConstListBox::setData(void * data)
{
   TListBoxRec * p = (TListBoxRec *)data;
   items = (TConstCollection *)p->items;
   if (items != 0) setRange( items->getCount() );
     else setRange( 0 );
   if (p->selection < 0) p->selection = 0;
   if (range > 0) focusItem(p->selection);
   drawView();
}

void TIntConstListBox::getData(void * data)
{
   TListBoxRec * p = (TListBoxRec *)data;
   TConstCollection * i = (TConstCollection *)items;
   p->items = items;
   p->selection = ((TConstData *)(i->at(focused)))->Value;
}

TConstCollection::TConstCollection(const char * aListName):
                 TCollection(0, 1)
{
   listName = newStr(aListName);
   sortmode = -1;
}

TConstCollection::~TConstCollection()
{
   delete[] listName;
}

void TConstCollection::change(int index, const char * Ident, int Value)
{
  if (count > 0 && index < count && !((TConstData *)at(index))->ReadOnly )
  {
     delete[] ((TConstData *)at(index))->Id;
     ((TConstData *)at(index))->Id = newStr(Ident);
     ((TConstData *)at(index))->Value = Value;
  }
}

void TConstCollection::add(const char * Ident, int Value, bool IsReadOnly)
{
   TConstData * item = (TConstData *)calloc(1, sizeof(TConstData));
   item->Id = newStr(Ident);
   item->Value = Value;
   item->Order = count;
   item->ReadOnly = IsReadOnly;
   sort(DEFAULT);
   insert(item);
}


static Boolean identMatch(void * item, void * ident)
{
   return (strcmp(((TConstData *)item)->Id, (char *)ident) == 0);
}

static Boolean valueMatch(void * item, void *value)
{
   return (((TConstData *)item)->Value == (int)value);
}

int TConstCollection::getIndex(char * Ident)
{
   void * i = firstThat(&identMatch, (void *)Ident);
   if (i) return indexOf(i); else return -1;
}

int TConstCollection::getIndex(int Value)
{
   void * i = firstThat(&valueMatch, (void *)&Value);
   if (i) return indexOf(i); else return -1;
}

char * TConstCollection::getId(int Value)
{
   if (getIndex(Value)!=-1) return ((TConstData*)at(getIndex(Value)))->Id;
     else return 0;
}

int TConstCollection::getValue(char * Ident)
{
   if (getIndex(Ident)!=-1) return ((TConstData*)at(getIndex(Ident)))->Value;
     else return 0;
}

void TConstCollection::freeItem(void * item)
{
   delete[] ((TConstData *)item)->Id;
   ((TConstData *)item)->Id = 0;
   free(item);
}

static int compareOrder(const void * key1, const void * key2)
{
   TConstData * v1 = (TConstData *)key1;
   TConstData * v2 = (TConstData *)key2;
   return (v1->Order > v2->Order) - (v1->Order < v2->Order);
}

static int compareId(const void * key1, const void * key2)
{
   TConstData * v1 = (TConstData *)key1;
   TConstData * v2 = (TConstData *)key2;
   return strcmp(v1->Id, v2->Id);
}

static int compareValue(const void * key1, const void * key2)
{
   TConstData * v1 = (TConstData *)key1;
   TConstData * v2 = (TConstData *)key2;
   return (v1->Value > v2->Value) - (v1->Value < v2->Value);
}

void TConstCollection::sort(int mode)
{
   if (mode == sortmode) return;
   sortmode = mode;
   switch (mode)
   {
      case DEFAULT : qsort(items, count, sizeof(void *), compareOrder); break;
      case BY_ID   : qsort(items, count, sizeof(void *), compareId); break;
      case BY_VALUE: qsort(items, count, sizeof(void *), compareValue);
   }
}


TConstEdit::TConstEdit():
            TWindowInit(&TConstEdit::initFrame),
            TDialog( TRect(0, 0, 45, 16) , __("Constants") )
{
   TScrollBar * sb;
   
   options |= ofCentered;
   
   sb = new TScrollBar(TRect(30, 2, 31, 13));
   List = new TIntConstListBox(TRect(2, 2, 30, 13), 1, sb);
   LVal = new TParamText( TRect(33, 12, 43, 14) );
   LVal->options |= ofFramed;
   EditBtn = new TButton(TRect(31, 2, 43, 4), __("~E~dit"), cmEdit, bfBroadcast );
   AddBtn = new TButton(TRect(31, 4, 43, 6), __("~A~dd"), cmAdd,  bfBroadcast );
   ClearBtn = new TButton(TRect(31, 6, 43, 8), __("C~l~ear"), cmCClear, bfBroadcast );
   DelBtn = new TButton(TRect(31, 8, 43, 10), __("~D~elete"), cmDel, bfBroadcast );
   SortRB = new TRadioButtons(TRect(2, 14, 31, 15), new TSItem(__("None"),
                                                 new TSItem(__("Id"),
                                                 new TSItem(__("Value"), 0))));
   insert(sb);
   insert(new TLabel( TRect(1, 1, 15, 2), __("~V~alue list:"), List ) );
   insert(LVal);
   insert(List);
   insert(new TLabel( TRect(1, 13, 12, 14), __("~O~rder by:"), SortRB ) );
   insert(SortRB);
   insert(EditBtn);
   insert(AddBtn);
   insert(ClearBtn);
   insert(DelBtn);
   LVal->setText("Value:\n%s", "<empty>");
   cList = 0;
   EditBtn->makeDefault(True);
   selectNext(False);
}


void TConstEdit::Edit(TConstCollection * AItems)
{
   TListBoxRec rec;
   
   if (!AItems) return;
   delete[] title;

   rec.items = AItems;
   rec.selection = 0;
   
   title = newStr(AItems->listName);
   AItems->sort(DEFAULT);
   List->setData(&rec);
   List->focused = 0;
   
   AddBtn->setState(sfDisabled, false);
   ClearBtn->setState(sfDisabled, false);
   DelBtn->setState(sfDisabled, false);
   cList = AItems;
   LVal->setText("Value:\n%s", "<empty>");
   TProgram::deskTop->insert(this);
}


void TConstEdit::handleEvent(TEvent& event)
{
   ushort sort;
   int tmp=0;
   TDialog * dlg;
   
   struct {
     char id_value[50];
     char value[5];
   } datarec;

   TDialog::handleEvent(event);
   
   if (!cList) return;
   
   if (event.what == evBroadcast)
   switch(event.message.command)
   {
      case cmListItemFocused:
         if ( cList )
         {
            tmp = ((TConstData *)(cList->at(List->focused)))->Value;
            LVal->setText("Value:\n%i (%x)", tmp, tmp);
         }
         break;
      case cmListItemSelected:
         sort = cList->sortmode;
         cList->sort(BY_VALUE);
         if ( (cList->getCount() > 0) && (List->focused >= 0) )
         {
            tmp = ((TConstData *)(cList->at(List->focused)))->Value;
            sprintf(datarec.value, "%i", tmp);
            strcpy(datarec.id_value,
             ((TConstData *)(cList->at(List->focused)))->Id );
         }
         dlg = ConstInputDlg(new TFilterValidator("0123456789-"));
         if ( execDialog(dlg, &datarec) == cmOK )
            cList->change( List->focused, datarec.id_value,
                       atoi(datarec.value) );  else cList->sort(sort);
         List->drawView();
         clearEvent(event);
         break;
      case cmClusterMovedTo:
      case cmClusterPress:
         ((TRadioButtons *)event.message.infoPtr)->getData(&sort);
         cList->sort(sort);
         List->drawView();
         clearEvent(event);
         break;
      case cmEdit:
         if (cList->getCount() == 0)
         {
            event.message.command = cmAdd;
            putEvent(event);
            clearEvent(event);
         }
         else
         {
            AddBtn->makeDefault(False);
            EditBtn->makeDefault(True);
            event.message.command = cmListItemSelected;
            putEvent(event);
            clearEvent(event);
         }
         break;
      case cmAdd:
         sort = cList->sortmode;
         cList->sort(BY_VALUE);
         memset(&datarec, 0, sizeof(datarec));
         if ((cList->getCount() > 0) && (List->focused >= 0))
         {
            tmp = ( (TConstData *)(cList->at(cList->getCount()-1)) )->Value;
            tmp++;
            sprintf(datarec.value, "%i", tmp);
         }
         dlg = ConstInputDlg(new TFilterValidator("0123456789-"));
         if ( execDialog(dlg, &datarec) == cmOK )
         {
            cList->add( datarec.id_value, atoi(datarec.value) );
            List->setRange(List->range + 1);
            EditBtn->makeDefault(False);
            AddBtn->makeDefault(True);
            List->focusItem( cList->getCount() - 1 );
         }
         else cList->sort(sort);
         List->drawView();
         clearEvent(event);
         break;
      case cmDel:
         if ( messageBox(__("Are you sure ?"), mfYesNoCancel) == cmYes )
         {
            cList->atFree(List->focused);
            clearEvent(event);
            cList->pack();
            List->setRange(List->range - 1);
            List->drawView();
         }
         break;
      case cmCClear:
         if ( messageBox(__("Are you sure ?"), mfYesNoCancel) == cmYes )
         {
            cList->freeAll();
            cList->pack();
            List->setRange(0);
            List->drawView();
         }
         clearEvent(event);
         break;
   }
}


TConstCollection * InitCommandLister()
{
   TConstCollection * rst = new TConstCollection(_("Commands"));
   rst->add("cmValid", 0, true);
   rst->add("cmQuit", 1, true);
   rst->add("cmError", 2, true);
   rst->add("cmMenu", 3, true);
   rst->add("cmClose", 4, true);
   rst->add("cmZoom", 5, true);
   rst->add("cmResize", 6, true);
   rst->add("cmNext", 7, true);
   rst->add("cmPrev", 8, true);
   rst->add("cmHelp", 9, true);
   rst->add("cmOK", 10, true);
   rst->add("cmCancel", 11, true);
   rst->add("cmYes", 12, true);
   rst->add("cmNo", 13, true);
   rst->add("cmDefault", 14, true);
   rst->add("cmCut", 20, true);
   rst->add("cmCopy", 21, true);
   rst->add("cmPaste", 22, true);
   rst->add("cmUndo", 23, true);
   rst->add("cmClear", 24, true);
   rst->add("cmTile", 25, true);
   rst->add("cmCascade", 26, true);
   rst->add("cmReceivedFocus", 50, true);
   rst->add("cmReleasedFocus", 51, true);
   rst->add("cmCommandSetChanged", 52, true);
   rst->add("cmScrollBarChanged", 53, true);
   rst->add("cmScrollBarClicked", 54, true);
   rst->add("cmSelecteWindowNum", 55, true);
   rst->add("cmListItemSelected", 56, true);
   rst->add("cmClosingWindow", 57, true);
   rst->add("cmClusterMovedTo", 58, true);
   rst->add("cmClusterPress", 59, true);
   rst->add("cmRecordHistory", 60, true);
   rst->add("cmListItemFocused", 61, true);
   rst->add("cmGrabDefault", 62, true);
   rst->add("cmReleaseDefault", 63, true);
   return rst;
}

TConstCollection * InitHelpCtxLister()
{
   TConstCollection * rst = new TConstCollection(_("Help contexts"));
   rst->add("hcNoContext", 0, true);
   return rst;
}

TConstCollection *  CommandsList()
{
  if (!CommandList) CommandList = InitCommandLister();
  return CommandList;
}

TConstCollection *  HelpCtxsList()
{
  if (!HelpCtxList) HelpCtxList = InitHelpCtxLister();
  return HelpCtxList;
}

#undef DEFAULT
#undef BY_ID
#undef BY_VALUE
