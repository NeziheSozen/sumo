/****************************************************************************/
/// @file    ROVehicleCont.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A container for vehicles sorted by their departure time
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

#include <string>
#include <utils/common/NamedObjectCont.h>
#include <queue>
#include "ROVehicle.h"
#include "ROHelper.h"
#include "ROVehicleCont.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
ROVehicleCont::ROVehicleCont() throw() {}


ROVehicleCont::~ROVehicleCont() throw() {}


const ROVehicle * const
ROVehicleCont::getTopVehicle() const throw() {
    if (size()==0) {
        return 0;
    }
    return mySorted.top();
}


bool
ROVehicleCont::add(const std::string &id, ROVehicle *item) throw() {
    if (NamedObjectCont<ROVehicle*>::add(id, item)) {
        mySorted.push(item);
        return true;
    }
    return false;
}


void
ROVehicleCont::clear() throw() {
    mySorted = std::priority_queue<ROVehicle*, std::vector<ROVehicle*>, ROVehicleByDepartureComperator>();
    NamedObjectCont<ROVehicle*>::clear();
}


bool
ROVehicleCont::erase(const std::string &id) throw() {
    const ROVehicle * const topVeh = getTopVehicle();
    bool wasTop = topVeh!=0&&topVeh->getID()==id;
    if (!NamedObjectCont<ROVehicle*>::erase(id)) {
        return false;
    }
    if (wasTop) {
        mySorted.pop();
    } else {
        rebuildSorted();
    }
    return true;
}


void
ROVehicleCont::rebuildSorted() throw() {
    mySorted = std::priority_queue<ROVehicle*, std::vector<ROVehicle*>, ROVehicleByDepartureComperator>();
    std::map<std::string, ROVehicle*>::const_iterator i;
    const std::map<std::string, ROVehicle*> &mmap = getMyMap();
    for (i=mmap.begin(); i!=mmap.end(); ++i) {
        mySorted.push((*i).second);
    }
}


/****************************************************************************/
