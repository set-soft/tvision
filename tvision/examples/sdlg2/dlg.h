/*

File:	DLG.H
Author:	Patrick Reilly (TeamB)  CIS: 71333,2764
Date:	9/16/93
Desc:	Header file for ScrollGroup and ScrollDialog classes.

	This file is the header file for a pair of classes that allow use of a
scroll-able dialog box. It is designed for (but doesn't require) a TV.H-like
master header file with the following lines:

	...

	#ifdef	Uses_ScrollDialog
	#define	Uses_TDialog
	#define INC_DLG_H
	#endif

	#ifdef	Uses_ScrollGroup
	#define Uses_TGroup
	#define INC_DLG_H
	#endif

	...

	#include <tv.h>

	...

	#ifdef	INC_DLG_H
	#include "Dlg.h"
	#endif

	...


*/

#if defined(Uses_ScrollGroup) && !defined(Def_ScrollGroup)
#define Def_ScrollGroup

class TRect;
class TScrollBar;
struct TEvent;
class TBackground;

class ScrollGroupInit
{
public:

	ScrollGroupInit(TBackground* (*fn)(TRect)) : bkgdMaker(fn)
	{}

	TBackground* (*bkgdMaker)(TRect);
};

class ScrollGroup : public TGroup, public virtual ScrollGroupInit
{
public:

	ScrollGroup(const TRect&, TScrollBar*, TScrollBar*);

	virtual void changeBounds(const TRect&);
	virtual void handleEvent(TEvent&);
	virtual void scrollDraw();
	virtual void scrollTo(int, int);
	virtual void setLimit(int, int);
	virtual void setState(ushort, Boolean);
	virtual void focusSubView(TView*);

	static TBackground* initBackground(TRect);

	TScrollBar* hScrollBar;
	TScrollBar* vScrollBar;
	TBackground* background;
	TPoint delta;
	TPoint limit;
};

#endif

#if defined(Uses_ScrollDialog) && !defined(Def_ScrollDialog)
#define Def_ScrollDialog

class TRect;
struct TEvent;
class ScrollGroup;
const ushort sbHorBar = 0x0001;
const ushort sbVerBar = 0x0002;

class ScrollDialog : public TDialog
{
public:

	ScrollDialog(const TRect&, const char*, ushort);

	virtual void handleEvent(TEvent&);

	ScrollGroup* scrollGroup;
};

#endif
