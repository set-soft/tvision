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
#define Uses_fpstream
#define Uses_stdio
#include <tv.h>
#include <tv/tvedit.h>

int main()
{
 TEditorApp *myApp=new TEditorApp();

#if defined (__BORLANDC__)
 fprintf(stderr,"This test was compiled using BC++ %d.%d.%d\n",
         (__BORLANDC__/0x100) & 0xF,
         (__BORLANDC__/0x10)  & 0xF,
          __BORLANDC__ & 0xF);
#elif defined (_MSC_VER)
 fprintf(stderr,"This test was compiled using MSVC++ compiler v%d.%d\n",
         (_MSC_VER/100) & 0xFF,
         (_MSC_VER - ((_MSC_VER/100) & 0xFF)*100));
#endif

 if (myApp)
   {
    // It works if we are windowed and the screen is large enough:
    // myApp->setScreenMode(90,30);
    myApp->run();
    delete myApp;
   }

 // This is a small test to check if streams are working
 #ifdef TEST_STREAM
 fpstream *f=new fpstream("Test.dat",CLY_IOSOut | CLY_IOSBin);
 *f << 20;
 f->close();
 #endif

 return 0;
}

