/***************************************************************************

    propedit.h - Prototype for object editor
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
 
#if !defined(_PROPEDIT_HPP_)
#define _PROPEDIT_HPP_

#define Uses_TWindow
#define Uses_TDialog
#define Uses_TEvent
#define Uses_TListBox
#define Uses_TScrollBar
#define Uses_TStringCollection
#define Uses_TCollection
#define Uses_TInputLine
#define Uses_TValidator
#define Uses_opstream
#define Uses_ipstream

#include <tv.h>

class TDsgObj;
class TStructMap;

// Sort function prototype
typedef int (* ccSortFunc)(const void *, const void *);

typedef struct TDsgLink
{
  TView * v;
  TDsgObj * d;
};

// The manager for TDsgObj<->TView linked list.
// Editable objects are descendants from TView and TDsgObj.
// It's make easy some operations but one can't be externaly
// referenced as other. To make that we need this class.
class TLinkList: public TCollection
{
public:
   TLinkList(): TCollection(0, 1) { };
   TDsgLink * viewFind(void * aView);     // Find the link for a TView
   TDsgLink * dsgObjFind(void * aDsgObj); // Find the link for a TDsgObj
   TDsgLink * scrollFind(char *);         // Find the scrollbar by index
   TDsgLink * linkFind(char *);           // Returns the TDLabel linked to
   void removeMe(TDsgObj * aDsgObj);      // Remove the link for a TDsgObj
   void add(TView * aView, TDsgObj * aDsgObj); // Add new link
   virtual void freeItem(void * item);
   virtual void * readItem(ipstream &) { return 0; };    // It not need be loaded or...
   virtual void writeItem(void *, opstream &) { }; // saved
   void sort(ccSortFunc Compare);         // Tabulation
   void doReOrder();                      // methods
   void removeNotify(TCollection * aCollection, int Index);
   void sortForBuild();
   void linkChangedName(char * oldName, char * newName);
};

// Ordinary number and short string editor
// Is a TInputLine descendant to be executed in a TWindow
class TInPlaceEdit: public TInputLine
{
   ushort endState;
public:
   TInPlaceEdit(const TRect& bounds, ushort aMaxLen,
               TValidator * aValidator);
   virtual ushort execute();
   virtual void handleEvent(TEvent& event);
   virtual TPalette& getPalette() const;
};

// A grid style viewer and editor for objects
class TObjEditView: public TView
{
   const char * getValueFor(const TStructMap * map);
   void editItem(const TStructMap * map);
   int separator;
   TScrollBar * sb;
   const TStructMap * dataMap;
   const TStructMap * currentMap;
   void * data;
public:
   TObjEditView(const TRect& bounds, TScrollBar * v);
   virtual void handleEvent(TEvent& event);
   virtual void draw();
   void setMap(const TStructMap * aMap, void * Data);
   int vtCurrent;
};

// A TWindow to host the object editor
class TObjEdit: public TWindow
{
   void editItem(TStructMap * map);
   TObjEditView * dataView;
public:
   TObjEdit(TDsgObj * Obj);
   void setObjData(TDsgObj * DsgObj);
   virtual void handleEvent(TEvent& event);
   TDsgObj * object;
};

int execDialog(TDialog * dialog, void *data);

TLinkList * ObjectLinker();

#endif // _PROPEDIT_HPP_
