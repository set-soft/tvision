#! /usr/bin/perl
#
# This perl script creates the files needed to compile the dynamic version
# of the library. As RHIDE 1.4.7 can't generate makefiles for it I simply
# apply a patch to the Makefile used for the static version.
#
# Be careful because it calls make.
#
# It generates the installation script too.
#
require "../miscperl.pl";

SeeCommandLine();
GetVersion('../');

$destination='/usr/lib';
$intver=$Version;
$extver=$VersionMajor;
unlink('tcedit.dst',glob('*.bkp'));

#
# Fix the makefile
#
$f=&cat('../linux/Makefile');
&replaceVar($f,'RHIDE_TYPED_LIBS_DJGPP.cc=stdcxx','RHIDE_TYPED_LIBS_DJGPP.cc=');
&replaceVar($f,'RHIDE_TYPED_LIBS_Linux.cc=stdc\+\+','RHIDE_TYPED_LIBS_Linux.cc=');
# We will specify the libraries in the special flags
&replaceVar($f,'RHIDE_LIBS=(.*)','RHIDE_LIBS=');
&replaceVar($f,'SPECIAL_CFLAGS=','SPECIAL_CFLAGS=-fPIC');
&replaceVar($f,'SPECIAL_LDFLAGS=',"SPECIAL_LDFLAGS= -shared -Wl,-soname,librhtv.so.$intver -L/usr/X11R6/lib -lX11 -lgpm -lncurses -lstdc++ -lm -lc");
&replaceVar($f,'MAIN_TARGET=librhtv.a',"MAIN_TARGET=librhtv.so.$intver");
&replaceVar($f,'librhtv.a:: \$\(DEPS_0\)',"librhtv.so.$intver\:\: \$(DEPS_0)");
&replaceVar($f,'\$\(RHIDE_COMPILE_ARCHIVE\)','$(RHIDE_COMPILE_LINK)');
&replaceVar($f,'all:: librhtv.a',"all\:\: librhtv.so.$intver");
&replaceVar($f,'-Wall','');
&replaceVar($f,'-Werror','');
#&replaceVar($f,'-gstabs3','');
# Change the compat make by one here
&replaceVar($f,'\$\(MAKE\) -C \.\.\/compat\/','$(MAKE)');
# Make references to .o files from compat to the local obj dir
$f=~s/\.\.\/compat\///g;
&replace('Makefile',$f);

#
# Fix the compat.mak file
#
$f=&cat('../compat/compat.mak');
&replaceVar($f,'vpath_src=','vpath_src=../compat ');
&replaceVar($f,'RHIDE_TYPED_LIBS_DJGPP.cc=stdcxx','RHIDE_TYPED_LIBS_DJGPP.cc=');
&replaceVar($f,'RHIDE_TYPED_LIBS_Linux.cc=stdc\+\+','RHIDE_TYPED_LIBS_Linux.cc=');
&replaceVar($f,'SPECIAL_CFLAGS=','SPECIAL_CFLAGS=-fPIC');
&replaceVar($f,'\$\(RHIDE_COMPILE_ARCHIVE\)','$(RHIDE_COMPILE_LINK)');
&replaceVar($f,'-Wall','');
&replaceVar($f,'-Werror','');
&replace('compat.mak',$f);

# RHIDE 1.4.9 includes rhide.env
system('ln -s ../linux/rhide.env rhide.env') unless (-s 'rhide.env');
system('ln -s ../linux/gkeyli.cc gkeyli.cc') unless (-s 'gkeyli.cc');
system("make");
# Create the links so the library can be used from here
system("rm -f librhtv.so");
system("rm -f librhtv.so.$extver");
system("ln -s librhtv.so.$intver librhtv.so");
system("ln -s librhtv.so.$intver librhtv.so.$extver");

#
# Generate a small installation script
#
open(FIL,'>instlib') || die "Can't create the installation script";
print FIL ("rm -f $destination/librhtv.so\n");
print FIL ("rm -f $destination/librhtv.so.$extver\n");
print FIL ("rm -f $destination/librhtv.so.$intver\n");
print FIL ("ln -s $destination/librhtv.so.$intver $destination/librhtv.so\n");
print FIL ("ln -s $destination/librhtv.so.$intver $destination/librhtv.so.$extver\n");
print FIL ("cp librhtv.so.$intver $destination/\n");
print FIL ("strip --strip-debug $destination/librhtv.so.$intver\n");
print FIL ("ldconfig\n");
close(FIL);
system('chmod +x instlib');

if (!$DontShowInstMes)
  {
print "
Running the ./instlib script you can install the libraries in $destination
you should read and modify it according to your needs.\n\n";
  }

sub replaceVar
{
 my $a=\$_[0],$search=$_[1],$repl=$_[2];
 $$a =~ s/$search/$repl/;
}

sub SeeCommandLine
{
 my $i;

 foreach $i (@ARGV)
   {
    if ($i eq '--no-inst-message')
      {
       $DontShowInstMes=1;
      }
    else
      {
       if ($i eq '--help')
         {
          print "Usage: makemak.pl [--no-inst-message] [--help]\n\n";
          print "--no-inst-message  omit the installation message\n";
          print "--help             shows this message\n\n";
          die "\n";
         }
      }
   }
}
