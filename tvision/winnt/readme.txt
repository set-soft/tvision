  This file contains information about how to build the Win32 version of the
library. I tested it under Windows 95, 98 and NT 4 so I think it works for
any of the Win32 family members (2000, XP, etc.).
  I recently got positive reports using Windows 2000.

  The Win32 version port was originally contributed by Anatoli Soltan, but
currently three drivers are available. Please read more about it in the doc
directory of the distribution.

* What compilers are supported?

  Borland C++ Builder 5 or the free command line tools available in the web
(BC++ 5.5 compiler).
  Cygwin, if you have it just follow the Linux/UNIX instructions, you just
need to run the configure script and then run make.
  MinGW, currently you can install the MSys package and some Perl interpreter
(like SiePerl) and then follow the Linux/UNIX instruction. More about is can
be found in the ../win32/readme.txt file.
  MSVC (5.0/6.0/7.0/7.1) is also supported.

* Why this target have special instructions and isn't explained in the main
  readme file?

  Because the configuration mechanism to compile the library uses tools that
aren't common for BC++ and MSVC users.
  
* How do I compile the library?

  Borland: Run make from this directory. It uses the Makefile archive.
  MSVC: Run nmake from this directory. It uses the Makefile.nmk archive. Note
        that usually you must edit this file to indicate where are installed
        the MSVC header files.

* How do I compile the demo?

  Is compiled by the makefile, the name is tvex.exe.

* Some additional comments:

  The library is slower than the DOS version or the Linux/VCSA version.
That's because the Win32 console API isn't fast. It seems to run better under
NT.

* How can I reconfigure the makefile?

  You need a Perl interpreter for it. The script confignt.pl located in the
root directory of the distribution is used for it. Run "perl confignt.pl
--help" to get help. Note that this is only available for the Boland
makefile.

