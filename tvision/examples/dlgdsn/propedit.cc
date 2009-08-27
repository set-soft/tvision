/***************************************************************************

    propedit.cc - object properties editor
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
 
#include <stdio.h>
#include <string.h>

#define Uses_TEvent
#define Uses_TProgram
#define Uses_TDeskTop
#define Uses_TView
#define Uses_TScrollBar
#define Uses_TKeys
#define Uses_TDrawBuffer
#define Uses_TDialog
#define Uses_TEventQueue
#define Uses_MsgBox
#define Uses_TListBox
#define Uses_TStringCollection
#define Uses_TCollection
#define Uses_TInputLine
#define Uses_TPalette
#define Uses_TValidator
#define Uses_TFilterValidator

#include <tv.h>

#include "dsgdata.h"
#include "propedit.h"
#include "dsgobjs.h"
#include "propdlgs.h"
#include "consted.h"
#include "fdconsts.h"

enum TDataEditorType
  {
     etNone,
     etStringEditor,         // Short string value up to 60 bytes
     etIntegerEditor,        // Integer value
     etOptionsEditor,        // View options value
     etEventMaskEditor,      // View eventMask value
     etStateEditor,          // View initial state value ( some of them )
     etGrowModeEditor,       // View growMode value
     etDragModeEditor,       // View dragMode value
     etHelpCtxEditor,        // hc## value
     etCommandEditor,        // cm## value
     etButtonFlagEditor,     // Button flag value
     etWindowFlagEditor,     // Window flag value
     etValidatorEditor,      // TValidator type value
     etCharPtrEditor,        // Null terminated string value
     etValidatorDataEditor,  // TValidator data value (Filter | range)
     etStrCollectionEditor,  // Generic string lists
     etLinkEditor,           // Links for TDLabel
     etScrollBarEditor,      // ScrollBars
  };

// Objects that can be a link for a TDLabel
static TStringCollection * LinkList = 0;
// Available V and H scrollbars
static TStringCollection * ScrollList = 0;

// Global object manager list
static TLinkList * ObjLink = 0;

// Current object editor
static TObjEdit * ObjEdit;

// To resolve (sprintf "%s") when there is nothing to do
static char * blank = "";

//extern int TabOrder;

/* functions ==============================================================*/

// Standard execDialog without free the dialog
int execDialog_NoFree(TDialog * dialog, void *data)
{
   int rst;
   
   TView * d = TProgram::application->validView(dialog);
   if (d)
   {
      if (data) d->setData(data);
      rst = TProgram::deskTop->execView(d);
      if ((rst!=cmCancel) && (data)) d->getData(data);
      return rst;
   }
   else return cmCancel;
}

// Standard execDialog
int execDialog(TDialog * dialog, void *data)
{
   int rst;
   
   TView * d = TProgram::application->validView(dialog);
   if (d)
   {
      if (data) d->setData(data);
      rst = TProgram::deskTop->execView(d);
      if ((rst!=cmCancel) && (data)) d->getData(data);
      TObject::CLY_destroy( d );
      return rst;
   }
   else return cmCancel;
}

/* TLinkList ============================================================*/

// Sorter func by tabulation order
static int byTabOrder(const void * key1, const void * key2)
{
   TDsgObj * d1  = (TDsgObj *)((TDsgLink *)key1)->d;
   TDsgObj * d2  = (TDsgObj *)((TDsgLink *)key2)->d;
   
   bool d1CanTab = d1->tabStop();
   bool d2CanTab = d2->tabStop();
   
   if (!d1CanTab && d2CanTab) return -1;
   if (!d1CanTab && !d2CanTab) return 0;
   if (d1CanTab && !d2CanTab) return 1;
   
   short t1 = ((TDsgObjData *)d1->attributes)->tabOrder;
   short t2 = ((TDsgObjData *)d2->attributes)->tabOrder;

   if (d1CanTab && d2CanTab)
   {
      if ((t1 > t2) - (t1 < t2) == 0)
      {
        if (d1 == ObjEdit->object) return 1;
        if (d2 == ObjEdit->object) return -1;
      } else return (t1 > t2) - (t1 < t2);
   }
   return 0;
}
/*
static int byCreationOrder(void * key1, void * key2)
{
   c1  = ((TDsgObj *)((TDataLink *&)*key1)->d)->creationOrder;
   c2  = ((TDsgObj *)((TDataLink *&)*key2)->d)->creationOrder;
   return (c1 > c2) - (c1 < c2);
}
*/
// Test functions
static Boolean matchView(void * link, void * view)
{
   return (((TDsgLink *)link)->v == view);
}

static Boolean matchDsgObj(void * link, void * dsgObj)
{
   return (((TDsgLink *)link)->d == dsgObj);
}

static Boolean matchScroll(void * link, void * aName)
{
   TDsgObj * d = ((TDsgLink *)link)->d;
   return (
     (d->viewType == vtVScroll || d->viewType == vtHScroll) &&
     strcmp( ((TDsgObjData*)d->attributes)->thisName, (char *)aName ) == 0 );
}

