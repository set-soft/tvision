/*

Author:	Patrick Reilly
Date:  	9/16/93

	This is a demo program to demonstrate using the ScrollDialog (and
ScrollGroup) classes. Released to the public domain. Use at your own risk <g>.

*/

#define Uses_ScrollDialog
	#define Uses_TDialog
#define Uses_ScrollGroup
	#define Uses_TGroup

#define Uses_TStaticText
#define Uses_TInputLine
#define Uses_TApplication
#define Uses_TDeskTop
#include <tv.h>
#include "dlg.h"

class TApp : public TApplication
{
public:

	TApp();
};

static char ctrlString[] = "Control 01";

TApp::TApp() : TProgInit(initStatusLine, initMenuBar, initDeskTop)
{
	ScrollDialog* dlg = new ScrollDialog(TRect(0,0,40,10), "Test Dialog", sbVertical);
	dlg->options |= ofCentered;
	dlg->flags |= wfGrow;

	for(int x = 0; x < 40; x++)
		{
		if(x%10)
			{
			int n = x+1;
			ctrlString[8] = '0' + (n/10);
			ctrlString[9] = '0' + (n%10);
			dlg->scrollGroup->insert(new TStaticText(TRect(0,x,10,x+1), ctrlString));
			}
		else
			dlg->scrollGroup->insert(new TInputLine(TRect(0,x,10,x+1), 20));
		}
	dlg->scrollGroup->selectNext(False);
	dlg->scrollGroup->setLimit(0, 40);

	if(validView(dlg) != 0)
		{
		deskTop->execView(dlg);
		CLY_destroy(dlg);
		}
}

int main()
{
	TApp app;
	app.run();
	app.shutDown();
	return 0;
}
