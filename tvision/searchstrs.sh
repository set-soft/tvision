#!/bin/sh
xgettext --default-domain=dummy --add-comments --keyword=_ --keyword=__ --omit-header  --add-location `perl -e 'while($a=<>){$a=~s/^[-+]//;if(!($a=~/^examples/) && $a=~/\.cc/){print"$a";}}' lista`
