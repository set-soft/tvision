#!/usr/bin/perl
# Copyright (C) 1996,1997,1998,1999 by Salvador E. Tropea (SET),
# see copyrigh file for details
#

# Needs: cvs, cp, tar, gzip, zip, uname, chmod, dtou (only in DOS)

# Configuration:
$a=`uname`;
$DOS= $a=~/DOS/;
$pkg='tv';


if (length($ARGV[0])==0)
  {
   die "snapshot.pl Copyright (c) 1999 by Salvador E. Tropea

  Creates a patch and new files needed to apply over a release to recreate
the current state of the project. Some processing of the cvs output is
needed for it.

Use: snapshot.pl CVS_REVISION\n";
  }

if ($DOS)
  {
   $crlf="\n";
  }
else
  {
   $crlf="\r\n";
  }

# TODO enable it and do OS ok
print "Running diff against revision: $ARGV[0]\n\n";
system(RedirErrOut("cvs diff -u -r $ARGV[0]",'error.out','std.out'));

($sec,$min,$hour,$mday,$mon,$year,$wday,$ydat,$isdst)=localtime();
$mon++;
$year+=1900;
$snapdir="snap-shot-$pkg-";
$fecha="$year";
$fecha.='0' unless $mon>=10;
$fecha.=$mon;
$fecha.='0' unless $mday>=10;
$fecha.=$mday;
$snapdir.=$fecha;
mkdir($snapdir,00755);

# Process the diffs file to copy binary files that changed
print "Looking for binary files that changed\n\n";
open(FIL,'std.out');
open(DEST,">$snapdir/main.pat");
$used=1;
while ($a=<FIL>)
  {
   if ($a =~ /RCS file: (\/[^\/]+)+\/$pkg\/([^,]+)/)
     {
      $curfile=$2;
      if ($1=~/\/$pkg/) { $curfile=$pkg.'/'.$curfile; }
      $used=0;
     }
   # The idiot just puts the name of the file, of course patch isn't magician
   # and also needs to know the directory
   if ($used==0 && $a =~ /\+\+\+ ([^\s]+)/)
     {
      $a =~ s/\+\+\+ ([^\s]+)/\+\+\+ $curfile/;
      $used=1;
     }
   if ($a =~ /Binary files /)
     {
      print "$curfile is binary and changed, copying\n";
      # Redirect the error because directories generates problems
      system(RedirErr("cp -Pp $curfile $snapdir",'/dev/null'));
     }
   print DEST ($a);
  }
close FIL;
close DEST;
`rm std.out`;
dtou("$snapdir/main.pat");

# Process the new files
print "\nCopying new files\n\n";
open(FIL,'error.out');
while ($a=<FIL>)
  {
   if ($a =~ /is not in file ([^\n]+)/)
     {
      print "$1 is a new file, copying\n";
      system(RedirErr("cp -Pp $1 $snapdir",'/dev/null'));
      dtou("$snapdir/$1");
     }
  }
close FIL;
`rm error.out`;

# Generate a file to do the patch easier
open(FIL,">$snapdir/doit.bat");
print FIL ("patch -p0 < main.pat$crlf");
close FIL;
open(FIL,">$snapdir/doit");
print FIL ("#!/bin/sh\npatch -p0 < main.pat\n");
close FIL;
dtou("$snapdir/doit");
`chmod +x $snapdir/doit`;

# Compress it
print "\nCompressing the snap-shot\n";
if ($DOS)
  {
   `tar -zcvf $snapdir.tar.gz $snapdir`;
   `zip -9rm $snapdir $snapdir`;
  }
else
  {
   `tar -zcvf $snapdir.tar.gz $snapdir`;
   `zip -9rm $snapdir $snapdir`;
  }

# Generate an HTML file with links to download the patches
open(FIL,">snap-$pkg.html");
print FIL ("<HTML><HEAD><TITLE>Patch for the last snap-shot of $pkg</TITLE></HEAD>\n");
print FIL ('<BODY BGCOLOR="#A0D0FF" TEXT="#000000" LINK="#FF0000" VLINK="#000000">'."\n");
print FIL ("<Font size=+2><b>Snap shot $fecha</b></Font><Font size=+1><p>\n");
print FIL ("Uncompress the file in a temporal directory, a $snapdir directory will be\n");
print FIL ("created containing all the files and directories. Copy these\n");
print FIL ("files and directories over the old sources ($ARGV[0]) and then run the <i>doit.bat</i>\n");
print FIL ("script (or <i>doit</i> for Linux).<br>\n");
print FIL ("Ignore errors from patch about non-existent files with .cvsignore\n");
print FIL ("names, they are CVS specific and not distributed.<br>\n");
print FIL ('<p>File zipped: <a href="'.$snapdir.'.zip">'.$snapdir.'.zip</a><p>');
print FIL ('File tar/gzipped: <a href="'.$snapdir.'.tar.gz">'.$snapdir.'.tar.gz</a><p>');
print FIL ("Note I: Both patch files are in UNIX format, that is because djgpp and SETEdit supports both type of files. I can't say the same about Linux's patch and gcc.<p>");
print FIL ("Note II: This file was created by the script that creates the snap-shot, sorry if something looks funny.\n");
print FIL ("</HTML></BODY>\n");
close FIL;
dtou("snap-$pkg.html");


die "Chau!\n";

sub dtou
{
 if ($DOS)
   { # djgpp is better than Linux, both \n and \r\n are suported
    `dtou $_[0]`;
   }
}

sub RedirErr
{
 local $ret;
 if ($DOS)
   {
    $ret="redir -e $_[1] $_[0]";
   }
 else
   {
    $ret="$_[0] 2> $_[1]";
   }
 $ret;
}

sub RedirErrOut
{
 local $ret;
 if ($DOS)
   {
    $ret="redir -e $_[1] -o $_[2] $_[0]";
   }
 else
   {
    $ret="$_[0] 2> $_[1] > $_[2]";
   }
 $ret;
}
