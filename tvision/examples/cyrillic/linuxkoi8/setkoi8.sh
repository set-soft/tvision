#!/bin/sh
#
# This script sets a russian configuration.
# Is customized for my personal needs and is only an example.
#
# Load a KOI8-R font. It contains a fake SFM.
#
consolechars -f koi8-r.psf.gz
#
# Load a KOI8-R map. That's a fake ACM.
#
consolechars -m koi8-r.acm.gz
#
# Load a russian keyboard map.
#
loadkeys ru1.kmap.gz
