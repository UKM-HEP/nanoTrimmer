#!/bin/bash

set -e

start_time=$SECONDS

# process the samples in the folder
python ./scripts/trim.py -d /disk01/cms-open-data/prod_v3/7TeV_tnp/RunI/7TeV/Run2011A_MuOnia -o $PWD -n 30 -c 5
python ./scripts/trim.py -d /disk01/cms-open-data/prod_v3/7TeV_tnp/RunI/7TeV/JPsiToMuMu_2MuPEtaFilter -o $PWD -n 30 -c 5

elapsed=$(( SECONDS - start_time ))
echo
eval "echo Elapsed time: $(date -ud "@$elapsed" +'$((%s/3600/24)) days %H hr %M min %S sec')"
