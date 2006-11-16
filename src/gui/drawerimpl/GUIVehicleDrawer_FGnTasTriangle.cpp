//---------------------------------------------------------------------------//
//                        GUIVehicleDrawer_FGnTasTriangle.cpp -
//  Class for drawing vehicles on full geom with no tooltips as triangles
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 02.09.2003
//  copyright            : (C) 2003 by Daniel Krajzewicz
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.18  2006/11/16 10:50:43  dkrajzew
// warnings removed
//
// Revision 1.17  2006/11/14 06:40:30  dkrajzew
// removed unneeded code
//
// Revision 1.16  2006/10/12 09:28:14  dkrajzew
// patched building under windows
//
// Revision 1.15  2006/10/12 07:50:43  dkrajzew
// c2c visualisation added (unfinished)
//
// Revision 1.14  2006/10/04 13:18:13  dkrajzew
// debugging internal lanes, multiple vehicle emission and net building
//
// Revision 1.13  2006/07/07 11:51:51  dkrajzew
// further work on lane changing
//
// Revision 1.12  2006/07/06 06:26:44  dkrajzew
// added blinker visualisation and vehicle tracking (unfinished)
//
// Revision 1.11  2006/03/17 11:03:01  dkrajzew
// made access to positions in Position2DVector c++ compliant
//
// Revision 1.10  2006/01/09 11:50:21  dkrajzew
// new visualization settings implemented
//
// Revision 1.9  2005/10/07 11:36:48  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.8  2005/09/22 13:30:40  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.7  2005/09/15 11:05:29  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.6  2005/07/12 11:58:17  dkrajzew
// visualisation of large vehicles added
//
// Revision 1.5  2005/04/27 09:44:26  dkrajzew
// level3 warnings removed
//
// Revision 1.4  2004/11/23 10:05:22  dkrajzew
// removed some warnings and adapted the new class hierarchy
//
// Revision 1.3  2004/03/19 12:34:30  dkrajzew
// porting to FOX
//
// Revision 1.2  2003/09/17 06:45:11  dkrajzew
// some documentation added/patched
//
// Revision 1.1  2003/09/05 14:50:39  dkrajzew
// implementations of artefact drawers moved to folder "drawerimpl"
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

#include <guisim/GUIVehicle.h>
#include <guisim/GUILaneWrapper.h>
#include <guisim/GUIEdge.h>
#include <utils/geom/Position2DVector.h>
#include "GUIVehicleDrawer_FGnTasTriangle.h"
#include <utils/glutils/GLHelper.h>
#include <microsim/MSAbstractLaneChangeModel.h>
#include <microsim/MSGlobals.h>
#include <microsim/lanechanging/MSLCM_DK2004.h>

#include <utils/gui/div/GUIGlobalSelection.h>
#include <guisim/GUIVehicle.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUIVehicleDrawer_FGnTasTriangle::GUIVehicleDrawer_FGnTasTriangle(
        std::vector<GUIEdge*> &edges)
    : GUIBaseVehicleDrawer(edges)
{
}


GUIVehicleDrawer_FGnTasTriangle::~GUIVehicleDrawer_FGnTasTriangle()
{
}


std::map<int, FXColor> colorBla;

