/*****************************************************************************

  Hi! here SET writing ;-)
  That's a pretty silly example, but I include it more as a test. The real
code for this example is inside the library and is called tvedit?.cc.
Basically it defines a basic text editor applition. You can create your own
text editor inheriting from TEditorApp and overriding the desired members.
  Robert included it in the library in the past and I think is good idea
because you can do an application based on TEditorApp in a few minutes.

*****************************************************************************/

#define Uses_TApplication
#define Uses_TEditorApp
#define Uses_TScreen
#include <tv.h>

int main()
{
 //TScreen::setShowCursorEver(True);
 TEditorApp *myApp=new TEditorApp();
 if (myApp)
   {
    myApp->run();
    delete myApp;
   }
 return 0;
}
