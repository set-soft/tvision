#!/usr/bin/perl
# Copyright (C) 1996,1997,1998,1999 by Salvador E. Tropea (SET),
# see copyrigh file for details
#
# Common configuration routines.
#

$ErrorLog='errormsg.txt';
$MakeDefsRHIDE={};

sub GetCache
{
 my $val;

 if (open(FIL,'<configure.cache'))
   {
    while ($val=<FIL>)
      {
       if ($val=~/(.*) => (.*)\n/)
         {
          @conf{$1}=$2;
          #print "$1 => $2\n";
         }
       else
         {
          #print "No toma: ($val)";
         }
      }
    close(FIL);
   }
}


sub CreateCache
{
 my $i,$ff=1;

 if (open(FIL,'>configure.cache'))
   {
    foreach $i (%conf)
      {
       if ($ff)
         {
          print FIL ("$i => @conf{$i}\n");
          $ff=0;
         }
       else
         {
          $ff=1;
         }
      }
    close(FIL);
   }
}


sub CompareVersion
{
 my ($actual,$needed)=@_;
 my $vact,$vneed;

 $actual=~/(\d+)\.(\d+)(\.(\d+))?/;
 $vact=$1*1000000+$2*1000+$4;
 $needed=~/(\d+)\.(\d+)(\.(\d+))?/;
 $vneed=$1*1000000+$2*1000+$4;
 #print "$vact v.s. $vneed\n";
 $vact>=$vneed;
}


sub LookForFile
{
 my ($file,@path)=@_;
 my $i,@a,$f;

 foreach $i (@path)
   {
    #print "Testing for ".$i.'/'.$file."\n";
    @a=glob($i.'/'.$file);
    foreach $f (@a)
      {
       if (-e $f)
         {
          return $i;
         }
      }
   }
 '';
}

###[txh]####################################################################
#
# Prototype: RunGCCTest($gcc,$extension,$prog,$flags)
# Description:
#  Compiles a test program with gcc and runs it callecting the output.@*
#  You must pass the name of the compiler as argument, also the extension of
# the file (.c, .cc, etc.), the code to compile and any etra switch options
# needed to compile.@*
#  Additionally the following global variables are used: $CFLAGS: standard
# flags for the C compiler (used when extension=='c'), $CXXFLAGS: standard
# flags for the C++ compiler (used when extension=='cc'), $ErrorLog: file
# to log the actions and errors and the $OS guess.@*
#
# Return: The value returned from the program (from stdout).
#
####################################################################[txi]###

sub RunGCCTest
{
 my ($cc,$ext,$test,$switchs)=@_;
 my $file='test.'.$ext,$command,$label,$flags='';

 replace($file,$test);
 $flags=$CFLAGS if ($ext eq 'c');
 $flags=$CXXFLAGS if ($ext eq 'cc');
 $command="$cc -o test.exe $flags $file $switchs";
 #print "Running: $command\n";
 $label=$command.":\n";
 `echo $label >> $ErrorLog`;
 if ($OS eq 'dos')
   {
    `redir -ea $ErrorLog $command`;
   }
 else
   {
    `$command 2>> $ErrorLog`;
   }
 $test=`./test.exe`;
 unlink('test.o',$file,'test.exe');
 $test;
}

###[txh]####################################################################
#
# Prototype: LookForPrefix()
# Description:
#   Tries to find the right installation prefix. The following mechanisms
# are used:@*
# 1) Look for cached prefix key.@*
# 2) Look for prefix environment variable.@*
# 3) Look for PREFIX environment variable.@*
# Linux:@*
# 4) Find where make is installed and guess from it.@*
# 5) If not found default to /usr@*
# DOS:@*
# 6) Use the DJDIR environment variable.@*
#   The result is stored in the prefix configuration key.
#
####################################################################[txi]###

