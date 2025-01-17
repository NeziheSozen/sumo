/****************************************************************************/
/// @file    NIVissimDisturbance.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
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


#include <map>
#include <string>
#include <iostream>
#include <cassert>
#include <utils/common/ToString.h>
#include <utils/common/MsgHandler.h>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/Boundary.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNodeCont.h>
#include "NIVissimEdge.h"
#include "NIVissimConnection.h"
#include "NIVissimNodeDef.h"
#include "NIVissimDisturbance.h"
#include "NIVissimNodeParticipatingEdgeVector.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS
// ===========================================================================
// used namespaces
// ===========================================================================

using namespace std;

NIVissimDisturbance::DictType NIVissimDisturbance::myDict;
int NIVissimDisturbance::myRunningID = 100000000;

int NIVissimDisturbance::refusedProhibits = 0;


NIVissimDisturbance::NIVissimDisturbance(int id,
        const std::string &name,
        const NIVissimExtendedEdgePoint &edge,
        const NIVissimExtendedEdgePoint &by,
        SUMOReal timegap, SUMOReal waygap,
        SUMOReal vmax)
        : myID(id), myNode(-1), myName(name), myEdge(edge), myDisturbance(by),
        myTimeGap(timegap), myWayGap(waygap), myVMax(vmax) {}


NIVissimDisturbance::~NIVissimDisturbance() {}



bool
NIVissimDisturbance::dictionary(int id,
                                const std::string &name,
                                const NIVissimExtendedEdgePoint &edge,
                                const NIVissimExtendedEdgePoint &by,
                                SUMOReal timegap, SUMOReal waygap, SUMOReal vmax) {
    int nid = myRunningID++;
    NIVissimDisturbance *o =
        new NIVissimDisturbance(nid, name, edge, by, timegap, waygap, vmax);
    if (!dictionary(nid, o)) {
        delete o;
    }
    return true;
}


bool
NIVissimDisturbance::dictionary(int id, NIVissimDisturbance *o) {
    DictType::iterator i=myDict.find(id);
    if (i==myDict.end()) {
        myDict[id] = o;
        return true;
    }
    return false;
}


NIVissimDisturbance *
NIVissimDisturbance::dictionary(int id) {
    DictType::iterator i=myDict.find(id);
    if (i==myDict.end()) {
        return 0;
    }
    return (*i).second;
}

IntVector
NIVissimDisturbance::getWithin(const AbstractPoly &poly) {
    IntVector ret;
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        if ((*i).second->crosses(poly)) {
            ret.push_back((*i).second->myID);
        }
    }
    return ret;
}


void
NIVissimDisturbance::computeBounding() {
    assert(myBoundary==0);
    Boundary *bound = new Boundary();
    if (NIVissimAbstractEdge::dictionary(myEdge.getEdgeID())!=0) {
        bound->add(myEdge.getGeomPosition());
    }
    if (NIVissimAbstractEdge::dictionary(myDisturbance.getEdgeID())!=0) {
        bound->add(myDisturbance.getGeomPosition());
    }
    myBoundary = bound;
    assert(myBoundary!=0&&myBoundary->xmax()>=myBoundary->xmin());
}



