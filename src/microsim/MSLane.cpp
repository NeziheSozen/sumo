/***************************************************************************
                          MSLane.cpp  -  The place where Vehicles
                          operate.
                             -------------------
    begin                : Mon, 05 Mar 2001
    copyright            : (C) 2001 by ZAIK http://www.zaik.uni-koeln.de/AFS
    author               : Christian Roessel
    email                : roessel@zpr.uni-koeln.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

namespace
{
    const char rcsid[] =
    "$Id$";
}

// $Log$
// Revision 1.9  2003/03/03 14:56:20  dkrajzew
// some debugging; new detector types added; actuated traffic lights added
//
// Revision 1.8  2003/02/07 10:41:50  dkrajzew
// updated
//
// Revision 1.7  2002/10/29 10:43:38  dkrajzew
// bug of trying to set the destination lane for vehicles that vanish before they reach the point of halt removed
//
// Revision 1.6  2002/10/28 12:59:38  dkrajzew
// vehicles are now deleted whe the tour is over
//
// Revision 1.5  2002/10/18 11:51:03  dkrajzew
// breakRequest or driveRequest may be set, althoug no first vehicle exists due to regarding a longer break gap...; assertion in moveFirst replaced by a check with a normal exit
//
// Revision 1.4  2002/10/17 13:35:23  dkrajzew
// insecure usage of potentially null-link-lanes patched
//
// Revision 1.3  2002/10/17 10:43:35  dkrajzew
// error of setting of link-driverequests out of a vehicles route repaired
//
// Revision 1.2  2002/10/16 16:43:48  dkrajzew
// regard of artifactsarising from traffic lights implemented; debugged
//
// Revision 1.1  2002/10/16 14:48:26  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.14  2002/10/15 10:24:30  roessel
// MSLane::MeanData constructor checks now for illegal intervals.
//
// Revision 1.13  2002/09/25 17:14:42  roessel
// MeanData calculation and output implemented.
//
// Revision 1.12  2002/07/31 17:33:00  roessel
// Changes since sourceforge cvs request.
//
// Revision 1.15  2002/07/26 11:10:50  dkrajzew
// unset myAllowedLanes when departing from a source debugged; gap on
// URGENT_LANECHANGE_WISH added
//
// Revision 1.14  2002/07/24 16:29:40  croessel
// New method isEmissionSuccess(), used by MSTriggeredSource.
//
// Revision 1.13  2002/07/08 05:07:49  dkrajzew
// MSVC++-compilation problems removed
//
// Revision 1.12  2002/07/03 15:48:40  croessel
// Implementation of findNeigh changed.
// Call to MSVehicle::move adapted to new signature.
// Emit methods use now MSVehicle::safeEmitGap instead of safeGap.
// decel2much uses now MSVehicle::hasSafeGap instead of safeGap.
//
// Revision 1.11  2002/06/20 11:14:13  dkrajzew
// False assertion corrected
//
// Revision 1.10  2002/06/19 15:12:00  croessel
// In moveExceptFirst: Check for timeheadway < deltaT situations.
//
// Revision 1.9  2002/05/29 17:06:03  croessel
// Inlined some methods. See the .icc files.
//
// Revision 1.8  2002/04/24 13:06:47  croessel
// Changed signature of void detectCollisions() to void detectCollisions(
// MSNet::Time )
//
// Revision 1.7  2002/04/18 14:51:11  croessel
// In setDriveRequests(): set gap to UINT_MAX instead of 0 for vehicles
// without a predecessor.
//
// Revision 1.6  2002/04/18 14:05:21  croessel
// Changed detectCollisions() output from pointer-address to vehicle-id.
//
// Revision 1.5  2002/04/17 14:44:32  croessel
// Forgot to reset visited lanes (see previous revision).
//
// Revision 1.4  2002/04/17 14:02:11  croessel
// Bugfix in setLookForwardState: myGap may be < 0 in the PRED_ON_SUCC
// state if pred just entered the succ-lane from another source lane. In
// this case the lane's first vehicle shouldn't leave this lane.
//
// Revision 1.3  2002/04/11 15:25:55  croessel
// Changed float to double.
//
// Revision 1.2  2002/04/11 12:32:07  croessel
// Added new lookForwardState "URGENT_LANECHANGE_WISH" for vehicles that
// may drive beyond the lane but are not on a lane that is linked to
// their next route-edge. A second succLink method, named succLinkSec was
// needed.
//
// Revision 1.1.1.1  2002/04/08 07:21:23  traffic
// new project name
//
// Revision 2.7  2002/03/27 12:11:09  croessel
// In constructor initialization of member myLastVeh added.
//
// Revision 2.6  2002/03/27 10:35:10  croessel
// moveExceptFirst(...): Last vehicle state must be buffered
// for multi-lane traffic too.
//
// Revision 2.5  2002/03/20 15:58:32  croessel
// Return to previous revision.
//
// Revision 2.3  2002/03/13 17:40:59  croessel
// Calculation of rearPos in setLookForwardState() fixed by introducing
// the new member myLastVeh in addition to myLastVehState. We need both,
// the state and the length of the vehicle.
//
// Revision 2.2  2002/03/13 16:56:35  croessel
// Changed the simpleOutput to XMLOutput by introducing nested classes
// XMLOut. Output is now indented.
//
// Revision 2.1  2002/03/06 15:57:25  croessel
// Modifications to setLookForwardState() so that overlapping vehicles
// from the succeeding lane won't cause collisions.
//
// Revision 2.0  2002/02/14 14:43:16  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.16  2002/02/13 10:05:26  croessel
// Front-gap calculation in emit() and emitTry() modified by usage of
// MSVehicleType::maxLength() and MSVehicleType::minDecel().
//
// Revision 1.15  2002/02/05 13:51:52  croessel
// GPL-Notice included.
// In *.cpp files also config.h included.
//
// Revision 1.14  2002/02/05 11:47:45  croessel
// Adaption of method-name-change in MSVehicle.
//
// Revision 1.13  2002/01/30 16:05:51  croessel
// Bug in detectCollisions() fixed. myVehicles must contain two or more
// vehicles to enter the for-loop.
//
// Revision 1.12  2002/01/24 18:30:40  croessel
// Changes due to renaming of MSVehicle's gap methods.
//
// Revision 1.11  2002/01/23 11:16:28  croessel
// emit() rewritten to avoid redundant code and make things clearer. This
// leads to some overloaded emitTry()'s and a new enoughSpace() method.
//
// Revision 1.10  2002/01/16 15:41:42  croessel
// New implementation of emit(). Should be collision free now. Introduced
// a new protected method "bool emitTry()" for convenience.
//
// Revision 1.9  2002/01/09 14:58:42  croessel
// Added MSLane::Link::setPriority( bool ) for StopLights to modify the
// link's priority. Used to implement the "green arrow".
//
// Revision 1.8  2001/12/19 16:29:31  croessel
// New std::-files included.
//
// Revision 1.7  2001/12/13 15:59:47  croessel
// In request(): Adaption due to new junction hierarchy.
//
// Revision 1.6  2001/12/06 13:13:25  traffic
// Junction types extracted into a separate class
//
// Revision 1.5  2001/11/21 15:21:01  croessel
// Bug fixes.
// Adapted setLookForwardState() to the new member myLastState and the
// MSVehicle::nextState to changed parameter list.
//
// Revision 1.4  2001/11/15 17:17:35  croessel
// Outcommented the inclusion of the inline *.iC files. Currently not needed.
// Vehicle-State introduced. Simulation is now independant of vehicle's
// speed. Still, the position is fundamental, also a gap between
// vehicles. But that's it.
//
// Revision 1.3  2001/11/14 15:47:34  croessel
// Merged the diffs between the .C and .cpp versions. Numerous changes
// in MSLane, MSVehicle and MSJunction.
//
// Revision 1.2  2001/11/14 10:49:06  croessel
// CR-line-end removed.
//
// Revision 1.1  2001/10/24 07:12:36  traffic
// new extension
//
// Revision 1.7  2001/10/23 09:31:00  traffic
// parser bugs removed
//
// Revision 1.5  2001/09/06 15:47:08  croessel
// Numerous changes during debugging session.
//
// Revision 1.4  2001/08/16 13:11:46  traffic
// minor MSVC++-problems solved
//
// Revision 1.3  2001/07/25 12:17:39  traffic
// CC problems with make_pair repaired
//
// Revision 1.2  2001/07/16 12:55:47  croessel
// Changed id type from unsigned int to string. Added string-pointer
// dictionaries and dictionary methods.
//
// Revision 1.1.1.1  2001/07/11 15:51:13  traffic
// new start
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <helpers/PreStartInitialised.h>
#include <utils/common/UtilExceptions.h>
#include "MSVehicle.h"
#include "MSVehicleType.h"
#include "MSEdge.h"
#include "MSJunction.h"
#include "MSLogicJunction.h"
#include "MSInductLoop.h"
#include "MSLink.h"
#include "MSLane.h"
#include <cmath>
#include <bitset>
#include <iostream>
#include <cassert>
#include <functional>
#include <algorithm>
#include <iterator>
#include <exception>
#include <climits>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static member definitions
 * ======================================================================= */