static Boolean matchLabel(void * link, void * aLink)
{
   TDsgObj * d = ((TDsgLink *)link)->d;
   return ( d->viewType == vtLabel &&
      strcmp( ((TDLabelData *)d->attributes)->link, (char *)aLink ) == 0 );
}

// Find functions
TDsgLink * TLinkList::viewFind(void * aView) // by View
{
   return (TDsgLink *)firstThat(&matchView, aView);
}

TDsgLink * TLinkList::dsgObjFind(void * aDsgObj) // by DsgObj
{
   return (TDsgLink *)firstThat(&matchDsgObj, aDsgObj);
}

TDsgLink * TLinkList::linkFind(char * aName) // Look for a label
{
   return (TDsgLink *)firstThat(&matchLabel, aName);
}

TDsgLink * TLinkList::scrollFind(char * aScroll) // Look for a ScrollBar
{
   return (TDsgLink *)firstThat(&matchScroll, aScroll);
}

// Remove the object from and notificates all others in the list
void TLinkList::removeMe(TDsgObj * aDsgObj)
{
   int index;

   void * link = firstThat(&matchDsgObj, aDsgObj);
   if (link)
   {
      TDsgObjData * data = (TDsgObjData *)aDsgObj->attributes;
//    atFree(indexOf(link));
      this->free(link);
      switch(aDsgObj->viewType)
      {
         case vtVScroll:
         case vtHScroll:
            if (ScrollList->search(&data->thisName, index))
            {
               removeNotify(ScrollList, index);
               ScrollList->atRemove(index);
            }
         break;
         case vtStatic: break;
         case vtLabel: break;
         default:
            if (LinkList->search(&data->thisName, index))
            {
               removeNotify(LinkList, index);
               LinkList->atRemove(index);
            }
      }
   }
}

// Inserts a pair of pointers from the (TView, TDsgObj) descendant to the list.
// This is necessary because one TView descendant that also inherits the
// TDsgObj code cannot be directly refered as a TDsgObj outside its context.
void TLinkList::add(TView * aView, TDsgObj * aDsgObj)
{
   if (!aView || !aDsgObj) return;
   
   if (viewFind(aView) != 0) return;
   
   TDsgLink * link = new TDsgLink;
   link->v = aView;
   link->d = aDsgObj;
   insert(link);

   TDsgObjData * data = (TDsgObjData *)aDsgObj->attributes;
   switch(aDsgObj->viewType)
   {
      case vtVScroll:
      case vtHScroll: ScrollList->insert(&data->thisName); break;
      case vtStatic: break;
      case vtButton: break;
      case vtLabel:
         strcpy( ((TDLabelData *)aDsgObj->attributes)->link, blank );
      break;
      case vtMemo:
         strcpy( ((TDMemoData *)aDsgObj->attributes)->hScroll, blank );
         strcpy( ((TDMemoData *)aDsgObj->attributes)->vScroll, blank );
         LinkList->insert(&data->thisName);
      break;
      case vtListBox:
         strcpy( ((TDListBoxData *)aDsgObj->attributes)->scrollBar, blank );
         LinkList->insert(&data->thisName);
      break;
      default: LinkList->insert(&data->thisName);
   }
}

void TLinkList::freeItem(void * item)
{
   delete (char *)item;
}

// To reorganize the views in a correct tabulation order
void TLinkList::doReOrder()
{
   short Tab = 0;
   int i;
   TDsgLink * dsg;
   TDsgObjData * dsgData;
   
   if (count == 0) return;

   sort(byTabOrder);
   
   for (i = 0; i < count; i++)
   {
      dsg = (TDsgLink *)at(i);
      dsgData = (TDsgObjData *)dsg->d->attributes;
      if (dsg->d->tabStop())
      {
         dsgData->tabOrder = Tab;
         Tab++;
      }
   }
   for (i = count - 1; i > 0; i--)
   {
      dsg = (TDsgLink *)at(i);
      dsg->v->makeFirst();
   }
}

// Sorting with qsort
void TLinkList::sort(ccSortFunc Compare)
{
   qsort(items, count, sizeof(void *), Compare);
}

// Try to reorganize the objects in an ideal creation order
void TLinkList::sortForBuild()
{
#if 0
#define _item_move_(a) do { remove(tmp); atInsert(a, tmp); i++; } while(0)
   int i = 0;
   char * s1, * s2;
   TViewData * attr = 0;
   
   if (count <= 1) return;
   
   doReOrder();
   void * tmp;
   do {
      attr = (TViewData *)((TDsgLink *)items[i])->d->attributes;
      if (tmp = linkFind(attr->thisName)) _item_move_( ((i + 1) < count) ? i + 1 : i );
      i++;
   } while (i < count);
   i = 0;
   do {
      s1 = blank;
      s2 = blank;
      attr = (TViewData *)((TDsgLink *)items[i])->d->attributes;
      if ((((TDsgLink *)items[i])->d)->viewType == vtListBox)
         s1 = ((TDListBoxData *)attr)->scrollBar;
      else if ((((TDsgLink *)items[i])->d)->viewType == vtMemo)
      {
         s1 = ((TDMemoData *)attr)->vScroll;
         s2 = ((TDMemoData *)attr)->hScroll;
      }
      if (tmp = scrollFind(s2))
        { _item_move_( ((i - 1) >= 0) ? i - 1: 0 ); if (s1 != blank) i--; }
      if (tmp = scrollFind(s1))
        { _item_move_( ((i - 1) >= 0) ? i - 1: 0 ); }
      i++;
   } while (i < count);
#undef _item_move_
#endif
}

