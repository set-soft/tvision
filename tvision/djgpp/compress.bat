cd ..\..
rm tv101s.zip tv101b.zip
zip -r9 tv101s tvision -x *.o *.gdt *.bkp *.dst *.a "*CVS/*" "*cvs/*"
zip -9 tv101b @tvision/djgpp/distlist
cd tvision\djgpp

