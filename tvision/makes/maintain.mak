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
mkfs_list=$(EXDIR)/bhelp/thelp $(EXDIR)/clipboard/test \
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
imks_list=librhtv ../compat/compat
allprjs=$(mkfs_list) $(imks_list)
# Makefiles for them
mkfs_files=$(addsuffix .mkf,$(mkfs_list))
# Dependencies for them
imks_files=$(addsuffix .imk,$(imks_list)) $(addsuffix .umk,$(mkfs_list))

# Tool to extract dependencies
extrimk.exe: extrimk.cc
	$(RHIDE_GXX) -o $@ $< $(STDCPP_LIB)

# Tool to create temporal .mak files and call  extrimk
genimk.exe: genimk.cc extrimk.exe
	$(RHIDE_GXX) -o $@ $< $(STDCPP_LIB)

librhtv.imk: librhtv.gpr ../compat/compat.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/bhelp/thelp.umk: $(EXDIR)/bhelp/thelp.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/clipboard/test.umk: $(EXDIR)/clipboard/test.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/cyrillic/linuxkoi8/test.umk: $(EXDIR)/cyrillic/linuxkoi8/test.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/cyrillic/x11koi8/test.umk: $(EXDIR)/cyrillic/x11koi8/test.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/demo/demo.umk: $(EXDIR)/demo/demo.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/desklogo/desklogo.umk: $(EXDIR)/desklogo/desklogo.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/desklogo/set-logo.umk: $(EXDIR)/desklogo/set-logo.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/desklogo/tv_logo.umk: $(EXDIR)/desklogo/tv_logo.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/dlgdsn/libtest.umk: $(EXDIR)/dlgdsn/libtest.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/dyntxt/dyntext.umk: $(EXDIR)/dyntxt/dyntext.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/fonts/genraw.umk: $(EXDIR)/fonts/genraw.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/fonts/test.umk: $(EXDIR)/fonts/test.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/helpdemo/helpdemo.umk: $(EXDIR)/helpdemo/helpdemo.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/i18n/test.umk: $(EXDIR)/i18n/test.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/inplis/implis.umk: $(EXDIR)/inplis/implis.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/listvi/lst_view.umk: $(EXDIR)/listvi/lst_view.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/msgcls/msgcls.umk: $(EXDIR)/msgcls/msgcls.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/progba/test.umk: $(EXDIR)/progba/test.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/sdlg/test.umk: $(EXDIR)/sdlg/test.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/sdlg2/test.umk: $(EXDIR)/sdlg2/test.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tcombo/test.umk: $(EXDIR)/tcombo/test.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/terminal/test.umk: $(EXDIR)/terminal/test.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tprogb/test.umk: $(EXDIR)/tprogb/test.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tutorial/tvguid01.umk: $(EXDIR)/tutorial/tvguid01.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tutorial/tvguid02.umk: $(EXDIR)/tutorial/tvguid02.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tutorial/tvguid03.umk: $(EXDIR)/tutorial/tvguid03.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tutorial/tvguid04.umk: $(EXDIR)/tutorial/tvguid04.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tutorial/tvguid05.umk: $(EXDIR)/tutorial/tvguid05.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tutorial/tvguid06.umk: $(EXDIR)/tutorial/tvguid06.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tutorial/tvguid07.umk: $(EXDIR)/tutorial/tvguid07.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tutorial/tvguid08.umk: $(EXDIR)/tutorial/tvguid08.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tutorial/tvguid09.umk: $(EXDIR)/tutorial/tvguid09.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tutorial/tvguid10.umk: $(EXDIR)/tutorial/tvguid10.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tutorial/tvguid11.umk: $(EXDIR)/tutorial/tvguid11.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tutorial/tvguid12.umk: $(EXDIR)/tutorial/tvguid12.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tutorial/tvguid13.umk: $(EXDIR)/tutorial/tvguid13.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tutorial/tvguid14.umk: $(EXDIR)/tutorial/tvguid14.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tutorial/tvguid15.umk: $(EXDIR)/tutorial/tvguid15.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tutorial/tvguid16.umk: $(EXDIR)/tutorial/tvguid16.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tvedit/tvedit.umk: $(EXDIR)/tvedit/tvedit.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/tvhc/tvhc.umk: $(EXDIR)/tvhc/tvhc.gpr $(EXDEP)
	@./genimk.exe $@ $^

$(EXDIR)/videomode/test.umk: $(EXDIR)/videomode/test.gpr $(EXDEP)
	@./genimk.exe $@ $^

makes: extrimk.exe genimk.exe $(mkfs_files) $(imks_files)

