/***************************************************************************

    dsgdata.h - Editable structs prototypes for editable classes
    ---------------------------------------------------------------------
    May, 2000
    Copyright (C) 2000 by Warlei Alves
    walves@usa.net
    
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
#if !defined(_DSGDATA_HPP_)
#define _DSGDATA_HPP_

#define MAX_LABEL_LENGTH 30
#define MAX_TITLE_LENGTH 62

typedef char TNameStr[MAX_LABEL_LENGTH];
typedef char TTitleStr[MAX_TITLE_LENGTH];

typedef struct TPoint;

#define _viewdata_() \
   TPoint origin;       /*   8 */ \
   TPoint size;         /*   8 */ \
   TNameStr className;  /*  30 */ \
   TNameStr thisName;   /*  30 */ \
   ushort options;      /*   2 */ \
   ushort eventMask;    /*   2 */ \
   ushort state;        /*   2 */ \
   ushort growMode;     /*   2 */ \
   ushort dragMode;     /*   2 */ \
   ushort helpCtx       /*   2 */ \
//                          88 % 4 = 0
typedef struct TDefaultData
{
   ushort options;
   ushort eventMask;
   ushort state;
   ushort growMode;
   ushort dragMode;
   ushort helpCtx;
   ushort flags;
};

typedef struct TViewData // Wrapper struct
{
  _viewdata_();
};

typedef struct TDsgObjData
{
  _viewdata_();
  int tabOrder;
};

typedef struct TDDialogData
{
  _viewdata_();
  TTitleStr title;
  ushort flags;
};

typedef struct TDInputData
{
   _viewdata_();
   int tabOrder;
   int maxLen;
   ushort validatorType;
   TNameStr validatorData;
};

typedef struct TDLabelData
{
   _viewdata_();
   TNameStr text;
   TNameStr link;
};

typedef struct TDMemoData
{
   _viewdata_();
   int tabOrder;
// uint32 bufSize;
// TNameStr Indicator;
   TNameStr hScroll;
   TNameStr vScroll;
   uint32 bufSize;
};

typedef struct TDStaticData
{
   _viewdata_();
   char * text;
};

typedef struct TDButtonData
{
   _viewdata_();
   int tabOrder;
   TNameStr title;
   ushort command;
   ushort flags;
};

typedef struct TDClusterData
{
   _viewdata_();
   int tabOrder;
   void * items;
};

typedef struct TDListBoxData
{
   _viewdata_();
   int tabOrder;
   short columns;
   TNameStr scrollBar;
};

#define TDCheckData    TDClusterData
#define TDRadioData    TDClusterData
#define TDVScrollData  TViewData
#define TDHScrollData  TViewData

#undef _viewdata_

#endif // _DSGDATA_HPP_
