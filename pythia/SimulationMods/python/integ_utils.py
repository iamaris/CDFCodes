from string import replace

def removeParens(aString):
    temp1 = replace(aString, "(", " ")
    temp2 = replace(temp1, ")", " ")
    return temp2
#    return (aString.replace("("," ")).replace(")"," ") # for Python 2.0

def removeBrackets(aString):
    temp1 = replace(aString, "[", " ")
    temp2 = replace(temp1, "]", " ")
    return temp2
#    return (aString.replace("["," ")).replace("]"," ") # for Python 2.0

def removeBraces(aString):
    temp1 = replace(aString, "{", " ")
    temp2 = replace(temp1, "}", " ")
    return temp2
#    return (aString.replace("{"," ")).replace("}"," ") # for Python 2.0
