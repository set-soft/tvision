#!/usr/bin/perl
# Copyright (C) 2001 by Salvador E. Tropea (SET),
# see copyrigh file for details
#

require "../miscperl.pl";

$a=cat($ARGV[0]);
if ($a!~/\r\n/)
  {
   $a=~s/\n/\r\n/g;
   $a=~s/charset\=CP850/charset\=ISO\-8859\-1/;
  } 
replace($ARGV[1],$a);

