/***************************************************************************

    codebldr.cc - Code builder?
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

#include <string.h>
#include <stdio.h>
#include "dsgobjs.h"
#include "propedit.h"
#include "propdlgs.h"
#include "dsgdata.h"
#include "consted.h"

#define _commomget_(a, b, c) \
   ushort a = ((TViewData *)aObj->attributes)->a;\
   if (a != aObj->defaultData.a)\
      return buildStr(ctrlName, c, aObj->defaultData.a, a, &b);\
   else return blank\

static char * blank = "";

static int scrollCount = 0;

typedef char * (*ccGetString)(ushort);

const char * buildStr(const char * ctrlname,
                      const char * alabel,
                      ushort Default,
                      ushort value,
                      ccGetString f)
{
   static char buf1[255];
   static char buf2[100];
   strcpy(buf1, "");
   strcpy(buf2, "");
   if (value == Default) return blank;
   if ((value & ~Default) != 0)
      sprintf(buf1, "   %s->%s |= (%s);\n", ctrlname, alabel, f(value &~ Default));
   if ((Default & ~value) != 0)
      sprintf(buf2, "   %s->%s &= ~(%s);\n", ctrlname, alabel, f(Default &~ value));
   return strcat(buf1, buf2);
}

const char * getOptions(TDsgObj * aObj, char * ctrlName)
{
   _commomget_(options, OptionsStr, "options");
}

const char * getEventMask(TDsgObj * aObj, char * ctrlName)
{
   _commomget_(eventMask, EventMaskStr, "eventMask");
}

const char * getState(TDsgObj * aObj, char * ctrlName)
{
   _commomget_(state, StateStr, "state");
}

const char * getFlags(TDsgObj * aObj, char * ctrlName)
{
   if (aObj->viewType != vtDialog) return blank;
   ushort flags = ((TDDialogData *)aObj->attributes)->flags;
   if (flags != aObj->defaultData.flags)
      return buildStr(ctrlName, "flags",
          aObj->defaultData.flags, flags, &WindowFlagStr);
   else return blank;
}

const char * getGrowMode(TDsgObj * aObj, char * ctrlName)
{
   _commomget_(growMode, GrowModeStr, "growMode");
}

const char * getDragMode(TDsgObj * aObj, char * ctrlName)
{
   _commomget_(dragMode, DragModeStr, "dragMode");
}

const char * getHelpCtx(TDsgObj * aObj, char * ctrlName)
{
   ushort i = ((TViewData *)aObj->attributes)->helpCtx;
   if (i == 0) return blank;
   return (char *)HelpCtxsList()->getId(i);
}

const char * getValidator(TDsgObj * aObj, char * ctrlName)
{
   if (aObj->viewType != vtInput) return blank;

   static char buf[255];
   switch (((TDInputData *)aObj->attributes)->validatorType)
   {
//    case 1: sprintf(strbuf, "vtUserDefined"); break;
      case 2:
         sprintf(buf,
           "   (TInputLine *)%s->SetValidator(\n"
           "                           new TRangeValidator(%s));\n",
           ctrlName, ((TDInputData *)aObj->attributes)->validatorData);
      break;
      case 3:
         sprintf(buf,
           "   (TInputLine *)%s->SetValidator(\n"
           "                           new TFilterValidator(%s));\n",
           ctrlName, ((TDInputData *)aObj->attributes)->validatorData);
      break;
//    case 4: sprintf(strbuf, "vtPictureValidator"); break;
      default: return blank;
   }
   return buf;
}

const char * getCommand(TDsgObj * aObj)
{
   int i;
   
   if (aObj->viewType==vtButton)
     {
      i = ((TDButtonData *)aObj->attributes)->command;
      return (char *)CommandsList()->getId(i);
     }
   return 0;
}

const char * getPointerName(TDsgObj * aObj)
{
   return (char *)&((TViewData *)aObj->attributes)->thisName;
}

const char * getTextParam(TDsgObj * aObj)
{
   static char buf[16383]; // 16k must be enough
   static char itemstr[100];
   
   TStringCollection * strs;
   int i, ic;
   
   switch (aObj->viewType)
   {
      case vtLabel: return ((TDLabelData *)aObj->attributes)->text;
      case vtStatic: return ((TDStaticData *)aObj->attributes)->text;
      case vtButton: return ((TDButtonData *)aObj->attributes)->title;
      case vtDialog: return ((TDDialogData *)aObj->attributes)->title;
      case vtRadioButton:
      case vtCheckBox:
        if ( ((TDClusterData *)aObj->attributes)->items == 0 )
           strcpy(buf, "0");
        else
        {
           strcpy(buf, "");
//         strs = (TCollection *)((TDClusterData *)aObj->attributes)->items;
           strs = (TStringCollection *)aObj->dsgGetData();
           for (i = 0; i < strs->getCount(); i++)
           {
              sprintf(itemstr, "   TSItem(\x22%s\x22,", (char *)strs->at(i) );
              if (i == strs->getCount() - 1) strcat(itemstr, " 0 ");
                else strcat(itemstr, "\n");
              strcat(buf, itemstr);
              if (i == strs->getCount() - 1)
                 for (ic = 0; ic <= i; ic++) strcat(buf, ")");
           }
        }
        return buf;
      default: return blank;
   }
}

const char * getRectStr(const TRect& r)
{
   static char buf[40];
   sprintf(buf, "TRect( %i, %i, %i, %i )", r.a.x, r.a.y, r.b.x, r.b.y);
   return buf;
}

const char * getDlgRectStr(TDsgObj * aObj, TView * v)
{
  if (((TDDialogData *)aObj->attributes)->options & ofCentered == ofCentered)
     return getRectStr(v->getExtent()); else
     return getRectStr(v->getBounds());
}

const char * getDlgViews(TDsgObj * aObj)
{
   static char buf[32767]; // 32k must be enough

   int c = ObjectLinker()->getCount();
   
   strcpy(buf, "");
   if (c == 0) return buf;
   for(int i = 0; i < c; i++)
      buildCode(((TDsgLink *)ObjectLinker()->at(i))->d, buf);
   return buf;
}

const char * getDlgVariables(TDsgObj * aObj)
{
   TDsgLink * dlink;
/* TDsgObj * d;
   TView *f, *w;
   TDialog *dlg; */
   TViewData * vd;
   static char buf[1024];
   bool hasLinks = false, hasOptions = false, hasValidators = false;

   int c = ObjectLinker()->getCount();

   strcpy(buf, "");
   if (c == 0) return buf;
   for(int i = 0; i < c; i++)
   {
       dlink = (TDsgLink *)ObjectLinker()->at(i);
       if (dlink)
       {
          vd = (TViewData *)dlink->d->attributes;
          if (dlink->d->viewType == vtVScroll ||
              dlink->d->viewType == vtHScroll) scrollCount++;
          hasLinks = (dlink->d->viewType == vtLabel);
          hasOptions = (
            (vd->options != dlink->d->defaultData.options) ||
            (vd->eventMask != dlink->d->defaultData.eventMask) ||
            (vd->growMode != dlink->d->defaultData.growMode) ||
            (vd->dragMode != dlink->d->defaultData.dragMode) ||
            (vd->state != dlink->d->defaultData.state) ||
            (vd->helpCtx != 0) );
            /*hasValidators = (dlink->d->viewType == vtInput &&\
            ((TDInputData *)dlink->d->attributes)->validatorType > 0);*/
       }
   }
   if (scrollCount > 1) strcat(buf, "   TScrollBar * sb1, * sb2;\n"); else
   if (scrollCount == 1) strcat(buf, "   TScrollBar * sb1;\n"); else
   if (hasLinks || hasOptions) strcat(buf, "   TView * control;\n");
   if (hasLinks) strcat(buf, "   TLabel * lb;\n");
   if ((scrollCount > 0) || hasLinks || hasValidators || hasOptions)
        strcat(buf, "\n");
   return buf;
}

