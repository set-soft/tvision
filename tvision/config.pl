#!/usr/bin/perl
# Copyright (C) 1999-2007 by Salvador E. Tropea (SET),
# see copyrigh file for details
#
# To specify the compilation flags define the CFLAGS environment variable.
#

require "miscperl.pl";
require "conflib.pl";

# If the script is newer discard the cache.
#GetCache() unless (-M 'config.pl' < -M 'configure.cache');
GetVersion('');

# I never tested with an older version, you can try reducing it.
$GPMVersionNeeded='1.10';
# I never tested with an older version, you can try reducing it.
#$NCursesVersionNeeded='1.9.9';
# That's a test to see if that works:
$NCursesVersionNeeded='1.8.6';
$DJGPPVersionNeeded='2.0.2';
unlink $ErrorLog;
$UseDummyIntl=0;

SeeCommandLine();

if ($JustSpec)
  {
   UpdateSpec();
   exit 0;
  }

print "Configuring Turbo Vision v$Version library\n\n";
# Determine the OS
$OS=DetectOS();
# Test for a working gcc
$GCC=CheckGCC();
# Determine C flags
$CFLAGS=FindCFLAGS();
# Determine C++ flags
$CXXFLAGS=FindCXXFLAGS();
# Extra lib directories
$LDExtraDirs=FindLDExtraDirs();
# Check if gcc can compile C++
$GXX=CheckGXX();
# Which architecture are we using?
DetectCPU();
# 32 or 64 bits pointers?
DetectPointersSize();
# Some platforms aren't easy to detect until we can compile.
DetectOS2();
# The prefix can be better determined if we know all the rest
# about the environment
LookForPrefix();
# Only gnu make have the command line and commands we use.
LookForGNUMake();
# Same for ar, it could be `gar'
$GAR=LookForGNUar();
# Similar for install tool.
LookForGNUinstall();
# Look for xgettext
LookForGettextTools();
# Is the right djgpp?
if ($OS eq 'DOS')
  {
   LookForDJGPP($DJGPPVersionNeeded);
  }

if ($OS eq 'UNIX')
  {
   LookForGPM($GPMVersionNeeded);
   LookForNCurses($NCursesVersionNeeded);
   LookForKeysyms();
   LookForXlib();
   # Used for X11 driver. Linux implementation of POSIX threads is very bad
   # and needs a lot of workarounds. Some of them could be just bugs in the
   # glibc I use but the fact is that the needed tricks make it very Linux
   # dependent.
   LookForPThread() if $OSf eq 'Linux';
   LookForOutB();
  }
if ($Compf eq 'Cygwin')
  {# Cygwin incorporates a XFree86 port
   LookForXlib();
  }
LookForIntlSupport();
LookForEndianess();
LookForMaintainerTools() if $conf{'MAINTAINER_MODE'} eq 'yes';

print "\n";
GenerateMakefile();
#
# For the examples
#
$here=RunRedirect('pwd',$ErrorLog);
chop($here);
if (!$here && ($OS ne 'UNIX'))
  {# command.com, cmd.exe, etc. have it.
   $here=`cd`;
   chop($here);
  }
if ($Compf eq 'MinGW')
  {
   $here=~s/\\/\//g;
  }
$realPrefix=@conf{'real-prefix'};
$realPrefix=@conf{'prefix'} unless $realPrefix;
# Path for the includes, used for examples
$MakeDefsRHIDE[1]='TVSRC=../../include ';
$MakeDefsRHIDE[1].=$here.'/include ' unless $conf{'libs-here'} eq 'no';
$MakeDefsRHIDE[1].=$realPrefix.'/include/rhtvision';
$MakeDefsRHIDE[1].=' '.$conf{'X11IncludePath'} if (@conf{'HAVE_X11'} eq 'yes');
# Path reported by rhtv-config --include
$MakeDefsRHIDE[11]='INCLUDE='.$realPrefix.'/include/rhtvision';
$MakeDefsRHIDE[11].=' '.$conf{'X11IncludePath'} if (@conf{'HAVE_X11'} eq 'yes');
# Extra path for the includes, used for the library
$MakeDefsRHIDE[9]='EXTRA_INCLUDE_DIRS='.$conf{'EXTRA_INCLUDE_DIRS'};
$MakeDefsRHIDE[7]='RHIDE_LIBS=';
# Libraries needed
$MakeDefsRHIDE[2]='RHIDE_OS_LIBS=';
# RHIDE doesn't know about anything different than DJGPP and Linux so -lstdc++ must
# be added for things like FreeBSD or SunOS. But not for QNX.
$MakeDefsRHIDE[2].=substr($stdcxx,2); # unless (($OS eq 'DOS') || ($OSf eq 'Linux') || ($OSf eq 'QNXRtP'));
# Linux, Solaris and FreeBSD have gettext in its C library.
$OSUSesIntl=!(($OSf eq 'Linux') || ($OSf eq 'Solaris') || ($OSf eq 'FreeBSD'));
if ($OSUSesIntl)
  {
   if ((@conf{'intl-force-dummy'} ne 'yes') && (@conf{'intl'} eq 'yes'))
     {
      $MakeDefsRHIDE[2].=' intl';
     }
   else
     {# Use the dummy unless the user requested not to use it
      if ((@conf{'intl-force-dummy'} eq 'yes') || (@conf{'no-intl'} ne 'yes'))
        {
         $MakeDefsRHIDE[2].=' tvfintl';
         $UseDummyIntl=1;
        }
     }
  }
