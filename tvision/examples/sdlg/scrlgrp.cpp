#define Uses_ScrollGroup
// ===================================================================
#define Uses_TGroup
#define Uses_TPoint
#define Uses_TRect
// ===================================================================

#define Uses_TScrollBar
#define Uses_TBackground
#define Uses_TEvent
// ===================================================================
#include <tv.h>
#include "dlg.h"
// ===================================================================

ScrollGroup::ScrollGroup(const TRect& bounds, TScrollBar* hsb, TScrollBar* vsb) :
	ScrollGroupInit(initBackground),
	TGroup(bounds),
	hScrollBar(hsb),
	vScrollBar(vsb),
	background(0)
{
	eventMask |= evBroadcast;
	delta.x = delta.y = limit.x = limit.y = 0;

	if(bkgdMaker != 0 && (background = bkgdMaker(getExtent())) != 0)
		{
		background->growMode = gfGrowHiX | gfGrowHiY;
		insert(background);
		}
}

void ScrollGroup::changeBounds(const TRect& bounds)
{
	lock();
	TGroup::changeBounds(bounds);
	setLimit(limit.x, limit.y);
	unlock();
	drawView();
}

static Boolean isView(TView* view, void* args)
{
	return Boolean(view == args);
}

void ScrollGroup::handleEvent(TEvent& event)
{
	TGroup::handleEvent(event);

	if(event.what == evBroadcast)
		{
		if(event.message.command == cmScrollBarChanged &&
				(event.message.infoPtr == hScrollBar ||
					event.message.infoPtr == vScrollBar))
			scrollDraw();
		else if(event.message.command == cmReceivedFocus &&
				firstThat(isView, event.message.infoPtr) != 0)
			focusSubView((TView*) event.message.infoPtr);
		}
}

struct ScrollInfo
{
	TPoint delta;
	TView* ignore;
};

static void doScroll(TView* view, void* args)
{
	ScrollInfo* info = (ScrollInfo*) args;

	if(view != info->ignore)
		{
		TPoint dest = view->origin + info->delta;
		view->moveTo(dest.x, dest.y);
		}
}

void ScrollGroup::scrollDraw()
{
	TPoint  d;

	d.x = hScrollBar ? hScrollBar->value : 0;
	d.y = vScrollBar ? vScrollBar->value : 0;

	if( d.x != delta.x || d.y != delta.y )
		{
		ScrollInfo info;

		info.delta = delta-d;
		info.ignore = background;
		lock();
		forEach(doScroll, &info);
		delta = d;
		unlock();
		drawView();
		}
}

void ScrollGroup::scrollTo(int x, int y)
{
	lock();
	if( hScrollBar != 0 && x != hScrollBar->value)
		hScrollBar->setValue(x);
	if( vScrollBar != 0 && y != vScrollBar->value)
		vScrollBar->setValue(y);
	unlock();
	scrollDraw();
}

void ScrollGroup::setLimit(int x, int y)
{
	limit.x = x;
	limit.y = y;
	lock();
	if(hScrollBar != 0)
		hScrollBar->setParams(hScrollBar->value, 0, x-size.x, size.x-1, 1);
	if(vScrollBar != 0 )
		vScrollBar->setParams(vScrollBar->value, 0, y-size.y, size.y-1, 1);
	unlock();
	scrollDraw();
}

void ScrollGroup::setState(ushort aState, Boolean enable)
{
	TGroup::setState(aState, enable);
	if((aState & (sfActive | sfSelected)) != 0 )
		{
		if(hScrollBar != 0)
			if(enable)
				hScrollBar->show();
			else
				hScrollBar->hide();
		if(vScrollBar != 0)
			if(enable)
				vScrollBar->show();
			else
				vScrollBar->hide();
		}
}

void ScrollGroup::focusSubView(TView* view)
{
	TRect rview = view->getBounds();

	TRect r = getExtent();
	r.intersect(rview);
	if(r != rview)
		{
		int dx, dy;

		dx = delta.x;
		if(view->origin.x < 0)
			dx = delta.x + view->origin.x;
		else if(view->origin.x + view->size.x > size.x)
			dx = delta.x+view->origin.x+view->size.x-size.x;

		dy = delta.y;
		if(view->origin.y < 0)
			dy = delta.y + view->origin.y;
		else if(view->origin.y + view->size.y > size.y)
			dy = delta.y+view->origin.y+view->size.y-size.y;

		scrollTo(dx, dy);
		}
}

TBackground* ScrollGroup::initBackground(TRect r)
{
	return new TBackground(r, ' ');
}
