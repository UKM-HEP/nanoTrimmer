import os, sys, time, glob
import subprocess as sp
from optparse import OptionParser

cwd = os.getcwd()

usage = "usage: %prog [options]"
parser = OptionParser(usage)
parser.add_option("-d","--directory", action="store", type="string", dest="directory", default="/disk05/cmsopendata/8TeV_tnp/RunI/8TeV") # point to where datasets are stored
parser.add_option("-o","--output", action="store", type="string", dest="output", default=os.environ['PWD'])
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
pd_samples= ""

######################################################################################

def execute( outdirectory_ , jobname_  ):
    outname = "%s" %( jobname_.replace(".txt",".root") )
    cmd="./trim"
    if batch :
        print("")
    else :
        trun = time.time();
        cmd+=" %s %s" %( jobname_ , outname )
        tproc = time.time()
        if test:
            print(cmd)
            sys.exit()
        else:
            print(cmd)
            os.system(cmd)
            #os.system('gdb --args %s' %cmd)
            print("--- running on %s took : %.3f seconds (%.3f minutes) ---" % ( samplename , (time.time() - tproc) , (time.time() - tproc)/60. ) )
            print("")
            print("--- Total run time : %.3f seconds (%.3f minutes) ---" % ( (time.time() - trun) , (time.time() - trun)/60. ) )
            
    pass

if __name__ == "__main__":

    for isample in glob.glob('%s/*' %directory):
        #samplename = "_".join(isample.split('/')[-1].strip('.root').split('_')[:-1])
        
        if "Run" or "JPsi" in isample:
            pd_samples = isample.split('/')[-1]
            outdirectory= '%s/%s_trim' %( output , pd_samples )
        else:
            pd_samples = '%s_%s' %( isample.split('/')[-1] , isample.split('/')[-2] )
            outdirectory= '%s/%s_trim' %( output , pd_samples )
       
        outdirectory= '%s/%s_trim' %( output , pd_samples )

        if not os.path.exists(outdirectory):
            os.system( "mkdir -p %s" %outdirectory )
        else:
            os.system( "rm -rf %s"   %outdirectory )
            os.system( "mkdir -p %s" %outdirectory )

        #os.system("make")
        #if batch: os.system('voms-proxy-init -voms cms -valid 168:00')

        # loop on root files
        count=0
        gcount=0
        rootfiles=[]
        for ifile in glob.glob('%s/*' %isample ):

            if count != nfile:
                jobname = '%s/%s__part-%s.txt' %( outdirectory , pd_samples , gcount )
                f=open( jobname , 'w' )
                if batch : ifile = 'root://eoscms.cern.ch/'+ifile

                count+=1
                rootfiles.append(ifile)
            else:
                f.write( '\n'.join(rootfiles) )
                f.close()
                execute( outdirectory , jobname )

                print("")
                count=0; rootfiles.clear()
                gcount+=1
