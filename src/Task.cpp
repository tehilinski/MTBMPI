/*------------------------------------------------------------------------------------------------------------
file		Task.cpp
class		mtbmpi::Task
brief 		Class to control a single concurrent task.
details
		Tasks are created by the TaskFactory and owns
		a work task object as a TaskAdapter.
		The work tasks are derived from TaskAdapterBase.

		The task knows its state, and can perform these actions:
		Initialize, Start, Stop, Pause, Resume, AcceptData.

		A task has its own arc, argv pair, so that the task can be an adapter
		to a command-line application. In this implementation, the master
		will initialize a task with command-line arguments for its job
		as though that particular task were run from the command-line.

project		Master-Task-Blackboard MPI Framework
author		Thomas E. Hilinski <https://github.com/tehilinski>
copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#include "Task.h"
#include "Master.h"
#include "MsgTags.h"
#include "UtilitiesMPI.h"

// define the following to write diagnostics to std::cout
// #define DBG_MPI_TASK

#ifdef DBG_MPI_TASK
  #include <iostream>
  using std::cout;
  using std::endl;
#endif

namespace mtbmpi {


Task::Task (
    Master & useParent,				// parent of task
    std::string const & taskName,		// name of this task
    IDNum const myID,				// task mpi task rank
    IDNum const controllerID,			// controller task rank
    IDNum const blackBoardID,			// blackboard task rank
    TaskFactoryPtr pTaskFactory,		// task factory
    ArgPair args)
    : SendsMsgsToLog (myID, blackBoardID),
      parent (useParent),
      name ( taskName ),
      idController (controllerID),
      argPair (args),
      state (State_Unknown),
      action (NoAction)
{
    // string with Tracker index: 1-based
    idStr = ToString ( myID - parent.GetFirstTaskID() + 1 );
    bufferState[0] = myID;			// constant

    #ifdef DBG_MPI_TASK
      cout << "Tracker ID " << idStr << ": " << "constructor" << endl;
    #endif

    // get cmd-line args
    StrVec cmdLineArgs (
		    GetArgs().second,
		    GetArgs().second + GetArgs().first );

    // create and start task
    pTaskAdapter = pTaskFactory->Create (*this, name, cmdLineArgs);
    SetState( State_Created );

    // Master decides when to activate this class
    // so that derived Task class can do things after this constructor
    // Activate ();
}

Task::~Task ()
{
    #ifdef DBG_MPI_TASK
      cout << "Tracker ID " << idStr << ": ~Task" << endl;
    #endif
    SetState( State_Terminated );
}

void Task::Activate ()
{
    #ifdef DBG_MPI_TASK
      cout << "Tracker ID " << idStr << ": " << "Activate: enter" << endl;
    #endif

    // Handle messages
    while ( !( IsCompleted(state) ||
	       IsTerminated(state) ||
	       IsError(state) ) )
    {
	#ifdef DBG_MPI_TASK
	    cout << "Tracker ID " << idStr << ": "
		 << "Activate: PI::COMM_WORLD.Probe: start"
		 << ": state = " << AsString(state)
		 << endl;
	#endif

	// Wait for messages
	MPI::Status status;
	mtbmpi::comm.Probe ( idController, MPI_ANY_TAG, status );

	#ifdef DBG_MPI_TASK
	    cout << "Tracker ID " << idStr << ": "
		 << "Activate: PI::COMM_WORLD.Probe: done"
		 << endl;

	    static int probeCount = 0;
	    if ( !IsMsgTagValid( status.Get_tag() ) )
	    {
		if ( probeCount < 10 )
		{
		    std::ostringstream os;
		    os << "Task::Activate: Probe error."
		       << " Error: "  << status.Get_error()
		       << " Tag: "    << status.Get_tag()
		       << " Source: " << status.Get_source()
		       << " MPI::ERRORS_RETURN: " << MPI::ERRORS_RETURN;
		    Log().Error( os.str() );
		    cout << os.str() << endl;
		    ++probeCount;
		}
	    }
	#endif

	action = ProcessMessage (status);

	#ifdef DBG_MPI_TASK
	if ( IsMsgTagValid( status.Get_tag() ) )
	{
	    cout << "Tracker ID " << idStr << ": "
		 << "Activate: ProcessMessage: "
		 << " Tag: "    << status.Get_tag()
		 << " Source: " << status.Get_source()
		 << " Action = " << action
		 << endl;
	}
	#endif

	switch (action)
	{
	  case ActionInitialize: DoActionInitialize ();     break;
	  case ActionStart:      DoActionStart ();          break;
	  case ActionStop:       DoActionStop ();           break;
	  case ActionPause:      DoActionPause ();          break;
	  case ActionResume:     DoActionResume ();         break;
	  case ActionAcceptData: DoActionAcceptData();      break;

	  case NoAction:
	  default:
	    break;
	}

    } // while

    #ifdef DBG_MPI_TASK
    cout << "Tracker ID " << idStr << ": " << "Activate: done" << endl;
    #endif
}

/// @cond SKIP_PRIVATE

void Task::DoActionInitialize ()
{
    #ifdef DBG_MPI_TASK
    cout << "Tracker ID " << idStr << ": "
	 << "Activate: ActionInitialize" << endl;
    #endif

    if ( IsError(state) )
    {
	LogState();
	return;
    }

    SetState( pTaskAdapter->InitializeTask () );
    LogState();
}

void Task::DoActionStart ()
{
    #ifdef DBG_MPI_TASK
    cout << "Tracker ID " << idStr << ": "
	 << "Activate: ActionStart: enter" << endl;
    #endif

    if ( IsError(state) )
    {
	LogState();
	return;
    }

    if ( !IsInitialized(state) )
    {
	// handle failed initialization
	SendMsgToLog ("initialization failed");
	SetState( State_Error );
	LogState();
	return;
    }

    SetState( pTaskAdapter->StartTask() ); // returns state after start
    LogState();

    #ifdef DBG_MPI_TASK
    cout << "Task " << idStr << ": "
	 << "Activate: ActionStart: done" << endl;
    #endif
}

void Task::DoActionStop ()
{
    #ifdef DBG_MPI_TASK
    cout << "Tracker ID " << idStr << ": "
	 << "Activate: ActionStop: enter" << endl;
    #endif

    SetState( pTaskAdapter->StopTask() );
    if ( !IsCompleted(state) && !IsTerminated(state) )  // not stopped?
    {
	// force stop - ouch!
	pTaskAdapter.reset();
	SetState( State_Terminated );
    }
    else // if ( IsCompleted(state) || IsTerminated(state) )
    {
	action = NoAction;
    }
    LogState();

    // check for and discard any remaining msgs
    short count = 0;
    while ( count < 10 )    // number of checks
    {
	if ( mtbmpi::comm.Iprobe ( idController, MPI_ANY_TAG ) )
	{
	    // cout << "Task " << idStr << ": discarding msg" << endl;
	    mtbmpi::comm.Recv ( 0, 0, MPI::BYTE, idController, MPI_ANY_TAG );
	}
	else // wait a bit
	{
	    Sleep();
	}
	++count;
    }

    #ifdef DBG_MPI_TASK
    cout << "Tracker ID " << idStr << ": "
	 << "Activate: ActionStop: done" << endl;
    #endif
}

void Task::DoActionPause ()
{
    SetState( pTaskAdapter->PauseTask() );
    LogState();
    if ( IsPaused(state) )
	action = NoAction;	// wait for resume msg
}

void Task::DoActionResume ()
{
    if ( IsError(state) )
    {
	LogState();
	return;
    }

    SetState( pTaskAdapter->ResumeTask() );
    LogState();
    if ( IsRunning(state) || IsCompleted(state) )
	action = NoAction;
}

void Task::DoActionAcceptData ()
{
    if ( IsError(state) )
    {
	LogState();
	return;
    }

    /// @todo DoActionAcceptData
}

void Task::SendStateToController ()
{
    #ifdef DBG_MPI_TASK
    cout << "Tracker ID " << idStr << ": "
	 << "SendStateToController: "
	 << "state = " << AsString(state)
	 << endl;
    #endif

    // bufferState[0] = GetID();	// always
    bufferState[1] = static_cast<int>(state);
    mtbmpi::comm.Send (
	    &bufferState,	// [0] = ID, [2] = enum State
	    2, 			// size of bufferState
	    MPI::INT,		// data type
	    idController, 	// destination
	    Tag_State );	// msg type
}

void Task::LogState ()
{
    std::string msg = "Tracker ID ";
    msg += idStr;
    msg += ": state = ";
    msg += AsString(state);
    Log().Message( msg );
}

Task::ActionNeeded Task::ProcessMessage (
    MPI::Status const & status)
{
    ActionNeeded newAction = NoAction;
    switch ( status.Get_tag() )
    {
      case Tag_InitializeTask:
      {
	mtbmpi::comm.Recv ( 0, 0, MPI::BYTE, idController, Tag_InitializeTask );
	newAction = ActionInitialize;
	break;
      }
      case Tag_StartTask:
      {
	mtbmpi::comm.Recv ( 0, 0, MPI::BYTE, idController, Tag_StartTask );
	newAction = ActionStart;
	break;
      }
      case Tag_RequestStopTask:
      {
	mtbmpi::comm.Recv ( 0, 0, MPI::BYTE, idController, Tag_RequestStopTask );
	newAction = ActionStop;
	break;
      }
      case Tag_RequestStop:
      {
	mtbmpi::comm.Recv ( 0, 0, MPI::BYTE, idController, Tag_RequestStop );
	newAction = ActionStop;
	break;
      }
      case Tag_RequestPauseTask:
      {
	mtbmpi::comm.Recv ( &bufferState, 2, MPI::INT, idController, Tag_RequestPauseTask );
	newAction = ActionPause;
	break;
      }
      case Tag_RequestResumeTask:
      {
	mtbmpi::comm.Recv ( &bufferState, 2, MPI::INT, idController, Tag_RequestResumeTask );
	newAction = ActionResume;
	break;
      }
      case Tag_Data:
      {
	/// @todo Tag_Data
	newAction = ActionAcceptData;
	break;
      }
      default:
      {
	// unknown message - mark as received but discard
	// mtbmpi::comm.Recv ( 0, 0, MPI::BYTE, idController, MPI_ANY_TAG );
	break;
      }
    }
    return newAction;
}

/// @endcond

void Task::SetState (State const newState)
{
    state = newState;
    SendStateToController ();
}

void Task::SendMsgToLog ( std::string const & logMsg )
{
    Log().Message( logMsg, idStr );
}

void Task::SendMsgToLog ( char const * const logMsg )
{
    Log().Message( logMsg, idStr );
}


} // namespace mtbmpi
