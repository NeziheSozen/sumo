#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    cadytsIterate.py
@author  Yun-Pang.Wang@dlr.de
@date    2010-09-15
@version $Id$

Run cadyts to calibrate the simulation with given routes and traffic measurements.
Respective traffic zones information has to exist in the given route files.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""
import os, sys, subprocess, types
from datetime import datetime
from optparse import OptionParser
from duaIterate import call, writeSUMOConf, initOptions

def main():
    optParser = initOptions()
    optParser.add_option("-r", "--route-alternatives", dest="routes",
                         help="route alternatives from sumo (comma separated list, mandatory)", metavar="FILE")
    optParser.add_option("-d", "--detector-values", dest="detvals",
                         help="adapt to the flow on the given edges", metavar="FILE")
    optParser.add_option("-c", "--classpath", dest="classpath",
                         default=os.path.join(os.path.dirname(sys.argv[0]), "..", "contributed", "calibration", "cadytsSumoController.jar"),
                         help="classpath for the calibrator [default: %default]")
    optParser.add_option("-s", "--last-calibration-step", dest="calibStep",
                         type="int", default=100, help="last step of the calibration [default: %default]")
    optParser.add_option("-S", "--demandscale", dest="demandscale", type="float", default=2., help="scaled demand [default: %default]")
    optParser.add_option("-o", "--od-matrix", dest="odmatrix",
                         help="sent estimated O-D matrix to", metavar="FILE")
    optParser.add_option("-F", "--freezeit",  dest="freezeit",
                         type="int", default=85, help="define the number of iterations for stablizing the results in the DTA-calibration")
    optParser.add_option("-V", "--varscale",  dest="varscale",
                         type="float", default=1., help="define variance of the measured traffi flows for the DTA-calibration")
    optParser.add_option("-P", "--PREPITS",  type="int", dest="PREPITS",
                         default = 5, help="number of preparatory iterations")
    optParser.add_option("-W", "--flows-evaluation", dest="flowevaluation",
                         help="sent flow evaluation to", metavar="FILE")
    optParser.add_option("-X", "--measformat",  type="choice", dest="measformat",
                         choices=('SUMO', 'Cadyts'), 
                         default = 'Cadyts',help="choose measurement format: SUMO or Cadyts")
    optParser.add_option("-Y", "--bruteforce", action="store_true", dest="bruteforce",
                         default = False, help="fit the traffic counts as accurate as possible")
    optParser.add_option("-Z", "--mincountstddev", type="float", dest="mincountstddev",
                         default = 25., help="minimal traffic count standard deviation")
    optParser.add_option("-O", "--overridett", action="store_true", dest="overridett",
                         default= False, help="override depart times according to updated link travel times", metavar="FILE")
    optParser.add_option("-z", "--equiprate", type="float", dest="equiprate",
                         default = 0., help="vehicle re-identification equipment rate [default: %default]")


    (options, args) = optParser.parse_args()
    if not options.net or not options.routes or not options.detvals:
        optParser.error("--net-file, --routes and --detector-values have to be given!")

    if options.mesosim:
        sumoBinary = os.environ.get("SUMO_BINARY", os.path.join(options.path, "meso"))
    else:
        sumoBinary = os.environ.get("SUMO_BINARY", os.path.join(options.path, "sumo"))
    calibrator = ["java", "-cp", options.classpath, "cadyts.interfaces.sumo.SumoController"]
    log = open("cadySumo-log.txt", "w+")
    
    # calibration init
    starttime = datetime.now()
    evalprefix = None
    if options.flowevaluation:
        evalprefix = options.flowevaluation[:options.flowevaluation.rfind('.')]

    # begin the calibration
    if options.odmatrix:
        call(calibrator + ["INIT", "-varscale", options.varscale, "-freezeit", options.freezeit,
              "-measfile", options.detvals, "-binsize", options.aggregation, "-PREPITS", options.PREPITS,
               "-measformat", options.measformat, "-bruteforce", options.bruteforce, "-demandscale", options.demandscale,
               "-mincountstddev", options.mincountstddev, "-overridett", options.overridett, "-equiprate", options.equiprate], log)
    else:
        call(calibrator + ["INIT", "-varscale", options.varscale, "-freezeit", options.freezeit,
              "-measfile", options.detvals, "-binsize", options.aggregation, "-PREPITS", options.PREPITS,
               "-measformat", options.measformat, "-bruteforce", options.bruteforce, "-mincountstddev", options.mincountstddev,
               "-overridett", options.overridett, "-equiprate", options.equiprate], log)

    for step in range(options.calibStep):
        print 'calibration step:', step
        files = []

        # calibration choice
        firstRoute = options.routes.split(",")[0]
        output = "%s_%s.cal.xml" % (firstRoute[:firstRoute.rfind('.')], step)
        if options.odmatrix:
            matrixfile = options.odmatrix[:options.odmatrix.rfind('.')] + '_%s.xml' % step
            call(calibrator + ["CHOICE", "-choicesetfile", options.routes, "-choicefile", "%s" % output, "-odmatrix", matrixfile], log)
        else:
            call(calibrator + ["CHOICE", "-choicesetfile", options.routes, "-choicefile", "%s" % output], log)
        files.append(output)
    
        # simulation
        print ">> Running simulation"
        btime = datetime.now()
        print ">>> Begin time: %s" % btime
        writeSUMOConf(step, options, ",".join(files))
        retCode = call([sumoBinary, "-c", "iteration_%s.sumo.cfg" % step], log)
        etime = datetime.now()
        print ">>> End time: %s" % etime
        print ">>> Duration: %s" % (etime-btime)
        print "<<"
        
        # calibration update
        if evalprefix:
            call(calibrator + ["UPDATE", "-netfile", "dump_%s_%s.xml" % (step, options.aggregation), "-flowfile", "%s_%s.txt" % (evalprefix, step)], log)
        else:
            call(calibrator + ["UPDATE", "-netfile", "dump_%s_%s.xml" % (step, options.aggregation)], log)
        print "< Step %s ended (duration: %s)" % (step, datetime.now() - btime)
        print "------------------\n"
        log.flush()
            
    print "calibration ended (duration: %s)" % (datetime.now() - starttime)
    log.close()

if __name__ == "__main__":
    main()