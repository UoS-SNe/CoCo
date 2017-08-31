import argparse
import os
import glob
import shutil

def parse_command_line(description=("CoCo 'lightcurve' test submission "
                "script, that will read in a list of 'lightcurves' and "
                "print the result(in reality square the number).")):
    """
    Parser of command line arguments
    """

    parser = argparse.ArgumentParser(
        description=description,
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    parser.add_argument("-j", "--jobnum", default='testinput.txt',
                        help="Job number of completed run")
                        
    parser.add_argument("-o", "--outdir", default='./output', 
                        help="Output directory")

    parser.add_argument("-w", "--wcard", default='*.dat',
                        help="Wildcard to search for e.g. '*.dat'")
    
    return parser.parse_args()
    
def locate_subjob_files(pattern, root_path):
    # Finds matching files in subjobs by walking along path
    matched = []
    for path, dirs, files in os.walk(os.path.abspath(root_path)):
        for filename in fnmatch.filter(files, pattern):
            matched.append(os.path.join(path, filename))
    return matched
    
def transfer_files(jn, outdir, wc):
    j = jobs(jn)
    gangaOutDir = j.outputdir
    
    try: #Make the output directory if it doesn't exist.
        os.makedirs(outdir)
    except OSError, e:
        if e.errno != os.errno.EEXIST:
            raise   
        pass
        
    if os.path.isdir(gangaOutDir): # Is a single job, files should be here
        files = glob.glob("{}/{}".format(gangaOutDir,wc))
    else: # A job made of subjobs, so need to check each subjob output
        gangaJobDir = gangaOutDir.replace('output/','')
        files = locate_subjob_files(wc, gangaJobDir)
        
    for outfile in files: # Copy files across
        shutil.copy(outfile, outdir)
        
args = parse_command_line()
transfer_files(args.jobnum, args.outdir, args.wcard)
