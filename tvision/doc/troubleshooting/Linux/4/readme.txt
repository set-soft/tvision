Introduction:
-------------

UNIX consoles are quite complex to handle in a way that's fast and reliable
at the same time. Buffered output can do a real mess.

This test will determine:
-------------------------

If Linux is executing all the escape sequences.

Steps:
------

1) Compile two versions of the test program:

g++ -o t1 test.cc
g++ -o t2 -DSEPARATED_HANDLE=1 test.cc

2) Run "./t1 1 2> log_4_1.txt"
Describe the output, the exact name of the colors is not really important,
but the sequence is. The colors should go from the 0 to 15 for foreground
and 15 to 0 for background 13 times.
Also pay attention to the separation between " Hello! " messages.

3) Run "./t1 20 2> /dev/null"
Check if the output is the same as (2), try with other numbers other than
20.

4) Run "./t2 1 2> log_4_2.txt"
Is the same output?

5) Run "./t2 20 2> /dev/null"
Is the same?

6) Just in case: reset the console using "reset" ;-)

7) Send me the log files and the descriptions.

