#!/usr/bin/env bash

#Echo all commands
set -o xtrace

#Parameters
EUPS=$1
SPECTRA=$2

echo "Cloning CoCo from github"
git clone https://github.com/UoS-SNe/CoCo.git

echo "Unpacking EUPS and Spectra tarballs into CoCo directory"
tar -xzf $(basename $EUPS) --directory CoCo 2>&1
tar -xzf $(basename $SPECTRA) --directory CoCo 2>&1

cd CoCo 
echo "Switching to gridPP branch"
git checkout gridPP 
cp GridPP/CoCosim.py ./

echo "Setting up EUPS"
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

echo "Building CoCo"
make
python setup.py build_ext --inplace

echo "Running on" `date`
echo "Running CoCo: python CoCoSim.py "
python CoCosim.py 
echo "DONE"
