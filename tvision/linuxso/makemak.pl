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

$destination='/usr/lib';
$intver='1.0.6';
$extver='1';
unlink('tcedit.dst',glob('*.bkp'));
$f=&cat('../linux/Makefile');
&replace($f,'RHIDE_TYPED_LIBS_DJGPP.cc=stdcxx','RHIDE_TYPED_LIBS_DJGPP.cc=');
&replace($f,'RHIDE_TYPED_LIBS_Linux.cc=stdc\+\+','RHIDE_TYPED_LIBS_Linux.cc=');
&replace($f,'SPECIAL_CFLAGS=','SPECIAL_CFLAGS=-fPIC -pipe');
&replace($f,'SPECIAL_LDFLAGS=',"SPECIAL_LDFLAGS=-shared -Wl,-soname,librhtv.so.$extver -lc -lncurses -lm -lgpm");
&replace($f,'MAIN_TARGET=librhtv.a','MAIN_TARGET=librhtv.so.1.0');
&replace($f,'librhtv.a:: \$\(DEPS_0\)',"librhtv.so.$intver\:\: \$(DEPS_0)");
&replace($f,'\$\(RHIDE_COMPILE_ARCHIVE\)','$(RHIDE_COMPILE_LINK)');
&replace($f,'all:: librhtv.a',"all\:\: librhtv.so.$intver");
&replace($f,'-Wall','');
&replace($f,'-Werror','');
#&replace($f,'-gstabs3','');
&replaceFile('Makefile',$f);
system('ln -s ../linux/gkeyli.cc gkeyli.cc') unless (-s 'gkeyli.cc');
system("make");

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

print "
Running the ./instlib script you can install the libraries in $destination
you should read and modify it according to your needs.\n\n";

sub replace
{
 my $a=\$_[0],$search=$_[1],$repl=$_[2];
 $$a =~ s/$search/$repl/;
}

sub replaceFile
{
 my $b=$_[1];

 open(FIL,">$_[0]") || return 0;
 print FIL ($b);
 close(FIL);
}

sub cat
{
 local $/;
 my $b;

 open(FIL,$_[0]) || return 0;
 $b=<FIL>;
 close(FIL);

 $b;
}
