:@Rem Copyright (C) 1999-2003 by Salvador E. Tropea (SET),
:@Rem see copyrigh file for details
@echo .
@echo This batch file configures the packages using the settings that the
@echo *maintainer* uses for your own needs.
@echo These settings doesn't have to be the same you want.
@echo The most common defaults are obtained by just running configure.bat.
@echo If you want to reconfigure the package first delete configure.cache
@echo .
@pause
@del configure.cache
@set CFLAGS=-O2 -Wall -Werror -gstabs+3
@set CXXFLAGS=-O2 -Wall -Werror -gstabs+3 -fno-exceptions -fno-rtti
@configure.bat --enable-maintainer-mode --with-ssc --force-dummy %1 %2 %3 %4 %5 %6 %7 %8 %9
