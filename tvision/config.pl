#!/usr/bin/perl
# Copyright (C) 1999,2000 by Salvador E. Tropea (SET),
# see copyrigh file for details
#
# To specify the compilation flags define the CFLAGS environment variable.
#

require "miscperl.pl";
require "conflib.pl";

GetCache();
GetVersion('');

# I never tested with an older version, you can try reducing it.
$GPMVersionNeeded='1.10';
# I never tested with an older version, you can try reducing it.
#$NCursesVersionNeeded='1.9.9';
# That's a test to see if that works:
$NCursesVersionNeeded='1.8.6';
# Adds some nice stuff to define key sequences.
$NCursesVersionRecomended='4.2';
$DJGPPVersionNeeded='2.0.2';
unlink $ErrorLog;

SeeCommandLine();

print "Configuring Turbo Vision v$Version library\n\n";
# Determine the OS
$OS=DetectOS();
LookForPrefix();
# Determine C flags
$CFLAGS=FindCFLAGS();
# Determine C++ flags
$CXXFLAGS=FindCXXFLAGS();
# Test for a working gcc
$GCC=CheckGCC();
# Check if gcc can compile C++
$GXX=CheckGXX();
# Is the right djgpp?
if ($OS eq 'DOS')
  {
   LookForDJGPP($DJGPPVersionNeeded);
  }

if ($OS eq 'UNIX')
  {
   LookForGPM($GPMVersionNeeded);
   LookForNCurses($NCursesVersionNeeded,$NCursesVersionRecomended);
   LookForKeysyms();
   LookForOutB();
  }
LookForIntlSupport();
LookForEndianess();

print "\n";
GenerateMakefile();
if ($OS eq 'UNIX')
  {
   $MakeDefsRHIDE[0]='RHIDE_STDINC=/usr/include /usr/local/include /usr/include/g++ /usr/local/include/g++ /usr/lib/gcc-lib /usr/local/lib/gcc-lib';
   ModifyMakefiles('linux/Makefile');
   CreateRHIDEenvs('linux/rhide.env','examples/config.env','compat/rhide.env');
  }
elsif ($OS eq 'DOS')
  {
   $MakeDefsRHIDE[0]='RHIDE_STDINC=$(DJDIR)/include $(DJDIR)/lang/cxx $(DJDIR)/lib/gcc-lib';
   ModifyMakefiles('djgpp/makefile');
   CreateRHIDEenvs('djgpp/rhide.env','examples/config.env','compat/rhide.env');
  }
elsif ($OS eq 'Win32')
  {
   $ExtraModifyMakefiles{'vpath_src'}="../classes/win32 ../stream ../names ../classes .. ../djgpp\nvpath %.h ../djgpp";
   `cp djgpp/makefile win32/Makefile`;
   ModifyMakefiles('win32/Makefile');
  }
CreateConfigH();

# Generate the BC++ makefile
# 1) Get the list of files used by djgpp version
$col=14;
$a=ExtractItemsMak('djgpp/makefile',$col);
# 2) Remove djgpp specific things
$a=~s/vga.cc//;
$a=~s/vgaregs.c//;
$a=~s/vgastate.c//;
# 3) Add BC++ specific
$a.=' gkeyw32.cc';
$a=~s/(\w+)\.(\w+)/\+$1\.obj/g;
$a=~s/\t//g;
# 4) Generate makefile from the template
$ReplaceTags{'TV_OBJS_BCC'}=$a;
ReplaceText('winnt/bccmake.in','winnt/Makefile');
$a='';

$ReplaceTags{'LIB_GPM_SWITCH'}=@conf{'HAVE_GPM'} eq 'yes' ? '-lgpm' : '';
ReplaceText('linuxso/makemak.in','linuxso/makemak.pl');
chmod(0755,'linuxso/makemak.pl');


print "\nSuccesful configuration!\n\n";

GiveAdvice();
CreateCache();
unlink $ErrorLog;

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
    elsif ($i eq '--no-intl')
      {
       $conf{'no-intl'}='yes';
      }
