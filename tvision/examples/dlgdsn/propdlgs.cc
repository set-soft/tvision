/***************************************************************************

    propdlgs.cc - Implements the object property dialog editor
    ---------------------------------------------------------------------
    May, 2000
    Copyright (C) 2000 by Warlei Alves
    walves@usa.net
    
    Modified by Salvador E. Tropea to compile without warnings.
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
 
#include <string.h>
#include <stdio.h>

#define Uses_TDialog
#define Uses_TRect
#define Uses_TCheckBoxes
#define Uses_TButton
#define Uses_TView
#define Uses_TLabel
#define Uses_TSItem
#define Uses_TCluster
#define Uses_TRadioButtons
#define Uses_TScrollBar
#define Uses_MsgBox
#define Uses_TStringCollection
#define Uses_TMemo

#include <tv.h>

#include "consted.h"
#include "propedit.h"

// String makers

static char strbuf[1024];

#define _appendstr_(a) do { if (strlen(strbuf)) strcat(strbuf, " | ");\
            strcat(strbuf, a); } while(0)
/*
void _appendstr_(const char * s)
{
  strcat(strbuf, " | ", strcat(strbuf, s));
}
*/
char * OptionsStr(ushort aOptions)
{
   if (aOptions == 0) { strcpy(strbuf, "<none>"); return strbuf; }
   strcpy(strbuf, "");
   if (aOptions & ofSelectable) _appendstr_("ofSelectable");
   if (aOptions & ofTopSelect) _appendstr_("ofTopSelect");
   if (aOptions & ofFirstClick) _appendstr_("ofFirstClick");
   if (aOptions & ofFramed) _appendstr_("ofFramed");
   if (aOptions & ofPreProcess) _appendstr_("ofPreProcess");
   if (aOptions & ofPostProcess) _appendstr_("ofPostProcess");
   if (aOptions & ofBuffered) _appendstr_("ofBuffered");
   if (aOptions & ofTileable) _appendstr_("ofTileable");
   if ((aOptions & ofCentered) == ofCentered)
     _appendstr_("ofCentered");
   else
   {
     if (aOptions & ofCenterX) _appendstr_("ofCenterX");
     if (aOptions & ofCenterY) _appendstr_("ofCenterY");
   }
   return strbuf;
}

char * EventMaskStr(ushort aEventMask)
{
   if (aEventMask == 0) { strcpy(strbuf, "evNothing"); return strbuf; }
   strcpy(strbuf, "");
   if ((aEventMask & evMouse) == evMouse) _appendstr_("evMouse");
   else
   {
     if (aEventMask & evMouseDown) _appendstr_("evMouseDown");
     if (aEventMask & evMouseUp) _appendstr_("evMouseUp");
     if (aEventMask & evMouseMove) _appendstr_("evMouseMove");
     if (aEventMask & evMouseAuto) _appendstr_("evMouseAuto");
   }
   if (aEventMask & evKeyboard) _appendstr_("evKeyboard");
   if (aEventMask & evCommand) _appendstr_("evCommand");
   if (aEventMask & evBroadcast) _appendstr_("evBroadcast");
   return strbuf;
}

char * StateStr(ushort aState)
{
   if (aState == 0) { strcpy(strbuf, "<none>"); return strbuf; }
   strcpy(strbuf, "");
   if (aState & sfVisible) _appendstr_("sfVisible");
   if (aState & sfShadow) _appendstr_("sfShadow");
   if (aState & sfDisabled) _appendstr_("sfDisabled");
   if (aState & sfModal) _appendstr_("sfModal");
   if (aState & sfDefault) _appendstr_("sfDefault");
   return strbuf;
}

char * GrowModeStr(ushort aGrowMode)
{
   if (aGrowMode == 0) { strcpy(strbuf, "<none>"); return strbuf; }
   strcpy(strbuf, "");
   if ((aGrowMode & gfGrowAll) == gfGrowAll) _appendstr_("gfGrowAll");
   else
   {
     if (aGrowMode & gfGrowLoX) _appendstr_("gfGrowLoX");
     if (aGrowMode & gfGrowLoY) _appendstr_("gfGrowLoY");
     if (aGrowMode & gfGrowHiX) _appendstr_("gfGrowHiX");
     if (aGrowMode & gfGrowHiY) _appendstr_("gfGrowHiY");
   }
   if (aGrowMode & gfGrowRel) _appendstr_("gfGrowRel");
   return strbuf;
}

