These are the differences between the original directory and this one. You
can see what things I changed to make it work. Some of them aren't really
needed.

Only in .: Makefile.am
Only in .: Makefile.in
diff -u ./ascii.cc E:\DJ\CONTRIB\tvision\examples\demo\ascii.cc
--- ./ascii.cc	Mon Jan 19 15:16:34 1998
+++ E:\DJ\CONTRIB\tvision\examples\demo\ascii.cc	Sat Jan  9 20:25:36 1999
@@ -25,15 +25,16 @@
 #define Uses_TStreamable
 #define Uses_TView
 #define Uses_TWindow
-#include <tvision/tv.h>
+#include <tv.h>
 __link( RView )
 __link( RWindow )
 
 #include <string.h>
 #include <stdlib.h>
 #include <ctype.h>
-#include <strstream.h>
-#include <iomanip.h>
+//#include <strstream.h> SET: These two are only to bloat the code.
+//#include <iomanip.h>
+#include <stdio.h>  // SET: printf does the job much simplier
 
 #include "ascii.h"
 
@@ -208,13 +209,17 @@
     TDrawBuffer buf;
     char        color = getColor(6);
     char        str[80];
-    ostrstream  statusStr( str, sizeof str );
 
+    sprintf(str, "%s%c%s%3d%s%2X",
+                 "  Char: ", (asciiChar==0)?(char)0x20:(char)asciiChar,
+                 " Decimal: ", (int)asciiChar,
+                 " Hex ", (int)asciiChar);
+    /* SET: from Robert's port
     statusStr
       << "  Char: " << (char ) ((asciiChar == 0) ? 0x20 : asciiChar)
       << " Decimal: " << setw(3) << (int) asciiChar
       << " Hex " << hex << setiosflags(ios::uppercase)
-      << setw(2) << (int) asciiChar << "     " << ends;
+      << setw(2) << (int) asciiChar << "     " << ends;*/
 
     buf.moveStr(0, str, color);
     writeLine(0, 0, 32, 1, buf);
diff -u ./calc.cc E:\DJ\CONTRIB\tvision\examples\demo\calc.cc
--- ./calc.cc	Mon Jan 19 15:16:34 1998
+++ E:\DJ\CONTRIB\tvision\examples\demo\calc.cc	Sat Jan  9 20:12:38 1999
@@ -24,7 +24,8 @@
 #define Uses_TStreamable
 #define Uses_TView
 #define Uses_TDialog
-#include <tvision/tv.h>
+#define Uses_TPalette // SET: added
+#include <tv.h>
 __link( RView )
 __link( RDialog )
 __link( RButton )
diff -u ./calendar.cc E:\DJ\CONTRIB\tvision\examples\demo\calendar.cc
--- ./calendar.cc	Mon Jan 19 15:16:34 1998
+++ E:\DJ\CONTRIB\tvision\examples\demo\calendar.cc	Sat Jan  9 20:30:12 1999
@@ -22,15 +22,16 @@
 #define Uses_TStreamable
 #define Uses_TView
 #define Uses_TWindow
-#include <tvision/tv.h>
+#include <tv.h>
 __link( RView )
 __link( RWindow )
 
 #include <string.h>
 #include <stdlib.h>
 #include <ctype.h>
-#include <strstream.h>
-#include <iomanip.h>
+//#include <strstream.h>
+//#include <iomanip.h>
+#include <stdio.h>
 #include <time.h>
 
 #include "calendar.h"