$MakeDefsRHIDE[2].=' iconv' if (@conf{'iconv'} eq 'yes') && !$UseDummyIntl;
$MakeDefsRHIDE[2].=' '.$conf{'NameCurses'} if ($conf{'ncurses'} ne 'no') && ($OS eq 'UNIX');
$MakeDefsRHIDE[2].=' m' if ($OS eq 'UNIX');
$MakeDefsRHIDE[2].=' gpm' if @conf{'HAVE_GPM'} eq 'yes';
$MakeDefsRHIDE[2].=' '.$conf{'X11Lib'} if ($conf{'HAVE_X11'} eq 'yes');
$MakeDefsRHIDE[2].=' mss' if @conf{'mss'} eq 'yes';
$MakeDefsRHIDE[2].=' intl' if ((($OSf eq 'FreeBSD') || ($OSf eq 'QNXRtP')) && ($conf{'intl'} eq 'yes'));
$MakeDefsRHIDE[2].=' pthread' if $conf{'HAVE_LINUX_PTHREAD'} eq 'yes';
$MakeDefsRHIDE[2].=' termlib unix' if ($OSf eq 'QNX4');
if ($OS eq 'UNIX')
  {
   $MakeDefsRHIDE[0]='RHIDE_STDINC=/usr/include /usr/local/include /usr/include/g++ /usr/local/include/g++ /usr/lib/gcc-lib /usr/local/lib/gcc-lib';
   if (@conf{'HAVE_X11'} eq 'yes')
     {
      $aux=$conf{'X11IncludePath'} ? ' '.$conf{'X11IncludePath'} : ' /usr/X11R6/include';
      $MakeDefsRHIDE[0].=$aux;
      $MakeDefsRHIDE[9].=$aux;
     }
   $MakeDefsRHIDE[3]='TVOBJ='.$LDExtraDirs.' ';
   # QNX 6.2 beta 3 workaround
   $MakeDefsRHIDE[3].='/lib ' if ($OSf eq 'QNXRtP');
   # Link with installed libraries
   $MakeDefsRHIDE[3].=$realPrefix.'/lib ';
   $MakeDefsRHIDE[3].='../../makes ';
   $MakeDefsRHIDE[3].=$here.'/makes ' unless $conf{'libs-here'} eq 'no';
   $MakeDefsRHIDE[3].='../../intl/dummy ' if $UseDummyIntl;
   $MakeDefsRHIDE[3].=$conf{'X11LibPath'}.' ' if ($conf{'HAVE_X11'} eq 'yes');
  }
elsif ($OS eq 'DOS')
  {
   $MakeDefsRHIDE[0]='RHIDE_STDINC=$(DJDIR)/include $(DJDIR)/lang/cxx $(DJDIR)/lib/gcc-lib';
   # DJGPP's gcc includes djgpp.ver from the same directory where specs
   $a=`redir -eo $GCC -v`;
   if ($a=~/(\w:)(.*)\/specs/i)
     {
      $here=RunRedirect('pwd',$ErrorLog);
      chop($here);
      chdir("$1$2");
      $a=RunRedirect('pwd',$ErrorLog);
      chop($a);
      chdir($here);
      $MakeDefsRHIDE[0].=" $a"
     }
   $MakeDefsRHIDE[3]='TVOBJ=../../makes ';
   $MakeDefsRHIDE[3].=$here.'/makes ' unless $conf{'libs-here'} eq 'no';
   $MakeDefsRHIDE[3].=$realPrefix.'/lib '.$LDExtraDirs;
   $MakeDefsRHIDE[3].=' ../../intl/dummy' if $UseDummyIntl;
  }
elsif ($OS eq 'Win32')
  {
   $MakeDefsRHIDE[0]='RHIDE_STDINC=';
   $MakeDefsRHIDE[2].=' gdi32'; # Needed for WinGr driver
   $MakeDefsRHIDE[3]='TVOBJ=../../makes ';
   $MakeDefsRHIDE[3].=$here.'/makes ' unless $conf{'libs-here'} eq 'no';
   $MakeDefsRHIDE[3].=$realPrefix.'/lib '.$LDExtraDirs;
   $MakeDefsRHIDE[3].=' ../../intl/dummy' if $UseDummyIntl;
   $MakeDefsRHIDE[3].=' '.$conf{'X11LibPath'} if ($conf{'HAVE_X11'} eq 'yes');
  }
$MakeDefsRHIDE[4]='STDCPP_LIB='.$stdcxx;
# C options for dynamic lib
if ($OSf ne 'Darwin')
  {
   $MakeDefsRHIDE[5]='SHARED_CODE_OPTION=-fPIC';
   $MakeDefsRHIDE[5].=' -shared' if ($OSf eq 'QNXRtP');
  }
else
  {# PPC code is always position independent
   # However, the linker doesn't allow "common" symbols in shared libraries.
   $MakeDefsRHIDE[5]='SHARED_CODE_OPTION=-fno-common';
  }
# Flags to link as a dynamic lib
$ldflags1='RHIDE_LDFLAGS=';
$ldflags2=$ldflags1;
if ($OS eq 'UNIX')
  {
   if ($OSf ne 'Darwin')
     {
      $soname='-soname';
      if ($OSf eq 'Solaris')
        {
         system("$GCC -v 2> test.ld");
         $test=cat('test.ld');
         unlink 'test.ld';
         # Why?! I think gcc should translate it when using the native ld.
         $soname='-h' if $test=~'ccs/bin/ld';
        }
      $ldflags1.='-L/lib' if ($OSf eq 'QNXRtP');
      $ldflags1.=' -shared -Wl,'.$soname.',librhtv.so.'.$Version;
     }
   else
     {# Darwin semantic for dynamic libs is quite different
      $ldflags1.='-dynamiclib -install_name '.$realPrefix.'/lib/librhtv.'.$Version.'.dylib';
      $ldflags1.=' -compatibility_version '.$Version.' -current_version '.$Version;
     }
   $libs=$conf{'X11Lib'};
   $libs=~s/(\S+)/-l$1/g;
   $aux='';
   $aux.=" -L".$conf{'X11LibPath'}." $libs" if @conf{'HAVE_X11'} eq 'yes';
   $aux.=' -lgpm' if @conf{'HAVE_GPM'} eq 'yes';
   $aux.=(($OSf eq 'QNXRtP') ? ' -lncursesS' : ' -lncurses') unless $conf{'ncurses'} eq 'no';
   $aux.=" $stdcxx -lm -lc";
   $aux.=' -lpthread' if $conf{'HAVE_LINUX_PTHREAD'} eq 'yes';
   $aux.=' libtvfintl.a' if ($OSf eq 'Darwin') && $UseDummyIntl;
   $MakeDefsRHIDE[7].=$aux;
  }

$MakeDefsRHIDE[8]="LIB_VER=$Version";
$MakeDefsRHIDE[10]="LIB_VER_MAJOR=$VersionMajor";

