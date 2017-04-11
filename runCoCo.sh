#!/usr/bin/env bash
echo $@
source /cvmfs/lsst.opensciencegrid.org/fnal/products/eups/bin/setups.sh
MY_EUPS=$PWD/eups
export EUPS_PATH=$MY_EUPS:/cvmfs/lsst.opensciencegrid.org/fnal/products/
echo $EUPS_PATH

echo "Setup required modules"
setup gcc v4_9_2
setup gsl
setup numpy
setup setuptools
setup python v2_7_8

echo "Exporting paths"
#export PYTHONPATH=$PYTHONPATH:$PWD/im3shape
#export PATH=$PATH:$PWD/deps/bin
#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD/deps/lib
#export PYTHONPATH=$PYTHONPATH:$PWD/deps/lib/python2.7/site-packages

echo "Running CoCo"
# This is where CoCo is run
python $PWD/cocosimtest.py 
echo "Did it work?"
