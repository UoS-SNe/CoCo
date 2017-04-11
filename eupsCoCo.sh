#!/usr/bin/env bash

source /cvmfs/lsst.opensciencegrid.org/fnal/products/eups/bin/setups.sh
MY_EUPS="$(cd "$(dirname "$1")"; pwd)/$(basename "$1")"
export EUPS_PATH=$MY_EUPS:/cvmfs/lsst.opensciencegrid.org/fnal/products/
echo $EUPS_PATH

echo "Setup required modules"
echo "gcc v4_9_2"; setup gcc v4_9_2
echo "gsl"; setup gsl
echo "numpy"; setup numpy
echo "setuptools"; setup setuptools

read -p "Python v2_7_8 setup breaks eups. Do you wish to continue? " -n 1 -r
echo    # (optional) move to a new line
if [[ $REPLY =~ ^[Yy]$ ]]
then
	echo "python v2_7_8"; setup python v2_7_8
fi



