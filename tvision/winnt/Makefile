# Makefile for building Turbo Vision library for Win32



# Borland C++
CC       = bcc32
LD       = bcc32
CXX      = $(CC)
INC      = -I.\include -I..\include

!ifdef DEBUG
CDEBUGFLAGS = -v -vi -y -6 -Od
!else
CDEBUGFLAGS = -O2 -6 -T -DNDEBUG
!endif

!ifdef DYNRTL
DYNRTLDFLAGS = -WR
!endif

# SET: I disabled the condition always true warning. BC++ should be more
#      cleaver about it, code like this: if (1) ... isn't wrong and is
#      obviously on purpose. GCC reports it for things like
#      if ((unsigned)a<0) which is always false and looks like an error ;-)
# Note: SAA disabled Asigned a value that is never used because BC++ is very
#       annoying about assigning a default value for a variable that is
#       assigned latter.
CFLAGS   = -D_BCPP -w-aus -w-ccc -a8 $(DYNRTLDFLAGS) -WC -X -q $(CDEBUGFLAGS)
# SET: I disabled the member x hides virtual member y because I don't
#      understand why it is reported if you know please explain me.
CXXFLAGS = -P -RT- -w-hid $(CFLAGS)
LDFLAGS   = $(DYNRTLDFLAGS) -WC

# Microsoft Visual C++
# NOTE: does not work yet
#CXX      = cl
#INC      = -I.\include\msvc -I.\include -I..\include
#CXXFLAGS = -c -TP -D_MSVC -MDd

.SUFFIXES: .cc .c .obj

TV_OBJS_ =  +beep.obj +drivevalid.obj +expandpath.obj +fexpand.obj +fileattrs.obj\
+fileexists.obj +filelength.obj +fixpath.obj +fnmatch.obj +getcurdir.obj +getline.obj\
+getopt.obj +getopt1.obj +getshell.obj +glob.obj +globfree.obj +iffilelen.obj +isdir.obj\
+isrelpath.obj +iswild.obj +itoa.obj +mkstemp.obj +nl_langinfo.obj +ntdirent.obj\
+pathvalid.obj +regex.obj +releasec.obj +strlwr.obj +strupr.obj +ticks.obj +unc.obj\
+unc_95.obj +unc_nt.obj +validfilename.obj +yieldpro.obj +calcdisp.obj +calculat.obj\
+dosdis.obj +doskey.obj +dosmouse.obj +dosscr.obj +drivers2.obj +dual.obj\
+fileview.obj +fpbase.obj +fpstream.obj +help.obj +helpbase.obj +histlist.obj\
+ifpstrea.obj +iopstrea.obj +ipstream.obj +misc.obj +msgbox.obj +nbackgro.obj\
+nbutton.obj +ncalcdis.obj +ncalcula.obj +nchdirdi.obj +ncheckbo.obj\
+nclrdisp.obj +ncluster.obj +ncollect.obj +ncolordi.obj +ncolorgr.obj\
+ncolorit.obj +ncolorse.obj +ndesktop.obj +ndialog.obj +ndircoll.obj\
+ndirlist.obj +neditor.obj +neditwi.obj +newstr.obj +nfilecol.obj +nfiledia.obj\
+nfiledtr.obj +nfileinf.obj +nfileinp.obj +nfilelis.obj +nfilterv.obj +nframe.obj\
+ngroup.obj +nhistory.obj +nindicat.obj +ninputli.obj +nlabel.obj +nlistbox.obj\
+nlistvie.obj +nmemo.obj +nmenubar.obj +nmenubox.obj +nmenuvie.obj +nmonosel.obj\
+nparamte.obj +nradiobu.obj +nrangeva.obj +nresourc.obj +nscrollb.obj\
+nscrolle.obj +nsortedc.obj +nsortedl.obj +nstatict.obj +nstatusl.obj\
+nstringc.obj +nstringl.obj +nvalidat.obj +nview.obj +nwindow.obj +ofpstrea.obj\
+opstream.obj +pstream.obj +rhscreen.obj +sbackgro.obj +sbutton.obj +scalcdis.obj\
+scalcula.obj +schdirdi.obj +scheckbo.obj +sclrdisp.obj +scluster.obj\
+scolordi.obj +scolorgr.obj +scolorit.obj +scolorse.obj +screen.obj +sdesktop.obj\
+sdialog.obj +sdircoll.obj +sdirlist.obj +seditor.obj +seditwin.obj +sescreen.obj\
+sfilecol.obj +sfiledia.obj +sfileedi.obj +sfileinf.obj +sfileinp.obj\
+sfilelis.obj +sfilterv.obj +sframe.obj +sgroup.obj +shistory.obj +sindicat.obj\
+sinputli.obj +slabel.obj +slistbox.obj +slistvie.obj +smemo.obj +smenubar.obj\
+smenubox.obj +smenuvie.obj +smonosel.obj +sparamte.obj +sradiobu.obj\
+srangeva.obj +sresourc.obj +sscrollb.obj +sscrolle.obj +sstatict.obj\
+sstatusl.obj +sstringc.obj +sstringl.obj +sview.obj +swindow.obj +tapplica.obj\
+tbackgro.obj +tbutton.obj +tchdirdi.obj +tcheckbo.obj +tclrdisp.obj\
+tcluster.obj +tcollect.obj +tcolordi.obj +tcolorgr.obj +tcolorit.obj\
+tcolorse.obj +tcommand.obj +tdesktop.obj +tdialog.obj +tdircoll.obj\
+tdirlist.obj +tdisplay.obj +tdrawbuf.obj +teditor.obj +teditorf.obj\
+teditwin.obj +tevent.obj +tfilecol.obj +tfiledia.obj +tfileedi.obj +tfileinf.obj\
+tfileinp.obj +tfilelis.obj +tfilterv.obj +tfont.obj +tframe.obj +tgkey.obj\
+tgroup.obj +thistory.obj +thistvie.obj +thistwin.obj +thwmouse.obj +tindicat.obj\
+tinputli.obj +tlabel.obj +tlistbox.obj +tlistvie.obj +tmemo.obj +tmenubar.obj\
+tmenubox.obj +tmenuvie.obj +tmonosel.obj +tmouse.obj +tnscolle.obj +tnssorte.obj\
+tobject.obj +tpalette.obj +tparamte.obj +tpoint.obj +tprogini.obj +tprogram.obj\
+tpwobj.obj +tpwreado.obj +tpwritte.obj +tradiobu.obj +trangeva.obj +trescoll.obj\
+tresfile.obj +tscreen.obj +tscrollb.obj +tscrolle.obj +tsortedc.obj\
+tsortedl.obj +tstatict.obj +tstatusd.obj +tstatusl.obj +tstrinde.obj\
+tstringc.obj +tstringl.obj +tstrlist.obj +tstrmcla.obj +tstrmtyp.obj\
+tsubmenu.obj +ttermina.obj +ttextdev.obj +tvalidat.obj +tvedit1.obj +tvedit2.obj\
+tvedit3.obj +tview.obj +tvtext1.obj +tvtext2.obj +twindow.obj  \
  +win32dis.obj +win32key.obj +win32mouse.obj\
