#!/usr/bin/env python
"""
@file    routes2OD.py
@author  Julia.Ringel@dlr.de
@date    2009-04-15
@version $Id$

Creates an OD-matrix in visum format $O from a route.xml file
when starting the programm the name for the inputfile and the outputfile
have to de supplied via the command line
 
Copyright (C) 2008 DLR/FS, Germany
All rights reserved
"""

from xml.sax import make_parser
from xml.sax.handler import ContentHandler
import sys

class InputDataHandler(ContentHandler):
    def startElement(self, name, attrs):
        #print "name:", name
        for i in attrs.getNames():
            if i == 'depart':
                depart = int(attrs.getValue('depart'))
                if myTimes.Start > depart:
                    myTimes.Start = depart
                if myTimes.End < depart:
                    myTimes.End = depart

            if i == 'edges':
                route = attrs.getValue(i).split()
                start = route [0]
                end = route.pop()
                if start in myData:
                    if end in myData[start]:
                        myData[start][end] = myData[start][end]+1
                    else: 
                        myData[start][end] = 1
                else:
                    myData[start]={end:1}

class Times:
    def __init__(self):
        self.Start = 86400
        self.End = 0

    def seconds2TimeOfDay(self, seconds):
        hours = seconds/3600
        minutes = (seconds - (hours*3600))/60
        seconds_remaining = seconds - (hours*3600) - (minutes*60)
        if seconds_remaining > 29:
            minutes = minutes +1
        if minutes == 60:
            hours = hours + 1
            minutes = 0
        if minutes < 10:
            str_minutes = "0" + str(minutes)
        else:
            str_minutes = str(minutes)
        result = str(hours) + '.' + str_minutes
        return result
    
myData = {}
myTimes = Times()

try:
    myInputFile = sys.argv[1] 
    inputData = open(myInputFile, 'r')
    myparser = make_parser()
    myparser.setContentHandler(InputDataHandler())
    myparser.parse(inputData)
    inputData.close()

    """Ausgabe"""
    myOutputFile = sys.argv[2]
    outputData = open(myOutputFile, 'w')
    print >> outputData, '$OR;D2'
    print >> outputData, '*From-Time To-Time'
    print >> outputData, myTimes.seconds2TimeOfDay(myTimes.Start) + ' ' + myTimes.seconds2TimeOfDay(myTimes.End)
    print >> outputData, '* Factor'
    print >> outputData, '1.00'
    print >> outputData, '*some additional comments'
    startbez = myData.keys()
    startbez.sort()
    for x in startbez:
        endbez = myData[x].keys()
        endbez.sort()
        x1 = str(x).rjust(23)
        for y in endbez:
            y1 = str(y).rjust(20)
            value1 = (str(myData[x][y])+".00").rjust(12)
            print >>outputData, x1,y1,value1
    outputData.close()
except IndexError:
    l = len(sys.argv)
    if l == 1: 
        print "Es wurde kein Dateiname eingegeben."
    elif l==2:
        print "Dateiname fuer die Ausgabedatei wurde nicht angegeben"
    else:
        print "sonstiger fehler"
except IOError:
    print "Datei existiert nicht"
except:
    print "unerwarteter Fehler. ich weiss auch nicht, was hier nicht stimmt"
print "Fertig, Fertig"