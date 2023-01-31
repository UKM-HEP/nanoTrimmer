#!/bin/bash

set -e

start_time=$SECONDS

# process the samples in the folder
python ./scripts/trim.py -d ${PWD}/samples_test/Run2012A_SingleMu -o $PWD -n 3 -c 2
python ./scripts/trim.py -d ${PWD}/samples_test/DYJetsToLL_M-50_TuneZ2Star -o $PWD -n 3 -c 2

elapsed=$(( SECONDS - start_time ))
echo
eval "echo Elapsed time: $(date -ud "@$elapsed" +'$((%s/3600/24)) days %H hr %M min %S sec')"
