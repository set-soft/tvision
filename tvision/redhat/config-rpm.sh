#!/bin/sh
if [ "$SET_RPM_DIR" ]; then
   SET_RPM_DIR="$SET_RPM_DIR";
else
   SET_RPM_DIR="$HOME";
fi
echo "%_topdir       " $SET_RPM_DIR/rpm > ~/.rpmmacros