ModifyMakefiles('intl/dummy/Makefile');
$MakeDefsRHIDE[6]=$ldflags1;
CreateRHIDEenvs('makes/rhide.env','compat/rhide.env');
$MakeDefsRHIDE[6]=$ldflags2;
CreateRHIDEenvs('examples/rhide.env');

# Repeated later for other targets
CreateConfigH();

# Help BC++ target
system("perl confignt.pl");

#
# Adjust .mak files
#
print "Makefiles for examples.\n";
chdir('examples');
`perl patchenv.pl`;
chdir('..');

print "Makefiles for translations.\n";
ReplaceText('intl/gnumake.in','intl/Makefile');
UpdateSpec();

print "\nSuccesful configuration!\n\n";

GiveAdvice();
CreateCache();
unlink $ErrorLog;
unlink 'test.exe';

sub UpdateSpec()
{
 $ReplaceTags{'version'}=$Version;
 ReplaceText('redhat/librhtv.spec.in',"redhat/librhtv-$Version.spec");
 ReplaceText('qnxrtp/tvision.qpg.in',"qnxrtp/tvision.qpg");
}

sub SeeCommandLine
{
 my $i;

 foreach $i (@ARGV)
   {
    if ($i eq '--help')
      {
       ShowHelp();
       die "\n";
      }
    elsif ($i=~'--prefix=(.*)')
      {
       $conf{'prefix'}=$1;
      }
    elsif ($i=~'--real-prefix=(.*)')
      {
       $conf{'real-prefix'}=$1;
      }
    elsif ($i eq '--no-intl')
      {
       $conf{'no-intl'}='yes';
      }
    elsif ($i eq '--force-dummy')
      {
       $conf{'intl-force-dummy'}='yes';
      }
    elsif ($i=~'--cflags=(.*)')
      {
       @conf{'CFLAGS'}=$1;
      }
    elsif ($i=~'--cxxflags=(.*)')
      {
       @conf{'CXXFLAGS'}=$1;
      }
    elsif ($i eq '--fhs')
      {
       $conf{'fhs'}='yes';
      }
    elsif ($i eq '--no-fhs')
      {
       $conf{'fhs'}='no';
      }
    elsif ($i=~'--X11lib=(.*)')
      {
       $conf{'X11Lib'}=$1;
      }
    elsif ($i eq '--with-mss')
      {
       $conf{'mss'}='yes';
      }
    elsif ($i eq '--without-mss')
      {
       $conf{'mss'}='no';
      }
    elsif ($i=~'--x-include=(.*)')
      {
       $conf{'X11IncludePath'}=$1;
      }
    elsif ($i=~'--x-lib=(.*)')
      {
       $conf{'X11LibPath'}=$1;
      }
    elsif ($i eq '--with-debug')
      {
       $conf{'debugInfo'}='yes';
      }
    elsif ($i eq '--without-debug')
      {
       $conf{'debugInfo'}='no';
      }
    elsif ($i eq '--with-ssc')
      {
       $conf{'HAVE_SSC'}='yes';
      }
    elsif ($i eq '--without-ssc')
      {
       $conf{'HAVE_SSC'}='no';
      }
    elsif ($i eq '--no-libs-here')
      {
       $conf{'libs-here'}='no';
      }
    elsif ($i eq '--enable-maintainer-mode')
      {
       $conf{'MAINTAINER_MODE'}='yes';
      }
    elsif ($i eq '--just-spec')
      {
       $JustSpec=1;
      }
    elsif ($i eq '--with-pthread')
      {
       $conf{'try-pthread'}='yes';
      }
    elsif ($i eq '--without-pthread')
      {
       $conf{'try-pthread'}='no';
      }
    elsif ($i eq '--without-static')
      {
       $conf{'no-static'}='yes';
      }
    elsif ($i eq '--without-dynamic')
      {
       $conf{'no-dynamic'}='yes';
      }
    elsif ($i eq '--unsafe-memcpy')
      {
       $conf{'HAVE_UNSAFE_MEMCPY'}='yes';
      }
    elsif ($i eq '--safe-memcpy')
      {
       $conf{'HAVE_UNSAFE_MEMCPY'}='no';
      }
    elsif ($i=~'--include=(.*)')
      {
       $conf{'EXTRA_INCLUDE_DIRS'}.=" $1";
      }
    elsif ($i eq '--debug')
      {
       $conf{'CFLAGS'}=
       $conf{'CXXFLAGS'}='-O2 -Wall -Werror -gstabs+3';
      }
   # For compatibility with autoconf:
    # LinCS/tiger - ignore some autoconf generated params
    elsif (($i=~'--cache-file=(.*)') || ($i=~'--srcdir=(.*)') || ($i=~'--enable-ltdl-convenience'))
      {
      }
    else
      {
       ShowHelp();
       die "Unknown option: $i\n";
      }
   }
}

sub ShowHelp
{
 print "Available options:\n\n";
 print "Flags:\n";
 print "--cflags=val    : normal C flags [default is env. CFLAGS].\n";
 print "--cxxflags=val  : normal C++ flags [default is env. CXXFLAGS].\n";
 print "--debug         : selects C/C++ switches for debugging\n";
 
 print "\nPaths and library names:\n";
 print "--x-include=path: X11 include path [/usr/X11R6/lib].\n";
 print "--x-lib=path    : X11 library path [/usr/X11R6/include].\n";
 print "--X11lib=val    : Name of X11 libraries [default is X11 Xmu].\n";
 print "--include=path  : Add this path for includes. Repeat for each dir.\n";
 print "--no-libs-here  : Don't use the sources path for libs.\n";
 
 print "\nIntallation:\n";
 print "--prefix=path   : defines the base directory for installation.\n";
 print "--fhs           : force the FHS layout under UNIX.\n";
 print "--no-fhs        : force to not use the FHS layout under UNIX.\n";
 print "--with-debug    : install dynamic library without running strip.\n";
 print "--without-debug : run strip to reduce the size [default]\n";
 print "--real-prefix=pa: real prefix, for Debian package\n";
 
 print "\nLibraries:\n";
 print "--force-dummy    : use the dummy intl library even when gettext is detected.\n";
 print "--no-intl        : don't use international support.\n";
 print "--without-static : don't create the static library.\n";
 print "--without-dynamic: don't create the dynamic library.\n";
 print "--with-mss       : compiles with MSS memory debugger.\n";
 print "--without-mss    : compiles without MSS [default].\n";
 print "--with-ssc       : compiles using Simple Streams Compatibility.\n";
 print "--without-ssc    : compiles without SSC [default].\n";
 print "--with-pthread   : uses pthread for X11 driver.\n";
 print "--without-pthread: avoids pthread for X11 driver [default].\n";
 
 print "\nOthers:\n";
 print "--enable-maintainer-mode:\n";
 print "                : enables header dependencies and other stuff needed\n";
 print "                  for developement, not just use the editor.\n";
 print "--just-spec     : update RPMs spec file and exit.\n";
 print "--unsafe-memcpy : disable the use of memcpy when memory overlaps\n";
 print "--help          : displays this text.\n";
}