sub LookForPrefix
{
 my $test,$prefix;

 print 'Looking for prefix: ';
 $prefix=@conf{'prefix'};
 if ($prefix)
   {
    print "$prefix (cached/specified)\n";
    return;
   }
 $prefix=@ENV{'prefix'};
 if (!$prefix)
   {
    $prefix=@ENV{'PREFIX'};
   }
 if ($prefix)
   {
    print "$prefix (environment)\n";
    @conf{'prefix'}=$prefix;
    return;
   }
 if ($OS eq 'linux')
   {
    if (`which make`=~/(.*)\/bin\/make/)
      {
       $prefix=$1;
      }
    else
     {
      $prefix='/usr';
     }
   }
 else
   {
    $prefix=@ENV{'DJDIR'};
   }
 @conf{'prefix'}=$prefix;
 print "$prefix\n";
}


###[txh]####################################################################
#
# Prototype: CheckGCC()
# Description:
#   Tries to find a working C compiler. The following tests are done:@*
# 1) Look for cached GCC key.@*
# 2) Look for CC environment variable.@*
# 3) Default to gcc.@*
#   Then a small test program is compiled. If it fails the program dies.@*
#   The result is stored in the GCC configuration key.
#
# Return: The name of the working compiler.
#
####################################################################[txi]###

sub CheckGCC
{
 my $cc,$test;

 print 'Looking for a working gcc: ';
 $cc=@conf{'GCC'};
 if ($cc)
   {
    print "$cc (cached) OK\n";
    return $cc;
   }
 $cc=$ENV{'CC'};
 if (!length($cc))
   {
    $cc='gcc';
   }
 print "$cc ";
 $test='#include <stdio.h>
int main(void)
{
 printf("OK\n");
 return 0;
}
';
 $test=RunGCCTest($cc,'c',$test,'');
 if ($test ne "OK\n")
   {
    CreateCache();
    die 'Not working gcc found';
   }
 print "OK\n";
 $conf{'GCC'}=$cc;
 $cc;
}

###[txh]####################################################################
#
# Prototype: CheckGCCcanXX($gcc)
# Description:
#   Checks if the C compiler can be used to compile C++ code.
#
# Return: 'yes' or 'no' according to a compilation test.
#
####################################################################[txi]###

sub CheckGCCcanXX
{
 my $cc=$_[0],$ret,$test;

 print "$cc can compile C++ code: ";
 $test='#include <iostream.h>
int main(void)
{
 cout << "OK" << endl;
 return 0;
}';
 $test=RunGCCTest($cc,'cc',$test,$stdcxx);
 if ($test eq "OK\n")
   {
    print "yes\n";
    $ret=1;
   }
 else
   {
    print "no\n";
    $ret=0;
   }
 $ret;
}


###[txh]####################################################################
#
# Prototype: CheckGXX()
# Description:
#   Finds the C++ compiler. Mechanism:@*
# 1) Cached GXX key.@*
# 2) Compiler declared in global $GCC variable.@*
# 3) Return value from CheckGXXReal().@*
#   The result is stored in the GXX configuration key and the $GXX global
# variable.
#
# Return: The $GXX value.
#
####################################################################[txi]###

sub CheckGXX
{
 if (@conf{'GXX'})
   {
    print "C++ compiler: @conf{'GXX'} (cached) OK\n";
    return @conf{'GXX'};
   }
 if (CheckGCCcanXX($GCC))
   {
    $GXX=$GCC;
   }
 else
   {
    # Test for a working g++
    $GXX=CheckGXXReal();
   }
 $conf{'GXX'}=$GXX;
}

###[txh]####################################################################
#
# Prototype: CheckGXXReal()
# Description:
#   Tries to find if $defaultCXX can compile C++. This global variable is
# determined during the OS detection. If it fails the program dies.
#
# Return: The name of the compiler.
#
####################################################################[txi]###

sub CheckGXXReal
{
 my $test;

 print 'Looking for the C++ compiler: ';
 $test='#include <iostream.h>
int main(void)
{
 cout << "OK" << endl;
 return 0;
}';
 $test=RunGCCTest($defaultCXX,'cc',$test,$stdcxx);
 if ($test ne "OK\n")
   {
    CreateCache();
    die('can not find it');
   }
 print "$defaultCXX\n";
 $defaultCXX;
}

###[txh]####################################################################
#
# Prototype: LookForDJGPP($versionNeeded)
# Description:
#   Finds if the installed djgpp version is the needed. Note that the
# version should be provided in the following way: djgpp v2.01 => 2.0.1.
# I did it to accomodate to the version comparisson test. Mechanism:@*
# 1) Cached djgpp key.@*
# 2) Compilation of a small program printing DJGPP.0.DJGPP_MINOR.@*
#   If the version is incorrect the program dies.@*
#   The result is stored in the 'djgpp' configuration key.
#
####################################################################[txi]###

