#ifndef MSEDDETECTORINTERFACE_H
#define MSEDDETECTORINTERFACE_H

///
/// @file    MSEDDetectorInterface.h
/// @author  Christian Roessel <christian.roessel@dlr.de>
/// @date    Started Tue Dec 02 2003 20:04 CET
///
/// @brief
///
///

/* Copyright (C) 2003 by German Aerospace Center (http://www.dlr.de) */

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.6  2006/10/25 12:22:37  dkrajzew
// updated
//
// Revision 1.5  2005/10/07 11:37:46  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.4  2005/09/15 11:09:33  dkrajzew
// LARGE CODE RECHECK
//
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

#include "MSDetectorInterfaceCommon.h"
#include <string>

/// @namespace ED The namespace ED contains the specifics of the ED
/// (event data) detectors. These detector update their value if a
/// specific event happens (e.g. a vehicle halts).
/// @see namespace LD, namespace TD

namespace ED
{
    /// This abstract class declares the particular methods for the ED
    /// (event data) detectors. Here, there are no particular methods
    /// like in TD::MSDetectorInterface and
    /// LD::MSDetectorInterface. The particular method update() of a
    /// concrete ED::MSDetector is inherited there.
    class MSDetectorInterface
        :
        public MSDetectorInterfaceCommon
    {
    public:
        /// Dtor.
        virtual ~MSDetectorInterface( void )
            {}

    protected:

        /// Ctor. Passed the id to base class.
        ///
        /// @param id The detector's id.
        MSDetectorInterface( std::string id )
            : MSDetectorInterfaceCommon( id )
            {}

    private:

    };

} // end namespace ED


// Local Variables:
// mode:C++
// End:

#endif // MSEDDETECTORINTERFACE_H
