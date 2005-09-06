#!/usr/bin/make
#
# Copyright (c) 2003-2005 by Salvador E. Tropea
# Covered by the GPL license.
#
# This file contains the definitions that configures the compilation.
# They include the C and C++ options and linker options.
#

WATCOM_WARN=-w3
WATCOM_OPTIMIZE=-otexan
WATCOM_RELAX_ALIAS=#-oa
WATCOM_ERRCOUNT=-e25
WATCOM_QUIET=-zq
WATCOM_CALLING_CONV=-5r #-zz
WATCOM_DLLTARGET=-bd
WATCOM_LIBTARGET=
WATCOM_THREAD_MODEL=#-bm
WATCOM_STDDEBUG=-d2 -hd #-DDEBUG_MALLOC
WATCOM_STACK=-of+

!ifdef DEBUG
# Options to debug
CFLAGS   = $(WATCOM_WARN) $(WATCOM_ERRCOUNT) $(WATCOM_QUIET) $(WATCOM_OPTIMIZE) \
	$(WATCOM_RELAX_ALIAS) $(WATCOM_THREAD_MODEL) $(WATCOM_CALLING_CONV) \
	$(WATCOM_STACK) $(WATCOM_STDDEBUG)
!else
# Options for optimization (production)
CFLAGS   = $(WATCOM_WARN) $(WATCOM_ERRCOUNT) $(WATCOM_QUIET) $(WATCOM_OPTIMIZE) \
	$(WATCOM_RELAX_ALIAS) $(WATCOM_THREAD_MODEL) $(WATCOM_CALLING_CONV) \
	$(WATCOM_STACK)
LDFLAGS  =
!endif
LDFLAGS  = $(CFLAGS) -bt=nt -mf
CXXFLAGS = $(CFLAGS)

RHIDE_GCC=wcc386
RHIDE_GXX=wpp386
RHIDE_LD=wpp386
RHIDE_AR=wlink
RHIDE_ARFLAGS=SYS nt_dll op m op maxe=25 op q op symf d dwarf

# Sets the variables for the sub-makes
EXTRALIB_MAKE_ARGS=_SUBMAKE=1 CC="$(RHIDE_GCC)" LD="$(RHIDE_LD)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" -f Makefile.wat

MK_EXT=.wat
MKF_EXT=.wat

