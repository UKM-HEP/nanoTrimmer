#!/bin/bash

set -e

# process the samples in the folder
# -b batch;
python ./scripts/trim.py -d ${PWD}/samples_test/Run2012B_SingleMu -o $PWD -n 3 -c 2
python ./scripts/trim.py -d ${PWD}/samples_test/DYJetsToLL_M-50_TuneZ2Star -o $PWD -n 3 -c 2
