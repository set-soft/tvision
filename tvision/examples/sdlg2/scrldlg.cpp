#define Uses_ScrollDialog
// =================================================
#define Uses_TDialog
// =================================================
#define Uses_ScrollGroup
// =================================================
#define Uses_TGroup
// =================================================

#define Uses_TScrollBar
#define Uses_TEvent
#define Uses_TKeys
// =================================================
#include <tv.h>
#include "dlg.h"
// =================================================

ScrollDialog::ScrollDialog(const TRect& bounds, const char* aTitle, ushort f) :
	TWindowInit(initFrame),
	TDialog(bounds, aTitle),
	scrollGroup(0)
{
	TScrollBar* hsb = 0;
	TScrollBar* vsb = 0;

	if(f & sbHorBar)
		hsb = standardScrollBar(sbHorizontal|sbHandleKeyboard);
	if(f & sbVerBar)
		vsb = standardScrollBar(sbVertical|sbHandleKeyboard);

	TRect r = getExtent();
	r.grow(-1,-1);
	scrollGroup = new ScrollGroup(r, hsb, vsb);
	scrollGroup->growMode = gfGrowHiX | gfGrowHiY;
	insert(scrollGroup);
}

void ScrollDialog::handleEvent(TEvent& event)
{
	if(event.what == evKeyDown &&
		(event.keyDown.keyCode == kbTab ||
			event.keyDown.keyCode == kbShiftTab))
		{
		scrollGroup->selectNext(Boolean(event.keyDown.keyCode == kbShiftTab));
		clearEvent(event);
		}
	TDialog::handleEvent(event);
}
