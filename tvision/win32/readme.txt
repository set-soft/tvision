  This file contains information about how to build the Win32 version of the
library. I tested it under Windows 95 and NT 4 so I think it works for any of
the Win32 family members (98, 2000, etc.).

  The Win32 version port was contributed by Vadim Beloborodov.
  After some adjustments (with the help of Ronald Landheer) we are getting it
working for Cygwin also.

* What compilers are supported?

  Cygwin, I used 1.3.3-2 for testing.
  MinGW (use the gcc 2.95.3 port with 20010726 runtime or newer, you'll
face some problems if you use something like 19980701 release). I tested it
and works very well.
  MSVC (5.0/6.0) is also supported, but I don't have access to it so I can't
be sure if it works ok. You should have no problems or at most very simple
ones. Also note that Vadim wrote the MSVC make file with GNU tools in mind
(he used rm instead of just del ;-). I'll sugest you to first try compiling
with MinGW and after it (you'll get a working makefile and GNU tools) try
with MSVC.
  If you are looking for a Borland C++ port take a look to the
../winnt/readme.txt file.

* Why this target have special instructions and isn't explained in the main
  readme file?

  Currently the Win32 target is under test, Vadim did a very good work and I
think the port is very stable and functional, but some details remains to be
solved.
  If you will use Cygwin you can just follow the regular compilation
instructions. Basically go to ../ and run ./configure then just run make.
Note you need perl, fileutils, a generic shell in called /bin/sh (could be a
link to bash) and make installed.
  The main problem is with MinGW and MSVC, they lack most of the interesting
GNU tools used for the configuration process. The configuration process uses
uname to find the name of the OS, cp, rm, mv and install to work with files
and perl to run the scripts. Of course MSVC lacks it and lamentably MinGW
does it too.
  To run the configuration process you need:

1) MinGW.
2) From Cygwin: rm.exe, cp.exe, mv.exe, uname.exe, cygwin1.dll, sh.exe (or
bash.exe) and install.exe. All copied to the bin directory of your MinGW
installation. You can also get these tools from the djgpp distribution but I
think is more probable you have the Cygwin version of the files already
installed.
3) From djgpp distribution: perl552b.zip or newer. Uncompress it in the same
directory where you installed MinGW. Or you can just drag the perl.exe file
and put it in the bin directory, that's enough for what we need.
Note that Cygwin's Perl doesn't work because for MinGW the end of line is
\r\n and Cygwin missinterprets it. DJGPP's port works ok.

  If that's too much asking please insist to the MinGW people to include some
of these tools. Porting rm, cp, mv and uname is really easy.
  Then just run the configure.bat file. It will generate a configuration
header and a Makefile. Then run make and if all is successful run "make
install". Send me comments about it.

* How do I compile the library?

  I really encourage to try using the configuration process. But you can try
with the already configured Makefile:

  MinGW: make
  MSVC: Use make to compile, the file makefile is called makefile.ms
        (make -f makefile.ms)
          This makefile is most probably outdated, if anyone is really
        interested on it let me know.

* How do I compile the demo?

  MinGW: make -f demo.mak
  MSVC: The makefile is called demo.ms (make -f demo.ms)
          This makefile is most probably outdated, if anyone is really
        interested on it let me know.

* Some additional comments:

  The library is slower than the DOS version or the Linux/VCSA version.
That's because the Win32 console API isn't fast.


