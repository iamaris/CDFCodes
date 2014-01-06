#
# A class to represent a single IntegrationLeg
#

import re
from Point import Point
from IntegrationData import IntegrationData
from integ_utils import removeParens

paren_pattern   = re.compile(r"\(.*?\)") # match anything in parenthesis ()
bracket_pattern = re.compile(r"\[.*?\]") # match anything in brackets []
brace_pattern   = re.compile(r"\{.*?\}") # match anything in braces {}

def parseIntegralLine(aString):
    """Return a 6--tuple of (name, material, startPoint, endPoint, current,
    cumulative)"""
    # It seems there should be a way to combine the contents of two tuples
    # into a single tuple, efficiently. If there is, this should be
    # re-written using it.
    name, material, start, end = paren_pattern.findall(aString)
    current, cumulative = bracket_pattern.findall(aString)
    return (name, material, start, end, current, cumulative)

class IntegrationLeg:
    def __init__(self, startPoint = Point(),
                 endPoint = Point(),
                 current = IntegrationData(),
                 cumulative = IntegrationData(),
                 material = "unk"):
        self.data = (startPoint, endPoint, current, cumulative)
        self.mat = material
        self.selftest()
    def start(self):
        return self.data[0]
    def end(self):
        return self.data[1]
    def current(self):
        return self.data[2]
    def cumulative(self):
        return self.data[3]
    def material(self):
        return self.mat
    def __str__(self):
        return "Material: %s, Start:%s, End:%s\nCurrent:%s\nCumulative:%s" % \
            ( self.material(), self.start(),
              self.end(), self.current(), self.cumulative() )
    def selftest(self):
        assert isinstance (self.start(), Point)
        assert isinstance (self.end(), Point)
        assert isinstance (self.current(), IntegrationData)
        assert isinstance (self.cumulative(), IntegrationData)
    def fromString(self, aString):
        """Make an IntegrationLeg from a line of text found in the
        IntegrationControl output"""
        dummy, mat, start, end, curr, cumul = parseIntegralLine(aString)
        self.data[0].fromString(start)
        self.data[1].fromString(end)
        self.data[2].fromString(curr)
        self.data[3].fromString(cumul)
        self.mat = removeParens(mat)
        self.selftest()

if __name__ == "__main__":
    # The following line came from read cdfIntegrate output.
    test_line = '(CDF 1/CCAL762/CMOD816/CHAM780)        (SILICON)       (1.72337,0.226886,6.63956)      [0.00336344     0.000689483     0.472227        0.0314818       2]      [0.00796142     0.0027121       26.5458 1.76972 14]     (1.75458,0.230995,6.63957)'
    ip = IntegrationLeg()
    ip.fromString(test_line)
    print ip