sub LookForDJGPP
{
 my $vNeed=$_[0];
 my $test;

 print 'Checking DJGPP version: ';
 $test=@conf{'djgpp'};
 if ($test)
   {
    print "$test (cached) OK\n";
    return;
   }
 $test='
#include <stdio.h>
int main(void)
{
 printf("%d.0.%d",DJGPP,DJGPP_MINOR);
 return 0;
}';
 $test=RunGCCTest($GCC,'c',$test,'');
 if (!CompareVersion($test,$vNeed))
   {
    print "\nWrong DJGPP version, please use $vNeed or newer\n";
    print "Look in $ErrorLog for potential compile errors of the test\n";
    CreateCache();
    die "Wrong version\n";
   }
 print "$test OK\n";
 $conf{'djgpp'}=$test;
}

###[txh]####################################################################
#
# Prototype: FindCFLAGS()
# Description:
#   Determines the flags to be used for compilation. Mechanism:@*
# 1) Cached CFLAGS key.@*
# 2) Environment variable CFLAGS.@*
# 3) -O2 -gstabs+3 (-pipe if Linux).@*
#   The result is stored in the 'CFLAGS' configuration key.
#
# Return: The value determined.
#
####################################################################[txi]###

sub FindCFLAGS
{
 my $ret;

 print 'Determining the C compilation flags: ';
 $ret=@conf{'CFLAGS'};
 if ($ret)
   {
    print "$ret (cached)\n";
    return $ret;
   }
 $ret=@ENV{'CFLAGS'};
 if (!$ret)
   {
    $ret='-O2 -gstabs+3';
    $ret.=' -pipe' unless ($OS eq 'dos');
   }
 print "$ret\n";
 $conf{'CFLAGS'}=$ret;
 $ret;
}


###[txh]####################################################################
#
# Prototype: FindCXXFLAGS()
# Description:
#   Determines the flags to be used for C++ compilation. Mechanism:@*
# 1) Cached CXXFLAGS key.@*
# 2) Environment variable CXXFLAGS.@*
# 3) Cached CFLAGS key.@*
# 4) Environment variable CFLAGS.@*
# 5) -O2 -gstabs+3 (-pipe if Linux).@*
#   The result is stored in the 'CXXFLAGS' configuration key.
#
# Return: The value determined.
#
####################################################################[txi]###

sub FindCXXFLAGS
{
 my $ret;

 print 'Determining the C++ compilation flags: ';
 $ret=@conf{'CXXFLAGS'};
 if ($ret)
   {
    print "$ret (cached)\n";
    return $ret;
   }
 $ret=@ENV{'CXXFLAGS'};
 $ret=@conf{'CFLAGS'} unless $ret;
 $ret=@ENV{'CFLAGS'} unless $ret;
 if (!$ret)
   {
    $ret='-O2 -gstabs+3';
    $ret.=' -pipe' unless ($OS eq 'dos');
   }
 print "$ret\n";
 $conf{'CXXFLAGS'}=$ret;
 $ret;
}


###[txh]####################################################################
#
# Prototype: DetectOS()
# Description:
#   Determines the OS using the uname command. If it returns MS-DOS then dos
# is assumed, if Linux or linux is returned the linux is assumed. If none
# of these is returned the program dies. The following global variables are
# filled according to the OS:@*
# $OS: dos or linux.@*
# $stdcxx: C++ library (stdcxx or stdc++).@*
# $defaultCXX: C++ compiler (gxx or g++).@*
# $supportDir: Directory to look for OS specific support (djgpp or linux).@*
#
# Return: The determined OS.
#
####################################################################[txi]###

sub DetectOS
{
 my $os,$OS;
 $os=`uname`;
 print 'Determining OS: ';
 
 if ($os=~/MS\-DOS/)
   {
    $OS='dos';
    $stdcxx='-lstdcxx';
    $defaultCXX='gxx';
    $supportDir='djgpp';
   }
 else
   {
    if ($os=~/[Ll]inux/)
      {
       $OS='linux';
       $stdcxx='-lstdc++';
       $defaultCXX='g++';
       $supportDir='linux';
      }
    else
      {
       die('Unknown OS, you must do things by yourself');
      }
   }
 print "$OS\n";
 $OS;
}