#    elsif ($i=~'--cflags=(.*)')
#      {
#       @conf{'CFLAGS'}=$1;
#      }
    elsif ($i eq '--fhs')
      {
       $conf{'fhs'}='yes';
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
 print "--help         : displays this text.\n";
 print "--prefix=path  : defines the base directory for installation.\n";
 print "--no-intl      : don't use international support.\n";
 print "--fhs          : force the FHS layout under UNIX.\n";
}

sub GiveAdvice
{
 if ((@conf{'intl'} eq 'no') && (@conf{'no-intl'} ne 'yes'))
   {
    print "\n";
    print "* The international support was disabled because gettext library could not\n";
    print "  be detected.\n";
    if ($OSflavor eq 'Linux')
      {
       print "  Starting with glibc 2.0 this is included in libc, perhaps your system\n";
       print "  just lacks the propper header file.\n";
      }
    elsif ($OS eq 'DOS')
      {
       print "  Install the gtxtNNNb.zip package from the v2gnu directory of djgpp's\n";
       print "  distribution. Read the readme file for more information.\n";
      }
    elsif ($OS eq 'Win32')
      {
       print "  That's normal for Win32.\n";
      }
   }
 if ((@conf{'HAVE_GPM'} eq 'no') && ($OSflavor eq 'Linux'))
   {
    print "\n";
    print "* No mouse support for console! please install the libgpm package needed\n";
    print "  for development. (i.e. libgpmg1-dev_1.13-5.deb).\n";
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
}';
 $test=RunGCCTest($GCC,'c',$test,'');
 chop($test);
 $conf{'TV_BIG_ENDIAN'}=($test eq "big") ? 'yes' : 'no';
 print "$test endian\n";
}

sub LookForIntlSupport
{
 my $vNeed=$_[0],$test,$a,$djdir;

 print 'Checking for international support: ';
 if (@conf{'no-intl'} eq 'yes')
   {
    print "disabled by user request.\n";
    $conf{'intl'}='no';
    `cp include/tv/nointl.h include/tv/intl.h`;
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
 $test='
#include <stdio.h>
#define FORCE_INTL_SUPPORT
#include <tv/intl.h>
int main(void)
{
 printf("%s\n",_("OK"));
 return 0;
}';
 $test=RunGCCTest($GCC,'c',$test,'-Iinclude/ '.($OS eq 'DOS' ? '-lintl' : ''));
 if ($test ne "OK\n")
   {
    print "not available or not usable, disabling.\n";
    $conf{'intl'}='no';
   }
 else
   {
    print "yes OK\n";
    $conf{'intl'}='yes';
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
}';
 $test=RunGCCTest($GCC,'c',$test,'');
 if ($test eq "OK\n")
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
static volatile void Test(void) { outb(0x300,10); }
#endif
int main(void)
{
 printf("OK\n");
 return 0;
}';
 $test=RunGCCTest($GCC,'c',$test,'');
 $conf{'HAVE_OUTB_IN_SYS'}=($test eq "OK\n") ? 'yes' : 'no';
 print "@conf{'HAVE_OUTB_IN_SYS'}\n";
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
}';
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
 my ($vNeed,$vReco)=@_,$test;

 print 'Looking for ncurses library: ';
 if (@conf{'ncurses'})
   {
    print "@conf{'ncurses'} (cached) OK\n";
    return;
   }
 $test='
#include <stdio.h>
#include <ncurses.h>
void dummy() {initscr();}
int main(void)
{
 printf(NCURSES_VERSION);
 return 0;
}';
 $test=RunGCCTest($GCC,'c',$test,'-lncurses');
 if (!length($test))
   {
    print "\nError: ncurses library not found, please install ncurses $vNeed or newer\n";
    print "Look in $ErrorLog for potential compile errors of the test\n";
    CreateCache();
    die "Missing library\n";
   }
 if (!CompareVersion($test,$vNeed))
   {
    print "$test, too old\n";
    print "Please upgrade your ncurses library to version $vNeed or newer.\n";
    print "You can try with $test forcing the configure scripts.\n";
    CreateCache();
    die "Old library\n";
   }
 print "$test OK\n";
 @conf{'ncurses'}=$test;
 if (!CompareVersion($test,$vReco))
   {
    print "Warning: $vReco version is recommended\n";
    $conf{'HAVE_DEFINE_KEY'}=0;
   }
 else
   {
    $conf{'HAVE_DEFINE_KEY'}=1;
   }
}

