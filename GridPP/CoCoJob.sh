#!/usr/bin/env bash
start=`date +%s`
#Echo all commands
set -o xtrace

#Parameters
EUPS=$1
SPECTRA=$2
INFILE=$3
NLCS=$4

echo "Cloning CoCo from github"
git clone https://github.com/UoS-SNe/CoCo.git

endclone=`date +%s`
echo "$((endclone-start)) seconds to clone from github" >> $(basename $INFILE.log.dat)

echo "Unpacking EUPS and Spectra tarballs into CoCo directory"
tar -xzf $(basename $EUPS) --directory CoCo 2>&1
tar -xzf $(basename $SPECTRA) --directory CoCo 2>&1

enduntar=`date +%s`
echo "$((enduntar-endclone)) seconds to untar" >> $(basename $INFILE.log.dat)

cp $INFILE CoCo
cd CoCo 
echo "Switching to gridPP branch"
git checkout gridPP 
cp GridPP/CoCosim2.py ./
cp GridPP/sim_functions.py ./

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

endeups=`date +%s`
echo "$((endeups-enduntar)) seconds to set up EUPS" >> $(basename $INFILE.log.dat)

echo "Building CoCo"
make
python setup.py build_ext --inplace

endbuild=`date +%s`
echo "$((endbuild-endeups)) seconds to build pyCoCo" >> $(basename $INFILE.log.dat)

echo "Running on" `date`
echo "Running CoCo: python CoCoSim2.py "
python CoCosim2.py $INFILE $NLCS
echo "DONE"

endsim=`date +%s`
echo "$((endsim-endbuild)) seconds to run simulations" >> $(basename $INFILE.log.dat)
echo "$((endsim-start)) seconds to run simulations" >> $(basename $INFILE.log.dat)
