/****************************************************************************/
/// @file    MSMeanData_Harmonoise.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// Noise data collector for edges/lanes
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
#ifndef MSMeanData_Harmonoise_h
#define MSMeanData_Harmonoise_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <cassert>
#include "MSMeanData.h"
#include <limits>


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;
class MSLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSMeanData_Harmonoise
 * @brief Noise data collector for edges/lanes
 *
 * This structure does not contain the data itself, it is stored within
 *  MSLaneMeanDataValues-MoveReminder objects.
 * This class is used to build the output, optionally, in the case
 *  of edge-based dump, aggregated over the edge's lanes.
 *
 * @todo consider error-handling on write (using IOError)
 */
class MSMeanData_Harmonoise : public MSMeanData {
public:
    /**
     * @class MSLaneMeanDataValues
     * @brief Data structure for mean (aggregated) edge/lane values
     *
     * Structure holding values that describe the noise aggregated over
     *  some seconds.
     */
    class MSLaneMeanDataValues : public MSMeanData::MeanDataValues {
    public:
        /** @brief Constructor */
        MSLaneMeanDataValues(MSLane * const lane, const SUMOReal length, const bool doAdd,
                             const std::set<std::string>* const vTypes=0,
                             const MSMeanData_Harmonoise *parent=0) throw();

        /** @brief Destructor */
        virtual ~MSLaneMeanDataValues() throw();


        /** @brief Resets values so they may be used for the next interval
         */
        void reset(bool afterWrite=false) throw();

        /** @brief Add the values to this meanData
         */
        void addTo(MSMeanData::MeanDataValues& val) const throw();


        /// @name Methods inherited from MSMoveReminder.
        /// @{

        /** @brief Computes current emission values and adds them to their sums
         *
         * The fraction of time the vehicle is on the lane is computed and
         *  used as a weight for the vehicle's current emission values
         *  which are computed using the current velocity and acceleration.
         *
         * @param[in] veh The regarded vehicle
         * @param[in] oldPos Position before the move-micro-timestep.
         * @param[in] newPos Position after the move-micro-timestep.
         * @param[in] newSpeed The vehicle's current speed
         * @return false, if the vehicle is beyond the lane, true otherwise
         * @see MSMoveReminder
         * @see MSMoveReminder::isStillActive
         * @see HelpersHarmonoise
         */
        bool isStillActive(SUMOVehicle& veh, SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed) throw();


        /** @brief Computes current emission values and adds them to their sums
         *
         * The fraction of time the vehicle is on the lane is computed and
         *  used as a weight for the vehicle's current emission values
         *  which are computed using the current velocity and acceleration.
         *
         * @param[in] veh The vehicle that enters the lane
         * @param[in] isEmit whether the vehicle was just emitted into the net
         * @param[in] isLaneChange whether the vehicle changed to the lane
         * @see MSMoveReminder::notifyEnter
         * @return Always true
         */
        bool notifyEnter(SUMOVehicle& veh, bool isEmit, bool isLaneChange) throw();
        //@}


        /** @brief Computes the noise in the last time step
         *
         * The sum of noises collected so far (in the last seen step)
         *  is built, and added to meanNTemp; currentTimeN is resetted.
         */
        void update() throw();

        /** @brief Writes output values into the given stream
         *
         * @param[in] dev The output device to write the data into
         * @param[in] period Length of the period the data were gathered
         * @param[in] numLanes The total number of lanes for which the data was collected
         * @param[in] length The length of the object for which the data was collected
         * @exception IOError If an error on writing occurs (!!! not yet implemented)
         */
        void write(OutputDevice &dev, const SUMOTime period,
                   const SUMOReal numLanes,
                   const int numVehicles=-1) const throw(IOError);


        /// @name Collected values
        /// @{

        /// @brief Sum of produced noise at this time step(pow(10, (<NOISE>/10.)))
        SUMOReal currentTimeN;

        /// @brief Sum of produced noise over time (pow(10, (<NOISE>/10.)))
        SUMOReal meanNTemp;
        //@}


        /// @brief The meandata parent
        const MSMeanData_Harmonoise* myParent;
    };


public:
    /** @brief Constructor
     *
     * @param[in] id The id of the detector
     * @param[in] dumpBegin Begin time of dump
     * @param[in] dumpEnd End time of dump
     * @param[in] useLanes Information whether lane-based or edge-based dump shall be generated
     * @param[in] withEmpty Information whether empty lanes/edges shall be written
     * @param[in] withInternal Information whether internal lanes/edges shall be written
     * @param[in] trackVehicles Information whether vehicles shall be tracked
     * @param[in] maxTravelTime the maximum travel time to use when calculating per vehicle output
     * @param[in] minSamples the minimum number of sample seconds before the values are valid
     * @param[in] vTypes the set of vehicle types to consider
     */
    MSMeanData_Harmonoise(const std::string &id,
                          const SUMOTime dumpBegin, const SUMOTime dumpEnd,
                          const bool useLanes, const bool withEmpty, const bool withInternal,
                          const bool trackVehicles,
                          const SUMOReal minSamples, const SUMOReal maxTravelTime,
                          const std::set<std::string> vTypes) throw();


    /// @brief Destructor
    virtual ~MSMeanData_Harmonoise() throw();

    /** @brief Updates the detector
     */
    virtual void update(const SUMOTime step) throw();


protected:
    /** @brief Create an instance of MeanDataValues
     *
     * @param[in] lane The lane to create for
     * @param[in] doAdd whether to add the values as reminder to the lane
     */
    MSMeanData::MeanDataValues* createValues(MSLane * const lane, const SUMOReal length, const bool doAdd) const throw(IOError);

private:
    /// @brief Invalidated copy constructor.
    MSMeanData_Harmonoise(const MSMeanData_Harmonoise&);

    /// @brief Invalidated assignment operator.
    MSMeanData_Harmonoise& operator=(const MSMeanData_Harmonoise&);

};


#endif

/****************************************************************************/

