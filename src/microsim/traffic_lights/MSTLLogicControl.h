/****************************************************************************/
/// @file    MSTLLogicControl.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A class that stores and controls tls and switching of their programs
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
#ifndef MSTLLogicControl_h
#define MSTLLogicControl_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <map>
#include "MSTrafficLightLogic.h"
#include "MSSimpleTrafficLightLogic.h"
#include <utils/common/Command.h>
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSTLLogicControl
 * @brief A class that stores and controls tls and switching of their programs
 *
 * This class holds all traffic light logics (programs) and their
 *  variants during the simulation. In addition, the schedule for switching
 *  between different tls programs are also stored.
 *
 * When a WAUT is forced to switch, for each TLS, a switching procedure
 *  derived from WAUTSwitchProcedure is initialised and is asked repeatedly
 *  whether a switch could be done until it returns true.
 */
class MSTLLogicControl {
public:
    /**
     * @class OnSwitchAction
     * @brief Base class for things to execute if a tls switches
     * @see Design Patterns, Gamma et al.
     */
    class OnSwitchAction {
    public:
        /// @brief Destructor.
        virtual ~OnSwitchAction() throw() {};


        /** @brief Executes the action
         *
         */
        virtual void execute() throw() = 0;

    };



    /**
     * @class TLSLogicVariants
     * @brief Storage for all programs of a single tls
     *
     * This class joins all programs of a single tls.
     */
    class TLSLogicVariants {
    public:
        /// @brief Constructor
        TLSLogicVariants() throw();


        /// @brief Destructor
        ~TLSLogicVariants() throw();


        /** @brief Verifies traffic lights loaded from the network
         *
         * Comparse the phase state sizes of each phase with the accoring tls' number
         *  of controlled links.
         * @return Whether all tls programs are valid
         */
        bool checkOriginalTLS() const throw();


        /** @brief Adds a logic (program)
         *
         * @param[in] programID The sub-id of this program
         * @param[in] logic The logic to add
         * @param[in] netWasLoaded Whether the network has already been loaded (the links have been assigned)
         * @param[in] isNewDefault Whether this logic shall be treated as the currently active logic
         */
        bool addLogic(const std::string &programID, MSTrafficLightLogic*logic, bool netWasLoaded,
                      bool isNewDefault=true) throw(ProcessError);



        MSTrafficLightLogic* getLogic(const std::string &programID) const;
        void addSwitchCommand(OnSwitchAction *c);
        std::vector<MSTrafficLightLogic*> getAllLogics() const;
        void saveInitialStates();
        bool isActive(const MSTrafficLightLogic *tl) const;
        MSTrafficLightLogic* getActive() const;
        bool switchTo(MSTLLogicControl &tlc, const std::string &programID);
        MSTrafficLightLogic*getLogicInstantiatingOff(MSTLLogicControl &tlc,
                const std::string &programID);
        void executeOnSwitchActions() const;
        void addLink(MSLink *link, MSLane *lane, unsigned int pos) throw();



    private:
        /// @brief The currently used program
        MSTrafficLightLogic *myCurrentProgram;

        /// @brief A map of subkeys to programs
        std::map<std::string, MSTrafficLightLogic*> myVariants;

        /// @brief Originally loaded link states
        std::map<MSLink*, MSLink::LinkState> myOriginalLinkStates;

        /// @brief The list of actions/commands to execute on switch
        std::vector<OnSwitchAction*> mySwitchActions;


    private:
        /// @brief Invalidated copy constructor.
        TLSLogicVariants(const TLSLogicVariants&);

        /// @brief Invalidated assignment operator.
        TLSLogicVariants& operator=(const TLSLogicVariants&);


    };


    /// @brief Constructor
    MSTLLogicControl() throw();


    /// @brief Destructor
    ~MSTLLogicControl() throw();


