#!/usr/bin/env python

import os,subprocess,sys,time
import xml.dom.minidom as dom
sys.path.append(os.path.join(os.path.abspath(os.path.dirname(sys.argv[0])), "../../../../tools/traci"))
import traciControl
import time

if sys.argv[1]=="sumo":
    sumoBinary = os.environ.get("SUMO_BINARY", os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', 'bin', 'sumo'))
    addOption = ""
else:
    sumoBinary = os.environ.get("GUISIM_BINARY", os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', 'bin', 'sumo-gui'))
    addOption = "-Q"
PORT = 8813

def patchFile(ifile, ofile, replacements):
    fdi = open(ifile)
    fdo = open(ofile, "w")
    for line in fdi:
        for r in replacements:
            line = line.replace(r[0], r[1])
        fdo.write(line)
    fdi.close()
    fdo.close()

def runSingle(traciEndTime):
    step = 0
    sumoProcess = subprocess.Popen("%s -c used.sumo.cfg %s" % (sumoBinary, addOption), shell=True, stdout=sys.stdout)
    traciControl.initTraCI(PORT)
    while not step>traciEndTime:
        traciControl.cmdSimulationStep(1)
        step += 1
    print "Print ended at step %s" % traciControl.cmdGetSimulationVariable_currentTime()
    traciControl.cmdClose()
    sys.stdout.flush()
    
print "=========== long route ==========="
fdo = open("input_routes.rou.xml", "w")
print >> fdo, '<routes>"'
print >> fdo, '   <route id="horizontal" edges="2fi 2si 1o 1fi 1si 3o 3fi 3si 4o 4fi 4si"/>'
print >> fdo, '   <vehicle id="horiz" route="horizontal" depart="0"/>'
print >> fdo, '</routes>'
fdo.close()
print "----------- SUMO end time is smaller than TraCI's -----------"
sys.stdout.flush()
patchFile("sumo.sumo.cfg", "used.sumo.cfg", [ ["%end%", "<end value='50'/>"] ])
runSingle(99)
print "----------- SUMO end time is not given -----------"
sys.stdout.flush()
patchFile("sumo.sumo.cfg", "used.sumo.cfg", [ ["%end%", ""] ])
runSingle(99)


print "=========== empty routes in SUMO ==========="
fdo = open("input_routes.rou.xml", "w")
print >> fdo, '<routes>"'
print >> fdo, '</routes>'
fdo.close()
print "----------- SUMO end time is smaller than TraCI's -----------"
sys.stdout.flush()
patchFile("sumo.sumo.cfg", "used.sumo.cfg", [ ["%end%", "<end value='50'/>"] ])
runSingle(99)
print "----------- SUMO end time is not given -----------"
sys.stdout.flush()
patchFile("sumo.sumo.cfg", "used.sumo.cfg", [ ["%end%", ""] ])
runSingle(99)


print "=========== vehicle leaves before TraCI ends ==========="
fdo = open("input_routes.rou.xml", "w")
print >> fdo, '<routes>"'
print >> fdo, '   <route id="horizontal" edges="2fi 2si"/>'
print >> fdo, '   <vehicle id="horiz" route="horizontal" depart="0"/>'
print >> fdo, '</routes>'
fdo.close()
print "----------- SUMO end time is smaller than TraCI's -----------"
sys.stdout.flush()
patchFile("sumo.sumo.cfg", "used.sumo.cfg", [ ["%end%", "<end value='50'/>"] ])
runSingle(99)
print "----------- SUMO end time is not given -----------"
sys.stdout.flush()
patchFile("sumo.sumo.cfg", "used.sumo.cfg", [ ["%end%", ""] ])
runSingle(99)