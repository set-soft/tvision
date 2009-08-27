/***************************************************************************

    dsgobjs.cc - Implements editable classes
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
 
#define Uses_string
#define Uses_stdio
#define Uses_stdlib
#define Uses_iostream

#define Uses_TProgram
#define Uses_TApplication
#define Uses_TDeskTop
#define Uses_TKeys
#define Uses_TSItem
#define Uses_TEventQueue
#define Uses_MsgBox
#define Uses_TStringCollection
#define Uses_ofpstream
#define Uses_ifpstream

#include <tv.h>

#include "dsgobjs.h"
#include "propedit.h"
#include "dsgdata.h"
#include "strmoper.h"

static int vtAttrSize[vtDialog + 1] = { sizeof(TDsgObjData),
                                        sizeof(TDLabelData),
                                        sizeof(TDInputData),
                                        sizeof(TDMemoData),
                                        sizeof(TDStaticData),
                                        sizeof(TDButtonData),
                                        sizeof(TDListBoxData),
                                        sizeof(TDClusterData),
                                        sizeof(TDClusterData),
                                        sizeof(TViewData),
                                        sizeof(TViewData),
                                        sizeof(TDDialogData) };
                                        
static char * TheClassName[vtDialog + 1] = { "TUser",
                                             "TLabel",
                                             "TInputLine",
                                             "TMemo",
                                             "TStaticText",
                                             "TButton",
                                             "TListBox",
                                             "TRadioButtons",
                                             "TCheckBoxes",
                                             "TScrollBar",
                                             "TScrollBar",
                                             "TDialog" };


#if 0 // A useful visual map of AppPalette indexed by dialog and app

/* The palette of an internal view is mapped in the palette of a group view
   which owns it.
   if you need to make an internal view visible out of the group for what
   it was designed then you must to redefine the getPalette() method to
   return a TPalette that is its real palette index in the global palette. */
   
// app "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
  Attr "\x71\x70\x78\x74\x20\x28\x24\x17\x1F\x1A\x31\x31\x1E\x71\x00"
// app "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f"
  Attr "\x37\x3F\x3A\x13\x13\x3E\x21\x00\x70\x7F\x7A\x13\x13\x70\x7F\x00"

// Below the indices of our interest

// app "\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f"
#  dlg "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10"
  Attr "\x70\x7F\x7A\x13\x13\x70\x70\x7F\x7E\x20\x2B\x2F\x78\x2E\x70\x30"
    
// app "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f"
#  dlg "\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f\x20"
  Attr "\x3F\x3E\x1F\x2F\x1A\x20\x72\x31\x31\x30\x2F\x3E\x31\x13\x00\x00"
    
#endif

// Macros *******************************************************************

// Define the palette for editable objects using global index
#define cpDragLabel   "\x26\x27\x28\x28"
#define cpDragInput   "\x32\x33\x34\x35"
#define cpDragMemo    "\x39\x3a"
#define cpDragStatic  "\x25"
#define cpDragBtn     "\x29\x2a\x2b\x2c\x2d\x2d\x2d\x2e"
#define cpDragLB      "\x39\x39\x3a\x3b\x3c"
#define cpDragCluster "\x2f\x30\x31\x31\x3e"
#define cpDragScroll  "\x23\x24\x24"

// The palette to the selection state. Attr to this index (0x20) was changed
// in the app constructor. See freedsgn.cpp.TFreeDsgnApp::TFreeDsgnApp
#define cpSelLabel    "\x20\x20\x20\x20"
#define cpSelInput    "\x20\x20\x20\x20"
#define cpSelMemo     "\x20\x20"
#define cpSelStatic   "\x20"
#define cpSelBtn      "\x20\x20\x20\x0d\x20\x20\x20\x0f"
#define cpSelLB       "\x20\x20\x20\x20\x20"
#define cpSelCluster  "\x20\x20\x20\x20\x20"
#define cpSelScroll   "\x20\x20\x20"

// Answers to ImOwned(TView *). Used to decide somethings
#define ByToolDlg  0
#define ByDesktop  1
#define ByEditor   2

// This state flags maybe removed in future
#define sfEditing        0x1000
#define sfLoading        0x2000

// Commom event handler code fragment for editable classes
#define _h_(a)\
    if (ImOwned(this) == ByToolDlg)                                         \
    { if (!HandleEventOnToolDlg(this, event, viewType))                     \
       a::handleEvent(event); }                                             \
    else if (ImOwned(this) == ByEditor)                                     \
    { if (!HandleEventOnEditor(this, event, viewType)) a::handleEvent(event); }

// Common getpalette function code fragment for editable classes
// SET: Palettes are supposed to be constants, so they should call members
// like here
#define _p_(a,b,c)                                                          \
  static TPalette palette( a, sizeof(a) - 1);                               \
  static TPalette pal_sel( b, sizeof(b) - 1);                               \
  if (owner == TProgram::deskTop) return palette;                           \
  else return c::getPalette();
/*  else if ((owner != TProgram::deskTop) && (getState(sfSelected)))          \
    return pal_sel; else return c::getPalette();*/

// Commom changeBounds code fragment for editable classes
#define _chgbnds_(a) a::changeBounds(bounds); \
        setPos(origin, size); ObjEdit->CLY_Redraw(); \
        EditDlg->modified = true;

// Commom setState code fragment for all editable classes
#define _setstate_(t) do {                                                  \
  t::setState(aState, enable);                                              \
  if (aState & sfEditing && enable)                                         \
/*{*/ ObjectLinker()->add(this, this); /* <- was DsgObj */                  \
/*    setViewData((TDsgObjData &)*attributes); }  */                        \
  if (aState & (sfFocused | sfSelected) )                                   \
  { if (owner != EditDlg) { drawView(); return; }                           \
    if (enable) ObjEdit->setObjData(this); drawView(); } } while(0)

// Commom shutDown code fragment for all editable classes
#define _shutdown_(a) /* delete dsgObj; */ \
/*  TDsgLink * l = ObjectLinker()->viewFind(this); */ \
/*  if (l) delete l->d;  */ \
    a::shutDown()

// Static *******************************************************************

// This always points to Current Tool Dialog and Editor Dialog
static TDDialog * EditDlg;
static TDialog * ToolDlg;
static TObjEdit * ObjEdit;

