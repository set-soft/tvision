del configure.cache
set CFLAGS=-O2 -Wall -Werror -gstabs+3
set CXXFLAGS=-O2 -Wall -Werror -gstabs+3 -fno-exceptions -fno-rtti
configure.bat --force-dummy %1 %2 %3 %4 %5 %6 %7 %8 %9
