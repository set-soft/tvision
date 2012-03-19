#!/usr/bin/perl
# To update all the mkf files use: perl gen_mkf.pl `find . -name "*.mkf"`
# To create one: perl gen_mkf.pl ./DIR/FILE.mkf

for (@ARGV)
   {
    $_=~/([^\/]*)\.mkf/;
    $make=$1;
    $c=scalar(split(/\//))-1;
    $d='';
    while ($c--) { $d.='../'; }
    print "$_ $1 $d\n";
    open(FIL,">$_");
    print FIL "#!/usr/bin/make
#
# Copyright (c) 2003-2012 by Salvador E. Tropea.
# Covered by the GPL license.
#
vpath_src=
vpath_obj=.
INCLUDE_DIRS=".$d."include
COMMON_IMK=".$d."makes/common.imk
LIBS=rhtv
include \$(COMMON_IMK)
include $1\$(ExIMK)
";
    close(FIL);
   }


