/****************************************************************************/
/// @file    RONet.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The router's network representation
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
#ifndef RONet_h
#define RONet_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <set>
#include <fstream>
#include <deque>
#include <vector>
#include "ROEdge.h"
#include "RONode.h"
#include "ROVehicleCont.h"
#include "ROVehicle.h"
#include "RORouteDef.h"
#include <utils/common/SUMOVTypeParameter.h>
#include <utils/common/SUMOAbstractRouter.h>


// ===========================================================================
// class declarations
// ===========================================================================
class RONode;
class RORouteDef;
class OptionsCont;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RONet
 * @brief The router's network representation.
 *
 * A router network is responsible for watching loaded edges, nodes,!!!
 *
 * @todo Vehicle ids are not tracked; it may happen that the same id is added twice...
 */
class RONet {
public:
    /// @brief Constructor
    RONet() throw();


    /// @brief Destructor
    virtual ~RONet() throw();


    /// @name Insertion and retrieval of graph parts
    //@{

    /* @brief Adds a read edge to the network
     *
     * If the edge is already known (another one with the same id exists),
     *  an error is generated and given to msg-error-handler. The edge
     *  is deleted in this case and false is returned.
     *
     * @param[in] edge The edge to add
     * @return Whether the edge was added (if not, it was deleted, too)
     */
    virtual bool addEdge(ROEdge *edge) throw();


    /** @brief Retrieves an edge from the network
     *
     * This is not very pretty, but necessary, though, as routes run
     *  over instances, not over ids.
     *
     * @param[in] name The name of the edge to retrieve
     * @return The named edge if known, otherwise 0
     * @todo Check whether a const pointer may be returned
     */
    ROEdge *getEdge(const std::string &name) const throw() {
        return myEdges.get(name);
    }


    /* @brief Adds a read node to the network
     *
     * If the node is already known (another one with the same id exists),
     *  an error is generated and given to msg-error-handler. The node
     *  is deleted in this case
     *
     * @param[in] node The node to add
     */
    void addNode(RONode *node) throw();


    /** @brief Retrieves an node from the network
     *
     * @param[in] name The name of the node to retrieve
     * @return The named node if known, otherwise 0
     * @todo Check whether a const pointer may be returned
     */
    RONode *getNode(const std::string &id) const throw() {
        return myNodes.get(id);
    }
    //@}



    /// @name Insertion and retrieval of vehicle types, vehicles, routes, and route definitions
    //@{

    /* @brief Adds a read vehicle type definition to the network
     *
     * If the vehicle type definition is already known (another one with
     *  the same id exists), false is returned, and the vehicle type
     *  is deleted.
     *
     * @param[in] def The vehicle type to add
     * @return Whether the vehicle type could be added
     */
    virtual bool addVehicleType(SUMOVTypeParameter *type) throw();


    /** @brief Retrieves the named vehicle type
     *
     * If the named vehicle type was not added to the net before, a default
     *  vehicle type which consists of the id only is generated, added to the net
     *  and returned.
     *
     * Only if the name is "", 0 is returned.
     *
     * @param[in] id The id of the vehicle type to return
     * @return The named vehicle type
     * @todo Check whether a const pointer may be returned
     */
    SUMOVTypeParameter *getVehicleTypeSecure(const std::string &id) throw();


    /* @brief Adds a route definition to the network
     *
     * If the route definition is already known (another one with
     *  the same id exists), false is returned, but the route definition
     *  is not deleted.
     *
     * @param[in] def The route definition to add
     * @return Whether the route definition could be added
     * @todo Rename myRoutes to myRouteDefinitions
     */
    bool addRouteDef(RORouteDef *def) throw();


    /** @brief Returns the named route definition
     *
     * @param[in] name The name of the route definition to retrieve
     * @return The named route definition if known, otherwise 0
     * @todo Check whether a const pointer may be returned
     * @todo Rename myRoutes to myRouteDefinitions
     */
    RORouteDef *getRouteDef(const std::string &name) const throw() {
        return myRoutes.get(name);
    }


    /* @brief Adds a vehicle to the network
     *
     * If the vehicle is already known (another one with the same id
     *  exists), false is returned, but the vehicle is not deleted.
     *
     * Otherwise, the number of loaded routes ("myReadRouteNo") is increased.
     *
     * @param[in] id The id of the vehicle to add
     * @param[in] veh The vehicle to add
     * @return Whether the vehicle could be added
     */
    virtual bool addVehicle(const std::string &id, ROVehicle *veh) throw();
    // @}



