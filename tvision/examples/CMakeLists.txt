cmake_minimum_required(VERSION 2.8)

add_executable(bhelp
	bhelp/bhelp.h
	bhelp/bhelp.cc
	bhelp/thelp.cc
)
target_link_libraries(bhelp rhtv ${TV_DEP_LIBS})

add_executable(clipboard
	clipboard/test.cc
)
target_link_libraries(clipboard rhtv ${TV_DEP_LIBS})

add_executable(cyrillic_x11
	cyrillic/x11koi8/test.cc
)
target_link_libraries(cyrillic_x11 rhtv ${TV_DEP_LIBS})

add_executable(demo
	demo/ascii.h
	demo/calendar.h
	demo/demohelp.h
	demo/fileview.h
	demo/gadgets.h
	demo/mousedlg.h
	demo/puzzle.h
	demo/tvcmds.h
	demo/tvdemo.h
	demo/ascii.cc
	demo/calendar.cc
	demo/fileview.cc
	demo/gadgets.cc
	demo/mousedlg.cc
	demo/puzzle.cc
	demo/tvdemo1.cc
	demo/tvdemo2.cc
	demo/tvdemo3.cc
)
target_link_libraries(demo rhtv ${TV_DEP_LIBS})

add_executable(desklogo
	desklogo/desklogo.cc
)
target_link_libraries(desklogo rhtv ${TV_DEP_LIBS})

add_executable(set-logo
	desklogo/set-logo.cc
)
target_link_libraries(set-logo rhtv ${TV_DEP_LIBS})

add_executable(tv_logo
	desklogo/tv_logo.cc
)
target_link_libraries(tv_logo rhtv ${TV_DEP_LIBS})

add_executable(dlgdsn
	dlgdsn/consted.h
	dlgdsn/dsgdata.h
	dlgdsn/dsgobjs.h
	dlgdsn/fdconsts.h
	dlgdsn/propdlgs.h
	dlgdsn/propedit.h
	dlgdsn/strmoper.h
	dlgdsn/codebldr.cc
	dlgdsn/consted.cc
	dlgdsn/dsgobjs.cc
	dlgdsn/freedsgn.cc
	dlgdsn/propdlgs.cc
	dlgdsn/propedit.cc
	dlgdsn/strmoper.cc
	dlgdsn/tappwnd.cc
)
target_link_libraries(dlgdsn rhtv ${TV_DEP_LIBS})

add_executable(dyntxt
	dyntxt/dyntext.h
	dyntxt/dyntext.cpp
	dyntxt/testdyn.cpp
)
target_link_libraries(dyntxt rhtv ${TV_DEP_LIBS})

add_executable(genraw
	fonts/genraw.cc
)
target_link_libraries(genraw rhtv ${TV_DEP_LIBS})

add_executable(fonts
	fonts/test.cc
)
target_link_libraries(fonts rhtv ${TV_DEP_LIBS})

add_executable(helpdemo
	helpdemo/helpdemo.cc
)
target_link_libraries(helpdemo rhtv ${TV_DEP_LIBS})

add_executable(inplis
	inplis/inplist.h
	inplis/inplist.cpp
	inplis/test.cpp
)
target_link_libraries(inplis rhtv ${TV_DEP_LIBS})

add_executable(listvi
	listvi/lst_view.h
	listvi/listbox2.cpp
	listvi/lst_view.cpp
)
target_link_libraries(listvi rhtv ${TV_DEP_LIBS})

add_executable(msgcls
	msgcls/tlnmsg.h
	msgcls/testdyn.cpp
	msgcls/tlnmsg.cpp
)
target_link_libraries(msgcls rhtv ${TV_DEP_LIBS})

add_executable(progba
	progba/tprogbar.h
	progba/example.cpp
	progba/tprogbar.cpp
)
target_link_libraries(progba rhtv ${TV_DEP_LIBS})

add_executable(sdlg
	sdlg/main.cpp
	sdlg/scrldlg.cpp
	sdlg/scrlgrp.cpp
)
target_link_libraries(sdlg rhtv ${TV_DEP_LIBS})

add_executable(sdlg2
	sdlg2/dlg.h
	sdlg2/main.cpp
	sdlg2/scrldlg.cpp
	sdlg2/scrlgrp.cpp
)
target_link_libraries(sdlg2 rhtv ${TV_DEP_LIBS})

add_executable(tcombo
	tcombo/tcombobx.h
	tcombo/tsinputl.h
	tcombo/nmcmbobx.cpp
	tcombo/nmcmbovw.cpp
	tcombo/nmcmbown.cpp
	tcombo/nmsinput.cpp
	tcombo/scombobx.cpp
	tcombo/scombovw.cpp
	tcombo/scombown.cpp
	tcombo/ssinputl.cpp
	tcombo/tcmbovwr.cpp
	tcombo/tcmbowin.cpp
	tcombo/tcombobx.cpp
	tcombo/test.cpp
	tcombo/tsinputl.cpp
)
target_link_libraries(tcombo rhtv ${TV_DEP_LIBS})

add_executable(terminal
	terminal/terminal.cc
)
target_link_libraries(terminal rhtv ${TV_DEP_LIBS})

add_executable(tprogb
	tprogb/tprogbar.h
	tprogb/calc.cpp
	tprogb/tprogbar.cpp
)
target_link_libraries(tprogb rhtv ${TV_DEP_LIBS})

add_executable(tvedit
	tvedit/tvedit.cc
)
target_link_libraries(tvedit rhtv ${TV_DEP_LIBS})

add_executable(tvhc
	tvhc/tvhc.h
	tvhc/tvhc.cc
)
target_link_libraries(tvhc rhtv ${TV_DEP_LIBS})

add_executable(videomode
	videomode/test.cc
)
target_link_libraries(videomode rhtv ${TV_DEP_LIBS})

add_executable(viewers
	viewers/test.cc
)
target_link_libraries(viewers rhtv ${TV_DEP_LIBS})

ADD_SUBDIRECTORY(tutorial)