sub GiveAdvice
{
 if ((@conf{'intl'} eq 'no') && (@conf{'no-intl'} ne 'yes'))
   {
    print "\n";
    print "* The international support was disabled because gettext library could not\n";
    print "  be detected.\n";
    if ($OSf eq 'Linux')
      {
       print "  Starting with glibc 2.0 this is included in libc, perhaps your system\n";
       print "  just lacks the propper header file.\n";
      }
    elsif ($OS eq 'DOS')
      {
       print "  Install the gtxtNNNb.zip package from the v2gnu directory of djgpp's\n";
       print "  distribution. Read the readme file for more information.\n";
      }
    elsif ($Compf eq 'MinGW')
      {
       print "  That's normal for MinGW.\n";
      }
    elsif ($Compf eq 'Cygwin')
      {
       print "  Install gettext library.\n";
      }
   }
 if ($UseDummyIntl)
   {
    print "  [[[[[[[*******************>>>>> IMPORTANT!!! <<<<<*******************]]]]]]]\n";
    print "  You must link with libtvfintl.a or you'll get undefined symbols. To avoid\n";
    print "  using this library reconfigure using --no-intl. Read about it in the readme.\n";
    print "  [[[[[[[*******************>>>>> IMPORTANT!!! <<<<<*******************]]]]]]]\n";
   }
 if ((@conf{'HAVE_GPM'} eq 'no') && ($OSf eq 'Linux'))
   {
    print "\n";
    print "* No mouse support for console! please install the libgpm package needed\n";
    print "  for development. (i.e. libgpmg1-dev_1.13-5.deb).\n";
   }
 if (@conf{'GNU_Make'} ne 'make')
   {
    print "\n";
    print "* Please use $conf{'GNU_Make'} instead of make command.\n";
   }
 if ((@conf{'HAVE_X11'} eq 'no') && ($OS eq 'UNIX'))
   {
    print "\n";
    print "* No X11 support detected. If X11 is installed make sure you have the\n";
    print "  development package installed (i.e. xlibs-dev). Also check the options\n";
    print "  to specify the paths for X11 libs and headers\n";
   }
}

sub LookForEndianess
{

 my $test;

 print 'Checking endianess: ';

 if (@conf{'TV_BIG_ENDIAN'} eq 'yes')
   {
    print "big endian (cached)\n";
    return;
   }
 if (@conf{'TV_BIG_ENDIAN'} eq 'no')
   {
    print "little endian (cached)\n";
    return;
   }
 $test='
#include <stdio.h>
int main(void)
{
 int a=1;
 char *s=(char *)&a;
 printf("%s\n",s[0]==1 ? "little" : "big");
 return 0;
}
';
 $test=RunGCCTest($GCC,'c',$test,'');
 $test=~s/\W//g;
 $conf{'TV_BIG_ENDIAN'}=($test eq "big") ? 'yes' : 'no';
 print "$test endian\n";
}

sub LookForIntlSupport
{
 my $vNeed=$_[0];
 my ($test,$a,$djdir,$intllib,$intltest,$libdir);

 print 'Checking for international support: ';
 if (@conf{'intl-force-dummy'} eq 'yes')
   {
    print "using dummy by user request.\n";
    $conf{'intl'}='yes';
    $conf{'iconv'}='no';
    return;
   }
 if (@conf{'no-intl'} eq 'yes')
   {
    print "disabled by user request.\n";
    $conf{'intl'}='no';
    $conf{'iconv'}='no';
    #`cp include/tv/nointl.h include/tv/intl.h`;
    return;
   }
 if (@conf{'intl'} eq 'yes')
   {
    print "yes (cached)\n";
    return;
   }
 if (@conf{'intl'} eq 'no')
   {
    print "no (cached)\n";
    return;
   }
 if ($OS eq 'DOS')
   { # gettext 0.10.32 port have a bug in the headers, correct it
    $djdir=@ENV{'DJDIR'};
    $a=cat("$djdir/include/libintl.h");
    if (length($a) && $a=~/\@INCLUDE_LOCALE_H@/)
      {
       $a=~s/\@INCLUDE_LOCALE_H\@//;
       replace("$djdir/include/libintl.h",$a);
      }
   }
 $intltest='
#include <stdio.h>
#define FORCE_INTL_SUPPORT
#include <tv/intl.h>
int main(void)
{
 printf("%s\n",_("OK"));
 return 0;
}
';
 $intllib=(($OS eq 'DOS') || ($OS eq 'Win32') || ($OSf eq 'FreeBSD') || ($OSf eq 'QNXRtP')) ? '-lintl' : '';
 $libdir=$LDExtraDirs;
 $libdir=~s/(\S+)/-L$1/g;
 $test=RunGCCTest($GCC,'c',$intltest,'-Iinclude/ '.$libdir.' '.$intllib);
 $test=~s/\W//g;
 if ($test ne "OK")
   {
    print "no, additional check required.\n";
    print "Checking for extra libs for international support: ";
    $test=RunGCCTest($GCC,'c',$intltest,'-Iinclude/ '.$intllib.' -liconv');
    $test=~s/\W//g;
    if ($test ne "OK")
      {
       print "none found\n";
       print "International support absent or non-working\n";
       $conf{'intl'}='no';
       $conf{'iconv'}='no';
      }
    else
      {
       print "-liconv, OK\n";
       $conf{'intl'}='yes';
       $conf{'iconv'}='yes';
      }
   }
 else
   {
    print "yes OK\n";
    $conf{'intl'}='yes';
    $conf{'iconv'}='no';
   }
}