// To help keyboard driven object positionment
static TView * FloatingView;
static bool keyPut = false;

// Counters used to define Labels
static int LabelCount   = 0;
static int InputCount   = 0;
static int MemoCount    = 0;
static int StaticCount  = 0;
static int ButtonCount  = 0;
static int ListBoxCount = 0;
static int RadioCount   = 0;
static int CheckCount   = 0;
static int VScrollCount = 0;
static int HScrollCount = 0;
static int UserCount    = 0;
static int DialogCount  = 0;
static int TabOrder     = 0;

///extern TLinkList * ObjLink;

// Holds the current mouse position on screen
static MouseEventType * Mouse;

// Flag for visibility of grid in the Dialog Editor
static bool GridState = true;

// Functions ****************************************************************

// Set the visibility of Grid in the dialog editor
void SetGrid(bool enable)
{
   GridState = enable;
   EditDlg->CLY_Redraw();
}

bool GetGrid()
{
   return GridState;
}

// ToolDlg (Object Dialog) creation function
TDialog * CreateTool()
{
   TDialog * d = new TDialog(TRect(0, 0, 55, 6), "Obj");
   TDMemo * Memo;
   TDListBox * LB;
   TView * view;
   TMemoData buf;

   d->flags &= ~wfClose;
   // d->flags |= wfGrow;
   d->insert(new TDInputLine(TRect(2, 1, 7, 2)));
   d->insert(view = new TDLabel(TRect(8, 1, 13, 2)));
   view->options |= ofSelectable;
   d->insert(view = new TDStaticText(TRect(2, 3, 7, 4)));
   view->options |= ofSelectable;
   d->insert(new TDButton(TRect(7, 3, 13, 5)));
   d->insert(new TDCheckBoxes(TRect(15, 1, 20, 2), new TSItem("", 0) ));
   d->insert(new TDRadioButtons(TRect(15, 3, 20, 4), new TSItem("", 0) ));
   d->insert(LB = new TDListBox(TRect(22, 1, 27, 4)));
   d->insert(view = new TVScrollBar(TRect(28, 1, 29, 4)));
   view->options |= ofSelectable;
   view->growMode = 0;
   d->insert(Memo = new TDMemo(TRect(31, 1, 37, 2)));
   buf.length = 6;
   strcpy(buf.buffer, " Memo ");
   Memo->setData(&buf);
   Memo->growMode = 0;
   d->insert(view = new THScrollBar(TRect(31, 3, 37, 4)));
   view->options |= ofSelectable;
   view->select();
   view->growMode = 0;
   d->selectNext(False);
   return d;
}

// Called to initialize the editor. if FileName = NULL creates a blank dialog
void InitDlgEditor(const char * FileName)
{
/* if (EditDlg != 0)
   {
      if (!EditDlg->Save()) return;
      TProgram::deskTop->remove(EditDlg);
      TObject::destroy(EditDlg);
      EditDlg = 0;
   }*/

   Mouse = &TEventQueue::curMouse;

   if (!ObjEdit)
   {
      ObjEdit = new TObjEdit(EditDlg);
      TProgram::deskTop->insert(ObjEdit);
   }
   DialogCount++;
   if (!EditDlg) EditDlg = new TDDialog();
     else if ((EditDlg) && EditDlg->modified)
     {
         if (!EditDlg->Save(cmCancel)) return;
         TProgram::CLY_destroy(EditDlg);
         EditDlg = new TDDialog();
     }
   TProgram::deskTop->insert(EditDlg);
   if (FileName != 0) EditDlg->loadFromFile(FileName);
   if (!ToolDlg)
   {
      ToolDlg = CreateTool();
      TProgram::deskTop->insert(ToolDlg);
   }
}


TDDialog * DialogEditor() { return EditDlg; }

TDialog * ToolDialog() { return ToolDlg; }

TWindow * ObjectEditor() { return ObjEdit; }

/* DefineRect ------------------------------------------------------------*/
// returns the initial bounds rect for each type of view
TRect DefineRect(TViewType vt, TPoint p)
{
   switch(vt)
   {
      case vtLabel: return TRect(p.x, p.y, p.x + 7, p.y + 1);
      case vtInput: return TRect(p.x, p.y, p.x + 10, p.y + 1);
      case vtStatic: return TRect(p.x, p.y, p.x + 12, p.y + 1);
      case vtButton: return TRect(p.x, p.y, p.x + 12, p.y + 2);
      case vtVScroll: return TRect(p.x, p.y, p.x + 1, p.y + 5);
      case vtHScroll: return TRect(p.x, p.y, p.x + 10, p.y + 1);
      default: return TRect(p.x, p.y, p.x + 10, p.y + 3);
   }
}

/* GetSizeLimits ---------------------------------------------------------*/
// returns the size limits for each type of view
void GetSizeLimits(TPoint& min, TPoint& max, TViewType vt)
{
   max.x = max.y = INT_MAX;
   switch(vt)
   {
      case vtButton: min.x = 5; min.y = 2; break;
      case vtVScroll: min.x = 1; min.y = 3; break;
      case vtHScroll: min.x = 3; min.y = 1; break;
      default: min.x = 3; min.y = 1;
   }
}

/* InitObject ------------------------------------------------------------*/
// returns a valid tview object of type vt
TView * InitObject(TPoint Position, TViewType vt, ushort mode)
{
   TView * rst;
#define _createobj_(o,c) if (mode != 0) c++;\
         rst = new o(DefineRect(vt, Position)); break
#define _createcluster_(o, c) if (mode != 0) c++;\
         rst = new o( DefineRect(vt, Position),\
         new TSItem("FreeDsgn",\
         new TSItem("For",\
         new TSItem("TVision", 0 ))) ); break
   switch(vt)
   {
      case vtLabel: _createobj_(TDLabel, LabelCount);
      case vtInput: _createobj_(TDInputLine, InputCount);
      case vtMemo: _createobj_(TDMemo, MemoCount);
      case vtStatic: _createobj_(TDStaticText, StaticCount);
      case vtButton: _createobj_(TDButton, ButtonCount);
      case vtListBox: _createobj_(TDListBox, ListBoxCount);
      case vtRadioButton: _createcluster_(TDRadioButtons, RadioCount);
      case vtCheckBox: _createcluster_(TDCheckBoxes, CheckCount);
      case vtVScroll: _createobj_(TVScrollBar, VScrollCount);
      case vtHScroll: _createobj_(THScrollBar, HScrollCount);
      default: return 0;
   }
   return rst;
#undef _createobj_
#undef _createcluster_
}