MSLane::DictType MSLane::myDict;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * methods of MSLane::PosGreater
 * ----------------------------------------------------------------------- */
MSLane::PosGreater::result_type
MSLane::PosGreater::operator() (first_argument_type veh1,
                                second_argument_type veh2) const
{
    return veh1->pos() > veh2->pos();
}


/* -------------------------------------------------------------------------
 * methods of MSLane
 * ----------------------------------------------------------------------- */
MSLane::~MSLane()
{
    for(MSLinkCont::iterator i=myLinks.begin(); i!=myLinks.end(); i++) {
        delete *i;
    }
    // TODO
}

/////////////////////////////////////////////////////////////////////////////

MSLane::MSLane( MSNet &net,
                string id,
                double maxSpeed,
                double length,
                MSEdge* edge
                )  :
    PreStartInitialised(net),
    myID( id ),
    myVehicles(),
    myMaxSpeed( maxSpeed ),
    myLength( length ),
    myEdge( edge ),
//    myNextJunction( 0 ),
    myVehBuffer( 0 ),
    myMeanData()
{
}

/////////////////////////////////////////////////////////////////////////////

void
MSLane::initialize( /*MSJunction* backJunction,*/
                    MSLinkCont* links )
{
//    myNextJunction = backJunction;
    myLinks = *links;
    delete links;
}

