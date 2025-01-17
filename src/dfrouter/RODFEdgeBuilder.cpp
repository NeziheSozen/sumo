/****************************************************************************/
/// @file    RODFEdgeBuilder.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 16.03.2006
/// @version $Id$
///
// Interface for building instances of dfrouter-edges
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

#include "RODFEdgeBuilder.h"
#include "RODFEdge.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
RODFEdgeBuilder::RODFEdgeBuilder() throw() {}


RODFEdgeBuilder::~RODFEdgeBuilder() throw() {}


ROEdge *
RODFEdgeBuilder::buildEdge(const std::string &name, RONode *from, RONode *to) throw() {
    return new RODFEdge(name, from, to, getNextIndex());
}


/****************************************************************************/

