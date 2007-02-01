/****************************************************************************/
/// @file    NBEdgeCont.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id: $
///
// A container for all of the nets edges
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

#include <vector>
#include <string>
#include <cassert>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <utils/geom/GeomHelper.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>
#include "NBEdgeCont.h"
#include "nodes/NBNodeCont.h"
#include "NBHelpers.h"
#include "NBCont.h"
#include "NBTrafficLightLogicCont.h"
#include "NBDistrictCont.h"
#include <cmath>
#include "NBTypeCont.h"
#include <iostream>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// debug definitions
// ===========================================================================
#define DEBUG_OUT cout


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// some definitions (debugging only)
// ===========================================================================
#define DEBUG_OUT cout


// ===========================================================================
// method definitions
// ===========================================================================
NBEdgeCont::NBEdgeCont()
        : EdgesSplit(0)
{}


NBEdgeCont::~NBEdgeCont()
{
    clear();
}

bool
NBEdgeCont::insert(NBEdge *edge)
{
    EdgeCont::iterator i = _edges.find(edge->getID());
    if (i!=_edges.end()) {
        return false;
    }
    if (OptionsSubSys::getOptions().isSet("edges-min-speed")) {
        if (edge->getSpeed()<OptionsSubSys::getOptions().getFloat("edges-min-speed")) {
            edge->getFromNode()->removeOutgoing(edge);
            edge->getToNode()->removeIncoming(edge);
            delete edge;
            return true;
        }
    }
    // check whether the edge is a named edge to keep
    if (!OptionsSubSys::getOptions().getBool("keep-edges.postload")
            &&
            OptionsSubSys::getOptions().isSet("keep-edges")) {

        if (!OptionsSubSys::helper_CSVOptionMatches("keep-edges", edge->getID())) {
            edge->getFromNode()->removeOutgoing(edge);
            edge->getToNode()->removeIncoming(edge);
            delete edge;
            return true;
        }
    }
    // check whether the edge is a named edge to remove
    if (OptionsSubSys::getOptions().isSet("remove-edges")) {
        if (OptionsSubSys::helper_CSVOptionMatches("remove-edges", edge->getID())) {
            edge->getFromNode()->removeOutgoing(edge);
            edge->getToNode()->removeIncoming(edge);
            delete edge;
            return true;
        }
    }
    // check whether the edge shall be removed due to a allow an unwished class
    if (OptionsSubSys::getOptions().isSet("remove-edges.by-type")) {
        int matching = 0;
        std::vector<SUMOVehicleClass> allowed = edge->getAllowedVehicleClasses();
        // !!! don't do this each time
        StringTokenizer st(OptionsSubSys::getOptions().getString("remove-edges.by-type"), ";");
        while (st.hasNext()) {
            SUMOVehicleClass vclass = getVehicleClassID(st.next());
            std::vector<SUMOVehicleClass>::iterator i =
                find(allowed.begin(), allowed.end(), vclass);
            if (i!=allowed.end()) {
                allowed.erase(i);
                matching++;
            }
        }
        // remove the edge if all allowed
        if (allowed.size()==0&&matching!=0) {
            edge->getFromNode()->removeOutgoing(edge);
            edge->getToNode()->removeIncoming(edge);
            delete edge;
            return true;
        }
    }
    _edges.insert(EdgeCont::value_type(edge->getID(), edge));
    return true;
}


NBEdge *
NBEdgeCont::retrieve(const string &id) const
{
    EdgeCont::const_iterator i = _edges.find(id);
    if (i==_edges.end()) return 0;
    return (*i).second;
}


bool
NBEdgeCont::computeTurningDirections()
{
    for (EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        (*i).second->computeTurningDirections();
    }
    return true;
}


bool
NBEdgeCont::sortOutgoingLanesConnections()
{
    for (EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        (*i).second->sortOutgoingLanesConnections();
    }
    return true;
}


bool
NBEdgeCont::computeEdge2Edges()
{
    for (EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        (*i).second->computeEdge2Edges();
    }
    return true;
}



