import argparse
import os

def parse_command_line(description=("CoCo submission script")):
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
    
    parser.add_argument("-i", "--infiles", nargs='+', 
                                            help="List of input files")
                    
    parser.add_argument("-n", "--nlcs", default='1000', 
                                help="Number of required light curves "
                                "per input file")
    
    parser.add_argument("-l", "--local", action="store_true",
        help="Flag to run locally. WARNING: Must set local EUPS/Spec " 
             "file if set.")

    return parser.parse_args()

def submit(arglist):
    j = Job(application=Executable())
    
    # Set the application executable (this also contains EUPS setup steps)
    j.application.exe = File('CoCoJobSplit.sh')
    
    # Set up splitter to generate subjobs for each job
    j.splitter = GenericSplitter()
    
    # Set arguments and individual input files for each subjob, assign 
    # to splitter (makes subjobs across args/infiles pairs) - NOTE:
    # inputfiles here should be path strings, not Local/DiracFile as 
    # below.
    appargs = [[os.path.basename(args.eups),
                os.path.basename(args.spec),
                os.path.basename(fname), 
                arglist.nlcs] for fname in arglist.infiles]
                
    infiles = [[fname] for fname in arglist.infiles]
    
    j.splitter.multi_attrs = { "application.args": appargs, 
                               "inputfiles": infiles}
    
    # Set backend to GridPP if local flag not set.
    # Set input files sent to each worker node (need to be defined as
    # LocalFile(), DiracFile(), etc depending on where job is being run)
    if arglist.local: 
        j.inputfiles = [LocalFile(args.eups), LocalFile(args.spec)]
    else:
        j.inputfiles = [DiracFile(lfn=args.eups), DiracFile(lfn=args.spec)]
        j.backend = Dirac()

    # Define output file wildcard from script and where to put it after 
    # completion LocalFile brings back to local machines Ganga directory
    j.outputfiles = [LocalFile("*.dat")]
    
    j.submit()

    return 

args = parse_command_line()
submit(args)

