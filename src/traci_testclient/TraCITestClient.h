/****************************************************************************/
/// @file    TraCITestClient.h
/// @author  Friedemann Wesner <wesner@itm.uni-luebeck.de>
/// @date    2008/04/07
/// @version $Id$
///
/// A dummy client to simulate communication to a TraCI server
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
#ifndef TRACITESTCLIENT_H
#define TRACITESTCLIENT_H

// ===========================================================================
// included modules
// ===========================================================================
#include <string>
#include <sstream>
#include <vector>

#include <foreign/tcpip/socket.h>
#include <utils/common/SUMOTime.h>

namespace testclient {
// ===========================================================================
// Definitions
// ===========================================================================
struct Position2D {
    float x;
    float y;
};

struct Position3D {
    float x;
    float y;
    float z;
};

struct PositionRoadMap {
    std::string roadId;
    float pos;
    int laneId;
};

struct BoundingBox {
    Position2D lowerLeft;
    Position2D upperRight;
};

typedef std::vector<Position2D> Polygon;

struct TLPhase {
    std::string precRoadId;
    std::string succRoadId;
    int phase;
};

typedef std::vector<TLPhase> TLPhaseList;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TraCITestClient
 */
class TraCITestClient {

public:
    TraCITestClient(std::string outputFileName = "testclient_result.out");

    ~TraCITestClient();

    bool run(std::string fileName, int port, std::string host = "localhost");

    bool connect(int port, std::string host = "localhost");

    bool close();

    // simulation commands
    void commandSimulationStep(SUMOTime time, int posFormat);
    void commandSimulationStep2(SUMOTime time);

    void commandSetMaximumSpeed(int nodeId, float speed);

    void commandStopNode(int nodeId, testclient::Position2D pos, float radius, SUMOTime waitTime);
    void commandStopNode(int nodeId, testclient::Position3D pos, float radius, SUMOTime waitTime);
    void commandStopNode(int nodeId, testclient::PositionRoadMap pos, float radius, SUMOTime waitTime);

    void commandChangeLane(int nodeId, int laneId, SUMOTime fixTime);

    void commandSlowDown(int nodeId, float minSpeed, SUMOTime timeInterval);

    void commandChangeRoute(int nodeId, std::string roadId, double travelTime);

    void commandChangeTarget(int nodeId, std::string roadId);

    void commandPositionConversion(testclient::Position2D pos, int posId);
    void commandPositionConversion(testclient::Position3D pos, int posId);
    void commandPositionConversion(testclient::PositionRoadMap pos, int posId);

    void commandDistanceRequest(testclient::Position2D pos1, testclient::Position2D pos2, int flag);
    void commandDistanceRequest(testclient::Position3D pos1, testclient::Position3D pos2, int flag);
    void commandDistanceRequest(testclient::Position2D pos1, testclient::Position3D pos2, int flag);
    void commandDistanceRequest(testclient::Position3D pos1, testclient::Position2D pos2, int flag);
    void commandDistanceRequest(testclient::PositionRoadMap pos1, testclient::PositionRoadMap pos2, int flag);
    void commandDistanceRequest(testclient::PositionRoadMap pos1, testclient::Position2D pos2, int flag);
    void commandDistanceRequest(testclient::PositionRoadMap pos1, testclient::Position3D pos2, int flag);
    void commandDistanceRequest(testclient::Position2D pos1, testclient::PositionRoadMap pos2, int flag);
    void commandDistanceRequest(testclient::Position3D pos1, testclient::PositionRoadMap pos2, int flag);

    void commandScenario(int flag, int domain, int domainId, int variable, int valueDataType);
    void commandScenario(int flag, int domain, int domainId, int variable, std::string stringVal);
    void commandScenario(int flag, int domain, int domainId, int variable, testclient::Position3D pos3dVal);
    void commandScenario(int flag, int domain, int domainId, int variable, testclient::PositionRoadMap roadPosVal);

    void commandGetTLStatus(int tlId, SUMOTime intervalStart, SUMOTime intervalEnd);
    void commandGetVariable(int domID, int varID, const std::string &objID);
    void commandGetVariablePlus(int domID, int varID, const std::string &objID, std::ifstream &defFile);
    void commandSubscribeVariable(int domID, const std::string &objID, int beginTime, int endTime, int varNo, std::ifstream &defFile);
    void commandSetValue(int domID, int varID, const std::string &objID, std::ifstream &defFile);

    void commandClose();

private:
    void writeResult();

    void errorMsg(std::stringstream& msg);

    void commandStopNode(int nodeId, testclient::Position2D* pos2D,
                         testclient::Position3D* pos3D,
                         testclient::PositionRoadMap* posRoad,
                         float radius, SUMOTime waitTime);

    void commandPositionConversion(testclient::Position2D* pos2D,
                                   testclient::Position3D* pos3D,
                                   testclient::PositionRoadMap* posRoad,
                                   int posId);

    void commandDistanceRequest(testclient::Position2D* pos1_2D,
                                testclient::Position3D* pos1_3D,
                                testclient::PositionRoadMap* pos1_Road,
                                testclient::Position2D* pos2_2D,
                                testclient::Position3D* pos2_3D,
                                testclient::PositionRoadMap* pos2_Road,
                                int flag);

    void commandScenario(int flag, int domain, int domainId, int variable, int valueDataType,
                         int* intVal, int* byteVal, int* ubyteVal,
                         float* floatVal, double* doubleVal, std::string* stringVal,
                         testclient::Position3D* pos3dVal, testclient::PositionRoadMap* roadPosVal,
                         testclient::BoundingBox* boxVal, testclient::Polygon* polyVal,
                         testclient::TLPhaseList* tlphaseVal);

    // validation of received command responses
    bool reportResultState(tcpip::Storage& inMsg, int command, bool ignoreCommandId=false);

    bool validateSimulationStep(tcpip::Storage& inMsg);
    bool validateSimulationStep2(tcpip::Storage& inMsg);
    bool validateSubscription(tcpip::Storage& inMsg);

    bool validateStopNode(tcpip::Storage& inMsg);

    bool validatePositionConversion(tcpip::Storage& inMsg);

    bool validateDistanceRequest(tcpip::Storage& inMsg);

    bool validateScenario(tcpip::Storage& inMsg);

    bool validateGetTLStatus(tcpip::Storage& inMsg);

    bool readAndReportTypeDependent(tcpip::Storage &inMsg, int valueDataType);


    /** @brief Parses the next value type / value pair from the stream and inserts it into the storage
     *
     * @param[out] into The storage to add the value type and the value into
     * @param[in] defFile The file to read the values from
     * @param[out] msg If any error occurs, this should be filled
     * @return The number of written bytes
     */
    int setValueTypeDependant(tcpip::Storage &into, std::ifstream &defFile, std::stringstream &msg);



private:
    tcpip::Socket* socket;

    std::string outputFileName;

    std::stringstream answerLog;
};

}

#endif
