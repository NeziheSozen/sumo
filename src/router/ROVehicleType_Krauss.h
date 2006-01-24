#ifndef ROVehicleType_Krauss_h
#define ROVehicleType_Krauss_h
//---------------------------------------------------------------------------//
//                        ROVehicleType_Krauss.h -
//  Description of parameters of a krauss-modeled vehicle
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
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
// Revision 1.11  2006/01/24 13:43:53  dkrajzew
// added vehicle classes to the routing modules
//
// Revision 1.10  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.9  2005/09/23 06:04:36  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.8  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.7  2005/05/04 08:55:13  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.6  2004/01/26 08:01:21  dkrajzew
// loaders and route-def types are now renamed in an senseful way;
//  further changes in order to make both new routers work;
//  documentation added
//
// Revision 1.5  2003/10/17 07:16:45  dkrajzew
// errors patched
//
// Revision 1.4  2003/07/22 15:14:13  dkrajzew
// debugging (false vehicle length)
//
// Revision 1.3  2003/07/16 15:36:50  dkrajzew
// vehicles and routes may now have colors
//
// Revision 1.2  2003/02/07 10:45:07  dkrajzew
// updated
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

#include <string>
#include <iostream>
#include "ROVehicleType.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ROVehicleType_Krauss
 * The parameters of the Krau�-model being a vehicle type.
 */
class ROVehicleType_Krauss : public ROVehicleType {
public:
    /// Constructor - uses default values
    ROVehicleType_Krauss();

    /// Parametrised constructor
	ROVehicleType_Krauss(const std::string &id, const RGBColor &col,
        SUMOReal length, SUMOVehicleClass vclass,
        SUMOReal a, SUMOReal b, SUMOReal eps, SUMOReal maxSpeed);

    /// Destructor
	~ROVehicleType_Krauss();

    /// Saves the vehicle type into the given stream using SUMO-XML
	std::ostream &xmlOut(std::ostream &os) const;

private:
    /// Krau�-parameter
	SUMOReal myA, myB, myEps, myMaxSpeed;

public:
    /// default parameter
    static SUMOReal myDefault_A, myDefault_B, myDefault_EPS,
        myDefault_LENGTH, myDefault_MAXSPEED;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