/////////////////////////////////////////////////////////////////////////////

void
MSLane::moveNonCriticalSingle()
{
    myFirstDistance = 100000;
    myApproaching = 0;
    // Buffer last vehicles state.
    if ( myVehicles.size() == 0 ) {
        myFirstUnsafe = myVehicles.size();
        myLastState = MSVehicle::State(myLength, 10000);
        return;
    }

        myLastState = (*myVehicles.begin())->myState;

        myFirstUnsafe = 0;
        // Move vehicles except first and all vehicles that may reach something
        //  that forces them to stop
        VehCont::iterator lastBeforeEnd = myVehicles.end() - 1;
        VehCont::iterator veh;
        for ( veh = myVehicles.begin();
              !(*veh)->reachingCritical(myLength) && veh != lastBeforeEnd;
              ++veh,myFirstUnsafe++ ) {

            VehCont::iterator pred = veh + 1;
            (*veh)->_lcAction = MSVehicle::LCA_STRAIGHT;
            ( *veh )->move( this, *pred, 0 );
            ( *veh )->meanDataMove();

	        // Check for timeheadway < deltaT
            MSVehicle *veh1 = *veh;
            MSVehicle *veh2 = *pred;
	    assert( ( *veh )->pos() < ( *pred )->pos() );
            assert( ( *veh )->pos() <= myLength );
	    }
}


void
MSLane::moveCriticalSingle()
{
    // Buffer last vehicles state.
    if ( myVehicles.size() == 0 ) {
        return;
    }
        VehCont::iterator lastBeforeEnd = myVehicles.end() - 1;
        VehCont::iterator veh;
      // Move all next vehicles beside the first
        for ( veh=myVehicles.begin()+myFirstUnsafe;veh != lastBeforeEnd;
              ++veh ) {

            VehCont::iterator pred = veh + 1;
            (*veh)->_lcAction = MSVehicle::LCA_STRAIGHT;
            ( *veh )->moveRegardingCritical( this, *pred, 0 );
            ( *veh )->meanDataMove();

	        // Check for timeheadway < deltaT
	        assert( ( *veh )->pos() < ( *pred )->pos() );
            assert( ( *veh )->pos() <= myLength );
    	}
        (*veh)->_lcAction = MSVehicle::LCA_STRAIGHT;
	    ( *veh )->moveRegardingCritical( this, 0, 0 );
	    ( *veh )->meanDataMove();
        assert( ( *veh )->pos() <= myLength );
}


void
MSLane::moveNonCriticalMulti()
{
    myFirstDistance = 100000;
    myApproaching = 0;
    // Buffer last vehicles state.
    if ( myVehicles.size() == 0 ) {
        myLastState = MSVehicle::State(myLength, 10000);
        myFirstUnsafe = myVehicles.size();
        return;
    }

        myLastState = (*myVehicles.begin())->myState;

        myFirstUnsafe = 0;
        // Move vehicles except first and all vehicles that may reach something
        //  that forces them to stop
        VehCont::iterator lastBeforeEnd = myVehicles.end() - 1;
        VehCont::iterator veh;
        for ( veh = myVehicles.begin();
              !(*veh)->reachingCritical(myLength) && veh != lastBeforeEnd;
              ++veh,myFirstUnsafe++ ) {

            VehCont::iterator pred = veh + 1;
            ( *veh )->move( this, *pred, 0 );
            ( *veh )->meanDataMove();

	        // Check for timeheadway < deltaT
            MSVehicle *veh1 = *veh;
            MSVehicle *veh2 = *pred;
	    assert( ( *veh )->pos() < ( *pred )->pos() );
            assert( ( *veh )->pos() <= myLength );
	    }
}

void
MSLane::moveCriticalMulti()
{
    // Buffer last vehicles state.
    if ( myVehicles.size() == 0 ) {
        return;
    }

        VehCont::iterator lastBeforeEnd = myVehicles.end() - 1;
        VehCont::iterator veh;
      // Move all next vehicles beside the first
        for ( veh=myVehicles.begin()+myFirstUnsafe;veh != lastBeforeEnd;
              ++veh ) {

            VehCont::iterator pred = veh + 1;
            ( *veh )->moveRegardingCritical( this, *pred, 0 );
            ( *veh )->meanDataMove();

	        // Check for timeheadway < deltaT
	        assert( ( *veh )->pos() < ( *pred )->pos() );
            assert( ( *veh )->pos() <= myLength );
    	}
	    ( *veh )->moveRegardingCritical( this, 0, 0 );
	    ( *veh )->meanDataMove();
        assert( ( *veh )->pos() <= myLength );
}

/////////////////////////////////////////////////////////////////////////////

