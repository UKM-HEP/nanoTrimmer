#!/bin/bash

set -e

start_time=$SECONDS

# process the samples in the folder
python ./scripts/trim.py -d /disk01/cms-open-data/prod_v3/8TeV_tnp/RunI/8TeV/Run2012B_SingleMu -o $PWD -n 30 -c 15
python ./scripts/trim.py -d /disk01/cms-open-data/prod_v2/8TeV_tnp/RunI/8TeV/DYJetsToLL_M-50_TuneZ2Star -o $PWD -n 30 -c 15

elapsed=$(( SECONDS - start_time ))
echo
eval "echo Elapsed time: $(date -ud "@$elapsed" +'$((%s/3600/24)) days %H hr %M min %S sec')"
