  This file contains information about how to build the Win32 version of the
library. I tested it under Windows 95 and NT 4 so I think it works for any of
the Win32 family members (98, 2000, etc.).

  The Win32 version port was contributed by Vadim Beloborodov.
  After some adjustments (with the help of Ronald Landheer) we are getting it
working for Cygwin also. In version 2.0 things changed a lot, internally, and
I had to make a lot of changes in the code. I also added some new members
taking a look at a Win32 API reference. I must admit the API is clean and
simple and the 2.0 code is quite compact and simple.

* What compilers are supported?

  Cygwin, I used 1.3.3-2 for testing.
  
  MinGW, I did the last test and adjustments with MinGW 2.0.0, MSys 1.0.8 and
the Siemens Perl port (SiePerl). This provides a good environment for
compilation. You must ensure that MSys' make is used because MinGW 1.1 ships
a make command that fails. Attention!!! never run TV program from the MSys
prompt, at least in v1.0.7/8 it doesn't support Win32 console applications
(redirects stdin/stdout).
  I also recommend configuring the package to generate files without debug
information and without international support:
$ perl config.pl --no-intl --cflags=-O2 --cxxflags=-O2

  MSVC (5.0/6.0/7.0) is also supported, but I don't have access to it so I can't
be sure if it works ok. You should have no problems or at most very simple
ones. Also note that Vadim wrote the MSVC make file with GNU tools in mind
(he used rm instead of just del ;-). I'll sugest you to first try compiling
with MinGW and after it (you'll get a working makefile and GNU tools) try
with MSVC. Take a look at the ../winnt directory for a possible solution.
  If you are looking for a Borland C++ port take a look at the
../winnt/readme.txt file.

* Why this target have special instructions and isn't explained in the main
  readme file?

  Currently the Win32 target is under test, Vadim did a very good work and I
think the port is very stable and functional, but some details remains to be
solved.
  In the past these targets needed special treatment, currently using Cygwin
or MinGW+MSys the compilation process is the same used for DOS and UNIX.
Basically go to ../ and run ./configure then just run make. Note you need
perl, fileutils, a generic shell called /bin/sh (could be a link to bash) and
make installed.
  If you are using MSVC you can try using the Makefile.ms file, but you'll
need GNU make and some GNU tools. You can also try using nmake and the file
located at ../winnt, but this is under development.
  To run the configuration process you need:

* How do I compile the library without configuring?

  I really encourage to try using the configuration process. But you can try
with the already configured Makefile:

  MinGW: make
  MSVC: Use make to compile, the file makefile is called makefile.ms
        (make -f makefile.ms)
          This makefile is most probably outdated, if anyone is really
        interested on it let me know.

* How do I compile the demo without configuring?

  MinGW: make -f demo.mak
  MSVC: The makefile is called demo.ms (make -f demo.ms)
          This makefile is most probably outdated, if anyone is really
        interested on it let me know.

* Some additional comments:

  The library is slower than the DOS version or the Linux/VCSA version.
That's because the Win32 console API isn't fast.
  You can obtain some GNU utilities compiled by MSVC for Win32 from 'GNU
utilities for Win32` project located at http://unxutils.sourceforge.net/