void
MSLane::moveNonCriticalMulti( MSEdge::LaneCont::const_iterator firstNeighLane,
                         MSEdge::LaneCont::const_iterator lastNeighLane )
{
    myFirstDistance = 100000;
    myApproaching = 0;
    // Buffer last vehicles state.
    if ( myVehicles.size() == 0 ) {
        myLastState = MSVehicle::State(myLength, 10000);
        myFirstUnsafe = myVehicles.size();
        return;
    }

        myLastState = (*myVehicles.begin())->myState;


        myFirstUnsafe = 0;
        // Move vehicles except first and all vehicles that may reach something
        //  that forces them to stop
        VehCont::iterator lastBeforeEnd = myVehicles.end() - 1;
        VehCont::iterator veh;

        for ( veh = myVehicles.begin();
              !(*veh)->reachingCritical(myLength) && veh != lastBeforeEnd;
              ++veh,++myFirstUnsafe ) {

            VehCont::const_iterator pred( veh + 1 );
            const MSVehicle*
                neigh = findNeigh( *veh, firstNeighLane, lastNeighLane );
            // veh has neighbour to regard.
            if ( neigh != *veh ) {

                ( *veh )->move( this, *pred, neigh );
            }

            // veh has no neighbour to regard.
            else {

                ( *veh )->move( this, *pred, 0);
            }

            ( *veh )->meanDataMove();
            // Check for timeheadway < deltaT
            MSVehicle *vehicle = (*veh);
            MSVehicle *predec = (*pred);
            assert( ( *veh )->pos() < ( *pred )->pos() );
            assert( ( *veh )->pos() <= myLength );
        }
}


void
MSLane::moveCriticalMulti( MSEdge::LaneCont::const_iterator firstNeighLane,
                         MSEdge::LaneCont::const_iterator lastNeighLane )
{
    // Buffer last vehicles state.
    if ( myVehicles.size() == 0 ) {
        return;
    }

        VehCont::iterator lastBeforeEnd = myVehicles.end() - 1;
        VehCont::iterator veh;
      // Move all next vehicles beside the first
        for ( veh=myVehicles.begin()+myFirstUnsafe;veh != lastBeforeEnd;
              ++veh ) {

            VehCont::const_iterator pred( veh + 1 );
            const MSVehicle*
                neigh = findNeigh( *veh, firstNeighLane, lastNeighLane );

            // veh has neighbour to regard.
            if ( neigh != *veh ) {

                ( *veh )->moveRegardingCritical( this, *pred, neigh );
            }

            // veh has no neighbour to regard.
            else {

                ( *veh )->moveRegardingCritical( this, *pred, 0);
            }

            ( *veh )->meanDataMove();
            // Check for timeheadway < deltaT
            assert( ( *veh )->pos() < ( *pred )->pos() );
            assert( ( *veh )->pos() <= myLength );
        }
	( *veh )->moveRegardingCritical( this, 0, 0 );
	( *veh )->meanDataMove();
            assert( ( *veh )->pos() <= myLength );

}

/////////////////////////////////////////////////////////////////////////////

void
MSLane::detectCollisions( MSNet::Time timestep ) const
{
    if ( myVehicles.size() < 2 ) {
        return;
    }

    VehCont::const_iterator lastVeh = myVehicles.end() - 1;
    for ( VehCont::const_iterator veh = myVehicles.begin();
          veh != lastVeh; ++veh ) {

        VehCont::const_iterator pred = veh + 1;
        double gap = ( *pred )->pos() - ( *pred )->length() - ( *veh )->pos();
        if ( gap < 0 ) {
            cerr << "MSLane::detectCollision: Collision of " << ( *veh )->id()
                 << " with " << ( *pred )->id() << " on MSLane " << myID
                 << " during timestep " << timestep << endl;
            cerr << ( *veh )->id() << ":" << ( *veh )->pos() << ", " << ( *veh )->speed() << endl;
            cerr << ( *pred )->id() << ":" << ( *pred )->pos() << ", " << ( *pred )->speed() << endl;
            throw ProcessError();
        }
    }
}

/////////////////////////////////////////////////////////////////////////////

bool
MSLane::emit( MSVehicle& veh )
{
    // If this lane is empty, set newVeh on position beyond safePos =
    // brakeGap(laneMaxSpeed) + MaxVehicleLength. (in the hope of that
    // the precening lane hasn't a much higher MaxSpeed)
    // This safePos is ugly, but we will live with it in this revision.
    double safePos = pow( myMaxSpeed, 2 ) / ( 2 * MSVehicleType::minDecel() ) +
                    MSVehicle::tau() + MSVehicleType::maxLength();
//    assert( safePos < myLength ); // Lane has to be longer than safePos,
    // otherwise emission (this kind of emission) makes no sense.

    // Here the emission starts
    if ( empty() ) {

        return emitTry( veh );
    }

    // Try to emit as last veh. (in driving direction)
    VehCont::iterator leaderIt = myVehicles.begin();
    if ( emitTry( veh, leaderIt ) ) {

        return true;
    }

    // if there is only one veh on this lane, try to
    // emit in front of this veh. (leader becomes follower)
    if ( leaderIt + 1 ==  myVehicles.end() ) {

        return emitTry( leaderIt, veh );
    }

    // At least two vehicles on lane.
    // iterate over follow/leader -pairs
    VehCont::iterator followIt = leaderIt;
    ++leaderIt;
    for (;;) {

        // try to emit between follower and leader
        if ( emitTry( followIt, veh, leaderIt ) ) {

            return true;
        }

        // if leader is the first veh on this lane, try
        // to emit in front of it.
        if ( leaderIt + 1 == myVehicles.end() ) {

            return emitTry( leaderIt, veh );
        }

        // iterate
        ++leaderIt;
        ++followIt;
    }
}