    /** @brief Lets MSTLLogicControl know that the network has been loaded
     *
     * This method must be called after the network (including the initial tls
     *  definitions) was loaded.
     *
     * The originally loaded traffic lights are also verified herein by calling
     *  TLSLogicVariants::checkOriginalTLS, first.
     *
     * The MSTLLogicControl is informed in order to know that link information
     *  is known for the tls programs loaded afterwards so that it may be adapted
     *  from the previously loaded tls (a net may only contain one program per tls).
     *
     * The states of the links controlled by tls are saved for their potential later usage
     *  (if the tls is switched to off-mode).
     *
     * @return Whether the traffic lights could be initialised and are correct
     */
    bool closeNetworkReading() throw();


    /** @brief Lets all running (current) tls programs apply their current signal states to links they control
     * @param[in] t The current time
     * @see MSTrafficLightLogic::setTrafficLightSignals
     * @see MSLink::LinkState
     * @see MSLink::setTLState
     */
    void setTrafficLightSignals(SUMOTime t) const throw();


    /** @brief Returns a vector which contains all logics
     *
     * All logics are included, active (current) and non-active
     * @return A vector containing all loaded logics
     */
    std::vector<MSTrafficLightLogic*> getAllLogics() const;


    /** @brief Returns the variants of a named tls
     *
     * @param[in] id The id of the tls to get variants of
     * @return The variants of the named tls
     */
    TLSLogicVariants &get(const std::string &id) const throw(InvalidArgument);


    /** @brief Returns a single program (variant) defined by the tls id and the program programID
     *
     * @param[in] id The id of the tls to get program of
     * @param[in] programID The program id of the tls program to get
     * @return The defined tls program if existing, 0 otherwise
     */
    MSTrafficLightLogic * const get(const std::string &id, const std::string &programID) const;


    /** @brief Returns the active program of a named tls
     *
     * @param[in] id The id of the tls to get the active program of
     * @return The current program of the defined tls if existing, 0 otherwise
     */
    MSTrafficLightLogic * const getActive(const std::string &id) const;

    /**
     * Returns the ids of all existing variants-structures, wich are the ids of their
     * contained tls logics (not the logic's programm-ids)
     * @return the list of ids
     */
    std::vector<std::string> getAllTLIds() const;


    /** @brief Adds a tls program to the container
     *
     * If a tls with the given id is not yet known, a TLSLogicVariants structure
     *  is built for this tls and added to the internal container and the tls
     *  program is used as the new default.
     *
     * If the tls to add is loaded from an additional file (indicated by myNetWasLoaded,
     *  see closeNetworkReading), links from previously loaded tls are adapted to the logic.
     *  This may throw a ProcessError in the case no tls program was loaded for this
     *  tls before (was not defined in the network).
     *
     * The parameter newDefault defines whether this program will be used as the new
     *  default program of this tls. This means that an existing tls program for this
     *  tls is replaced within the according TLSLogicVariants structure.
     *
     * @param[in] id The id of the tls (program) to add
     * @param[in] programID The program id of the tls (program) to add
     * @param[in] logic The tls logic to insert
     * @exception ProcessError In the case an additional tls program is loaded and no one for the tls existed in the network
     * @return true if the tls program could be added, false otherwise
     */
    bool add(const std::string &id, const std::string &programID,
             MSTrafficLightLogic *logic, bool newDefault=true) throw(ProcessError);


    /// Returns the information whether the named tls is stored
    bool knows(const std::string &id) const;

    /// Returns the information whether the given tls program is the currently active for his tls
    bool isActive(const MSTrafficLightLogic *tl) const;

    /** @brief Switches the named (id) tls to the named (programID) program
     *
     * The program with the used programID must be previously added.
     * If the tls itself or the program to switch to is not known, false is returned.
     */
    bool switchTo(const std::string &id, const std::string &programID);


    /// @name WAUT definition methods
    /// @{

    /** @brief Adds a WAUT definition
     *
     * Throws an InvalidArgument if the given id is already in use.
     *
     * @param[in] refTime The reference time of the WAUT
     * @param[in] id The ID of the WAUT
     * @param[in] startProg The begin program of the WAUT
     * @exception InvalidArgument If the id is already used by another WAUT
     */
    void addWAUT(SUMOTime refTime, const std::string &id,
                 const std::string &startProg) throw(InvalidArgument);