void
GUIVehicleDrawer_FGnTasTriangle::drawLanesVehicles(GUILaneWrapper &lane,
        GUIBaseColorer<GUIVehicle> &colorer, float upscale, bool showBlinker)
{
    // retrieve vehicles from lane; disallow simulation
    const MSLane::VehCont &vehicles = lane.getVehiclesSecure();
    const DoubleVector &lengths = lane.getShapeLengths();
    const DoubleVector &rots = lane.getShapeRotations();
    const Position2DVector &geom = lane.getShape();
    const Position2D &laneBeg = geom[0];

    MSLane::VehCont::const_iterator v;
    for(v=vehicles.begin(); v!=vehicles.end(); v++) {
        MSVehicle *veh = *v;
        /*
            if(true&&veh->isEquipped()) {
                const MSVehicle::VehCont &neigh = veh->getConnections();
                for(MSVehicle::VehCont::const_iterator q=neigh.begin(); q!=neigh.end(); ++q) {
                    Position2D pos = (*q).second->connectedVeh->getPosition();
                    //pos.sub(veh->getPosition());
                    glBegin(GL_LINES);
                    glVertex2f(veh->getPosition().x(), veh->getPosition().y());
                    glVertex2f(pos.x(), pos.y());
                    glEnd();
                }
            }
            */
    }

    glPushMatrix();
    glTranslated(laneBeg.x(), laneBeg.y(), 0);
    glRotated(rots[0], 0, 0, 1);
    // go through the vehicles
    size_t shapePos = 0;
    SUMOReal positionOffset = 0;
    for(v=vehicles.begin(); v!=vehicles.end(); v++) {
        MSVehicle *veh = *v;
        SUMOReal vehiclePosition = veh->getPositionOnLane();
        while( shapePos<rots.size()-1
               &&
               vehiclePosition>positionOffset+lengths[shapePos]) {

            glPopMatrix();
            positionOffset += lengths[shapePos];
            shapePos++;
            glPushMatrix();
            glTranslated(geom[shapePos].x(), geom[shapePos].y(), 0);
            glRotated(rots[shapePos], 0, 0, 1);
        }
        glPushMatrix();
        glTranslated(0, -(vehiclePosition-positionOffset), 0);
            if(true&&veh->isEquipped()) {
                int cluster = veh->getClusterId();
				if(veh->getConnections().size()==0){
					cluster = -1;
				}
                if(colorBla.find(cluster)==colorBla.end()) {
                    int r = (int) ((double) rand() / (double) RAND_MAX * 255.);
                    int g = (int) ((double) rand() / (double) RAND_MAX * 255.);
                    int b = (int) ((double) rand() / (double) RAND_MAX * 255.);
                    colorBla[cluster] = FXRGB(r, g, b);
                }
                FXColor c = colorBla[cluster];
                glColor3f(
					(float) ((float) FXREDVAL(c) /255.),
					(float) ((float) FXGREENVAL(c) /255.),
					(float) ((float) FXBLUEVAL(c) /255.));
                GLHelper::drawOutlineCircle(MSGlobals::gLANRange, MSGlobals::gLANRange-2, 24);
            }
        drawVehicle(static_cast<GUIVehicle&>(*veh), colorer, upscale);
        if(showBlinker) {
            int state = veh->getLaneChangeModel().getState();
            glColor3f(1, 0, 1);
            if((state&LCA_URGENT)!=0) {
                glColor3f(1, 0, 0);
            }
            if((state&LCA_SPEEDGAIN)!=0) {
                glColor3f(.5, 0, 0);
            }
            if((state&LCA_LEFT)!=0) {
                glTranslated(1, .5, 0);
                GLHelper::drawFilledCircle(.5, 8);
                glTranslated(-1, -.5, 0);
            }
            if((state&LCA_RIGHT)!=0) {
                glTranslated(-1, .5, 0);
                GLHelper::drawFilledCircle(.5, 8);
                glTranslated(1, -.5, 0);
            }
            glColor3f(1, 0, 1);
/*
            MSLinkCont::const_iterator link =
                veh->getLane().succLinkSec( *veh, 1, veh->getLane() );
            if(link!=veh->getLane().getLinkCont().end()) {
                switch((*link)->getDirection()) {
                case MSLink::LINKDIR_TURN:
                case MSLink::LINKDIR_LEFT:
                case MSLink::LINKDIR_PARTLEFT:
                    glTranslated(1, .5, 0);
                    GLHelper::drawFilledCircle(.5, 8);
                    glTranslated(-1, -.5, 0);
                    break;
                case MSLink::LINKDIR_RIGHT:
                case MSLink::LINKDIR_PARTRIGHT:
                    glTranslated(-1, .5, 0);
                    GLHelper::drawFilledCircle(.5, 8);
                    glTranslated(1, -.5, 0);
                    break;
                default:
                    break;
                }
            }
*/
            if(true) {//!!!
                MSLCM_DK2004 &m = static_cast<MSLCM_DK2004&>(veh->getLaneChangeModel());
                glColor3f(.5, .5, 1);
                glBegin(GL_LINES);
                glVertex2f(0, 0);
                glVertex2f(m.getChangeProbability(), .5);
                glEnd();
            }
            /*
            if(true) {//!!!
                glColor3f(1, .5, 0);
                std::vector<MSVehicle::LaneQ> preb = veh->getBestLanes()[0];
                int midx = 0;
                SUMOReal maxLen = 0;
                int o;
                for(o=0; o<preb.size(); o++) {
                    if(preb[o].lane==&veh->getLane()) {
                        midx = o;
                    }
                    maxLen = MAX2(maxLen, preb[o].length);
                }
                for(o=0; o<preb.size(); o++) {
                    glBegin(GL_LINES);
                    glVertex2f(0.4 * (SUMOReal) (o-midx), 0);
                    glVertex2f(0.4 * (SUMOReal) (o-midx), preb[o].length/maxLen/2.);
                    glEnd();
                }
            }
                /
                const MSLane &l = veh->getLane();
                SUMOReal r1 = veh->allowedContinuationsLength(&l, 0);
                SUMOReal r2 = l.getLeftLane()!=0 ? veh->allowedContinuationsLength(l.getLeftLane(), 0) : 0;
                SUMOReal r3 = l.getRightLane()!=0 ? veh->allowedContinuationsLength(l.getRightLane(), 0) : 0;
                SUMOReal mmax = MAX3(r1, r2, r3);
                glBegin(GL_LINES);
                glVertex2f(0, 0);
                glVertex2f(0, r1/mmax/2.);
                glEnd();
                glBegin(GL_LINES);
                glVertex2f(.4, 0);
                glVertex2f(.4, r2/mmax/2.);
                glEnd();
                glBegin(GL_LINES);
                glVertex2f(-.4, 0);
                glVertex2f(-.4, r3/mmax/2.);
                glEnd();
                /
            */
        }
        glPopMatrix();
    }
    // allow lane simulation
    lane.releaseVehicles();
    glPopMatrix();
}


