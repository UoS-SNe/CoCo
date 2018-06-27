#!/usr/bin/env bash
start=`date +%s`

# Echo all commands
set -o xtrace

# Run parameters set via Ganga
EUPS=$1
SPECTRA=$2
INFILE=$3
NLCS=$4

# Retrieving code from GitHub - git not installed on some nodes, so use wget
echo "Retrieving CoCo from GitHub"
wget https://github.com/UoS-SNe/CoCo/archive/gridPP.zip
unzip gridPP.zip 
mv CoCo-gridPP CoCo

# Unpack tarballs containing Spectra and EUPS directories
echo "Unpacking EUPS and Spectra tarballs into CoCo directory"
tar -xzf $(basename $EUPS) --directory CoCo 2>&1
tar -xzf $(basename $SPECTRA) --directory CoCo 2>&1

# Copy files to correct locations in CoCo directory 
cp $INFILE CoCo
cd CoCo 
cp GridPP/CoCosim2.py ./
cp GridPP/sim_functions.py ./

# Use EUPS to set up prerequisites
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
echo "scipy v0_14_0"; setup scipy v0_14_0
echo "python v2_7_8"; setup python v2_7_8

# Make and build CoCo libraries
echo "Building CoCo"
make
python setup.py build_ext --inplace

# Run simulation script for input file
echo "Running on" `date`
echo "Running CoCo: python CoCoSim2.py "
python CoCosim2.py $(basename $INFILE) $NLCS
echo "DONE"
