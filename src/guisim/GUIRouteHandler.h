/****************************************************************************/
/// @file    GUIRouteHandler.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 17. Jun 2004
/// @version $Id: $
///
// Parser and container for routes during their loading
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIRouteHandler_h
#define GUIRouteHandler_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <microsim/MSRouteHandler.h>
#include <utils/gfx/RGBColor.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIRouteHandler
 */
class GUIRouteHandler : public MSRouteHandler
{
public:
    /// standard constructor
    GUIRouteHandler(const std::string &file, MSVehicleControl &vc,
                    bool addVehiclesDirectly, int incDUABase, int incDUAStage);

    /// standard destructor
    virtual ~GUIRouteHandler();

protected:
    /** parses an occured vehicle type definition */
    virtual void addVehicleType(const Attributes &attrs);

    /** adds the parsed vehicle type */
    virtual void addParsedVehicleType(const std::string &id,
                                      const SUMOReal length, const SUMOReal maxspeed, const SUMOReal bmax,
                                      const SUMOReal dmax, const SUMOReal sigma, SUMOVehicleClass vclass,
                                      const RGBColor &c);

    void closeRoute();
    void openRoute(const Attributes &attrs);

private:
    RGBColor myColor;

private:
    /** invalid copy constructor */
    GUIRouteHandler(const GUIRouteHandler &s);

    /** invalid assignment operator */
    GUIRouteHandler &operator=(const GUIRouteHandler &s);

};


#endif

/****************************************************************************/

