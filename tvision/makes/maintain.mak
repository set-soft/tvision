#!/usr/bin/make
#
# Copyright (c) 2003 by Salvador E. Tropea.
# Covered by the GPL license.
#
# This makefile maintains the .imk and .umk files synchronized with the RHIDE
# project. Is used only when the package is configured with
# --enable-maintainer-mode.
#
# TODO: include utf8 example in Unicode branch
#

include rhide.env

EXDIR=../examples
EXDEP=genimk.cc extrimk.cc

all: makes

# The list of "targets"
mkfs_list=librhtv $(EXDIR)/bhelp/thelp $(EXDIR)/clipboard/test \
	$(EXDIR)/cyrillic/linuxkoi8/test $(EXDIR)/cyrillic/x11koi8/test \
	$(EXDIR)/demo/demo $(EXDIR)/desklogo/desklogo $(EXDIR)/desklogo/set-logo \
	$(EXDIR)/desklogo/tv_logo $(EXDIR)/dlgdsn/libtest $(EXDIR)/dyntxt/dyntext \
	$(EXDIR)/fonts/genraw $(EXDIR)/fonts/test $(EXDIR)/helpdemo/helpdemo \
	$(EXDIR)/i18n/test $(EXDIR)/inplis/implis $(EXDIR)/listvi/lst_view \
	$(EXDIR)/msgcls/msgcls $(EXDIR)/progba/test $(EXDIR)/sdlg/test \
	$(EXDIR)/sdlg2/test $(EXDIR)/tcombo/test $(EXDIR)/terminal/test \
	$(EXDIR)/tprogb/test $(EXDIR)/tutorial/tvguid01 $(EXDIR)/tutorial/tvguid02 \
	$(EXDIR)/tutorial/tvguid03 $(EXDIR)/tutorial/tvguid04 \
	$(EXDIR)/tutorial/tvguid05 $(EXDIR)/tutorial/tvguid06 \
	$(EXDIR)/tutorial/tvguid07 $(EXDIR)/tutorial/tvguid08 \
	$(EXDIR)/tutorial/tvguid09 $(EXDIR)/tutorial/tvguid10 \
	$(EXDIR)/tutorial/tvguid11 $(EXDIR)/tutorial/tvguid12 \
	$(EXDIR)/tutorial/tvguid13 $(EXDIR)/tutorial/tvguid14 \
	$(EXDIR)/tutorial/tvguid15 $(EXDIR)/tutorial/tvguid16 \
	$(EXDIR)/tvedit/tvedit $(EXDIR)/tvhc/tvhc $(EXDIR)/videomode/test
allprjs=$(mkfs_list) ../compat/compat
# Makefiles for them
mkfs_files=$(addsuffix .mkf,$(mkfs_list))
# Dependencies for them
imks_files=$(addsuffix .imk,$(allprjs))

# Tool to extract dependencies
extrimk.exe: extrimk.cc
	$(RHIDE_GCC) -o $@ $< $(STDCPP_LIB)

# Tool to create temporal .mak files and call  extrimk
genimk.exe: genimk.cc extrimk.exe
	$(RHIDE_GCC) -o $@ $< $(STDCPP_LIB)

librhtv.imk: librhtv.gpr ../compat/compat.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/bhelp/thelp.imk: $(EXDIR)/bhelp/thelp.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/clipboard/test.imk: $(EXDIR)/clipboard/test.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/cyrillic/linuxkoi8/test.imk: $(EXDIR)/cyrillic/linuxkoi8/test.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/cyrillic/x11koi8/test.imk: $(EXDIR)/cyrillic/x11koi8/test.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/demo/demo.imk: $(EXDIR)/demo/demo.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/desklogo/desklogo.imk: $(EXDIR)/desklogo/desklogo.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/desklogo/set-logo.imk: $(EXDIR)/desklogo/set-logo.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/desklogo/tv_logo.imk: $(EXDIR)/desklogo/tv_logo.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/dlgdsn/libtest.imk: $(EXDIR)/dlgdsn/libtest.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/dyntxt/dyntext.imk: $(EXDIR)/dyntxt/dyntext.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/fonts/genraw.imk: $(EXDIR)/fonts/genraw.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/fonts/test.imk: $(EXDIR)/fonts/test.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/helpdemo/helpdemo.imk: $(EXDIR)/helpdemo/helpdemo.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/i18n/test.imk: $(EXDIR)/i18n/test.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/inplis/implis.imk: $(EXDIR)/inplis/implis.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/listvi/lst_view.imk: $(EXDIR)/listvi/lst_view.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/msgcls/msgcls.imk: $(EXDIR)/msgcls/msgcls.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/progba/test.imk: $(EXDIR)/progba/test.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/sdlg/test.imk: $(EXDIR)/sdlg/test.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/sdlg2/test.imk: $(EXDIR)/sdlg2/test.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tcombo/test.imk: $(EXDIR)/tcombo/test.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/terminal/test.imk: $(EXDIR)/terminal/test.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tprogb/test.imk: $(EXDIR)/tprogb/test.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tutorial/tvguid01.imk: $(EXDIR)/tutorial/tvguid01.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tutorial/tvguid02.imk: $(EXDIR)/tutorial/tvguid02.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tutorial/tvguid03.imk: $(EXDIR)/tutorial/tvguid03.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tutorial/tvguid04.imk: $(EXDIR)/tutorial/tvguid04.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tutorial/tvguid05.imk: $(EXDIR)/tutorial/tvguid05.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tutorial/tvguid06.imk: $(EXDIR)/tutorial/tvguid06.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tutorial/tvguid07.imk: $(EXDIR)/tutorial/tvguid07.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tutorial/tvguid08.imk: $(EXDIR)/tutorial/tvguid08.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tutorial/tvguid09.imk: $(EXDIR)/tutorial/tvguid09.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tutorial/tvguid10.imk: $(EXDIR)/tutorial/tvguid10.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tutorial/tvguid11.imk: $(EXDIR)/tutorial/tvguid11.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tutorial/tvguid12.imk: $(EXDIR)/tutorial/tvguid12.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tutorial/tvguid13.imk: $(EXDIR)/tutorial/tvguid13.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tutorial/tvguid14.imk: $(EXDIR)/tutorial/tvguid14.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tutorial/tvguid15.imk: $(EXDIR)/tutorial/tvguid15.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tutorial/tvguid16.imk: $(EXDIR)/tutorial/tvguid16.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tvedit/tvedit.imk: $(EXDIR)/tvedit/tvedit.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tvhc/tvhc.imk: $(EXDIR)/tvhc/tvhc.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/videomode/test.imk: $(EXDIR)/videomode/test.gpr $(EXDEP)
	@./genimk.exe $@ $^

makes: extrimk.exe genimk.exe $(mkfs_files) $(imks_files)

