#!/usr/bin/env bash

#Echo all commands
#set -o xtrace

#Parameters
INFILE=$1   # name of the input text file
OUTFILE=$2

# Untar eups package (should be handled by Ganga/Dirac...)
tar -xzf eupscoco.tar.gz

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

#Run the main code
echo "Running on" `date`
echo "Running Test: python $PWD/testscript.py"
python $PWD/testScript.py -i $INFILE -o $OUTFILE
echo "DONE"