@@ -133,49 +134,50 @@
 
 void TCalendarView::draw()
 {
-    char str[23];
-    char current = (char)(1 - dayOfWeek(1, month, year));
-    char days = (char)( daysInMonth[month] +
-                        ((year % 4 == 0 && month == 2) ? 1 : 0) );
+    char str[size.x+1];
+    unsigned current = 1 - dayOfWeek(1, month, year);
+    unsigned  days = daysInMonth[month] + ((year % 4 == 0 && month == 2) ? 1 : 0);
     char color, boldColor;
-    short  i, j;
+    int  i, j;
     TDrawBuffer buf;
 
     color = getColor(6);
     boldColor = getColor(7);
 
-    buf.moveChar(0, ' ', color, 22);
+    buf.moveChar(0, ' ', color, size.x);
 
-    ostrstream( str, sizeof str)
+    sprintf(str, "\036%15s %4d\037", monthNames[month], year);
+    /*ostrstream( str, sizeof str)
       << setw(9) << monthNames[month] << " " << setw(4) << year
-      << " " << (char) 30 << "  " << (char) 31 << " " << ends;
+      << " " << (char) 30 << "  " << (char) 31 << " " << ends;*/
 
     buf.moveStr(0, str, color);
-    writeLine(0, 0, 22, 1, buf);
+    writeLine(0, 0, size.x, 1, buf);
 
-    buf.moveChar(0, ' ', color, 22);
+    buf.moveChar(0, ' ', color, size.x);
     buf.moveStr(0, "Su Mo Tu We Th Fr Sa", color);
-    writeLine(0, 1, 22, 1, buf);
+    writeLine(0, 1, size.x, 1, buf);
 
     for(i = 1; i <= 6; i++)
         {
-        buf.moveChar(0, ' ', color, 22);
+        buf.moveChar(0, ' ', color, size.x);
         for(j = 0; j <= 6; j++)
             {
             if(current < 1 || current > days)
-                buf.moveStr((short)(j*3), "   ", color);
+                buf.moveStr(j*3, "   ", color);
             else
                 {
-                ostrstream( str, sizeof str )
-                  << setw(2) << (int) current << ends;
+                sprintf(str, "%2d", (int)current);
+                /*ostrstream( str, sizeof str ) Ugh! then people say C++ is slow ...
+                  << setw(2) << (int) current << ends;*/
                 if(year == curYear && month == curMonth && current == curDay)
-                    buf.moveStr((short)(j*3), str, boldColor);
+                    buf.moveStr(j*3, str, boldColor);
                 else
-                    buf.moveStr((short)(j*3), str, color);
+                    buf.moveStr(j*3, str, color);
                 }
             current++;
             }
-        writeLine(0, (short)(i+1), 22, 1, buf);
+        writeLine(0, (short)(i+1), size.x, 1, buf);
         }
 }
 
Only in E:\DJ\CONTRIB\tvision\examples\demo\: demo.gpr
diff -u ./fileview.cc E:\DJ\CONTRIB\tvision\examples\demo\fileview.cc
--- ./fileview.cc	Mon Jan 19 15:16:34 1998
+++ E:\DJ\CONTRIB\tvision\examples\demo\fileview.cc	Sat Jan  9 20:30:18 1999
@@ -22,7 +22,7 @@
 #define Uses_TProgram
 #define Uses_TDeskTop
 #define Uses_TStreamableClass
-#include <tvision/tv.h>
+#include <tv.h>
 __link(RScroller)
 __link(RScrollBar)
 
diff -u ./fileview.h E:\DJ\CONTRIB\tvision\examples\demo\fileview.h
--- ./fileview.h	Mon Jan 19 15:16:32 1998
+++ E:\DJ\CONTRIB\tvision\examples\demo\fileview.h	Sat Jan  9 20:30:22 1999
@@ -17,7 +17,7 @@
 #define Uses_TCollection
 #define Uses_TScroller
 #define Uses_TWindow
-#include <tvision/tv.h>
+#include <tv.h>
 
 const hlChangeDir = cmChangeDir;
 
diff -u ./gadgets.cc E:\DJ\CONTRIB\tvision\examples\demo\gadgets.cc
--- ./gadgets.cc	Mon Jan 19 15:16:34 1998
+++ E:\DJ\CONTRIB\tvision\examples\demo\gadgets.cc	Sat Jan  9 20:30:28 1999
@@ -22,7 +22,7 @@
 #define Uses_TRect
 #define Uses_TView
 #define Uses_TDrawBuffer