    /// @name Processing stored vehicle definitions
    //@{

    /** @brief Computes routes described by their definitions and saves them
     *
     * As long a vehicle with a departure time not larger than the given
     *  exists, its route is computed and it is written and removed from
     *  the internal container.
     *
     * @param[in] options The options used during this process
     * @param[in] router The router to use for routes computation
     * @param[in] options The time until which route definitions shall be processed
     * @return The last seen departure time>=time
     */
    SUMOTime saveAndRemoveRoutesUntil(OptionsCont &options,
                                      SUMOAbstractRouter<ROEdge,ROVehicle> &router, SUMOTime time);


    /// Returns the information whether further vehicles are stored
    virtual bool furtherStored();
    //@}





    /** @brief Opens the output for computed routes
     *
     * If the second parameter is true, a second file for route alternatives
     *  will be opened. The route alternatives files is simply the given
     *  name with ".alt" appended (before the ".xml"-suffix).
     * If one of the file outputs can not be build, an IOError is thrown
     *
     * @param[in] filename The (base) name of the file(s) to create
     * @param[in] useAlternatives Whether a file for writing alternatives shall be created
     */
    void openOutput(const std::string &filename, bool useAlternatives) throw(IOError);


    /** @brief closes the file output for computed routes */
    void closeOutput() throw();




    /** @brief Returns a random edge which may be used as a starting point
     *
     * If the list of possible source (roads with no predecessor, "mySourceEdges") is empty,
     *  it is tried to be built, first.
     * @return A random edge from the list of edges with no predecessor
     */
    ROEdge *getRandomSource() throw();


    /** @brief Returns a random edge which may be used as a starting point
     *
     * If the list of possible sources (roads with no predecessor, "mySourceEdges") is empty,
     *  it is tried to be built, first.
     * @return A random edge from the list of edges with no predecessor
     */
    const ROEdge *getRandomSource() const throw();


    /** @brief Returns a random edge which may be used as an ending point
     *
     * If the list of possible destinations (roads with no successor, "myDestinationEdges") is empty,
     *  it is tried to be built, first.
     * @return A random edge from the list of edges with no successor
     */
    ROEdge *getRandomDestination() throw();


    /** @brief Returns a random edge which may be used as an ending point
     *
     * If the list of possible destinations (roads with no successor, "myDestinationEdges") is empty,
     *  it is tried to be built, first.
     * @return A random edge from the list of edges with no successor
     */
    const ROEdge *getRandomDestination() const throw();


    /// Returns the number of edges thenetwork contains
    unsigned int getEdgeNo() const;

    const std::map<std::string, ROEdge*> &getEdgeMap() const;

    bool hasRestrictions() const;

    void setRestrictionFound();


protected:
    bool computeRoute(OptionsCont &options,
                      SUMOAbstractRouter<ROEdge,ROVehicle> &router, const ROVehicle * const veh);

    /// Initialises the lists of source and destination edges
    void checkSourceAndDestinations() const;

protected:
    /// Known vehicle ids
    std::set<std::string> myVehIDs;

    /// Known nodes
    NamedObjectCont<RONode*> myNodes;

    /// Known edges
    NamedObjectCont<ROEdge*> myEdges;

    /// Known vehicle types
    NamedObjectCont<SUMOVTypeParameter*> myVehicleTypes;

    /// Known routes
    NamedObjectCont<RORouteDef*> myRoutes;

    /// Known vehicles
    ROVehicleCont myVehicles;

    /// List of source edges
    mutable std::vector<ROEdge*> mySourceEdges;

    /// List of destination edges
    mutable std::vector<ROEdge*> myDestinationEdges;

    /// The file to write the computed routes into
    OutputDevice *myRoutesOutput;

    /// The file to write the computed route alternatives into
    OutputDevice *myRouteAlternativesOutput;

    /// @brief The number of read routes
    unsigned int myReadRouteNo;

    /// @brief The number of discarded routes
    unsigned int myDiscardedRouteNo;

    /// @brief The number of written routes
    unsigned int myWrittenRouteNo;

    /// @brief Whether the network contains edges which not all vehicles may pass
    bool myHaveRestrictions;


private:
    /// @brief Invalidated copy constructor
    RONet(const RONet &src);

    /// @brief Invalidated assignment operator
    RONet &operator=(const RONet &src);

};


#endif

/****************************************************************************/

