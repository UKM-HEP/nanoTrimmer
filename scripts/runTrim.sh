#!/bin/bash

set -e

# process the samples in the folder
# -b batch;
python ./scripts/trim.py -d ${PWD}/samples_test/Run2011A_MuOnia -o $PWD -n 3 -c 3
#python ./scripts/trim.py -d ${PWD}/samples_test/JPsiToMuMu_2MuPEtaFilter -o $PWD -n 3 -c 2
