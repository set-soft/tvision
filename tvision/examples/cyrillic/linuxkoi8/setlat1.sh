#!/bin/sh
#
# This script restores the configuration you use.
# Is customized for my personal needs and is only an example.
#
#
# I use ISO-8859-1 fonts:
#
consolechars -f lat1-16.psf.gz
#
# In Debian 3.0 Woody the boot time keymap is here:
#
loadkeys /etc/console/boottime.kmap.gz
