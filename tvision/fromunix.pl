open(FI,"lista.dos") || die "Can't open lista";

while ($a=<FI>)
  {
   chop($a);

   if (length($a))
     {
      &ToDOS($a) || die "Failed to convert $a";
     }
  }

sub ToDOS
{
 local $/;
 my $b;
 my $a=$_[0];

 print "Processing $a\n";
 open(FIL,$a) || return 0;
 $b=<FIL>;
 $b =~ s/[!\r]\n/\r\n/g;
 close(FIL);

 open(FIL,">$a") || return 0;
 print FIL ($b);
 close(FIL);
}
