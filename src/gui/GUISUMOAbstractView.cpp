//---------------------------------------------------------------------------//
//                        GUISUMOAbstractView.cpp -
//  A view on the simulation; this views is a microscopic one
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.2  2003/06/05 06:26:16  dkrajzew
// first tries to build under linux: warnings removed; Makefiles added
//
// Revision 1.1  2003/05/20 09:25:13  dkrajzew
// new view hierarchy; some debugging done
//
// Revision 1.12  2003/04/16 09:50:04  dkrajzew
// centering of the network debugged; additional parameter of maximum display size added
//
// Revision 1.11  2003/04/14 08:24:57  dkrajzew
// unneeded display switch and zooming option removed; new glo-objct concept implemented; comments added
//
// Revision 1.10  2003/04/07 10:15:16  dkrajzew
// glut reinserted
//
// Revision 1.9  2003/04/04 15:13:20  roessel
// Commented out #include <glut.h>
// Added #include <qcursor.h>
//
// Revision 1.8  2003/04/04 08:37:51  dkrajzew
// view centering now applies net size; closing problems debugged; comments added; tootip button added
//
// Revision 1.7  2003/04/02 11:50:28  dkrajzew
// a working tool tip implemented
//
// Revision 1.6  2003/03/20 16:17:52  dkrajzew
// windows eol removed
//
// Revision 1.5  2003/03/12 16:55:19  dkrajzew
// centering of objects debugged
//
// Revision 1.3  2003/03/03 15:10:20  dkrajzew
// debugging
//
// Revision 1.2  2003/02/07 10:34:14  dkrajzew
// files updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <iostream>
#include <utility>
#include <cmath>
#include <guisim/GUINet.h>
#include <guisim/GUIEdge.h>
#include <guisim/GUILane.h>
#include <guisim/GUIVehicle.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <qgl.h>
#include <qevent.h>
#include <qpainter.h>
#include <qtoolbar.h>
#include <qdialog.h>
#include <qcombobox.h>
#include <qpixmap.h>
#include <qcursor.h>
#include <qpopupmenu.h>
#include <utils/gfx/RGBColor.h>
#include "QGUIToggleButton.h"
#include "QGUIImageField.h"
#include "QGLObjectToolTip.h"
#include "GUIChooser.h"
#include "GUISUMOViewParent.h"
#include "GUITriangleVehicleDrawer.h"
#include "GUISimpleLaneDrawer.h"
#include "GUIDanielPerspectiveChanger.h"
#include "GUISUMOAbstractView.h"
#include "QGLObjectPopupMenu.h"
#include "GUIApplicationWindow.h"

#include "icons/view_traffic/colour_lane.xpm"
#include "icons/view_traffic/colour_vehicle.xpm"
#include "icons/view_traffic/show_grid.xpm"
#include "icons/view_traffic/show_tooltips.xpm"


#ifdef WIN32
#include <glut.h>
#endif

#ifndef WIN32
#include "GUISUMOAbstractView.moc"
#endif

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUISUMOAbstractView::GUISUMOAbstractView(GUIApplicationWindow *app,
                                         GUISUMOViewParent *parent, GUINet &net)
    : QGLWidget(parent, ""),
    _app(app),
    _parent(parent),
    _net(net),
    _showGrid(false),
    _changer(0),
    _useToolTips(false),
    _noDrawing(0),
    _mouseHotspotX(cursor().hotSpot().x()),
    _mouseHotspotY(cursor().hotSpot().y()),
    _popup(0)
{
    // set window sizes
    setMinimumSize(100, 30);
    setBaseSize(_parent->getMaxGLWidth(), _parent->getMaxGLHeight());
    setMaximumSize(_parent->getMaxGLWidth(), _parent->getMaxGLHeight());
    // compute the net scale
    double nw = _net.getBoundery().getWidth();
    double nh = _net.getBoundery().getHeight();
    _netScale = (nw < nh ? nh : nw);
    // show the middle at the beginning
    _changer = new GUIDanielPerspectiveChanger(*this);
    _changer->setNetSizes((size_t) nw, (size_t) nh);
    _toolTip = new QGLObjectToolTip(this);
    setMouseTracking(true);
}


GUISUMOAbstractView::~GUISUMOAbstractView()
{
}


void
GUISUMOAbstractView::toggleShowGrid()
{
    _showGrid = !_showGrid;
    update();
}

void
GUISUMOAbstractView::toggleToolTips()
{
    _useToolTips = !_useToolTips;
    update();
}