char * DragModeStr(ushort aDragMode)
{
   if (aDragMode == 0) { strcpy(strbuf, "<none>"); return strbuf; }
   strcpy(strbuf, "");
   if (aDragMode & dmDragMove) _appendstr_("dmDragMove");
   if (aDragMode & dmDragGrow) _appendstr_("dmDragGrow");
   if ((aDragMode & dmLimitAll) == dmLimitAll)
      _appendstr_("dmLimitAll");
   else
   {
     if (aDragMode & dmLimitLoX) _appendstr_("dmLimitLoX");
     if (aDragMode & dmLimitLoY) _appendstr_("dmLimitLoY");
     if (aDragMode & dmLimitHiX) _appendstr_("dmLimitHiX");
     if (aDragMode & dmLimitHiY) _appendstr_("dmLimitHiY");
   }
   return strbuf;
}

char * HelpCtxStr(ushort aHelpCtx)
{
   return HelpCtxsList()->getId(aHelpCtx);
}

char * CommandStr(ushort aCommand)
{
   return CommandsList()->getId(aCommand);
}

char * ButtonFlagStr(ushort aButtonFlag)
{
   strcpy(strbuf, "bfNormal");
   if (aButtonFlag == 0) return strbuf; else strcpy(strbuf, "");
   if (aButtonFlag & bfDefault) _appendstr_("bfDefault");
   if (aButtonFlag & bfLeftJust) _appendstr_("bfLeftJust");
   if (aButtonFlag & bfBroadcast) _appendstr_("bfBroadcast");
   return strbuf;
}

char * WindowFlagStr(ushort aWindowFlag)
{
   if (aWindowFlag == 0) { strcpy(strbuf, "<none>"); return strbuf; }
   strcpy(strbuf, "");
   if (aWindowFlag & wfMove) _appendstr_("wfMove");
   if (aWindowFlag & wfGrow) _appendstr_("wfGrow");
   if (aWindowFlag & wfClose) _appendstr_("wfClose");
   if (aWindowFlag & wfZoom) _appendstr_("wfZoom");
   return strbuf;
}

char * ValidatorStr(ushort aValidator)
{
   strcpy(strbuf, "");
   switch(aValidator)
   {
      case 0: strcpy(strbuf, "<none>"); break;
      case 1: strcpy(strbuf, "vtUserDefined"); break;
      case 2: strcpy(strbuf, "vtRangeValidator"); break;
      case 3: strcpy(strbuf, "vtFilterValidator"); break;
      case 4: strcpy(strbuf, "vtPictureValidator"); break;
      default: strcpy(strbuf, "<invalid>");
   }
   return strbuf;
}

char * ItemsStr(void * aItemPtr)
{
  if (aItemPtr) sprintf(strbuf, "%s (%p)", "(void *)", aItemPtr);
   else sprintf(strbuf, "%s", "<null>");
  return strbuf;
}

char * CharPtrStr(void * aItemPtr)
{
  if (aItemPtr) sprintf(strbuf, "%s (%p)", "(char *)", aItemPtr);
   else sprintf(strbuf, "%s", "<null>");
  return strbuf;
}

#undef _appendstr_

// Properties editors

TDialog * SelDialog(const char * Title, TView * c)
{
  int h = c->size.y;
  
  TDialog * d = new TDialog( TRect(1, 1, 26, 6 + h), Title );
  
  d->options |= ofCentered;
  d->flags &= ~wfClose;
  c->options |= ofFramed;
  d->insert(c);
  d->insert( new TButton( TRect(1, d->size.y - 3, 12, d->size.y - 1),
            __("~O~k"), cmOK, bfDefault ) );
  d->insert( new TButton( TRect(12, d->size.y - 3, 23, d->size.y - 1),
            __("~C~ancel"), cmCancel, 0 ) );
  d->selectNext(False);
  return d;
}

TDialog * OptionsEditor()
{
  return SelDialog(_("Options"),
   new TCheckBoxes( TRect( 1, 1, 24, 12), new TSItem("ofSelectable",
                                          new TSItem("ofTopSelect",
                                          new TSItem("ofFirstClick",
                                          new TSItem("ofFramed",
                                          new TSItem("ofPreProcess",
                                          new TSItem("ofPostProcess",
                                          new TSItem("ofBuffered",
                                          new TSItem("ofTileable",
                                          new TSItem("ofCenterX",
                                          new TSItem("ofCenterY",
                                          new TSItem("ofBeVerbose", 0
                                          )))))))))))) );
}

TDialog * EventMaskEditor()
{
  return SelDialog(_("Event masks"),
   new TCheckBoxes( TRect( 1, 1, 24, 8), new TSItem("evMouseDown",
                                         new TSItem("evMouseUp",
                                         new TSItem("evMouseMove",
                                         new TSItem("evMouseAuto",
                                         new TSItem("evKeyDown",
                                         new TSItem("evCommand",
                                         new TSItem("evBroadcast", 0
                                         )))))))) );
}

