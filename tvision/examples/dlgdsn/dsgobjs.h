/***************************************************************************

    dsgobjs.h - Editable classes prototypes
    ---------------------------------------------------------------------
    May, 2000
    Copyright (C) 2000 by Warlei Alves
    walves@usa.net
    
    Modified by Salvador E. Tropea to compile without warnings.
    This header allocated a couple of structures in each file that used it.
    
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
#if !defined(_DSGOBJS_HPP_)
#define _DSGOBJS_HPP_

#include <stdlib.h>

#define Uses_TDialog
#define Uses_TFrame
#define Uses_TPalette
#define Uses_TInputLine
#define Uses_TButton
#define Uses_TCheckBoxes
#define Uses_TRadioButtons
#define Uses_TScrollBar
#define Uses_TRect
#define Uses_TEvent
#define Uses_TStaticText
#define Uses_TListBox
#define Uses_TProgram
#define Uses_TMemo
#define Uses_TLabel
#define Uses_TSItem
#define Uses_TCollection
#define Uses_TMenuBar
#define Uses_TStatusLine
#define Uses_TDeskTop
#define Uses_TWindow

#include <tv.h>

#include "fdconsts.h"
#include "dsgdata.h"
#include "propedit.h"
/*
typedef short TViewType;

const TViewType
   vtNone = -1,
   vtOther = 0,
   vtLabel = 1,
   vtInput = 2,
   vtMemo = 3,
   vtStatic = 4,
   vtButton = 5,
   vtListBox = 6,
   vtRadioButton = 7,
   vtCheckBox = 8,
   vtVScroll = 9,
   vtHScroll = 10,
   vtDialog = 11;
*/
enum TViewType { vtNone = -1, vtOther, vtLabel, vtInput, vtMemo, vtStatic,
   vtButton, vtListBox, vtRadioButton, vtCheckBox, vtVScroll, vtHScroll,
   vtDialog };
   

class TEditCollection: public TCollection
{
public:
   TEditCollection(): TCollection(0, 1) { };
   virtual void * readItem(ipstream &) { return 0; };
   virtual void writeItem(void *, opstream &) { };
};

class TAppWindow: public TWindow
{
   TDeskTop * deskTop;
   TMenuBar * menuBar;
   TStatusLine * statusLine;
   void removeStatusLine();
   void removeMenuBar();
public:
   TAppWindow();
   ~TAppWindow();
   TEditCollection * dsgMenuBar;
   TEditCollection * dsgSatusLine;
   TEditCollection * dialogs;
   static TFrame * initFrame(TRect r);
   virtual TPalette& getPalette() const;
   void setMenuBar(TEditCollection * aItems);
   void setStatusLine(TEditCollection * aItems);
   void setMenuBar(TMenuBar * aMenuBar);
   void setStatusLine(TStatusLine * aStatusLine);
   virtual void handleEvent(TEvent& event);
   virtual void changeBounds(const TRect& bounds);
};

class TDFrame: public TFrame
{
public:
  TDFrame(const TRect& bounds);
  virtual void draw();
};

class opstream;

class TDsgObj
{
protected:
   int attrSize;
   int nameIndex;
   void buildName(TNameStr * name, int mode);
   virtual void setViewData(TDsgObjData& data);
   virtual TView * Me() { return 0; };
   virtual void setupView(TView * View);
public:
   TDsgObj(TViewType ViewType);
   virtual ~TDsgObj();
   char * getScript(ushort ScriptType);
   void setPos(TPoint neworigin, TPoint newsize);
   virtual void dsgUpdate();
   virtual void * dsgGetData() { return 0; };
   virtual TView * createView(TView *) { return 0; };
   int getAttrSize() { return attrSize; };
  
   void * attributes;
   TViewType viewType;
   TDefaultData defaultData;
   TDsgObj * dsgObj;
   bool tabStop();
};

class TDDialog: public TDialog, public TDsgObj
{
   char * fileName;
protected:
   virtual TView * Me();
public:
   TDDialog();
   virtual void shutDown();
   virtual void draw();
   static TFrame * initFrame(TRect r);
   virtual void handleEvent(TEvent& event);
   virtual void setState(ushort aState, Boolean enabled);
// void setCurrentClass(ushort aClass);
   void dinsert(TView * aView);
   virtual void changeBounds(const TRect& bounds);
   void dlgRun();
   
   virtual void dsgUpdate();
   virtual void * dsgGetData();
   Boolean saveToFile(const char * FileName);
   Boolean Save(int aCommand = cmCancel);
   Boolean loadFromFile(const char * FileName);
   void setModified(Boolean);
   Boolean modified;
};

//class TDsgView: public TView, public TDsgObj { };

class TDLabel: public TLabel, public TDsgObj
{
protected:
   virtual TView * Me();
public:
   TDLabel(const TRect& bounds);
   virtual void shutDown();
   virtual void handleEvent(TEvent& event);
   virtual TPalette& getPalette() const;
   virtual void sizeLimits(TPoint& min, TPoint& max);
   virtual void setState(ushort aState, Boolean enable);
   virtual void changeBounds(const TRect& bounds);

   virtual void dsgUpdate();
   virtual void * dsgGetData();
   virtual TView * createView(TView * aLink);
};