bool
NBEdgeCont::computeLanes2Edges()
{
    for (EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        (*i).second->computeLanes2Edges();
    }
    return true;
}



bool
NBEdgeCont::recheckLanes()
{
    for (EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        (*i).second->recheckLanes();
    }
    return true;
}


/*
bool
NBEdgeCont::computeLinkPriorities(bool verbose)
{
    for(EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        (*i).second->computeLinkPriorities();
    }
    return true;
}
*/

bool
NBEdgeCont::appendTurnarounds()
{
    for (EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        (*i).second->appendTurnaround();
    }
    return true;
}


void
NBEdgeCont::writeXMLEdgeList(ostream &into, std::vector<std::string> toAdd)
{
    into << "   <edges no=\"" << (_edges.size()+toAdd.size()) << "\">";
    for (vector<string>::iterator j=toAdd.begin(); j!=toAdd.end(); j++) {
        if (j!=toAdd.begin()) {
            into << ' ';
        }
        into << (*j);
    }
    if (toAdd.size()!=0) {
        into << ' ';
    }

    for (EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        if (i!=_edges.begin()) {
            into << ' ';
        }
        into << (*i).first;
    }
    into << "</edges>" << endl;
}


void
NBEdgeCont::writeXMLStep1(std::ostream &into)
{
    for (EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        (*i).second->writeXMLStep1(into);
    }
    into << endl;
}


void
NBEdgeCont::writeXMLStep2(std::ostream &into, bool includeInternal)
{
    for (EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        (*i).second->writeXMLStep2(into, includeInternal);
    }
    into << endl;
}

int NBEdgeCont::size()
{
    return _edges.size();
}


int
NBEdgeCont::getNo()
{
    return _edges.size();
}


void
NBEdgeCont::clear()
{
    for (EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++)
        delete((*i).second);
    _edges.clear();
}


void
NBEdgeCont::report()
{
    WRITE_MESSAGE("   " + toString<int>(getNo()) + " edges loaded.");
    if (EdgesSplit>0) {
        WRITE_WARNING("The split of edges was performed "+ toString<int>(EdgesSplit) + " times.");
    }
}


bool
NBEdgeCont::splitAt(NBDistrictCont &dc, NBEdge *edge, NBNode *node)
{
    return splitAt(dc, edge, node,
                   edge->getID() + "[0]", edge->getID() + "[1]",
                   edge->_nolanes, edge->_nolanes);
}


bool
NBEdgeCont::splitAt(NBDistrictCont &dc, NBEdge *edge, NBNode *node,
                    const std::string &firstEdgeName,
                    const std::string &secondEdgeName,
                    size_t noLanesFirstEdge, size_t noLanesSecondEdge)
{
    SUMOReal pos;
    pos = edge->getGeometry().nearest_position_on_line_to_point(node->getPosition());
    if (pos<=0) {
        pos = GeomHelper::nearest_position_on_line_to_point(
                  edge->_from->getPosition(), edge->_to->getPosition(),
                  node->getPosition());
    }
    if (pos<=0) {
        return false;
    }
    return splitAt(dc, edge, pos, node, firstEdgeName, secondEdgeName,
                   noLanesFirstEdge, noLanesSecondEdge);
    //!!! does not regard the real edge geometry
}