void
GUIVehicleDrawer_FGnTasTriangle::drawVehicle(const GUIVehicle &vehicle,
            GUIBaseColorer<GUIVehicle> &colorer, float upscale)
{
    glScaled(upscale, upscale, upscale);
    colorer.setGlColor(vehicle);
    SUMOReal length = vehicle.getLength();
    if(length<8) {
/*
        if(scheme!=GUISUMOAbstractView::VCS_LANECHANGE3) {
        */
        glScaled(1, length, 1);//, 1);
    glBegin( GL_TRIANGLES );
            glVertex2d(0, 0);
            glVertex2d(0-1.25, 1);//length);
            glVertex2d(0+1.25, 1);//length);
    glEnd();
            /*
        } else {
            setVehicleColor1Of3(vehicle);
            glVertex2d(0, 0);
            setVehicleColor2Of3(vehicle);
            glVertex2d(0-1.25, length);
            setVehicleColor3Of3(vehicle);
            glVertex2d(0+1.25, length);
        }
        */
    } else {
        /*
        if(scheme!=GUISUMOAbstractView::VCS_LANECHANGE3) {
        */
    glBegin( GL_TRIANGLES );
            glVertex2d(0, 0);
            glVertex2d(0-1.25, 0+2);
            glVertex2d(0+1.25, 0+2);
            glVertex2d(0-1.25, 2);
            glVertex2d(0-1.25, length);
            glVertex2d(0+1.25, length);
            glVertex2d(0+1.25, 2);
            glVertex2d(0-1.25, 2);
            glVertex2d(0+1.25, length);
    glEnd();
            /*
        } else {
            setVehicleColor1Of3(vehicle);
            glVertex2d(0, 0);
            glVertex2d(0-1.25, 0+2);
            glVertex2d(0+1.25, 0+2);
            setVehicleColor2Of3(vehicle);
            glVertex2d(0-1.25, 2);
            glVertex2d(0-1.25, length);
            glVertex2d(0+1.25, length);
            setVehicleColor3Of3(vehicle);
            glVertex2d(0+1.25, 2);
            glVertex2d(0-1.25, 2);
            glVertex2d(0+1.25, length);
        }
        */
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


