#!/usr/bin/perl

for (@ARGV)
   {
    $_=~/([^\/]*)\.mkf/;
    $make=$1;
    $c=scalar(split(/\//));
    $d='';
    while ($c--) { $d.='../'; }
    print "$_ $1 $d\n";
    open(FIL,">$_");
    print FIL "#!/usr/bin/make
#
# Copyright (c) 2003-2004 by Salvador E. Tropea.
# Covered by the GPL license.
#
vpath_src=
vpath_obj=.
INCLUDE_DIRS=".$d."include
COMMON_IMK=".$d."makes/common.imk
include \$(COMMON_IMK)
include $1\$(ExIMK)
";
    close(FIL);
   }

