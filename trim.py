import os, sys, time, glob
import subprocess as sp
from optparse import OptionParser

cwd = os.getcwd()

usage = "usage: %prog [options]"
parser = OptionParser(usage)
parser.add_option("-d","--directory", action="store", type="string", dest="directory", default="/disk05/cmsopendata/8TeV_tnp/RunI/8TeV") # point to where datasets are stored
parser.add_option("-o","--output", action="store", type="string", dest="output", default="./")
parser.add_option("-b","--batch", action="store_true", dest="batch", default=False)
parser.add_option("-t","--test", action="store_true", dest="test", default=False)
parser.add_option("-n","--nfile", action="store", type="int", dest="nfile", default=10) # how many file to process in batch

(options, args) = parser.parse_args()

cwd = os.getcwd()
trun = time.time();

directory = options.directory
output = options.output
batch = options.batch
test = options.test
nfile = options.nfile


if __name__ == "__main__":

    for isample in glob.glob('%s/*' %directory):
        samplename = "_".join(isample.split('/')[-1].strip('.root').split('_')[:-1])
        outdirectory= '%s/%s' %( output , samplename )
        if not os.path.exists(outdirectory):
            os.system( "mkdir -p %s" %outdirectory )
        else:
            os.system( "rm -rf %s"   %outdirectory )
            os.system( "mkdir -p %s" %outdirectory )

        os.system("make")
        if batch: os.system('voms-proxy-init -voms cms -valid 168:00')

        # loop on root files
        count=0
        rootfiles=[]
        for ifile in glob.glob('%s/*' %isample ):
            count+=1
            rootfiles.append(ifile)

            if count <= nfile:
                execute( )
                count=0; rootfiles.clear()

    os.system("make")
    if batch: os.system('voms-proxy-init -voms cms -valid 168:00')
    for iproc in samplelists:
        sample = iproc.split('/')[-1].split('.txt')[0]
        execute( sample , iproc , output , dataset_['lumi'] , dataset_['year'] )
