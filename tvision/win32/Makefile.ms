# Makefile for MS VC 5.0-6.0

CFLAGS=-O2 -D_WIN32 /GR- /GX-
CXXFLAGS=-O2 -I../include -D_WIN32 /GR- /GX-

OBJDIR=obj_ms

vpath %.cc ../names ../stream ./ ../ ../classes/win32 ../classes ../examples/tutorial
vpath %.o ./$(OBJDIR)

CC=cl /nologo
CXX=cl /nologo /TP

OBJS=$(OBJDIR)/calcdisp.o $(OBJDIR)/calculat.o $(OBJDIR)/drivers2.o $(OBJDIR)/dual.o\
	$(OBJDIR)/fileview.o $(OBJDIR)/fpbase.o $(OBJDIR)/fpstream.o $(OBJDIR)/histlist.o\
	$(OBJDIR)/ifpstrea.o $(OBJDIR)/iopstrea.o $(OBJDIR)/ipstream.o $(OBJDIR)/misc.o\
	$(OBJDIR)/mouse.o $(OBJDIR)/msgbox.o $(OBJDIR)/nbackgro.o $(OBJDIR)/nbutton.o\
	$(OBJDIR)/ncalcdis.o $(OBJDIR)/ncalcula.o $(OBJDIR)/nchdirdi.o $(OBJDIR)/ncheckbo.o\
    $(OBJDIR)/nclrdisp.o $(OBJDIR)/ncluster.o $(OBJDIR)/ncollect.o $(OBJDIR)/ncolordi.o\
    $(OBJDIR)/ncolorgr.o $(OBJDIR)/ncolorit.o $(OBJDIR)/ncolorse.o $(OBJDIR)/ndesktop.o\
    $(OBJDIR)/ndialog.o $(OBJDIR)/ndircoll.o $(OBJDIR)/ndirlist.o $(OBJDIR)/neditor.o\
    $(OBJDIR)/neditwi.o $(OBJDIR)/newstr.o $(OBJDIR)/nfilecol.o $(OBJDIR)/nfiledia.o\
    $(OBJDIR)/nfiledtr.o $(OBJDIR)/nfileinf.o $(OBJDIR)/nfileinp.o $(OBJDIR)/nfilelis.o\
    $(OBJDIR)/nfilterv.o $(OBJDIR)/nframe.o $(OBJDIR)/ngroup.o $(OBJDIR)/nhistory.o\
    $(OBJDIR)/nindicat.o $(OBJDIR)/ninputli.o $(OBJDIR)/nlabel.o $(OBJDIR)/nlistbox.o\
    $(OBJDIR)/nlistvie.o $(OBJDIR)/nmemo.o $(OBJDIR)/nmenubar.o $(OBJDIR)/nmenubox.o\
    $(OBJDIR)/nmenuvie.o $(OBJDIR)/nmonosel.o $(OBJDIR)/nparamte.o $(OBJDIR)/nradiobu.o\
    $(OBJDIR)/nrangeva.o $(OBJDIR)/nresourc.o $(OBJDIR)/nscrollb.o $(OBJDIR)/nscrolle.o\
    $(OBJDIR)/nsortedc.o $(OBJDIR)/nsortedl.o $(OBJDIR)/nstatict.o $(OBJDIR)/nstatusl.o\
    $(OBJDIR)/nstringc.o $(OBJDIR)/nstringl.o $(OBJDIR)/nvalidat.o $(OBJDIR)/nview.o\
    $(OBJDIR)/nwindow.o $(OBJDIR)/ofpstrea.o $(OBJDIR)/opstream.o $(OBJDIR)/pstream.o\
    $(OBJDIR)/rhscreen.o $(OBJDIR)/sbackgro.o $(OBJDIR)/sbutton.o $(OBJDIR)/scalcdis.o\
    $(OBJDIR)/scalcula.o $(OBJDIR)/schdirdi.o $(OBJDIR)/scheckbo.o $(OBJDIR)/sclrdisp.o\
    $(OBJDIR)/scluster.o $(OBJDIR)/scolordi.o $(OBJDIR)/scolorgr.o $(OBJDIR)/scolorit.o\
    $(OBJDIR)/scolorse.o $(OBJDIR)/screen.o $(OBJDIR)/sdesktop.o $(OBJDIR)/sdialog.o\
    $(OBJDIR)/sdircoll.o $(OBJDIR)/sdirlist.o $(OBJDIR)/seditor.o $(OBJDIR)/seditwin.o\
    $(OBJDIR)/sescreen.o $(OBJDIR)/sfilecol.o $(OBJDIR)/sfiledia.o $(OBJDIR)/sfileedi.o\
    $(OBJDIR)/sfileinf.o $(OBJDIR)/sfileinp.o $(OBJDIR)/sfilelis.o $(OBJDIR)/sfilterv.o\
    $(OBJDIR)/sframe.o $(OBJDIR)/sgroup.o $(OBJDIR)/shistory.o $(OBJDIR)/sindicat.o\
    $(OBJDIR)/sinputli.o $(OBJDIR)/slabel.o $(OBJDIR)/slistbox.o $(OBJDIR)/slistvie.o\
    $(OBJDIR)/smemo.o $(OBJDIR)/smenubar.o $(OBJDIR)/smenubox.o $(OBJDIR)/smenuvie.o\
    $(OBJDIR)/smonosel.o $(OBJDIR)/sparamte.o $(OBJDIR)/sradiobu.o $(OBJDIR)/srangeva.o\
    $(OBJDIR)/sresourc.o $(OBJDIR)/sscrollb.o $(OBJDIR)/sscrolle.o $(OBJDIR)/sstatict.o\
    $(OBJDIR)/sstatusl.o $(OBJDIR)/sstringc.o $(OBJDIR)/sstringl.o $(OBJDIR)/sview.o\
    $(OBJDIR)/swindow.o $(OBJDIR)/tapplica.o $(OBJDIR)/tbackgro.o $(OBJDIR)/tbutton.o\
    $(OBJDIR)/tchdirdi.o $(OBJDIR)/tcheckbo.o $(OBJDIR)/tclrdisp.o $(OBJDIR)/tcluster.o\
    $(OBJDIR)/tcollect.o $(OBJDIR)/tcolordi.o $(OBJDIR)/tcolorgr.o $(OBJDIR)/tcolorit.o\
    $(OBJDIR)/tcolorse.o $(OBJDIR)/tcommand.o $(OBJDIR)/tdesktop.o $(OBJDIR)/tdialog.o\
    $(OBJDIR)/tdircoll.o $(OBJDIR)/tdirlist.o $(OBJDIR)/tdisplay.o $(OBJDIR)/tdrawbuf.o\
    $(OBJDIR)/teditor.o $(OBJDIR)/teditorf.o $(OBJDIR)/teditwin.o $(OBJDIR)/tevent.o\
    $(OBJDIR)/tfilecol.o $(OBJDIR)/tfiledia.o $(OBJDIR)/tfileedi.o $(OBJDIR)/tfileinf.o\
    $(OBJDIR)/tfileinp.o $(OBJDIR)/tfilterv.o $(OBJDIR)/tfont.o\
    $(OBJDIR)/tframe.o $(OBJDIR)/tgroup.o $(OBJDIR)/thistory.o $(OBJDIR)/thistvie.o\
    $(OBJDIR)/thistwin.o $(OBJDIR)/thwmouse.o $(OBJDIR)/ticks.o $(OBJDIR)/tindicat.o\
    $(OBJDIR)/tinputli.o $(OBJDIR)/tlabel.o $(OBJDIR)/tlistbox.o $(OBJDIR)/tlistvie.o\
    $(OBJDIR)/tmemo.o $(OBJDIR)/tmenubar.o $(OBJDIR)/tmenubox.o $(OBJDIR)/tmenuvie.o\
    $(OBJDIR)/tmonosel.o $(OBJDIR)/tmouse.o $(OBJDIR)/tnscolle.o $(OBJDIR)/tnssorte.o\
    $(OBJDIR)/tobject.o $(OBJDIR)/tpalette.o $(OBJDIR)/tparamte.o $(OBJDIR)/tpoint.o\
    $(OBJDIR)/tprogini.o $(OBJDIR)/tprogram.o $(OBJDIR)/tpwobj.o $(OBJDIR)/tpwreado.o\
    $(OBJDIR)/tpwritte.o $(OBJDIR)/tradiobu.o $(OBJDIR)/trangeva.o $(OBJDIR)/trescoll.o\
    $(OBJDIR)/tresfile.o $(OBJDIR)/tscreen.o $(OBJDIR)/tscrollb.o $(OBJDIR)/tscrolle.o\
    $(OBJDIR)/tsortedc.o $(OBJDIR)/tsortedl.o $(OBJDIR)/tstatict.o $(OBJDIR)/tstatusd.o\
    $(OBJDIR)/tstatusl.o $(OBJDIR)/tstrinde.o $(OBJDIR)/tstringc.o $(OBJDIR)/tstringl.o\
    $(OBJDIR)/tstrlist.o $(OBJDIR)/tstrmcla.o $(OBJDIR)/tstrmtyp.o $(OBJDIR)/tsubmenu.o\
    $(OBJDIR)/ttermina.o $(OBJDIR)/ttextdev.o $(OBJDIR)/tvalidat.o $(OBJDIR)/tvedit1.o\
    $(OBJDIR)/tvedit2.o $(OBJDIR)/tvedit3.o $(OBJDIR)/tview.o $(OBJDIR)/tvtext1.o\
    $(OBJDIR)/tvtext2.o $(OBJDIR)/twindow.o $(OBJDIR)/bc.o $(OBJDIR)/gkey.o\
	$(OBJDIR)/tfilelis.o $(OBJDIR)/console.o $(OBJDIR)/trans.o

$(OBJDIR)/%.o: %.cc
	$(COMPILE.cc) $(subst /,\\,$<) -Fo$(subst /,\\,$@)

all:: libtv.lib

libtv.lib : $(OBJS)
	-rm -f libtv.lib
	lib /OUT:libtv.lib $(OBJS)

clean:
	rm -f $(OBJS)
	rm -f $(TUTORIAL)

