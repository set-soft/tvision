  This directory contains an example of how to exploit the new features found
in Turbo Vision v1.0.7 (July 1999) under Eterm. The text is oriented for any
program and as example I talk about my editor (setedit). As this file will be
probably distributed with both (the library and the editor) don't get
surprised if I sometimes I say "the editor" instead of "the program".
  Eterm is a very good replacement for xterm (UNIX X11 terminal emulator).
  You'll need Eterm v0.8.10 or a CVS snapshot that contains my patch to add
the --report-as-keysyms option to Eterm. [Thanx to MEJ who accepted this
crazy patch!].


1. What I gain?
2. What I need?
3. How it works?
4. Final comments


1. What I gain?

  When using Eterm and some associated things you gain a lot of information
from the keyboard. Things like Shift+Arrows can be detected by TV (Turbo
Vision).


2. What I need?

  You need Eterm 0.8.10 or newer, you also need to install the xterm-eterm-tv
terminal description in your system. In Debian GNU/Linux v2.0 the process is
this:

a) Compile the file: (for more information see the tic(1m) manpages)
# tic xterm-eterm-tv
b) Copy the compiled terminal description file to the right directory:
# cp /usr/share/terminfo/x/xterm-eterm-tv /etc/terminfo/x/

  Finally you need to run Eterm with a "theme" for your program. The files
contained here are a theme for the editor (setedit).
  If you are installing this files for setedit just copy the Setedit
directory to the directory where the Eterm themes are located, in my system
that's: /usr/share/Eterm/themes/
  Once this theme is installed you can invoke Eterm like this:

# Eterm -t Setedit --report-as-keysyms

  and it will automagically start setedit inside the window.
  In the case you need to adapt this theme for other TV program just read the
Eterm documentation or the comments in the theme called "Eterm" (that's the
default and is very well commented).
  You can then add an entry in the X wm menu or in GNOME menu or any other
menu an entry to avoid typing such a long line.


3. How it works?

  When Eterm is started with --report-as-keysyms the special keys (all
control keys) are passed to the program as escape sequences. X uses some
codes called keysyms to identify the keys, these values are defined by the X
consortium and includes all the keys known in UNIX consoles. The values are
16 bits, the last 256 values (0xFFxx) are reserved for control keys. When
Eterm receives one of these keys instead of translating it to a control key
and then emit the corresponding escape sequence just informs the keysym to
the application with the following sequence: \e[knn;xx~
Where: \e is the ESC code (0x1B), nn are the lower 8 bits of the modifier
keys (state of alt, shift, etc.) and xx the lower 8 bits of the keysym. Note
that if nn and xx can be just one digit and are encoded in hexadecimal.
  Why only the keysyms over 0xFF00? simple the keysyms under it are just
letters, they include symbols from many alphabets (cyrilic, hebrew, japanese,
etc.) but not control keys like arrows.
  This mode can be also enabled/disabled sending some special escape
sequences to Eterm, I don't remmember right now what Eterm extension number I
used right now so look in the Eterm's manual (I taked the first available).
  How the application knows that is running in a terminal with such a
feature? Simple, TV looks in the term name and only if it is xterm-eterm-tv
enables these things. The terminal name is defined in the Eterm theme and
that's why you need to install the terminal description file.


4. Final comments

  I added it because I really wanted to see the editor working well under X.
Even when I usually don't use the editor under X is nice to see it working as
well.
  I want feadback from this feature, what do you think about it? is too hard
to setup? what are the steps in your Linux? etc.


Enjoy, SET
