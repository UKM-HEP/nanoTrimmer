#!/bin/bash

set -e

start_time=$SECONDS

# process the samples in the folder
python ./scripts/trim.py -d ${PWD}/samples_test/Run2011A_MuOnia -o $PWD -n 3 -c 3
python ./scripts/trim.py -d ${PWD}/samples_test/JPsiToMuMu_2MuPEtaFilter -o $PWD -n 3 -c 2

elapsed=$(( SECONDS - start_time ))
echo
eval "echo Elapsed time: $(date -ud "@$elapsed" +'$((%s/3600/24)) days %H hr %M min %S sec')"
