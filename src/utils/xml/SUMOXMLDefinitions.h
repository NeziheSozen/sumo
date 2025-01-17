/****************************************************************************/
/// @file    SUMOXMLDefinitions.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Definitions of elements and attributes known by SUMO
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
#ifndef SUMOXMLDefinitions_h
#define SUMOXMLDefinitions_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


// ===========================================================================
// definitions
// ===========================================================================
/**
 * @enum SumoXMLTag
 * @brief Numbers representing SUMO-XML - element names
 * @see GenericSAXHandler
 * @see SUMOSAXHandler
 */
enum SumoXMLTag {
    /** invalid tag */
    SUMO_TAG_NOTHING,
    /** begin/end of the description of an edge */
    SUMO_TAG_EDGE,
    /** begin/end of the description of a single lane */
    SUMO_TAG_LANE,
    /** begin/end of the description of a polygon */
    SUMO_TAG_POI,
    /** begin/end of the description of a polygon */
    SUMO_TAG_POLY,
    /** begin/end of the description of a junction */
    SUMO_TAG_JUNCTION,
#ifdef _MESSAGES
    /** tag for a message emitter */
    SUMO_TAG_MSG_EMITTER,
    /** a msg detector (e-4 detector*/
    SUMO_TAG_MSG,
#endif
    /** a detector */
    /// @deprecated begins
    SUMO_TAG_DETECTOR,
    /// @deprecated ends
    /** an e1-detector */
    SUMO_TAG_E1DETECTOR,
    /** an e2-detector */
    SUMO_TAG_E2DETECTOR,
    /** an e3-detector */
    SUMO_TAG_E3DETECTOR,
    /** an edge based mean data detector */
    SUMO_TAG_MEANDATA_EDGE,
    /** a lane based mean data detector */
    SUMO_TAG_MEANDATA_LANE,
    /** an e3-entry point*/
    SUMO_TAG_DET_ENTRY,
    /** an e3-exit point */
    SUMO_TAG_DET_EXIT,
    /// @brief A variable speed sign
    SUMO_TAG_VSS,
    /// @brief An emitter (deprecated!!!)
    SUMO_TAG_EMITTER,
    /// @brief A calibrator
    SUMO_TAG_CALIBRATOR,
    /// @brief A rerouter
    SUMO_TAG_REROUTER,
    /// @brief A bus stop
    SUMO_TAG_BUS_STOP,
    /** a vtypeprobe detector */
    SUMO_TAG_VTYPEPROBE,
    /** a routeprobe detector */
    SUMO_TAG_ROUTEPROBE,
    /** description of a vehicle */
    SUMO_TAG_VEHICLE,
    /** description of a vehicle type */
    SUMO_TAG_VTYPE,
    /** description of a car following model */
    SUMO_TAG_CFMODEL,
    /** begin/end of the description of a route */
    SUMO_TAG_ROUTE,
    /** begin/end of the description of succeeding lanes */
    SUMO_TAG_SUCC,
    /** description of a single succeeding lane */
    SUMO_TAG_SUCCLANE,
    /** begin/end of the key of a junction */
    SUMO_TAG_KEY,
    /** begin of a right-of-way logic */
    SUMO_TAG_ROWLOGIC,
    /** the request size (of a junction logic) */
    SUMO_TAG_REQUESTSIZE,
    /** the number of lanes */
    SUMO_TAG_LANENUMBER,
    /** description of a single junction logic part */
    SUMO_TAG_LOGICITEM,
    /** a source */
    SUMO_TAG_SOURCE,
    /** a district */
    SUMO_TAG_DISTRICT,
    /** a source within a district (connection road) */
    SUMO_TAG_DSOURCE,
    /** a sink within a district (connection road) */
    SUMO_TAG_DSINK,
    /** a traffic light logic */
    SUMO_TAG_TLLOGIC,
    /** the time offset from the first switch */
    SUMO_TAG_OFFSET,
    /** a single phase description */
    SUMO_TAG_PHASE,
    /** a single trip definition (used by router) */
    SUMO_TAG_TRIPDEF,
    /** a flow definition (used by router) */
    SUMO_TAG_FLOW,
    /** the number of the logic (traffic light logic) */
    SUMO_TAG_SUBKEY,
    /** a trigger definition */
    SUMO_TAG_TRIGGER,
    /** trigger: a step description */
    SUMO_TAG_STEP,
    /** an aggreagated-output interval */
    SUMO_TAG_INTERVAL,
    /// The tag enclosing an object's shape
    SUMO_TAG_SHAPE,
    /// The definition of a periodic event
    SUMO_TAG_TIMEDEVENT,
    /** begin/end of the list of incoming lanes (into a junction) */
    SUMO_TAG_INCOMING_LANES,
    /** begin/end of the list of internal lanes (of a junction) */
    SUMO_TAG_INTERNAL_LANES,
    /// Incoming edge specification (jp-router)
    SUMO_TAG_FROMEDGE,
    /// Outgoing edge specification (jp-router)
    SUMO_TAG_TOEDGE,
    /// Sink(s) specification
    SUMO_TAG_SINK,
    SUMO_TAG_SUPPLEMENTARY_WEIGHTS,
    SUMO_TAG_PARAM,
    SUMO_TAG_WAUT,
    SUMO_TAG_WAUT_SWITCH,
    SUMO_TAG_WAUT_JUNCTION,
    SUMO_TAG_NET_OFFSET,
    SUMO_TAG_CONV_BOUNDARY,
    SUMO_TAG_ORIG_BOUNDARY,
    SUMO_TAG_ORIG_PROJ,
    SUMO_TAG_ROUTEDISTELEM,
    SUMO_TAG_VTYPEDISTELEM,
    SUMO_TAG_EMIT,
    SUMO_TAG_RESET,
    SUMO_TAG_STOP,
    SUMO_TAG_DEST_PROB_REROUTE,
    SUMO_TAG_CLOSING_REROUTE,
    SUMO_TAG_ROUTE_PROB_REROUTE,
    SUMO_TAG_POLYTYPE,
    SUMO_TAG_CONNECTION,
    SUMO_TAG_PROHIBITION,
    SUMO_TAG_SPLIT,
    SUMO_TAG_NODE,
    SUMO_TAG_TYPE,
    SUMO_TAG_DETECTOR_DEFINITION,
    SUMO_TAG_ROUTE_DISTRIBUTION,
    SUMO_TAG_VTYPE_DISTRIBUTION,
    SUMO_TAG_VAPORIZER,