const char * buildScroll(TDsgLink * Link)
{
   char hs[5], vs[5];
   static char tmp[255];
   strcpy(tmp, blank);
   
   if (Link == 0) return tmp;
   
   strcpy(vs, (scrollCount > 1) ? "sb2" : "sb1");
   strcpy(hs, "sb1");
   if (Link->d->viewType == vtVScroll)
     sprintf(tmp, "   d->insert( %s = new TScrollBar( %s ) );\n", vs, getRectStr( Link->v->getBounds() ) );
   else
     sprintf(tmp, "   d->insert( %s = new TScrollBar( %s ) );\n", hs, getRectStr( Link->v->getBounds() ) );
   return tmp;
}

const char * ifScroll(TDsgObj * aObj)
{
   TDsgLink * l;
   static char tmp[255];
   strcpy(tmp, blank);

   if (aObj->viewType == vtListBox)
   {
      l = ObjectLinker()->scrollFind(
         ((TDListBoxData *)aObj->attributes)->scrollBar);
      return buildScroll(l);
   }
   else if (aObj->viewType == vtMemo)
   {
      l = ObjectLinker()->scrollFind(
         ((TDMemoData *)aObj->attributes)->hScroll);
      strcpy(tmp, buildScroll(l));
      l = ObjectLinker()->scrollFind(
         ((TDMemoData *)aObj->attributes)->vScroll);
      strcat(tmp, buildScroll(l));
   }
   //else return blank;
   return blank;
}

