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
#include <tv.h>
#include <tv/tvedit.h>
#define USEUNIT(f)

USEUNIT("calcdisp.cpp");
USEUNIT("calculat.cpp");
USEUNIT("fpbase.cpp");
USEUNIT("fpstream.cpp");
USEUNIT("ifpstrea.cpp");
USEUNIT("iopstrea.cpp");
USEUNIT("ipstream.cpp");
USEUNIT("ofpstrea.cpp");
USEUNIT("opstream.cpp");
USEUNIT("pstream.cpp");
USEUNIT("tapplica.cpp");
USEUNIT("tbackgro.cpp");
USEUNIT("tbutton.cpp");
USEUNIT("tchdirdi.cpp");
USEUNIT("tcheckbo.cpp");
USEUNIT("tclrdisp.cpp");
USEUNIT("tcluster.cpp");
USEUNIT("tcollect.cpp");
USEUNIT("tcolordi.cpp");
USEUNIT("tcolorgr.cpp");
USEUNIT("tcolorit.cpp");
USEUNIT("tcolorse.cpp");
USEUNIT("tcommand.cpp");
USEUNIT("tdesktop.cpp");
USEUNIT("tdialog.cpp");
USEUNIT("tdircoll.cpp");
USEUNIT("tdirlist.cpp");
USEUNIT("tdisplay.cpp");
USEUNIT("tdrawbuf.cpp");
USEUNIT("teditor.cpp");
USEUNIT("teditorf.cpp");
USEUNIT("teditwin.cpp");
USEUNIT("tevent.cpp");
USEUNIT("tfilecol.cpp");
USEUNIT("tfiledia.cpp");
USEUNIT("tfileedi.cpp");
USEUNIT("tfileinf.cpp");
USEUNIT("tfileinp.cpp");
USEUNIT("tfilelis.cpp");
USEUNIT("tfilterv.cpp");
USEUNIT("tfont.cpp");
USEUNIT("tframe.cpp");
USEUNIT("tgroup.cpp");
USEUNIT("thistory.cpp");
USEUNIT("thistvie.cpp");
USEUNIT("thistwin.cpp");
USEUNIT("thwmouse.cpp");
USEUNIT("tindicat.cpp");
USEUNIT("tinputli.cpp");
USEUNIT("tlabel.cpp");
USEUNIT("tlistbox.cpp");
USEUNIT("tlistvie.cpp");
USEUNIT("tmemo.cpp");
USEUNIT("tmenubar.cpp");
USEUNIT("tmenubox.cpp");
USEUNIT("tmenuvie.cpp");
USEUNIT("tmonosel.cpp");
USEUNIT("tmouse.cpp");
USEUNIT("tnscolle.cpp");
USEUNIT("tnssorte.cpp");
USEUNIT("tobject.cpp");
USEUNIT("tpalette.cpp");
USEUNIT("tparamte.cpp");
USEUNIT("tpoint.cpp");
USEUNIT("tprogini.cpp");
USEUNIT("tprogram.cpp");
USEUNIT("tpwobj.cpp");
USEUNIT("tpwreado.cpp");
USEUNIT("tpwritte.cpp");
USEUNIT("tradiobu.cpp");
USEUNIT("trangeva.cpp");
USEUNIT("trescoll.cpp");
USEUNIT("tresfile.cpp");
USEUNIT("tscreen.cpp");
USEUNIT("tscrollb.cpp");
USEUNIT("tscrolle.cpp");
USEUNIT("tsortedc.cpp");
USEUNIT("tsortedl.cpp");
USEUNIT("tstatict.cpp");
USEUNIT("tstatusd.cpp");
USEUNIT("tstatusl.cpp");
USEUNIT("tstrinde.cpp");
USEUNIT("tstringc.cpp");
USEUNIT("tstringl.cpp");
USEUNIT("tstrlist.cpp");
USEUNIT("tstrmcla.cpp");
USEUNIT("tstrmtyp.cpp");
USEUNIT("tsubmenu.cpp");
USEUNIT("ttermina.cpp");
USEUNIT("ttextdev.cpp");
USEUNIT("tvalidat.cpp");
USEUNIT("tvedit1.cpp");
USEUNIT("tvedit2.cpp");
USEUNIT("tvedit3.cpp");
USEUNIT("tview.cpp");
USEUNIT("tvtext1.cpp");
USEUNIT("tvtext2.cpp");
USEUNIT("twindow.cpp");
USEUNIT("gkeyw32.cpp");
USEUNIT("ticks.cpp");
USEUNIT("gkey.cpp");
USEUNIT("dual.cpp");
USEUNIT("names\nbackgro.cpp");
USEUNIT("names\nbutton.cpp");
USEUNIT("names\ncalcdis.cpp");
USEUNIT("names\ncalcula.cpp");
USEUNIT("names\nchdirdi.cpp");
USEUNIT("names\ncheckbo.cpp");
USEUNIT("names\nclrdisp.cpp");
USEUNIT("names\ncluster.cpp");
USEUNIT("names\ncollect.cpp");
USEUNIT("names\ncolordi.cpp");
USEUNIT("names\ncolorgr.cpp");
USEUNIT("names\ncolorit.cpp");
USEUNIT("names\ncolorse.cpp");
USEUNIT("names\ndesktop.cpp");
USEUNIT("names\ndialog.cpp");
USEUNIT("names\ndircoll.cpp");
USEUNIT("names\ndirlist.cpp");
USEUNIT("names\neditor.cpp");
USEUNIT("names\neditwi.cpp");
USEUNIT("names\nfilecol.cpp");
USEUNIT("names\nfiledia.cpp");
USEUNIT("names\nfiledtr.cpp");
USEUNIT("names\nfileinf.cpp");
USEUNIT("names\nfileinp.cpp");
USEUNIT("names\nfilelis.cpp");
USEUNIT("names\nfilterv.cpp");
USEUNIT("names\nframe.cpp");
USEUNIT("names\ngroup.cpp");
USEUNIT("names\nhistory.cpp");
USEUNIT("names\nindicat.cpp");
USEUNIT("names\ninputli.cpp");
USEUNIT("names\nlabel.cpp");
USEUNIT("names\nlistbox.cpp");
USEUNIT("names\nlistvie.cpp");
USEUNIT("names\nmemo.cpp");
USEUNIT("names\nmenubar.cpp");
USEUNIT("names\nmenubox.cpp");
USEUNIT("names\nmenuvie.cpp");
USEUNIT("names\nmonosel.cpp");
USEUNIT("names\nparamte.cpp");
USEUNIT("names\nradiobu.cpp");
USEUNIT("names\nrangeva.cpp");
USEUNIT("names\nresourc.cpp");
USEUNIT("names\nscrollb.cpp");
USEUNIT("names\nscrolle.cpp");
USEUNIT("names\nsortedc.cpp");
USEUNIT("names\nsortedl.cpp");
USEUNIT("names\nstatict.cpp");
USEUNIT("names\nstatusl.cpp");
USEUNIT("names\nstringc.cpp");
USEUNIT("names\nstringl.cpp");
USEUNIT("names\nvalidat.cpp");
USEUNIT("names\nview.cpp");
USEUNIT("names\nwindow.cpp");
USEUNIT("misc.cpp");
USEUNIT("msgbox.cpp");
USEUNIT("histlist.cpp");
USEUNIT("drivers2.cpp");
USEUNIT("newstr.cpp");
USEUNIT("bc.cpp");
USEUNIT("..\lib\fnmatch.c");
USEUNIT("..\lib\globfree.c");
USEUNIT("..\lib\glob.c");
USEUNIT("..\lib\fixpath.c");
//---------------------------------------------------------------------------
int main()
{
 TEditorApp *myApp=new TEditorApp();
 if (myApp)
   {
    myApp->run();
    delete myApp;
   }
 return 0;
}