/* DragObject -----------------------------------------------------------*/
// Drags the view from the ToolDlg to EditDlg across the desktop
void DragObject(TView * P, TEvent& Event, TViewType vt)
{
   TRect Limits;
   TPoint Min, Max, mouse;
  
//EditDlg->select();
   if (Event.what == evCommand) FloatingView = P;
   TProgram::deskTop->insert(P);
   Limits = TProgram::deskTop->getExtent();
   mouse = TProgram::deskTop->makeLocal(Event.mouse.where);
   if (!FloatingView) P->moveTo(mouse.x, mouse.y);
   P->state |= sfShadow;
   P->sizeLimits(Min, Max);
   P->dragView(Event, dmDragMove, Limits, Min, Max);
   message(EditDlg, evBroadcast, cmItemDropped, (void *)vt);
   TProgram::deskTop->remove(P);
   FloatingView = NULL;
   delete P;
}

/* ImOwned --------------------------------------------------------------*/

ushort ImOwned(TView * view)
{
   if (view->owner == TProgram::deskTop) return ByDesktop; else
   if (view->owner == EditDlg) return ByEditor; else
   if (view->owner == ToolDlg) return ByToolDlg;
   return 0;
}

/* DsgDragView ----------------------------------------------------------*/
// Move and resize the view v in the EditDlg
void DsgDragView(TView * v, ushort Mode, TEvent& event)
{
   TRect Limits;
   TPoint Min, Max;

   Limits = v->owner->getExtent();
   Limits.grow(-1, -1);
   v->sizeLimits(Min, Max);
   v->dragView(event, v->dragMode | Mode, Limits, Min, Max);
   v->clearEvent(event);
}

/* HandleEventOnEditor --------------------------------------------------*/
// Common event handler for editable views which are owned by EditDlg
bool HandleEventOnEditor(TView * view, TEvent& event,
                         TViewType vt)
{
   TPoint mouse;
   TDsgLink * d;

   if ( (event.what == evMouseDown) &&
        (view->mouseInView(event.mouse.where)) )
   {
      d = ObjectLinker()->viewFind(view);
      if ((d) && ObjEdit->object != d->d) ObjEdit->setObjData(d->d);
      view->select();
      if (event.mouse.buttons == mbRightButton)
      {
         mouse = view->makeLocal(event.mouse.where);
         if ((mouse.x == view->size.x - 1) &&
             (mouse.y == view->size.y - 1))
            DsgDragView(view, dmDragGrow, event);
         else
            DsgDragView(view, dmDragMove, event);
      }
      return false;
   }
   else return false;
}

/* HandleEventOnToolDlg --------------------------------------------------*/
// Common event handler for editable views which are owned by ToolDlg
bool HandleEventOnToolDlg(TView * view, TEvent& event, TViewType vt)
{
   TRect Location;
   TView * P;

   if ( (event.what == evMouseDown) ||
        ( (event.what == evCommand) &&
          (event.message.command == cmResizeObj) &&
          (view->getState(sfFocused|sfSelected)) ) )
   {
      if (event.what == evMouseDown)
         Location.a = view->makeGlobal(event.mouse.where);
      else
      {
         keyPut = true;
         Location.a =
         TProgram::deskTop->makeLocal(view->owner->makeGlobal(view->origin));
      }
      P = InitObject(Location.a, vt, 0);
      if (P) DragObject(P, event, vt);
      return true;
   }
   if (event.what == evKeyboard)
   {
      switch(event.keyDown.keyCode)
      {
         case kbUp:
            view->owner->selectNext(true);
            view->clearEvent(event);
         return true;
         case kbDown:
            view->owner->selectNext(false);
            view->clearEvent(event);
         return true;
         case kbLeft:
            view->owner->selectNext(true);
            view->clearEvent(event);
         return true;
         case kbRight:
            view->owner->selectNext(false);
            view->clearEvent(event);
         return true;
         case kbEnter:
            view->clearEvent(event);
            if (!keyPut)
            {
               keyPut = true;
               message(view->owner, evCommand, cmResizeObj, 0);
            }
            keyPut = false;
         return false;
         default: return false;
      }
   }
   return false;
}

/* InsertLink -----------------------------------------------------------*/
// returns true if a TDLabel was found linked to aView and insert both
bool InsertLink(char * aName, TView * aView, TDialog * aOwner)
{
   TLabel * l;
   
   TDsgLink * dl = ObjectLinker()->linkFind(aName);
   if (dl)
   {
       l = (TLabel *)dl->d->createView(aView);
       aOwner->insert(aView);
       aOwner->insert(l);
       return true;
   } else return false;
}

/* FindScroll -----------------------------------------------------------*/
// Inserts and returns a valid TScrollBar if one named as aScroll was found
TScrollBar * FindScroll(char * aScroll, TDialog * aOwner)
{
   TScrollBar * sb;
   
   TDsgLink * dl = ObjectLinker()->scrollFind(aScroll);
   if (dl)
   {
      sb = (TScrollBar *)dl->d->createView(0);
      aOwner->insert(sb);
      return sb;
   } else return 0;
}

/* StringsToSItems ------------------------------------------------------*/
// Converts a TStringCollection to TSItems
TSItem * strCollToItems(TStringCollection * strs)
{
   TSItem *first, * cur, * last;
   int i;
   
   first = cur = last = 0;
   
   if ((strs) && (strs->getCount() > 0))
   {
      for (i = 0; i < strs->getCount(); i++)
      {
         cur = new TSItem(newStr((char *)strs->at(i)), 0);
         if (!first) first = cur;
         if (last) last->next = cur;
         last = cur;
      }
      return first;
   } else return 0;
}

/* LoadObject ===========================================================*/

void * readDsgInfo( ipstream& s, TViewType vt )
{
   void * rst = malloc(vtAttrSize[vt]);
   s.readBytes(rst, vtAttrSize[vt]);
   return rst;
}

void readStrings( ipstream& s, TCollection * c, int limit )
{
   ushort i, len;
   char ch[100];
   c->freeAll();
   c->setLimit(limit);
   for (i = 0; i < limit; i++)
   {
      s >> len;
      s.readBytes(&ch, len);
      ch[len] = 0;
      c->atInsert(i, newStr(ch));
   }
}

