# Copyright (C) 2001-2002 by Salvador E. Tropea (SET),
# see copyrigh file for details
#
# This file is mostly used to clean the examples directories, but can be
# used to compile any of them or just one from this directory.
#
.PHONY: clean

examples=demo/demo desklogo/desklogo desklogo/set-logo \
	desklogo/tv_logo dlgdsn/libtest dyntxt/dyntext \
	inplis/implis listvi/lst_view msgcls/msgcls \
	progba/test sdlg/test sdlg2/test tcombo/test \
	terminal/test tprogb/test tutorial/tvguid01 \
	tutorial/tvguid02 tutorial/tvguid03 tutorial/tvguid04 \
	tutorial/tvguid05 tutorial/tvguid06 tutorial/tvguid07 \
	tutorial/tvguid08 tutorial/tvguid09 tutorial/tvguid10 \
	tutorial/tvguid11 tutorial/tvguid12 tutorial/tvguid13 \
	tutorial/tvguid14 tutorial/tvguid15 tutorial/tvguid16 \
	tvedit/tvedit tvhc/tvhc clipboard/test fonts/test \
	helpdemo/test fonts/genraw

all: $(addsuffix .exe,$(examples))

clipboard/test.exe: clipboard/test.gpr

demo/demo.exe: demo/demo.gpr

desklogo/desklogo.exe: desklogo/desklogo.gpr

desklogo/set-logo.exe: desklogo/set-logo.gpr

desklogo/tv_logo.exe: desklogo/tv_logo.gpr

dlgdsn/libtest.exe: dlgdsn/libtest.gpr

dyntxt/dyntext.exe: dyntxt/dyntext.gpr

fonts/test.exe: fonts/test.gpr

fonts/genraw.exe: fonts/genraw.gpr

helpdemo/helpdemo.exe: helpdemo/helpdemo.gpr

inplis/implis.exe: inplis/implis.gpr

listvi/lst_view.exe: listvi/lst_view.gpr

msgcls/msgcls.exe: msgcls/msgcls.gpr

progba/test.exe: progba/test.gpr

sdlg/test.exe: sdlg/test.gpr

sdlg2/test.exe: sdlg2/test.gpr

tcombo/test.exe: tcombo/test.gpr

terminal/test.exe: terminal/test.gpr

tprogb/test.exe: tprogb/test.gpr

tutorial/tvguid01.exe: tutorial/tvguid01.gpr

tutorial/tvguid02.exe: tutorial/tvguid02.gpr

tutorial/tvguid03.exe: tutorial/tvguid03.gpr

tutorial/tvguid04.exe: tutorial/tvguid04.gpr

tutorial/tvguid05.exe: tutorial/tvguid05.gpr

tutorial/tvguid06.exe: tutorial/tvguid06.gpr

tutorial/tvguid07.exe: tutorial/tvguid07.gpr

tutorial/tvguid08.exe: tutorial/tvguid08.gpr

tutorial/tvguid09.exe: tutorial/tvguid09.gpr

tutorial/tvguid10.exe: tutorial/tvguid10.gpr

tutorial/tvguid11.exe: tutorial/tvguid11.gpr

tutorial/tvguid12.exe: tutorial/tvguid12.gpr

tutorial/tvguid13.exe: tutorial/tvguid13.gpr

tutorial/tvguid14.exe: tutorial/tvguid14.gpr

tutorial/tvguid15.exe: tutorial/tvguid15.gpr

tutorial/tvguid16.exe: tutorial/tvguid16.gpr

tvedit/tvedit.exe: tvedit/tvedit.gpr

tvhc/tvhc.exe: tvhc/tvhc.gpr

%.exe: %.gpr
	$(MAKE) -C $(dir $<) -f $(addsuffix .mak,$(basename $(notdir $<)))

clean:
	@echo 'Cleaning examples.'
	-@rm -f */*.o */*.exe */*.gdt */*.bkp *.bkp */*.dst