bool
NBEdgeCont::splitAt(NBDistrictCont &dc,
                    NBEdge *edge, SUMOReal pos, NBNode *node,
                    const std::string &firstEdgeName,
                    const std::string &secondEdgeName,
                    size_t noLanesFirstEdge, size_t noLanesSecondEdge)
{
    // build the new edges' geometries
    std::pair<Position2DVector, Position2DVector> geoms =
        edge->getGeometry().splitAt(pos);
    if (geoms.first[-1]!=node->getPosition()) {
        geoms.first.pop_back();
        geoms.first.push_back(node->getPosition());
    }

    if (geoms.second[0]!=node->getPosition()) {
        geoms.second.pop_front();
        geoms.second.push_front(node->getPosition());
    }
#ifdef _DEBUG
#ifdef CHECK_UNIQUE_POINTS_GEOMETRY
    if (!geoms.first.assertNonEqual()) {
        DEBUG_OUT << "first: " << edge->getID() << endl;
        DEBUG_OUT << geoms.first << endl;
        DEBUG_OUT << geoms.second << endl;
    }
    if (!geoms.second.assertNonEqual()) {
        DEBUG_OUT << "second: " << edge->getID() << endl;
        DEBUG_OUT << geoms.first << endl;
        DEBUG_OUT << geoms.second << endl;
    }
#endif
#endif
    // build and insert the edges
    NBEdge *one = new NBEdge(firstEdgeName, firstEdgeName,
                             edge->_from, node, edge->_type, edge->_speed, noLanesFirstEdge,
                             pos, edge->getPriority(), geoms.first, edge->myLaneSpreadFunction,
                             edge->_basicType);
    size_t i;
    for (i=0; i<noLanesFirstEdge&&i<edge->getNoLanes(); i++) {
        one->setLaneSpeed(i, edge->getLaneSpeed(i));
    }
    NBEdge *two = new NBEdge(secondEdgeName, secondEdgeName,
                             node, edge->_to, edge->_type, edge->_speed, noLanesSecondEdge,
                             edge->_length-pos, edge->getPriority(), geoms.second,
                             edge->myLaneSpreadFunction, edge->_basicType);
    for (i=0; i<noLanesSecondEdge&&i<edge->getNoLanes(); i++) {
        two->setLaneSpeed(i, edge->getLaneSpeed(i));
    }
    two->copyConnectionsFrom(edge);
    // replace information about this edge within the nodes
    edge->_from->replaceOutgoing(edge, one, 0);
    edge->_to->replaceIncoming(edge, two, 0);
    // the edge is now occuring twice in both nodes...
    //  clean up
    edge->_from->removeDoubleEdges();
    edge->_to->removeDoubleEdges();
    // add connections from the first to the second edge
    // check special case:
    //  one in, one out, the outgoing has one lane more
    if (noLanesFirstEdge==noLanesSecondEdge-1) {
        for (i=0; i<one->getNoLanes(); i++) {
            if (!one->addLane2LaneConnection(i, two, i+1, false)) {// !!! Bresenham, here!!!
                MsgHandler::getErrorInstance()->inform("Could not set connection!");
                throw ProcessError();
            }
        }
        one->addLane2LaneConnection(0, two, 0, false);
    } else {
        for (i=0; i<one->getNoLanes()&&i<two->getNoLanes(); i++) {
            if (!one->addLane2LaneConnection(i, two, i, false)) {// !!! Bresenham, here!!!
                MsgHandler::getErrorInstance()->inform("Could not set connection!");
                throw ProcessError();
            }
        }
    }
    // erase the splitted edge
    erase(dc, edge);
    insert(one);
    insert(two);
    EdgesSplit++;
    return true;
}


void
NBEdgeCont::erase(NBDistrictCont &dc, NBEdge *edge)
{
    _edges.erase(edge->getID());
    edge->_from->removeOutgoing(edge);
    edge->_to->removeIncoming(edge);
    dc.removeFromSinksAndSources(edge);
    delete edge;
}


NBEdge *
NBEdgeCont::retrievePossiblySplitted(const std::string &id,
                                     const std::string &hint,
                                     bool incoming) const
{
    // try to retrieve using the given name (iterative)
    NBEdge *edge = retrieve(id);
    if (edge!=0) {
        return edge;
    }
    // now, we did not find it; we have to look over all possibilities
    EdgeVector hints;
    // check whether at least the hint was not splitted
    NBEdge *hintedge = retrieve(hint);
    if (hintedge==0) {
        hints = getGeneratedFrom(hint);
    } else {
        hints.push_back(hintedge);
    }
    EdgeVector candidates = getGeneratedFrom(id);
    for (EdgeVector::iterator i=hints.begin(); i!=hints.end(); i++) {
        NBEdge *hintedge = (*i);
        for (EdgeVector::iterator j=candidates.begin(); j!=candidates.end(); j++) {
            NBEdge *poss_searched = (*j);
            NBNode *node = incoming
                           ? poss_searched->_to : poss_searched->_from;
            const EdgeVector &cont = incoming
                                     ? node->getOutgoingEdges() : node->getIncomingEdges();
            if (find(cont.begin(), cont.end(), hintedge)!=cont.end()) {
                return poss_searched;
            }
        }
    }
    return 0;
}


