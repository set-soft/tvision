Hi!

The small sections are:

1. Introduction
2. Notes about the package
3. Compilation and use
4. Linux dynamic libraries
5. Libraries and tools needed
6. Examples
7. Targets supported, limitations
8. How to submit a patch
9. Contact information

****** Important! for old users: the library generated is now called ******
****** librhtv.a this change in the name is to avoid confusion with  ******
****** other ports of the library.                                   ******

1. Introduction:
---------------

  That's the Turbo Vision distribution (TVision for short) v1.0.5.
  The sources are copyrighted by Borland (currently Inprise) and are freely
available in internet from:

ftp://ftp.inprise.com/pub/borlandcpp/devsupport/archive/turbovision/tv.zip

  Look in the borland.txt file for more information. Additionally here is
what Inprise says about this package:

URL: http://www.inprise.com/devsupport/bcppbuilder/faq/QNA906.html
------------------------------------------------------------------------------
Question and Answer Database

QNA        :Turbo Vision
Category   :C/C++ Language Issues
Platform   :Windows95
Product   :C++Builder  ALL

Question:
Where can I find the public domain version of Turbo Vision?

Answer:
It can be found at
ftp.inprise.com/pub/borlandcpp/devsupport/archive/turbovision/

the turbo vision libraries are in the tv.zip archve, the rest of the 
files are examples.

7/2/1998 10:32:32 AM
------------------------------------------------------------------------------
  So according to Inprise web tv.zip is a Public Domain version.
<Clarification>
  I added this small text because some people have doubts about copyrights
issues.
</Clarification>

  They were heavily patched to work with gcc. Some patch fixes bugs, others
changes the behavior of things limited to 16 bits values, the resulting
TVision isn't 100% compatible with the original. See the examples/porting.txt
file for more information, I explain which differences I found while porting
the examples there.
  Most of the patches were made by Robert Hoehne. I contributed with various
patches and modules mainly to extend the original functionality.
  The library is distributed under the GPL license, it seems to be OK because
according to a FAQ in the Inprise web site the base library is Public
Domain. The file copying contains the GPL license and the file copying.rh
contains the RHIDE copyright.

  The purpose of this distribution is avoid downloading the whole RHIDE to
use it. I'm doing the distribution because Robert doesn't have enough time
to do it. Additionally this distribution will make RHIDE's compilation much
more simple for people don't familiarized with it.

  Most of the notes applies only to the source distribution but there are a
lot of important information for the binary too.

  *** IMPORTANT *** Note: This information is about the DOS version but could
be valid for the Linux version too:
  the binary distribution was compiled with gcc 2.8.0 be careful if you have
2.8.1, Artur Zaroda <zaroda@mimuw.edu.pl> reported problems with some of the
examples compiled with 2.8.1 and linked with the binary distribution. All was
solved after recompiling the library with 2.8.1. So don't forget it.




2. Notes about the package:
--------------------------

  As you are reading it you already unpacked it but here are some notes:
(I usually read the readmes without unpacking)
  The files that must be in DOS format are listed in lista.dos, the rest are
binary or can be UNIX files because djgpp tools handles these UNIX files very
well.
  If you take a look at the tree you'll see I included both, the DOS and
Linux, makefiles and .gpr files. That's because the source distribution is
basically the same for DOS and Linux. If you have the Linux distribution and
you want to use it under DOS just unpack the files and run the fromunix.pl
Perl script. The reverse is valid: the DOS package can be used under Linux
running the fromdos.pl Perl script.
  Starting with 1.0.5 I include the Debian package files. As these files
aren't 8+3 clean if you uncompress the files in plain DOS you'll get some
warnings about files with the same name, so I removed these files from the
DOS distribution in 1.0.6, now they are only in the Linux distribution. So
if you want to try generating the .deb packages you *must* have the Linux
distribution of the editor.
  The compress and compress.bat files are used to create the distribution
packages and are included.




3. Compilation and use:
----------------------

  To compile the library go to the directory for your system (djgpp or linux)
and run rhide or make in this directory.
  All the .o files are stored in the obj directory it helps to delete or
compress these files if you want to save disk space and you won't be
modifying the sources all the time.

  To use the library add the include directory in your includes path and the
directory where the library was created to your libraries path (or just move
the library to the lib directory). I recommend using an environment variable
pointing to the base directory where the files are located and add the rest
to it. For example:
TVISION=e:/dj/contrib/tvision
Then $(TVISION)/include is the include directory and $(TVISION)/linux or
$(TVISION)/djgpp is the library path.

  I use gcc 2.8.0 under DOS so the library is by default compiled with
