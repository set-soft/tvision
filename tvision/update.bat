:@Rem Copyright (C) 2003 by Salvador E. Tropea (SET),
:@Rem see copyrigh file for details
:@Rem
:@Rem This file updates the sources from CVS. I redirect the stderr to null
:@Rem because I don't want to fill the screen with the directory names. You
:@Rem must be careful because the errors are discarded.
:@Rem The -d option updates new directories, without it they aren't
:@Rem created.
:@Rem The -P option purges empty directories, they are no longer used
:@Rem directories that CVS keeps to allow recovering old tree structures.
:@Rem
@echo .
@echo Updating from CVS. Errors are discarded!
@echo .
@redir -e /dev/null cvs update -dP
