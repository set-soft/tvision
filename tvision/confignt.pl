#!/usr/bin/perl
# Copyright (C) 1996-2002 by Salvador E. Tropea (SET),
# see copyrigh file for details
#

require "miscperl.pl";
require "conflib.pl";

SeeCommandLine();

# 1) Get the list of files used by djgpp version
$col=14;
$a=ExtractItemsMak('djgpp/Makefile',$col);
# 2) Remove djgpp specific things
$a=~s/vga.cc//;
$a=~s/vgaregs.c//;
$a=~s/vgastate.c//;
$a=~s/vesa.cc//;
# 3) Add BC++ specific
#$a.=' gkeyw32.cc';
$a=~s/(\w+)\.(\w+)/\+$1\.obj/g;
$a=~s/\t//g;
# 4) Generate makefile from the template
$ReplaceTags{'TV_OBJS_BCC'}=$a;

$ReplaceTags{'DYNRTL'} = $conf{'dynrtl'} ? "DYNRTL = 1" : "";

ReplaceText('winnt/bccmake.in','winnt/Makefile');

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
    elsif ($i eq '--with-dynrtl')
      {
       $conf{'dynrtl'} = 1;
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
 print "--bindir=path  : defines the directory where *.exe files will be installed.\n";
 print "--setdir=path  : defines the directory where documentation andsupport files\n" .
       "                 will be installed.\n";
 print "--with-dynrtl  : compile with DLL runtime.\n";
}