EdgeVector
NBEdgeCont::getGeneratedFrom(const std::string &id) const
{
    size_t len = id.length();
    EdgeVector ret;
    for (EdgeCont::const_iterator i=_edges.begin(); i!=_edges.end(); i++) {
        string curr = (*i).first;
        // the next check makes it possibly faster - we don not have
        //  to compare the names
        if (curr.length()<=len) {
            continue;
        }
        // the name must be the same as the given id but something
        //  beginning with a '[' must be appended to it
        if (curr.substr(0, len)==id&&curr[len]=='[') {
            ret.push_back((*i).second);
            continue;
        }
        // ok, maybe the edge is a compound made during joining of edges
        size_t pos = curr.find(id);
        // surely not
        if (pos==string::npos) {
            continue;
        }
        // check leading char
        if (pos>0) {
            if (curr[pos-1]!=']'&&curr[pos-1]!='+') {
                // actually, this is another id
                continue;
            }
        }
        if (pos+id.length()<curr.length()) {
            if (curr[pos+id.length()]!='['&&curr[pos+id.length()]!='+') {
                // actually, this is another id
                continue;
            }
        }
        ret.push_back((*i).second);
    }
    return ret;
}

std::vector<std::string>
NBEdgeCont::buildPossibilities(const std::vector<std::string> &s)
{
    std::vector<std::string> ret;
    for (std::vector<std::string>::const_iterator i=s.begin(); i!=s.end(); i++) {
        ret.push_back((*i) + "[0]");
        ret.push_back((*i) + "[1]");
    }
    return ret;
}



