#!/bin/bash

if [[ -z "$TBHOME" ]] ; then

export TBHOME=`pwd`
export TBLIB=$TBHOME/build/lib
export PATH=$TBHOME/bin:$TBHOME/python:$PATH
export PYTHONPATH=$TBHOME/python:$PYTHONPATH
export LD_LIBRARY_PATH=$TBHOME/build/lib:$LD_LIBRARY_PATH

# hack until someone figures out how to check for root-config in the Makefile
if [ -e /usr/bin/root ] ; then
  if [ -z $ROOTSYS ] ; then export ROOTSYS="." ; fi
fi

else 

echo "TBHOME is already set: $TBHOME"

fi



