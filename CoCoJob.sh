#!/usr/bin/env bash

#Echo all commands
set -o xtrace

#Parameters
CODE_LOCAL_PATH=$1   # name of the software tar file
EUPS_LOCAL_PATH=$2
SPECTRA_LOCAL_PATH=$3

# unzip the software and change to its directory
# tar -xzf $(basename $CODE_LOCAL_PATH)  2>&1

echo $@
source /cvmfs/lsst.opensciencegrid.org/fnal/products/eups/bin/setups.sh
MY_EUPS=$PWD/eups
export EUPS_PATH=$MY_EUPS:/cvmfs/lsst.opensciencegrid.org/fnal/products/
echo $EUPS_PATH

echo "Setup required modules"
echo "gcc v4_9_2"; setup gcc v4_9_2
echo "gsl"; setup gsl
echo "numpy v1_9_1"; setup numpy v1_9_1
echo "setuptools"; setup setuptools
echo "minuit2"; setup minuit2
echo "python v2_7_8"; setup python v2_7_8

#echo "Exporting paths (if needed)"
#export PYTHONPATH=$PYTHONPATH:$PWD/im3shape
#export PATH=$PATH:$PWD/deps/bin
#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD/deps/lib
#export PYTHONPATH=$PYTHONPATH:$PWD/deps/lib/python2.7/site-packages

echo "Cloning CoCo from github"
git clone https://github.com/UoS-SNe/CoCo.git

echo "Unpacking EUPS and Spectra tarballs into CoCo directory"
tar -xzf $(basename $EUPS_LOCAL_PATH) --directory CoCo 2>&1
tar -xzf $(basename $SPECTRA_LOCAL_PATH) --directory CoCo 2>&1

cd CoCo 
echo "Switching to gridPP branch"
git checkout gridPP 

echo "Building CoCo"
make
python setup.py build_ext --inplace

#Run the main code - also sets up environment
echo "Running on" `date`
echo "Running CoCo: python $PWD/cocosimtest.py "
python $PWD/cocosimtest.py 
echo "DONE"
