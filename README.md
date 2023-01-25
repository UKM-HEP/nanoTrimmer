# V1 nanoTrimmer
Just a skimming code, using RDataframe, on cms opendata.

For now, the code cater for Tag and Probe study.

## What does it do?

The packages (collective codes) use to pre-processing the NanoAOD root files into a lightweight and convenient format for offline tag-and-probe study
using UKM's basic computing facility.

## How to run

The script ```runtrim.sh``` encoded the basic syntex running on processes in the ```samples_test``` folder. The python script
```trim.py``` will be called, with argument(s) ```-d```, point to the root files contained in name of process/datasets ```Run2012B_SingleElectron```;
```-o```, point to the output folders containing the output root files; ```-n```, specify the number of root files input to the ```trim``` executable.

Running 
```
./runTrimmer.sh
``` 
will demonstrate the basic working example.

## User input

The relevant codes are:

| code/script  | What it does  |
|---|---|
| ```runTrim.sh``` | Specifies the source of the root files  |
| ```trim.cxx``` | pre-selection and driver for ```input/input.h``` |
| ```input/HLT.dat``` | High Level Trigger information to be kept  |
| ```input/input.h``` | Choices of parameters needed for tag and probe study | 
| ```input/out.dat``` | output variables needed for tag and probe efficiency calculation |

