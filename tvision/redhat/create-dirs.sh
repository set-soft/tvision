#!/bin/sh
if [ "$SET_RPM_DIR" ]; then
   SET_RPM_DIR="$SET_RPM_DIR";
else
   SET_RPM_DIR="$HOME";
fi
mkdir $SET_RPM_DIR/rpm
mkdir $SET_RPM_DIR/rpm/BUILD
mkdir $SET_RPM_DIR/rpm/RPMS
mkdir $SET_RPM_DIR/rpm/RPMS/i386
mkdir $SET_RPM_DIR/rpm/SOURCES
mkdir $SET_RPM_DIR/rpm/SPECS
mkdir $SET_RPM_DIR/rpm/SRPMS