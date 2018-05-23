Introduction:
-------------

My Debian GNU/Linux Woody starts in code page mode, as Red Hat < 8.0 also
does. But some people told me Red Hat 8.0 starts in UTF-8 mode.
I tried setting the UTF-8 mode and starting SETEdit the result wasn't
really bad:
* When using /dev/vcs*: All worked perfectly and at exit the console was
in UTF-8.
* When using only escape sequences: Some drawings failed at the beggining
but it got normal quite soon. At exit the console was in code page mode.

This test will determine:
-------------------------

I) Is the console in UTF-8 mode?
II) If that's the case: Can we exit from UTF-8 mode?

Steps:
------

1) Compile code.cc and utf8.cc:
g++ -o code code.cc
g++ -o utf8 utf8.cc

2) Run "cat test.txt"
a) If you can see the accented vowels then this console is in normal mode.
b) If nothing is displayed then the console is in UTF-8 mode.

3) Run "cat test-utf8.txt"
This will confirm step 2:
a) Normal console: Displays a mix between A with ~ and other chars.
b) UTF-8 console: Displayes the accented vowels.

4) If the console is in UTF-8 mode try:
a) Run ./code
b) Do the steps 2 and 3. It should work as points 2a and 3a
c) Run ./utf8
d) Do the steps 2 and 3. It should work as the for the firt try.

Comment me about the results for each step.

