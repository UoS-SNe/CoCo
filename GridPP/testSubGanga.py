import argparse
import numpy as np
import os

def parse_command_line(description=("CoCo 'lightcurve' test submission "
                "script, that will read in a list of 'lightcurves' and "
                "print the result(in reality square the number).")):
    """
    Parser of command line arguments
    """

    parser = argparse.ArgumentParser(
        description=description,
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    parser.add_argument("-f", "--infile", default='testinput.txt',
                        help="Input file")
                        
    parser.add_argument("-n", "--nlines", default=25, type=int,
                        help="Number of lines of input per job")
                        
    parser.add_argument("-m", "--maxjobs", default=0, type=int,
                        help="Number of max jobs (0 = no limit, split "
                        "all of input file)")
                        
    parser.add_argument("-p", "--prefix", default='output',
                        help="Output prefix of filename: "
                        "<prefix>.txt")
                        
    parser.add_argument("-d", "--jobdir", default='./split/',
                        help="Directory to store input job files.")
                        
    parser.add_argument("-e", "--eups", help="EUPS tarball location",
                    default='/lsst/user/d/darren.white/eupscoco.tar.gz')
                        
    parser.add_argument("-l", "--local", action="store_true",
        help="Flag to run locally. WARNING: Must set local infiles")

    return parser.parse_args()


###################################################### 
# The split_infile needs to be defined to create the individual input
# files, which your script then
def split_infile(infile,nline,maxjobs,jobdir):
	
    splitfiles = []
    # Read in file
    inlist = np.loadtxt(infile)
    
    # Split all of infile into chunks of specified length
    chunks = [inlist[i:i + nline] for i in xrange(0, len(inlist), nline)]
    
    if maxjobs!=0: # Only use up to max number of subjobs, if set
        chunks = chunks[0:maxjobs]
    
    # Create folder to store input files, if not already there
    try:
        os.makedirs(jobdir)
    except OSError, e:
        if e.errno != os.errno.EEXIST:
            raise   
        pass
        
    # Get basename without extension
    basefile = os.path.splitext(os.path.basename(infile))[0]
    
    # Write each chunk of input to make separate input file per subjob
    for i,chunk in enumerate(chunks):
        
        new_infile = '{0}/{1}_{2:04}in.txt'.format(jobdir,basefile,i+1)
        np.savetxt(new_infile, chunk, fmt='%d')
        splitfiles.append(new_infile)
        
    return splitfiles


def make_jobs(splitlist,prefix,local):

    # Set up a job with an executable application
    j = Job(application=Executable())
    j.application.exe = File('testJob.sh')
    
    # Define generic output filename
    outfile = '{0}.txt'.format(prefix)
    
    # Set up splitter to generate subjobs for each job
    j.splitter = GenericSplitter()
    
    # Set arguments and individual input files for each subjob, assign 
    # to splitter (makes subjobs across args/infiles pairs) - NOTE:
    # inputfiles here hsould be path strings, not Local/DiracFile as 
    # above.
    appargs = [[os.path.basename(fname),outfile] for fname in splitlist]
    infiles = [[fname] for fname in splitlist]
    j.splitter.multi_attrs = { "application.args": appargs, 
                               "inputfiles": infiles}
    
    # Define output file from script and where to put it after completion
    # LocalFile brings back to local machines ganga directory
    j.outputfiles = [LocalFile(outfile)]
    
    # Set backend to GridPP if local flag not set.
    # Set input files sent to each worker node (need to be defined as
    # LocalFile(), DiracFile(), etc depending on where job is being run)
    if local: 
        j.inputfiles = [LocalFile(args.eups), LocalFile('testScript.py')]
    else:
        j.inputfiles = [DiracFile(args.eups), LocalFile('testScript.py')]
        j.backend = Dirac()
    
    # Submit job, sent as individual subjobs that run independantly.
    j.submit()

    return 

args = parse_command_line()
splitlist = split_infile(args.infile,args.nlines,args.maxjobs,
                                                            args.jobdir)
make_jobs(splitlist,args.prefix,args.local)
