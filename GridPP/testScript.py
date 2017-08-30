#!/usr/bin/env python

import argparse
import numpy as np

def parse_command_line(description=("CoCo 'lightcurve' test submission"
                "script, that will read in a list of 'lightcurves' and "
                "print the result(in reality square the job number).")):
    """
    Parser of command line arguments
    """

    parser = argparse.ArgumentParser(
        description=description,
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    parser.add_argument("-i", "--infile", default='testinput.txt',
                        help="File containing MJDs")

    parser.add_argument("-o", "--outfile", default='out.txt',
                        help="Output filename")

    return parser.parse_args()
    
def readinfile(infile):

	return np.loadtxt(args.infile)

def sim(indata):
	
	simdata = indata**2
	return simdata
	
def writefile(outfile,simdata):
	
	np.savetxt(outfile, simdata, fmt='%d')
	
	return
	
if __name__ == "__main__":
    
	args = parse_command_line()
	indata = readinfile(args.infile)
	simdata = sim(indata)
	writefile(args.outfile,simdata)
	print "Job Complete"
	
