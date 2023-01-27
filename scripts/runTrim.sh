#!/bin/bash

set -e

# process the samples in the folder
# -b batch;
python trim.py -d ${PWD}/samples_test/Run2012B_SingleElectron -o $PWD -n 3 -c 2
python trim.py -d ${PWD}/samples_test/DYJetsToLL_M-50_TuneZ2Star -o $PWD -n 3 -c 2