
TLNMSG.H has the prototype for the postMsg(const char *) and
postInfo(int, const char *) functions.  You don't have to use these
functions if you don't want to, but they simplify the calling parameters
and are easy to call from within "straight C" functions.  Actually,
we are compiling standalone versions of our software and an integrated
version.  One function call to postInfo or postMsg works in both cases
using conditional compilation.  For example:

#if defined(APPLICATION)
#include "tlnmsg.h"
#else
#include <stdio.h>
void postMsg(const char * msg)
{
  printf("%s\n", msg);
}
void postInfo(int line, const char * msg)
{
  printf("%s\n", msg);
}
#endif

This is an example of how to use the two classes defined in TLNMSG.CPP.
Note that you have to explicitly call getEvent(TEvent&) to find out
if the user pressed escape.  Another method would be to make kbEsc a
case in your main handleEvent loop.  For us, that was impractical
considering there is no handleEvent loop in our stand alone code.

void handleEvent(event)
{
   switch( event.message.command )
   {
     ...

     case cmTestCase:
            TEvent anEvent;
            Boolean userBreak = False;

            postInfo(1, " Line 1");
            postInfo(2, " Line 2");

            // This should be highlighted in blue as
            // it is the last line in the box.

            postInfo(10, "\003Press escape to continue");

            for (i = 0; i < 1000 && !userBreak; ++i) {
              sprintf(msg, "\003Compiling Line: %2d", i);
              postInfo(4, msg);

              // break if the user presses escape

              getEvent(anEvent);
              userBreak = (Boolean)
                          ((anEvent.what == evKeyboard) &&
                           (anEvent.keyDown.keyCode == kbEsc));

            } // endfor

            postInfo(-1, "");
            if (userBreak) postMsg("Exited on user break");

            break;
     ...
   } // endswitch

} // end handleEvent()

Comments and criticisms are welcome, but questions may or may not
be answered (call Borland, they get paid for it!)  Send all
correspondance to Rick Hagerbaumer c/o 72700,351
