  This file contains information about how to build the Win32 version of the
library. I tested it under Windows 95 and NT 4 so I think it works for any of
the Win32 family members (98, 2000, etc.).

  The Win32 version port was contributed by Anatoli Soltan.

* What compilers are supported?

  Borland C++ Builder 5 or the free command line tools available in the web
(BC++ 5.5 compiler).
  If you are looking for a Mingw32 or MSVC (5.0/6.0) port take a look to the
../win32/readme.txt file.

* Why this target have special instructions and isn't explained in the main
  readme file?

  Because the configuration mechanism to compile the library uses tools that
aren't common for BC++ users.
  
* How do I compile the library?

  Run make from this directory.

* How do I compile the demo?

  Is compiled by the makefile, the name is tvex.exe.

* Some additional comments:

  The library is slower than the DOS version or the Linux/VCSA version.
That's because the Win32 console API isn't fast. It seems to run better under
NT.

* How can I reconfigure the makefile?

  You need a Perl interpreter for it. The script confignt.pl located in the
root directory of the distribution is used for it. Run "perl confignt.pl
--help" to get help.
