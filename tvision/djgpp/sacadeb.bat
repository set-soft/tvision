cp libtv.a tmp
cd tmp
ar -x libtv.a
strip --strip-debug *.o
ar -r libtv.a *.o
mv ../libtv.a ../libtvdb.a
mv libtv.a ..
rm *
cd ..