###[txh]####################################################################
#
# Prototype: ModifyMakefiles(@list)
# Description:
#   Patches the listed RHIDE makefiles to use the detected C compiler, C++
# compiler and compilation flags.@*
#
####################################################################[txi]###

sub ModifyMakefiles
{
 my $a,$text,$rep;

 print 'Configuring makefiles: ';
 foreach $a (@_)
   {
    print "$a ";
    $text=cat($a);
    if ($text)
      {
       $text=~s/RHIDE_GCC=(.*)\n/RHIDE_GCC=$GCC\n/;
       $text=~s/RHIDE_GXX=(.*)\n/RHIDE_GXX=$GXX\n/;
       $text=~s/RHIDE_LD=(.*)\n/RHIDE_LD=$GXX\n/;
       $text=~s/RHIDE_OS_CFLAGS=(.*)\n/RHIDE_OS_CFLAGS=$CFLAGS\n/;
       $text=~s/RHIDE_OS_CXXFLAGS=(.*)\n/RHIDE_OS_CXXFLAGS=$CXXFLAGS\n/;
       #foreach $rep (MakeDefsRHIDE)
       #  {
       #   $text=~s/$rep[0]\n/$rep[1]\n/;
       #  }
       replace($a,$text);
      }
   }
 print "\n";
}


###[txh]####################################################################
#
# Prototype: CreateRHIDEenvs(@list)
# Description:
#   Generates all the needed rhide.env files to configure RHIDE with the
# detected C compiler, C++ compiler and compilation flags.@*
#   That's needed to use RHIDE after configuring.
#
####################################################################[txi]###

sub CreateRHIDEenvs
{
 my $a,$text,$rep;

 print 'Configuring RHIDE: ';
 foreach $a (@_)
   {
    print "$a ";
    $text ="#\n# Automatically generated by configure script\n#\n\n";
    $text.="RHIDE_GCC=$GCC\n" unless ($GCC eq 'gcc');
    $text.="RHIDE_GXX=$GXX\n" unless ($GXX eq 'gcc');
    $text.="RHIDE_LD=$GXX\n"  unless ($GXX eq 'gcc');
    $text.="RHIDE_OS_CFLAGS=$CFLAGS\n";
    $text.="RHIDE_OS_CXXFLAGS=$CXXFLAGS\n";
    foreach $rep (@MakeDefsRHIDE)
      {
       $text.="$rep\n";
      }

    if ($text)
      {
       replace($a,$text);
      }
    else
      {
       unlink $a;
      }
   }
 print "\n";
}


###[txh]####################################################################
#
# Prototype: ConfigIncDef($defVar,$defComment)
# Description:
#   That's used to generate the config.h file and generates a line with
# /* $defComment */\n#define $defVar or not according to the existence of
# the $defVar configure key.
#
# Return: The string generated.
#
####################################################################[txi]###

sub ConfigIncDef
{
 my $text;

 $text ="\n\n/* $_[1] */\n";
 $text.='/*' unless (@conf{$_[0]});
 $text.="#define $_[0]";
 $text.='*/' unless (@conf{$_[0]});
 $text;
}

###[txh]####################################################################
#
# Prototype: ConfigIncDefYes($defVar,$defComment)
# Description:
#   That's similar to ConfigIncDef, but here the key must be 'yes'.
#
# Return: The string generated.
#
####################################################################[txi]###

sub ConfigIncDefYes
{
 my $text;

 $text ="\n\n/* $_[1] */\n";
 $text.='/*' unless (@conf{$_[0]} eq 'yes');
 $text.="#define $_[0]";
 $text.='*/' unless (@conf{$_[0]} eq 'yes');
 $text;
}

sub ParentDir
{
 my $cur,$parent;
 $cur=`pwd`;
 chop $cur;
 chdir('..');
 $parent=`pwd`;
 chop $parent;
 chdir($cur);
 $parent;
}

1;
