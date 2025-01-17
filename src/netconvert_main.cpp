/****************************************************************************/
/// @file    netconvert_main.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Main for NETCONVERT
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

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

#include <iostream>
#include <string>
#include <netbuild/NBNetBuilder.h>
#include <netimport/NIOptionsIO.h>
#include <netimport/NILoader.h>
#include <netbuild/NBTypeCont.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBTypeCont.h>
#include <netbuild/NBDistrictCont.h>
#include <netbuild/NBTrafficLightLogicCont.h>
#include <netbuild/NBDistribution.h>
#include <utils/options/OptionsIO.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/MsgHandler.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int
main(int argc, char **argv) {
    OptionsCont &oc = OptionsCont::getOptions();
    // give some application descriptions
    oc.setApplicationDescription("Road network importer / builder for the road traffic simulation SUMO.");
    oc.setApplicationName("netconvert", "SUMO netconvert Version " + (std::string)VERSION_STRING);
    int ret = 0;
    try {
        XMLSubSys::init(false);
        NIOptionsIO::fillOptions();
        OptionsIO::getOptions(true, argc, argv);
        if (oc.processMetaOptions(argc < 2)) {
            SystemFrame::close();
            return 0;
        }
        MsgHandler::initOutputOptions();
        if (!NIOptionsIO::checkOptions()) throw ProcessError();
        RandHelper::initRandGlobal();
        NBNetBuilder nb;
        nb.applyOptions(oc);
        // load data
        NILoader nl(nb);
        nl.load(oc);
        if (oc.getBool("dismiss-loading-errors")) {
            MsgHandler::getErrorInstance()->clear();
        }
        // check whether any errors occured
        if (MsgHandler::getErrorInstance()->wasInformed()) {
            throw ProcessError();
        }
        nb.buildLoaded();
    } catch (ProcessError &e) {
        if (std::string(e.what())!=std::string("Process Error") && std::string(e.what())!=std::string("")) {
            MsgHandler::getErrorInstance()->inform(e.what());
        }
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
        ret = 1;
#ifndef _DEBUG
    } catch (...) {
        MsgHandler::getErrorInstance()->inform("Quitting (on unknown error).", false);
        ret = 1;
#endif
    }
    NBDistribution::clear();
    OutputDevice::closeAll();
    SystemFrame::close();
    // report about ending
    if (ret==0) {
        std::cout << "Success." << std::endl;
    }
    return ret;
}



/****************************************************************************/

