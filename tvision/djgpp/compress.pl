#!/usr/bin/perl
# Copyright (C) 1999-2003 by Salvador E. Tropea (SET),
# see copyrigh file for details
#
# Creates a distribution zip file.
#
require "../miscperl.pl";

GetVersion('../');

$r=cat('../include/tv.h');
if ($r!~/$Version/)
  {
   die "Error! inconsistent version in tv.h\n";
  }

$doBinary=0;
foreach $i (@ARGV)
  {
   if ($i eq '--binary')
     {
      $doBinary=1;
     }
   else
     {
      print "Unknown command line option: $i\n";
      print "Usage: compres.pl [--binary]\n\n";
     }
  }

# Update the makefile if needed
print 'makefile: ';
if (-M 'makefile' > -M 'librhtv.gpr')
  {# This fixes some details:
   system('gprexp librhtv.gpr');
   # Export the file:
   system('gpr2mak librhtv.gpr');
   # Now fix the remaining. This is needed only for RHIDE 1.5 CVS
   $a=cat('librhtv.mak');
   $a=~s/$here/\.\.\//mg;
   replace('Makefile',$a);
   unlink('librhtv.mak');
   print "(1) updated";
  }
else
  {
   print "(1) uptodate";
  }
if (-M '../compat/compat.mak' > -M '../compat/compat.gpr')
  {
   system('gprexp ../compat/compat.gpr');
   system('gpr2mak ../compat/compat.gpr');
   print "(2) updated\n";
  }
else
  {
   print "(2) uptodate\n";
  }
@files=glob('../examples/*/*.gpr');
foreach $i (@files)
  {
   $r=$i;
   $r=~s/\.gpr/\.mak/;
   print "$r: ";
   if (!(-e $r) || (-M $r > -M $i))
     {
      system("gprexp $i");
      system("gpr2mak $i");
      print "updated\n";
     }
   else
     {
      print "uptodate\n";
     }
  }
print "\n";
# BC++ 5.5 Makefile
chdir('..');
`perl confignt.pl`;
chdir('djgpp');
# Patch the version number in the readme.txt
print "Processing readme file\n";
$r=cat('../readme.in');
$r=~s/\@version\@/$Version/g;
replace('../readme.txt',$r);

# Generate the manifest and version files
$nv=$Version;
$nv=~ s/\.//g;
$binmft="manifest/tv$nv".'b.mft';
$binver="manifest/tv$nv".'b.ver';
$srcmft="manifest/tv$nv".'s.mft';
$srcver="manifest/tv$nv".'s.ver';

chdir('../../..');

open(FIL,'contrib/tvision/lista');
@files=<FIL>;
close(FIL);
$r='';
foreach $i (@files)
  {
   if ($i =~ /debian\//)
     {
      next;
     }
   if ((substr($i,0,1) eq '-') or (substr($i,0,1) eq '+'))
     {
      $i=substr($i,1);
     }
   $r.='contrib/tvision/'.$i;
  }
@files=glob('contrib/tvision/examples/*/*.mak');
$r.=join("\n",@files);
$r.="\n$srcmft\n$srcver\n";
@files=split(/\n/,$r);
@files=sort(@files);
$r=join("\n",@files);
replace($srcmft,$r);

open(FIL,'contrib/tvision/djgpp/distlist') || die "Can't open distrib list";
@files=<FIL>;
close(FIL);
$r='';
foreach $i (@files)
  {
   chop($i);
   print 'contrib/'.$i."\n";
   $r.=join("\r",glob('contrib/'.$i))."\r";
  }
$r.="$binmft\n$binver\n";
replace($binmft,$r);
replace($binver,"Turbo Vision - C++ Text User Interface library, binaries (version $Version)");
replace($srcver,"Turbo Vision - C++ Text User Interface library, sources  (version $Version)");

# Generate the zip files
$bindist="tv$nv".'b.zip';
$srcdist="tv$nv".'s.zip';
unlink($bindist,$srcdist);
print "\nGenerating source distribution\n\n";
system("zip -9 $srcdist \@$srcmft");
if ($doBinary)
  {
   print "\nGenerating binary distribution\n\n";
   system("zip -9 $bindist \@$binmft");
  }

chdir('contrib/tvision/djgpp');