TView * LoadObject( ipstream& s )
{
   TView * rst=0;
   TDsgObj * obj;
   static TViewType vt;
   static int auxVt;
   TStringCollection * items=0;
   ushort Count;
   char * Text;
   TPoint p;
   
   s >> auxVt;
   vt=(TViewType)auxVt;
   if (vt == vtNone) return 0;
   void * attr = readDsgInfo(s, vt);
   if (!attr) return 0;
   p.x = 0; p.y = 0;
   if (vt != vtDialog) rst = InitObject( p, vt, 0 );
   rst->setState(sfEditing, True);
   obj = ObjectLinker()->viewFind(rst)->d;
   memcpy(obj->attributes, attr, vtAttrSize[vt]);
   free(attr);
   if (vt==vtStatic)
     {
      s >> Count;
      if (Count > 0)
      {
         Text = (char *)malloc(Count);
         s.readBytes(Text, Count);
         delete[] ((TDStaticData *)obj->attributes)->text;
         ((TDStaticData *)obj->attributes)->text = Text;
      }
     }
   if (vt>=vtRadioButton && vt<=vtCheckBox)
     {
      s >> Count;
      if (Count > 0 && Count <= 32)
      {
         items = (TStringCollection *)obj->dsgGetData();
         readStrings(s, items, Count);
      } else items->freeAll();
      ((TDClusterData *)obj->attributes)->items = items;
     }
// obj->dsgUpdate();
   return rst;
}

void SaveObject( ofpstream& s, TDsgObj * obj )
{
   ushort Count, i;
   TStringCollection * items;
   char * ch;
   
   s << (int)obj->viewType;
   s.writeBytes(obj->attributes, vtAttrSize[obj->viewType]);
   switch (obj->viewType)
   {
      case vtStatic:
        ch = ((TDStaticData *)obj->attributes)->text;
        Count = strlen(ch);
        if (Count > 0)
        {
           s << Count + 1;
           if (Count > 0) s.writeBytes( ch, Count + 1 );
        }
        else s << Count;
        break;
      case vtRadioButton:
      case vtCheckBox:
        items = (TStringCollection *)((TDClusterData *)obj->attributes)->items;
        Count = items->getCount();
        s << Count;
        if (Count > 0)
          for ( i = 0; i < items->getCount(); i++)
          {
             ch = (char *)items->at(i);
             Count = strlen(ch);
             s << Count;
             s.writeBytes( ch, Count );
          }
       default: return;
   }
}

/* TDFrame ==============================================================*/

TDFrame::TDFrame(const TRect& bounds): TFrame(bounds) { }

void TDFrame::draw()
{
   TDrawBuffer bl, b;
   ushort c, i;
  
   TFrame::draw();

   if (!GridState) return; // Grid mode conditioner
  
   if ((owner->state & sfEditing) != 0)
   {
      for (i = 0; i <= size.x; i++)
      {
         c = ((i + 1) % 10 == 0) ? 0x7e : 0x78;
         b.moveChar(i, 'ú', c, 1);
      }
      bl.moveChar(0, 'ú', 0x7e, size.x - 2);
      for (i = 1; i <= size.y - 1; i++)
      {
         if (i % 10 == 0)
           writeLine(1, i, size.x - 2, 1, bl);
         else
           writeLine(1, i, size.x - 2, 1, b);
      }
      for (i = 1; i <= size.y - 2; i++)
      {
         c = (i % 10 == 0) ? 0x7e : 0x78;
         b.moveChar(0, (i % 10) + 48, c, 1);
         writeLine(0, i, 1, 1, b);
      }
      for (i = 1; i <= size.x; i++)
      {
         c = (i % 10 == 0) ? 0x7e : 0x78;
         b.moveChar(i - 1, (i % 10) + 48, c, 1);
      }
         writeLine(1, size.y - 1, size.x - 2, size.y, b);
   }
}

/* TDsgObj ==============================================================*/

void TDsgObj::buildName(TNameStr * name, int mode)
{
   char fmt[6];
   if (viewType == vtVScroll) strcpy(fmt, "V%s%i"); else
   if (viewType == vtHScroll) strcpy(fmt, "H%s%i"); else strcpy(fmt, "%s%i");
   if (!mode) strcpy((char *)name, TheClassName[viewType]);
   else sprintf((char *)name, fmt,
      (char *)&(TheClassName[viewType][1]), nameIndex);
}

TDsgObj::TDsgObj(TViewType ViewType)
{
#define _allocattr_(t,c) \
         nameIndex = c; \
         attrSize = sizeof(t); \
         attributes = new t; break /* calloc(1, attrSize);
         break; */
//       setViewData((TDsgObjData &)*attributes);
   viewType = ViewType;
   switch (viewType)
   {
      case vtOther: _allocattr_(TDsgObjData, UserCount);
      case vtLabel: _allocattr_(TDLabelData, LabelCount);
      case vtInput: _allocattr_(TDInputData, InputCount);
      case vtMemo: _allocattr_(TDMemoData, MemoCount);
      case vtStatic: _allocattr_(TDStaticData, StaticCount);
      case vtButton: _allocattr_(TDButtonData, ButtonCount);
      case vtListBox: _allocattr_(TDListBoxData, ListBoxCount);
      case vtRadioButton: _allocattr_(TDRadioData, RadioCount);
      case vtCheckBox: _allocattr_(TDCheckData, CheckCount);
      case vtVScroll: _allocattr_(TDVScrollData, VScrollCount);
      case vtHScroll: _allocattr_(TDHScrollData, HScrollCount);
      case vtDialog: _allocattr_(TDDialogData, DialogCount);
      case vtNone: break;
   }
   dsgObj = this;
#undef _allocattr_
}

