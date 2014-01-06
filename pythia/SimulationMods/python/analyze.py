#
# This is a primitive script to parse the output of cdfIntegrate, and
# to generate a set of files (one for each track) that contains some of
# the integration results.
#
# It is intended as an example from which more useful scripts can be
# generated.
#
import sys
import re
import Point
from IntegrationLeg import IntegrationLeg
from math import sqrt

def geantPathLengths(x):
    f = open("output7.log")
    lines = f.readlines()
    for line in lines:
        if isGEANTLine(line):
            parts = string.split(line)
            name = parts[4] + '/' + parts[5]
            pathLength = pars[6]
            tuple = (name, pathLength)
            x.append(tuple)

def isIntegralLine(aString):
    """Return true if the aString is part of the integration output of
    IntegrationControl"""
    return aString[0:7]=="(CDF 1/"

def usage():
    print "Usage: analyze <filename>"
    print "<filename> must be the name of an output file created by cdfIntegrate"

if __name__ == "__main__":
    # Get the name of the input file.
    numArgs = len(sys.argv)
    if (numArgs != 2):
        usage()
        sys.exit(0)
    # Read the input file
    filename = sys.argv[1]
    try:
        f = open(filename, "r")
    except:
        print "Could not open the file: %s, exiting" % (filename)
        sys.exit(1)

    # Read the input
    int_lines = filter(isIntegralLine, f.readlines())

    # Proceed with processing

    # Crank out a set of files, one for each track, containing the following:
    # Col 1: cylindrical radius of the point
    # Col 2: integrated radiation lengths
    # Col 3: name of the last material
    ifile = 0
    ofilename = filename + str(ifile)
    print "Opening file: %s" % (ofilename)
    sys.stdout = open(ofilename, "w")
    previous_rho = 0.0 # hackery, used to detect when we've gone on to another track
    for line in int_lines:
        ip = IntegrationLeg()
        ip.fromString(line)
        rho = sqrt(ip.end().x()**2 +ip.end().y()**2)
        if rho < previous_rho:
            ifile = ifile + 1
            ofilename = filename + str(ifile)
            sys.stdout = sys.__stdout__
            print "Opening file: %s" % (ofilename)
            sys.stdout = open(ofilename, "w")
        previous_rho = rho
        print "%g %g %s" % (rho, ip.cumulative().radL(), ip.material())
