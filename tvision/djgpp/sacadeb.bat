cp librhtv.a tmp
cd tmp
ar -x librhtv.a
strip --strip-debug *.o
ar -r librhtv.a *.o
mv ../librhtv.a ../libtvdb.a
mv librhtv.a ..
rm *
cd ..