void TDsgObj::setViewData(TDsgObjData& data)
{
   TView * me = Me();
   
   buildName(&data.className, 0);  // TName
   buildName(&data.thisName, 1);   // Name#
   data.origin = me->origin;
   data.size = me->size;
   defaultData.options = data.options = me->options;
   defaultData.eventMask = data.eventMask = me->eventMask;
   defaultData.state = data.state = me->state;
   defaultData.growMode = data.growMode = me->growMode;
   defaultData.dragMode = data.dragMode = me->dragMode;
   defaultData.helpCtx = data.helpCtx = me->helpCtx;
   
   switch(viewType)
   {
      case vtLabel:
         strcpy( ((TDLabelData &)data).text, (char *)(this->dsgGetData()) );
      break;
      case vtInput:
         ((TDInputData &)data).maxLen = 0;
         ((TDInputData &)data).validatorType = 0;
      break;
      case vtStatic:
         ((TDStaticData &)data).text = (char *)this->dsgGetData();
      break;
      case vtButton:
         strcpy( ((TDButtonData &)data).title, (char *)this->dsgGetData() );
         ((TDButtonData &)data).command =  ((TDButton *)me)->getCommand();
         ((TDButtonData &)data).flags =  ((TDButton *)me)->getFlags();
      break;
      case vtRadioButton:
         ((TDRadioData &)data).items = ((TDRadioButtons *)me)->dsgGetData();
      break;
      case vtCheckBox:
         ((TDCheckData &)data).items = ((TDCheckBoxes *)me)->dsgGetData();
      break;
      case vtDialog:
         strcpy( ((TDDialogData &)data).title, ((TDialog *)me)->title);
         ((TDDialogData &)data).flags =  ((TDialog *)me)->flags;
      default: return;
   }
}

void TDsgObj::setPos(TPoint neworigin, TPoint newsize)
{
  ((TDsgObjData *)attributes)->origin = neworigin;
  ((TDsgObjData *)attributes)->size = newsize;
  ((TDDialog *)Me()->owner)->setModified(True);
}

TDsgObj::~TDsgObj()
{
   if (this!=EditDlg && ObjEdit->object == this) ObjEdit->setObjData(EditDlg);
   ObjectLinker()->removeMe(this); // <- was DsgObj
   delete (char *)attributes;
}

char * TDsgObj::getScript(ushort ScriptType)
{
// criar e retorna um texto script conforme o tipo
 return 0;
}

void TDsgObj::setupView(TView * View)
{
   TDsgObjData *editData = (TDsgObjData *)attributes;
   View->options = editData->options;
   View->growMode = editData->growMode;
   View->dragMode = editData->dragMode;
   View->state = editData->state;
}

bool TDsgObj::tabStop()
{
   return ((viewType != vtLabel &&
            viewType != vtStatic &&
            viewType != vtVScroll &&
            viewType != vtHScroll) ||
           ( ((TDsgObjData *)attributes)->options & ofSelectable ) );
}

void TDsgObj::dsgUpdate()
{
   TView * me = Me();
   TDsgObjData * d = (TDsgObjData *)attributes;
   TRect r = TRect(d->origin.x, d->origin.y,
           d->origin.x + d->size.x, d->origin.y + d->size.y);
   if (r != me->getBounds()) me->changeBounds(r);
   if (tabStop()) ObjectLinker()->doReOrder();
   ((TDDialog *)me->owner)->setModified(True);
   me->owner->CLY_Redraw();
}

/* TDDialog =============================================================*/

TDDialog::TDDialog(): 
             TWindowInit(TDDialog::initFrame),
	     TDialog( TRect(0, 6, 55, 23), "TVISION Dialog Editor"),
             TDsgObj( vtDialog )
{
   setViewData(*((TDsgObjData *)attributes));
   defaultData.flags = flags;
   ObjEdit->setObjData(this);
   flags &= ~wfClose;
   flags |=  wfGrow | wfZoom;
// options |= ofCentered;
   state |= sfEditing;
   EditDlg = this;
   setModified(False);
}

void TDDialog::shutDown()
{
   if (modified) Save(cmCancel);
   if (fileName) delete[] fileName;
   _shutdown_(TDialog);
}

void TDDialog::setModified(Boolean aState)
{
   modified = aState;
}

void TDDialog::handleEvent(TEvent& event)
{
   TView * P;
   TPoint mouse;

   if (event.what == evKeyboard)
   {
      switch(event.keyDown.keyCode)
      {
         case kbUp: selectNext(true); clearEvent(event); break;
         case kbDown: selectNext(false); clearEvent(event); break;
         case kbLeft: selectNext(true); clearEvent(event); break;
         case kbRight: selectNext(false); clearEvent(event); break;
         case kbCtrlDel:
            if (current)
            {
               P = current;
               remove(current);
               delete P;
               clearEvent(event);
            }
         break;
         case kbEnter:
            if (current) DsgDragView(current, dmDragMove|dmDragGrow, event);
            clearEvent(event);
         break;
         default: TDialog::handleEvent(event);
      }
   }
   else
   if (event.what == evBroadcast)
   {
      if (event.message.command == cmItemDropped)
      {
         P = NULL;
         if (FloatingView) { mouse = FloatingView->origin; mouse.y++; }
         else mouse = Mouse->where;
         if ( mouseInView(mouse) && !ToolDlg->mouseInView(mouse) &&
              !ObjEdit->mouseInView(mouse) )
            P = InitObject(makeLocal(mouse),
            (TViewType)event.message.infoLong, sfEditing);
         if (P) dinsert(P);
      }
   }
   TDialog::handleEvent(event);
   if (event.what == evMouseDown) ObjEdit->setObjData(this);
}

void TDDialog::draw() { TDialog::draw(); }

TFrame * TDDialog::initFrame( TRect r ) { return new TDFrame(r); }

void TDDialog::setState(ushort aState, Boolean enable)
{
   TDialog::setState(aState, enable);
}

void doCloneView(void * v, void * d)
{
   TDsgObj * Dsg = ((TDsgLink *)v)->d;
   if (Dsg->viewType == vtLabel ||
       Dsg->viewType == vtVScroll || Dsg->viewType == vtHScroll) return;
   TView * rst = Dsg->createView((TDialog *)d);
   if (rst) ((TDialog *)d)->insert(rst);
}

void TDDialog::dlgRun()
{
   TDDialogData * editData = (TDDialogData *)attributes;
   
   TDialog * d = new TDialog( TRect( getBounds() ), editData->title );
   ObjectLinker()->forEach(&doCloneView, (void *)d);
   setupView(d);
   d->flags = editData->flags;
   hide();
   d->selectNext(false);
   TProgram::deskTop->execView(d);
   changeBounds(d->getBounds());
   TObject::CLY_destroy(d);
   show();
}

