Introduction:
-------------

Linux can be configured to use a custom font with an arbitrary encoding. This
font doesn't have to follow any standard and in fact most standars doesn't
define how to display control characters ([0-31] + [127-159]).

This test will determine:
-------------------------

The font and Unicode maps.

Steps:
------

1) Run:

consolechars -F font.psf -M map.acm -U map.sfm

2) Send me the output files: font.psf, map.acm and map.sfm