void
GUISUMOAbstractView::mouseMoveEvent ( QMouseEvent *e )
{
    killTimers();
    _changer->mouseMoveEvent(e);
    QGLWidget::mouseMoveEvent(e);
}


void
GUISUMOAbstractView::mousePressEvent ( QMouseEvent *e )
{
    delete _popup;
    _popup = 0;
    if(_useToolTips) {
        _toolTip->hide();
    }
    killTimers();
    _changer->mousePressEvent(e);
    _timer = startTimer(250);
    assert(_timer!=0);
    _timerReason = e->button();
    QGLWidget::mousePressEvent(e);
}


void
GUISUMOAbstractView::mouseReleaseEvent ( QMouseEvent *e )
{
    _changer->mouseReleaseEvent(e);
    delete _popup;
    _popup = 0;
    killTimers();
    if(_useToolTips) {
        _toolTip->hide();
    }
    QGLWidget::mouseReleaseEvent(e);
}



bool
GUISUMOAbstractView::event( QEvent *e )
{
    if(e->type()==QEvent::MouseButtonDblClick) {
        cout << "doppelt" << endl;
    }
    return QGLWidget::event(e);
}


void
GUISUMOAbstractView::initializeGL()
{
    _lock.lock();
    _widthInPixels = _parent->getMaxGLWidth();
    _heightInPixels = _parent->getMaxGLHeight();
    _ratio = (double) _widthInPixels / (double) _heightInPixels;
    glViewport( 0, 0, _parent->getMaxGLWidth()-1, _parent->getMaxGLHeight()-1 );
    glClearColor( 1.0, 1.0, 1.0, 1 );
    _changer->otherChange();
    doInit();
    _lock.unlock();
}


void
GUISUMOAbstractView::resizeGL( int width, int height )
{
    _lock.lock();
    _widthInPixels = width;
    _heightInPixels = height;
    _changer->applyCanvasSize(width, height);
    _lock.unlock();
}


void
GUISUMOAbstractView::updateToolTip()
{
    if(!_useToolTips) {
        return;
    }
    makeCurrent();
    _lock.lock();
    _noDrawing++;
    // ...and return when drawing is already being done
    if(_noDrawing>1) {
        _noDrawing--;
        _lock.unlock();
        return;
    }
    // initialise the select mode
    unsigned int id = getObjectUnderCursor();
    showToolTipFor(id);
    // mark end-of-drawing
    _noDrawing--;
    _lock.unlock();
}

void
GUISUMOAbstractView::paintGL()
{
    _lock.lock();
    // return if the canvas is not visible
    if(!isVisible()) {
		_lock.unlock();
        return;
    }
    _noDrawing++;
    // ...and return when drawing is already being done
    if(_noDrawing>1) {
        _noDrawing--;
        _lock.unlock();
        return;
    }
    _widthInPixels = width();
    _heightInPixels = height();
    // draw
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    applyChanges(1.0, 0, 0);
    doPaintGL(GL_RENDER, 1.0);
    // check whether the select mode /tooltips)
    //  shall be computed, too
    if(!_useToolTips) {
        _noDrawing--;
        glFlush();
        swapBuffers();
        _lock.unlock();
        return;
    }
    // get the object under the cursor
    unsigned int id = getObjectUnderCursor();
    showToolTipFor(id);
    _noDrawing--;
    glFlush();
    swapBuffers();
    _lock.unlock();
}


unsigned int
GUISUMOAbstractView::getObjectUnderCursor()
{
//    const int SENSITIVITY = 2;
/*    double scale =
        _widthInPixels / _net.getBoundery().getWidth() < _heightInPixels / _net.getBoundery().getHeight()
        ? double(_widthInPixels)/double(SENSITIVITY)
        : double(_heightInPixels)/double(SENSITIVITY);*/
    const int SENSITIVITY = 4;
    const int NB_HITS_MAX = 1000;

    // Prepare the selection mode
    static GLuint hits[NB_HITS_MAX];
    static GLint nb_hits = 0;

    glSelectBuffer(NB_HITS_MAX, hits);
    glInitNames();

    double scale = double(getMaxGLWidth())/double(SENSITIVITY);

    applyChanges(scale, _toolTipX+_mouseHotspotX,
        _toolTipY+_mouseHotspotY);

    // paint in select mode
    bool tmp = _useToolTips;
    _useToolTips = true;
    doPaintGL(GL_SELECT, scale);
    _useToolTips = tmp;
    // Get the results
    nb_hits = glRenderMode(GL_RENDER);
    if(nb_hits==0) {
        return 0;
    }
//    glFlush();

    // Interpret results
    //  Vehicles should have a greater id
    //  than the previously allocated lanes
    unsigned int idMax = 0;
    for (int i=0; i<nb_hits; ++i) {
        assert (i*4+3<NB_HITS_MAX);
        if (hits[i*4+3] > idMax) {
            idMax = hits[i*4+3];
            assert (i*4+3<NB_HITS_MAX);
        }
    }
    return idMax;
}


