Hi!

The small sections are:

1. Introduction
2. Notes about the package
3. Compilation and use
4. Linux dynamic libraries
5. Libraries needed
6. Examples
7. How to submit a patch
8. Contact information


1. Introduction:
---------------

  That's the Turbo Vision distribution (TVision for short) v1.0.3.
  The sources are copyrighted by Borland (currently Inprise) and are freely
available in internet from:

ftp://ftp.borland.com/pub/borlandcpp/devsupport/archive/turbovision/tv.zip

  Look in the borland.txt file for more information.
  They were heavily patched to work with gcc. Some patch fixes bugs, others
changes the behavior of things limited to 16 bits values, the resulting
TVision isn't 100% compatible with the original. See the examples/porting.txt
file for more information, I explain which differences I found while porting
the examples there.
  Most of the patches were made by Robert Hoehne. I contributed with various
patches and modules mainly to extend the original functionality.
  The library is distributed under the GPL license even when I seems to be OK
because according to a FAQ in the Inprise web site the base library is Public
Domain. The file copying contains the GPL license and the file copying.rh
contains the RHIDE copyright.

  The purpose of this distribution is avoid downloading the whole RHIDE to
use it. I'm doing the distribution because Robert doesn't have enough time
to do it. Additionally this distribution will make RHIDE's compilation much
more simple for people don't familiarized with it.

  Most of the notes applies only to the source distribution but there are a
lot of important information for the binary too.




2. Notes about the package:
--------------------------

  As you are reading it you already unpacked it but here are some notes:
  The files that must be in DOS format are listed in lista.dos, the rest are
binary or can be UNIX files because djgpp tools handles these UNIX files very
well.
  If you take a look at the tree you'll see I included both, the DOS and
Linux, makefiles and .gpr files. That's because the source distribution is
basically the same for DOS and Linux. If you have the Linux distribution and
you want to use it under DOS just unpack the files and run the fromunix.pl
Perl script. The reverse is valid: the DOS package can be used under Linux
running the fromdos.pl Perl script.
  The compress and compress.bat files are used to create the distribution
package and are included.




3. Compilation and use:
----------------------

  To compile the library go to the directory for your system (djgpp or linux)
and run rhide or make in this directory.
  All the .o files are stored in the obj directory it helps to delete or
compress these files if you want to save disk space and you won't be
modifying the sources all the time.

  To use the library add the include directory in your library path and the
directory where the library was created to your libraries path (or just move
the library to the lib directory). I recommend using an environment variable
pointing to the base directory where the files are located and add the rest
to it. For example:
TVISION=e:/dj/contrib/tvision
Then $(TVISION)/include is the include directory and $(TVISION)/linux or
$(TVISION)/djgpp is the library path.

  I use gcc 2.8.0 under DOS so the library is by default compiled with
-fno-exceptions, -fno-rtti and -funsigned-char so you should use these
settings. If you don't like these settings change it before compiling or
rebuild the library with your favorite settings.
  In Linux I use gcc 2.7.2.3 and hence -fno-exceptions isn't needed and is
rejected by gcc too. So if you compile the library for Linux using 2.8.x you
could want to change it.
  The library is compiled with -O2 as optimization, that's good enough for
this kind of code and any processor.
  The library contains debug information, the default settings use -gstabs3,
it works very well and isn't much greater than -g. If you want to delete
the debug information you can do it running the sacadeb batch or script. It
will rename the original library to libtvdb.a and create a new one without
debug information.




4. Linux dynamic libraries:
--------------------------

From sources:
  To create the .so library go to the linuxso directory, run makemak.pl
(yes, you'll need Perl for it, that's standard in Debian) and then edit and
run the instlib installation script.

From binary distribution:
  Edit and run the instlib script.

  That's very experimental so be careful I don't know if the version numbers
are OK and what will happend if you try to execute something created with
the 1.0.3 library using the 1.0.4 library. If you have hints about this
topic please contact me.
  My tests shows that a small TVision example (examples/dyntxt) is only 14Kb
long when linked dynamically and without debug information. But you should
know that dynamically linked executables are slower than static ones,
specially when you call a lot of small and fast library functions, that's
because the address resolution overhead.
  I distribute the .so file with debug information but the installation
script removes it in the installed copy. You can change the script or move
the version with debug information to your libs directory.




5. Libraries needed:
-------------------

Linux:
  In Linux you'll need ncurses (I use 3.4) and gpm. If you are using libc5
you'll need gettext too (libintl) that isn't needed for glibc.
  The binary distribution was created with Debian 2.0 (glibc2, ncurses3, etc).

DOS:
  You must install gettext, you can find it in the v2gnu directory in
Simtelnet. Is called (for example) gtxt032b.zip. There are a bug in the
headers of gtxt032b.zip, in the file libintl.h there are a line that says
@INCLUDE_LOCALE_H@, you can safetly remove it.





6. Examples:
-----------

  I included a collection of nine examples I got from the net and ported to
this TVision. The examples are shipped only with the sources distribution.




7. How to submit a patch:
------------------------

  The simplest way is running a diff between your current directory and a
copy of the library as you downloaded it. Don't forget to specify the exact
version of the distribution and the purpose of the patchs. Use the unified
mode (-u), this mode is the best for humans ;-)




8. Contact information:
----------------------

Salvador E. Tropea (SET)
Curapaligue 2124
3 de Febrero, Caseros
(1678) Bs.As. Argentina

e-mail: salvador@inti.gov.ar, set-soft@usa.net, set@computer.org
http:   setsoft.home.ml.org



Enjoy, SET


