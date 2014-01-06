#
# This is a primitive script to parse the output of cdfIntegrate, and
# to generate a set of files (one for each track) that contains some of
# the integration results.
#
# It is intended as an example from which more useful scripts can be
# generated.
#
import sys, re, regex, regsub, math
from math import sqrt, atan2

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

     	ofilename = filename + "0"
    	print "Opening file: %s" % (ofilename)
    	sys.stdout = open(ofilename, "w")
	whereAmI = 0
	theta = 0
	r_previous = 0
	intRadLengthsAtStart = 0
	intRadLengthsAtEnd = 0
    	for line in f.readlines():
		if(whereAmI==0):
			c = re.search('CFPL',line)
			if(not c==None):
				whereAmI = 1
		if (whereAmI==1):
			c = re.search('integral so far: *(-?[0-9.]+)',line)
			if(not c==None):
				intRadLengthsAtStart = float(c.group(1))
				whereAmI = 2
		if (whereAmI==2):
			c = re.search('CSPR',line)
			if (not c==None):
				whereAmI = 3
		if (whereAmI==3):
			c = re.search('integral so far: *(-?[0-9.]+)',line)
			if(not c==None):
				intRadLengthsAtEnd = float(c.group(1))
				whereAmI = 4
		
		d = re.search('coordinates of end: *\((-?[0-9.]+),(-?[0-9.]+),(-?[0-9.]+)\)',line)
		if (d!=None):
			x = float(d.group(1))
			y = float(d.group(2))
			z = float(d.group(3))
			r = sqrt(x**2+y**2)
			if(r<r_previous):
		        	print "%g %g" % (theta,intRadLengthsAtEnd-intRadLengthsAtStart)
				whereAmI = 0
			r_previous = r
			if(r>0):
				theta = atan2(r,z)*180./3.141592
	print "%g %g" % (theta,intRadLengthsAtEnd-intRadLengthsAtStart)