-fno-exceptions and -fno-rtti so you should use these settings. If you
don't like these settings change it before compiling or rebuild the library
with your favorite settings.
  In Linux I use gcc 2.7.2.3 and hence -fno-exceptions isn't needed and is
rejected by gcc too. So if you compile the library for Linux using 2.8.x you
could want to change it.

<Clarification>
  *** Important *** the above sentences seems to be not enough so here I
clarify it: code compiled with -fno-exceptions and -fno-rtti doesn't have the
hooks needed for C++ exceptions in the classes so you can't link this code
with code compiled with gcc 2.8.x if this code wasn't compiled with
-fno-exceptions and -fno-rtti. Why we use it? (1) We don't use C++ exceptions
mechanism (2) It adds unnecesary code and data (3) libraries compiled with
these hooks using 2.8.x can't be linked with objects from 2.7.x.
  So, if you use gcc 2.8.x or newer compiler and want to use the precompiled
library you must compile your code with -fno-exceptions and -fno-rtti.
</Clarification>

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
the 1.0.1 library using the 1.0.2 library. If you have hints about this
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
  In Linux you'll need ncurses (I use 3.4) and gpm (I use 1.3). If you are
using libc5 you'll need gettext too (libintl) that isn't needed for glibc
(I use 2.0.7).
  The binary distribution was created with Debian 2.0 (glibc2, ncurses3, etc).

DOS:
  You must install gettext, you can find it in the v2gnu directory in
Simtelnet. Is called (for example) gtxt032b.zip. There are a bug in the
headers of gtxt032b.zip, in the file libintl.h there are a line that says
@INCLUDE_LOCALE_H@, you can safetly remove it. An alternative is rename
nointl.h to libintl.h, but isn't well tested.