-#include <tvision/tv.h>
+#include <tv.h>
 
 #include <string.h>
 #include <stdlib.h>
diff -u ./gadgets.h E:\DJ\CONTRIB\tvision\examples\demo\gadgets.h
--- ./gadgets.h	Mon Jan 19 15:16:34 1998
+++ E:\DJ\CONTRIB\tvision\examples\demo\gadgets.h	Sat Jan  9 20:30:30 1999
@@ -17,7 +17,7 @@
 #define Uses_TEvent
 #define Uses_TRect
 #define Uses_TView
-#include <tvision/tv.h>
+#include <tv.h>
 
 
 class THeapView : public TView
Only in E:\DJ\CONTRIB\tvision\examples\demo\: help.cc
Only in E:\DJ\CONTRIB\tvision\examples\demo\: help.h
Only in E:\DJ\CONTRIB\tvision\examples\demo\: helpbase.cc
Only in E:\DJ\CONTRIB\tvision\examples\demo\: helpbase.h
diff -u ./mousedlg.cc E:\DJ\CONTRIB\tvision\examples\demo\mousedlg.cc
--- ./mousedlg.cc	Mon Jan 19 15:16:34 1998
+++ E:\DJ\CONTRIB\tvision\examples\demo\mousedlg.cc	Sat Jan  9 20:38:50 1999
@@ -27,7 +27,8 @@
 #define Uses_TButton
 #define Uses_TSItem
 #define Uses_TEventQueue
-#include <tvision/tv.h>
+#define Uses_TPalette
+#include <tv.h>
 
 #include <string.h>
 #include <stdlib.h>
@@ -65,7 +66,8 @@
 
     if (event.what == evMouseDown)
         {
-        if (event.mouse.eventFlags & meDoubleClick)
+        //if (event.mouse.eventFlags & meDoubleClick) SET:
+        if (event.mouse.doubleClick)
             {
             clicked = (short)((clicked) ? 0 : 1);
             drawView();
Only in .: pp
diff -u ./puzzle.cc E:\DJ\CONTRIB\tvision\examples\demo\puzzle.cc
--- ./puzzle.cc	Mon Jan 19 15:16:34 1998
+++ E:\DJ\CONTRIB\tvision\examples\demo\puzzle.cc	Sat Jan  9 20:39:12 1999
@@ -22,7 +22,8 @@
 #define Uses_TStreamable
 #define Uses_TView
 #define Uses_TWindow
-#include <tvision/tv.h>
+#define Uses_TPalette
+#include <tv.h>
 __link( RView )
 __link( RWindow )
 
Only in E:\DJ\CONTRIB\tvision\examples\demo\: rhide.env
diff -u ./tvdemo.h E:\DJ\CONTRIB\tvision\examples\demo\tvdemo.h
--- ./tvdemo.h	Mon Jan 19 15:16:34 1998
+++ E:\DJ\CONTRIB\tvision\examples\demo\tvdemo.h	Sat Jan  9 21:09:16 1999
@@ -29,7 +29,7 @@
     static TMenuBar *initMenuBar( TRect r );
     virtual void handleEvent(TEvent& Event);
     virtual void getEvent(TEvent& event);
-//    virtual TPalette& getPalette() const;
+    virtual TPalette& getPalette() const;
     virtual void idle();              // Updates heap and clock views
 
 private:
diff -u ./tvdemo1.cc E:\DJ\CONTRIB\tvision\examples\demo\tvdemo1.cc
--- ./tvdemo1.cc	Tue Jul 21 10:48:00 1998
+++ E:\DJ\CONTRIB\tvision\examples\demo\tvdemo1.cc	Sat Jan  9 20:40:28 1999
@@ -28,14 +28,14 @@
 #define Uses_TWindow
 #define Uses_TDeskTop
 
-#include <tvision/tv.h>
+#include <tv.h>
 
 #include "tvdemo.h"
 #include "gadgets.h"
 #include "fileview.h"
 #include "puzzle.h"
 #include "demohelp.h"
-#include <tvision/help.h>
+#include <help.h>
 
 #include <stdio.h>
 #include <string.h>
diff -u ./tvdemo2.cc E:\DJ\CONTRIB\tvision\examples\demo\tvdemo2.cc
--- ./tvdemo2.cc	Mon Jan 19 15:16:34 1998
+++ E:\DJ\CONTRIB\tvision\examples\demo\tvdemo2.cc	Sat Jan  9 21:18:56 1999
@@ -29,7 +29,7 @@
 #define Uses_TChDirDialog
 #define Uses_TScreen
 
-#include <tvision/tv.h>
+#include <tv.h>
 
 #include "tvdemo.h"
 #include "tvcmds.h"
@@ -48,7 +48,7 @@
 void TVDemo::shell()
 {
 	/* SS: this simulates a Ctrl-Z */
-	raise(SIGTSTP);		/* stop the process */
+	/*raise(SIGTSTP);*/		/* stop the process */
 }
 
 
@@ -130,6 +130,22 @@
 }
 
 
+ushort executeDialog( TDialog* pD, void* data=0 )
+{
+    ushort c=cmCancel;
+
+    if (TProgram::application->validView(pD))
+        {
+        if (data)
+        pD->setData(data);
+        c = TProgram::deskTop->execView(pD);
+        if ((c != cmCancel) && (data))
+            pD->getData(data);
+        destroy(pD);
+        }
+
+    return c;
+}
 
 //
 // About Box function()
@@ -338,7 +354,8 @@
 
     TColorGroup &group5 = group1 + group2 + group3 + group4;
 
-    TColorDialog *c = new TColorDialog((TPalette*)0, &group5 );
+    TPalette *temp_pal=new TPalette(getPalette());
+    TColorDialog *c = new TColorDialog(temp_pal, &group5 );
 
     if( validView( c ) != 0 )
     {
@@ -351,4 +368,5 @@
             }
         destroy( c );
     }