// Called by removeMe to notify dependencies
void TLinkList::removeNotify(TCollection * aCollection, int Index)
{
   int i;
   TDsgObj * d;
   for (i = 0; i < count; i++)
   {
      d = (TDsgObj *)((TDsgLink *)at(i))->d;
      if (aCollection == LinkList && d->viewType == vtLabel)
      {
         if ( strcmp( ((TDLabelData *)d->attributes)->link,
                      (char *)aCollection->at(Index) ) == 0 )
             strcpy ( ((TDLabelData *)d->attributes)->link, blank );
      }
      else if (aCollection == ScrollList && d->viewType == vtListBox)
      {
         if ( strcmp( ((TDListBoxData *)d->attributes)->scrollBar,
                      (char *)aCollection->at(Index) ) == 0 )
             strcpy ( ((TDListBoxData *)d->attributes)->scrollBar, blank );
      }
      else if (aCollection == ScrollList && d->viewType == vtMemo)
      {
         if ( strcmp( ((TDMemoData *)d->attributes)->hScroll,
                      (char *)aCollection->at(Index) ) == 0 )
            strcpy ( ((TDMemoData *)d->attributes)->hScroll, blank );
         if ( strcmp( ((TDMemoData *)d->attributes)->hScroll,
                      (char *)aCollection->at(Index) ) == 0 )
            strcpy ( ((TDMemoData *)d->attributes)->vScroll, blank );
      }
   }
}

// Called when an object has its "name" changed in the editor
void TLinkList::linkChangedName(char * oldName, char * newName)
{
   int i;

   pack();
   
   for (i = 0; i < count; i++)
   {
      TDsgLink * d = (TDsgLink *)items[i];
      TDsgObj * obj = d->d;
      if (obj->viewType == vtListBox)
      {
         if (strcmp( (char *)((TDListBoxData *)obj->attributes)->scrollBar,
            oldName ) == 0) strcpy(
              (char *)((TDListBoxData *)obj->attributes)->scrollBar, newName);
      }
      else if (obj->viewType == vtMemo)
      {
         if (strcmp( (char *)((TDMemoData *)obj->attributes)->hScroll,
            oldName ) == 0) strcpy(
              (char *)((TDMemoData *)obj->attributes)->hScroll, newName);
         if (strcmp( (char *)((TDMemoData *)obj->attributes)->vScroll,
            oldName ) == 0) strcpy(
              (char *)((TDMemoData *)obj->attributes)->vScroll, newName);
      }
      else if (obj->viewType == vtLabel)
      {
         if (strcmp( (char *)((TDLabelData *)obj->attributes)->link,
            oldName ) == 0) strcpy(
               (char *)((TDLabelData *)obj->attributes)->link, newName);
      }
   }
}

/* ObjectLinker -----------------------------------------------------------*/

TLinkList * ObjectLinker() { return ObjLink; }

/* TInplaceEdit ===========================================================*/
// A quick editor for int values and short string values
//                       Nor Nor Sel Arrows
#define cpInplaceEdit  "\x06\x06\x07\x05"


TInPlaceEdit::TInPlaceEdit(const TRect& bounds, ushort aMaxLen,
                           TValidator * aValidator):
                           TInputLine(bounds, aMaxLen)
                           { setValidator(aValidator); }
                           
void TInPlaceEdit::handleEvent(TEvent& event)
{
   if (event.what == evKeyDown)
   {
      switch(event.keyDown.keyCode)
      {
         case kbEnter:
            endState = cmOK;
            clearEvent(event);
         break;
         case kbEsc:
            endState = cmCancel;
            clearEvent(event);
         break;
         case '-': if ((validator) && curPos != 0) clearEvent(event);
      }
   }
   if ((event.what == evMouseDown) &&
      (!containsMouse(event)) ) endState = cmCancel;
   TInputLine::handleEvent(event);
}

TPalette& TInPlaceEdit::getPalette() const
{
  static TPalette palette( cpInplaceEdit, sizeof(cpInplaceEdit) - 1 );
  return palette;
}

ushort TInPlaceEdit::execute()
{
   TEvent e;
   
   endState = 0;
   
   do
   {
     getEvent(e);
     handleEvent(e);
   } while (endState == 0);
   
   return endState;
}

/* Inplace editors --------------------------------------------------------*/

