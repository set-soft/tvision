# Makefile for building Turbo Vision library for Win32

# Borland C++
CC       = bcc32
CXX      = $(CC)
INC      = -I.\include -I..\include
CDEBUGFLAGS = -O2 -6 -T -DNDEBUG
#CDEBUGFLAGS = -v -vi -y -6 -Od
# SET: I disabled the condition always true warning. BC++ should be more
#      cleaver about it, code like this: if (1) ... isn't wrong and is
#      obviously on purpose. GCC reports it for things like
#      if ((unsigned)a<0) which is always false and looks like an error ;-)
# Note: SAA disabled Asigned a value that is never used because BC++ is very
#       annoying about assigning a default value for a variable that is
#       assigned latter.
CFLAGS   = -D_BCPP -w-aus -w-ccc -a8 -WR -WC -X -q $(CDEBUGFLAGS)
# SET: I disabled the member x hides virtual member y because I don't
#      understand why it is reported if you know please explain me.
CXXFLAGS = -P -RT- -w-hid $(CFLAGS)
LFLAGS   = -WR -WC

# Microsoft Visual C++
# NOTE: does not work yet
#CXX      = cl
#INC      = -I.\include\msvc -I.\include -I..\include
#CXXFLAGS = -c -TP -D_MSVC -MDd

.SUFFIXES: .cc .c .obj

TV_OBJS_ = +tdisplay.obj +tfont.obj +thwmouse.obj +tscreen.obj +gkeyw32.obj \
+calcdisp.obj   +calculat.obj   +fpbase.obj     +fpstream.obj   +ifpstrea.obj \
+iopstrea.obj   +ipstream.obj   +ofpstrea.obj   +opstream.obj   +pstream.obj \
+tapplica.obj   +tbackgro.obj   +tbutton.obj    +tchdirdi.obj   +tcheckbo.obj \
+tclrdisp.obj   +tcluster.obj   +tcollect.obj   +tcolordi.obj   +tcolorgr.obj \
+tcolorit.obj   +tcolorse.obj   +tcommand.obj   +tdesktop.obj   +tdialog.obj \
+tdircoll.obj   +tdirlist.obj   +tdrawbuf.obj                   +teditor.obj \
+teditorf.obj   +teditwin.obj   +tevent.obj     +tfilecol.obj   +tfiledia.obj \
+tfileedi.obj   +tfileinf.obj   +tfileinp.obj   +tfilelis.obj   +tfilterv.obj \
+tfont.obj      +tframe.obj     +tgroup.obj     +thistory.obj   +thistvie.obj \
+thistwin.obj   +thwmouse.obj   +tindicat.obj   +tinputli.obj   +tlabel.obj \
+tlistbox.obj   +tlistvie.obj   +tmemo.obj      +tmenubar.obj   +tmenubox.obj \
+tmenuvie.obj   +tmonosel.obj   +tmouse.obj     +tnscolle.obj   +tnssorte.obj \
+tobject.obj    +tpalette.obj   +tparamte.obj   +tpoint.obj     +tprogini.obj \
+tprogram.obj   +tpwobj.obj     +tpwreado.obj   +tpwritte.obj   +tradiobu.obj \
+trangeva.obj   +trescoll.obj   +tresfile.obj                   +tscrollb.obj \
+tscrolle.obj   +tsortedc.obj   +tsortedl.obj   +tstatict.obj   +tstatusd.obj \
+tstatusl.obj   +tstrinde.obj   +tstringc.obj   +tstringl.obj   +tstrlist.obj \
+tstrmcla.obj   +tstrmtyp.obj   +tsubmenu.obj   +ttermina.obj   +ttextdev.obj \
+tvalidat.obj   +tvedit1.obj    +tvedit2.obj    +tvedit3.obj    +tview.obj \
+tvtext1.obj    +tvtext2.obj    +twindow.obj    
TV_OBJS = $(TV_OBJS_:+=objs\)


TVN_OBJS_ = \
+nbackgro.obj   +nbutton.obj    +ncalcdis.obj   +ncalcula.obj   +nchdirdi.obj \
+ncheckbo.obj   +nclrdisp.obj   +ncluster.obj   +ncollect.obj   +ncolordi.obj \
+ncolorgr.obj   +ncolorit.obj   +ncolorse.obj   +ndesktop.obj   +ndialog.obj \
+ndircoll.obj   +ndirlist.obj   +neditor.obj    +neditwi.obj    +nfilecol.obj \
+nfiledia.obj   +nfiledtr.obj   +nfileinf.obj   +nfileinp.obj   +nfilelis.obj \
+nfilterv.obj   +nframe.obj     +ngroup.obj     +nhistory.obj   +nindicat.obj \
+ninputli.obj   +nlabel.obj     +nlistbox.obj   +nlistvie.obj   +nmemo.obj \
+nmenubar.obj   +nmenubox.obj   +nmenuvie.obj   +nmonosel.obj   +nparamte.obj \
+nradiobu.obj   +nrangeva.obj   +nresourc.obj   +nscrollb.obj   +nscrolle.obj \
+nsortedc.obj   +nsortedl.obj   +nstatict.obj   +nstatusl.obj   +nstringc.obj \
+nstringl.obj   +nvalidat.obj   +nview.obj      +nwindow.obj    
TVN_OBJS = $(TVN_OBJS_:+=objs\)

