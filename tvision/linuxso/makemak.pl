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

$destination='/lib';
$intver='1.0.4';
$extver='1';
unlink('tcedit.dst',glob('*.bkp'));
$f=&cat('../linux/Makefile');
&replace($f,'RHIDE_TYPED_LIBS_DJGPP.cc=stdcxx','RHIDE_TYPED_LIBS_DJGPP.cc=');
&replace($f,'RHIDE_TYPED_LIBS_Linux.cc=stdc\+\+','RHIDE_TYPED_LIBS_Linux.cc=');
&replace($f,'SPECIAL_CFLAGS=','SPECIAL_CFLAGS=-fPIC');
&replace($f,'SPECIAL_LDFLAGS=',"SPECIAL_LDFLAGS=-shared -Wl,-soname,libtv.so.$extver");
&replace($f,'MAIN_TARGET=libtv.a','MAIN_TARGET=libtv.so.1.0');
&replace($f,'libtv.a:: \$\(DEPS_0\)',"libtv.so.$intver\:\: \$(DEPS_0)");
&replace($f,'\$\(RHIDE_COMPILE_ARCHIVE\)','$(RHIDE_COMPILE_LINK)');
&replace($f,'all:: libtv.a',"all\:\: libtv.so.$intver");
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
print FIL ("rm -f $destination/libtv.so\n");
print FIL ("rm -f $destination/libtv.so.$extver\n");
print FIL ("rm -f $destination/libtv.so.$intver\n");
print FIL ("ln -s $destination/libtv.so.$intver $destination/libtv.so\n");
print FIL ("ln -s $destination/libtv.so.$intver $destination/libtv.so.$extver\n");
print FIL ("cp libtv.so.$intver $destination/\n");
print FIL ("strip --strip-debug $destination/libtv.so.$intver\n");
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