class TDInputLine: public TInputLine, public TDsgObj
{
protected:
   virtual TView * Me();
public:
   TDInputLine(const TRect& bounds);
   virtual void shutDown();
   virtual void handleEvent(TEvent& event);
   virtual TPalette& getPalette() const;
   virtual void sizeLimits(TPoint& min, TPoint& max);
   virtual void setState(ushort aState, Boolean enable);
   virtual void changeBounds(const TRect& bounds);

//   virtual void dsgUpdate();
//   virtual void * dsgGetData();
   virtual TView * createView(TView * aView);
};

class TDMemo: public TMemo, public TDsgObj
{
protected:
   virtual TView * Me();
public:
   TDMemo(const TRect& bounds);
   virtual void shutDown();
   virtual void handleEvent(TEvent& event);
   virtual TPalette& getPalette() const;
   virtual void sizeLimits(TPoint& min, TPoint& max);
   virtual void setState(ushort aState, Boolean enable);
   virtual void changeBounds(const TRect& bounds);

// virtual void dsgUpdate();
// virtual void * dsgGetData();
   virtual TView * createView(TView * aView);
};

class TDStaticText: public TStaticText, public TDsgObj
{
protected:
   virtual TView * Me();
public:
   TDStaticText(const TRect& bounds);
   virtual void shutDown();
   virtual void handleEvent(TEvent& event);
   virtual TPalette& getPalette() const;
   virtual void sizeLimits(TPoint& min, TPoint& max);
   virtual void setState(ushort aState, Boolean enable);
   virtual void changeBounds(const TRect& bounds);

   virtual void dsgUpdate();
   virtual void * dsgGetData();
   virtual TView * createView(TView * aView);
};


class TDButton: public TButton, public TDsgObj
{
protected:
   virtual TView * Me();
public:
   TDButton(const TRect& bounds);
   virtual void shutDown();
   virtual void handleEvent(TEvent& event);
   virtual TPalette& getPalette() const;
   virtual void sizeLimits(TPoint& min, TPoint& max);
   virtual void setState(ushort aState, Boolean enable);
   virtual void changeBounds(const TRect& bounds);

   ushort getCommand();
   ushort getFlags();
   virtual void dsgUpdate();
   virtual void * dsgGetData();
   virtual TView * createView(TView * aView);
};

class TDListBox: public TListBox, public TDsgObj
{
protected:
   virtual TView * Me();
public:
   TDListBox(const TRect& bounds);
   virtual void shutDown();
   virtual void handleEvent(TEvent& event);
   virtual TPalette& getPalette() const;
   virtual void sizeLimits(TPoint& min, TPoint& max);
   virtual void setState(ushort aState, Boolean enable);
   virtual void changeBounds(const TRect& bounds);

   virtual void dsgUpdate();
   virtual void * dsgGetData();
   virtual TView * createView(TView * aView);
};


class TDRadioButtons: public TRadioButtons, public TDsgObj
{
protected:
   virtual TView * Me();
public:
   TDRadioButtons(const TRect& bounds, TSItem * aStrings);
   virtual void shutDown();
   virtual void handleEvent(TEvent& event);
   virtual TPalette& getPalette() const;
   virtual void sizeLimits(TPoint& min, TPoint& max);
   virtual void setState(ushort aState, Boolean enable);
   virtual void changeBounds(const TRect& bounds);

// virtual void dsgUpdate();
   virtual void * dsgGetData();
   virtual TView * createView(TView * aView);
};


class TDCheckBoxes: public TCheckBoxes, public TDsgObj
{
protected:
   virtual TView * Me();
public:
   TDCheckBoxes(const TRect& bounds, TSItem * aStrings);
   virtual void shutDown();
   virtual void handleEvent(TEvent& event);
   virtual TPalette& getPalette() const;
   virtual void sizeLimits(TPoint& min, TPoint& max);
   virtual void setState(ushort aState, Boolean enable);
   virtual void changeBounds(const TRect& bounds);

// virtual void dsgUpdate();
   virtual void * dsgGetData();
   virtual TView * createView(TView * aView);
};

class TVScrollBar: public TScrollBar, public TDsgObj
{
protected:
   virtual TView * Me();
public:
   TVScrollBar(const TRect& bounds);
   virtual void shutDown();
   virtual void handleEvent(TEvent& event);
   virtual TPalette& getPalette() const;
   virtual void sizeLimits(TPoint& min, TPoint& max);
   virtual void setState(ushort aState, Boolean enable);
   virtual void changeBounds(const TRect& bounds);
   virtual TView * createView(TView * aView);
};

class THScrollBar: public TScrollBar, public TDsgObj
{
protected:
   virtual TView * Me();
public:
   THScrollBar(const TRect& bounds);
   virtual void shutDown();
   virtual void handleEvent(TEvent& event);
   virtual TPalette& getPalette() const;
   virtual void sizeLimits(TPoint& min, TPoint& max);
   virtual void setState(ushort aState, Boolean enable);
   virtual void changeBounds(const TRect& bounds);
   virtual TView * createView(TView * aView);
};

void InitDlgEditor(const char * FileName);

void DoneDialogEditor();

TDDialog * DialogEditor();

TDialog * ToolDialog();

TWindow * ObjectEditor();

void SetGrid(bool enable);

bool GetGrid();

// Code builder functions
const char * buildCode(TDsgObj * aObj, char * buffer);

#endif //_DSGOBJS_HPP_