    SUMO_TAG_WAY,
    SUMO_TAG_ND,
    SUMO_TAG_TAG,

    SUMO_TAG_VIEWSETTINGS_DECAL,
    SUMO_TAG_VIEWSETTINGS_SCHEME,
    SUMO_TAG_VIEWSETTINGS_OPENGL,
    SUMO_TAG_VIEWSETTINGS_BACKGROUND,
    SUMO_TAG_VIEWSETTINGS_EDGES,
    SUMO_TAG_VIEWSETTINGS_EDGE_COLOR_ITEM,
    SUMO_TAG_VIEWSETTINGS_VEHICLES,
    SUMO_TAG_VIEWSETTINGS_VEHICLE_COLOR_ITEM,
    SUMO_TAG_VIEWSETTINGS_JUNCTIONS,
    SUMO_TAG_VIEWSETTINGS_ADDITIONALS,
    SUMO_TAG_VIEWSETTINGS_POIS,
    SUMO_TAG_VIEWSETTINGS_LEGEND,
    SUMO_TAG_INCLUDE,
    SUMO_TAG_DELAY,
    SUMO_TAG_VIEWPORT,
    SUMO_TAG_SNAPSHOT,
    SUMO_TAG_LOCATION,
    SUMO_TAG_COLORSCHEME,
    SUMO_TAG_ENTRY,

