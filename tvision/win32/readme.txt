  This file contains information about how to build the Win32 version of the
library. I tested it under Windows 95 and NT 4 so I think it works for any of
the Win32 family members (98, 2000, etc.).

  The Win32 version port was contributed by Vadim Beloborodov.

* What compilers are supported?

  Mingw32 (use the gcc 2.95.2 port with 19991107 runtime or newer, you'll
face some problems if you use something like 19980701 release). I tested it
and works very well.
  MSVC (5.0/6.0) is also supported, but I don't have access to it so I can't
be sure if it works ok. You should have no problems or at most very simple
ones. Also note that Vadim wrote the MSVC make file with GNU tools in mind
(he used rm instead of just del ;-).
  If you are looking for a Borland C++ port take a look to the
../winnt/readme.txt file.

* Why this target have special instructions and isn't explained in the main
  readme file?

  Currently the win32 target is under test, Vadim did a very good work and I
think the port is very stable and functional, but some details remains to be
solved.
  The main problem is that Mingw32 and MSVC lacks most of the interesting GNU
tools used for the configuration process. The configuration process uses
uname to find the name of the OS, cp, rm, mv and install to work with files
and perl to run the scripts. Of course MSVC lacks it and lamentably Mingw32
does it too. I didn't have too much success using Cygwin and it also doesn't
have Perl (you must get it from a separated site).
  All of this makes very hard to integrate the Win32 port to the
configuration process. But I'll do it in some way, even as a very optional
one.
  If you want to try the configure script just as beta tester install the
following tools:
1) Mingw32.
2) From Cygwin: rm.exe, cp.exe, mv.exe, uname.exe, cygwin1.dll, sh.exe and
install.exe. All copied to the bin directory of your Mingw installation.
3) From djgpp distribution: perl552b.zip. Uncompress it in the same directory
where you installed Mingw.
  Then just run the configure.bat file. It will generate a configuration
header and a Makefile. Then run make and if all is successful run "make
install". Send me comments about it.

* How do I compile the library?

  Mingw32: make
  MSVC: Use make to compile, the file makefile is called makefile.ms
        (make -f makefile.ms)

* How do I compile the demo?

  Mingw32: make -f demo.mak
  MSVC: The makefile is called demo.ms (make -f demo.ms)

* Some additional comments:

  The library is slower than the DOS version or the Linux/VCSA version.
That's because the Win32 console API isn't fast.


