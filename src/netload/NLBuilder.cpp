/****************************************************************************/
/// @file    NLBuilder.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// The main interface for loading a microsim
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "NLBuilder.h"
#include <microsim/MSNet.h>
#include <microsim/MSEmitControl.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSGlobals.h>
#include <iostream>
#include <vector>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <string>
#include <map>
#include "NLHandler.h"
#include "NLEdgeControlBuilder.h"
#include "NLJunctionControlBuilder.h"
#include "NLDetectorBuilder.h"
#include "NLTriggerBuilder.h"
#include <microsim/MSVehicleControl.h>
#include <microsim/MSVehicleTransfer.h>
#include <microsim/MSRouteLoaderControl.h>
#include <microsim/MSRouteLoader.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/TplConvert.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/SysUtils.h>
#include <utils/common/ToString.h>
#include <utils/xml/XMLSubSys.h>
#include <microsim/output/MSDetectorControl.h>
#include <microsim/MSFrame.h>
#include <microsim/MSEdgeWeightsStorage.h>
#include <utils/iodevices/BinaryInputDevice.h>
#include "NLGeomShapeBuilder.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// NLBuilder::EdgeFloatTimeLineRetriever_EdgeWeight - methods
// ---------------------------------------------------------------------------
void
NLBuilder::EdgeFloatTimeLineRetriever_EdgeEffort::addEdgeWeight(const std::string &id,
        SUMOReal value, SUMOReal begTime, SUMOReal endTime) const throw() {
    MSEdge *edge = MSEdge::dictionary(id);
    if (edge!=0) {
        myNet.getWeightsStorage().addEffort(edge, begTime, endTime, value);
    } else {
        MsgHandler::getErrorInstance()->inform("Trying to set the effort for the unknown edge '" + id + "'.");
    }
}


// ---------------------------------------------------------------------------
// NLBuilder::EdgeFloatTimeLineRetriever_EdgeTravelTime - methods
// ---------------------------------------------------------------------------
void
NLBuilder::EdgeFloatTimeLineRetriever_EdgeTravelTime::addEdgeWeight(const std::string &id,
        SUMOReal value, SUMOReal begTime, SUMOReal endTime) const throw() {
    MSEdge *edge = MSEdge::dictionary(id);
    if (edge!=0) {
        myNet.getWeightsStorage().addTravelTime(edge, begTime, endTime, value);
    } else {
        MsgHandler::getErrorInstance()->inform("Trying to set the travel time for the unknown edge '" + id + "'.");
    }
}


// ---------------------------------------------------------------------------
// NLBuilder - methods
// ---------------------------------------------------------------------------
NLBuilder::NLBuilder(OptionsCont &oc,
                     MSNet &net,
                     NLEdgeControlBuilder &eb,
                     NLJunctionControlBuilder &jb,
                     NLDetectorBuilder &db,
                     NLHandler &xmlHandler) throw()
        : myOptions(oc), myEdgeBuilder(eb), myJunctionBuilder(jb),
        myDetectorBuilder(db),
        myNet(net), myXMLHandler(xmlHandler) {}


NLBuilder::~NLBuilder() throw() {}


bool
NLBuilder::build() throw(ProcessError) {
    // try to build the net
    if (!load("net-file")) {
        return false;
    }
    buildNet();
#ifdef HAVE_MESOSIM
    // load the previous state if wished
    if (myOptions.isSet("load-state")) {
        long before = SysUtils::getCurrentMillis();
        BinaryInputDevice strm(myOptions.getString("load-state"));
        if (!strm.good()) {
            MsgHandler::getErrorInstance()->inform("Could not read state from '" + myOptions.getString("load-state") + "'!");
        } else {
            MsgHandler::getMessageInstance()->beginProcessMsg("Loading state from '" + myOptions.getString("load-state") + "'...");
            unsigned int step = myNet.loadState(strm);
            if (myOptions.isDefault("begin")) {
                myOptions.set("begin", time2string(step));
            }
            if (step != string2time(myOptions.getString("begin"))) {
                WRITE_WARNING("State was written at a different time " + time2string(step) + " than the begin time " + myOptions.getString("begin") + "!");
            }
        }
        if (MsgHandler::getErrorInstance()->wasInformed()) {
            return false;
        }
        MsgHandler::getMessageInstance()->endProcessMsg("done (" + toString(SysUtils::getCurrentMillis()-before) + "ms).");
    }
#endif
    // load weights if wished
    if (myOptions.isSet("weight-files")) {
        if (!myOptions.isUsableFileList("weight-files")) {
            return false;
        }
        // build and prepare the weights handler
        std::vector<SAXWeightsHandler::ToRetrieveDefinition*> retrieverDefs;
        //  travel time, first (always used)
        EdgeFloatTimeLineRetriever_EdgeTravelTime ttRetriever(myNet);
        retrieverDefs.push_back(new SAXWeightsHandler::ToRetrieveDefinition("traveltime", true, ttRetriever));
        //  the measure to use, then
        EdgeFloatTimeLineRetriever_EdgeEffort eRetriever(myNet);
        std::string measure = myOptions.isSet("measure") ? myOptions.getString("measure") : "traveltime";
        if (measure!="traveltime") {
            std::string umeasure = measure;
            if (measure=="CO"||measure=="CO2"||measure=="HC"||measure=="PMx"||measure=="NOx"||measure=="fuel") {
                umeasure = measure + "_perVeh";
            }
            retrieverDefs.push_back(new SAXWeightsHandler::ToRetrieveDefinition(umeasure, true, eRetriever));
        }
        //  set up handler
        SAXWeightsHandler handler(retrieverDefs, "");
        // start parsing; for each file in the list
        std::vector<std::string> files = myOptions.getStringVector("weight-files");
        for (std::vector<std::string>::iterator i=files.begin(); i!=files.end(); ++i) {
            // report about loading when wished
            WRITE_MESSAGE("Loading weights from '" + *i + "'...");
            // parse the file
            if (!XMLSubSys::runParser(handler, *i)) {
                return false;
            }
        }
    }
    // load routes
    if (myOptions.isSet("route-files")&&myOptions.getInt("route-steps")<=0) {
        if (!load("route-files")) {
            return false;
        }
    }
    // load additional net elements (sources, detectors, ...)
    if (myOptions.isSet("additional-files")) {
        if (!load("additional-files")) {
            return false;
        }
    }
    WRITE_MESSAGE("Loading done.");
    return true;
}


