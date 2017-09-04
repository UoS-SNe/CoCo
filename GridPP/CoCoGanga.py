import argparse
import os

def parse_command_line(description=("CoCo 'lightcurve' test submission"
                "script, that will read in a list of 'lightcurves' and "
                "print the result(in reality square the job number).")):
    """
    Parser of command line arguments
    """

    parser = argparse.ArgumentParser(
        description=description,
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    parser.add_argument("-e", "--eups", help="EUPS tarball location",
                    default='/lsst/user/d/darren.white/eupscoco.tar.gz')
                        
    parser.add_argument("-s", "--spec", help="Spectra tarball location",
                    default='/lsst/user/d/darren.white/spectra.tar.gz')
    
    parser.add_argument("-i", "--infile", help="Input csv",
                    default='GridPP/WFDinput.csv')
                    
    parser.add_argument("-n", "--nlcs", help="Number of required light curves",
                    default='1000')
    
    parser.add_argument("-l", "--local", action="store_true",
        help="Flag to run locally. WARNING: Must set local EUPS/Spec " 
             "file if set.")

    return parser.parse_args()


def submit(arglist):
	

    j = Job(application=Executable())
    
    # Set the application executable to that stored at the top of
    # the script
    j.application.exe = File('CoCoJob.sh')
    
    # Define the arguments passed to the script on running on the 
    # worker nodes.
    j.application.args = [os.path.basename(args.eups),
                          os.path.basename(args.spec),
                          os.path.basename(arglist.infile), 
                          arglist.nlcs]
    
    if args.local: # Local submission, need local tarballs
        # Define input files
        j.inputfiles = [LocalFile(args.eups),
                        LocalFile(args.spec),
                        LocalFile(arglist.infile)]
    else: # GridPP submission, need DiracFile tarballs and set backend
        # Define input files
        j.inputfiles = [DiracFile(lfn=args.eups),
                        DiracFile(lfn=args.spec),
                        LocalFile(arglist.infile)]
        j.backend = Dirac() 
    
    # Define output file from script. LocalFile(..) brings back to 
    # local machine from GridPP (in to ganga working directory)
    j.outputfiles = [LocalFile('*.dat')]
    
    j.submit()

    return 

args = parse_command_line()
submit(args)