TDialog * StateEditor()
{
  TCheckBoxes * t =
  new TCheckBoxes( TRect( 1, 1, 24, 13), new TSItem("sfVisible",      //0
                                         new TSItem("sfCursorVis",    //1
                                         new TSItem("sfCursorIns",    //2
                                         new TSItem("sfShadow",       //3
                                         new TSItem("sfActive",       //4
                                         new TSItem("sfSelected",     //5
                                         new TSItem("sfFocused",      //6
                                         new TSItem("sfDragging",     //7
                                         new TSItem("sfDisabled",     //8
                                         new TSItem("sfModal",        //9
                                         new TSItem("sfDefault",      //10
                                         new TSItem("sfExposed", 0    //11
                                         )))))))))))));
#if 0
  t->setButtonState(5, false);
  t->setButtonState(6, false);
  t->setButtonState(7, false);
//t->setButtonState(9, false);
  t->setButtonState(11, false);
#endif
  return SelDialog(_("TView States"), t);
}

TDialog * GrowModeEditor()
{
  return SelDialog(_("Grow modes"),
  new TCheckBoxes( TRect( 1, 1, 24, 6), new TSItem("gfGrowLoX",
                                        new TSItem("gfGrowLoY",
                                        new TSItem("gfGrowHiX",
                                        new TSItem("gfGrowHiY",
                                        new TSItem("gfGrowRel", 0
                                        )))))) );

}

TDialog * DragModeEditor()
{
  return SelDialog(_("Drag modes"),
  new TCheckBoxes( TRect( 1, 1, 24, 7), new TSItem("dmDragMove",
                                        new TSItem("dmDragGrow",
                                        new TSItem("dmLimitLoX",
                                        new TSItem("dmLimitLoY",
                                        new TSItem("dmLimitHiX",
                                        new TSItem("dmLimitHiY", 0
                                        ))))))) );
}

TDialog * DlgRefList(const char * aTitle)
{
   TRect r;
   TScrollBar * sb = new TScrollBar( TRect(30, 1, 31, 10) );
   TIntConstListBox * l = new TIntConstListBox( TRect( 1, 1, 31, 10), 1, sb );
   TDialog * d = SelDialog(aTitle, l);
   r = d->getBounds();
   r.b.x += 7;
   d->changeBounds(r);
   d->insert(sb);
   return d;
}

TDialog * HelpCtxEditor()
{
   return DlgRefList(_("Help contexts"));
}

TDialog * CommandEditor()
{
   return DlgRefList(_("Commands"));
}

TDialog * DlgObjList(const char * aTitle)
{
   TRect r;
   TScrollBar * sb = new TScrollBar( TRect(30, 1, 31, 10) );
   TListBox * l = new TListBox( TRect( 1, 1, 31, 10), 1, sb );
   TDialog * d = SelDialog(aTitle, l);
   r = d->getBounds();
   r.b.x += 7;
   d->changeBounds(r);
   d->insert(sb);
   return d;
}

TDialog * LinkEditor()
{
   return DlgObjList(_("Avail. views"));
}

TDialog * ScrollEditor()
{
   return DlgObjList(_("Avail. scrolls"));
}

TDialog * ButtonFlagEditor()
{
  return SelDialog(_("TButton flags"),
  new TCheckBoxes( TRect( 1, 1, 24, 4), new TSItem("bfDefault",
                                        new TSItem("bfLeftJust",
                                        new TSItem("bfBroadcast", 0
                                        )))) );
}

TDialog * WindowFlagEditor()
{
  return SelDialog(_("TWindow flags"),
  new TCheckBoxes( TRect( 1, 1, 24, 5), new TSItem("wfMove",
                                        new TSItem("wfGrow",
                                        new TSItem("wfClose",
                                        new TSItem("wfZoom", 0
                                        ))))) );
}

TDialog * ValidatorEditor()
{
  return SelDialog(_("TValidators"),
  new TRadioButtons( TRect( 1, 1, 24, 6), new TSItem("<none>",
                                          new TSItem("vtUserDefined",
                                          new TSItem("vtRangeValidator",
                                          new TSItem("vtFilterValidator",
                                          new TSItem("vtPictureValidator", 0
                                          )))))) );
}

class ipstream;
class opstream;

// TItemsEditor

class TSafeListBox: public TListBox
{
public:
   TSafeListBox(const TRect& bounds, ushort aNumCols, TScrollBar * sb):
     TListBox(bounds, aNumCols, sb) { };
   void getText(char *dest, ccIndex item, short maxLen)
   {
      if (strlen((char *)items->at(item)) > (size_t)maxLen)
      {
         strncpy(dest, (char *)items->at(item), maxLen);
         dest[maxLen] = 0;
      } else strcpy(dest, (char *)items->at(item));
   }
};

