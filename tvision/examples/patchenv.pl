#!/usr/bin/perl
# Copyright (C) 1996,1997,1998,1999,2000 by Salvador E. Tropea (SET),
# see copyrigh file for details
#
#   This perl script reads the rhide.env file and replaces all the
# definitions of this file in the known .mak files. Only the needed
# definitions are patched, it means I don't replace a definition that is 100%
# equal to the new one. Additionally the .mak file is stored in disk only if
# it was changed.
#   The script prints a . for each search, a + for a one line replace and a
# * for a multiline replace.
#
@files=glob('*/*.gpr');

# Get the current rhide.env
open(FIL,'rhide.env') || die 'Hey! where is rhide.env?';
@vars=<FIL>;
close(FIL);

PatchFile(1,0);

sub PatchFile
{
 my $TVsp=$_[0],$NoWall=$_[1];

 # Patch the environment variables
 foreach $i (@files)
   {
    $i=~s/\.gpr/\.mak/;
    print 'Processing '."$i: ";
    $r=&cat($i);
    $changes=0;
    foreach $search (@vars)
      {
       if ($NoWall)
         {
          $search=~s/-Wall//g;
          $search=~s/-Werror//g;
         }
       # Skip comments
       next if (substr($search,0,1) eq '#');
       # Get the name of the variable
       if ($search =~ /([\w_]*)(\s*)=(\s*)(.*)/)
         {
          $name=$1;
          $value=$4;
          #print "$name\n$value\n";
          print '.';
         }
       else
         {
          next; #?
         }
 
       if ($name eq "TVISION_INC" && $TVsp)
         { # That's very special because affects the dependencies
          # Search the original definition
          $repl="^$name"."=(.+)";
          die "Can't find original TVISION_INC!" if (!($r =~ /$repl/m));
          if ($1 ne $value)
            {
             # Ok, now replace any dependency
             $dep=$1;
             $r =~ s/$dep\/([\w\.\/]+)/$value\/$1/mg;
             # And the value
             $r =~ s/$repl/$name=$value/mg;
             print 'd';
            }
         }
       else
         {
          $repl="^$name"."=((.+\\\\\\n)+(.+))";
          if (($r =~ /$repl/m) && ($1 ne $value))
            { # Ok multiline
             #print "\nReplacing:\n>$repl<\n>$1<\n>$name=$value<\n";
             $r =~ s/$repl/$name=$value/mg;
             print '*';
             $changes++;
            }
          else
            { # Single line
             $repl="^$name"."=(.*)";
             if (($r =~ /$repl/m) && ($1 ne $value))
               {
                $r =~ s/$repl/$name=$value/mg;
                print '+';
                $changes++;
               }
             #print "\$1=$1\n";
            }
         }
      }
    if ($changes)
      {
       replace($i,$r);
       print ' updated ';
      }
    print "\n";
   }
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

sub replace
{
 my $b=\$_[1];

 open(FIL,">$_[0]") || return 0;
 print FIL ($$b);
 close(FIL);
}