    /** @brief Adds a WAUT switch step to a previously built WAUT
     *
     * Throws an InvalidArgument if the given WAUT id is not known.
     *
     * @param[in] wautid The ID of the WAUT
     * @param[in] when The switch procedure begin
     * @param[in] to The program the WAUT shall start to switch to at the given time
     * @exception InvalidArgument If the named WAUT is not known
     */
    void addWAUTSwitch(const std::string &wautid, SUMOTime when,
                       const std::string &to) throw(InvalidArgument);


    /** @brief Adds a tls to the list of tls to be switched by the named WAUT
     *
     * Passes the values directly to the used tls control. This throws an InvalidArgument
     *  if the given WAUT id or the given junction id is not known.
     *
     * @param[in] wautid The ID of the WAUT
     * @param[in] tls The id of the tls to be switched
     * @param[in] proc The switching procedure to use
     * @param[in] synchron Whether the switching shall be done in synchron mode
     * @exception InvalidArgument If the named WAUT or the named tls are not known
     * @exception ProcessError If the initial switch fails
     */
    void addWAUTJunction(const std::string &wautid, const std::string &tls,
                         const std::string &proc, bool synchron) throw(InvalidArgument, ProcessError);


    /** @brief Closes loading of a WAUT
     *
     * Insatntiates the first switch ("SwitchInitCommand") for the WAUT into the
     *  network's simulation time step begin event control.
     *
     * Throws an InvalidArgument if the given WAUT id is not known.
     *
     * @param[in] wautid The ID of the WAUT
     * @exception InvalidArgument If the named WAUT is not known
     * @see SwitchInitCommand
     */
    void closeWAUT(const std::string &wautid) throw(InvalidArgument);
    /// @}



    /** @brief Checks whether any WAUT is trying to switch a tls into another program
     *
     * Called from MSNet::simulationStep
     */
    void check2Switch(SUMOTime step);


    /** @brief return the complete phase definition for a named traffic lights logic
    */
    std::pair<SUMOTime, MSPhaseDefinition> getPhaseDef(const std::string &tlid) const;


protected:
    /**
     * @class SwitchInitCommand
     * @brief This event-class is used to initialise a WAUT switch at a certain time.
     *
     * This command is reused. The index of the WAUT-switch is incremented at each
     *  call to the control.
     */
    class SwitchInitCommand : public Command {
    public:
        // / Constructor
        SwitchInitCommand(MSTLLogicControl &p, const std::string &wautid, unsigned int index) throw()
                : myParent(p), myWAUTID(wautid), myIndex(index) { }

        /// Destructor
        ~SwitchInitCommand() throw() { }


        /// @name Derived from Command
        /// @{

        /** @brief Begins a WAUT switch by executing the command.
         *
         * The parent's "initWautSwitch" method is called supporting
         *  this command as an argument. The result of "initWautSwitch"
         *  is returned.
         *
         * "initWautSwitch" may throw an ProcessError if the program
         *  to switch to is not known.
         *
         * @param[in] currentTime The current simulation time (unused)
         * @return The time after which the command shall be executed again
         * @exception ProcessError If the program to switch to does not exist
         * @see MSTLLogicControl::initWautSwitch
         */
        SUMOTime execute(SUMOTime) throw(ProcessError) {
            return myParent.initWautSwitch(*this);
        }
        /// @}



        /// Returns the WAUT-id
        const std::string &getWAUTID() const {
            return myWAUTID;
        }

        /// Returns a reference to the index
        unsigned int &getIndex() {
            return myIndex;
        }

    protected:
        /// The control to call
        MSTLLogicControl &myParent;

        /// The id of the WAUT that shall switch
        std::string myWAUTID;

        /// The current index within the WAUT switch table
        unsigned int myIndex;

    private:
        /// @brief Invalidated copy constructor.
        SwitchInitCommand(const SwitchInitCommand&);

        /// @brief Invalidated assignment operator.
        SwitchInitCommand& operator=(const SwitchInitCommand&);

    };

public:
    /** @brief Initialises switching a WAUT
     *
     * This method is called from a previously built SwitchInitCommand
     */
    SUMOTime initWautSwitch(SwitchInitCommand &cmd);


protected:
    /**
     * @struct WAUTSwitch
     * @brief Storage for a WAUTs switch point
     */
    struct WAUTSwitch {
        /// The time the WAUT shall switch the TLS
        SUMOTime when;
        /// The program name the WAUT shall switch the TLS to
        std::string to;
    };