bool IntegerEditor(int& value, TPoint place, TGroup * host)
{
   bool rst = false;
   char strval[20];

   TRect r(place.x, place.y, place.x +
           (host->size.x - place.x - 1), place.y + 1);
   
   sprintf(strval, "%i", value);

   TFilterValidator * val = new TFilterValidator("-0123456789");
   
   TInPlaceEdit * editor = new TInPlaceEdit(r, 6, val);
       
   editor->setData(&strval);
   rst = ( host->execView(editor) == cmOK );
   if (rst)
   {
      editor->getData(&strval);
      value = atoi(strval);
   }
   TObject::CLY_destroy( editor );
   return rst;
}

bool StringEditor(char * string, TPoint place, TGroup * host, ushort aMaxLen)
{
   bool rst = false;

   TRect r(place.x, place.y, place.x +
           (host->size.x - place.x - 1), place.y + 1);
   
   TInPlaceEdit * editor = new TInPlaceEdit(r, aMaxLen, 0);
       
   editor->setData(string);
   rst = ( host->execView(editor) == cmOK );
   if (rst) editor->getData(string);
   TObject::CLY_destroy( editor );
   return rst;
}

/* TStructMap =============================================================*/
// Configurable data mapper used to edit values in the TDsgObj::attributes
// field
class TStructMap
{
public:
   TStructMap(const char * Label,
//          int OffSet,
            int DataSize,
            TDataEditorType aEditorType,
            bool ReadOnly = false,
            TStructMap *Next = 0);
   ~TStructMap();
   
   TStructMap * prev;
   TStructMap * next;
   int index;
   int offset;
   int dataSize;
   int editorType;
   char * label;
   bool readOnly;
};

TStructMap::TStructMap(const char * Label,
            int DataSize,
            TDataEditorType aEditorType,
            bool ReadOnly ,
            TStructMap *Next )
{
   label = newStr(Label);
   offset = 0;
   dataSize = DataSize;
   editorType = aEditorType;
   index = 0;
   readOnly = ReadOnly;
   prev = NULL;
   next = Next;
   if (next != 0)
   {
      next->prev = this;
      next->offset = offset + dataSize;
      next->index = index + 1;
   }
}

TStructMap::~TStructMap()
{
   delete[] label;
   if (next) delete(next);
}

TStructMap& operator + ( TStructMap& map1, TStructMap& map2 )
{
   TStructMap *map = &map1;
   while (map->next) map = map->next;
   map->next = &map2;
   map2.offset = map->offset + map->dataSize;
   map2.prev = map;
   if (map2.dataSize > 0)
   {
      while ((map->dataSize == 0) && (map->prev)) map = map->prev;
      map2.index = map->index + 1;
   }
   return map2;
}

/* Data mappers -----------------------------------------------------------*/

// Commom TView data map initialization code fragment
#define _separator_(a) *new TStructMap(a, 0, etNone, true, 0)

#define _viewmap_() \
     _separator_("TView data")+ \
     *new TStructMap("Left", sizeof(int), etIntegerEditor)+\
     *new TStructMap("Top", sizeof(int), etIntegerEditor)+\
     *new TStructMap("Width", sizeof(int), etIntegerEditor)+\
     *new TStructMap("Height", sizeof(int), etIntegerEditor)+\
     *new TStructMap("ClassName", MAX_LABEL_LENGTH, etStringEditor, true)+\
     *new TStructMap("Name", MAX_LABEL_LENGTH, etStringEditor)+\
     *new TStructMap("Options", sizeof(ushort), etOptionsEditor)+\
     *new TStructMap("EventMask", sizeof(ushort), etEventMaskEditor)+\
     *new TStructMap("State", sizeof(ushort), etStateEditor)+\
     *new TStructMap("GrowMode", sizeof(ushort), etGrowModeEditor)+\
     *new TStructMap("DragMode", sizeof(ushort), etDragModeEditor)+\
     *new TStructMap("HelpContext", sizeof(ushort), etHelpCtxEditor)

/* TPoint origin;\
   TPoint size;\
   TNameStr className;\
   TNameStr thisName;\
   ushort options;\
   ushort eventMask;\
   ushort state;\
   ushort growMode;\
   ushort dragMode;\
   ushort helpCtx\ */

static const TStructMap * TViewMap = &(_viewmap_());

static const TStructMap * TDialogMap = &(
     _viewmap_()+
     _separator_("TDialog data") +
     *new TStructMap("Title", MAX_TITLE_LENGTH, etStringEditor) +
     *new TStructMap("Flags", sizeof(ushort), etWindowFlagEditor, 0) );

static const TStructMap * TLabelMap = &(
     _viewmap_()+
     _separator_("TLabel data")+
     *new TStructMap("Text", MAX_LABEL_LENGTH, etStringEditor)+
     *new TStructMap("Link", MAX_LABEL_LENGTH, etLinkEditor, 0) );

