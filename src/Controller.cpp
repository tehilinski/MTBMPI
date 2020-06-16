/*------------------------------------------------------------------------------------------------------------
file		Controller.cpp
class		mtbmpi::Controller
brief 		Manages all actions related to active processes.
details
		Owns the Blackboard object.
		Initializes, starts and stops work tasks.
		Runs in MPI rank == 0 along with Master, and is owned by Master.

project		Master-Task-Blackboard MPI Framework
author		Thomas E. Hilinski <https://github.com/tehilinski>
copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#include "Controller.h"
#include "Master.h"
#include "UtilitiesMPI.h"
#include <sstream>

// define the following to write diagnostics to std::cout
// #define DBG_MPI_CONTROLLER

#ifdef DBG_MPI_CONTROLLER
  #include <iostream>
  using std::cout;
  using std::endl;
#endif

namespace mtbmpi {


Controller::Controller (
    Master & useParent,			// parent of task
    IDNum const myId,			// controller (master) rank
    IDNum const blackBoardID,		// blackboard rank
    int const numTasks,			// number of work processes
    IDNum const firstTaskID,		// rank of 1st work process
    ConfigurationPtr configPtr)		// configuration object (shares my rank)
    : SendsMsgsToLog (myId, blackBoardID),
      className ( "mtbmpi::Controller" ),
      parent (useParent),
      idFirstTask (firstTaskID),
      pConfig (configPtr),
      stateBB ( State_Unknown )
{
    pTracker.reset ( new Tracker (numTasks) );
    #ifdef DBG_MPI_CONTROLLER
	Log().Message("Controller started.");
    #endif
   // following is done by creating object
   // Activate ();
}

void Controller::Activate ()
{
    #ifdef DBG_MPI_CONTROLLER
      std::string const myName = "Controller::Activate: ";
      cout << myName << "enter" << endl;
    #endif

    // StartAllTasks ();
    LogCmdLineArgs ();

    // Derived class actions
    parent.ActionsBeforeTasks ();

    // aggregate state flags
    bool tasksAreCreated     = GetTracker().AreAllCreated();
    bool tasksAreInitialized = false;
    bool tasksAreStarted     = false;
    bool requestedInitAll    = false;
    bool tasksAreStopped     = false;

    //---------------------------------------------------------------------
    bool listenForMsgs = true;
    while (listenForMsgs)				// event loop
    {
	// check states; initialize and start tasks
	if ( !tasksAreCreated )
	{
	    tasksAreCreated = GetTracker().AreAllCreated();
	    #ifdef DBG_MPI_CONTROLLER
	      cout << myName << "tasksAreCreated: "
		   << ( tasksAreCreated ? "yes" : "no" )
		   << endl;
	    #endif
	}
	if ( tasksAreCreated && !tasksAreInitialized )
	{
	    tasksAreInitialized = GetTracker().AreAllInitialized();
	    #ifdef DBG_MPI_CONTROLLER
	      cout << myName << "tasksAreInitialized: "
		   << ( tasksAreInitialized ? "yes" : "no" )
		   << endl;
	    #endif
	}

	// actions based on states
	if ( !requestedInitAll )
	{
	    if ( tasksAreCreated && !tasksAreInitialized  )
	    {
		InitializeAllTasks ();
		requestedInitAll = true;
	    }
	}
	if ( tasksAreInitialized && !tasksAreStarted )
	{
	    StartAllTasks ();
	    tasksAreStarted = true;
	}

	tasksAreStopped = GetTracker().AreAllStopped();
	#ifdef DBG_MPI_CONTROLLER
	    cout << myName << "tasksAreStopped: "
		 << ( tasksAreStopped ? "yes" : "no" )
		 << endl;
	#endif

	// incoming msg handler
	if ( listenForMsgs )
	{
	    if ( tasksAreStarted && !tasksAreStopped )
	    {
		#ifdef DBG_MPI_CONTROLLER
		  cout << myName << "ActionsWhileActive" << endl;
		#endif
		parent.ActionsWhileActive();	// Derived class actions
	    }

	    // Wait for messages from tasks.
	    // Change task state according to message.
	    // When all task are done, send msg to master.
	    #ifdef DBG_MPI_CONTROLLER
	      cout << myName << "comm.Probe: start" << endl;
	    #endif
	    MPI::Status status;
	    mtbmpi::comm.Probe ( MPI_ANY_SOURCE, MPI_ANY_TAG, status );
	    #ifdef DBG_MPI_CONTROLLER
	      cout << myName << "comm.Probe: processing msg" << endl;
	    #endif

	    switch ( status.Get_tag() )
	    {
	      case Tag_State:
		DoActionState (status);
		break;

	      case Tag_RequestStop:
		DoActionRequestStop (status);
		break;

	      case Tag_RequestCmdLineArgs:
		DoActionRequestCmdLineArgs (status);
		break;

	      case Tag_RequestConfig:
		DoActionRequestConfig (status);
		break;

	      default:
		/// @todo  log unhandled message received
		break;

	    } // switch ( status.Get_tag() )
	} // listenForMsgs

	tasksAreStopped = GetTracker().AreAllStopped();	// update
	if ( tasksAreStopped )
	{
	    #ifdef DBG_MPI_CONTROLLER
	      cout << myName << "GetTracker().AreAllStopped() == true" << endl;
	    #endif

	    parent.ActionsAfterTasks ();	// Derived class actions
	    Log().Message("Controller: all tasks are stopped.");

	    timer.stop();
	    std::ostringstream os;
	    os << "Elapsed time for all tasks (seconds): " << timer.read();
	    Log().Message( os.str() );

	    StopBlackboard ();
	    listenForMsgs = false;
	}

    } // while
    Log().Message("Controller stopped.");

    #ifdef DBG_MPI_CONTROLLER
    cout << myName << "done" << endl;
    #endif
}

/// @cond SKIP_PRIVATE

void Controller::DoActionState ( MPI::Status status )
{
    #ifdef DBG_MPI_CONTROLLER
      std::string const myName = "Controller::DoActionState: ";
      cout << myName << "Tag_State: enter: "
	   << "task ID = " << ( status.Get_source() - idFirstTask )
	   << endl;
    #endif

    if ( (TaskID::IDNum) status.Get_source() == parent.GetBlackboardID() )
	; /// @todo anything?
    else if ( (TaskID::IDNum) status.Get_source() >= idFirstTask )
	SetTaskState (status);

    #ifdef DBG_MPI_CONTROLLER
      cout << myName << "Tag_State: done" << endl;
    #endif
}

void Controller::DoActionRequestStop ( MPI::Status status )
{
    Log().Message("Controller: received stop request.");
    #ifdef DBG_MPI_CONTROLLER
      std::string const myName = "Controller::DoActionRequestStop: ";
      cout << myName << "Tag_RequestStop: enter" << endl;
    #endif

    // do a recv so message is marked as received
    char charBuffer = NULL_CHAR;
    mtbmpi::comm.Recv (
	    &charBuffer, 1, MPI::CHAR,
	    status.Get_source(), Tag_RequestStop, status );

    #ifdef DBG_MPI_CONTROLLER
      cout << myName << "Tag_RequestStop: StopAllTasks" << endl;
    #endif

    // controller cannot stop until the work tasks are stopped
    if ( StopAllTasks () )
    {
	Log().Message("Controller: all tasks stopped.");
    }
    else
    {
	Log().Message("Controller: stop all tasks failed.");
	    /// @todo  handle Tag_RequestStop - failed
    }

    // controller cannot stop until the blackboard is stopped
    #ifdef DBG_MPI_CONTROLLER
      cout << myName << "StopBlackboard" << endl;
    #endif
    StopBlackboard ();

    #ifdef DBG_MPI_CONTROLLER
      cout << myName << "Tag_RequestStop: done" << endl;
    #endif
}

void Controller::DoActionRequestCmdLineArgs ( MPI::Status status )
{
    #ifdef DBG_MPI_CONTROLLER
      std::string const myName = "Controller::DoActionRequestCmdLineArgs: ";
      cout << myName << "Tag_RequestCmdLineArgs: enter" << endl;
    #endif

    // do a recv so message is marked as received
    char charBuffer = NULL_CHAR;
    mtbmpi::comm.Recv (
	    &charBuffer, 1, MPI::CHAR,
	    status.Get_source(), Tag_RequestCmdLineArgs, status );

    // create a buffer to hold the args to send to the task
    std::string buffer;
    JoinStrings (buffer, GetConfiguration().GetArgs(), NL_CHAR );
    mtbmpi::comm.Send (
		buffer.c_str(), buffer.size(), MPI::CHAR,
		status.Get_source(), Tag_CmdLineArgs );
    CheckErrorMPI( className );

    #ifdef DBG_MPI_CONTROLLER
      cout << myName << "Tag_RequestCmdLineArgs: done" << endl;
    #endif
}

void Controller::DoActionRequestConfig ( MPI::Status status )
{
    #ifdef DBG_MPI_CONTROLLER
      std::string const myName = "Controller::DoActionRequestConfig: ";
      cout << myName << "Tag_RequestConfig: enter" << endl;
    #endif

    // do a recv so message is marked as received
    mtbmpi::comm.Recv (
	    0, 0, MPI::BYTE,
	    status.Get_source(), Tag_RequestConfig, status );

    /// @todo  Tag_RequestConfig

    #ifdef DBG_MPI_CONTROLLER
      cout << myName << "Tag_RequestConfig: done" << endl;
    #endif
}

void Controller::SetTaskState (
	MPI::Status & status)		// status from Probe
{
    #ifdef DBG_MPI_CONTROLLER
      std::string const myName = "Controller::SetTaskState: ";
      cout << myName << "enter" << endl;
    #endif

    MPI::Datatype bufferType = MPI::INT;
    int const bufferSize =
	    status.Get_count (bufferType) * sizeof(int) * 2;
	    // status.Get_count (bufferType) * bufferType.Get_extent() * 2;
    int * buffer = new int [bufferSize];
    mtbmpi::comm.Recv (
	    static_cast<void *>(buffer),
	    bufferSize, bufferType,
	    status.Get_source(), Tag_State, status );

    /// @todo assert status.Get_source() == buffer[0]

    int const taskID = buffer[0];
    if ( taskID >= 0 )
    {
	State const taskState = static_cast<State>( buffer[1] );
	#ifdef DBG_MPI_CONTROLLER
	  State const previousState =
	#endif
	    GetTracker().SetState ( taskID - idFirstTask, taskState );

	#ifdef DBG_MPI_CONTROLLER
	    cout << myName << "task rank = " << status.Get_source()
		<< ": previous state = " << AsString(previousState)
		<< ": new state = " << AsString(taskState)
		<< endl;
	    // tell log file
	    std::ostringstream os;
	    os << "Controller: changed state for task rank " << taskID
	       << " to " << AsString(taskState);
	    Log().Message( os.str() );
	#endif
    }
    delete [] buffer;

    #ifdef DBG_MPI_CONTROLLER
      cout << myName << "done" << endl;
    #endif
}


void Controller::InitializeAllTasks ()
{
    std::string const myName = "Controller::InitializeAllTasks: ";
    #ifdef DBG_MPI_CONTROLLER
      cout << myName << pTracker->Size() << " tasks" << endl;
    #endif

    timer.start();
    parent.ActionsAtInitTasks();

    std::vector<MPI::Request> requests;
    for ( Tracker::size_type taskNum = 0; taskNum < pTracker->Size(); ++taskNum )
    {
	requests.push_back(
	    mtbmpi::comm.Isend (
		0, 0, MPI::BYTE,
		(idFirstTask + taskNum), Tag_InitializeTask ) );
    }

    std::vector<MPI::Status> status ( requests.size() );
    MPI::Request::Waitall ( requests.size(), requests.data(), status.data() );

    // check for errors
    for ( std::vector<MPI::Status>::const_iterator i = status.begin();
	  i != status.end();
	  ++i )
    {
	if ( i->Get_error() != MPI_SUCCESS )
	{
	    std::ostringstream os;
	    os << myName << "send message error."
	       << " Error: "  << i->Get_error()
	       << " Tag: "    << i->Get_tag()
	       << " Source: " << i->Get_source();
	    Log().Error( os.str() );
	}
    }

    #ifdef DBG_MPI_CONTROLLER
      cout << myName << "done" << endl;
    #endif
}

void Controller::StartAllTasks ()
{
    std::string const myName = "Controller::InitializeAllTasks: ";
    #ifdef DBG_MPI_CONTROLLER
      cout << myName << "comm.Isend: start "
	   << pTracker->Size() << " tasks"
	   << endl;
    #endif

    parent.ActionsBeforeTasksStart();

    std::vector<MPI::Request> requests ( pTracker->Size() );
    for ( Tracker::size_type taskNum = 0; taskNum < pTracker->Size(); ++taskNum )
    {
	requests[taskNum] = mtbmpi::comm.Isend (
	    0, 0, MPI::BYTE,
	    (idFirstTask + taskNum), Tag_StartTask );
    }

    #ifdef DBG_MPI_CONTROLLER
    cout << myName << "Request::Waitall: "
	 << requests.size() << " tasks"
	 << endl;
    #endif

    std::vector<MPI::Status> status ( requests.size() );
    MPI::Request::Waitall ( requests.size(), requests.data(), status.data() );

    // check for errors
    for ( std::vector<MPI::Status>::const_iterator i = status.begin();
	  i != status.end();
	  ++i )
    {
	if ( i->Get_error() != MPI_SUCCESS )
	{
	    std::ostringstream os;
	    os << myName << "send message error."
	       << " Error: "  << i->Get_error()
	       << " Tag: "    << i->Get_tag()
	       << " Source: " << i->Get_source();
	    Log().Error( os.str() );
	}
    }

    #ifdef DBG_MPI_CONTROLLER
    cout << myName << "done" << endl;
    #endif
}

bool Controller::StopAllTasks ()	// return true if all tasks are stopped
{
    Log().Message("Controller stopping all tasks.");
    #ifdef DBG_MPI_CONTROLLER
      std::string const myName = "Controller::StopAllTasks: ";
      cout << myName << "checking " << GetTracker().Size() << " tasks." << endl;
    #endif

    int taskNum = 0;		// idFirstTask - idFirstTask
    for ( Tracker::const_iterator iTask = GetTracker().Begin();
	  iTask != GetTracker().End();
	  ++iTask, ++taskNum )
    {
	if ( GetTracker().GetState(taskNum) != State_Completed &&
	     GetTracker().GetState(taskNum) != State_Terminated &&
	     GetTracker().GetState(taskNum) != State_Error )
	{
	    // stop task
	    mtbmpi::comm.Send ( 0, 0, MPI::BYTE, (idFirstTask + taskNum), Tag_RequestStopTask );
	    CheckErrorMPI( className );
	}
    }

    #ifdef DBG_MPI_CONTROLLER
    cout << myName << "GetTracker().AreAllStopped() = "
	 << ( GetTracker().AreAllStopped() ? "yes" : "no" )
	 << endl;
    #endif

    return GetTracker().AreAllStopped();
}

void Controller::StopBlackboard ()
{
    #ifdef DBG_MPI_CONTROLLER
    cout << "Controller::StopBlackboard: enter." << endl;
    #endif

    if (stateBB != State_Completed)
    {
	// send request stop
	#ifdef DBG_MPI_CONTROLLER
	Log().Message( "Controller: requesting Blackboard to stop" );
	#endif
	mtbmpi::comm.Send ( 0, 0, MPI::BYTE, parent.GetBlackboardID(), Tag_StopBlackboard );
	// wait for confirmation
	mtbmpi::comm.Recv ( 0, 0, MPI::BYTE, parent.GetBlackboardID(), Tag_Confirmation );
	stateBB = State_Completed;
	Sleep();	// time for BB to actually stop
    }

    #ifdef DBG_MPI_CONTROLLER
    cout << "Controller::StopBlackboard: done." << endl;
    #endif
}

void Controller::WaitUntilCanStop ()	// return when task rank == 0 can stop safely
{
    while ( !GetTracker().AreAllStopped() )
	Sleep();

    if (stateBB != State_Completed)
    	StopBlackboard ();
}

void Controller::LogCmdLineArgs ()		// write cmd-line args to log file
{
    std::ostringstream os;
    os << "Command-line arguments: ";
    if ( parent.GetArgs().first > 1 )
    {
	os << NL_CHAR;
	for ( short i = 1; i < parent.GetArgs().first; ++i )
	    os << i << ": " << parent.GetArgs().second[i] << NL_CHAR;
    }
    else
	os << "none";
    Log().Message( os.str() );
}

/// @endcond


} // namespace mtbmpi