    SUMO_TAG_CF_KRAUSS,
    SUMO_TAG_CF_KRAUSS_ORIG1,
    SUMO_TAG_CF_IDM,
    SUMO_TAG_CF_PWAGNER2009,
    SUMO_TAG_CF_BKERNER,

    SUMO_TAG_OPENDRIVE_HEADER,
    SUMO_TAG_OPENDRIVE_ROAD,
    SUMO_TAG_OPENDRIVE_PREDECESSOR,
    SUMO_TAG_OPENDRIVE_SUCCESSOR,
    // !!! SUMO_TAG_OPENDRIVE_NEIGHBOR,
    // !!! SUMO_TAG_OPENDRIVE_TYPE,
    SUMO_TAG_OPENDRIVE_GEOMETRY,
    SUMO_TAG_OPENDRIVE_LINE,
    SUMO_TAG_OPENDRIVE_SPIRAL,
    SUMO_TAG_OPENDRIVE_ARC,
    SUMO_TAG_OPENDRIVE_POLY3,
    SUMO_TAG_OPENDRIVE_LANESECTION,
    SUMO_TAG_OPENDRIVE_LEFT,
    SUMO_TAG_OPENDRIVE_CENTER,
    SUMO_TAG_OPENDRIVE_RIGHT,
    SUMO_TAG_OPENDRIVE_LANE,

    SUMO_TAG_PERSON,
    SUMO_TAG_RIDE,
    SUMO_TAG_WALK,

    /**
     * ActivityGen Tags
     **/
    AGEN_TAG_GENERAL,
    /* streets object */
    AGEN_TAG_STREET,

    /* workingHours object */
    AGEN_TAG_WORKHOURS,
    AGEN_TAG_OPENING,
    AGEN_TAG_CLOSING,

    /* school object */
    AGEN_TAG_SCHOOLS,
    AGEN_TAG_SCHOOL,

    /* busStation and bus objects */
    AGEN_TAG_BUSSTATION,
    AGEN_TAG_BUSLINE,
    AGEN_TAG_STATIONS,
    AGEN_TAG_REV_STATIONS,
    AGEN_TAG_STATION,
    AGEN_TAG_FREQUENCY,

    /* population and children accompaniment brackets */
    AGEN_TAG_POPULATION,
    AGEN_TAG_BRACKET,
    //AGEN_TAG_CHILD_ACOMP,

    /* city entrances */
    AGEN_TAG_CITYGATES,
    AGEN_TAG_ENTRANCE,

    /* parameters */
    AGEN_TAG_PARAM

};


/**
 * @enum SumoXMLAttr
 * @brief Numbers representing SUMO-XML - attributes
 * @see GenericSAXHandler
 * @see SUMOSAXHandler
 */
