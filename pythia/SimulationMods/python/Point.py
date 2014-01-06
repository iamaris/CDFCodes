#
# A very primitive 3-d point class.
#

from math import sqrt
from integ_utils import removeParens
# The following is needed by Python 1.5.2
from string import split



class Point:
    "This class represents a point in 3-d space"
    def __init__(self, xFloat= 0.0, yFloat=0.0, zFloat=0.0):
        self.data = (xFloat, yFloat, zFloat)
    def x(self):
        return self.data[0]
    def y(self):
        return self.data[1]
    def z(self):
        return self.data[2]
    def __repr__(self):
        return "Point(%s, %s, %s)" % (self.x(), self.y(), self.z())
    def dist2(self, aPoint):
        "Return the square of the distance from self to aPoint"
        return (self.x() - aPoint.x())**2 + (self.y() - aPoint.y())**2 + (self.z() - aPoint.z())**2
    def dist(self, aPoint):
        "Return the distance from self to aPoint"
        return sqrt(self.dist2(aPoint))
    def r(self):
        "Return the distance from self to the origin"
        return sqrt(self.dist2(Point()))
    def fromString(self, aString):
        dataString = removeParens(aString)
        t = split(dataString, ",")
        self.data = map(float, t)       
#       The following works under Python 2.0
#       t = dataString.split(",")
#       self.data = map(float, t)               

def test():
    "Self-test function for the Point class"
    p1 = Point(0.0, 4.0, -4.0)
    p2 = Point(3.0, 4.0, 0.0)
    d2_12 = p1.dist2(p2)
    d2_21 = p2.dist2(p1)
    assert d2_12 == 25.0, "Wrong result for d2_12"
    assert d2_21 == 25.0, "Wrong result for d2_21"
    d_12 = p1.dist(p2)
    d_21 = p2.dist(p1)
    assert d_12 == d_21, "Distance not symmetric"
    assert p2.r() == 5.0, "Distance from origin wrong"
    test_string = "(0.428965,-1.02667,7.82046)"
    p1.fromString(test_string)

if __name__ == "__main__":
    test()