sub LookForKeysyms
{
 my $test;

 print 'Looking for X keysyms definitions: ';
 if (@conf{'HAVE_KEYSYMS'})
   {
    print "@conf{'HAVE_KEYSYMS'} (cached)\n";
    return;
   }
 $test='
#include <stdio.h>
#include <X11/keysym.h>
int main(void)
{
 if (XK_Return!=0)
    printf("OK\n");
 return 0;
}
';
 $test=RunGCCTest($GCC,'c',$test,'');
 $test=~s/\W//g;
 if ($test eq "OK")
   {
    $conf{'HAVE_KEYSYMS'}='yes';
    print " yes OK\n";
   }
 else
   {
    $conf{'HAVE_KEYSYMS'}='no';
    print " no, disabling enhanced support for Eterm 0.8.10+\n";
   }
}

sub LookForXlib()
{
 my ($test,$o,$libs,$testPr,$libs);

 print 'Looking for X11 libs: ';
 if (@conf{'HAVE_X11'})
   {
    print "@conf{'HAVE_X11'} (cached)\n";
    return;
   }
 $testPr='
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
Display *Test()
{ return XOpenDisplay(""); }
int main(void)
{
 printf("OK, %d.%d\n",X_PROTOCOL,X_PROTOCOL_REVISION);
 return 0;
}
';
 $conf{'X11LibPath'}='/usr/X11R6/lib' unless $conf{'X11LibPath'};
 # Looks like Cygwin does all static!
 $libs=($Compf eq 'Cygwin') ? 'Xmu Xt SM ICE X11' : 'X11 Xmu';
 # QNX4 does all static!
 $libs='Xmu Xt SM ICE X11 socket' if ($OSf eq 'QNX4');
 $conf{'X11Lib'}=$libs unless $conf{'X11Lib'};
 $libs=$conf{'X11Lib'};
 $libs=~s/(\S+)/-l$1/g;
 $o='';
 $o.='-I'.$conf{'X11IncludePath'} if $conf{'X11IncludePath'};
 $o.=" -L$conf{'X11LibPath'} $libs";
 $test=RunGCCTest($GCC,'c',$testPr,$o);
 if ($test=~/OK, (\d+)\.(\d+)/)
   {
    $conf{'HAVE_X11'}='yes';
    print "yes OK (X$1 rev $2)\n";
   }
 else
   {
    if (!$conf{'X11IncludePath'})
      {
       $conf{'X11IncludePath'}='/usr/X11R6/include';
       $o.=" -I$conf{'X11IncludePath'} -L$conf{'X11LibPath'} $libs";
       $test=RunGCCTest($GCC,'c',$testPr,$o);
       if ($test=~/OK, (\d+)\.(\d+)/)
         {
          $conf{'HAVE_X11'}='yes';
          print "yes OK (X$1 rev $2)\n";
          return;
         }
      }
    $conf{'HAVE_X11'}='no';
    print "no, disabling X11 version\n";
   }
}

#
# GlibC 2.1.3 defines it by itself, lamentably doesn't have any protection
# mechanism to avoid collisions with the kernel headers, too bad.
#
sub LookForOutB
{
 my $test;

 print 'Looking for outb definition in sys/io.h: ';
 if (@conf{'HAVE_OUTB_IN_SYS'})
   {
    print "@conf{'HAVE_OUTB_IN_SYS'} (cached)\n";
    return;
   }
 $test='
#include <stdio.h>
#include <sys/io.h>
#ifdef __i386__
void Test(void) { outb(10,0x300); }
#endif
int main(void)
{
 printf("OK\n");
 return 0;
}
';
 $test=RunGCCTest($GCC,'c',$test,'');
 $test=~s/\W//g;
 $conf{'HAVE_OUTB_IN_SYS'}=($test eq "OK") ? 'yes' : 'no';
 print "@conf{'HAVE_OUTB_IN_SYS'}\n";
 #print ">$test<\n";
}

sub LookForGPM
{
 my $vNeed=$_[0],$test;

 print 'Looking for gpm library: ';
 if (@conf{'gpm'})
   {
    print "@conf{'gpm'} (cached) OK\n";
    return;
   }
 $test='
#include <stdio.h>
#include <gpm.h>
int main(void)
{
 int version;
 printf("%s",Gpm_GetLibVersion(&version));
 return 0;
}
';
 $test=RunGCCTest($GCC,'c',$test,'-lgpm');
 if (!length($test))
   {
    #print "\nError: gpm library not found, please install gpm $vNeed or newer\n";
    #print "Look in $ErrorLog for potential compile errors of the test\n";
    #CreateCache();
    #die "Missing library\n";
    $conf{'HAVE_GPM'}='no';
    print " no, disabling mouse support\n";
    return;
   }
 if (!CompareVersion($test,$vNeed))
   {
    #print "$test, too old\n";
    #print "Please upgrade your gpm library to version $vNeed or newer.\n";
    #print "You can try with $test forcing the configure scripts.\n";
    #CreateCache();
    #die "Old library\n";
    $conf{'HAVE_GPM'}='no';
    print " too old, disabling mouse support\n";
    return;
   }
 $conf{'gpm'}=$test;
 $conf{'HAVE_GPM'}='yes';
 print "$test OK\n";
}