Tools:
Compiler: I tested with gcc 2.7.x and 2.8.x. I know some people uses egcs but
I don't know if you need to change something or if the binary works.
Binutils: I use binutils 2.8.1, newers should work.
RHIDE: if you plan to use the project (.gpr) files you'll need the last
version of RHIDE available. Any older version will crash when loading the
projects.
make: (only if you plan to use the makefiles) I use make 3.77 under DOS.
Perl: (only if you need to run some special scripts) Any perl 5 should work,
I use 5.004 currently.
File utils: (I'm not sure but just in case) you need to have it installed if
you are in DOS, Linux of course have it by default.




6. Examples:
-----------

  I included a collection of 13 examples I got from the net and ported to
this TVision. The examples are shipped only with the sources distribution.




7. Targets supported, limitations:
---------------------------------

  The library compiles and runs in DOS and Linux. Only i386 Linux is fully
supported, we are working to support Alpha too, currently I think you can
get it working if you compile the library without warnings enabled, that's
needed because there are errors in system.h file for Alpha (an invalid
pointer convertion) that are reported only by C++ and not C, looks like
nobody includes these headers in C++ programs. Another problem I know about
Alpha is that egcs 2.91.66 generates some silly warnings, I think they are
due to some bug in egcs, removing the -Werror switch you'll get a working
library. Dim Zegebart <zager@post.comstar.ru> is the one testing the Alpha
stuff.
  There are some important things to note, specially what is supported in
each case:

[Sorted alphabetically]

DOS:
 Keyboard:
   Almost all the keyboard combinations are supported. Some keyboard
  combinations aren't reported by BIOS, they are:
  Control + `1457890=;,./
  Macro key (I have it in one of my keyboards)
   To workaround it I created a small TSR (hacking another TSR and rewriting
  the code inside the ISR ;-). This TSR is included with the sources
  distribution in the extra/doskeys directory, you can distribute it with
  your program if you want, just don't remove my copyright.
 Screen:
   Any text mode fully supported by BIOS and that is compatible with mode 3
  (VGA color) is supported, VESA modes are supported too, if you have
  problems please report it. If you use a program that sets the video mode
  by tweaking the VGA registers it will also set the BIOS data area to
  reflect the new screen size or it won't work.
   Additionally the library supports some build in modes:
  82x25, 80x28, 80x30, 90x30, 94x30, 80x34, 90x34, 94x34, 80x35, 80x40,
  80x43 and 80x50.
   Note: if you wonder why 82x25 that's a mode where the characters have 8
  pixels of width and is very good for fonts with strange layouts like
  ISO-Latin-1.
   The hercules monochrome mode should be supported but I didn't test it for
  a long time so perhaps something is broken, please report your experience
  if you try it.
 Mouse
   Full support for drivers compatible with MSMouse v6.0.

Linux console for users with access to /dev/vcs* devices:
  Keyboard:
    If you are running in the console (ttyN, not ttypN for example) the
   support is excellent. The library uses TIOCLINUX function #6 to get the
   Alt, Ctrl, Shift, etc. and ncurses for the keys. Combining this
   information almost anything is supported. As some keys are used by Linux
   kernel the library patchs the keyboard tables to fool kernel, so things
   like Alt+Fn won't change console, or Shift+PgUp won't scroll up. To
   switch consoles use Ctrl+Alt+Fn wich, by the way, is the combination used
   by X.
    An interesting limitation is that Ctrl+I is Tab and you can't avoid it.
  Screen:
    You need at least write access to the /dev/vcs* devices, if not you'll
   lose performance and some characters. The best is having read and write
   access, if you only have write access TScreen::suspend/resume doesn't
   work properly.
    The current video mode is used, no functions to change the video mode are
   provided. Currently the library is limited to the standard fonts (VGA)
   I'm working to fix it. The library supports 16 foreground and 16
   background colors.
  Mouse:
    Full support for gpm 1.x, pressing Alt+mouse releases the mouse to gpm.
   It alows copying from a Turbo Vision application to other application, but
   not the reverse, why? because gpm will simply type the keys and as you are
   holding Alt all will be received as Alt+key. But there are a solution ;-)
   if you configure the library to use the left alt key for the menues
   (see TGKey::SetAltSettings(value)) you can use the right alt for the
   mouse paste.

Linux using telnet:
  Keyboard:
    It means when you are running in a remote mode (ttypN for example), note
   that midnight commander runs the applications in this way!
    In this case you are at the mercy of ncurses wich uses some arcaic
   standards. You'll lose: Shift+Special keys and Ctrl+Special keys (and
   more). Special keys are arrows, home, end, insert, etc. This limitation is
   in the protocols and standards used and I can't do anything to workaround
   it, the system is broken.
  Screen:
    The support is slow (1/3.5 compared with vcsN) and if you are using a
   network the speed is of course limited by the transmision, wich could
   make the application too slow. As not all the characters are supported
   some things like the arrows of the scroll bars will be replaced by ASCII
   values. 16 foreground and 8 background colors.
  Mouse:
    Not supported, don't know if possible, in fact kmous is not defined in
   the terminfo file for Linux.

Linux using xterm:
  Keyboard:
    The same limitations that "Linux using telnet" because xterms and ncurses
   can't do more.
  Screen:
    Good support, the library supports SIGWINCH so changing the size of the
   window will resize the application. 8 foreground and 8 background colors
   are supported, but using bold/normal the library gets 16 foreground
   colors.
  Mouse:
    Supported, lamentably isn't very interactive because xterm only reports
   when you press or release a button, but not mouse movement. You must get
   acustomed, but works.

Linux using Xterm: That's the way to go if we want very good support for X,
                   currently I don't have time to do it so if you can help
                   please volunteer. I have all the information needed.

Windows 3.1: Currently I don't test under this platform, tell me if you have
             problems, I tested in the past and worked OK. Most of the W9x
             restrictions applies.

Windows 9x:
  Keyboard:
    Almost the same as DOS so read the DOS section first. The TSR works at
   least in Windows '95. Some keys are used by the GUI so not all are
   available I include a .pif example showing how to avoid W9x take control
   of Alt+Space and other keys. The example is in extra/pifexamp.
  Screen:
    The support is similar to the one found in DOS, but I know some video
   drivers have bugs in the VESA handling so perhaps it could make problems.
   If you face such a problem please contact me, in the past I fixed VESA
   problems with S3 boards thanks to the help of a user.
    Usually isn't a good idea to run applications that change the video mode
   inside a window, so if your program does it recommend not running in a
   window to your users. One way to avoid it is using a .pif file, it will
   prevent 98% of the people from "windowizing" the program. See the
   extra/pifexamp example for it, it instructs W95 to run the program in full
   screen and disable Alt+Enter, the example also gives up to 64Mb to the
   application (the maximun W95 will give to a DOS task).
  Mouse:
    Full support.
  Note: I think all of it applies to Windows 98, if you see any difference
        please tell me.

Windows NT: This platform isn't supported, I know the mouse fails to work and
            you should disable it. I think NT's support of DOS applications
            is completly broken so I don't care about it. If you have patches
            I'll receive it.




8. How to submit a patch:
------------------------

  The simplest way is running a diff between your current directory and a
copy of the library as you downloaded it. Don't forget to specify the exact
version of the distribution and the purpose of the patchs. Use the unified
mode (-u), this mode is the best for humans ;-)




9. Contact information:
----------------------

Salvador E. Tropea (SET)
Curapaligue 2124
3 de Febrero, Caseros
(1678) Bs.As. Argentina

Phone: (+5411) 4759-0013

e-mail: salvador@inti.gov.ar, set-soft@usa.net, set@computer.org
http:   www.geocities.com/SiliconValley/Vista/6552/



Enjoy, SET


