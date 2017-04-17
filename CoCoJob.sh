#!/usr/bin/env bash

#Echo all commands
set -o xtrace

#Parameters of this run
CODE_LOCAL_PATH=$1   # name of the software tar file
EUPS_LOCAL_PATH=$2

# unzip the software and change to its directory
tar -xzf $(basename $CODE_LOCAL_PATH)  2>&1
tar -xzf $(basename $EUPS_LOCAL_PATH) CoCo 2>&1
cd CoCo 2>&1
pwd

#Run the main code - also sets up environment
echo Running code `date`
ls 
echo ./runCoCo.sh  
./runCoCo.sh 2>&1
echo "DONE"