void
GUISUMOAbstractView::showToolTipFor(unsigned int id)
{
    if(id!=0) {
        GUIGlObject *object = _net._idStorage.getObjectBlocking(id);
        _toolTip->setObjectTip(object, _mouseX, _mouseY);
        if(object!=0) {
            _net._idStorage.unblockObject(id);
        }
    } else {
        _toolTip->hide();
    }
}


void
GUISUMOAbstractView::paintGLGrid()
{
    glBegin( GL_LINES );
    glColor3f(0.5, 0.5, 0.5);
    double ypos = 0;
    double xpos = 0;
    double xend = (_net._edgeGrid.getNoXCells())
        * _net._edgeGrid.getXCellSize();
    double yend = (_net._edgeGrid.getNoYCells())
        * _net._edgeGrid.getYCellSize();
    for(size_t yr=0; yr<_net._edgeGrid.getNoYCells()+1; yr++) {
        glVertex2f(0, ypos);
        glVertex2f(xend, ypos);
        ypos += _net._edgeGrid.getYCellSize();
    }
    for(size_t xr=0; xr<_net._edgeGrid.getNoXCells()+1; xr++) {
        glVertex2f(xpos, 0);
        glVertex2f(xpos, yend);
        xpos += _net._edgeGrid.getXCellSize();
    }
    glEnd();
}


