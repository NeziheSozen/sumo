//---------------------------------------------------------------------------//
//                        MSDiscreteEventControl.cpp -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 29.05.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//

// $Log$
// Revision 1.7  2006/10/12 10:14:28  dkrajzew
// synchronized with internal CVS (mainly the documentation has changed)
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include "MSDiscreteEventControl.h"
#include <utils/helpers/Command.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * member method definitions
 * ======================================================================= */
MSDiscreteEventControl::MSDiscreteEventControl()
{
}


MSDiscreteEventControl::~MSDiscreteEventControl()
{
}


bool
MSDiscreteEventControl::hasAnyFor(EventType et, SUMOTime currentTime)
{
    return myEventsForAll.find(et)!=myEventsForAll.end();
}


void
MSDiscreteEventControl::execute(EventType et, SUMOTime currentTime)
{
    TypedEvents::iterator i = myEventsForAll.find(et);
    if(i!=myEventsForAll.end()) {
        const CommandVector &av = (*i).second;
        for(CommandVector::const_iterator j=av.begin(); j!=av.end(); j++) {
            (*j)->execute(currentTime);
        }
    }
}



void
MSDiscreteEventControl::add(EventType et, Command *a)
{
    if(myEventsForAll.find(et)==myEventsForAll.end()) {
        myEventsForAll[et] = CommandVector();
    }
    myEventsForAll[et].push_back(a);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