TVS_OBJS_ = \
+sbackgro.obj   +sbutton.obj    +scalcdis.obj   +scalcula.obj   +schdirdi.obj \
+scheckbo.obj   +sclrdisp.obj   +scluster.obj   +scolordi.obj   +scolorgr.obj \
+scolorit.obj   +scolorse.obj   +sdesktop.obj   +sdialog.obj    +sdircoll.obj \
+sdirlist.obj   +seditor.obj    +seditwin.obj   +sfilecol.obj   +sfiledia.obj \
+sfileedi.obj   +sfileinf.obj   +sfileinp.obj   +sfilelis.obj   +sfilterv.obj \
+sframe.obj     +sgroup.obj     +shistory.obj   +sindicat.obj   +sinputli.obj \
+slabel.obj     +slistbox.obj   +slistvie.obj   +smemo.obj      +smenubar.obj \
+smenubox.obj   +smenuvie.obj   +smonosel.obj   +sparamte.obj   +sradiobu.obj \
+srangeva.obj   +sresourc.obj   +sscrollb.obj   +sscrolle.obj   +sstatict.obj \
+sstatusl.obj   +sstringc.obj   +sstringl.obj   +sview.obj      +swindow.obj
TVS_OBJS = $(TVS_OBJS_:+=objs\)

TVM_OBJS_ = \
+drivers2.obj +dual.obj +histlist.obj +misc.obj +msgbox.obj +newstr.obj +gkey.obj
TVM_OBJS = $(TVM_OBJS_:+=objs\)

MISC_OBJS_ = +fixpath.obj +fnmatch.obj +glob.obj +globfree.obj +unc.obj \
+getopt.obj +yieldpro.obj +ticks.obj +releasec.obj +filelength.obj \
+expandpath.obj +fexpand.obj +isrelpath.obj +drivevalid.obj +isdir.obj \
+pathvalid.obj +validfilename.obj +iswild.obj +fileexists.obj +getopt1.obj \
+regex.obj +getcurdir.obj +ntdirent.obj +beep.obj +fileattrs.obj +mkstemp.obj
MISC_OBJS = $(MISC_OBJS_:+=objs\)

OBJS = $(TV_OBJS) $(TVN_OBJS) $(TVS_OBJS) $(TVM_OBJS) $(MISC_OBJS)

all: rhtv.lib tvex.exe

rhtv.lib: $(OBJS)
	del $@
	tlib /E /P256 $@ @&&!
+$(**: = &^
+)
!

tvex.exe: objs\tvex.obj rhtv.lib
	$(CC) $(LFLAGS) -etvex.exe objs\tvex.obj rhtv.lib
	
{.}.cc{objs}.obj:
	$(CXX) -c $(CXXFLAGS) $(INC) -nobjs $<

{..\classes}.cc{objs}.obj:
	$(CXX) -c $(CXXFLAGS) $(INC) -nobjs $<

{..\classes\WinNT}.cc{objs}.obj:
	$(CXX) -c $(CXXFLAGS) $(INC) -nobjs $<

{..\classes\WinNT}.cc{objs}.obj:
	$(CXX) -c $(CXXFLAGS) $(INC) -nobjs $<
	
{..\names}.cc{objs}.obj:
	$(CXX) -c $(CXXFLAGS) $(INC) -nobjs $<
	
{..\stream}.cc{objs}.obj:
	$(CXX) -c $(CXXFLAGS) $(INC) -nobjs $<
	
{..}.cc{objs}.obj:
	$(CXX) -c $(CXXFLAGS) $(INC) -nobjs $<
	
{..\compat}.c{objs}.obj:
	$(CC) -c $(CFLAGS) $(INC) -nobjs $<
	
objs\tscreen.obj: ..\classes\WinNT\tscreen.cc
objs\tdsiplay.obj: ..\classes\WinNT\tdsiplay.cc
objs\tfont.obj: ..\classes\WinNT\tfont.cc
objs\thwmouse.obj: ..\classes\WinNT\thwmouse.cc

clean:
	-del objs\*.obj
	-del *.obj
	-del *.exe
	-del *.tds
	-del *.lib
	