sub GenerateMakefile
{
 my $text,$rep,$makeDir;

 print "Generating Makefile\n";
 $text=cat('Makefile.in');
 if (!$text)
   {
    CreateCache();
    die "Can't find Makefile.in!!\n";
   }
 $rep='static-lib';
 $rep.=' dynamic-lib' if ($OS eq 'UNIX');
 $text=~s/\@targets\@/$rep/g;
 $text=~s/\@OS\@/$OS/g;
 $text=~s/\@prefix\@/@conf{'prefix'}/g;

 $makeDir='linux' if ($OS eq 'UNIX');
 $makeDir='djgpp' if ($OS eq 'DOS');
 $makeDir='win32' if ($OS eq 'Win32');
 # Write target rules:
 #$rep="static-lib: $makeDir/librhtv.a\n$makeDir/librhtv.a:\n\t\$(MAKE) -C ".$makeDir;
 $rep="static-lib:\n\t\$(MAKE) -C ".$makeDir;
 $text=~s/\@target1_rule\@/$rep/g;
 if ($OS eq 'UNIX')
   {
    #$rep="linuxso/librhtv.so.$Version";
    #$rep="dynamic-lib: $rep\n$rep:\n\tcd linuxso; ./makemak.pl --no-inst-message";
    $rep="dynamic-lib:\n\tcd linuxso; ./makemak.pl --no-inst-message";
    $text=~s/\@target2_rule\@/$rep/g;
   }
 else
   {
    $text=~s/\@target2_rule\@//g;
   }

 # Write install stuff
 $rep= "install -d -m 0755 \$(prefix)/include/rhtvision\n";
 $rep.="\trm -f \$(prefix)/include/rhtvision/*.h\n";
 $rep.="\tinstall -m 0644 include/*.h \$(prefix)/include/rhtvision\n";
 $rep.="\tinstall -d -m 0755 \$(prefix)/include/rhtvision/tv\n";
 $rep.="\tinstall -m 0644 include/tv/*.h \$(prefix)/include/rhtvision/tv\n";
 # This should be created if the target is a new directory
 $rep.="\tinstall -d -m 0755 \$(libdir)\n";
 $rep.="\tinstall -m 0644 $makeDir/librhtv.a \$(libdir)\n";
 if ($OS eq 'UNIX')
   {
    $rep.="\trm -f \$(libdir)/librhtv.so\n";
    $rep.="\trm -f \$(libdir)/librhtv.so.1\n";
    $rep.="\trm -f \$(libdir)/librhtv.so.$Version\n";
    $rep.="\tcd \$(libdir); ln -s librhtv.so.$Version librhtv.so\n";
    # Not needed if the soname changes which each version (at least Ivan says that)
    #$rep.="\tcd \$(libdir); ln -s librhtv.so.$Version librhtv.so.1\n";
    $rep.="\tinstall -m 0644 linuxso/librhtv.so.$Version \$(libdir)\n";
    $rep.="\tstrip --strip-debug \$(libdir)/librhtv.so.$Version\n";
    $rep.="\tldconfig\n";
   }
 $text=~s/\@install_rule\@/$rep/g;

 replace('Makefile',$text);
}

sub CreateConfigH
{
 my $text="/* Generated automatically by the configure script */",$old;

 print "Generating configuration header\n";

 $text.=ConfigIncDef('HAVE_DEFINE_KEY','ncurses 4.2 or better have define_key');
 $text.=ConfigIncDefYes('HAVE_KEYSYMS','The X11 keysyms are there');
 $conf{'HAVE_INTL_SUPPORT'}=@conf{'intl'};
 $text.=ConfigIncDefYes('HAVE_INTL_SUPPORT','International support with gettext');
 $text.=ConfigIncDefYes('HAVE_GPM','GPM mouse support');
 $text.=ConfigIncDefYes('HAVE_OUTB_IN_SYS','out/in functions defined by glibc');
 $text.=ConfigIncDefYes('TV_BIG_ENDIAN','Byte order for this machine');
 $text.="\n\n";
 $text.="#define TVOS_$OS\n#define TVOSf_$OSflavor\n";

 $old=cat('include/tv/configtv.h');
 replace('include/tv/configtv.h',$text) unless $text eq $old;
}