void
NLBuilder::buildNet() throw(ProcessError) {
    MSEdgeControl *edges = 0;
    MSJunctionControl *junctions = 0;
    MSRouteLoaderControl *routeLoaders = 0;
    MSTLLogicControl *tlc = 0;
    try {
        myJunctionBuilder.closeJunctions(myDetectorBuilder);
        edges = myEdgeBuilder.build();
        junctions = myJunctionBuilder.build();
        routeLoaders = buildRouteLoaderControl(myOptions);
        tlc = myJunctionBuilder.buildTLLogics();
        MSFrame::buildStreams();
        std::vector<SUMOTime> stateDumpTimes;
        std::vector<std::string> stateDumpFiles;
#ifdef HAVE_MESOSIM
        const std::vector<int> times = myOptions.getIntVector("save-state.times");
        for (std::vector<int>::const_iterator i = times.begin(); i != times.end(); ++i) {
            stateDumpTimes.push_back(TIME2STEPS(*i));
        }
        if (!myOptions.isDefault("save-state.prefix")) {
            const std::string prefix = myOptions.getString("save-state.prefix");
            for (std::vector<SUMOTime>::iterator i = stateDumpTimes.begin(); i != stateDumpTimes.end(); ++i) {
                stateDumpFiles.push_back(prefix + "_" + time2string(*i) + ".bin");
            }
        } else {
            stateDumpFiles = StringTokenizer(myOptions.getString("save-state.files")).getVector() ;
        }
#endif
        myNet.closeBuilding(edges, junctions, routeLoaders, tlc, stateDumpTimes, stateDumpFiles);
    } catch (IOError &e) {
        delete edges;
        delete junctions;
        delete routeLoaders;
        delete tlc;
        throw ProcessError(e.what());
    } catch (ProcessError &) {
        delete edges;
        delete junctions;
        delete routeLoaders;
        delete tlc;
        throw;
    }
}


bool
NLBuilder::load(const std::string &mmlWhat) {
    if (!OptionsCont::getOptions().isUsableFileList(mmlWhat)) {
        return false;
    }
    std::vector<std::string> files = OptionsCont::getOptions().getStringVector(mmlWhat);
    for (std::vector<std::string>::const_iterator fileIt=files.begin(); fileIt!=files.end(); ++fileIt) {
        if (!gSuppressMessages) {
            MsgHandler::getMessageInstance()->beginProcessMsg("Loading " + mmlWhat + " from '" + *fileIt + "' ...");
        }
        long before = SysUtils::getCurrentMillis();
        if (!XMLSubSys::runParser(myXMLHandler, *fileIt)) {
            WRITE_MESSAGE("Loading of " + mmlWhat + " failed.");
            return false;
        }
        if (!gSuppressMessages) {
            MsgHandler::getMessageInstance()->endProcessMsg(" done (" + toString(SysUtils::getCurrentMillis()-before) + "ms).");
        }
    }
    return true;
}


MSRouteLoaderControl *
NLBuilder::buildRouteLoaderControl(const OptionsCont &oc) throw(ProcessError) {
    // build the loaders
    MSRouteLoaderControl::LoaderVector loaders;
    // check whether a list is existing
    if (oc.isSet("route-files")&&oc.getInt("route-steps")>0) {
        std::vector<std::string> files = oc.getStringVector("route-files");
        for (std::vector<std::string>::const_iterator fileIt=files.begin(); fileIt!=files.end(); ++fileIt) {
            if (!FileHelpers::exists(*fileIt)) {
                throw ProcessError("The route file '" + *fileIt + "' does not exist.");
            }
        }
        // open files for reading
        for (std::vector<std::string>::const_iterator fileIt=files.begin(); fileIt!=files.end(); ++fileIt) {
            loaders.push_back(new MSRouteLoader(myNet, new MSRouteHandler(*fileIt, false)));
        }
    }
    // build the route control
    return new MSRouteLoaderControl(myNet, oc.getInt("route-steps"), loaders);
}


/****************************************************************************/

