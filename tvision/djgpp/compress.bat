cd ..\..
rm tv100s.zip tv100b.zip
zip -r9 tv100s tvision -x *.o *.gdt *.bkp *.dst *.a
zip -9 tv100b tvision/readme.src tvision/copying* tvision/borland.txt tvision/djgpp/libtv.a tvision/include/*.h tvision/djgpp/sacadeb.bat tvision/djgpp/tmp
cd tvision\djgpp

