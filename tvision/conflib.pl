#!/usr/bin/perl
# Copyright (C) 1999-2004 by Salvador E. Tropea (SET),
# see copyrigh file for details
#
# Common configuration routines.
#

$ErrorLog='errormsg.txt';
$MakeDefsRHIDE={};
$ExtraModifyMakefiles={};
# DOS, UNIX, Win32
$OS='';
# Linux, FreeBSD, NetBSD, Solaris, QNXRtP, QNX4
$OSf='';
# x86, Alpha, SPARC64, SPARC, PPC, HPPA, MIPS, Itanium, Unknown
$CPU='';
# GCC, BCPP, MSVC
$Comp='';
# djgpp, MinGW, Cygwin
$Compf='';
# gcc's -pipe option
$UsePipe=0;

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
 my ($i,$ff);

 $ff=1;
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
 my ($vact,$vneed);

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
 my ($i,@a,$f);

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

sub LookForFileInPath
{
 my ($file)=@_;
 my @list;

 @list=(($OS eq 'UNIX') || ($Compf eq 'Cygwin')) ? split(/:/,@ENV{'PATH'}) : split(/;/,@ENV{'PATH'});
 return LookForFile($file,@list);
}

###[txh]####################################################################
#
# Prototype: RunRedirect($command,$ErrorLog)
# Description:
#  Executes the desired command sending the stderr to $ErrorLog file
# (appending) and returning the stdout.
#
# Return: The value returned from the program (from stdout).
#
####################################################################[txi]###

sub RunRedirect
{
 my ($command,$ErrorLog)=@_;
 my $ret;

 if ($OS eq 'DOS')
   {
    $ret=`redir -ea $ErrorLog $command`;
   }
 elsif ($OS eq 'Win32')
   {
    $ret=`sh -c "$command 2>> $ErrorLog"`;
   }
 else
   {
    $ret=`$command 2>> $ErrorLog`;
   }
 $ret;
}

