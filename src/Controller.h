/*! ----------------------------------------------------------------------------------------------------------
@file		Controller.h
@class		mtbmpi::Controller
@brief 		Manages all actions related to active processes.
@details
		Owns the Blackboard object.
		Initializes, starts and stops work tasks.
		Runs in MPI rank == 0 along with Master, and is owned by Master.
@internal
project		Master-Task-Blackboard MPI Framework
@author		Thomas E. Hilinski <https://github.com/tehilinski>
@copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#ifndef INC_mtbmpi_Controller_h
#define INC_mtbmpi_Controller_h

#include "SendsMsgsToLog.h"
#include "Tracker.h"
#include "Task.h"
#include "MsgTags.h"
#include "TimerMPI.h"
#include <memory>

namespace mtbmpi {


class Configuration;
class Master;

class Controller : public SendsMsgsToLog
{
    friend class Master;

  public:

    typedef std::shared_ptr<Tracker>		TrackerPtr;
    typedef std::shared_ptr<Configuration>	ConfigurationPtr;

    /// Constructor
    Controller (
      Master & useParent,		///< parent of task
      IDNum const myId,			///< controller (master) rank
      IDNum const blackBoardID,		///< blackboard rank
      int const numTasks,		///< number of work processes
      IDNum const firstTaskID,		///< rank of 1st work process
      ConfigurationPtr configPtr	///< configuration object (shares my rank)
      );	// here for doxygen bug

    /// Start the main loop of the controller.
    /// Wait for messages from tasks.
    /// Perform action according to type of message.
    void Activate ();

    Tracker & GetTracker () const { return *pTracker; }

    Configuration const & GetConfiguration () const { return *pConfig; }

  private:

    /// @cond SKIP_PRIVATE

    std::string const className;
    Master & parent;
    IDNum const idFirstTask;		// rank of 1st work task
    TrackerPtr pTracker;
    ConfigurationPtr pConfig;		// configuration
    State stateBB;			// blackboard state
    TimerMPI timer;			// timer for job using MPI timer

    void LogCmdLineArgs ();		// write cmd-line args to log file

    void SetTaskState (
      MPI::Status & status);		// status from Probe

    void SetBlackboardState (
      State const newState );

    void InitializeAllTasks ();
    void StartAllTasks ();
    bool StopAllTasks ();
    void StopBlackboard ();		// call this only after all tasks are stopped
    void WaitUntilCanStop ();		// true if Master can stop

    void DoActionState ( MPI::Status status );
    void DoActionRequestStop ( MPI::Status status );
    void DoActionRequestCmdLineArgs ( MPI::Status status );
    void DoActionRequestConfig ( MPI::Status status );

    // functions that should not be used; are not defined
    Controller (Controller const & object);
    Controller & operator= (Controller const & object);
    bool operator== (Controller const & object) const;
    bool operator!= (Controller const & object) const;

    /// @endcond
};


/// @cond SKIP_PRIVATE

inline void Controller::SetBlackboardState (
    State const newState )
{
    /// @todo Is it valid to change current BB state to this one?
    stateBB = newState;
}

/// @endcond


} // namespace mtbmpi


#endif // INC_mtbmpi_Controller_h
