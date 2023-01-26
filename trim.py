import os, sys, time, glob
import subprocess as sp
from optparse import OptionParser

cwd = os.getcwd()

usage = "usage: %prog [options]"
parser = OptionParser(usage)
parser.add_option("-d","--directory", action="store", type="string", dest="directory", default="/disk05/cmsopendata/8TeV_tnp/RunI/8TeV") # point to where datasets are stored
parser.add_option("-o","--output", action="store", type="string", dest="output", default=os.environ['PWD']) # point where to output file
parser.add_option("-b","--batch", action="store_true", dest="batch", default=False) # using batch submission system @ CERN
parser.add_option("-t","--test", action="store_true", dest="test", default=False) # test on small samples
parser.add_option("-n","--nfile", action="store", type="int", dest="nfile", default=10) # how many file to process
parser.add_option("-c","--core", action="store", type="int", dest="core", default=-1 )

(options, args) = parser.parse_args()

cwd = os.getcwd()
trun = time.time();

directory = options.directory
output = options.output
batch = options.batch
test = options.test
nfile = options.nfile
core = options.core

######################################################################################

def tosubmit(outname_, cmd_ ):

    outscript=outname_.replace('.root','.sh')
    #outfilename=outname_.split('/')[-1].split('.root')[0]
    rootname=cmd_.split(' ')[-1].split('.root')[0]
    
    # condor
    with open( '%s' %(outscript) , 'a') as script :
        script.write( '#!/bin/bash\n' )
        script.write( 'export X509_USER_PROXY=/afs/cern.ch/user/'+os.environ["USER"][:1]+'/'+os.environ["USER"]+'/.proxy\n' )
        script.write( 'voms-proxy-info\n' )
        #script.write( 'export SCRAM_ARCH=$SCRAM_ARCH\n' )
        script.write( 'export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch\n' )
        script.write( 'source $VO_CMS_SW_DIR/cmsset_default.sh\n' )
        script.write( 'export HOME=%s\n' %os.environ['PWD'] )
        #script.write( 'source /cvmfs/sft.cern.ch/lcg/views/setupViews.sh LCG_98python3 x86_64-centos7-gcc8-opt\n' ) # hardcoded
        script.write( 'source /cvmfs/sft.cern.ch/lcg/views/setupViews.sh LCG_101 x86_64-centos7-gcc8-opt\n' )
        script.write( 'cd $HOME\n' )
        #script.write( 'cd $TMPDIR\n' )
        script.write( 'pwd\n' )
        script.write( '%s\n' %(cmd_) )
        script.write( 'xrdcp %s.root root://eosuser.cern.ch/%s/trim/%s/%s.root\n' %(rootname , directory , rootname.split('__')[0] , rootname) )
        script.close()
    os.system( 'chmod +x %s' %outscript )
    
    # submission script
    outscriptSub = outscript.replace( '.sh' , '.sub' )
    with open( '%s' %(outscriptSub) , 'a' ) as script :
        script.write( 'executable              = %s\n' %outscript )
        # https://batchdocs.web.cern.ch/local/submit.html#job-flavours
        script.write( '+JobFlavour             = \"microcentury\"\n') #espresso, microcentury, longlunch
        # https://batchdocs.web.cern.ch/local/submit.html#resources-and-limits
        script.write( 'request_cpus            = 4\n' )
        script.write( 'output                  = %s.out\n' %( outscript.replace('.sh' , '') ) )
        script.write( 'error                   = %s.err\n' %( outscript.replace('.sh' , '') ) )
        script.write( 'log                     = %s.log\n' %( outscript.replace('.sh','') ) )
        #script.write( 'transfer_output_remaps  = \"%s.root=%s\"\n' %( rootname , outname_ ) )
        #script.write( 'transfer_output_files = %s\n' %( outname_ ) )
        #script.write( 'output_destination = %s\n' %( cwd if not remoteout else root://eosuser.cern.ch//eos/user/s/shoh/cmsopendata/8TeV_tnp/RunI/8TeV/ ) )
        script.write( 'queue\n' )
        script.close()
    if not test:
        print('condor_submit %s' %(outscriptSub) )
        os.system( 'condor_submit %s' %(outscriptSub) )
    else:
        print('condor_submit %s' %(outscriptSub) )
        os.system( 'condor_submit %s' %(outscriptSub) )
        sys.exit()
    pass
         

def execute( outdirectory_ , jobname_  ):
    outname = "%s" %( jobname_.replace(".txt",".root") )
    cmd="./trim"
    cmd+=" %s %s %s" %( jobname_ , outname , core )
    
    if batch :
        cmd = cmd.replace(cmd.split(' ')[-1], cmd.split(' ')[-1].split('/')[-1] )
        tosubmit(outname, cmd )
        print("")
    else :
        print(cmd)
        trun = time.time()
        os.system(cmd)
        tproc = time.time()
        
        #os.system('gdb --args %s' %cmd)
        print("--- running on %s took : %.3f seconds (%.3f minutes) ---" % ( outname.split('/')[-1].replace('.root','') , (time.time() - tproc) , (time.time() - tproc)/60. ) )
        print("")
        print("--- Total run time : %.3f seconds (%.3f minutes) ---" % ( (time.time() - trun) , (time.time() - trun)/60. ) )
        if test: sys.exit()
        
    pass

if __name__ == "__main__":

    os.system("make clean")
    os.system("make")
    if batch: os.system('voms-proxy-init -voms cms -valid 168:00')

    pd_sample = directory.split('/')[-1]
    outdirectory= '%s/%s_trimmed' %( output , pd_sample )

    if not os.path.exists(outdirectory):
        os.system( "mkdir -p %s" %outdirectory )

    # loop on root files
    count=0
    ncount=0
    rootfiles=[]
    filelist = glob.glob('%s/*.root' %directory )
    l_filelist = len(filelist)
    for gcount, ifile in enumerate(filelist, start=1):
        if batch : ifile = 'root://eosuser.cern.ch/'+ifile

        # count is lower than nfile
        if count == 0: #and count != len(filelist):
            ncount+=1
            jobname = '%s/%s__part-%s.txt' %( outdirectory , pd_sample , ncount )
            f=open( jobname , 'w' )

        if count <= nfile:
            rootfiles.append(ifile)
            count+=1
            
        # when to stop writing text file contents
        if count == nfile or ( count <= nfile and gcount == l_filelist ) :
            #print("nfile : ", nfile ," ; gcount : ", gcount, " ; l_filelist : ", l_filelist ," ; count : ", count )
            f.write( '\n'.join(rootfiles) )
            f.close()
            execute( outdirectory , jobname )
            count=0; rootfiles.clear()
            print("")