void TDDialog::dinsert(TView * aView)
{
   aView->options |= ofSelectable | ofFirstClick | ofPreProcess | ofPostProcess;
   aView->growMode = 0;
   aView->setState(sfEditing, true);
   TDsgLink * dl = ObjectLinker()->viewFind(aView);
   if ((dl) && dl->d->tabStop())
   {
      ((TDsgObjData *)dl->d->attributes)->tabOrder = TabOrder;
      TabOrder++;
   }
   insert(aView);
   setModified(True);
}

void TDDialog::changeBounds(const TRect& bounds) {_chgbnds_(TDialog); }

TView * TDDialog::Me() { return this; }

void TDDialog::dsgUpdate()
{
   TDsgObj::dsgUpdate();
   TDDialogData * d = (TDDialogData *)attributes;
   delete[] title;
   title = newStr(d->title);
   frame->drawView();
   setModified(True);
}

void * TDDialog::dsgGetData() { return (void *)title; }

Boolean TDDialog::Save(int aCommand)
{
   char * f;
   int cmd = aCommand;
   if (modified)
   {
      if (cmd != cmYes)
        cmd = messageBox(__("The current dialog was modified. Save it?"),
              mfYesNoCancel | mfWarning);
      if ( cmd == cmYes && (fileName == 0 || strlen(fileName) == 0) )
      {
         f = getFileName(_("Save dialog"), "*.fdg", 1);
         if (f)
         {
            delete[] fileName;
            fileName = f;
            return True;
         }
      }
      switch (cmd)
      {
          case cmYes: saveToFile(fileName); return True;
          case cmNo: return True;
          case cmCancel: return False;
      }
   } //else return True;
 return True;  
}

static void saveObject(void * v, void * d)
{
   TDsgLink * dsg = (TDsgLink *)v;
#if 0
   char buf[255];
   if (dsg)
   {
      sprintf(buf, "Salvando objeto %s.",
         ((TViewData *)dsg->d->attributes)->thisName);
      messageBox(buf, 0);
      SaveObject((ofpstream &)*d, dsg->d);
   }
#endif
#if 1
   if (dsg) SaveObject(*((ofpstream *)d), dsg->d);
#endif
}

Boolean TDDialog::saveToFile(const char * FileName)
{
    ofpstream * S = initFile(FileName, fileName, dialogFileSig);
    if (S != 0)
    {
        ofpstream& s = *S;
        s << viewType;
        s.writeBytes(attributes, vtAttrSize[viewType]);
        s << (GridState ? (char)1 : (char)0);
        s << LabelCount  << InputCount   << MemoCount
          << StaticCount << ButtonCount  << ListBoxCount << RadioCount
          << CheckCount  << VScrollCount << HScrollCount << UserCount;
        ObjectLinker()->sortForBuild();
        ObjectLinker()->forEach(&saveObject, &s);
        s << vtNone;
        s.close();
        delete S;
        return True;
        setModified(False);
    } else return False;
}

Boolean TDDialog::loadFromFile(const char * FileName)
{
   static int vtAux;
   static TViewType vt;
   
   if (!Save(cmYes)) return False;
   
   if (!FileName) return False;
   
   if (fileName)
   {
      if (strcmp(FileName, fileName) == 0) return True;
      delete[] fileName;
   }
   fileName = newStr(FileName);
     
   ifpstream * S = openFile(FileName, dialogFileSig);
   TView * v;
   TView * p = last;
   do {
      TView * t = p->prev();
      if (p != frame) CLY_destroy(p);
      p = t;
   } while (first() != last);
   
   if (S != 0)
   {
      ifpstream& s = *S;
      s >> vtAux; vt = (TViewType)vtAux;
      if (vt != vtDialog) { s.close(); return False; }
      void * attr = readDsgInfo(s, vt);
      memcpy(attributes, attr, vtAttrSize[vt]);
      free(attr);
      char aux;
      s >> aux; GridState=aux ? True : False;
      s >> LabelCount  >> InputCount   >> MemoCount
        >> StaticCount >> ButtonCount  >> ListBoxCount >> RadioCount
        >> CheckCount  >> VScrollCount >> HScrollCount >> UserCount;
      dsgUpdate();
      TabOrder = 0;
      v = LoadObject(s);
      while (v)
      {
         dinsert(v);
         TDsgLink * dsg = ObjectLinker()->viewFind(v);
         if (dsg) dsg->d->dsgUpdate();
         v = LoadObject(s);
      }
      if (fileName) delete[] fileName;
      fileName = strdup(FileName);
      selectNext(True);
      setModified(False);
      s.close();
      delete S;
   } //else return False;
 return False;  
}

/* TDLabel ==============================================================*/

TDLabel::TDLabel(const TRect& bounds):
         TLabel(bounds, __("~L~abel"), EditDlg),
         TDsgObj( vtLabel )
         { setViewData(*((TDsgObjData *)attributes)); }

void TDLabel::shutDown() { _shutdown_(TLabel); }

void TDLabel::handleEvent(TEvent& event) {_h_(TLabel); }

TPalette& TDLabel::getPalette() const
  {_p_(cpDragLabel, cpSelLabel, TLabel); }

void TDLabel::sizeLimits(TPoint& min, TPoint& max)
  { GetSizeLimits(min, max, viewType); }

void TDLabel::setState(ushort aState, Boolean enable) {_setstate_(TLabel); }
  
void TDLabel::changeBounds(const TRect& bounds) {_chgbnds_(TLabel); }

TView * TDLabel::Me() { return this; }

void TDLabel::dsgUpdate()
{
  TDLabelData * d = (TDLabelData *)attributes;
  delete[] text;
  text = newStr(d->text);
//  drawView();
  TDsgObj::dsgUpdate();
}

void * TDLabel::dsgGetData() { return (void *)text; }

TView * TDLabel::createView(TView * aLink)
{
   TDLabelData * d = (TDLabelData *)attributes;
   TLabel * rst = new TLabel(getBounds(), (char *)d->text, aLink);
   setupView(rst);
   return rst;
}

/* TDInputLine ==========================================================*/

TDInputLine::TDInputLine(const TRect& bounds):
    TInputLine(bounds, 0),
    TDsgObj( vtInput )
    { setViewData(*((TDsgObjData *)attributes)); }

