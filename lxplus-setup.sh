#!/bin/bash

# set your environment
source /cvmfs/sft.cern.ch/lcg/views/setupViews.sh LCG_101 x86_64-centos7-gcc8-opt

echo "Computing Environment Setup"
echo "GCC    : $(gcc --version | grep "gcc (GCC)" | awk -F " " '{print $NF}')"
echo "ROOT   : $(root-config --version)"
echo "Python : $(python --version | awk -F " " '{print $NF}')"

export TOP=${PWD}
export PYTHONPATH=$PYTHONPATH:$PWD

echo "TOP        : $TOP"
echo "PYTHONPATH : $PYTHONPATH"

# update submodule
#git submodule add git@github.com:afiqaize/framework.git
#git submodule update --recursive
