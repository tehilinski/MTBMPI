/*! ----------------------------------------------------------------------------------------------------------
@file		Task.h
@class		mtbmpi::Task
@brief 		Class to control a single concurrent task.
@details
		Tasks are created by the TaskFactory and owns
		a work task object as a TaskAdapter.
		The work tasks are derived from TaskAdapterBase.

		The task knows its state, and can perform these actions:
		Initialize, Start, Stop, Pause, Resume, AcceptData.

		A task has its own arc, argv pair, so that the task can be an adapter
		to a command-line application. In this implementation, the master
		will initialize a task with command-line arguments for its job
		as though that particular task were run from the command-line.
@internal
project		Master-Task-Blackboard MPI Framework
@author		Thomas E. Hilinski <https://github.com/tehilinski>
@copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#ifndef INC_mtbmpi_Task_h
#define INC_mtbmpi_Task_h

#include "SendsMsgsToLog.h"
#include "TaskAdapterBase.h"
#include "TaskFactoryBase.h"
#include <memory>

namespace mtbmpi {


class Master;

class Task : public SendsMsgsToLog
{
  public:

    typedef std::shared_ptr<TaskAdapterBase>			TaskAdapterPtr;
    typedef std::shared_ptr<TaskFactoryBase>			TaskFactoryPtr;
    typedef std::pair< int const, char const * const * >	ArgPair;	///< command-line arc, argv

    /// Constructor
    Task (
      Master & useParent,		///< parent of task
      std::string const & taskName,	///< name of this task
      IDNum const myID,			///< task mpi task rank
      IDNum const controllerID,		///< controller task rank
      IDNum const blackBoardID,		///< blackboard task rank
      TaskFactoryPtr pTaskFactory,	///< task factory
      ArgPair args			///< command-line args
      );	// here for doxygen bug

    ~Task ();

    void  SetState (State const newState);				///< Set the task state
    State GetState () const { return state; }				///< Get the task state

    void SendMsgToLog ( std::string const & logMsg );			///< Send a message to the log file
    void SendMsgToLog ( char const * const logMsg );			///< Send a message to the log file

    Master const &      GetParent () const { return parent; }		///< Get the task's master
    Master       &      GetParent ()       { return parent; }		///< Get the task's master
    std::string const & GetName ()   const { return name; }		///< Get the task name
    std::string const & GetIDStr ()  const { return idStr; }		///< Get the task ID as a string
    ArgPair const &     GetArgs ()   const { return argPair; }		///< get command-line argc, argv

    IDNum GetControllerID () const { return idController; }

    void Activate ();	// run the task's event loop

  private:

    /// @cond SKIP_PRIVATE

    enum ActionNeeded
      {
	ActionInitialize,
	ActionStart,
	ActionStop,
	ActionPause,
	ActionResume,
	ActionAcceptData,
	NoAction
      };

    Master & parent;
    std::string name;			// task name
    IDNum const idController;
    ArgPair argPair;			// arc, argv
    State state;
    ActionNeeded action;
    int bufferState[2];			// msg buffer for Tag_State; 0 = id, 1 = state
    TaskAdapterPtr pTaskAdapter;	// the actual task
    std::string idStr;			// string with Tracker index: 1-based


    ActionNeeded ProcessMessage (MPI::Status const & status);
    void SendStateToController ();
    void LogState();

    void DoActionInitialize ();
    void DoActionStart ();
    void DoActionStop ();
    void DoActionPause ();
    void DoActionResume ();
    void DoActionAcceptData ();

    // functions that should not be used
    Task (Task const & object);
    Task & operator= (Task const & object);
    bool operator== (Task const & object) const;
    bool operator!= (Task const & object) const;

    /// @endcond
};


} // namespace mtbmpi

#endif // INC_mtbmpi_Task_h
