/*------------------------------------------------------------------------------------------------------------
file		Master.cpp
class		mtbmpi::Master
brief		This is the main class for starting and ending a MPI job.
		Implements a master/task pattern with a blackboard for work process' communication.
details
		The Master runs in process with MPI rank == 0
		and owns the controller and configuration objects.
		The process with rank == ID_Blackboard has a Master owning a
		Blackboard but not a Controller.
		Other ranks > ID_Blackboard have a Master, that owns a Task,
		that owns a concrete TaskAdapterBase (the application's work task.)

		Your application must have its own master object which inherits mtbmpi::Master
		and implements the virtual private methods ``DoActions*``.

		The derived Master class should activate the Controller in its constructor,
		so that derived class can do things after this base class constructor but
		before the Controller's event loop is running:
		`pController->Activate();`
		The constructor can have code like this:

		if ( GetID() == GetControllerID() )
		{
		    if ( IsInitialized() )	// mtbmpi::Master is initialized?
		    {
			// other derived class initialization here... then:
			GetController().Activate();	// event loop
		    }
		}

example		../examples/SimpleExample.cpp

project		Master-Task-Blackboard MPI Framework
author		Thomas E. Hilinski <https://github.com/tehilinski>
copyright	Copyright 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#include "Master.h"
#include "ErrorHandling.h"
#include "versionMTBMPI.h"
#include "UtilitiesMPI.h"
#include <stdexcept>
#include <sstream>

// define the following to write diagnostics to std::cout
// #define DBG_MPI_MASTER

#ifdef DBG_MPI_MASTER
#include <iostream>
using std::cout;
using std::endl;
#endif

namespace mtbmpi {


MPI::Intracomm comm;					///< global communicator for tasks

MpiCollectiveCB_NoOp const null_MpiCollectiveCB;	///< A no-op MpiCollectiveCB object


Master::Master (
    int const argc,
    char const * const * const argv,
    IDNum const useMinNumProc,
    std::ostream & msgStream,
    TaskFactoryPtr useTaskFactory,
    OutputMgrPtr useOutputMgr,
    MpiCollectiveCBPtr mpiCollectiveCBPtr,
    std::string const logFileName)
    : SendsMsgsToLog   ( -1, ID_Blackboard ),
      numProc          ( 0 ),
      minNumProc       ( std::max( GetFirstTaskID() + 1, useMinNumProc ) ),
      pTaskFactory     ( useTaskFactory ),
      pMpiCollectiveCB ( mpiCollectiveCBPtr ),
      argPair          ( std::make_pair( argc, argv ) ),
      os               ( msgStream )
{
    #ifdef DBG_MPI_MASTER
      std::string const myName = "Master::Master: ";
    #endif

    if ( !MPI::Is_initialized() )
    {
	int argcCopy = argc;
	char** argvCopy = (char**)argv;
	MPI::Init ( argcCopy, argvCopy );
    }

    // create a communicator for C5asks
    #ifdef DBG_MPI_MASTER
      int const myRank = MPI::COMM_WORLD.Get_rank();
      cout << myName << "task ID " << myRank << ": MPI initialized" << endl;
      cout << myName << "task ID " << myRank << ": MPI::COMM_WORLD.Dup start" << endl;
      cout.flush();
    #endif
    mtbmpi::comm = MPI::COMM_WORLD.Dup();
    // hopefully will throw exception
    if ( mtbmpi::comm == MPI::COMM_NULL )
    {
	std::ostringstream oss;
	oss << "task ID " << GetID() << ": Error: MPI::COMM_WORLD.Dup()";
    	throw std::runtime_error( oss.str() );
    }
    #ifdef DBG_MPI_MASTER
      cout << myName << "task ID " << myRank << ": MPI::COMM_WORLD.Dup done" << endl;
      cout.flush();
    #endif

    std::string const msgErrorHandler = SetErrorHandler( mtbmpi::comm );
    if ( !msgErrorHandler.empty() )
	os << versionMTBMPI.ProductNameShort() << ": " << msgErrorHandler << std::endl;
    mtbmpi::comm.Set_name( versionMTBMPI.ProductNameShort().c_str() );
    SetIDs( mtbmpi::comm.Get_rank(), ID_Blackboard );
    numProc = mtbmpi::comm.Get_size();
    if ( numProc < GetMinimumNumberOfProcesses() )
    {
	if ( GetID() == ID_Master )
	{
	    os  << "\n    You requested " << numProc << " processes."
		<< "\n    The minimum number of MPI processes is "
		<< GetMinimumNumberOfProcesses()
		<< "\n"
		<< std::endl;;
	}
    	return;
    }

    // MPI init is done
    if ( GetID() > ID_Blackboard && pMpiCollectiveCB.get() )
    {
	pMpiCollectiveCB->SetID( GetID() );
	pMpiCollectiveCB->Initialize();
    }

    CreateProcesses( logFileName, useOutputMgr );
}

Master::~Master ()
{
    #ifdef DBG_MPI_MASTER
      std::string const myName = "Master::~Master: ";
      cout << myName << "MPI rank " << MPI::COMM_WORLD.Get_rank() << ": start" << endl;
      cout << myName << "task ID " << GetID() << ": start" << endl;
    #endif

    if ( MPI::Is_initialized() )
    {
	if ( GetID() == GetControllerID() )
	    WaitUntilStopped ();
	// all tasks
	if ( GetID() > ID_Blackboard && pMpiCollectiveCB.get() )
	    pMpiCollectiveCB->Finalize();  // derived classes already destroyed
	MPI::Finalize();
    }

    #ifdef DBG_MPI_MASTER
      cout << myName << "task ID " << GetID() << ": done" << endl;
    #endif
}

bool Master::IsInitialized () const
{
    if ( GetID() == ID_Master )
	return pController.get() != nullptr;
    else if ( GetID() == ID_Blackboard )
	return pBlackboard.get() != nullptr;
    else
	return pTask.get() != nullptr;
}

/// @cond SKIP_PRIVATE

void Master::CreateProcesses (
    std::string const & logFileName,
    OutputMgrPtr useOutputMgr)
{
    #ifdef DBG_MPI_MASTER
      std::string const myName = "Master::CreateProcesses: ";
      cout << myName << "start" << endl;
    #endif

    if ( GetID() == ID_Master )
    {
	#ifdef DBG_MPI_MASTER
	  cout << myName << "Creating Controller process" << endl;
	#endif

	// Log().Message("Creating Controller process");
	pConfig = std::make_shared<mtbmpi::Configuration>(
			GetArgs().first, GetArgs().second );
	pController = std::make_shared<mtbmpi::Controller>(
			*this, GetID(), GetBlackboardID(),
			numProc - GetFirstTaskID(), GetFirstTaskID(),
			pConfig );
	// Assume blackboard is (or shortly will be) available
	pController->SetBlackboardState ( State_Running );

	#ifdef DBG_MPI_MASTER
	  cout << myName << "Activating Controller process" << endl;
	#endif

	// following is done in the derived Master class
	// so that derived class can do things after this constructor
	// pController->Activate();
    }
    else if ( GetID() == ID_Blackboard )
    {
	#ifdef DBG_MPI_MASTER
	  cout << myName << "Creating Blackboard process" << endl;
	#endif

	// Log().Message("Creating Blackboard process");
	pBlackboard = std::make_shared<mtbmpi::Blackboard>(
			GetID(), GetControllerID(), useOutputMgr, logFileName );
	pBlackboard->Activate();

	#ifdef DBG_MPI_MASTER
	  cout << myName << "Master: Done creating Blackboard process" << endl;
	#endif
    }
    else // Task
    {
	#ifdef DBG_MPI_MASTER
	  cout << myName << "Creating task process " << GetID() << endl;
	#endif

	MakeTask( GetID() );
    }

    #ifdef DBG_MPI_MASTER
      cout << myName << "done" << endl;
    #endif
}

void Master::MakeTask (
    IDNum const id)		// mpi task rank
{
    // create and run task until stopped or completed
    std::string taskName = "Task "; taskName += ToString(id);
    pTask = std::make_shared<mtbmpi::Task>(
		   *this, taskName, id, GetControllerID(), GetBlackboardID(), pTaskFactory, GetArgs() );
    pTask->Activate();
    pTask.reset();
}

/// @endcond

void Master::StopAllTasks ()
{
    if ( &GetController() != 0 )
    {
	#ifdef DBG_MPI_MASTER
	{
	  std::string msg = "Master::StopAllTasks: sending msg to controller";
	  cout << msg << endl;
	  Log().Message(msg);
	}
	#endif
	pController->StopAllTasks ();
    }
}

void Master::WaitUntilStopped ()
{
    if ( GetID() == ID_Master && pController )
    {
	#ifdef DBG_MPI_MASTER
	{
	  std::string msg = "Master::WaitUntilStopped: sending msg to controller";
	  cout << msg << endl;
	  Log().Message(msg);
	}
	#endif
	GetController().WaitUntilCanStop();
    }
}


} // namespace mtbmpi