    /**
     * @struct WAUTJunction
     * @brief Storage for a junction assigned to a WAUT
     */
    struct WAUTJunction {
        /// The junction name
        std::string junction;
        /// The procedure to switch the junction with
        std::string procedure;
        /// Information whether this junction shall be switched synchron
        bool synchron;
    };

    /**
     * @struct WAUT
     * @brief A WAUT definition
     */
    struct WAUT {
        /// The id of the WAUT
        std::string id;
        /// The name of the start program
        std::string startProg;
        /// The reference time (offset to the switch times)
        SUMOTime refTime;
        /// The list of switches to be done by the WAUT
        std::vector<WAUTSwitch> switches;
        /// The list of switches assigned to the WAUT
        std::vector<WAUTJunction> junctions;
    };

    /**
     * @class WAUTSwitchProcedure
     * @brief This is the abstract base class for switching from one tls program to another.
     */
    class WAUTSwitchProcedure {
    public:
        /// Constructor
        WAUTSwitchProcedure(MSTLLogicControl &control, WAUT &waut,
                            MSTrafficLightLogic *from, MSTrafficLightLogic *to,
                            bool synchron)
                : myFrom(from), myTo(to), mySwitchSynchron(synchron), myWAUT(waut), myControl(control) { }

        /// Destructor
        virtual ~WAUTSwitchProcedure() { }

        /** @brief Determines whether a switch is possible.
         *
         * If a switch shall be done, this method should return true.
         */
        virtual bool trySwitch(SUMOTime step) = 0;

        // checks, if the position of a signal programm is at the GSP ("GuenstigerUmschaltPunkt")
        bool isPosAtGSP(SUMOTime step, MSSimpleTrafficLightLogic *testLogic);

        /// Returns the difference between an given position and the start of the phase
        unsigned int getDiffToStartOfPhase(MSSimpleTrafficLightLogic *givenLogic, unsigned int pos);

        // switches the given Logic directly to the given position
        void switchToPos(SUMOTime simStep, MSSimpleTrafficLightLogic *givenLogic, unsigned int toPos);

    protected:
        /// The current program of the tls to switch
        MSTrafficLightLogic *myFrom;

        /// The program to switch the tls to
        MSTrafficLightLogic *myTo;

        /// Information whether to switch synchron (?)
        bool mySwitchSynchron;

        /// The WAUT responsible for switching
        WAUT &myWAUT;

        /// The control the logic belongs to
        MSTLLogicControl &myControl;

        /// Returns the GSP-value which should be within the tls program definition
        SUMOReal getGSPValue(MSTrafficLightLogic *from) const;

    private:
        /// @brief Invalidated copy constructor.
        WAUTSwitchProcedure(const WAUTSwitchProcedure&);

        /// @brief Invalidated assignment operator.
        WAUTSwitchProcedure& operator=(const WAUTSwitchProcedure&);

    };


    /**
     * @class WAUTSwitchProcedure_JustSwitch
     * @brief This class simply switches to the next program
     */
    class WAUTSwitchProcedure_JustSwitch : public WAUTSwitchProcedure {
    public:
        /// Constructor
        WAUTSwitchProcedure_JustSwitch(MSTLLogicControl &control, WAUT &waut,
                                       MSTrafficLightLogic *from, MSTrafficLightLogic *to,
                                       bool synchron);

        /// Destructor
        ~WAUTSwitchProcedure_JustSwitch();

        /** @brief Determines whether a switch is possible.
         *
         * This implementation alsways returns true
         */
        bool trySwitch(SUMOTime step);

    private:
        /// @brief Invalidated copy constructor.
        WAUTSwitchProcedure_JustSwitch(const WAUTSwitchProcedure_JustSwitch&);

        /// @brief Invalidated assignment operator.
        WAUTSwitchProcedure_JustSwitch& operator=(const WAUTSwitchProcedure_JustSwitch&);

    };


