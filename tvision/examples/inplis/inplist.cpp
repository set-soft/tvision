/*

  Created by L. Borobia 92-11-30

  File: INPLIST.CPP

  A little example, how to execute an inputline in a
  focused item from a listbox.

*/

#include "stdio.h"
#include "string.h"
#include "inplist.h"

TExecInputLine::TExecInputLine(const TRect& bounds, int aMaxLen)
               :TInputLine(bounds, aMaxLen)
 {
 }

void TExecInputLine::set(void *rec)
 {
   setData(rec);
 }

void TExecInputLine::get(void *rec)
 {
   getData(rec);
 }

ushort TExecInputLine::execute()
 {
  TEvent e;
  Boolean leavemodal = False;
  do
   {
    getEvent(e);
    if (e.what == evKeyDown)
     {
      switch (e.keyDown.keyCode)
        {
          case kbDown:
          case kbUp:
          case kbEsc:
          case kbEnter:
          case kbPgDn:
          case kbPgUp:
          case kbCtrlPgDn:
          case kbCtrlPgUp:
            leavemodal = True;
           break;
          default:
            leavemodal = False;
           break;
       }
    }
    if (e.what == evMouseDown && e.mouse.buttons == mbLeftButton)
     {
      TPoint p = makeLocal(e.mouse.where);
      if (mouseInView(p) == False)
        leavemodal = True;
     }
    handleEvent(e);
   } while (leavemodal == False);
  return e.keyDown.keyCode;
}


TInputBox::TInputBox(const TRect& bounds, ushort aNumCols, TScrollBar *aScrollBar)
          :TListBox(bounds,aNumCols,aScrollBar)

 {
 }

void TInputBox::getText(char *dest, int item, short maxLen)
 {
  ListBoxItem *v;
  char s[MAXVALUELEN+MAXLABELLEN];
  if (list() != 0 )
    {
     v = (ListBoxItem *)(list()->at(item));
     sprintf(s, "%.*s = %.*s", MAXLABELLEN,v->label, MAXVALUELEN, v->value);
     strncpy( dest, s, maxLen );
     dest[maxLen] = '\0';
    }
   else
    *dest = EOS;
 }

ushort TInputBox::inputData( )
 {
  ListBoxItem *v;
  TExecInputLine *te;
  TRect r = getExtent();
  ushort control=0;
  if (list() != 0 )
   {
    v  = (ListBoxItem *)(list()->at(focused));
    r  = TRect(strlen(v->label)+5, focused-topItem+1,r.b.x-r.a.x,focused-topItem+2);
    te = new TExecInputLine(r,MAXVALUELEN);
    te->set(v->value);
    control = owner->execView((TView *)te);
    if (control != kbEsc)
       te->get(v->value);
    CLY_destroy((TView *)te);
   }
  return control;
 }


void TInputBox::handleEvent(TEvent& event)
 {
  TEvent e = event;
  ushort item;
  TListBox::handleEvent(event);
  item = focused;
  if ( event.what == evKeyDown )
   {
      if (event.keyDown.charScan.charCode >=  32)
       {
        putEvent(event);
        if (inputData() == kbUp)
             item--;
           else
             item++;
        drawView();
        focusItemNum(item);
       }
       else
        if ( event.keyDown.keyCode == kbRight ||
             event.keyDown.keyCode == kbLeft  ||
             event.keyDown.keyCode == kbEnter
            )
         {
          if (inputData() == kbUp)
             item--;
           else
             item++;
          drawView();
          focusItemNum(item);
         }
    }
   else
    if (e.what == evMouseDown &&
         e.mouse.buttons == mbLeftButton)
     {
      inputData();
      drawView();
     }
 }


TInputDialog::TInputDialog(const TRect& r, const char *aTitle, TCollection *aList)
                  : TWindowInit( &TInputDialog::initFrame )
                  , TDialog(r, aTitle)
                   
 {
   options   |= ofCentered;
   TRect r1   = getExtent();
   r1.grow(-1,-1);
   vScrollBar = new TScrollBar(TRect(r1.b.x-1,r1.a.y,r1.b.x,r1.b.y));
   inputBox   = new TInputBox(r1,1,vScrollBar);
   inputBox->newList(aList);
   insert(inputBox);
   insert(vScrollBar);
 }