+    delete temp_pal;
 }
diff -u ./tvdemo3.cc E:\DJ\CONTRIB\tvision\examples\demo\tvdemo3.cc
--- ./tvdemo3.cc	Tue Jul 21 10:48:16 1998
+++ E:\DJ\CONTRIB\tvision\examples\demo\tvdemo3.cc	Sat Jan  9 21:10:08 1999
@@ -30,7 +30,7 @@
 #define Uses_TDialog
 #define Uses_TEventQueue
 
-#include <tvision/tv.h>
+#include <tv.h>
 
 #include "tvdemo.h"
 #include "tvcmds.h"
@@ -39,7 +39,7 @@
 #include "demohelp.h"
 #include "fileview.h"
 
-#include <tvision/help.h>
+#include <help.h>
 
 //
 // Mouse Control Dialog Box function
@@ -96,12 +96,44 @@
 //
 // getPalette() function ( returns application's palette )
 //
-/*
+#define cpAppColor \
+       "\x71\x70\x78\x74\x20\x28\x24\x17\x1F\x1A\x31\x31\x1E\x71\x1F" \
+    "\x37\x3F\x3A\x13\x13\x3E\x21\x3F\x70\x7F\x7A\x13\x13\x70\x7F\x7E" \
+    "\x70\x7F\x7A\x13\x13\x70\x70\x7F\x7E\x20\x2B\x2F\x78\x2E\x70\x30" \
+    "\x3F\x3E\x1F\x2F\x1A\x20\x72\x31\x31\x30\x2F\x3E\x31\x13\x38\x00" \
+    "\x17\x1F\x1A\x71\x71\x1E\x17\x1F\x1E\x20\x2B\x2F\x78\x2E\x10\x30" \
+    "\x3F\x3E\x70\x2F\x7A\x20\x12\x31\x31\x30\x2F\x3E\x31\x13\x38\x00" \
+    "\x37\x3F\x3A\x13\x13\x3E\x30\x3F\x3E\x20\x2B\x2F\x78\x2E\x30\x70" \
+    "\x7F\x7E\x1F\x2F\x1A\x20\x32\x31\x71\x70\x2F\x7E\x71\x13\x78\x00" \
+    "\x37\x3F\x3A\x13\x13\x30\x3E\x1E"    // help colors
+
+#define cpAppBlackWhite \
+       "\x70\x70\x78\x7F\x07\x07\x0F\x07\x0F\x07\x70\x70\x07\x70\x0F" \
+    "\x07\x0F\x07\x70\x70\x07\x70\x0F\x70\x7F\x7F\x70\x07\x70\x07\x0F" \
+    "\x70\x7F\x7F\x70\x07\x70\x70\x7F\x7F\x07\x0F\x0F\x78\x0F\x78\x07" \
+    "\x0F\x0F\x0F\x70\x0F\x07\x70\x70\x70\x07\x70\x0F\x07\x07\x08\x00" \
+    "\x07\x0F\x0F\x07\x70\x07\x07\x0F\x0F\x70\x78\x7F\x08\x7F\x08\x70" \
+    "\x7F\x7F\x7F\x0F\x70\x70\x07\x70\x70\x70\x07\x7F\x70\x07\x78\x00" \
+    "\x70\x7F\x7F\x70\x07\x70\x70\x7F\x7F\x07\x0F\x0F\x78\x0F\x78\x07" \
+    "\x0F\x0F\x0F\x70\x0F\x07\x70\x70\x70\x07\x70\x0F\x07\x07\x08\x00" \
+    "\x07\x0F\x07\x70\x70\x07\x0F\x70"    // help colors
+
+#define cpAppMonochrome \
+       "\x70\x07\x07\x0F\x70\x70\x70\x07\x0F\x07\x70\x70\x07\x70\x00" \
+    "\x07\x0F\x07\x70\x70\x07\x70\x00\x70\x70\x70\x07\x07\x70\x07\x00" \
+    "\x70\x70\x70\x07\x07\x70\x70\x70\x0F\x07\x07\x0F\x70\x0F\x70\x07" \
+    "\x0F\x0F\x07\x70\x07\x07\x70\x07\x07\x07\x70\x0F\x07\x07\x70\x00" \
+    "\x70\x70\x70\x07\x07\x70\x70\x70\x0F\x07\x07\x0F\x70\x0F\x70\x07" \
+    "\x0F\x0F\x07\x70\x07\x07\x70\x07\x07\x07\x70\x0F\x07\x07\x01\x00" \
+    "\x70\x70\x70\x07\x07\x70\x70\x70\x0F\x07\x07\x0F\x70\x0F\x70\x07" \
+    "\x0F\x0F\x07\x70\x07\x07\x70\x07\x07\x07\x70\x0F\x07\x07\x01\x00" \
+    "\x07\x0F\x07\x70\x70\x07\x0F\x70"    // help colors
+
 TPalette& TVDemo::getPalette() const
 {
-    static TPalette newcolor ( cpAppColor cHelpColor, sizeof( cpAppColor cHelpColor )-1 );
-    static TPalette newblackwhite( cpAppBlackWhite cHelpBlackWhite, sizeof( cpAppBlackWhite cHelpBlackWhite)-1 );
-    static TPalette newmonochrome( cpAppMonochrome cHelpMonochrome, sizeof( cpAppMonochrome cHelpMonochrome)-1 );
+    static TPalette newcolor ( cpAppColor , sizeof( cpAppColor )-1 );
+    static TPalette newblackwhite( cpAppBlackWhite , sizeof( cpAppBlackWhite )-1 );
+    static TPalette newmonochrome( cpAppMonochrome , sizeof( cpAppMonochrome )-1 );
     static TPalette *palettes[] =
         {
         &newcolor,
@@ -111,7 +143,7 @@
     return *(palettes[appPalette]);
 
 }
-*/
+
 
 //
 // isTileable() function ( checks a view on desktop is tileable or not )