static const TStructMap * TInputMap = &(
     _viewmap_()+
     _separator_("TInputLine data") +
     *new TStructMap("TabOrder", sizeof(int), etIntegerEditor) +
     *new TStructMap("MaxLen", sizeof(int), etIntegerEditor) +
     *new TStructMap("Validator", sizeof(ushort), etValidatorEditor) +
     *new TStructMap("ValidatorData", MAX_LABEL_LENGTH, etValidatorDataEditor, 0) );

static const TStructMap * TMemoMap = &(
     _viewmap_()+
     _separator_("TMemo data") +
     *new TStructMap("TabOrder", sizeof(int), etIntegerEditor) +
     *new TStructMap("HScroll", MAX_LABEL_LENGTH, etScrollBarEditor, 0) +
     *new TStructMap("VScroll", MAX_LABEL_LENGTH, etScrollBarEditor, 0) +
     *new TStructMap("BufSize", sizeof(uint32), etIntegerEditor) );
//   *new TStructMap("Text", sizeof(char *), etCharPtrEditor, 0) );

static const TStructMap * TStaticMap = &(
     _viewmap_()+
     _separator_("TStaticText data") +
     *new TStructMap("Text", sizeof(char *), etCharPtrEditor, 0) );
     
static const TStructMap * TButtonMap = &(
     _viewmap_()+
     _separator_("TButton data") +
     *new TStructMap("TabOrder", sizeof(int), etIntegerEditor) +
     *new TStructMap("Title", MAX_LABEL_LENGTH, etStringEditor) +
     *new TStructMap("Command", sizeof(ushort), etCommandEditor) +
     *new TStructMap("Flags", sizeof(ushort), etButtonFlagEditor) );

static const TStructMap * TListBoxMap = &(
     _viewmap_()+
     _separator_("TListBox Data") +
     *new TStructMap("TabOrder", sizeof(int), etIntegerEditor) +
     *new TStructMap("Columns", sizeof(short), etIntegerEditor) +
     *new TStructMap("ScrollBar", MAX_LABEL_LENGTH, etScrollBarEditor) );

static const TStructMap * TClusterMap = &(
     _viewmap_()+
     _separator_("TCluster Data") +
     *new TStructMap("TabOrder", sizeof(int), etIntegerEditor) +
     *new TStructMap("Items", sizeof(void *), etStrCollectionEditor) );

#undef _viewmap_

/* TObjEditView ===========================================================*/

TObjEditView::TObjEditView(const TRect& bounds, TScrollBar * v):
              TView(bounds)
{
   sb = v;
   separator = size.x / 2;
   eventMask = evMouse | evKeyboard | evCommand | evBroadcast;
   currentMap=0;
}

void TObjEditView::setMap(const TStructMap * aMap, void * Data)
{
   data = Data;
   if (!data)
   {
      dataMap = 0;
      currentMap = 0;
      drawView();
      return;
   }
   while (aMap->next) aMap = aMap->next;
   sb->setRange(1, aMap->index);
   while (aMap->prev) aMap = aMap->prev;
   dataMap = aMap;
   if (aMap->dataSize == 0)
     while ((aMap->next) && (aMap->dataSize == 0)) aMap = aMap->next;
   currentMap = aMap;
   drawView();
}

void calcPlace(TPoint& p, short column, short line, const TStructMap * map)
{
   p.x = column + 1;
   p.y = line;
   while (map->next)
   {
      if (map->dataSize == 0) p.y++;
      map = map->next;
      if (map->index >= line) break;
   }
}

int findStr(TCollection * col, const char * cmp)
{
   int i, c = col->getCount();
   if (c == 0) return -1;
   for (i = 0; i < c; i++)
     if (strcmp(cmp, (char *)col->at(i)) == 0) return i;
   return 0;
}