const char * ifLabel(TDsgObj * aObj)
{
   static char tmp[255];
   TDsgLink * l;
   
   strcpy(tmp, blank);

   l = ObjectLinker()->linkFind(((TViewData*)aObj->attributes)->thisName);
   if (l)
   {
      sprintf(tmp,
              "   d->insert( new TLabel( %s, \x22%s\x22, control ) );\n",
              getRectStr( l->v->getBounds() ),
              ((TDLabelData *)l->d->attributes)->text
             );
   }
   return tmp;
}

void viewCreate(TDsgObj * aObj, const char * aConstructor, char * to)
{
   static char buf[1023];
   char ctrlname[30];
   
   strcpy(buf, "");
   
// if (aObj->viewType == vtHScroll) strcpy(ctrlname, "sb1"); else
// if (aObj->viewType == vtVScroll) strcpy(ctrlname, (scrollCount > 1) ? "sb2" : "sb1"); else
// if (aObj->viewType == vtLabel) strcpy(ctrlname, "lb");
/* else*/strcpy(ctrlname, "control");
      
   bool directInsert = false;

   directInsert = ( getOptions(aObj, 0) == blank &&
                    getEventMask(aObj, 0) == blank &&
                    getState(aObj, 0) == blank &&
                    getGrowMode(aObj, 0) == blank &&
                    getDragMode(aObj, 0) == blank &&
                    getHelpCtx(aObj, 0) == blank /*&&
                    aObj->viewType != vtHScroll &&
                    aObj->viewType != vtVScroll */);
                    
/* if (!directInsert)
   { */
     if (ObjectLinker()->linkFind( ((TViewData *)aObj->attributes)->thisName ))
        directInsert = false;
     if (aObj->viewType == vtMemo &&
        ( strlen(((TDMemoData *)aObj->attributes)->hScroll) != 0  ||
          strlen(((TDMemoData *)aObj->attributes)->vScroll) != 0 ) )
        directInsert = false;
     if (aObj->viewType == vtListBox &&
        ( strlen(((TDListBoxData *)aObj->attributes)->scrollBar) != 0 ) )
        directInsert = false;
/* } */
   
   if (directInsert)
      sprintf(buf, "   d->insert( %s );\n", aConstructor);
   else
   {
      sprintf(buf, "%s   %s = %s;\n%s%s%s%s%s%s%s%s   d->insert( %s );\n",
                    ifScroll(aObj),
                    ctrlname,
                    aConstructor,
                    getOptions(aObj, ctrlname),
                    getEventMask(aObj, ctrlname),
                    getState(aObj, ctrlname),
                    getGrowMode(aObj, ctrlname),
                    getDragMode(aObj, ctrlname),
                    getHelpCtx(aObj, ctrlname),
                    getValidator(aObj, ctrlname),
                    ifLabel(aObj),
                    ctrlname );
   }
   strcat(to, buf);
}


