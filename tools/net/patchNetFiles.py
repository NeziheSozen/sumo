#!/usr/bin/env python
"""
@file    patchNetFiles.py
@author  Daniel.Krajzewicz@dlr.de 
@date    2008-08-13
@version $Id$

Reads in a file where junctions controlled by
 tls are stored and one which contains correct
 lane numbers. Applies the information on 
 the given edge and node file and saves them.

todo:
- make this read XML-files using an XML-API

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""


import os, sys
from optparse import OptionParser

def getAttr(source, what):
    mbeg = source.find(what + "=")
    mbeg = source.find('"', mbeg) + 1
    mend = source.find('"', mbeg)
    return source[mbeg:mend]




optParser = OptionParser()
optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                     default=False, help="tell me what you are doing")
optParser.add_option("-t", "--tls-file", dest="lsas",
                     help="File with tls nodes", metavar="FILE")
optParser.add_option("-l", "--lanes-file", dest="lanes",
                     help="File with lane number patched", metavar="FILE")
optParser.add_option("-e", "--edges-file", dest="edges",
                     help="File with XML-edges", metavar="FILE")
optParser.add_option("-n", "--nodes-file", dest="nodes",
                     help="File with XML-nodes", metavar="FILE")
(options, args) = optParser.parse_args()


# read in lsa definitions
lsas = {}
if options.lsas:
    index = 0
    fd = open(options.lsas)
    for line in fd:
        line = line.strip();
        if line=="" or line[0]=='#':
            continue
        (id, tls) = line.split(":")
        tls = tls.split(",")
        if id=="tls":
            if len(tls)>1:
                id = "j_" + str(index)
                index = index + 1
            else:
                id = tls[0]
        for t in tls:
            if t in lsas:
                print "Junction's '" + t + "' TLS already defined"
            lsas[t] = id
    fd.close()
# read in lane number patches
lanes = {}
if options.lanes:
    fd = open(options.lanes)
    for line in fd:
        line = line.strip();
        if line=="" or line[0]=='#':
            continue
        (edge, laneNo) = line.split(":")
        if edge in lanes:
            print "Edge's '" + edge + "' lane number already defined"
        lanes[edge] = int(laneNo)
    fd.close()

# patch
	# edges
fdi = open(options.edges)
fdo = open(options.edges + ".new.xml", "w")
for line in fdi:
    if line.find("<edge ")>=0:
        id = getAttr(line, "id")
        if id in lanes:
            indexB = line.find("nolanes")
            indexB = line.find('"', indexB)+1
            indexE = line.find('"', indexB)
            line = line[:indexB] + str(lanes[id]) + line[indexE:]
    fdo.write(line)
fdo.close()
fdi.close()
	# nodes
fdi = open(options.nodes)
fdo = open(options.nodes + ".new.xml", "w")
for line in fdi:
    if line.find("<node ")>=0:
        id = getAttr(line, "id")
        if id in lsas:
            indexE = line.find("/>")
            line = line[:indexE] + " type=\"traffic_light\" tl=\"" + lsas[id] + "\"/>";
    fdo.write(line)
fdo.close()
fdi.close()


		