sub LookForNCurses
{
 my ($vNeed)=@_;
 my ($result,$test);

 print 'Looking for ncurses library: ';
 if (@conf{'ncurses'})
   {
    print "@conf{'ncurses'} (cached) OK\n";
    return;
   }
 # Assume it is -lncurses
 if ($OSf ne 'QNXRtP')
 {
  $conf{'NameCurses'}='ncurses'
 }
 else
 {
  $conf{'NameCurses'}='ncursesS'
 }
 $test='
#include <stdio.h>
#if defined(__QNX__) && !defined(__QNXNTO__)
 #include <curses.h>
#else
 #include <ncurses.h>
#endif
void dummy() {initscr();}
int main(void)
{
 printf(NCURSES_VERSION);
 return 0;
}
';
 $result=RunGCCTest($GCC,'c',$test,'-lncurses') if ($OSf ne 'QNX4');
 $result=RunGCCTest($GCC,'c',$test,'-lncurses -lunix') if ($OSf eq 'QNX4');
 if (!length($result))
   {# Try again with -lcurses, In Solaris ncurses is installed this way
    $result=RunGCCTest($GCC,'c',$test,'-lcurses');
    if (!length($result))
      {
       print "no, disabling UNIX driver\n";
       $conf{'ncurses'}='no';
       return;
      }
    $conf{'NameCurses'}='curses';
   }
 if (!CompareVersion($result,$vNeed))
   {
    print "$result, too old, disabling UNIX driver\n";
    $conf{'ncurses'}='no';
    return;
   }
 print "$result OK\n";
 @conf{'ncurses'}=$result;

 print 'Checking if ncurses have define_key: ';
 $test='
#include <stdio.h>
#include <ncurses.h>
void dummy(void) { define_key("\x1B[8~",KEY_F(59)); /* End */ }
int main(void)
{
 printf("Ok\n");
 return 0;
}
';
 $result=RunGCCTest($GCC,'c',$test,'-l'.$conf{'NameCurses'});
 $result=~s/\W//g;
 if ($result eq 'Ok')
   {
    print "yes\n";
    $conf{'HAVE_DEFINE_KEY'}=1;
   }
 else
   {
    print "no\n";
    $conf{'HAVE_DEFINE_KEY'}=0;
   }
}

sub LookForGettextTools
{
 my $test;

 print 'Looking for xgettext: ';
 if (@conf{'xgettext'})
   {
    print @conf{'xgettext'}." (cached)\n";
    return;
   }
 #$test=`xgettext --version`;
 $test=RunRedirect('xgettext --version',$ErrorLog);
 if ($test=~/(\d+\.\d+(\.\d+)?)/)
   {
    print "$1\n";
    $conf{'xgettext'}=$1;
   }
 else
   {
    print "no\n";
    $conf{'xgettext'}='no';
   }
}

sub LookForRecode
{
 my $test;

 print 'Looking for recode: ';
 if (@conf{'recode'})
   {
    print @conf{'recode'}." (cached)\n";
    return;
   }
 #$test=`recode --version`;
 $test=RunRedirect('recode --version',$ErrorLog);
 if ($test=~/(\d+\.\d+(\.\d+)?)/)
   {
    print "$1\n";
    $conf{'recode'}=$1;
    if (CompareVersion($test,'3.5'))
      {
       $conf{'recode_sep'}='..';
      }
    else
      {
       $conf{'recode_sep'}=':';
      }
   }
 else
   {
    print "no\n";
    $conf{'recode'}='no';
   }
}

sub LookForPThread
{
 my $test;

 if ($conf{'try-pthread'} ne 'yes')
   {
    $conf{'HAVE_LINUX_PTHREAD'}='no';
    return;
   }

 print 'Looking for pthread (LinuxThreads) library: ';
 if (@conf{'HAVE_LINUX_PTHREAD'})
   {
    print "@conf{'HAVE_LINUX_PTHREAD'} (cached)\n";
    return;
   }
 $test='
// We need it for recursive mutex
#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>

static pthread_t th;
static pthread_mutex_t mutex;

void *Test(void *unused)
{// The OK will be printed only if the thread worked
 printf("OK\n");
 return NULL;
}

int main(int argc, char *argv[])
{// Test the attribute for recursive mutex.
 pthread_mutexattr_t mt_attr;
 pthread_mutexattr_init(&mt_attr);
 pthread_mutexattr_settype(&mt_attr,PTHREAD_MUTEX_RECURSIVE);
 pthread_mutex_init(&mutex,&mt_attr);
 // Create a thread
 pthread_create(&th,NULL,Test,NULL);
 // Wait until it finishes
 pthread_join(th,NULL);
 return 0;
}
';
 $test=RunGCCTest($GCC,'c',$test,'-lpthread');
 if (!length($test))
   {
    $conf{'HAVE_LINUX_PTHREAD'}='no';
    print " no, disabling X11 update thread option\n";
    return;
   }
 $conf{'HAVE_LINUX_PTHREAD'}='yes';
 print "OK\n";
}

sub DetectPointersSize()
{
 my $test;

 print 'Looking for pointer size: ';
 if ($conf{'HAVE_64BITS_POINTERS'})
   {
    print ($conf{'HAVE_64BITS_POINTERS'} eq 'yes' ? '64' : '32')."bits (cached)\n";
    return;
   }

 $test='
#include <stdio.h>

int main(void)
{
 printf("%d\n",(int) sizeof(void *));
 return 0;
}
';
 $test=RunGCCTest($GCC,'c',$test,'');
 $test=~s/\W//g;
 if ($test eq '8')
   {
    $conf{'HAVE_64BITS_POINTERS'}='yes';
    print " 64 bits\n";
   }
 elsif ($test eq '4')
   {
    $conf{'HAVE_64BITS_POINTERS'}='no';
    print " 32 bits\n";
   }
 else
   {
    CreateCache();
    die "Unknown pointer size!!\n";
   }
}

