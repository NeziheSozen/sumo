/****************************************************************************/
/// @file    GUIDialog_AboutSUMO.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
// The application's "About" - dialog
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
#ifndef GUIDialog_AboutSUMO_h
#define GUIDialog_AboutSUMO_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIDialog_AboutSUMO
 * @brief The application's "About" - dialog
 */
class GUIDialog_AboutSUMO : public FXDialogBox {
public:
    /** @brief Constructor
     *
     * @param[in] parent The parent window
     * @param[in] name The title to show
     * @param[in] x x-position of the window to use
     * @param[in] y y-position of the window to use
     */
    GUIDialog_AboutSUMO(FXWindow* parent,  const char* name, int x, int y) throw();


    /// @brief Destructor
    ~GUIDialog_AboutSUMO() throw();


    /** @brief Creates the widget (and the icons)
     */
    void create();


private:
    /// @brief Icons for the widget
    FXIcon *myDLRIcon;

};


#endif

/****************************************************************************/