const char * buildCode(TDsgObj * aObj, char * buffer)
{
   TView * v;
   char tmp[65535];
   char viewConstructor[1023];
   char hs[5], vs[5];
   char *hi, *vi;
   const char * var;
   
   strcpy(hs, "NULL");
   strcpy(vs, "NULL");
   strcpy(tmp, "");
   strcpy(viewConstructor, "");

   if (aObj->viewType == vtLabel ||
       aObj->viewType == vtVScroll ||
       aObj->viewType == vtHScroll) return 0;
       
   if (aObj->viewType == vtDialog)
   {
      v = DialogEditor();
//      ObjectLinker()->sortForBuild();
      ObjectLinker()->doReOrder();
   }
   else
      v = ObjectLinker()->dsgObjFind(aObj)->v;
      
   switch(aObj->viewType)
   {
//- User -------------------------------------------------------------------
      case vtOther: break;
         sprintf(viewConstructor, "new %s( %s )",
            ((TViewData *)aObj->attributes)->className,
            getRectStr( v->getBounds()) );
         viewCreate(aObj, viewConstructor, tmp);
      break;
/*- Label ------------------------------------------------------------------
      case vtLabel:
         if (((TDLabelData *)aObj->attributes)->link < 0) return buffer;
         sprintf(viewConstructor, "new TLabel( %s, \x22%s\x22, control )",
            getRectStr( v->getBounds() ), getTextParam(aObj) );
         viewCreate(aObj, viewConstructor, tmp);
      break; */
//- InputLine --------------------------------------------------------------
      case vtInput:
         sprintf(viewConstructor, "new TInputLine( %s, %i )",
            getRectStr( v->getBounds() ),
            ((TDInputData *)aObj->attributes)->maxLen );
         viewCreate(aObj, viewConstructor, tmp);
      break;
//- Memo -------------------------------------------------------------------
      case vtMemo:
         hi = ((TDMemoData *)aObj->attributes)->hScroll;
         vi = ((TDMemoData *)aObj->attributes)->vScroll;
         if (strcmp("", hi) != 0) strcpy(hs, "sb1");
         if (strcmp("", vi) != 0)
             strcpy(vs, (scrollCount > 1) ? "sb2" : "sb1");
         sprintf(viewConstructor, "new TMemo( %s, %s, %s, NULL, %i )",
            getRectStr( v->getBounds() ),      //    h  s   ind bs
            hs, vs, ((TDMemoData *)aObj->attributes)->bufSize );
         viewCreate(aObj, viewConstructor, tmp);
      break;
//- StaticText -------------------------------------------------------------
      case vtStatic:
         sprintf(viewConstructor, "new TStaticText( %s, \x22%s\x22 )",
            getRectStr( v->getBounds() ), getTextParam(aObj) );
         viewCreate(aObj, viewConstructor, tmp);
      break;
//- Button -----------------------------------------------------------------
      case vtButton:
         sprintf(viewConstructor, "new TButton( %s, \x22%s\x22, %s, %s )",
            getRectStr( v->getBounds() ), getTextParam(aObj),
            getCommand(aObj), ButtonFlagStr(
            ((TDButtonData *)aObj->attributes)->flags) );
         viewCreate(aObj, viewConstructor, tmp);
      break;
//- ListBoxes --------------------------------------------------------------
      case vtListBox:
         vi = ((TDListBoxData *)aObj->attributes)->scrollBar;
         if (strcmp("", vi) != 0)
           strcpy(vs, (scrollCount > 1) ? "sb2" : "sb1");
         sprintf(viewConstructor, "new TListBox( %s, %i, %s )",
         getRectStr( v->getBounds() ),
         ((TDListBoxData *)aObj->attributes)->columns,  vs);
         viewCreate(aObj, viewConstructor, tmp);
      break;
//- RadioButtons -----------------------------------------------------------
      case vtRadioButton:
         sprintf(viewConstructor, "new TRadioButtons( %s, \n%s )",
            getRectStr( v->getBounds() ), getTextParam(aObj) );
         viewCreate(aObj, viewConstructor, tmp);
      break;
//- CheckBoxes -------------------------------------------------------------
      case vtCheckBox:
         sprintf(viewConstructor, "new TCheckBoxes( %s, \n%s )",
            getRectStr( v->getBounds() ), getTextParam(aObj) );
         viewCreate(aObj, viewConstructor, tmp);
      break;
/*- ScrollBar --------------------------------------------------------------
      case vtVScroll:
         sprintf(viewConstructor, "new TScrollBar( %s )", getRectStr( v->getBounds() ) );
         viewCreate(aObj, viewConstructor, tmp);
      break;
      case vtHScroll:
         sprintf(viewConstructor, "new TScrollBar( %s )", getRectStr( v->getBounds() ) );
         viewCreate(aObj, viewConstructor, tmp);
      break; */
//- Dialog -----------------------------------------------------------------
      case vtDialog:
         var = getDlgVariables(aObj);
         sprintf(tmp, "TDialog * build%s()\n{\n"
            "%s   TDialog * d = new TDialog( %s, \x22%s\x22 );\n\n"
            "%s%s%s%s%s%s%s%s\n   d->selectNext(True);\n   return d;\n}\n\n",
            getPointerName(aObj),
            var, //getDlgVariables(aObj),
            getDlgRectStr(aObj, v),
            getTextParam(aObj),
            getOptions(aObj, "d"),
            getEventMask(aObj, "d"),
            getState(aObj, "d"),
            getGrowMode(aObj, "d"),
            getDragMode(aObj, "d"),
            getHelpCtx(aObj, "d"),
            getFlags(aObj, "d"),
            getDlgViews(aObj) );
            scrollCount = 0;
         break;
      default: return buffer;
   }
   if (buffer)
   {
      strcat(buffer, tmp);
      return buffer;
   } else return newStr(tmp);
}
#undef _buildscroll_
#undef _commomget_
