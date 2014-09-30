#!/bin/bash

if [ $1 == ]; then
  LISTE="recettedialog depensedialog comptatriee"
  else
  LISTE=$1
fi

#if [ $LISTE == ]; then
#  exit
#fi

for i in $LISTE
  do
    if [ $i = 'recettedialog' ]  ||  [ $i = 'depensedialog' ] ; then
      cd src/comptabilite_interfaces_src/$i
      qmake-qt4 -nocache $i.pro
      make
      make clean
      cd ../../..
      echo ""
      echo "==========compilation $i OK ======================================"
      echo ""
    else 
      cd src/recettestriees/$i
      qmake-qt4 -nocache $i.pro
      make
      make clean
      cd ../../..
      echo ""
      echo "==========compilation $i OK ======================================"
      echo ""
    fi
   done

echo ""
echo "==============fin de la compilation les binaires sont dans /bin================"
echo ""

exit
  
