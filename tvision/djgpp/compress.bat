cd ..\..
rm tv100s.zip tv100b.zip
zip -r9 tv100s tvision -x *.o *.gdt *.bkp *.dst *.a "*CVS/*" "*cvs/*"
zip -9 tv100b @tvision/djgpp/distlist
cd tvision\djgpp