bool
NIVissimDisturbance::addToNode(NBNode *node, NBDistrictCont &dc,
                               NBNodeCont &nc, NBEdgeCont &ec) {
    myNode = 0;
    NIVissimConnection *pc =
        NIVissimConnection::dictionary(myEdge.getEdgeID());
    NIVissimConnection *bc =
        NIVissimConnection::dictionary(myDisturbance.getEdgeID());
    if (pc==0 && bc==0) {
        // This has not been tested completely, yet
        // Both competing abstract edges are normal edges
        // We have to find a crossing point, build a node here,
        //  split both edges and add the connections
        NIVissimEdge *e1 = NIVissimEdge::dictionary(myEdge.getEdgeID());
        NIVissimEdge *e2 = NIVissimEdge::dictionary(myDisturbance.getEdgeID());
        WRITE_WARNING("Ugly split to prohibit '" + toString<int>(e1->getID())+ "' by '" + toString<int>(e2->getID())+ "'.");
        Position2D pos = e1->crossesEdgeAtPoint(e2);
        std::string id1 = toString<int>(e1->getID()) + "x" + toString<int>(e2->getID());
        std::string id2 = toString<int>(e2->getID()) + "x" + toString<int>(e1->getID());
        NBNode *node1 = nc.retrieve(id1);
        NBNode *node2 = nc.retrieve(id2);
        NBNode *node = 0;
        assert(node1==0||node2==0);
        if (node1==0&&node2==0) {
            refusedProhibits++;
            return false;
            /*            node = new NBNode(id1, pos.x(), pos.y(), "priority");
                        if(!myNodeCont.insert(node)) {
                             "nope, NIVissimDisturbance" << endl;
                            throw 1;
                        }*/
        } else {
            node = node1==0 ? node2 : node1;
        }
        ec.splitAt(dc,
                   ec.retrievePossiblySplitted(
                       toString<int>(e1->getID()), myEdge.getPosition()),
                   node);
        ec.splitAt(dc,
                   ec.retrievePossiblySplitted(
                       toString<int>(e2->getID()), myDisturbance.getPosition()),
                   node);
        // !!! in some cases, one of the edges is not being build because it's too short
        // !!! what to do in these cases?
        NBEdge *mayDriveFrom = ec.retrieve(toString<int>(e1->getID()) + "[0]");
        NBEdge *mayDriveTo = ec.retrieve(toString<int>(e1->getID()) + "[1]");
        NBEdge *mustStopFrom = ec.retrieve(toString<int>(e2->getID()) + "[0]");
        NBEdge *mustStopTo = ec.retrieve(toString<int>(e2->getID()) + "[1]");
        if (mayDriveFrom!=0&&mayDriveTo!=0&&mustStopFrom!=0&&mustStopTo!=0) {
            node->addSortedLinkFoes(
                NBConnection(mayDriveFrom, mayDriveTo),
                NBConnection(mayDriveFrom, mayDriveTo));
        } else {
            refusedProhibits++;
            return false;
            // !!! warning
        }
//        }
    } else if (pc!=0 && bc==0) {
        // The prohibited abstract edge is a connection, the other
        //  is not;
        // The connection will be prohibitesd by all connections
        //  outgoing from the "real" edge

        NBEdge *e = ec.retrievePossiblySplitted(toString<int>(myDisturbance.getEdgeID()), myDisturbance.getPosition());
        if (e==0) {
            WRITE_WARNING("Could not prohibit '"+ toString<int>(myEdge.getEdgeID()) + "' by '" + toString<int>(myDisturbance.getEdgeID())+ "'. Have not found disturbance.");
            refusedProhibits++;
            return false;
        }
        if (e->getFromNode()==e->getToNode()) {
            WRITE_WARNING("Could not prohibit '"+ toString<int>(myEdge.getEdgeID()) + "' by '" + toString<int>(myDisturbance.getEdgeID())+ "'. Disturbance connects same node.");
            refusedProhibits++;
            // What to do with dummy edges?
            return false;
        }
        // get the begin of the prohibited connection
        std::string id_pcoe = toString<int>(pc->getFromEdgeID());
        std::string id_pcie = toString<int>(pc->getToEdgeID());
        NBEdge *pcoe = ec.retrievePossiblySplitted(id_pcoe, id_pcie, true);
        NBEdge *pcie = ec.retrievePossiblySplitted(id_pcie, id_pcoe, false);
        // check whether it's ending node is the node the prohibited
        //  edge end at
        if (pcoe!=0&&pcie!=0&&pcoe->getToNode()==e->getToNode()) {
            // if so, simply prohibit the connections
            NBNode *node = e->getToNode();
            const EdgeVector &connected = e->getConnectedEdges();
            for (EdgeVector::const_iterator i=connected.begin(); i!=connected.end(); i++) {
                node->addSortedLinkFoes(
                    NBConnection(e, *i),
                    NBConnection(pcoe, pcie));
            }
        } else {
            WRITE_WARNING("Would have to split edge '" + e->getID() + "' to build a prohibition");
            refusedProhibits++;
            // quite ugly - why was it not build?
            return false;
            /*
            std::string nid1 = e->getID() + "[0]";
            std::string nid2 = e->getID() + "[1]";

            if(ec.splitAt(e, node)) {
                node->addSortedLinkFoes(
                        NBConnection(
                            ec.retrieve(nid1),
                            ec.retrieve(nid2)
                        ),
                        getConnection(node, myEdge.getEdgeID())
                    );
            }
            */
        }
    } else if (bc!=0 && pc==0) {
        // The prohibiting abstract edge is a connection, the other
        //  is not;
        // We have to split the other one and add the prohibition
        //  description

        NBEdge *e = ec.retrievePossiblySplitted(
                        toString<int>(myEdge.getEdgeID()), myEdge.getPosition());
        if (e==0) {
            WRITE_WARNING("Could not prohibit '" + toString<int>(myEdge.getEdgeID()) + "' - it was not built.");
            return false;
        }
        std::string nid1 = e->getID() + "[0]";
        std::string nid2 = e->getID() + "[1]";
        if (e->getFromNode()==e->getToNode()) {
            WRITE_WARNING("Could not prohibit '" + toString<int>(myEdge.getEdgeID()) + "' by '" + toString<int>(myDisturbance.getEdgeID())+ "'.");
            refusedProhibits++;
            // What to do with dummy edges?
            return false;
        }
        // get the begin of the prohibiting connection
        std::string id_bcoe = toString<int>(bc->getFromEdgeID());
        std::string id_bcie = toString<int>(bc->getToEdgeID());
        NBEdge *bcoe = ec.retrievePossiblySplitted(id_bcoe, id_bcie, true);
        NBEdge *bcie = ec.retrievePossiblySplitted(id_bcie, id_bcoe, false);
        // check whether it's ending node is the node the prohibited
        //  edge end at
        if (bcoe!=0&&bcie!=0&&bcoe->getToNode()==e->getToNode()) {
            // if so, simply prohibit the connections
            NBNode *node = e->getToNode();
            const EdgeVector &connected = e->getConnectedEdges();
            for (EdgeVector::const_iterator i=connected.begin(); i!=connected.end(); i++) {
                node->addSortedLinkFoes(
                    NBConnection(bcoe, bcie),
                    NBConnection(e, *i));
            }
        } else {
            WRITE_WARNING("Would have to split edge '" + e->getID() + "' to build a prohibition");
            refusedProhibits++;
            return false;
            /*
            // quite ugly - why was it not build?
            if(ec.splitAt(e, node)) {
                node->addSortedLinkFoes(
                        getConnection(node, myDisturbance.getEdgeID()),
                        NBConnection(
                            ec.retrieve(nid1),
                            ec.retrieve(nid2)
                        )
                    );
            }
            */
        }
    } else {
        // both the prohibiting and the prohibited abstract edges
        //  are connections
        // We can retrieve the conected edges and add the desription
        NBConnection conn1 = getConnection(node, myDisturbance.getEdgeID());
        NBConnection conn2 = getConnection(node, myEdge.getEdgeID());
        if (!conn1.check(ec)||!conn2.check(ec)) {
            refusedProhibits++;
            return false;
        }
        node->addSortedLinkFoes(conn1, conn2);
    }
    return true;
}


