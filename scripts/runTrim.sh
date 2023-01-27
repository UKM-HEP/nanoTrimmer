#!/bin/bash

# process the samples in the folder
python trim.py -d ${PWD}/samples_test/Run2012B_SingleElectron -o $PWD -m -n 3
python trim.py -d ${PWD}/samples_test/DYJetsToLL_M-50_TuneZ2Star -o $PWD -m -n 3
