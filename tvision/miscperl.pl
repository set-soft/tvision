# Version is stored in the root directory in a file called version.txt.
# It contains the version in Major.Middle.Minor format followed by only one
# line feed.
sub GetVersion
{
 $Version=cat($_[0].'version.txt');
 if (!$Version)
   {
    die "Missing version file! ($_[0]version.txt)\n";
   }
 chop $Version;
 $Version=~/(\d+)\.(\d+)\.(\d+)/;
 $VersionMajor =$1;
 $VersionMiddle=$2;
 $VersionMinor =$3;
 $Version="$1.$2.$3";
}

sub replace
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

1;