enum SumoXMLAttr {
    /** invalid attribute */
    SUMO_ATTR_NOTHING,
    /* common attributes */
    SUMO_ATTR_ID,
    SUMO_ATTR_REFID,
    SUMO_ATTR_NAME,
    SUMO_ATTR_TYPE,
    SUMO_ATTR_PRIORITY,
    SUMO_ATTR_NOLANES,
    SUMO_ATTR_SPEED,
    /* edge attributes */
    SUMO_ATTR_LENGTH,
    SUMO_ATTR_FROMNODE,
    SUMO_ATTR_TONODE,
    SUMO_ATTR_XFROM,
    SUMO_ATTR_YFROM,
    SUMO_ATTR_XTO,
    SUMO_ATTR_YTO,
    /* node attributes */
    SUMO_ATTR_X,
    SUMO_ATTR_Y,
    /* sumo-junction attributes */
    SUMO_ATTR_KEY,
    SUMO_ATTR_REQUESTSIZE,
    SUMO_ATTR_LANENUMBER,
    SUMO_ATTR_REQUEST,
    SUMO_ATTR_RESPONSE,
    SUMO_ATTR_PROGRAMID,
    SUMO_ATTR_OFFSET,
    SUMO_ATTR_INCLANES,
    SUMO_ATTR_INTLANES,
    /// the weight of a district's source or sink
    SUMO_ATTR_WEIGHT,
    SUMO_ATTR_EDGE,
    /// the edges of a route
    SUMO_ATTR_EDGES,
    /* vehicle attributes */
    SUMO_ATTR_DEPART,
    SUMO_ATTR_DEPARTLANE,
    SUMO_ATTR_DEPARTPOS,
    SUMO_ATTR_DEPARTSPEED,
    SUMO_ATTR_ARRIVALLANE,
    SUMO_ATTR_ARRIVALPOS,
    SUMO_ATTR_ARRIVALSPEED,
    SUMO_ATTR_ROUTE,
    SUMO_ATTR_MAXSPEED,
    SUMO_ATTR_ACCEL,
    SUMO_ATTR_DECEL,
    SUMO_ATTR_VCLASS,
    SUMO_ATTR_REPNUMBER,
    SUMO_ATTR_SPEEDFACTOR,
    SUMO_ATTR_SPEEDDEV,
    SUMO_ATTR_CAR_FOLLOW_MODEL,
    SUMO_ATTR_LANE_CHANGE_MODEL,
    /* Car following model attributes */
    SUMO_ATTR_SIGMA,    // used by: Krauss
    SUMO_ATTR_TAU,      // Krauss
    SUMO_ATTR_MINIMUM_DISTANCE, // IDM
    SUMO_ATTR_TIME_HEADWAY_GAP, // IDM
    /* route alternatives / distribution attributes */
    SUMO_ATTR_LAST,
    SUMO_ATTR_COST,
    SUMO_ATTR_PROB,
    SUMO_ATTR_ROUTES,
    SUMO_ATTR_VTYPES,
    /* trip definition attributes */
    SUMO_ATTR_LANE,
    SUMO_ATTR_LANES,
    SUMO_ATTR_FROM,
    SUMO_ATTR_TO,
    SUMO_ATTR_PERIOD,
    SUMO_ATTR_FROM_TAZ,
    SUMO_ATTR_TO_TAZ,
    /* source definitions */
    SUMO_ATTR_FUNCTION,
    SUMO_ATTR_POSITION,
    SUMO_ATTR_FREQUENCY,
    SUMO_ATTR_STYLE,
    SUMO_ATTR_FILE,
    SUMO_ATTR_JUNCTION,
    SUMO_ATTR_YIELD,
    SUMO_ATTR_NO,
    SUMO_ATTR_PHASE,
    SUMO_ATTR_BRAKE,
    SUMO_ATTR_YELLOW,
    SUMO_ATTR_DURATION,
    SUMO_ATTR_UNTIL,
    /** trigger: definition of an object type to be triggered */
    SUMO_ATTR_OBJECTTYPE,
    /** trigger: definition of an attribute to trigger */
    SUMO_ATTR_ATTR,
    /** trigger: name of the object to manipulate */
    SUMO_ATTR_OBJECTID,
    /** trigger: the time of the step */
    SUMO_ATTR_TIME,
    /** weights: time range begin */
    SUMO_ATTR_BEGIN,
    /** weights: time range end */
    SUMO_ATTR_END,
    /** link: the traffic light id responsible for this link */
    SUMO_ATTR_TLID,
    /** link: the index of the link within the traffic light */
    SUMO_ATTR_TLLINKNO,
    /** edge: the shape in xml-definition */
    SUMO_ATTR_SHAPE,
    /// The information about how to spread the lanes from the given position
    SUMO_ATTR_SPREADFUNC,
    /// A color information
    SUMO_ATTR_COLOR,
    /// The abstract direction of a link
    SUMO_ATTR_DIR,
    /// The state of a link
    SUMO_ATTR_STATE,
    /// A layer number
    SUMO_ATTR_LAYER,
    /// Fill the polygon
    SUMO_ATTR_FILL,
    SUMO_ATTR_PREFIX,
    SUMO_ATTR_DISCARD,

