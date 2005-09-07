#!/usr/bin/make
#
# Copyright (c) 2003-2005 by Salvador E. Tropea.
# Covered by the GPL license.
#
# For Open Watcom. NOT TESTED PLEASE HELP ME TO FIX IT.
# This file contains the definitions needed to compile and link the rules
# found in .umk files.
# The definitions are used by all the makefiles.
#

#
# That configure the compilation and linking options.
# Is equivalent to rhide.env.
#
!ifdef CONFIG_PATH
# To avoid usig -I....
!include $(CONFIG_PATH)/config.wat
!else
!include config.wat
!endif

# Turn off the implicit rule to generate .obj files from .c files.
# If we don't do it make will use both rules.
.c.obj:

HELPER_TARGET=
# These files changes the compilation process so they are important
# dependencies.
MAKEFILE_DEPS=
# Extensions used by Borland tools
ExOBJ=.obj
ExEXE=.exe
ExLIB=.dll

# TODO: Write the following rules, currently they are the MSVC ...
RHIDE_COMPILE_LINK=$(RHIDE_LD) $(LDFLAGS) /OUT:$(OUTFILE) $(OBJFILES) \
	$(LIBRARIES) $(RHIDE_LIBDIRS) user32.lib gdi32.lib $(RHIDE_LIBS)
RHIDE_COMPILE_C=$(RHIDE_GCC) -c $(CFLAGS) $(INCLUDE_DIRS) -Fo$(OUTFILE) \
	$(SOURCE_NAME)
RHIDE_COMPILE_CC=$(RHIDE_GXX) -c $(CXXFLAGS) $(INCLUDE_DIRS) -Fo$(OUTFILE) \
	$(SOURCE_NAME)
RHIDE_COMPILE_ARCHIVE=$(RHIDE_AR) $(RHIDE_ARFLAGS) $(OUTFILE) obj
# The command line capacity isn't enough for this:
#RHIDE_COMPILE_ARCHIVE=$(RHIDE_AR) $(RHIDE_ARFLAGS) /OUT:$(OUTFILE) \
#	$(ALL_PREREQ)
# This is fake, the .S files are used only for DJGPP and they are protected
# by preprocessor constructions. For this reason they can be used as C sources
# to generate dummy .obj files.
RHIDE_COMPILE_ASM_FORCE=$(RHIDE_GCC) -c $(CFLAGS) /TC $(INCLUDE_DIRS) -Fo$(OUTFILE) \
	$(SOURCE_NAME)

SOURCE_NAME=$**
OUTFILE=$@
ALL_PREREQ=$**