void
NBEdgeCont::joinSameNodeConnectingEdges(NBDistrictCont &dc,
                                        NBTrafficLightLogicCont &tlc,
                                        EdgeVector edges)
{
    // !!! Attention!
    //  No merging of the geometry to come is being done
    //  The connections are moved from one edge to another within
    //   the replacement where the edge is a node's incoming edge.

    // count the number of lanes, the speed and the id
    size_t nolanes = 0;
    SUMOReal speed = 0;
    int priority = 0;
    string id;
    NBEdge::EdgeBasicFunction function = NBEdge::EDGEFUNCTION_UNKNOWN;
    sort(edges.begin(), edges.end(), NBContHelper::same_connection_edge_sorter());
    // retrieve the connected nodes
    NBEdge *tpledge = *(edges.begin());
    NBNode *from = tpledge->getFromNode();
    NBNode *to = tpledge->getToNode();
    EdgeVector::const_iterator i;
    for (i=edges.begin(); i!=edges.end(); i++) {
        // some assertions
        assert((*i)->getFromNode()==from);
        assert((*i)->getToNode()==to);
        // ad the number of lanes the current edge has
        nolanes += (*i)->getNoLanes();
        // build the id
        if (i!=edges.begin()) {
            id += "+";
        }
        id += (*i)->getID();
        // build the edge type
        if (function==NBEdge::EDGEFUNCTION_UNKNOWN) {
            function = (*i)->getBasicType();
        } else {
            if (function!=NBEdge::EDGEFUNCTION_NORMAL) {
                if (function!=(*i)->getBasicType()) {
                    function = NBEdge::EDGEFUNCTION_NORMAL;
                }
            }
        }
        // compute the speed
        speed += (*i)->getSpeed();
        // build the priority
        if (priority<(*i)->getPriority()) {
            priority = (*i)->getPriority();
        }
        // remove all connections to the joined edges
        /*
                for(EdgeVector::const_iterator j=edges.begin(); j!=edges.end(); j++) {
                    (*i)->removeFromConnections(*j);
                }
                */
    }
    speed /= edges.size();
    // build the new edge
    NBEdge *newEdge = new NBEdge(id, id, from, to, "", speed,
                                 nolanes, -1, priority, tpledge->myLaneSpreadFunction, function);
    insert(newEdge);
    /*    // sort edges as lanes
        sort(edges.begin(), edges.end(),
            NBContHelper::edge_to_lane_sorter(from, to));*/
    // replace old edge by current within the nodes
    //  and delete the old
    from->replaceOutgoing(edges, newEdge);
    to->replaceIncoming(edges, newEdge);
    // patch connections
    //  add edge2edge-information
    for (i=edges.begin(); i!=edges.end(); i++) {
        EdgeVector ev = (*i)->getConnected();
        for (EdgeVector::iterator j=ev.begin(); j!=ev.end(); j++) {
            newEdge->addEdge2EdgeConnection(*j);
        }
    }
    //  move lane2lane-connections
    size_t currLane = 0;
    for (i=edges.begin(); i!=edges.end(); i++) {
        newEdge->moveOutgoingConnectionsFrom(*i, currLane, false);
        currLane += (*i)->getNoLanes();
    }
    // patch tl-information
    currLane = 0;
    for (i=edges.begin(); i!=edges.end(); i++) {
        size_t noLanes = (*i)->getNoLanes();
        for (size_t j=0; j<noLanes; j++, currLane++) {
            // replace in traffic lights
            tlc.replaceRemoved(*i, j, newEdge, currLane);
        }
    }
    // delete joined edges
    for (i=edges.begin(); i!=edges.end(); i++) {
        erase(dc, *i);
    }
}


NBEdge *
NBEdgeCont::retrievePossiblySplitted(const std::string &id, SUMOReal pos) const
{
    // check whether the edge was not split, yet
    NBEdge *edge = retrieve(id);
    if (edge!=0) {
        return edge;
    }
    // find the part of the edge which matches the position
    SUMOReal seen = 0;
    std::vector<string> names;
    names.push_back(id + "[1]");
    names.push_back(id + "[0]");
    while (true) {
        // retrieve the first subelement (to follow)
        string cid = names[names.size()-1];
        names.pop_back();
        edge = retrieve(cid);
        // The edge was splitted; check its subparts within the
        //  next step
        if (edge==0) {
            names.push_back(cid + "[1]");
            names.push_back(cid + "[0]");
        }
        // an edge with the name was found,
        //  check whether the position lies within it
        else {
            seen += edge->getLength();
            if (seen>=pos) {
                return edge;
            }
        }
    }
    return 0;
}


void
NBEdgeCont::search(NBEdge *e)
{
    for (EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        DEBUG_OUT << (*i).second << ", " << (*i).second->getID() << endl;
        if ((*i).second==e) {
            int checkdummy = 0;
            checkdummy += 2;
        }
    }
    DEBUG_OUT << "--------------------------------" << endl;
}


bool
NBEdgeCont::normaliseEdgePositions(const NBNodeCont &nc)
{
    for (EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        (*i).second->normalisePosition(nc);
    }
    return true;
}


bool
NBEdgeCont::reshiftEdgePositions(SUMOReal xoff, SUMOReal yoff, SUMOReal rot)
{
    for (EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        (*i).second->reshiftPosition(xoff, yoff, rot);
    }
    return true;
}


bool
NBEdgeCont::computeEdgeShapes()
{
    for (EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        (*i).second->computeEdgeShape();
    }
    return true;
}


std::vector<std::string>
NBEdgeCont::getAllNames()
{
    std::vector<std::string> ret;
    for (EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); ++i) {
        ret.push_back((*i).first);
    }
    return ret;
}