class TItemsEditor: public TDialog
{
   TStringCollection * cList;
   TSafeListBox * List;
public:
   TItemsEditor();
   virtual void setData(void *);
   virtual void getData(void *) { };
   virtual void handleEvent(TEvent& event);
};

const ushort
  cmEdit   = 200,
  cmAdd    = 201,
  cmDel    = 202,
  cmCClear = 203;

TItemsEditor::TItemsEditor():
            TWindowInit(&TItemsEditor::initFrame),
            TDialog( TRect(0, 0, 45, 16) , __("Items") )
{
   TScrollBar * sb;
   
   options |= ofCentered;
   
   sb = new TScrollBar(TRect(30, 2, 31, 14));
   List = new TSafeListBox(TRect(2, 2, 30, 14), 1, sb);
   insert(sb);
   insert(new TLabel( TRect(1, 1, 15, 2), __("~I~tems list:"), List ) );
   insert(List);
   insert( new TButton(TRect(31, 2, 43, 4), __("~O~k"), cmOK, 0 ) );
   insert( new TButton(TRect(31, 4, 43, 6), __("~C~ancel"), cmCancel, 0 ) );
   insert( new TButton(TRect(31, 6, 43, 8), __("~E~dit"), cmEdit, bfBroadcast ) );
   insert( new TButton(TRect(31, 8, 43, 10), __("~A~dd"), cmAdd,  bfBroadcast ) );
   insert( new TButton(TRect(31, 10, 43, 12), __("C~l~ear"), cmCClear, bfBroadcast ) );
   insert( new TButton(TRect(31, 12, 43, 14), __("~D~elete"), cmDel, bfBroadcast ) );
   cList = 0;
   selectNext(False);
}

void TItemsEditor::setData(void * data)
{
   TListBoxRec rec;

   cList = (TStringCollection *)data;
   rec.items = cList;
   rec.selection = 0;
   List->setData(&rec);
}


void TItemsEditor::handleEvent(TEvent& event)
{
   char buf[50];
   char * c;
   TDialog::handleEvent(event);
   
   if (!cList) return;
   
   if (event.what == evBroadcast)
   switch(event.message.command)
   {
      case cmListItemSelected:
         if ( (cList->getCount() > 0) && (List->focused >= 0) )
         {
            c = (char *)(cList->at(List->focused));
            strcpy(buf, c);
            List->drawView();
            if (inputBox(_("Item Edit"), _("Item:"), buf, 50) == cmOK)
            {
               delete[] c;
               cList->atPut(List->focused, newStr(buf));
               List->drawView();
            }
         }
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
            event.message.command = cmListItemSelected;
            putEvent(event);
            clearEvent(event);
         }
         break;
      case cmAdd:
         buf[0] = 0;
         if (inputBox(_("Item Add"), _("Item:"), buf, 50) == cmOK)
         {
            cList->setLimit(cList->getCount() + 1);
            cList->atInsert(cList->getCount(), newStr(buf));
            List->setRange(List->range + 1);
            List->focusItem( cList->getCount() - 1 );
            List->drawView();
         }
         clearEvent(event);
         break;
      case cmDel:
         if ( messageBox(__("Are you sure ?"), mfYesNoCancel) == cmYes )
         {
            cList->atFree(List->focused);
            cList->pack();
            clearEvent(event);
            List->setRange(List->range - 1);
            List->drawView();
         }
         break;
      case cmCClear:
         if ( messageBox(__("Are you sure ?"), mfYesNoCancel) == cmYes )
         {
            cList->freeAll();
            List->setRange(0);
            List->drawView();
         }
         clearEvent(event);
         break;
   }
}

TDialog * ItemsEditor()
{
   return new TItemsEditor();
}

TDialog * CharPtrEditor()
{
   TDialog * d = new TDialog( TRect(0, 0, 40, 20), __("Text editor") );
   TScrollBar * VS = new TScrollBar( TRect(38, 1, 39, 15) );
   TScrollBar * HS = new TScrollBar( TRect(1, 15, 38, 16) );
   d->insert( new TMemo( TRect(1, 1, 39, 15), VS, HS, 0, 1024 ) );
   d->insert(VS);
   d->insert(HS);
   d->insert( new TButton ( TRect(17, 17, 27, 19), __("~O~k"), cmOK, bfDefault ) );
   d->insert( new TButton ( TRect(28, 17, 38, 19), __("~C~ancel"), cmCancel, 0 ) );
   d->selectNext(False);
   d->options |= ofCentered;
   return d;
}

