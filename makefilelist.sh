#!/bin/bash

CWD=${PWD}

###################################################
# Location containing all the sampels
BASE="/home/shoh/Works/cmsopendata/nanoTrimmer/samples_test"

# centre of mass energy
EN="8TeV"

# Name of the made input folder
PRODNAME="MYNAME"

# Define path for input file list directories
BASE_PATH="${PWD}/${PRODNAME}"
mkdir -p $BASE_PATH

## Choose your Data and MC Samples
PROCESSES=( \
	    #Run2011A_MuOnia \
	    #Run2011B_MuOnia \
	    #Run2012A_SingleMu \
	    #Run2012B_SingleMu \
	    #Run2012C_SingleMu \
	    #Run2012A_SingleElectron \
	    Run2012B_SingleElectron \
	    #Run2012C_SingleElectron \
	    #DYJetsToLL_M-50_TuneZ2Star \
	    #JPsiToMuMu_2MuPEtaFilter \
    )
###################################################

for PROCESS in ${PROCESSES[@]}
do
    for FILE in ${BASE}/${PROCESS}/*; do echo $FILE; done
    
done
