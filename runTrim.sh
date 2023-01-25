#!/bin/bash

# process the samples in the folder

#python trim.py -d /eos/user/s/shoh/cmsopendata/8TeV_tnp/RunI/8TeV -o $PWD -n 20 -t -b
#python trim.py -d /eos/user/s/shoh/cmsopendata/7TeV_tnp/RunI/7TeV -o $PWD -n 10 -t
python trim.py -d ${PWD}/samples_test/Run2012B_SingleElectron -o $PWD -n 2

#python trim.py -d thetest -o $PWD
#python trim.py -m