void
GUISUMOAbstractView::applyChanges(double scale,
                             size_t xoff, size_t yoff)
{
    _widthInPixels = width();
    _heightInPixels = height();
//    _ratio = (double) _widthInPixels / (double) _heightInPixels;
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    // rotate first;
    //  This is quite unchecked, so it's meaning and position is quite
    //  unclear
    glRotated(_changer->getRotation(), 0, 0, 1);
    // Fit the view's size to the size of the net
    glScaled(
        2.0/_netScale,
        2.0/_netScale,
        0);
    // apply ratio between window width and height
    glScaled(1/_ratio, 1, 0);
    // initially (zoom=100), the net shall be completely visible on the screen
    double xs = ((double) _widthInPixels / (double) _parent->getMaxGLWidth())
        / (_net.getBoundery().getWidth() / _netScale) * _ratio;
    double ys = ((double) _heightInPixels / (double) _parent->getMaxGLHeight())
        / (_net.getBoundery().getHeight() / _netScale);
    if(xs<ys) {
        glScaled(xs, xs, 1);
        _addScl = xs;
    } else {
        glScaled(ys, ys, 1);
        _addScl = ys;
    }
    // initially, leave some room for the net
    glScaled(0.97, 0.97, 1);
    _addScl *= .97;

    // Apply the zoom and the scale
    double zoom = _changer->getZoom() / 100.0 * scale;
    glScaled(zoom, zoom, 0);
    // Translate to the middle of the net
    glTranslated(
        -(_net.getBoundery().getCenter().first),
        -(_net.getBoundery().getCenter().second),
        0);
    // Translate in dependence to the view position applied by the user
    glTranslated(_changer->getXPos(), _changer->getYPos(), 0);
    // Translate to the mouse pointer, when wished
    if(xoff!=0||yoff!=0) {
        double absX = (double(xoff)-(double(_widthInPixels)/2.0));
        double absY = (double(yoff)-(double(_heightInPixels)/2.0));
        glTranslated(
            -p2m(absX),
            p2m(absY),
            0);
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // apply the widow size
    double xf = -1.0 *
        ((double) _parent->getMaxGLWidth() - (double) _widthInPixels)
        / (double) _parent->getMaxGLWidth();
    double yf = -1.0 *
        ((double) _parent->getMaxGLHeight() - (double) _heightInPixels)
        / (double) _parent->getMaxGLHeight();
    glTranslated(xf, yf, 0);
    _changer->applied();
}


void
GUISUMOAbstractView::displayLegend()
{
    size_t length = 1;
    string text = "1";
    while(true) {
        size_t pixelSize = (size_t) m2p((double) length);
        if(pixelSize>20) {
            QPainter paint( this );
            paint.setPen( QColor(0, 0, 0) );
            size_t h = height();
            paint.drawLine( 10, h-10, 10+pixelSize, h-10 );
            paint.drawLine( 10, h-10, 10, h-15);
            paint.drawLine( 10+pixelSize, h-10, 10+pixelSize, h-15);
            paint.drawText( 10, h-10, QString("0m"));
            text = text + string("m");
            paint.drawText( 10+pixelSize, h-10, QString(text.c_str()));
            return;
        }
        length *= 10;
        text = text + string("0");
    }
}


double
GUISUMOAbstractView::m2p(double meter)
{
    return
        (meter/_netScale
        * (_parent->getMaxGLWidth()/_ratio)
        * _addScl * _changer->getZoom() / 100.0);
}


double
GUISUMOAbstractView::p2m(double pixel)
{
    return
        pixel * _netScale /
        ((_parent->getMaxGLWidth()/_ratio) * _addScl *_changer->getZoom() / 100.0);
}


void
GUISUMOAbstractView::recenterView()
{
    _changer->recenterView();
}


void
GUISUMOAbstractView::centerTo(GUIGlObjectType type,
                              const std::string &name)
{
    switch(type) {
    case GLO_JUNCTION:
        {
            Position2D pos = _net.getJunctionPosition(name);
            centerTo(pos, 50); // !!! the radius should be variable
        }
        break;
    case GLO_LANE:
        {
            Boundery bound =
                _net.getEdgeBoundery(name.substr(0, name.find_last_of("_")));
            bound.grow(20.0);
            centerTo(bound);
        }
        break;
    case GLO_EDGE:
        {
            Boundery bound = _net.getEdgeBoundery(name);
            bound.grow(20.0);
            centerTo(bound);
        }
        break;
    case GLO_VEHICLE:
        {
            if(_net.vehicleExists(name)) {
                Position2D pos = _net.getVehiclePosition(name);
                centerTo(pos, 20); // !!! another radius?
            }
        }
        break;
    default:
        // should not happen
        throw 1;
    }
    _changer->otherChange();
    update();
}


void
GUISUMOAbstractView::centerTo(Position2D pos, double radius)
{
    _changer->centerTo(_net.getBoundery(), pos, radius);
}


void
GUISUMOAbstractView::centerTo(Boundery bound)
{
    _changer->centerTo(_net.getBoundery(), bound);
}


bool
GUISUMOAbstractView::allowRotation() const
{
    return _parent->allowRotation();
}


void
GUISUMOAbstractView::setTooltipPosition(size_t x, size_t y,
                                   size_t mouseX, size_t mouseY)
{
    _toolTipX = x;
    _toolTipY = y;
    _mouseX = mouseX;
    _mouseY = mouseY;
}


void
GUISUMOAbstractView::paintEvent ( QPaintEvent *e )
{
/*    // do not paint anything when the canvas is
    //  not visible
    if(!isVisible()) {
        return;
    }
    // mark that drawing is in process
    _noDrawing++;
    // ...and return when drawing is already
    //  being done
    if(_noDrawing>1) {
        _noDrawing--;
        return;
    }
    makeCurrent();
    glDraw();*/
    QGLWidget::paintEvent(e);
    if(_parent->showLegend()) {
        displayLegend();
    }
}



void
GUISUMOAbstractView::timerEvent ( QTimerEvent *e )
{
    killTimers();
    switch(_timerReason) {
    case Qt::LeftButton:
        cout << "Left Button" << endl;
        break;
    case Qt::RightButton:
        {
            makeCurrent();
            _lock.lock();
            _noDrawing++;
            // ...and return when drawing is already being done
            if(_noDrawing>1) {
                _noDrawing--;
                _lock.unlock();
                return;
            }
            // initialise the select mode
            unsigned int id = getObjectUnderCursor();
            if(id!=0) {
                 GUIGlObject *o = _net._idStorage.getObjectBlocking(id);
                 if(o!=0) {
                    _popup = o->getPopUpMenu(_app, this);
                    _popup->setGeometry(
                        _mouseX, _mouseY,
                        _popup->width()+_mouseX, _popup->height()+_mouseY);
                    _popup->show();
                    _changer->mouseReleaseEvent(0);
                 }
            }
            _noDrawing--;
            _lock.unlock();
        }
        break;
    default:
        break;
    }
    // !!! what to do with e?
}


void
GUISUMOAbstractView::makeCurrent()
{
    _lock.lock();
    QGLWidget::makeCurrent();
	_lock.unlock();
}



int
GUISUMOAbstractView::getMaxGLWidth() const
{
    return _parent->getMaxGLWidth();
}

int
GUISUMOAbstractView::getMaxGLHeight() const
{
    return _parent->getMaxGLHeight();
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUISUMOAbstractView.icc"
//#endif

// Local Variables:
// mode:C++
// End:


