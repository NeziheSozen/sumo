/****************************************************************************/
/// @file    NBDistrict.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A class representing a single district
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
#ifndef NBDistrict_h
#define NBDistrict_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <string>
#include <utility>
#include "NBCont.h"
#include <utils/common/Named.h>
#include <utils/common/VectorHelper.h>
#include <utils/geom/Position2D.h>
#include <utils/geom/Position2DVector.h>


// ===========================================================================
// class declarations
// ===========================================================================
class NBEdge;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBDistrict
 * @brief A class representing a single district
 *
 * A "district" is an area within the network which may be referenced by
 *  O/D-matrices. It stems from importing VISUM-networks. Work with VISUM-
 *  -networks also made it necessary that a district knows the edges at
 *  which new vehicles shall approach the simulated network (sources) and
 *  those to use when leaving the network (sinks). These connections to the
 *  network are weighted.
 *
 * Later work on VISUM required also parsing the shape of a district. This
 *  information is used by some external tools only, it is even not shown
 *  within the GUI.
 *
 * @todo Recheck whether this can be somehow joined with ODDistrict
 */
class NBDistrict : public Named {
public:
    /** @brief Constructor with id, and position
     *
     * @param[in] id The id of the district
     * @param[in] pos The position of the district
     */
    NBDistrict(const std::string &id, const Position2D &pos) throw();


    /** @brief Constructor without position
     *
     * The position must be computed later
     *
     * @param[in] id The id of the district
     */
    NBDistrict(const std::string &id) throw();


    /// @brief Destructor
    ~NBDistrict() throw();


    /** @brief Adds a source
     *
     * It is checked whether the edge has already been added as a source. false
     *  is returned in this case. Otherwise, the source is pushed into
     *  the list of sources and the weight into the list of source weights.
     *  both lists stay sorted this way. true is returned.
     *
     * @param[in] source An edge that shall be used as source
     * @param[in] weight The weight of the source
     * @return Whether the source could be added (was not added before)
     * @todo Consider using only one list for sources/weights
     */
    bool addSource(NBEdge * const source, SUMOReal weight) throw();


    /** @brief Adds a sink
     *
     * It is checked whether the edge has already been added as a sink. false
     *  is returned in this case. Otherwise, the sink is pushed into
     *  the list of sink and the weight into the list of sink weights.
     *  both lists stay sorted this way. true is returned.
     *
     * @param[in] sink An edge that shall be used as sink
     * @param[in] weight The weight of the sink
     * @return Whether the sink could be added (was not added before)
     * @todo Consider using only one list for sinks/weights
     */
    bool addSink(NBEdge * const sink, SUMOReal weight) throw();


    /** @brief Writes the sumo-xml-representation of this district into the given stream
     *
     * The district writes itself into the stream in XML-format.
     * The format looks as following:
     * @code
     *    <district id="<ID>" shape="<SHAPE>">
     *        [<dsource id="<EDGE_ID>" weight="<WEIGHT>"/>]*
     *        [<dsink id="<EDGE_ID>" weight="<WEIGHT>"/>]*
     *    </district>
     *
     * @param[in] into The device to write the xml-representation into
     */
    void writeXML(OutputDevice &into) throw();


    /** @brief Returns the position of this district's center
     *
     * @return The position of this district's center
     * @todo Recheck when this information is set/needed
     */
    const Position2D &getPosition() const throw();


    /** @brief Sets the center coordinates
     *
     * @param[in] pos The new center to assign
     * @todo Recheck when this information is set/needed
     */
    void setCenter(const Position2D &pos) throw();


    /** @brief Replaces incoming edges from the vector (sinks) by the given edge
     *
     * When an edge is split/joined/removed/etc., it may get necessary to replace prior
     *  edges by new ones. This method replaces all occurences of the edges from
     *  "which" within incoming edges (sinks) by the given edge.
     *
     * The new sink edge's weight is the sum of the weights of the replaced edges.
     *
     * @param[in] which List of edges to replace
     * @param[in] by The replacement
     */
    void replaceIncoming(const EdgeVector &which, NBEdge * const by) throw();


    /** @brief Replaces outgoing edges from the vector (source) by the given edge
     *
     * When an edge is split/joined/removed/etc., it may get necessary to replace prior
     *  edges by new ones. This method replaces all occurences of the edges from
     *  "which" within outgoing edges (sources) by the given edge.
     *
     * The new source edge's weight is the sum of the weights of the replaced edges.
     *
     * @param[in] which List of edges to replace
     * @param[in] by The replacement
     */
    void replaceOutgoing(const EdgeVector &which, NBEdge * const by) throw();


    /** @brief Removes the given edge from the lists of sources and sinks
     *
     * The according weights are removed, too.
     *
     * @param[in] e The edge to remove from sinks/sources
     */
    void removeFromSinksAndSources(NBEdge * const e) throw();


    /** @brief Sets the shape of this district
     *
     * @param[in] p The new shape
     */
    void addShape(const Position2DVector &p) throw();


    /** @brief Applies an offset to the district
     * @param[in] xoff The x-offset to apply
     * @param[in] yoff The y-offset to apply
     */
    void reshiftPosition(SUMOReal xoff, SUMOReal yoff) throw();


private:
    /// @brief Definition of a vector of connection weights
    typedef DoubleVector WeightsCont;

    /// @brief The sources (connection from district to network)
    EdgeVector mySources;

    /// @brief The weights of the sources
    WeightsCont mySourceWeights;

    /// @brief The sinks (connection from network to district)
    EdgeVector mySinks;

    /// @brief The weights of the sinks
    WeightsCont mySinkWeights;

    /// @brief The position of the district
    Position2D myPosition;

    /// @brief The shape of the dsitrict
    Position2DVector myShape;


private:
    /** invalid copy constructor */
    NBDistrict(const NBDistrict &s);

    /** invalid assignment operator */
    NBDistrict &operator=(const NBDistrict &s);


};


#endif

/****************************************************************************/