sub GenerateMakefile
{
 my ($text,$rep,$makeDir,$ver,$internac,$maintain);
 my ($dosta,$dodyn,$nameSO,$nameSOM,$nameSOV,$stripDebug);

 print "Generating Makefile\n";
 $text=cat('Makefile.in');
 if (!$text)
   {
    CreateCache();
    die "Can't find Makefile.in!!\n";
   }
 $internac=@conf{'xgettext'} ne 'no';
 $maintain=@conf{'MAINTAINER_MODE'} eq 'yes';

 $dosta=@conf{'no-static'} ne 'yes';
 $dodyn=($OS eq 'UNIX') && ($OSf ne 'QNX4') && (@conf{'no-dynamic'} ne 'yes');
 if (!$dosta && !$dodyn)
   {
    CreateCache();
    die "No static nor dynamic library created!!\n";
   }

 $rep ='';
 $rep.=' maintainance' if $maintain;
 $rep.=' static-lib' if $dosta;
 $rep.=' rhtv-config$(EXE_EXT)';
 $rep.=' dynamic-lib' if $dodyn;
 $rep.=' internac' if ($internac);
 $text=~s/\@targets\@/$rep/g;
 $text=~s/\@OS\@/$OS/g;
 $text=~s/\@prefix\@/@conf{'prefix'}/g;
 $text=~s/\@exe_ext\@/$ExeExt/g;
 $text=~s/\@maintainer_mode\@/MAINTAINER_MODE=1/g if $maintain;
 $text=~s/\@maintainer_mode\@//g                  unless $maintain;
 $text=~s/\@install\@/@conf{'GNU_INSTALL'}/;
 $text=~s/\@darwin\@/DARWIN=1/g   if $OSf eq 'Darwin';
 $text=~s/\@darwin\@//g           if $OSf ne 'Darwin';

 $makeDir='makes';

 # Write target maintainance rule:
 $rep='';
 $rep="maintainance:\n\t\$(MAKE) -C $makeDir -f maintain.mak" if $maintain;
 $text=~s/\@maintainance_rule\@/$rep/g;

 # Write target rules:
 $rep='';
 if ($dosta)
   {
    $rep.="static-lib:\n\t\$(MAKE) -C $makeDir -f librhtv.mkf";
    $rep.="\n\tranlib $makeDir/librhtv.a" if $conf{'UseRanLib'};
    $rep.="\n";
   }
 if ($dodyn)
   {
    $rep.="\ndynamic-lib: intl-dummy\n\t\$(MAKE) DYNAMIC_LIB=1 -C $makeDir -f librhtv.mkf\n";

    if ($OSf ne 'Darwin')
      {
       $nameSO='librhtv.so';
       $nameSOM="librhtv.so.$VersionMajor";
       $nameSOV="librhtv.so.$Version";
      }
    else
      {# Darwin uses a different name
       $nameSO='librhtv.dylib';
       $nameSOM="librhtv.$VersionMajor.dylib";
       $nameSOV="librhtv.$Version.dylib";
      }
    # Note: -sf should work for Solaris 9 but at least in the machines at S.F.
    # it doesn't. So we just delete the file and do the link.
    $rep.="\t-cd $makeDir; rm -f $nameSO; ln -s $nameSOV $nameSO\n";
    $rep.="\t-cd $makeDir; rm -f $nameSOM; ln -s $nameSOV $nameSOM\n";
   }
 if ($internac)
   {
    $rep.="\ninternac:\n";
    $rep.="\t\$(MAKE) -C intl\n";
   }
 $text=~s/\@target_rules\@/$rep/g;
 $rep="intl-dummy:\n\t\$(MAKE) -C intl/dummy\n";
 $rep.="\tcp intl/dummy/libtvfintl.a $makeDir\n";
 $rep.="\tranlib $makeDir/libtvfintl.a\n" if $conf{'UseRanLib'};
 $text=~s/\@intl_dummy_rule\@/$rep/g;

 $text=~s/\@GCC\@/$GCC/g;

 # Write install stuff
 # What versions of the library we will install
 $rep='';
 $rep.='install-static ' if $dosta;
 $rep.='install-dynamic ' if $dodyn;
 $rep.='install-internac ' if $internac;
 $text=~s/\@installers\@/$rep/g;

 # Headers

 $rep= GenInstallDir('0755','$(prefix)/include/rhtvision');
 $rep.="\trm -f \$(prefix)/include/rhtvision/*.h\n";
 $rep.="\t".GenInstallFiles('0644','include/*.h','$(prefix)/include/rhtvision');
 $rep.="\t".GenInstallDir('0755','$(prefix)/include/rhtvision/tv');
 $rep.="\t".GenInstallFiles('0644','include/tv/*.h','$(prefix)/include/rhtvision/tv');
 if ($OS eq 'DOS')
   {
    $rep.="\t".GenInstallDir('0755','$(prefix)/include/rhtvision/tv/dos');
    $rep.="\t".GenInstallFiles('0644','include/tv/dos/*.h','$(prefix)/include/rhtvision/tv/dos');
   }
 if ($OS eq 'UNIX')
   {
    $rep.="\t".GenInstallDir('0755','$(prefix)/include/rhtvision/tv/linux');
    $rep.="\t".GenInstallFiles('0644','include/tv/linux/*.h','$(prefix)/include/rhtvision/tv/linux');
   }
 if ($OSf eq 'QNXRtP')
   {
    $rep.="\t".GenInstallDir('0755','$(prefix)/include/rhtvision/tv/qnxrtp');
    $rep.="\t".GenInstallFiles('0644','include/tv/qnxrtp/*.h','$(prefix)/include/rhtvision/tv/qnxrtp');
   }
 if ($OS eq 'Win32')
   {
    $rep.="\t".GenInstallDir('0755','$(prefix)/include/rhtvision/tv/win32');
    $rep.="\t".GenInstallFiles('0644','include/tv/win32/*.h','$(prefix)/include/rhtvision/tv/win32');
   }
 if (@conf{'HAVE_X11'} eq 'yes')
   {
    $rep.="\t".GenInstallDir('0755','$(prefix)/include/rhtvision/tv/x11');
    $rep.="\t".GenInstallFiles('0644','include/tv/x11/*.h','$(prefix)/include/rhtvision/tv/x11');
   }
 $rep.="\t".GenInstallDir('0755','$(prefix)/include/rhtvision/cl');
 $rep.="\t".GenInstallFiles('0644','include/cl/*.h','$(prefix)/include/rhtvision/cl');
 $text=~s/\@install_headers\@/$rep/g;
 
 # Dummy replacement for i8n library
 $rep ="install-intl-dummy: intl-dummy\n";
 $rep.="\t".GenInstallDir('0755','$(libdir)');
 $rep.="\t".GenInstallFiles('0644','intl/dummy/libtvfintl.a','$(libdir)');
 # In Darwin the linker checks the time stamp of the ranlib pass and the one of
 # the file. It complains if we use ranlib and then copy the file.
 $rep.="\tranlib \$(libdir)/libtvfintl.a\n" if $conf{'UseRanLib'};
 $text=~s/\@intl_dummy_install_rule\@/$rep/g;

 # Static library
 $rep='';
 if ($dosta)
   {
    $rep.="install-static: static-lib\n";
    $rep.="\t".GenInstallDir('0755','$(libdir)');
    $rep.="\t".GenInstallFiles('0644',"$makeDir/librhtv.a",'$(libdir)');
   }

 if ($dodyn)
   {# Dynamic library
    $ver=($OSf eq 'FreeBSD') ? $nameSOM : $nameSOV;
    $rep.="\ninstall-dynamic: dynamic-lib\n";
    $rep.="\t".GenInstallDir('0755','$(libdir)');
    $rep.="\trm -f \$(libdir)/$nameSO\n";
    $rep.="\trm -f \$(libdir)/$nameSOM\n";
    $rep.="\trm -f \$(libdir)/$nameSOV\n";
    $rep.="\tcd \$(libdir); ln -s $ver $nameSO\n";
    $rep.="\tcd \$(libdir); ln -s $ver $nameSOV\n" if $OSf eq 'FreeBSD';
    $rep.="\t".GenInstallFiles('0644',"$makeDir/$ver",'$(libdir)');
    $stripDebug=($OSf eq 'Darwin') ? '-S' : '--strip-debug';
    $rep.="\tstrip $stripDebug \$(libdir)/$ver\n" unless $conf{'debugInfo'} eq 'yes';
    # FreeBSD: merge data from libdir.
    # Darwin: doesn't have ldconfig.
    if ($OSf ne 'Darwin')
      {
       $rep.=($OSf eq 'FreeBSD') ? "\t-ldconfig -m \$(libdir)\n" : "\t-ldconfig\n";
      }
   }
 if ($internac)
   {
    $rep.="\ninstall-internac:\n\t\$(MAKE) -C intl install\n";
   }
 $text=~s/\@install_rules\@/$rep/g;

 # rhtv-config installation
 $rep= GenInstallDir('0755','$(prefix)/bin');
 $rep.="\t".GenInstallFiles('0755','rhtv-config$(EXE_EXT)','$(prefix)/bin');
 $text=~s/\@install_config\@/$rep/g;

 $rep= "clean:\n";
 $rep.="\trm -f $makeDir/librhtv.so*\n";
 $rep.="\trm -f $makeDir/obj/*.o\n";
 $rep.="\trm -f $makeDir/obj/*.lo\n";
 $rep.="\trm -f $makeDir/librhtv.a\n";
 $rep.="\trm -f compat/obj/*.o\n";
 $rep.="\trm -f compat/obj/*.lo\n";
 $rep.="\trm -f intl/dummy/*.o\n";
 $rep.="\trm -f intl/dummy/*.lo\n";
 $rep.="\trm -f intl/dummy/*.a\n";
 $rep.="\t-\$(MAKE) -C examples clean\n";
 $rep.="\t-\$(MAKE) -C intl clean\n";
 $rep.="\trm -f configure.cache\n";
 $rep.="\trm -f rhtv-config\$(EXE_EXT)\n";
 $text=~s/\@clean\@/$rep/g;

 replace('Makefile',$text);
}

