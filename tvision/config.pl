#!/usr/bin/perl
# Copyright (C) 1996,1997,1998,1999 by Salvador E. Tropea (SET),
# see copyrigh file for details
#
# To specify the compilation flags define the CFLAGS environment variable.
#

require "miscperl.pl";
require "conflib.pl";

GetCache();
GetVersion('');

# I never tested with an older version, you can try reducing it.
$GPMVersionNeeded='1.13';
# I never tested with an older version, you can try reducing it.
$NCursesVersionNeeded='1.9.9';
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
if ($OS eq 'dos')
  {
   LookForDJGPP($DJGPPVersionNeeded);
  }

if ($OS eq 'linux')
  {
   LookForGPM($GPMVersionNeeded);
   LookForNCurses($NCursesVersionNeeded,$NCursesVersionRecomended);
   LookForKeysyms();
  }
LookForIntlSupport();

print "\n";
GenerateMakefile();
ModifyMakefiles('linux/Makefile','djgpp/makefile');
CreateRHIDEenvs('linux/rhide.env','djgpp/rhide.env','examples/config.env');
CreateConfigH();

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
    else {
    if ($i=~'--prefix=(.*)')
      {
       $conf{'prefix'}=$1;
      }
    else {
    if ($i eq '--no-intl')
      {
       $conf{'no-intl'}='yes';
      }
    else
#    if ($i=~'--cflags=(.*)')
#      {
#       @conf{'CFLAGS'}=$1;
#      }
#    else
      {
       ShowHelp();
       die "Unknown option: $i\n";
      }}}
   }
}

sub ShowHelp
{
 print "Available options:\n\n";
 print "--help         : displays this text.\n";
 print "--prefix=path  : defines the base directory for installation.\n";
 print "--no-intl      : don't use international support.\n";
}

sub GiveAdvice
{
 if ((@conf{'intl'} eq 'no') && (@conf{'no-intl'} ne 'yes'))
   {
    print "\n";
    print "* The international support was disabled because gettext library could not\n";
    print "  be detected.\n";
    if ($OS eq 'linux')
      {
       print "  Starting with glibc 2.0 this is included in libc, perhaps your system\n";
       print "  just lacks the propper header file.\n";
      }
    else
      {
       print "  Install the gtxtNNNb.zip package from the v2gnu directory of djgpp's\n";
       print "  distribution. Read the readme file for more information.\n";
      }
   }
}

sub LookForIntlSupport
{
 my $vNeed=$_[0],$test;

 print 'Checking for international support: ';
 if (@conf{'no-intl'} eq 'yes')
   {
    print "disabled by user request.\n";
    $conf{'intl'}='no';
    `cp include/nointl.h include/intl.h`;
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
 $test='
#include <stdio.h>
#define FORCE_INTL_SUPPORT
#include <intl.h>
int main(void)
{
 printf("%s\n",_("OK"));
 return 0;
}';
 $test=RunGCCTest($GCC,'c',$test,'-Iinclude/ '.($OS eq 'dos' ? '-lintl' : ''));
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
    print "\nError: gpm library not found, please install gpm $vNeed or newer\n";
    print "Look in $ErrorLog for potential compile errors of the test\n";
    CreateCache();
    die "Missing library\n";
   }
 if (!CompareVersion($test,$vNeed))
   {
    print "$test, too old\n";
    print "Please upgrade your gpm library to version $vNeed or newer.\n";
    print "You can try with $test forcing the configure scripts.\n";
    CreateCache();
    die "Old library\n";
   }
 @conf{'gpm'}=$test;
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
 $test=RunGCCTest($GCC,'c',$test,'-lgpm');
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
 $rep.=' dynamic-lib' if ($OS eq 'linux');
 $text=~s/\@targets\@/$rep/g;
 $text=~s/\@OS\@/$OS/g;
 $text=~s/\@prefix\@/@conf{'prefix'}/g;

 $makeDir=$OS eq 'linux' ? 'linux' : 'djgpp';
 # Write target rules:
 $rep="static-lib: $makeDir/librhtv.a\n$makeDir/librhtv.a:\n\t\$(MAKE) -C ".$makeDir;
 $text=~s/\@target1_rule\@/$rep/g;
 if ($OS eq 'linux')
   {
    $rep="linuxso/librhtv.so.$Version";
    $rep="dynamic-lib: $rep\n$rep:\n\tcd linuxso; ./makemak.pl --no-inst-message";
    $text=~s/\@target2_rule\@/$rep/g;
   }
 else
   {
    $text=~s/\@target2_rule\@//g;
   }

 # Write install stuff
 $rep= "install -d -m 0755 \$(prefix)/include/rhtvision\n";
 $rep.="\tinstall -m 0644 include/*.h \$(prefix)/include/rhtvision\n";
 $rep.="\tinstall -m 0755 $makeDir/librhtv.a \$(libdir)\n";
 if ($OS eq 'linux')
   {
    $rep.="\trm -f \$(libdir)/librhtv.so\n";
    $rep.="\trm -f \$(libdir)/librhtv.so.1\n";
    $rep.="\trm -f \$(libdir)/librhtv.so.$Version\n";
    $rep.="\tln -s \$(libdir)/librhtv.so.$Version \$(libdir)/librhtv.so\n";
    $rep.="\tln -s \$(libdir)/librhtv.so.$Version \$(libdir)/librhtv.so.1\n";
    $rep.="\tinstall -m 0755 linuxso/librhtv.so.$Version \$(libdir)\n";
    $rep.="\tstrip --strip-debug \$(libdir)/librhtv.so.$Version\n";
    $rep.="\tldconfig\n";
   }
 $text=~s/\@install_rule\@/$rep/g;

 replace('Makefile',$text);
}

sub CreateConfigH
{
 my $text="/* Generated automatically by the configure script */";

 print "Generating configuration header\n";

 $text.=ConfigIncDef('HAVE_DEFINE_KEY','ncurses 4.2 or better have define_key');
 $text.=ConfigIncDefYes('HAVE_KEYSYMS','The X11 keysyms are there');
 $conf{'HAVE_INTL_SUPPORT'}=@conf{'intl'};
 $text.=ConfigIncDefYes('HAVE_INTL_SUPPORT','International support with gettext');
 $text.="\n";

 replace('include/configtv.h',$text);
}