void TObjEditView::editItem(const TStructMap * map)
{
   ushort Val;
   TListBoxRec rec;
   void * v;
   TMemoData m;
   char * c;
   int i;
   TPoint place;
   char oldName[30];

#define _editlink_(list, editor)                                             \
          if (list->getCount() == 0) break;                                  \
          rec.items = list;                                                  \
          rec.selection = findStr(list, (char*)ldata);                       \
          if (rec.selection < 0) break;                                      \
          chg = (execDialog(editor(), &rec) == cmOK);                        \
          if (chg) strcpy((char *)ldata, (char *)list->at(rec.selection));   \
          break;
          
#define _do_(editor) chg = (execDialog(editor(), ldata) == cmOK); break;
#define _constsel_(list, editor)                                             \
          rec.items = list();                                                \
          rec.selection = list()->getIndex((ushort)(int)ldata);              \
          chg = (execDialog(editor(), &rec) == cmOK);                        \
          if (chg) memcpy(ldata, &rec.selection, sizeof(ushort)); break;


   if (map->readOnly)
   {
       messageBox("Ops! Read only value.", mfOKButton);
       return;
   }
  
   void * ldata = (void *)((char *)data + map->offset);
   bool chg = false;
  
   switch(map->editorType)
   {
       case etStringEditor:
          calcPlace(place, separator, map->index, dataMap);
          if (map->index == 6) strcpy(oldName, (char *)ldata);
          chg = StringEditor((char *)ldata, place, owner, map->dataSize);
          if ( (chg) && (map->index == 6 || vtCurrent == vtVScroll ||
                                            vtCurrent == vtHScroll ) )
          {
             if (strlen((char *)ldata) == 0)
             {
                strcpy((char *)ldata, oldName);
                chg = false;
                break;
             }
             else if (strcmp((char *)ldata, oldName) != 0)
                ObjLink->linkChangedName(oldName, (char *)ldata);
          }
          if (chg) message(owner, evMessage, cmValueChanged, 0);
       break;
       case etIntegerEditor:
          calcPlace(place, separator, map->index, dataMap);
          if (vtCurrent == vtListBox && map->index == 14)
               i = (short)(int)ldata; else i = (int)ldata;
          chg = IntegerEditor(i, place, owner);
          if (vtCurrent == vtListBox && map->index == 14)
             ldata = (void *)i; else ldata = (void *)i;
          if (chg) message(owner, evMessage, cmValueChanged, 0);
       break;
       case etOptionsEditor: _do_(OptionsEditor);
       case etEventMaskEditor:
          memcpy(&Val, ldata, sizeof(ushort));
          if (Val & evCommand) { Val &= ~evCommand; Val |= 0x20; }
          if (Val & evBroadcast) { Val &= ~evBroadcast; Val |= 0x40; }
          chg = (execDialog(EventMaskEditor(), ldata) == cmOK);
          if (chg)
          {
             if (Val & 0x20) { Val &= ~0x20; Val |= evCommand; }
             if (Val & 0x40) { Val &= ~0x40; Val |= evBroadcast; }
             memcpy(ldata, &Val, sizeof(ushort));
          }
       break;
       case etStateEditor: _do_(StateEditor);
       case etGrowModeEditor: _do_(GrowModeEditor);
       case etDragModeEditor: _do_(DragModeEditor);
       case etHelpCtxEditor: _constsel_(HelpCtxsList, HelpCtxEditor);
       case etCommandEditor: _constsel_(CommandsList, CommandEditor);
       case etButtonFlagEditor: _do_(ButtonFlagEditor);
       case etWindowFlagEditor: _do_(WindowFlagEditor);
       case etValidatorEditor: _do_(ValidatorEditor);
       case etValidatorDataEditor: break;
       case etStrCollectionEditor:
          chg = (execDialog(ItemsEditor(), ldata) == cmOK);
          if (chg) message(owner, evMessage, cmValueChanged, 0);
       break;
       case etCharPtrEditor:
          c = (char *)ldata;
          strcpy(m.buffer, c);
          c = (char *)m.buffer;
          v = c;
          while (c[0]) // Turning '\x3' to "\x3"
          {
              if (c[0] == 3/*&& (c + 3 - v) < 1024*/)
              {
                  memmove(c + 3, c + 1, strlen(c) + 1);
                  memcpy(c, "\\x3", 3);
              }
              c++;
          }
          c = (char *)v;
          m.length = strlen(c);
          chg = (execDialog(CharPtrEditor(), &m) == cmOK);
          if (chg)
          {
              if (m.length == 0) strcpy(c, "");
              else  // Turning simbolic chars to raw
              {
                  delete[] c;
                  c = (char *)m.buffer;
                  while (c[0]) // eating CR chars
                  {
                      if (c[0] == '\x0d') memmove(c, c + 1, strlen(c - 1));
                      c++;
                  }
                  c = (char *)m.buffer;
                  while (c[0]) // Turning "\x3" to '\x3'
                  {
                      i = 0;
                      if (c[0]==92)
                      {
                          while (c[i] != '3') { if (c[i] == 0) break; i++; }
                          if (i <= 4)
                          {
                              c[0] = 3;
                              memmove(&c[1], &c[i+1], strlen(c) - i);
                          }
                      }
                      c++;
                  }
                  ldata=(void *)newStr(m.buffer);
              }
              message(owner, evMessage, cmValueChanged, 0);
          }
       break;
       case etLinkEditor: _editlink_(LinkList, LinkEditor);
       case etScrollBarEditor: _editlink_(ScrollList, ScrollEditor);
   }
   if (chg) drawView();
#undef _editlink_
#undef _do_
}