NBConnection
NIVissimDisturbance::getConnection(NBNode *node, int aedgeid) {
    if (NIVissimEdge::dictionary(myEdge.getEdgeID())==0) {
        NIVissimConnection *c = NIVissimConnection::dictionary(aedgeid);
        NBEdge *from =
            node->getPossiblySplittedIncoming(toString<int>(c->getFromEdgeID()));
        NBEdge *to =
            node->getPossiblySplittedOutgoing(toString<int>(c->getToEdgeID()));

        // source is a connection
        return NBConnection(toString<int>(c->getFromEdgeID()), from,
                            toString<int>(c->getToEdgeID()), to);
    } else {
        WRITE_WARNING("NIVissimDisturbance: no connection");
        return NBConnection(0, 0);
//        throw 1; // !!! what to do?
    }

}

void
NIVissimDisturbance::clearDict() {
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete(*i).second;
    }
    myDict.clear();
}


void
NIVissimDisturbance::dict_SetDisturbances() {
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        NIVissimDisturbance *d = (*i).second;
        NIVissimAbstractEdge::dictionary(d->myEdge.getEdgeID())->addDisturbance((*i).first);
        NIVissimAbstractEdge::dictionary(d->myDisturbance.getEdgeID())->addDisturbance((*i).first);
    }
    /*    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
            delete (*i).second;
        }
        */
}


void
NIVissimDisturbance::reportRefused() {
    if (refusedProhibits>0) {
        WRITE_WARNING("Could not build " + toString<size_t>(refusedProhibits)+ " of " + toString<size_t>(myDict.size())+ " disturbances.");
    }
}



/****************************************************************************/

