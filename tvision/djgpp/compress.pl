#!/usr/bin/perl
$oldv='1.0.1';
$newv='1.0.2';
@files=('../readme.txt');

# Patch the version number
foreach $i (@files)
  {
   print 'Processing '."$i";
   $r=&cat($i);
   if ($r =~ /$oldv/)
     {
      $r =~ s/$oldv/$newv/g;
      &replace($i,$r);
      print " updated version\n";
     }
   else
     {
      print " is uptodate\n";
     }
  }

# Generate the manifest and version files
$nv=$newv;
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
   if ((substr($i,0,1) eq '-') or (substr($i,0,1) eq '+'))
     {
      $i=substr($i,1);
     }
   $r.='contrib/tvision/'.$i;
  }
$r.="$srcmft\n$srcver\n";
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
replace($binver,"Turbo Vision - C++ Text User Interface library, binaries (version $newv)");
replace($srcver,"Turbo Vision - C++ Text User Interface library, sources  (version $newv)");

# Generate the zip files
$bindist="tv$nv".'b.zip';
$srcdist="tv$nv".'s.zip';
unlink($bindist,$srcdist);
print "\nGenerating source distribution\n\n";
system("zip -9 $srcdist \@$srcmft");
print "\nGenerating binary distribution\n\n";
system("zip -9 $bindist \@$binmft");

chdir('contrib/tvision/djgpp');


sub cat
{
 local $/;
 my $b;

 open(FIL,$_[0]) || return 0;
 $b=<FIL>;
 close(FIL);

 $b;
}


sub replace
{
 my $b=$_[1];

 open(FIL,">$_[0]") || return 0;
 print FIL ($b);
 close(FIL);
}


