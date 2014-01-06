#
# A simple class to represent the blob of quantities that is accumulated
# by the cdfIntegrate program.
#
from integ_utils import removeBrackets
# The following is needed for Python 1.5.2
import string

class IntegrationData:
    """The data structure representing all the information accumulated for
    a single leg"""
    # We use a float for nsteps so that the fromString function is simple--
    # all conversion are to floats.
    def __init__(self,
                 rL = 0.0,
                 iL = 0.0,
                 bdl = 0.0,
                 dx = 0.0,
                 nsteps = 0.0):
        self.floatdata = (rL, iL, bdl, dx, nsteps)
    def __str__(self):
        return str(self.floatdata)
    def radL(self):
        return self.floatdata[0]
    def intL(self):
        return self.floatdata[1]
    def BdL(self):
        return self.floatdata[2]
    def dx(self):
        return self.floatdata[3]
    def nSteps(self):
        return self.floatdata[4]
    def fromString(self, aString):
        floatdataString = removeBrackets(aString)
        t = string.split(floatdataString)
        self.floatdata = map(float, t)
        
if __name__ == "__main__":
    # The test string was taken from actual cdfIntegrate program output.
    test_string = "[5.81274e-05\0112.0468e-05\0110\0111.76838\0114]"
    id = IntegrationData()
    id.fromString(test_string)
    assert id.radL() == 5.81274e-05, "radL wrong"