/////////////////////////////////////////////////////////////////////////////

bool
MSLane::isEmissionSuccess( MSVehicle* aVehicle )
{
//    aVehicle->departLane();
    MSLane::VehCont::iterator predIt =
        find_if( myVehicles.begin(), myVehicles.end(),
                 bind2nd( VehPosition(), aVehicle->pos() ) );

    if ( predIt != myVehicles.end() ) {

        MSVehicle* pred = *predIt;
        if ( ! aVehicle->isInsertTimeHeadWayCond( *pred ) ||
             ! aVehicle->isInsertBrakeCond( *pred ) ) {

            return false;
        }

        // emit
        aVehicle->enterLaneAtEmit( this );
        myVehicles.insert( predIt, aVehicle );
        return true;
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////

bool
MSLane::emitTry( MSVehicle& veh )
{
    // on sources, no vehicles may arrive from the back
    if(myEdge->isSource()) {
        veh.enterLaneAtEmit( this );
        myVehicles.push_front( &veh );

#ifdef ABS_DEBUG
	if(MSNet::searched1==veh.id()||MSNet::searched2==veh.id()) {
	    cout << "Using emitTry( MSVehicle& veh )/1" << endl;
	}
#endif

        return true;
    }
    // check for
    // empty lane emission.
    double safeSpace = 2 * pow( myMaxSpeed, 2 ) /
                      ( 2 * MSVehicleType::minDecel() ) +
                      MSVehicle::tau() +
                      veh.length(); // !!!
    if ( enoughSpace( veh, 0, myLength, safeSpace ) ) {
        veh.enterLaneAtEmit( this );
        myVehicles.push_front( &veh );

#ifdef ABS_DEBUG
	if(MSNet::searched1==veh.id()||MSNet::searched2==veh.id()) {
	    cout << "Using emitTry( MSVehicle& veh )/2" << endl;
	}
#endif

        return true;
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////

bool
MSLane::emitTry( MSVehicle& veh, VehCont::iterator leaderIt )
{
    // emission as last car (in driving direction)
    MSVehicle *leader = *leaderIt;
    double leaderPos = leader->pos() - leader->length();
    double safeSpace = 2 * pow( myMaxSpeed, 2 ) /
                      ( 2 * MSVehicleType::minDecel() ) +
                      MSVehicle::tau() +
                      veh.length() + veh.accelDist(); // !!!
    if ( enoughSpace( veh, 0, leaderPos, safeSpace ) ) {
        veh.enterLaneAtEmit( this );
        myVehicles.push_front( &veh );

#ifdef ABS_DEBUG
	if(MSNet::searched1==veh.id()||MSNet::searched2==veh.id()) {
	    cout << "Using emitTry( MSVehicle& veh, VehCont::iterator leaderIt )/2" << endl;
	}
#endif

        return true;
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////

bool
MSLane::emitTry( VehCont::iterator followIt, MSVehicle& veh )
{
    // emission as first car (in driving direction)
    double followPos = (*followIt)->pos();
    double safeSpace = (*followIt)->vaccel(this) * MSNet::deltaT() +
        (*followIt)->rigorousBrakeGap((*followIt)->vaccel(this))
        + veh.length();
    if ( enoughSpace( veh, followPos,
                      myLength - MSVehicleType::maxLength(), safeSpace ) ) {
        veh.enterLaneAtEmit( this );
        myVehicles.push_back( &veh );

#ifdef ABS_DEBUG
	if(MSNet::searched1==veh.id()||MSNet::searched2==veh.id()) {
	    cout << "Using emitTry( VehCont::iterator followIt, MSVehicle& veh )" << endl;
	}
#endif

        return true;
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////

bool
MSLane::emitTry( VehCont::iterator followIt, MSVehicle& veh,
                 VehCont::iterator leaderIt )
{
    MSVehicle *leader = *leaderIt;
    MSVehicle *follow = *followIt;
    // emission between follower and leader.
    double followPos = (*followIt)->pos();
    double leaderPos = (*leaderIt)->pos() - (*leaderIt)->length();
    double safeSpace = (*followIt)->vaccel(this) * MSNet::deltaT() +
        (*followIt)->rigorousBrakeGap((*followIt)->vaccel(this))
        + veh.length();
    if ( enoughSpace( veh, followPos, leaderPos, safeSpace ) ) {
        veh.enterLaneAtEmit( this );
        myVehicles.insert( leaderIt, &veh );

#ifdef ABS_DEBUG
	if(MSNet::searched1==veh.id()||MSNet::searched2==veh.id()) {
	    cout << "Using emitTry( VehCont::iterator followIt, MSVehicle& veh,VehCont::iterator leaderIt )" << endl;
	}
#endif

        return true;
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////

bool
MSLane::enoughSpace( MSVehicle& veh,
                     double followPos, double leaderPos, double safeSpace )
{
    double free = leaderPos - followPos - safeSpace;
    if ( free >= 0.01 ) {

        // prepare vehicle with it's position
        MSVehicle::State state;
        state.setPos( followPos + safeSpace + free / 2 );
        veh.moveSetState( state );
        return true;
    }
    return false;
}


void
MSLane::setCritical()
{
    for(VehCont::iterator i=myVehicles.begin() + myFirstUnsafe; i!=myVehicles.end(); i++) {
	    (*i)->moveFirstChecked();
        MSLane *target = (*i)->getTargetLane();
        if(target!=this) {
            target->push(pop());
            return;
        }
        (*i)->_assertPos();
    }
}

/////////////////////////////////////////////////////////////////////////////

bool
MSLane::dictionary(string id, MSLane* ptr)
{
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        myDict.insert(DictType::value_type(id, ptr));
        return true;
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////

MSLane*
MSLane::dictionary(string id)
{
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        return 0;
    }
    return it->second;
}

/////////////////////////////////////////////////////////////////////////////

void
MSLane::clear()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete (*i).second;
    }
    myDict.clear();
}

/////////////////////////////////////////////////////////////////////////////

bool
MSLane::push(MSVehicle* veh)
{
#ifdef ABS_DEBUG
    if(myVehBuffer!=0) {
	    cout << "Push Failed on Lane:" << myID << endl;
	    cout << myVehBuffer->id() << ", " << myVehBuffer->pos() << ", " << myVehBuffer->speed() << endl;
	    cout << veh->id() << ", " << veh->pos() << ", " << veh->speed() << endl;
    }
#endif

    // Insert vehicle only if it's destination isn't reached.
    assert( myVehBuffer == 0 );
    if ( ! veh->destReached( myEdge ) ) { // adjusts vehicles routeIterator
        myVehBuffer = veh;
        veh->enterLaneAtMove( this );
        veh->_assertPos();
        return false;
    }
    else {
        MSVehicle::remove(veh->id());
        return true;
        // TODO
        // This part has to be discussed, quick an dirty solution:
        // Destination reached. Vehicle vanishes.
        // maybe introduce a vehicle state ...
    }
}

/////////////////////////////////////////////////////////////////////////////

MSVehicle*
MSLane::pop()
{
    assert( ! myVehicles.empty() );
    MSVehicle* first = myVehicles.back( );
    first->leaveLaneAtMove();
    myVehicles.pop_back();
    return first;
}


bool
MSLane::appropriate(const MSVehicle *veh)
{
    MSLinkCont::iterator link = succLinkSec( *veh, 1, *this );
    return ( link != myLinks.end() );
}

//////////////////////////////////////////////////////////////////////////

void
MSLane::integrateNewVehicle()
{
    if ( myVehBuffer ) {
        myVehicles.push_front( myVehBuffer );
        myVehBuffer = 0;
    }
}

/////////////////////////////////////////////////////////////////////////////
/*
MSLinkCont::const_iterator
MSLane::succLink(const MSVehicle& veh, unsigned int nRouteSuccs,
                 const MSLane& succLinkSource) const
{
    const MSEdge* nRouteEdge = veh.succEdge( nRouteSuccs );
    assert( nRouteEdge != 0 );

    MSLinkCont::const_iterator ret = succLinkOneLane(nRouteEdge, succLinkSource);
    if(ret!=succLinkSource.myLinks.end()) {
        return ret;
    }
    // Check which link's lane belongs to the nRouteEdge.
    return succLinkSource.myEdge->succLink(nRouteEdge, succLinkSource);
}
*/
/////////////////////////////////////////////////////////////////////////////

MSLinkCont::const_iterator
MSLane::succLinkOneLane(const MSEdge* nRouteEdge,
                        const MSLane& succLinkSource) const
{
    // the link must be from a lane to the right or left from the current lane
    //  we have to do it via the edge
    for ( MSLinkCont::const_iterator link = succLinkSource.myLinks.begin();
          link != succLinkSource.myLinks.end() ; ++link ) {

        if ( ( *link )->myLane!=0 && ( *link )->myLane->myEdge == nRouteEdge ) {
            return link;
        }
    }
    return succLinkSource.myLinks.end();
}

/////////////////////////////////////////////////////////////////////////////

bool
MSLane::isLinkEnd(MSLinkCont::const_iterator &i) const
{
    return i==myLinks.end();
}

bool
MSLane::isLinkEnd(MSLinkCont::iterator &i)
{
    return i==myLinks.end();
}

/////////////////////////////////////////////////////////////////////////////

bool
MSLane::inEdge(const MSEdge *edge) const
{
    return myEdge==edge;
}

/////////////////////////////////////////////////////////////////////////////

const MSVehicle * const
MSLane::getLastVehicle() const
{
    if(myVehicles.size()==0) {
        return 0;
    }
    return *myVehicles.begin();
}

/////////////////////////////////////////////////////////////////////////////

MSLinkCont::iterator
MSLane::succLinkSec(const MSVehicle& veh, unsigned int nRouteSuccs,
                    MSLane& succLinkSource)
{
    const MSEdge* nRouteEdge = veh.succEdge( nRouteSuccs );
    assert( nRouteEdge != 0 );
    // the link must be from a lane to the right or left from the current lane
    //  we have to do it via the edge
    for ( MSLinkCont::iterator link = succLinkSource.myLinks.begin();
          link != succLinkSource.myLinks.end() ; ++link ) {

        if ( ( *link )->myLane!=0 && ( *link )->myLane->myEdge == nRouteEdge ) {
            return link;
        }
    }
    return succLinkSource.myLinks.end();
}


/////////////////////////////////////////////////////////////////////////////

const MSVehicle*
MSLane::findNeigh( MSVehicle* veh,
                   MSEdge::LaneCont::const_iterator first,
                   MSEdge::LaneCont::const_iterator last )
{
    MSVehicle* neighbour = veh;
    double vNeighEqual( 0 );

    for ( MSEdge::LaneCont::const_iterator neighLane = first;
          neighLane != last; ++neighLane ) {

        VehCont::const_iterator tmpNeighbour =
            find_if( ( *neighLane )->myVehicles.begin(),
                     ( *neighLane )->myVehicles.end(),
                     bind2nd( PosGreater(), veh ) );
        if ( tmpNeighbour == ( *neighLane )->myVehicles.end() ) {

            continue;
        }

        // neighbour found
        if ( (*tmpNeighbour)-> congested() ) {
            continue;
        }

        double tmpVNeighEqual = veh->vNeighEqualPos( **tmpNeighbour );
        if ( neighbour == veh || tmpVNeighEqual < vNeighEqual ) {

            neighbour   = *tmpNeighbour;
            vNeighEqual = tmpVNeighEqual;
        }
    }
    return neighbour;
}

/////////////////////////////////////////////////////////////////////////////

void
MSLane::resetMeanData( unsigned index )
{
    assert(index<myMeanData.size());
    MeanDataValues& md = myMeanData[ index ];
    md.nVehFinishedLane = 0;
    md.nVehContributed = 0;
    md.contTimestepSum = 0;
    md.discreteTimestepSum = 0;
    md.distanceSum = 0;
    md.speedSum = 0;
}

/////////////////////////////////////////////////////////////////////////////

void
MSLane::addVehicleData( double contTimesteps,
                        unsigned discreteTimesteps,
                        double travelDistance,
                        double speedSum,
                        double speedSquareSum,
                        unsigned index,
                        bool hasFinishedLane )
{
    assert(index<myMeanData.size());
    MeanDataValues& md = myMeanData[ index ];

    md.nVehFinishedLane     += hasFinishedLane;
    md.nVehContributed      += 1;
    md.contTimestepSum      += contTimesteps;
    md.discreteTimestepSum  += discreteTimesteps;
    md.distanceSum          += travelDistance;
    md.speedSum             += speedSum;
    md.speedSquareSum       += speedSquareSum;
}

/////////////////////////////////////////////////////////////////////////////

void
MSLane::collectVehicleData( unsigned index )
{
    for ( VehCont::iterator it = myVehicles.begin();
          it != myVehicles.end(); ++it ) {

        (*it)->dumpData( index );
    }
}


/////////////////////////////////////////////////////////////////////////////

ostream&
operator<<( ostream& os, const MSLane& lane )
{
    os << "MSLane: Id = " << lane.myID << endl;
    return os;
}

/////////////////////////////////////////////////////////////////////////////

MSLane::XMLOut::XMLOut( const MSLane& obj,
                        unsigned indentWidth,
                        bool withChildElemes ) :
    myObj( obj ),
    myIndentWidth( indentWidth ),
    myWithChildElemes( withChildElemes )
{
}

/////////////////////////////////////////////////////////////////////////////

ostream&
operator<<( ostream& os, const MSLane::XMLOut& obj )
{
    string indent( obj.myIndentWidth , ' ' );

    if ( obj.myWithChildElemes ) {

        if ( obj.myObj.myVehicles.empty() == true &&
             obj.myObj.myVehBuffer == 0 ) {

            os << indent << "<lane id=\"" << obj.myObj.myID
               << "\"/>" << endl;
        }
        else { // not empty
            os << indent << "<lane id=\"" << obj.myObj.myID << "\">"
               << endl;

            if ( obj.myObj.myVehBuffer != 0 ) {

                os << MSVehicle::XMLOut( *(obj.myObj.myVehBuffer),
                                         obj.myIndentWidth + 4,
                                         true );
            }

            for ( MSLane::VehCont::const_iterator veh =
                      obj.myObj.myVehicles.begin();
                  veh != obj.myObj.myVehicles.end(); ++veh ) {

                os << MSVehicle::XMLOut( **veh, obj.myIndentWidth + 4,
                                         false );
            }

            os << indent << "</lane>" << endl;

        }
    }

    else { // no child elemets, just print the number of vehicles

        unsigned nVeh = obj.myObj.myVehicles.size() +
                        ( obj.myObj.myVehBuffer != 0 );
        os << indent << "<lane id=\"" << obj.myObj.myID << "\" nVehicles=\""
           << nVeh << "\" />" << endl;
    }

    return os;
}

/////////////////////////////////////////////////////////////////////////////

MSLane::MeanData::MeanData( const MSLane& obj,
                            unsigned index,
                            MSNet::Time interval ) :
    myObj( obj ),
    myIndex( index ),
    myInterval( interval )
{
    if ( myInterval == 0 ){
        cerr << "MSLane::MeanData constructor: interval = 0, should be > 0.\n"
             << "I will set it to 5 minutes.\n";
        myInterval = static_cast<unsigned>( 300 / MSNet::deltaT() );
    }
}

/////////////////////////////////////////////////////////////////////////////

ostream&
operator<<( ostream& os, const MSLane::MeanData& obj )
{
    const double meanVehLength = 7.5;
    const MSLane& lane = obj.myObj;
    assert(lane.myMeanData.size()>obj.myIndex);
    const MSLane::MeanDataValues& meanData = lane.myMeanData[ obj.myIndex ];

    const_cast< MSLane& >( lane ).collectVehicleData( obj.myIndex );

    // calculate mean data
    double travelTime;
    double meanSpeed;
    double meanDensity;
    double meanFlow;

    if ( meanData.nVehContributed > 0 ) {

        if(meanData.distanceSum!=0) {
            travelTime  = ( meanData.contTimestepSum *
                            static_cast< double >( MSNet::deltaT() ) ) /
                ( meanData.distanceSum / lane.myLength );
        } else {
            travelTime = 0;
        }

        meanSpeed   = meanData.speedSum /
            static_cast< double >( meanData.discreteTimestepSum );

        meanDensity = static_cast< double >
            ( meanData.discreteTimestepSum * MSNet::deltaT() ) /
            ( lane.myLength / meanVehLength *
              static_cast< double >( obj.myInterval * MSNet::deltaT() ) );

        meanFlow    = static_cast< double >( meanData.nVehFinishedLane ) /
            static_cast< double >( obj.myInterval );
    }
    else {

        assert( meanData.nVehContributed == 0 );
        travelTime  = lane.myLength / lane.myMaxSpeed;
        meanSpeed   = -1;
        meanDensity = 0;
        meanFlow    = 0;
    }

    os << "      <lane id=\""      << obj.myObj.myID
       << "\" traveltime=\"" << travelTime
       << "\" speed=\""      << meanSpeed
       << "\" density=\""    << meanDensity
       << "\" flow=\""       << meanFlow
       << "\" noVehicles=\"" << meanData.nVehContributed
       << "\"/>" << endl;

    const_cast< MSLane& >( lane ).resetMeanData( obj.myIndex );

    return os;
}

/////////////////////////////////////////////////////////////////////////////

void
MSLane::setLinkPriorities(const std::bitset<64> &prios, size_t &beginPos)
{
    for(MSLinkCont::iterator i=myLinks.begin(); i!=myLinks.end(); i++) {
        (*i)->setPriority(prios.test(beginPos++));
    }
}

/////////////////////////////////////////////////////////////////////////////

const MSLinkCont &
MSLane::getLinkCont() const
{
    return myLinks;
}

/////////////////////////////////////////////////////////////////////////////

void
MSLane::init(MSNet &net)
{
    myMeanData.clear();
    size_t noIntervals = net.getNDumpIntervalls();
    if(net.withGUI()) {
        noIntervals++;
    }
    myMeanData.insert( myMeanData.end(), noIntervals, MeanDataValues() );
}


const std::string &
MSLane::id() const
{
    return myID;
}



void
MSLane::releaseVehicles()
{
}



const MSLane::VehCont &
MSLane::getVehiclesSecure()
{
    return myVehicles;
}


void
MSLane::swapAfterLaneChange()
{
    myVehicles = myTmpVehicles;
    myTmpVehicles.clear();
}


void
MSLane::setApproaching(double dist, MSVehicle *veh)
{
    myFirstDistance = dist;
    myApproaching = veh;
}


MSLane::VehCont::const_iterator 
MSLane::findNextVehicleByPosition(double pos) const
{
    assert(pos<myLength);
    // returns if no vehicle is available
    if(myVehicles.size()==0) {
        return myVehicles.end();
    }
    // some kind of a binary search
    size_t off1 = 0;
    size_t off2 = myVehicles.size() - 1;
    while(true) {
        size_t middle = (off1+off2)/2;
        MSVehicle *v1 = myVehicles[middle];
        if(v1->pos()>pos) {
            off2 = middle;
        } else if(v1->pos()<pos) {
            off1 = middle;
        }
        if(off1==off2) {
            return myVehicles.begin() + off1;
        }
    }
}


MSLane::VehCont::const_iterator 
MSLane::findPrevVehicleByPosition(const VehCont::const_iterator &beginAt,
                                  double pos) const
{
    assert(pos<myLength);
    // returns if no vehicle is available
    if(myVehicles.size()==0) {
        return myVehicles.end();
    }
    // some kind of a binary search
    size_t off1 = distance(myVehicles.begin(), beginAt);
    size_t off2 = myVehicles.size() - 1;
    while(true) {
        size_t middle = (off1+off2)/2;
        MSVehicle *v1 = myVehicles[middle];
        if(v1->pos()>pos) {
            off2 = middle;
        } else if(v1->pos()<pos) {
            off1 = middle;
        }
        if(off1==off2) {
            // there may be no vehicle before
            if(off1==0) {
                return myVehicles.end();
            }
            off1--;
            return myVehicles.begin() + off1;
        }
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#ifdef DISABLE_INLINE
#include "MSLane.icc"
#endif

// Local Variables:
// mode:C++
// End:
