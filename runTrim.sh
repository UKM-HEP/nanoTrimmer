#!/bin/bash

# process the samples in the folder
# -b batch;
python trim.py -d ${PWD}/samples_test/Run2012B_SingleElectron -o $PWD -n 3