    /**
     * @class WAUTSwitchProcedure_GSP
     * @brief This class switches using the GSP algorithm.
     */
    class WAUTSwitchProcedure_GSP : public WAUTSwitchProcedure {
    public:
        /// Constructor
        WAUTSwitchProcedure_GSP(MSTLLogicControl &control, WAUT &waut,
                                MSTrafficLightLogic *from, MSTrafficLightLogic *to,
                                bool synchron);

        /// Destructor
        ~WAUTSwitchProcedure_GSP();

        /** @brief Determines whether a switch is possible.
         */
        bool trySwitch(SUMOTime step);

        void adaptLogic(SUMOTime step);

    private:
        /// @brief Invalidated copy constructor.
        WAUTSwitchProcedure_GSP(const WAUTSwitchProcedure_GSP&);

        /// @brief Invalidated assignment operator.
        WAUTSwitchProcedure_GSP& operator=(const WAUTSwitchProcedure_GSP&);

    };


    /**
     * @class WAUTSwitchProcedure_Stretch
     * @brief This class switches using the Stretch algorithm.
     */
    class WAUTSwitchProcedure_Stretch : public WAUTSwitchProcedure {
    public:
        /// Constructor
        WAUTSwitchProcedure_Stretch(MSTLLogicControl &control, WAUT &waut,
                                    MSTrafficLightLogic *from, MSTrafficLightLogic *to,
                                    bool synchron);

        /// Destructor
        ~WAUTSwitchProcedure_Stretch();

        /** @brief Determines whether a switch is possible.
         */
        bool trySwitch(SUMOTime step);

        void adaptLogic(SUMOTime step, SUMOReal position);

        // cuts (stauchen) the Logic to synchronize,
        // returns false if cutting was imposible
        void cutLogic(SUMOTime step, unsigned int pos, size_t deltaToCut);

        // stretchs the logic to synchronize
        void stretchLogic(SUMOTime step, unsigned int startPos, size_t deltaToStretch);

    protected:
        /** @struct StretchBereichDef
         * @brief A definition of a stretch - Bereich
         */
        struct StretchBereichDef {
            /// The begin of a Bereich (?)
            SUMOReal begin;
            /// The end of a Bereich (?)
            SUMOReal end;
            /// The factor of a Bereich (?)
            SUMOReal fac;

        };

        /// Returns the number of given Stretch-Bereiche(?) for the given program
        int getStretchBereicheNo(MSTrafficLightLogic *from) const;

        /** @brief Returns the numbered Stretch-Bereich for the given program
         *
         * The first bereich has normally the number "1", not "0"!
         */
        StretchBereichDef getStretchBereichDef(MSTrafficLightLogic *from, int index) const;

    private:
        /// @brief Invalidated copy constructor.
        WAUTSwitchProcedure_Stretch(const WAUTSwitchProcedure_Stretch&);

        /// @brief Invalidated assignment operator.
        WAUTSwitchProcedure_Stretch& operator=(const WAUTSwitchProcedure_Stretch&);

    };


    /**
     * @struct WAUTSwitchProcess
     * @brief An initialised switch process
     */
    struct WAUTSwitchProcess {
        /// The id of the junction to switch
        std::string junction;
        /// The current program of the tls
        MSTrafficLightLogic *from;
        /// The program to switch the tls to
        MSTrafficLightLogic *to;
        /// The used procedure
        WAUTSwitchProcedure *proc;
    };

    /// A map of ids to corresponding WAUTs
    std::map<std::string, WAUT*> myWAUTs;

    /// A list of currently running switching procedures
    std::vector<WAUTSwitchProcess> myCurrentlySwitched;

    /// A map from ids to the corresponding variants
    std::map<std::string, TLSLogicVariants*> myLogics;

    /// Information whether the net was completely loaded
    bool myNetWasLoaded;


private:
    /// @brief Invalidated copy constructor.
    MSTLLogicControl(const MSTLLogicControl&);

    /// @brief Invalidated assignment operator.
    MSTLLogicControl& operator=(const MSTLLogicControl&);

};


#endif

/****************************************************************************/