sub AddIncludes
{
 my (@dirs)=split(' ',$conf{'EXTRA_INCLUDE_DIRS'});
 my ($res)='';
 for (@dirs)
    {
     $res.=" -I$_";
    }
 return $res;
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
 my ($command,$label);
 my ($file,$flags)=('test.'.$ext,'');

 unlink 'test.exe';
 replace($file,$test."\n");
 $flags=$CFLAGS if ($ext eq 'c');
 $flags=$CXXFLAGS if ($ext eq 'cc');
 $flags.=AddIncludes();
 $command="$cc -o test.exe $flags $file $switchs";
 #print "Running: $command\n";
 $label=$command.":\n";
 `echo $label >> $ErrorLog`;

 RunRedirect($command,$ErrorLog);
 $test=RunRedirect('./test.exe',$ErrorLog);
 #print "\n$command: $test";
 # Does anybody know why I can't delete the exe here?
 # In Linux it makes the variable $test to change!
 #unlink('test.o',$file,'test.exe');
 unlink('test.o',$file);
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
# UNIX:@*
# 4) Find where make is installed and guess from it.@*
# 5) If not found default to /usr@*
# DOS:@*
# 6) Use the DJDIR environment variable.@*
#   The result is stored in the prefix configuration key.
# WIN32:@*
# 7) Locate make.exe in PATH and use parent directory for prefix. If the
# prefix string contains backslashes, convert them to slashes.
# make.exe must reside in $prefix/bin directory
#
####################################################################[txi]###

sub LookForPrefix
{
 my ($test,$prefix,@lista,$i,$found);

 print 'Looking for prefix: ';
 $prefix=@conf{'prefix'};
 if ($prefix)
   {
    print "$prefix (cached/specified)\n";
    LookIfFHS();
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
    LookIfFHS();
    return;
   }
 if (($OS eq 'UNIX') || ($Compf eq 'Cygwin'))
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
 elsif ($OS eq 'DOS')
   {
    $prefix=@ENV{'DJDIR'};
   }
 else
   {
    if ($Compf eq 'MinGW')
      {
       @lista=(split(/;/,@ENV{'PATH'}),split(/:/,@ENV{'PATH'}));
       $found=0;
       foreach $i (@lista)
         {
          #print "Buscando ".$i."\\make.exe\n";
          if (!$found && (-e $i."\\make.exe") &&
              ($i=~/(.*)[\\\/][bB][iI][nN]/))
            {
             $prefix=$1;
             $prefix=~s/\\/\//g;
             $found=1;
            }
         }
      }
    if (!$found)
      {
       if (!$test)
         {
          die "Some critical tools aren't installed please read the reame files";
         }
       else
         {
          print "Unable to determine installation prefix, please use --prefix option.\n";
          print "You'll most probably need to quote the --prefix option (\"--prefix=path\").\n" if @ENV{'OS'} eq 'Windows_NT';
          die;
         }
      }
   }
 @conf{'prefix'}=$prefix;
 print "$prefix\n";
 LookIfFHS();
}

sub LookIfFHS
{
 if ($OS eq 'UNIX')
   {
    print 'Checking if this system follows the FHS: ';
    if ($conf{'fhs'})
      {
       print "$conf{'fhs'} (cached/specified)\n";
       return;
      }
    # FreeBSD uses share/doc but not shared/info and shared/man
    if ((-d $prefix.'/share/doc') && (-d $prefix.'/share/info') && (-d $prefix.'/share/man'))
      {
       $conf{'fhs'}='yes';
      }
    else
      {
       $conf{'fhs'}='no';
      }
    print "$conf{'fhs'}\n";
   }
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
 my ($cc,$test);

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
 $test=~s/\W//g;
 if ($test ne "OK")
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
 my $cc=$_[0];
 my ($ret,$test);

 print "$cc can compile C++ code: ";
 $test='
#if __GNUC__>=3
 #include <iostream>
 #define STD(a) std::a
#else
 #include <iostream.h>
 #define STD(a) a
#endif
int main(void)
{
 STD(cout) << "OK" << STD(endl);
 return 0;
}';
 $test=RunGCCTest($cc,'cc',$test,$stdcxx);
 $test=~s/\W//g;
 if ($test eq "OK")
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
 # Test for a working g++
 $GXX=CheckGXXReal();

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
 my ($test,$res,@list,$i,$cxx);

 print 'Looking for the C++ compiler: ';
 $test='
#if __GNUC__>=3
 #include <iostream>
 #define STD(a) std::a
#else
 #include <iostream.h>
 #define STD(a) a
#endif
int main(void)
{
 STD(cout) << "OK" << STD(endl);
 return 0;
}';
 @list=split(/:/,$defaultCXX);
 $cxx=$ENV{'CXX'};
 if (length($cxx))
   {
    unshift @list,$cxx;
   }
 foreach $i (@list)
   {
    $res=RunGCCTest($i,'cc',$test,$stdcxx);
    $res=~s/\W//g;
    if ($res eq "OK")
      {
       print "$i\n";
       return $i;
      }
   }
 return $GCC if (CheckGCCcanXX($GCC));
 CreateCache();
 die('can not find it');
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
}
';
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
# 3) -O2 -gstabs+3 (-pipe if UNIX).@*
#   The result is stored in the 'CFLAGS' configuration key.
#
# Return: The value determined.
#
####################################################################[txi]###

sub FindCFLAGS
{
 my ($ret,$ver);

 print 'C flags: ';
 $ret=@conf{'CFLAGS'};
 if ($ret)
   {
    print "$ret (cached)\n";
    return $ret;
   }
 $ret=@ENV{'CFLAGS'};
 if (!$ret)
   {
    $ret='-O2'; # -gstabs+3';
    # In UNIX pipes are in memory and allows multithreading so they are
    # usually faster. In Linux that's faster.
    $ret.=' -pipe' if $UsePipe;
    # Looks like that's common and some sysadmins doesn't configure gcc to
    # look there:
    $conf{'EXTRA_INCLUDE_DIRS'}.=' /usr/local/include' if ($OSf eq 'FreeBSD');
    # Darwin is using a temporal size
    $ret.=' -Wno-long-double' if ($OSf eq 'Darwin');
    # QNX4 X11 is in /usr/X11R6
    # This should be automatic now. (EXTRA_INCLUDE_DIRS).
    #$ret.=' -I/usr/X11R6/include' if ($OSf eq 'QNX4');
    $ver=RunRedirect("$GCC -dumpversion",$ErrorLog);
    $ret.=' -Wno-packed' if $ver>=4;
   }
 print "$ret\n";
 $conf{'CFLAGS'}=$ret;
 $ret;
}

###[txh]####################################################################
#
# Prototype: FindLDExtraDirs()
# Description:
#  Simple hook to get the extra dirs.
#
# Return: 
#  Space separated list of directories.
#
####################################################################[txi]###

sub FindLDExtraDirs()
{
 my $ret;

 $ret=@conf{'LDExtraDirs'};
 if ($ret)
   {
    return $ret;
   }
 $ret='';
 $ret.='/usr/local/lib' if ($OSf eq 'FreeBSD');
 $ret.='/usr/pkg/lib'   if ($OSf eq 'NetBSD');
 $conf{'LDExtraDirs'}=$ret;
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
# 5) -O2 -gstabs+3 (-pipe if UNIX).@*
#   The result is stored in the 'CXXFLAGS' configuration key.
#
# Return: The value determined.
#
####################################################################[txi]###

sub FindCXXFLAGS
{
 my $ret;

 print 'C++ flags: ';
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
    $ret='-O2'; # -gstabs+3';
    $ret.=' -pipe' if $UsePipe;
    $ret.=' -L/usr/local/include' if ($OSf eq 'FreeBSD');
    $conf{'EXTRA_INCLUDE_DIRS'}.=' /usr/local/include' if ($OSf eq 'FreeBSD');
    $ret.=' -Wno-long-double' if ($OSf eq 'Darwin');
    # QNX4 X11 is in /usr/X11R6
    # This should be automatic now. (EXTRA_INCLUDE_DIRS).
    #$ret.=' -I/usr/X11R6/include' if ($OSf eq 'QNX4');
   }
 print "$ret\n";
 $conf{'CXXFLAGS'}=$ret;
 $ret;
}


###[txh]####################################################################
#
# Prototype: FindXCFLAGS()
# Description:
#   Determines the flags to be used for special compilation. Mechanism:@*
# 1) Cached XCFLAGS key.@*
# 2) -O3 -fomit-frame-pointer -ffast-math (-pipe if not DOS).@*
#   The result is stored in the 'XCFLAGS' configuration key.
#
# Return: The value determined.
#
####################################################################[txi]###

sub FindXCFLAGS
{
 my $ret;

 print 'Special C flags: ';
 $ret=@conf{'XCFLAGS'};
 if ($ret)
   {
    print "$ret (cached/cmdline)\n";
    return $ret;
   }
 if (!$ret)
   {
    $ret='-O3 -fomit-frame-pointer -ffast-math';
    $ret.=' -pipe' if $UsePipe;
    $ret.=' -Wno-long-double' if ($OSf eq 'Darwin');
   }
 print "$ret\n";
 $conf{'XCFLAGS'}=$ret;
 $ret;
}


###[txh]####################################################################
#
# Prototype: FindXCXXFLAGS()
# Description:
#   Determines the flags to be used for C++ compilation. Mechanism:@*
# 1) Cached XCXXFLAGS key.@*
# 2) -O3 -fomit-frame-pointer -ffast-math (-pipe if not DOS).@*
#   The result is stored in the 'XCXXFLAGS' configuration key.
#
# Return: The value determined.
#
####################################################################[txi]###

sub FindXCXXFLAGS
{
 my $ret;

 print 'Special C++ flags: ';
 $ret=@conf{'XCXXFLAGS'};
 if ($ret)
   {
    print "$ret (cached/cmdline)\n";
    return $ret;
   }
 if (!$ret)
   {
    $ret='-O3 -fomit-frame-pointer -ffast-math';
    $ret.=' -pipe' if $UsePipe;
    $ret.=' -Wno-long-double' if ($OSf eq 'Darwin');
   }
 print "$ret\n";
 $conf{'XCXXFLAGS'}=$ret;
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
# $OS: DOS, UNIX, Win32.@*
# $OSf: OS flavor i.e. Linux.@*
# $Comp: Compiler i.e. GCC.@*
# $Compf: Compiler flavor i.e. djgpp.@*
# $stdcxx: C++ library (stdcxx or stdc++).@*
# $defaultCXX: C++ compiler (gxx or g++).@*
# $supportDir: Directory to look for OS specific support (djgpp or linux).@*
#
# Return: The determined OS.
#
####################################################################[txi]###

sub DetectOS
{
 my ($os,$release,$OS);
 $os=`uname`;
 if (!$os)
   {
    print "\n";
    print "* -------------------------------------- *\n";
    print " Please install the `uname' command.\n";
    print " DJGPP users: That's part of shell utils.\n";
    print "* -------------------------------------- *\n\n";
    die;
   }
 print 'Determining OS: ';

 $OSpr=0;
 # Currently the configuration is oriented for GNU systems
 # other systems should have a special configuration header
 # created by hand.
 $Comp='GCC';
 $ExeExt='';
 if ($os=~/MS\-DOS/)
   {
    $OS='DOS';
    $OSf='';
    $Compf='djgpp';
    $stdcxx='-lstdcxx';
    $defaultCXX='gpp:gxx';
    $supportDir='djgpp';
    $ExeExt='.exe';
   }
 elsif ($os=~/[Ll]inux/)
   {
    $OS='UNIX';
    $OSf='Linux';
    $Compf='';
    $stdcxx='-lstdc++';
    $defaultCXX='g++';
    $supportDir='linux';
   }
 elsif ($os=~/FreeBSD/)
   {
    $OS='UNIX';
    $OSf='FreeBSD';
    $Compf='';
    $stdcxx='-lstdc++';
    $defaultCXX='g++';
    $supportDir='linux';
   }
 elsif ($os=~/CYGWIN/)
   {
    $OS='Win32';
    $OSf='';
    $Compf='Cygwin/MinGW';
    $stdcxx='-lstdc++';
    $defaultCXX='g++';
    $supportDir='win32';
    $ExeExt='.exe';
   }
 elsif ($os=~/MINGW/)
   {
    $OS='Win32';
    $OSf='';
    $Compf='MinGW';
    $stdcxx='-lstdc++';
    $defaultCXX='g++';
    $supportDir='win32';
    $ExeExt='.exe';
   }
 elsif ($os=~/SunOS/)
   {
    $OS='UNIX';
    $OSf='Solaris';
    $Compf='';
    $stdcxx='-lstdc++';
    $defaultCXX='g++';
    $supportDir='linux';
   }
 elsif ($os=~/QNX/)
   {
    $release=`uname -r`;
    if ($release =~ /^6/)
      {
       $OS='UNIX';
       $OSf='QNXRtP';
       $Compf='';
       $stdcxx='-lstdc++';
       $defaultCXX='qcc -Y_gpp';
       $supportDir='linux';
      }
      else
      {
       $OS='UNIX';
       $OSf='QNX4';
       $Compf='';
       $stdcxx='-lstdc++';
       $defaultCXX='g++';
       $supportDir='linux';
      }
   }
 elsif ($os=~/HP-UX/)
   {
    $OS='UNIX';
    $OSf='HP-UX';
    $Compf='';
    $stdcxx='-lstdc++';
    $defaultCXX='g++';
    $supportDir='linux';
   }
 elsif ($os=~/Darwin/)
   {
    $OS='UNIX';
    $OSf='Darwin';
    $Compf='';
    $stdcxx='-lstdc++';
    $defaultCXX='c++';
    $supportDir='linux';
    $conf{'GCC'}='cc';
   }
 elsif ($os=~/NetBSD/)
   {
    $OS='UNIX';
    $OSf='NetBSD';
    $Compf='';
    $stdcxx='-lstdc++';
    $defaultCXX='g++';
    $supportDir='linux';
   }
 elsif ($os=~/OpenBSD/)
   {
    $OS='UNIX';
    $OSf='OpenBSD';
    $Compf='';
    $stdcxx='-lstdc++';
    $defaultCXX='g++';
    $supportDir='linux';
   }
 elsif ($os=~/OSF1/)
   {
    $OS='UNIX';
    $OSf='Tru64';
    $Compf='';
    $stdcxx='-lstdc++';
    $defaultCXX='g++';
    $supportDir='linux';
   }
 else
   {
    die('Unknown OS, you must do things by yourself');
   }
 # The gcc I found in an HP DS20E machine comes in a package called TWWfsw and
 # doesn't support -pipe. (Dual EV67 667 MHz machine running OSF1 v5.1).
 $UsePipe=($OS eq 'UNIX') && ($OSf ne 'Tru64');
 print "$OS";
 print " [$OSf]" if $OSf;
 print " [$Compf]" if $Compf;
 print "\n";
 $OS;
}

sub DetectOS2
{
 my $test;
 # Most MinGW users have Cygwin tools
 if ($Compf eq 'Cygwin/MinGW')
   {
    print 'Detecting compiler env. better: ';
    if ($conf{'Cygwin/MinGW'})
      {
       $Compf=$conf{'Cygwin/MinGW'};
       print "$Compf (cached)\n";
      }
    else
      {
       $test='
#include <stdio.h>
int main(void)
{
 #ifdef __CYGWIN__
 printf("Cygwin\n");
 #else
 printf("MinGW\n");
 #endif
 return 0;
}
';
       $Compf=RunGCCTest($GCC,'c',$test,'');
       chop($Compf);
       $conf{'Cygwin/MinGW'}=$Compf;
       print "$Compf\n";
      }
   }
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
 my ($a,$text,$rep,$repv,$line,$name,$value);

 print 'Configuring makefiles: ';
 foreach $a (@_)
   {
    print "$a ";
    $text=cat($a);
    if ($text)
      {
       $text=~s/RHIDE_GCC=(.*)\n/RHIDE_GCC=$GCC\n/g;
       $text=~s/RHIDE_GXX=(.*)\n/RHIDE_GXX=$GXX\n/g;
       $text=~s/RHIDE_LD=(.*)\n/RHIDE_LD=$GXX\n/g;
       $text=~s/RHIDE_OS_CFLAGS=(.*)\n/RHIDE_OS_CFLAGS=$CFLAGS\n/g;
       $text=~s/RHIDE_OS_CXXFLAGS=(.*)\n/RHIDE_OS_CXXFLAGS=$CXXFLAGS\n/g;
       $text=~s/RHIDE_AR=(.*)\n/RHIDE_AR=$GAR\n/g if ($GAR);
       $rep ='rc';
       $rep.='s' unless $conf{'UseRanLib'};
       $text=~s/RHIDE_ARFLAGS=(.*)\n/RHIDE_ARFLAGS=$rep\n/g;
       foreach $rep (%ExtraModifyMakefiles)
         {
          $repv="$rep=@ExtraModifyMakefiles{$rep}\n";
          $text=~s/$rep=(.*)\n/$repv/;
         }
       foreach $line (@MakeDefsRHIDE)
         {
          if ($line=~/([\w_]*)(\s*)=(\s*)(.*)/)
            {
             $name=$1;
             $value=$4;
             #print "$name=$value\n";
             $rep="^$name"."=((.+\\\\\\n)+(.+))";
             if ($text=~/$rep/m)
               { # Ok multiline
                $text=~s/$rep/$name=$value/mg;
               }
             else
               { # Single line
                $rep="^$name"."=(.*)";
                if ($text=~/$rep/m)
                  {
                   $text=~s/$rep/$name=$value/mg;
                  }
               }
            }
         }
       replace($a,$text);
      }
   }
 print "\n";
}

###[txh]####################################################################
#
# Prototype: ModifySimpleMakefiles(@list)
# Description:
#   Patches the listed makefiles to use the detected C compiler, C++
# compiler and compilation flags.@*
#
####################################################################[txi]###

sub ModifySimpleMakefiles
{
 my ($a,$text,$rep);

 print 'Configuring simple makefiles: ';
 foreach $a (@_)
   {
    print "$a ";
    $text=cat($a);
    if ($text)
      {
       $text=~s/GCC=(.*)\n/GCC=$GCC\n/;
       $text=~s/GXX=(.*)\n/GXX=$GXX\n/;
       $text=~s/LD=(.*)\n/LD=$GXX\n/;
       $text=~s/CFLAGS=(.*)\n/CFLAGS=$CFLAGS\n/;
       $text=~s/CXXFLAGS=(.*)\n/CXXFLAGS=$CXXFLAGS\n/;
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
#   Projects that needs faster C/C++ options are indicated with a +.
#
####################################################################[txi]###

sub CreateRHIDEenvs
{
 my ($a,$text,$rep,$useXtreme,$b,$old);

 print 'Configuring RHIDE: ';
 foreach $a (@_)
   {
    if (substr($a,0,1) eq "+")
      {
       $b=substr($a,1);
       $useXtreme=1;
      }
    else
      {
       $b=$a;
       $useXtreme=0;
      }
    print "$b ";
    $text ="#
# Automatically generated by configure script
# If you didn't configure this package please do it or edit this file
#

";
    $text.="RHIDE_GCC=$GCC\n";
    $text.="RHIDE_GXX=$GXX\n";
    $text.="RHIDE_LD=$GXX\n";
    $text.="RHIDE_AR=$GAR\n" if ($GAR);
    $rep ='rc';
    $rep.='s' unless $conf{'UseRanLib'};
    $text.="RHIDE_ARFLAGS=$rep\n";
    
    if ($useXtreme)
      {
       $text.="RHIDE_OS_CFLAGS=@conf{'XCFLAGS'}\n";
       $text.="RHIDE_OS_CXXFLAGS=@conf{'XCXXFLAGS'}\n";
      }
    else
      {
       $text.="RHIDE_OS_CFLAGS=$CFLAGS\n";
       $text.="RHIDE_OS_CXXFLAGS=$CXXFLAGS\n";
      }
    foreach $rep (@MakeDefsRHIDE)
      {
       $text.="$rep\n";
      }

    $old=cat($b);
    if ($text ne $old)
      {
       if ($text)
         {
          replace($b,$text);
         }
       else
         {
          unlink $b;
         }
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
 $text.="#define $_[0] 1";
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
 $text.="#define $_[0] 1";
 $text.='*/' unless (@conf{$_[0]} eq 'yes');
 $text;
}

sub ParentDir
{
 my ($cur,$parent);
 $cur=`pwd`;
 chop $cur;
 chdir('..');
 $parent=`pwd`;
 chop $parent;
 chdir($cur);
 $parent;
}

sub ReplaceText
{
 my ($Text,$Dest,$i,$se,$re,$ff);

 $Dest=$_[1];
 print "Processing $_[0] => $_[1]\n";
 $Text=cat($_[0]);
 $ff=0;
 foreach $i (%ReplaceTags)
   {
    $ff=!$ff;
    if ($ff)
      {
       $se='@'.$i.'@';
       $re=$ReplaceTags{$i};
       $Text =~ s/$se/$re/g;
      }
   }
 replace($Dest,$Text);
}

###[txh]####################################################################
#
# Prototype: ExtractItemsImk($makefile,\$column)
# Description:
#   Extracts the list of project items from a .imk.
#
# Return: The list of items.
#
####################################################################[txi]###

sub ExtractItemsImk
{
 my ($makefile,$col)=($_[0],\$_[1]);
 my ($mak,@items,$file,$result,$path);

 $result='';
 $makefile=~/((.+)\/+)/;
 $path=$1;
 print "Extracting from $makefile: ";
 $mak=cat($makefile);
 if ($mak=~ /(.*)::(((.+)\\\n)+(.+)\n)/ ||
     $mak=~ /(.*):: ((.+)\n)/)
   {
    print "processing\n";
    @items=split(/[\\\s]+/,$2);
    foreach $file (@items)
      {
       if ($file=~/\$\(ExOBJ\)/)
         {
          $file=~/((.+)\/+)(.*)/;
          $file=$3;
          $file=~s/\$\(ExOBJ\)/\.o/;
          #print "$file\n";
          if ($$col!=0)
            {
             if ($$col+length($file)>76)
               {
                $result.="\\\n\t";
                $$col=8;
               }
             else
               {
                $result.=' ';
                $$col++;
               }
            }
          $$col+=length($file);
          $result.="$file";
         }
      }
   }
 else
   {
    print "nothing found!\n";
   }
 $result;
}


###[txh]####################################################################
#
# Prototype: ExtractItemsMak($makefile,\$column)
# Description:
#   Extracts the list of project items from a .mak. Is recursive.
#
# Return: The list of items.
#
####################################################################[txi]###

sub ExtractItemsMak
{
 my ($makefile,$col)=($_[0],\$_[1]);
 my ($mak,@items,$file,$result,$path);

 $result='';
 $makefile=~/((.+)\/+)/;
 $path=$1;
 print "Extracting from $makefile\n";
 $mak=cat($makefile);
 if ($mak=~ /PROJECT_ITEMS=(((.+)\\\r?\n)+(.+)\r?\n)/ ||
     $mak=~ /PROJECT_ITEMS=((.+)\r?\n)/)
   {
    #print "$1\n";
    @items=split(/[\\\s]+/,$1);
    foreach $file (@items)
      {
       if ($file=~/\.gpr/)
         {
          $file=~s/\.gpr/\.mak/;
          $result.=ExtractItemsMak($path.$file,$$col);
          #print "**** Back ($path)\n";
         }
       else
         {
          #print "$file\n";
          if ($$col!=0)
            {
             if ($$col+length($file)>76)
               {
                $result.="\\\n\t";
                $$col=8;
               }
             else
               {
                $result.=' ';
                $$col++;
               }
            }
          $$col+=length($file);
          $result.="$file";
         }
      }
   }
 $result;
}

sub DetectCPU
{
 my $test;

 print 'Checking Architecture: ';

 if ($conf{'TV_CPU'})
   {
    print "$conf{'TV_CPU'} (cached)\n";
    $CPU=$conf{'TV_CPU'};
    return;
   }
 $test='
#include <stdio.h>
int main(void)
{
 #ifdef __i386__
 printf("x86\n");
 #elif defined(__alpha__) || defined(__alpha)
 printf("Alpha\n");
 #elif defined(__sparc_v9__) || defined(__sparcv9)
 printf("SPARC64\n");
 #elif defined(__sparc__) || defined(sparc)
 printf("SPARC\n");
 #elif defined(__PPC__) || defined(PPC) || defined(__ppc__)
 printf("PPC\n");
 #elif defined(__hppa__)
 printf("HPPA\n");
 #elif defined(__mips__)
 printf("MIPS\n");
 #elif defined(__ia64__)
 printf("Itanium\n");
 #elif defined(__amd64__)
 printf("AMD64\n");
 #else
 printf("Unknown\n");
 #endif
 return 0;
}
';
 $test=RunGCCTest($GCC,'c',$test,'');
 $test=~s/\W//g;
 $CPU=$conf{'TV_CPU'}=$test;
 print "$test\n";
}

sub LookForGNUMake
{
 my $test;
 print 'Looking for GNU make: ';

 if ($conf{'GNU_Make'})
   {
    print "$conf{'GNU_Make'} (cached)\n";
    return;
   }
 $test=RunRedirect('make --version',$ErrorLog);
 if ($test=~/GNU Make/)
   {
    $conf{'GNU_Make'}='make';
    print "make\n";
    return;
   }
 $test=RunRedirect('gmake --version',$ErrorLog);
 if ($test=~/GNU Make/)
   {
    $conf{'GNU_Make'}='gmake';
    print "gmake\n";
    return;
   }
 print "Unable to find GNU Make on this system.\n";
 print "Please install it and be sure it's in your path.\n";
 print "Also use `make' or `gmake' name for the binary.\n";
 die;
}

sub LookForGNUar
{
 my $test;
 print 'Looking for GNU ar: ';

 if ($conf{'GNU_AR'})
   {
    print "$conf{'GNU_AR'} (cached)\n";
    return $conf{'GNU_AR'};
   }
 $conf{'UseRanLib'}=0;
 $test=RunRedirect('ar --version',$ErrorLog);
 if (($test=~/GNU ar/) || ($test=~/BSD ar/))
   {
    $conf{'GNU_AR'}='ar';
    print "ar\n";
    return 'ar';
   }
 $test=RunRedirect('gar --version',$ErrorLog);
 if ($test=~/GNU ar/)
   {
    $conf{'GNU_AR'}='gar';
    print "gar\n";
    return 'gar';
   }
 if (!LookForFileInPath('ar'))
   {
    print "Unable to find GNU ar on this system.\n";
    print "Please install it and be sure it's in your path.\n";
    print "Also use `ar' or `gar' name for the binary.\n";
    die;
   }
 # I think all ar tools are usable but ...
 if (($OSf eq 'Darwin') || ($OSf eq 'HP-UX') || ($OSf eq 'Tru64') ||
     ($OSf eq 'Solaris'))
   {
    $conf{'GNU_AR'}='ar';
    $conf{'UseRanLib'}=1;
    print "ar (not GNU but usable!)\n";
    return 'ar';
   }
 if ($OSf eq 'QNX4')
   {
    $conf{'GNU_AR'}='ar';
    $conf{'UseRanLib'}=1;
    print "ar (WATCOM)\n";
    return 'ar';
   }
 print "Unable to find GNU ar on this system.\n";
 print "Please install it and be sure it's in your path.\n";
 print "Also use `ar' or `gar' name for the binary.\n";
 die;
}

sub LookForGNUinstall
{
 my ($test,$test2,$res,$testErr);

 print 'Looking for install tool: ';

 if ($conf{'GNU_INSTALL'})
   {
    print "$conf{'GNU_INSTALL'} (cached)\n";
    return $conf{'GNU_INSTALL'};
   }
 $conf{'SOLARIS_INSTALL'}=0;
 $test=RunRedirect('install --version',$ErrorLog);
 if ($test=~/Free Software Foundation/)
   {
    $conf{'GNU_INSTALL'}='install';
    print "install\n";
    return 'install';
   }
 $test2=RunRedirect('ginstall --version',$ErrorLog);
 if ($test2=~/Free Software Foundation/)
   {
    $conf{'GNU_INSTALL'}='ginstall';
    print "ginstall\n";
    return 'ginstall';
   }
 if (!LookForFileInPath('install'.$ExeExt))
   {
    print "Unable to find 'install' on this system.\n";
    print "Please install it and be sure it's in your path.\n";
    print "Also use `install' or `ginstall' name for the binary.\n";
    die;
   }
 $res='install';
 # Solaris install is bizarre.
 if ($OSf eq 'Solaris')
   {
    $conf{'SOLARIS_INSTALL'}=1;
   }
 # NetBSD install moves by default!!
 elsif ($OSf eq 'NetBSD')
   {
    $res='install -c';
   }
 $conf{'GNU_INSTALL'}=$res;
 print "$res\n";
 return $res;
}

sub GenInstallDir()
{
 my ($mode,$dir)=@_;

 return "\$(INSTALL) -d -m $mode $dir\n";
}

sub GenInstallFiles()
{
 my ($mode,$files,$dir)=@_;
 my (@f,$fl,$ret,$first);

 if ($conf{'SOLARIS_INSTALL'})
   {# Silly, crappy one ;-)
    @f=glob($files);
    return "\$(INSTALL) -m $mode -f $dir $files\n" if scalar(@f)==1;
    $ret='';
    $first=1;
    foreach $fl (@f)
      {
       if ($first)
         {
          $first=0;
         }
       else
         {
          $ret.="\t";
         }
       $ret.="\$(INSTALL) -m $mode -f $dir $fl\n";
      }
    return $ret;
   }
 return "\$(INSTALL) -m $mode $files $dir\n";
}

1;