const char * TObjEditView::getValueFor(const TStructMap * map)
{
  static char buf[60];
  static ushort ushrtval;
  static int intval;
  static void * ptrval;
  static void * ldata;
  
  ldata = data;
  ldata = (void *)((char *)ldata + map->offset);
  
  strcpy(buf, "<none>");

  if (map->editorType == etIntegerEditor)
  {
     memcpy(&intval, ldata, map->dataSize);
     sprintf(buf, "%i", intval);
  }
  else if ((map->editorType >= etOptionsEditor) &&
    (map->editorType <= etValidatorEditor))
  {
     memcpy(&ushrtval, ldata, map->dataSize);
     switch(map->editorType)
     {
        case etOptionsEditor: return OptionsStr(ushrtval);
        case etEventMaskEditor: return EventMaskStr(ushrtval);
        case etStateEditor: return StateStr(ushrtval);
        case etGrowModeEditor: return GrowModeStr(ushrtval);
        case etDragModeEditor: return DragModeStr(ushrtval);
        case etHelpCtxEditor: return HelpCtxStr(ushrtval);
        case etCommandEditor: return CommandStr(ushrtval);
        case etButtonFlagEditor: return ButtonFlagStr(ushrtval);
        case etWindowFlagEditor: return WindowFlagStr(ushrtval);
        case etValidatorEditor: return ValidatorStr(ushrtval);
     }
  }
  else if (map->editorType == etStrCollectionEditor ||
           map->editorType == etCharPtrEditor)
  {
     memcpy(&ptrval, ldata, map->dataSize);
     if (map->editorType == etCharPtrEditor)
     return CharPtrStr(ptrval); else return ItemsStr(ptrval);
  }
  else if ( map->editorType == etStringEditor ||
            map->editorType == etValidatorDataEditor )
  {
     memset(&buf, 0, 60);
     memcpy(&buf, ldata, map->dataSize);
     return buf;
  }
  else if (  map->editorType == etScrollBarEditor ||
             map->editorType == etLinkEditor )
  {
     if (strlen((char *)ldata) > 0) return strcpy(buf, (char *)ldata);
  }
  return buf;
}

class TLDrawBuffer: public TDrawBuffer
{
public:
   ushort getData(ushort i) { return data[i]; }
};

#define _lo_(a) ((uchar *)&a)[0]

void TObjEditView::draw()
{
   TLDrawBuffer b;
   const TStructMap * curMap;
   const TStructMap * cur;
   int r, l, tmp, line = 0;
   char lstr[100], rstr[100];
   char lfmt[10], rfmt[10];
   ushort attr;
   
   if (separator > size.x) separator = size.x / 2;
   r = (size.x - separator) + separator;
   l = separator;
   
   curMap = currentMap;
   if ((curMap) && size.y > 0)
   {
      cur = curMap;
      while (cur->prev) cur = cur->prev;
      do
      {
         if (cur->dataSize == 0)
         {
            tmp = (size.x - strlen(cur->label)) / 2;
            b.moveChar(0, 0x20, 0x3e, tmp);
            b.moveBuf(tmp, cur->label, 0x3e, strlen(cur->label));
            b.moveChar(tmp + strlen(cur->label), 0x20, 0x3e, tmp + 1);
            attr = b.getData(l);
            if (_lo_(attr) == 0x20) b.moveChar(l, 0xb3, 0x31, 1);
            writeLine(0, line, size.x, 1, b);
            line++;
         }
         else
         {
            sprintf(lfmt, "%%-%is", l - 1);
            sprintf(lstr, lfmt, cur->label);
            sprintf(rfmt, "%%-%is", r);
            char *tmp=newStr(getValueFor(cur));
            if (tmp && strlen(tmp)>=(size_t)r)
               tmp[r-1]=0;
            sprintf(rstr, rfmt, tmp ? tmp : "ERROR");
            DeleteArray(tmp);
            if (cur == curMap)
              attr = 0x71; else attr = 0x1f;
            b.moveBuf(0, lstr, attr, l);
            if (cur == curMap)
              attr = 0x71; else attr = 0x13;
            b.moveChar(l, 0xb3, attr, 1);
            if (cur == curMap)
              attr = 0x71; else attr = 0x1e;
            b.moveBuf(l + 1, rstr, attr, r);
            writeLine(0, line, size.x, 1, b);
            line++;
         }
         if (line > size.y - 1) break;
         cur = cur->next;
      } while (cur);
      b.moveChar(0, 0x20, 0x1f, size.x);
      b.moveChar(l, 0xb3, 0x13, 1);
      writeLine(0, line, size.x, size.y - line, b);
   }
   else TView::draw();
}

#undef _lo_

const TStructMap * itemForLine(int line, const TStructMap * Map)
{
   ushort i = 1;
   
   if (line == 0 || Map == 0) return 0;
   while (Map->next)
   {
      Map = Map->next;
      if (i++ == line) break;
   }
   if (Map->dataSize != 0) return Map; else return 0;
}

#define _endcase_(a) if (currentMap->dataSize == 0) \
 while ((currentMap->a) && (currentMap->dataSize == 0))\
 currentMap = currentMap->a; sb->setValue(currentMap->index); \
 drawView(); clearEvent(event)