void TDInputLine::shutDown() { _shutdown_(TInputLine); }

void TDInputLine::handleEvent(TEvent& event) {_h_(TInputLine); }

TPalette& TDInputLine::getPalette() const
  {_p_(cpDragInput, cpSelInput, TInputLine); }

void TDInputLine::sizeLimits(TPoint& min, TPoint& max)
  { GetSizeLimits(min, max, viewType); }

void TDInputLine::setState(ushort aState, Boolean enable)
  {_setstate_(TInputLine); }

void TDInputLine::changeBounds(const TRect& bounds) {_chgbnds_(TInputLine); }

TView * TDInputLine::Me() { return this; }

TView * TDInputLine::createView(TView * aOwner)
{
   TDInputData * d = (TDInputData *)attributes;
   TInputLine * rst = new TInputLine(getBounds(), d->maxLen);
// if (d->validatorType != vtNone)
//   switch(d->validatorType)
//   {
//
//   }
   setupView(rst);
   if (InsertLink(d->thisName, rst, (TDialog *)aOwner))
      return 0;
   else
      return rst;
}

/* TDMemo ===============================================================*/

TDMemo::TDMemo(const TRect& bounds):
         TMemo(bounds, 0, 0, 0, 0),
         TDsgObj( vtMemo )
         { setViewData(*((TDsgObjData *)attributes)); }

void TDMemo::shutDown() { _shutdown_(TMemo); }

void TDMemo::handleEvent(TEvent& event) {_h_(TMemo); }

TPalette& TDMemo::getPalette() const {_p_(cpDragMemo, cpSelMemo, TMemo); }

void TDMemo::sizeLimits(TPoint& min, TPoint& max)
  { GetSizeLimits(min, max, viewType); }

void TDMemo::setState(ushort aState, Boolean enable) {_setstate_(TMemo); }

void TDMemo::changeBounds(const TRect& bounds) {_chgbnds_(TMemo); }

TView * TDMemo::Me() { return this; }

TView * TDMemo::createView(TView * aOwner)
{
   TDMemoData * d = (TDMemoData *)attributes;
   TScrollBar * vsb = 0;
   TScrollBar * hsb = 0;
   if (d->hScroll >= 0) hsb = FindScroll(d->hScroll, (TDialog *)aOwner);
   if (d->vScroll >= 0) vsb = FindScroll(d->vScroll, (TDialog *)aOwner);
   TMemo * rst = new TMemo(getBounds(), hsb, vsb, 0, d->bufSize);
   setupView(rst);
   if (InsertLink(d->thisName, rst, (TDialog *)aOwner))
      return 0;
   else
      return rst;
}

/* TDStaticText =========================================================*/

TDStaticText::TDStaticText(const TRect& bounds):
              TStaticText(bounds, "StaticText"),
              TDsgObj( vtStatic )
              { setViewData(*((TDsgObjData *)attributes)); }

void TDStaticText::shutDown() { _shutdown_(TStaticText); }

void TDStaticText::handleEvent(TEvent& event) {_h_(TStaticText); }

TPalette& TDStaticText::getPalette() const
  {_p_(cpDragStatic, cpSelStatic, TStaticText); }

void TDStaticText::sizeLimits(TPoint& min, TPoint& max)
  { GetSizeLimits(min, max, viewType); }

void TDStaticText::setState(ushort aState, Boolean enable)
  {_setstate_(TStaticText); }

void TDStaticText::changeBounds(const TRect& bounds) {_chgbnds_(TStaticText); }

TView * TDStaticText::Me() { return this; }

void TDStaticText::dsgUpdate()
{
   TDStaticData * d = (TDStaticData *)attributes;
//  delete[] text;
//  text = newStr(d->text);
   text = d->text; // If was changed the text has been freed by the object
                   // editor and the new pointer is in attributes data
   TDsgObj::dsgUpdate();
//  drawView();
}

void * TDStaticText::dsgGetData() { return (void *)text; }

TView * TDStaticText::createView(TView * aOwner)
{
   TDStaticData * d = (TDStaticData *)attributes;
   TStaticText * rst = new TStaticText(getBounds(), d->text);
   setupView(rst);
   return rst;
}

/* TDButton =============================================================*/

TDButton::TDButton(const TRect& bounds):
          TButton(bounds, "~O~k", 0, 0),
          TDsgObj( vtButton )
          { setViewData(*((TDsgObjData *)attributes)); }

void TDButton::shutDown() { _shutdown_(TButton); }

void TDButton::handleEvent(TEvent& event) {_h_(TButton); }

TPalette& TDButton::getPalette() const
  {_p_(cpDragBtn, cpSelBtn, TButton); }

void TDButton::sizeLimits(TPoint& min, TPoint& max)
  { GetSizeLimits(min, max, viewType); }

void TDButton::setState(ushort aState, Boolean enable) {_setstate_(TButton); }

void TDButton::changeBounds(const TRect& bounds) {_chgbnds_(TButton); }

TView * TDButton::Me() { return this; }

void TDButton::dsgUpdate()
{
  TDButtonData * d = (TDButtonData *)attributes;
  delete[] title;
  title = newStr(d->title);
  TDsgObj::dsgUpdate();
//  drawView();
}

void * TDButton::dsgGetData() { return (void *)title; }

ushort TDButton::getCommand() { return command; }
ushort TDButton::getFlags() { return flags; }

TView * TDButton::createView(TView * aOwner)
{
   TDButtonData * d = (TDButtonData *)attributes;
   TButton * rst = new TButton(getBounds(), d->title, d->command, d->flags);
   setupView(rst);
   return rst;
}

/* TDListBox ============================================================*/

TDListBox::TDListBox(const TRect& bounds):
           TListBox(bounds, 1, 0),
           TDsgObj( vtListBox )
           { setViewData(*((TDsgObjData *)attributes)); }

void TDListBox::shutDown() { _shutdown_(TListBox); }

void TDListBox::handleEvent(TEvent& event) {_h_(TListBox); }

TPalette& TDListBox::getPalette() const
  {_p_(cpDragLB, cpSelLB, TListBox); }

void TDListBox::sizeLimits(TPoint& min, TPoint& max)
  { GetSizeLimits(min, max, viewType); }

void TDListBox::setState(ushort aState, Boolean enable) {_setstate_(TListBox); }

