# Makefile for gcc-2.95.2 (mingw32)
CFLAGS=-O2 -gstabs+3 -D_WIN32
CXXFLAGS=-O2 -gstabs+3 -D_WIN32

vpath %.cc ../names ../stream ./ ../ ../classes/win32 ../classes ../examples/tutorial ../examples/demo
vpath %.o ./obj ./obj_demo

OBJS=obj/calcdisp.o obj/calculat.o obj/drivers2.o obj/dual.o\
	obj/fileview.o obj/fpbase.o obj/fpstream.o obj/histlist.o\
	obj/ifpstrea.o obj/iopstrea.o obj/ipstream.o obj/misc.o\
	obj/mouse.o obj/msgbox.o obj/nbackgro.o obj/nbutton.o\
	obj/ncalcdis.o obj/ncalcula.o obj/nchdirdi.o obj/ncheckbo.o\
	obj/nclrdisp.o obj/ncluster.o obj/ncollect.o obj/ncolordi.o\
	obj/ncolorgr.o obj/ncolorit.o obj/ncolorse.o obj/ndesktop.o\
	obj/ndialog.o obj/ndircoll.o obj/ndirlist.o obj/neditor.o\
	obj/neditwi.o obj/newstr.o obj/nfilecol.o obj/nfiledia.o\
	obj/nfiledtr.o obj/nfileinf.o obj/nfileinp.o obj/nfilelis.o\
	obj/nfilterv.o obj/nframe.o obj/ngroup.o obj/nhistory.o\
	obj/nindicat.o obj/ninputli.o obj/nlabel.o obj/nlistbox.o\
	obj/nlistvie.o obj/nmemo.o obj/nmenubar.o obj/nmenubox.o\
	obj/nmenuvie.o obj/nmonosel.o obj/nparamte.o obj/nradiobu.o\
	obj/nrangeva.o obj/nresourc.o obj/nscrollb.o obj/nscrolle.o\
	obj/nsortedc.o obj/nsortedl.o obj/nstatict.o obj/nstatusl.o\
	obj/nstringc.o obj/nstringl.o obj/nvalidat.o obj/nview.o\
	obj/nwindow.o obj/ofpstrea.o obj/opstream.o obj/pstream.o\
	obj/rhscreen.o obj/sbackgro.o obj/sbutton.o obj/scalcdis.o\
	obj/scalcula.o obj/schdirdi.o obj/scheckbo.o obj/sclrdisp.o\
	obj/scluster.o obj/scolordi.o obj/scolorgr.o obj/scolorit.o\
	obj/scolorse.o obj/screen.o obj/sdesktop.o obj/sdialog.o\
	obj/sdircoll.o obj/sdirlist.o obj/seditor.o obj/seditwin.o\
	obj/sescreen.o obj/sfilecol.o obj/sfiledia.o obj/sfileedi.o\
	obj/sfileinf.o obj/sfileinp.o obj/sfilelis.o obj/sfilterv.o\
	obj/sframe.o obj/sgroup.o obj/shistory.o obj/sindicat.o\
	obj/sinputli.o obj/slabel.o obj/slistbox.o obj/slistvie.o\
	obj/smemo.o obj/smenubar.o obj/smenubox.o obj/smenuvie.o\
	obj/smonosel.o obj/sparamte.o obj/sradiobu.o obj/srangeva.o\
	obj/sresourc.o obj/sscrollb.o obj/sscrolle.o obj/sstatict.o\
	obj/sstatusl.o obj/sstringc.o obj/sstringl.o obj/sview.o\
	obj/swindow.o obj/tapplica.o obj/tbackgro.o obj/tbutton.o\
	obj/tchdirdi.o obj/tcheckbo.o obj/tclrdisp.o obj/tcluster.o\
	obj/tcollect.o obj/tcolordi.o obj/tcolorgr.o obj/tcolorit.o\
	obj/tcolorse.o obj/tcommand.o obj/tdesktop.o obj/tdialog.o\
	obj/tdircoll.o obj/tdirlist.o obj/tdisplay.o obj/tdrawbuf.o\
	obj/teditor.o obj/teditorf.o obj/teditwin.o obj/tevent.o\
	obj/tfilecol.o obj/tfiledia.o obj/tfileedi.o obj/tfileinf.o\
	obj/tfileinp.o obj/tfilterv.o obj/tfont.o\
	obj/tframe.o obj/tgroup.o obj/thistory.o obj/thistvie.o\
	obj/thistwin.o obj/thwmouse.o obj/ticks.o obj/tindicat.o\
	obj/tinputli.o obj/tlabel.o obj/tlistbox.o obj/tlistvie.o\
	obj/tmemo.o obj/tmenubar.o obj/tmenubox.o obj/tmenuvie.o\
	obj/tmonosel.o obj/tmouse.o obj/tnscolle.o obj/tnssorte.o\
	obj/tobject.o obj/tpalette.o obj/tparamte.o obj/tpoint.o\
	obj/tprogini.o obj/tprogram.o obj/tpwobj.o obj/tpwreado.o\
	obj/tpwritte.o obj/tradiobu.o obj/trangeva.o obj/trescoll.o\
	obj/tresfile.o obj/tscreen.o obj/tscrollb.o obj/tscrolle.o\
	obj/tsortedc.o obj/tsortedl.o obj/tstatict.o obj/tstatusd.o\
	obj/tstatusl.o obj/tstrinde.o obj/tstringc.o obj/tstringl.o\
	obj/tstrlist.o obj/tstrmcla.o obj/tstrmtyp.o obj/tsubmenu.o\
	obj/ttermina.o obj/ttextdev.o obj/tvalidat.o obj/tvedit1.o\
	obj/tvedit2.o obj/tvedit3.o obj/tview.o obj/tvtext1.o\
	obj/tvtext2.o obj/twindow.o obj/bc.o obj/gkey.o\
	obj/tfilelis.o obj/console.o obj/trans.o

DEMO_OBJS=./obj_demo/ascii.o ./obj_demo/calendar.o ./obj_demo/fileview.o\
	./obj_demo/gadgets.o ./obj_demo/help.o ./obj_demo/helpbase.o\
	./obj_demo/mousedlg.o ./obj_demo/puzzle.o ./obj_demo/tvdemo1.o\
	./obj_demo/tvdemo2.o ./obj_demo/tvdemo3.o

LIBS=-lrhtv -lstdc++
LDFLAGS=-L./

obj/%.o: %.cc
	$(COMPILE.cc) -I../include $< $(OUTPUT_OPTION)

obj_demo/%.o: %.cc
	$(COMPILE.cc) -I../include -I../examples/demo $< $(OUTPUT_OPTION)

all:: librhtv.a

librhtv.a : $(OBJS)
	-rm -f librhtv.a
	ar crv librhtv.a $(OBJS)

demo.exe: $(DEMO_OBJS) librhtv.a
	gcc $(LDFLAGS) -mconsole $(DEMO_OBJS) -o $@ $(LIBS)
   
clean:
	rm -f $(OBJS)
	rm -f $(TUTORIAL)

