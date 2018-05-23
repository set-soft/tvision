Introduction:
-------------

TV can inform a lot of things when compiled in debug mode.

This test will determine:
-------------------------

Which mode of drawing is in use.

Steps:
------

1) Uncomment the following line:

//#define DEBUG

located at the include/tv/linux/log.h header.

2) Uncomment the following line:

//#define DEBUG_CODEPAGE

is line 298 aprox. located at classes/linux/linuxscr.cc

3) Recompile TV and make sure the editor is using this version of the library.

4) Start the editor using:

e -r 2> log.txt

5) Just exit.

The log.txt will contain very important information about what was detected,
send it to me.
If the user doesn't have access to the /dev/vcs* devices generate another log
file for the root.