    SUMO_ATTR_DEST,
    SUMO_ATTR_SOURCE,
    SUMO_ATTR_VIA,
    SUMO_ATTR_INTERNALEND,
#ifdef _MESSAGES
    // Attributes for message emitter
    /// what events to emit
    SUMO_ATTR_EVENTS,
    /// reversed order?
    SUMO_ATTR_REVERSE,
    /// table output?
    SUMO_ATTR_TABLE,
    /// xy coordinates?
    SUMO_ATTR_XY,
    /// Steps (for heartbeat)
    SUMO_ATTR_STEP,
    // Attribute for detectors
    /// What message to emit?
    SUMO_ATTR_MSG,
#endif
    // Attributes for actuated traffic lights:
    /// minimum duration of a phase
    SUMO_ATTR_MINDURATION,
    /// maximum duration of a phase
    SUMO_ATTR_MAXDURATION,
    // Attributes for junction-internal lanes
    /// Information within the junction logic which internal lanes block external
    SUMO_ATTR_FOES,
    // Attributes for detectors
    /// Measures to retrieve
    SUMO_ATTR_MEASURES,
    /// Information whether the detector shall be continued on the folowing lanes
    SUMO_ATTR_CONT,
    SUMO_ATTR_HALTING_TIME_THRESHOLD,
    SUMO_ATTR_HALTING_SPEED_THRESHOLD,
    SUMO_ATTR_JAM_DIST_THRESHOLD,
    SUMO_ATTR_DELETE_DATA_AFTER_SECONDS,
    SUMO_ATTR_DET_OFFSET,

    SUMO_ATTR_WAUT_ID,
    SUMO_ATTR_JUNCTION_ID,
    SUMO_ATTR_PROCEDURE,
    SUMO_ATTR_SYNCHRON,
    SUMO_ATTR_REF_TIME,
    SUMO_ATTR_START_PROG,

    SUMO_ATTR_OFF,
    SUMO_ATTR_FRIENDLY_POS,
    SUMO_ATTR_UNCONTROLLED,
    SUMO_ATTR_PASS,
    SUMO_ATTR_FORCE_LENGTH,
    SUMO_ATTR_BUS_STOP,
    SUMO_ATTR_LINE,
    SUMO_ATTR_LINES,
    SUMO_ATTR_VCLASSES,
    SUMO_ATTR_VALUE,
    SUMO_ATTR_PROHIBITOR,
    SUMO_ATTR_PROHIBITED,
    SUMO_ATTR_ALLOW,
    SUMO_ATTR_DISALLOW,
    SUMO_ATTR_PREFER,
    SUMO_ATTR_CONTROLLED_INNER,
    SUMO_ATTR_VEHSPERHOUR,
    SUMO_ATTR_OUTPUT,
    SUMO_ATTR_GUIWIDTH,
    SUMO_ATTR_GUIOFFSET,
    SUMO_ATTR_GUISHAPE,
    SUMO_ATTR_EMISSIONCLASS,
    SUMO_ATTR_STARTPOS,
    SUMO_ATTR_ENDPOS,
    SUMO_ATTR_TRIGGERED,
    SUMO_ATTR_PARKING,
    SUMO_ATTR_INDEX,

    SUMO_ATTR_ENTERING,
    SUMO_ATTR_EXCLUDE_EMPTY,
    SUMO_ATTR_WITH_INTERNAL,
    SUMO_ATTR_TRACK_VEHICLES,
    SUMO_ATTR_MAX_TRAVELTIME,
    SUMO_ATTR_MIN_SAMPLES,

    SUMO_ATTR_LON,
    SUMO_ATTR_LAT,
    SUMO_ATTR_K,
    SUMO_ATTR_V,
    SUMO_ATTR_REF,
    SUMO_ATTR_HREF,
    SUMO_ATTR_ZOOM,
    SUMO_ATTR_INTERPOLATED,
    SUMO_ATTR_THRESHOLD,