void TObjEditView::handleEvent(TEvent& event)
{
   const TStructMap * t;
   ushort i;
   
   TView::handleEvent(event);
   
   if (!currentMap) return;
   
   TPoint Locate;
   if (event.what == evBroadcast &&
       event.message.command == cmScrollBarChanged &&
       event.message.infoPtr == sb)
   {
      i = sb->value;
      t = dataMap;
      while ((t->index != i) && (t->next)) t = t->next;
      if ((t->index == i) && (t != currentMap))
      {
         currentMap = t;
         drawView();
      }
   }
   else
   if (event.what == evMouseDown)
   {
      Locate = makeLocal(event.mouse.where);
      if (Locate.x == separator)
      {
        do
        {
           Locate = makeLocal(event.mouse.where);
           if ((Locate.x >= 1) && (Locate.x <= (size.x - 2)))
             {  separator = Locate.x; drawView(); }
        } while ( mouseEvent( event, evMouseMove ) );
      }
      else
      {
         t = itemForLine(Locate.y, dataMap);
         if ((t) && t != currentMap)
         {
            sb->setValue(t->index);
            currentMap = t;
            drawView();
         }
         else if ((t == currentMap) && (event.mouse.doubleClick) &&
             (event.mouse.buttons == mbLeftButton)) editItem(currentMap);
      }
   }
   else
   if (event.what == evKeyboard)
   {
      switch(event.keyDown.keyCode)
      {
         case kbUp:
           if (currentMap->prev)
           {
             currentMap = currentMap->prev;
             if (currentMap->dataSize == 0)
               if (currentMap->prev) currentMap = currentMap->prev;
             _endcase_(next);
           }
         break;
         case kbDown:
           if (currentMap->next)
           {
             currentMap = currentMap->next;
             _endcase_(next);
           }
         break;
         case kbPgUp:
           while (currentMap->prev) currentMap = currentMap->prev;
           _endcase_(next);
         break;
         case kbPgDn:
           while (currentMap->next) currentMap = currentMap->next;
           _endcase_(prev);
         break;
         case kbEnter:
           clearEvent(event);
           if (currentMap->dataSize > 0) editItem(currentMap);
         break;
         case kbDel:
           if (currentMap->editorType == etScrollBarEditor ||
               currentMap->editorType == etLinkEditor)
               strcpy((char *)data + currentMap->offset, blank);
           drawView();
           clearEvent(event);
         break;
      }
   }
}

#undef _endcase_

/* TObjedit ===============================================================*/

TObjEdit::TObjEdit(TDsgObj * Obj):
          TWindowInit(&TObjEdit::initFrame),
          TWindow( TRect(55, 0, 80, 23), _("Object editor"), 0 )
{
   flags &= ~wfClose;
   flags |= wfGrow;
   if (!LinkList) LinkList = new TStringCollection(0, 1);
     else LinkList->removeAll();
   if (!ScrollList) ScrollList = new TStringCollection(0, 1);
     else ScrollList->removeAll();
   LinkList->duplicates = False;
   ScrollList->duplicates = False;
   if (!ObjLink) ObjLink = new TLinkList();
     else ObjLink->removeAll();
   TScrollBar * sb = new TScrollBar( TRect(24, 1, 25, 19) );
   insert(sb);
   sb->growMode |= gfGrowHiY;
   TRect r = getExtent();
   r.grow(-1, -1);
   r.b.y -= 3;
   dataView = new TObjEditView( r, sb );
   dataView->growMode |= (gfGrowHiY | gfGrowHiX);
   dataView->options |= ofPreProcess | ofPostProcess | ofFramed;
   ObjEdit = this;
   insert(dataView);
   setObjData(Obj);
}

void TObjEdit::handleEvent(TEvent& event)
{
   TWindow::handleEvent(event);
   if (object == 0) return;
   if (event.what == evMessage &&
       event.message.command == cmValueChanged)
   {
      object->dsgUpdate();
      clearEvent(event);
   }
}

void TObjEdit::setObjData(TDsgObj * Obj)
{
   const TStructMap * Map = 0;
   
   if ((!Obj) || object == Obj) return;

   dataView->setMap(0, 0);
   
   object = Obj;
   
   switch (Obj->viewType)
   {
      case vtLabel: Map = TLabelMap; break;
      case vtInput: Map = TInputMap; break;
      case vtMemo: Map = TMemoMap; break;
      case vtStatic: Map = TStaticMap; break;
      case vtButton: Map = TButtonMap; break;
      case vtListBox: Map = TListBoxMap; break;
      case vtRadioButton: Map = TClusterMap; break;
      case vtCheckBox: Map = TClusterMap; break;
      case vtDialog: Map = TDialogMap; break;
      default: Map = TViewMap;
   }
   if (!Map)
   {
      messageBox("Sem mapa!", mfOKButton);
      dataView->setMap(0, 0);
      return;
   }
   
#if 0
   int i = 0;
   TStructMap * m = Map;
   char c[100];
   m = Map;
   while (m->prev) m = m->prev;
   while (m) { i += m->dataSize; m = m->next; };
   if (i != Obj->getAttrSize())
   {
      sprintf(c, "diferenca mapa: %i  registro: %i", i, Obj->getAttrSize());
      messageBox(c, mfOKButton);
   }
#endif
   dataView->vtCurrent = Obj->viewType;
   dataView->setMap(Map, Obj->attributes);
}


