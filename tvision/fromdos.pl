open(FI,"lista") || die "Can't open lista";

while ($a=<FI>)
  {
   chop($a);

   if (substr($a,0,1) eq "-")
     {
      $bin=1;
      $exe=0;
      $a=substr($a,1);
     }
   else
     {
      $bin=0;
      if (substr($a,0,1) eq "+")
        {
         $exe=1;
         $a=substr($a,1);
        }
      else
        {
         $exe=0;
        }
     }
   
   if (!$bin and length($a))
     {
      &ToUNIX($a) || die "Failed to convert $a";
     }
   if ($exe)
     {
      system("chmod +x $a");
     }
  }

sub ToUNIX
{
 local $/;
 my $b;
 my $a=$_[0];

 print "Processing $a\n";
 open(FIL,$a) || return 0;
 $b=<FIL>;
 $b =~ s/\r\n/\n/g;
 close(FIL);

 open(FIL,">$a") || return 0;
 print FIL ($b);
 close(FIL);
}
