#!/bin/sh

echo -n "Generating tvision package ... "

if [ ! -d ./repository ]
then
   if ! mkdir ./repository
   then
      echo "fail !"
      echo "   Can't create ./repository directory !"
      exit 1
   fi
fi

if [ ! -d ./archive ]
then
   if ! mkdir ./archive
   then
      echo "fail !"
      echo "   Can't create ./archive directory !"
      exit 2
   fi
fi

rm -f ./repository/* >/dev/null 2>/dev/null
rm -f ./archive/* >/dev/null 2>/dev/null
rm -f ./*.qpm >/dev/null 2>/dev/null
rm -f ./*.qpk >/dev/null 2>/dev/null

if packager ./tvision-dev.qpg >/dev/null 2>/dev/null
then
   rm -f ./tvision-slib* >/dev/null 2>/dev/null
   mv ./*.qpm ./repository >/dev/null 2>/dev/null
   mv ./*.qpk ./repository >/dev/null 2>/dev/null
   cd ./repository >/dev/null 2>/dev/null
   tar cfz ../archive/tvision_x86-2.0.0-x86-public.qpr * >/dev/null 2>/dev/null
   cd - >/dev/null 2>/dev/null
else
   echo "fail !"
   echo "   Can't create package, packager fail !"
   exit 3
fi

echo "done !"

