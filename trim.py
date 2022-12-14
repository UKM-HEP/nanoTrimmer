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
parser.add_option("-m","--mdebugging", action="store_true", dest="debugging", default=False)
#parser.add_option("-r","--remoteout", action="store_true", dest="test", default=False)
parser.add_option("-n","--nfile", action="store", type="int", dest="nfile", default=10) # how many file to process in batch

(options, args) = parser.parse_args()

cwd = os.getcwd()
trun = time.time();

directory = options.directory
output = options.output
batch = options.batch
test = options.test
debugging = options.debugging
nfile = options.nfile
#remoteout = options.remoteout
pd_samples= ""

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
    cmd+=" %s %s" %( jobname_ , outname )

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

    if debugging :
        outdirectory= '%s/test' %( output )
        #if not os.path.exists(outdirectory): os.system( "mkdir -p %s" %outdirectory )
        os.system("make")

        # make txt file
        os.system("ls $PWD/test/*_test_*.root > %s/test.txt " %( outdirectory ) )
        cmd="./trim %s/test.txt %s/test_out.root" %( outdirectory , outdirectory )
        print(cmd)
        os.system(cmd)
        print("Debugging End")
        sys.exit()

        
    for isample in glob.glob('%s/*' %directory):
        
        if "Run" or "JPsi" in isample:
            pd_samples = isample.split('/')[-1]
            outdirectory= '%s/%s_trim' %( output , pd_samples )
        else:
            pd_samples = '%s_%s' %( isample.split('/')[-1] , isample.split('/')[-2] )
            outdirectory= '%s/%s_trim' %( output , pd_samples )
       
        outdirectory= '%s/%s_trim' %( output , pd_samples )

        if not os.path.exists(outdirectory):
            os.system( "mkdir -p %s" %outdirectory )
        #else:
        #    os.system( "rm -rf %s"   %outdirectory )
        #    os.system( "mkdir -p %s" %outdirectory )

        os.system("make")
        if batch: os.system('voms-proxy-init -voms cms -valid 168:00')

        # loop on root files
        count=0
        gcount=0
        rootfiles=[]
        for ifile in glob.glob('%s/*' %isample ):

            #if batch : ifile = 'root://eoscms.cern.ch/'+ifile
            if batch : 
                ifile = 'root://eosuser.cern.ch/'+ifile

            if count != nfile:
                jobname = '%s/%s__part-%s.txt' %( outdirectory , pd_samples , gcount )
                f=open( jobname , 'w' )

                count+=1
                rootfiles.append(ifile)
            else:
                f.write( '\n'.join(rootfiles) )
                f.close()
                execute( outdirectory , jobname )

                print("")
                count=0; rootfiles.clear()
                gcount+=1