    SUMO_ATTR_NET_OFFSET,
    SUMO_ATTR_CONV_BOUNDARY,
    SUMO_ATTR_ORIG_BOUNDARY,
    SUMO_ATTR_ORIG_PROJ,

    SUMO_ATTR_CF_IDM_TIMEHEADWAY,
    SUMO_ATTR_CF_IDM_MINGAP,
    //SUMO_ATTR_CF_KERNER_K,
    SUMO_ATTR_CF_KERNER_PHI,

    SUMO_ATTR_OPENDRIVE_REVMAJOR,
    SUMO_ATTR_OPENDRIVE_REVMINOR,
    SUMO_ATTR_OPENDRIVE_ID,
    SUMO_ATTR_OPENDRIVE_LENGTH,
    SUMO_ATTR_OPENDRIVE_JUNCTION,
    SUMO_ATTR_OPENDRIVE_ELEMENTTYPE,
    SUMO_ATTR_OPENDRIVE_ELEMENTID,
    SUMO_ATTR_OPENDRIVE_CONTACTPOINT,
    SUMO_ATTR_OPENDRIVE_S,
    SUMO_ATTR_OPENDRIVE_X,
    SUMO_ATTR_OPENDRIVE_Y,
    SUMO_ATTR_OPENDRIVE_HDG,
    SUMO_ATTR_OPENDRIVE_CURVSTART,
    SUMO_ATTR_OPENDRIVE_CURVEND,
    SUMO_ATTR_OPENDRIVE_CURVATURE,
    SUMO_ATTR_OPENDRIVE_A,
    SUMO_ATTR_OPENDRIVE_B,
    SUMO_ATTR_OPENDRIVE_C,
    SUMO_ATTR_OPENDRIVE_D,
    SUMO_ATTR_OPENDRIVE_TYPE,
    SUMO_ATTR_OPENDRIVE_LEVEL,

    SUMO_ATTR_GENERATE_WALKS,

    /**
     * ActivityGen Tags
     **/
    /* general object */
    AGEN_ATTR_INHABITANTS,
    AGEN_ATTR_HOUSEHOLDS,
    AGEN_ATTR_CHILDREN,
    AGEN_ATTR_RETIREMENT,
    AGEN_ATTR_CARS,
    AGEN_ATTR_UNEMPLOYEMENT,
    AGEN_ATTR_MAX_FOOT_DIST,
    AGEN_ATTR_IN_TRAFFIC,
    AGEN_ATTR_OUT_TRAFFIC,

    /* streets object */
    //SUMO_ATTR_EDGE already defined
    AGEN_ATTR_POPULATION,
    AGEN_ATTR_OUT_WORKPOSITION,

    /* workHours object */
    AGEN_ATTR_HOUR,
    AGEN_ATTR_PROP,

    /* school object */
    //SUMO_ATTR_EDGE, SUMO_ATTR_POSITION, SUMO_ATTR_TYPE already defined
    AGEN_ATTR_CAPACITY,
    AGEN_ATTR_OPENING,
    AGEN_ATTR_CLOSING,

    /* busStation and Bus objects */
    // ID, EDGE, POSITION, REFID, BEGIN and END are already defined
    AGEN_ATTR_MAX_TRIP_DURATION,
    //AGEN_ATTR_ORDER,
    AGEN_ATTR_RATE,

    /* population and children accompaniment brackets */
    AGEN_ATTR_BEGINAGE,
    AGEN_ATTR_ENDAGE,
    AGEN_ATTR_PEOPLENBR,

    /* parameters */
    AGEN_ATTR_CARPREF,
    AGEN_ATTR_CITYSPEED,
    AGEN_ATTR_FREETIMERATE,
    AGEN_ATTR_UNI_RAND_TRAFFIC,
    AGEN_ATTR_DEP_VARIATION,

    /* city gates */
    AGEN_ATTR_INCOMING,
    AGEN_ATTR_OUTGOING

};


#endif

/****************************************************************************/