sub CreateConfigH
{
 my $text="/* Generated automatically by the configure script */";
 my $old;

 print 'Generating configuration header: ';

 $conf{'HAVE_NCURSES'}=($conf{'ncurses'} ne 'no') && ($OS eq 'UNIX') ? 'yes' : 'no';
 $text.=ConfigIncDefYes('HAVE_NCURSES','ncurses library');
 $text.=ConfigIncDef('HAVE_DEFINE_KEY','ncurses 4.2 or better have define_key (In Linux)');
 $text.=ConfigIncDefYes('HAVE_KEYSYMS','The X11 keysyms are there');
 $text.=ConfigIncDefYes('HAVE_X11','X11 library and headers');
 # Disable i8n only if the user requested, otherwise use gettext or the dummy
 $conf{'HAVE_INTL_SUPPORT'}=@conf{'no-intl'} eq 'yes' ? 'no' : 'yes';
 $text.=ConfigIncDefYes('HAVE_INTL_SUPPORT','International support with gettext');
 $text.=ConfigIncDefYes('HAVE_GPM','GPM mouse support');
 $text.=ConfigIncDefYes('HAVE_OUTB_IN_SYS','out/in functions defined by glibc');
 $text.=ConfigIncDefYes('HAVE_SSC','Use stream replacements');
 $text.=ConfigIncDefYes('TV_BIG_ENDIAN','Byte order for this machine');
 $text.=ConfigIncDefYes('HAVE_LINUX_PTHREAD','Linux implementation of POSIX threads');
 $text.=ConfigIncDefYes('HAVE_UNSAFE_MEMCPY','Memcpy doesn\'t support overlaps');
 $text.=ConfigIncDefYes('HAVE_64BITS_POINTERS','64 bits pointers');
 $text.="\n\n";
 $text.="#define TVOS_$OS\n";
 $text.="#define TVOSf_$OSf\n";
 $text.="#define TVCPU_$CPU\n";
 $text.="#define TVComp_$Comp\n";
 $text.="#define TVCompf_$Compf\n";
 $text.="\n#define MSS\n#include <mss.h>\n" if @conf{'mss'} eq 'yes';
 $text.="\n";
 foreach $line (@MakeDefsRHIDE)
   {
    if ($line=~/([\w_]*)(\s*)=(\s*)(.*)/)
      {
       $text.="#define TVCONFIG_$1 \"$4\"\n";
      }
   }
 $text.="#define TVCONFIG_CFLAGS   \"$CFLAGS\"\n";
 $text.="#define TVCONFIG_CXXFLAGS \"$CXXFLAGS\"\n";
 $text.="#define TVCONFIG_REF_DIR  \"$here\"\n\n";

 $old=cat('include/tv/configtv.h');
 if ($text eq $old)
   {
    print "no changes\n";
   }
 else
   {
    print "created new header\n";
    replace('include/tv/configtv.h',$text);
   }
}

sub LookForMaintainerTools
{
 my $file;

 $file=LookForFileInPath('gpr2mak*');
 if (!length($file))
   {
    die "\nRHIDE tools aren't installed. They are needed only for maintainer mode.\n".
        "Install RHIDE or disable the maintaner mode.\n\n";
   }
}