bool
NBEdgeCont::savePlain(const std::string &file)
{
    // try to build the output file
    ofstream res(file.c_str());
    if (!res.good()) {
        MsgHandler::getErrorInstance()->inform("Plain edge file '" + file + "' could not be opened.");
        return false;
    }
    std::fixed (res);
    res << "<edges>" << endl;
    for (EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        NBEdge *e = (*i).second;
        res << std::setprecision(2);
        res << "   <edge id=\"" << e->getID()
        << "\" fromnode=\"" << e->getFromNode()->getID()
        << "\" tonode=\"" << e->getToNode()->getID()
        << "\" nolanes=\"" << e->getNoLanes()
        << "\" speed=\"" << e->getSpeed() << "\"";
        // write the geometry only if larger than just the from/to positions
        if (e->getGeometry().size()>2) {
            res << std::setprecision(10);
            res << " shape=\"" << e->getGeometry() << "\"";
        }
        // write the spread type if not default ("right")
        if (e->getLaneSpreadFunction()!=NBEdge::LANESPREAD_RIGHT) {
            res << " spread_type=\"center\"";
        }
        // write the function if not "normal"
        if (e->getBasicType()!=NBEdge::EDGEFUNCTION_NORMAL) {
            switch (e->getBasicType()) {
            case NBEdge::EDGEFUNCTION_SOURCE:
                res << " function=\"source\"";
                break;
            case NBEdge::EDGEFUNCTION_SINK:
                res << " function=\"sink\"";
                break;
            default:
                // hmmm - do nothing? seems to be invalid anyhow
                break;
            }
        }
        // write the vehicles class if restrictions exist
        if (!e->hasRestrictions()) {
            res << "/>" << endl;
        } else {
            res << ">" << endl;
            e->writeLanesPlain(res);
            res << "   </edge>" << endl;
        }
    }
    res << "</edges>" << endl;
    return res.good();
}


bool
NBEdgeCont::removeUnwishedEdges(NBDistrictCont &dc, OptionsCont &)
{
    //
    std::vector<NBEdge*> toRemove;
    for (EdgeCont::iterator i=_edges.begin(); i!=_edges.end();) {
        NBEdge *edge = (*i).second;
        if (!OptionsSubSys::helper_CSVOptionMatches("keep-edges", edge->getID())) {
            edge->getFromNode()->removeOutgoing(edge);
            edge->getToNode()->removeIncoming(edge);
            toRemove.push_back(edge);
        }
        ++i;
    }
    for (std::vector<NBEdge*>::iterator j=toRemove.begin(); j!=toRemove.end(); ++j) {
        erase(dc, *j);
    }
    return true;
}


bool
NBEdgeCont::recomputeLaneShapes()
{
    for (EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); ++i) {
        (*i).second->computeLaneShapes();
    }
    return true;
}


bool
NBEdgeCont::splitGeometry(NBNodeCont &nc)
{
    for (EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); ++i) {
        if ((*i).second->getGeometry().size()<3) {
            continue;
        }
        (*i).second->splitGeometry(*this, nc);
    }
    return true;
}


void
NBEdgeCont::recheckLaneSpread()
{
    for (EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); ++i) {
        string oppositeID;
        if ((*i).first[0]=='-') {
            oppositeID = (*i).first.substr(1);
        } else {
            oppositeID = "-" + (*i).first;
        }
        if (_edges.find(oppositeID)!=_edges.end()) {
            (*i).second->setLaneSpreadFunction(NBEdge::LANESPREAD_RIGHT);
            _edges.find(oppositeID)->second->setLaneSpreadFunction(NBEdge::LANESPREAD_RIGHT);
        } else {
            (*i).second->setLaneSpreadFunction(NBEdge::LANESPREAD_CENTER);
        }
    }
}


void
NBEdgeCont::recheckEdgeGeomsForDoublePositions()
{
    for (EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); ++i) {
        (*i).second->recheckEdgeGeomForDoublePositions();
    }
}



/****************************************************************************/

