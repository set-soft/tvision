#!/bin/sh
# Copyright (C) 2003 by Salvador E. Tropea (SET),
# see copyrigh file for details
#
# This file executes the steps needed to create the RPM files. It doesn't
# check for errors and is really simple.
# It temporally changes the ~/.rpmmacros file, but the file is restored
# before exiting. If you stop the script you can recover the original file
# using the move located at the end.
# The file should be executed from the ../
#
if [ "$SET_RPM_DIR" ]; then
   SET_RPM_DIR="$SET_RPM_DIR";
else
   SET_RPM_DIR="$HOME";
fi
./configure --just-spec
chmod +x redhat/create-dirs.sh
chmod +x redhat/config-rpm.sh
redhat/create-dirs.sh
mv ~/.rpmmacros redhat/.rpmmacros-backup~
redhat/config-rpm.sh
cd linux; chmod +x compress; ./compress --use-bzip2 --dir-version; cd ..
mv ../rhtvision-`cat version.txt`.src.tar.bz2 $SET_RPM_DIR/rpm/SOURCES/librhtv-`cat version.txt`.tar.bz2
rpmbuild -bb redhat/librhtv-`cat version.txt`.spec
mv redhat/.rpmmacros-backup~ ~/.rpmmacros