+win32scr.obj +winntdis.obj +winntkey.obj +winntmouse.obj +winntscr.obj
OBJS = $(TV_OBJS_:+=objs\)

all: rhtv.lib tvex.exe

rhtv.lib: $(OBJS)
	del $@
	tlib /E /P256 $@ @&&!
+$(**: = &^
+)
!

tvex.exe: objs\tvex.obj rhtv.lib
	$(CC) $(LDFLAGS) -etvex.exe objs\tvex.obj rhtv.lib

{.}.cc{objs}.obj:
	$(CXX) -c $(CXXFLAGS) $(INC) -nobjs $<

{..\classes}.cc{objs}.obj:
	$(CXX) -c $(CXXFLAGS) $(INC) -nobjs $<

{..\classes\dos}.cc{objs}.obj:
	$(CXX) -c $(CXXFLAGS) $(INC) -nobjs $<

{..\classes\win32}.cc{objs}.obj:
	$(CXX) -c $(CXXFLAGS) $(INC) -nobjs $<

{..\classes\winnt}.cc{objs}.obj:
	$(CXX) -c $(CXXFLAGS) $(INC) -nobjs $<

{..\names}.cc{objs}.obj:
	$(CXX) -c $(CXXFLAGS) $(INC) -nobjs $<

{..\stream}.cc{objs}.obj:
	$(CXX) -c $(CXXFLAGS) $(INC) -nobjs $<

{..}.cc{objs}.obj:
	$(CXX) -c $(CXXFLAGS) $(INC) -nobjs $<

{..}.c{objs}.obj:
	$(CC) -c $(CFLAGS) $(INC) -nobjs $<

{..\compat}.c{objs}.obj:
	$(CC) -c $(CFLAGS) $(INC) -nobjs $<

{..\compat}.cc{objs}.obj:
	$(CXX) -c $(CXXFLAGS) $(INC) -nobjs $<

# objs\tscreen.obj: ..\classes\WinNT\tscreen.cc
# objs\tdsiplay.obj: ..\classes\WinNT\tdsiplay.cc
# objs\tfont.obj: ..\classes\WinNT\tfont.cc
# objs\thwmouse.obj: ..\classes\WinNT\thwmouse.cc

clean:
	-del objs\*.obj
	-del *.obj
	-del *.exe
	-del *.tds
	-del *.lib
