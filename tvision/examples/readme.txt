Hi!

  Here you'll find some Turbo Vision example and widgets. Perhaps I'll
incorporate some of them in the library in the future, but only if I get
feedback.
  The examples were collected from the net and I think they can distributed
freely. Some of them are public domain. The GPL doesn't apply to this
directory.
  To compile the examples you must:

1) Have RHIDE (1.4.7 for TVision 1.0.2 distribution). I'm not including
makefiles to save space.
2) Edit the file common.env to indicate the compiler switchs you want. In
particular the RTTI stuff.

  Please take a look to the common.env file, it will teach you a lot about
how to use RHIDE for more than one system and distribute files. This file is
included by the rhide.env files located in each directory.

  All of these examples where easylly ported, read the porting.txt file to
learn what incompatibilities I found while porting the examples.

autoconf - This directory contains an example of how you can write autoconf/
           automake macros for this library. If you don't know what autoconf
           is just skip it.
demo     - That's the Borland's demo adapted by Sergio Sigala, I did some
           changes to compile it with this port.
desklogo - An example of how to change the desktop appearance. Contributed by
           Joel <jso@europay.com>.
           One of the examples also shows how to stop a TV application using
           ^Z under Linux.
dlgdsn   - That's a dialog design/code generator by Warlei Alves. I included
           it because it could be useful for people not using tools like
           EasyDiag and because Warlei told me he couldn't maintain it and
           asked me to maintain your project.
dyntxt   - Implements a StaticText with getData/setData member to make it
           "non-static".
inplis   - That's interesting. It implements a class with pairs label/data
           and allows the modification and browse of the data in a scrollabel
           list.
listvi   - That's a buggy example, if you like the idea you could fix it. I
           fixed tons of bugs and still buggy. The idea is similar to the one
           in the inplis example. I included it just because I fixed some
           stuff and perhaps somebody wants to finish it.
msgcls   - It shows the implementation of a message window.
progba   - Implements a progress bar widget. Look how the author polls the
           buttons.
sdlg     - That's a very good (and usefull) example. It implements a
           scrollable/resizeble dialog box.
sdlg2    - Same as sdlg but more advanced.
tcombo   - Implements a Combo Box widget. The implementation is clean and
           complete. That's a true example ;-).
terminal - That's more a test than an example. I did it to test the class
           because Andris found a bug in the Robert's translation of the
           obfuscated assembler of the Borland's version. It just test the
           TTerminal class.
tprogb   - Another progress bar, less flexible.
tutorial - It contains a very simple example application made in 16 steps to
           show how each thing is added. It was part of some tutorial from
           Borland. I taked it from the Sergio Sigala port. No modifications
           were needed, I just changed #include <tvision/tv.h> by
           #include <th.h> that's the common use. I added some extra
           comments.

Salvador Eduardo Tropea (salvador@inti.gov.ar)