void TDListBox::changeBounds(const TRect& bounds) {_chgbnds_(TListBox); }

TView * TDListBox::Me() { return this; }

void TDListBox::dsgUpdate()
{
// setRange(items->getCount());
// if (items->getCount() > 0) focusItem(0);
// TDsgObj::dsgUpdate();
   drawView();
}

void * TDListBox::dsgGetData() { return items; }

TView * TDListBox::createView(TView * aOwner)
{
   TDListBoxData * d = (TDListBoxData *)attributes;
   TScrollBar * sb = 0;
   int cols = (d->columns > 0) ? d->columns : 1;
   if (d->scrollBar >= 0) sb = FindScroll(d->scrollBar, (TDialog *)aOwner);
   TListBox * rst = new TListBox(getBounds(), cols, sb);
// rst->newList(items);
   setupView(rst);
   if (InsertLink(d->thisName, rst, (TDialog *)aOwner))
      return 0;
   else
      return rst;
}

/* TDRadioButtons =======================================================*/

TDRadioButtons::TDRadioButtons(const TRect& bounds, TSItem * aStrings):
                TRadioButtons(bounds, aStrings),
                TDsgObj( vtRadioButton )
                { setViewData(*((TDsgObjData *)attributes)); }

void TDRadioButtons::shutDown() { _shutdown_(TRadioButtons); }

void TDRadioButtons::handleEvent(TEvent& event) {_h_(TRadioButtons); }

TPalette& TDRadioButtons::getPalette() const
  {_p_(cpDragCluster, cpSelCluster, TCluster); }

void TDRadioButtons::sizeLimits(TPoint& min, TPoint& max)
  { GetSizeLimits(min, max, viewType); }

void TDRadioButtons::setState(ushort aState, Boolean enable)
  {_setstate_(TRadioButtons); }

void TDRadioButtons::changeBounds(const TRect& bounds)
  {_chgbnds_(TRadioButtons); }

TView * TDRadioButtons::Me() { return this; }

void * TDRadioButtons::dsgGetData() { return strings; }

TView * TDRadioButtons::createView(TView * aOwner)
{
   TDRadioData * d = (TDRadioData *)attributes;
   TRadioButtons * rst = new TRadioButtons(getBounds(), strCollToItems(strings));
   rst->setData(&value);
   setupView(rst);
   if (InsertLink(d->thisName, rst, (TDialog *)aOwner))
      return 0;
   else
      return rst;
}

/* TDCheckBoxes =========================================================*/

TDCheckBoxes::TDCheckBoxes(const TRect& bounds, TSItem * aStrings):
              TCheckBoxes(bounds, aStrings),
              TDsgObj( vtCheckBox )
              { setViewData(*((TDsgObjData *)attributes)); }

void TDCheckBoxes::shutDown() { _shutdown_(TCheckBoxes); }

void TDCheckBoxes::handleEvent(TEvent& event) {_h_(TCheckBoxes); }

TPalette& TDCheckBoxes::getPalette() const
  {_p_(cpDragCluster, cpSelCluster, TCluster); }

void TDCheckBoxes::sizeLimits(TPoint& min, TPoint& max)
  { GetSizeLimits(min, max, viewType); }

void TDCheckBoxes::setState(ushort aState, Boolean enable)
  { _setstate_(TCheckBoxes); }
  
void TDCheckBoxes::changeBounds(const TRect& bounds) {_chgbnds_(TCheckBoxes); }

TView * TDCheckBoxes::Me() { return this; }

//void TDCheckBoxes::dsgUpdate() { TDsgObj::dsgUpDate(); }

void * TDCheckBoxes::dsgGetData() { return strings; }

TView * TDCheckBoxes::createView(TView * aOwner)
{
   TDCheckData * d = (TDCheckData *)attributes;
   TCheckBoxes * rst = new TCheckBoxes(getBounds(), strCollToItems(strings));
   rst->setData(&value);
   setupView(rst);
   if (InsertLink(d->thisName, rst, (TDialog *)aOwner))
      return 0;
   else
      return rst;
}

/* TVScrollBar ==========================================================*/

TVScrollBar::TVScrollBar(const TRect& bounds):
             TScrollBar(bounds),
             TDsgObj( vtVScroll )
             { setViewData(*((TDsgObjData *)attributes)); }

void TVScrollBar::shutDown() { _shutdown_(TScrollBar); }

void TVScrollBar::handleEvent(TEvent& event) {_h_(TScrollBar); }

TPalette& TVScrollBar::getPalette() const
  {_p_(cpDragScroll, cpSelScroll, TScrollBar); }

void TVScrollBar::sizeLimits(TPoint& min, TPoint& max)
  { GetSizeLimits(min, max, viewType); }

void TVScrollBar::setState(ushort aState, Boolean enable)
  {_setstate_(TScrollBar); }

void TVScrollBar::changeBounds(const TRect& bounds) {_chgbnds_(TScrollBar); }

TView * TVScrollBar::Me() { return this; }

TView * TVScrollBar::createView(TView * aOwner)
{
   TScrollBar * rst = new TScrollBar(getBounds());
   setupView(rst);
   return rst;
}

/* THScrollBar ==========================================================*/

THScrollBar::THScrollBar(const TRect& bounds):
             TScrollBar(bounds),
             TDsgObj( vtHScroll )
             { setViewData(*((TDsgObjData *)attributes)); }

void THScrollBar::shutDown() { _shutdown_(TScrollBar); }

void THScrollBar::handleEvent(TEvent& event) {_h_(TScrollBar); }

TPalette& THScrollBar::getPalette() const
  {_p_(cpDragScroll, cpSelScroll, TScrollBar); }

void THScrollBar::sizeLimits(TPoint& min, TPoint& max)
  { GetSizeLimits(min, max, viewType); }

void THScrollBar::setState(ushort aState, Boolean enable)
  {_setstate_(TScrollBar); }

void THScrollBar::changeBounds(const TRect& bounds) {_chgbnds_(TScrollBar); }

TView * THScrollBar::Me() { return this; }

TView * THScrollBar::createView(TView * aOwner)
{
   TScrollBar * rst = new TScrollBar(getBounds());
   setupView(rst);
   return rst;
}

/* Fim ==================================================================*/

#undef _h_
#undef _p_
#undef _chgbnds_
#undef _setstate_
